#!/bin/bash

set -e
set -x


# Create env file for docker-compose:
printf "UID=%s\nGID=%s\n" "$(id -u)" "$(id -g)" > .env

# allow local container processes to talk to your X server
xhost +SI:localuser:$USER

# If no changes were done to the docker image, docker compose should do nothing.
docker compose up -d --build

# Get inside the container.
docker exec -it --user chrom-d -e HOME=/home/chrom-d -w /chromium/src chrom-b bash -l


# To build, run inside container:
# chromium-build.sh

