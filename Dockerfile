# XXX(juagargi) from https://github.com/chromium/chromium/blob/main/docs/linux/build_instructions.md#docker

# Use an official Ubuntu base image with Docker already installed
FROM ubuntu:24.04

# With the same UID and GID as the current user.
ARG UID=1000
ARG GID=1000

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install Mandatory tools (curl git python3) and optional tools (vim sudo)
RUN apt-get update && \
    apt-get install -y curl git lsb-release python3 git file vim sudo && \
    rm -rf /var/lib/apt/lists/*

# Export depot_tools path
ENV PATH="/depot_tools:${PATH}"

# Set the working directory to the existing Chromium source directory.
# This can be either "/chromium/src" or "/chromium".
WORKDIR /chromium/src

# Expose any necessary ports (if needed)
# EXPOSE 8080

# Create a dummy user and group with desired UID and GID to avoid permission issues.
RUN groupadd -g ${GID} chrom-d && \
    useradd -u ${UID} -g ${GID} -m chrom-d

RUN echo "chrom-d ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/99-chrom-d

# ============================= DEPENDENCIES ====================
ENV DEBIAN_FRONTEND=noninteractive
COPY build/install-build-deps.py /tmp/install-build-deps.py
RUN chmod +x /tmp/install-build-deps.py

# Provide a no-op sudo for the script (build runs as root; sudo would fail)
# RUN printf '#!/bin/sh\nexec "$@"\n' >/usr/local/bin/sudo && chmod +x /usr/local/bin/sudo
# Minimal sudo shim that supports VAR=VAL prefixes and a few sudo flags.
RUN cat >/usr/local/bin/sudo <<'SH' && chmod +x /usr/local/bin/sudo
#!/bin/sh
# Usage: sudo [VAR=VAL ...] [flags] command [args...]
# We are root in the container; just handle env assignments and exec.
# swallow common sudo flags
while [ $# -gt 0 ]; do
  case "$1" in
    *=*) export "$1"; shift ;;         # handle VAR=VAL
    -E|-H|-n|-S|-k) shift ;;            # ignore simple flags
    -u) shift; shift ;;                 # skip "-u USER"
    --) shift; break ;;                 # explicit end of options
    *) break ;;
  esac
done
exec "$@"
SH

RUN apt-get update
RUN python3 /tmp/install-build-deps.py \
    --no-prompt --unsupported --lib32 --no-chromeos-fonts
RUN rm -f /usr/local/bin/sudo
# ==============================================================

# Install a simple build script:
RUN install -m755 /dev/stdin /usr/local/bin/chromium-build.sh <<'SH'
# Configure git for safe.directory
git config --global --add safe.directory /depot_tools && \
git config --global --add safe.directory /chromium/src

# Loop through each directory in /chromium/src/third_party and add
# them as safe directories in Git
for dir in /chromium/src/third_party/*; do
    if [ -d "$dir" ]; then
        git config --global --add safe.directory "$dir"
    fi
done
cd /chromium/src/
gn gen out/Default
time autoninja -C out/Default chrome
SH


# Create normal user with name "chrom-d". Optional and you can use root but
# not advised.
USER chrom-d

# Start Chromium Builder "chrom-d" (modify this command as needed)
# CMD ["autoninja -C out/Default chrome"]

CMD ["bash"]