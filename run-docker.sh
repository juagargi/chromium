#!/bin/bash

set -e
set -x

# If no changes were done to the docker image, docker compose should do nothing.
docker compose up -d --build

# Get inside the container.
docker exec -it chrom-b bash

# To build, run inside container:
# chromium-build.sh

