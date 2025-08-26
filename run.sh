#!/bin/bash

set -e
set -x

# # Update third party:
# cd .. && gclient sync -D

# # Update tools, sysroots, etc.
# cd .. && gclient runhooks

# Clean: ( DO NOT CLEAN!! )
#gn clean out/Gentoo

# Generate project:
# gn gen out/Gentoo --args='is_debug=true is_component_build=true'
gn gen out/Gentoo --args='is_debug=true is_component_build=true use_lld=true'

# Build:
# time autoninja -j16 -C out/Gentoo/ chrome
time autoninja -C out/Gentoo/ chrome

# Run:
# clear && ./out/Gentoo/chrome --user-data-dir=/tmp/chrome-dev --enable-logging=stderr --v=1
clear && ./out/Gentoo/chrome --user-data-dir=/tmp/chrome-dev
