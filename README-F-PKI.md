

# Build

Instructions here:
- https://github.com/chromium/chromium/blob/main/docs/linux/build_instructions.md#get-the-code
- https://github.com/chromium/chromium/blob/main/docs/linux/build_instructions.md#docker
- https://wiki.gentoo.org/wiki/Project:Chromium/How_to_make_a_Chromium_tarball

The script `run-docker.sh` creates a docker image with all necessary
dependencies in it, and build scripts ready inside it.
It then runs a long-lived container to build Chromium inside,
by allowing the user to exec a bash inside and run the build script:
```bash
./run-docker.sh
... OUTPUT OF DOCKER COMPOSE BUILDING THE IMAGE ...

chromium-build.sh
... OUTPUT OF BUILD PROCESS ...
```
