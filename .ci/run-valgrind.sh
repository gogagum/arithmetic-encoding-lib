#!/bin/bash

set -e

EXECUTABLE=${1:-executable}

valgrind --tool=memcheck \
         --gen-suppressions=all \
         --leak-check=full \
         --show-leak-kinds=all \
         --leak-resolution=med \
         --track-origins=yes \
         --vgdb=no \
         --error-exitcode=1 \
         build-valgrind/${EXECUTABLE}
    