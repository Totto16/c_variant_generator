#!/usr/bin/env bash

set -eu

if [ "$#" -ne 3 ]; then
    echo "ERROR: not enough arguments given" >&2
    exit 1
fi

exit 33
