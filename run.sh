#!/bin/bash

set -e
set -x

# # Update third party:
# cd .. && gclient sync -D

# # Update tools, sysroots, etc.
# cd .. && gclient runhooks

# # Generate project:
# gn gen out/Gentoo --args='is_debug=true is_component_build=true'

# Build:
time autoninja -j16 -C out/Gentoo/ chrome
# time autoninja -C out/Gentoo/ chrome

# Run:
# ./out/Gentoo/chrome
