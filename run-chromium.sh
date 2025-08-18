#!/bin/bash

set -e

# # This script is intended to run Chromium built in the docker container.

# # ./out/Default/chrome \
# #   --user-data-dir=/tmp/chrome-dev \
# #   --enable-logging=stderr --v=1 \
# #   --single-process \
# #   --disable-gpu


# ./out/Default/chrome \
# 	--ozone-platform=wayland \
# 	--use-gl=swiftshader \
# 	--disable-gpu


# ./chrome --disable-gpu --no-sandbox --disable-seccomp-filter-sandbox --disable-setuid-sandbox --user-data-dir=/tmp/chrome-dev --ozone-platform=x11

# ./chrome --disable-gpu  --user-data-dir=/tmp/chrome-dev --ozone-platform=x11


# Debug Chromium with a remote debugger, allow attaching to remote debugger:
cd out/Default && gdbserver 127.0.0.1:12345 ./chrome \
	--user-data-dir=/tmp/chrome-dev \
	--disable-gpu \
	--no-sandbox
