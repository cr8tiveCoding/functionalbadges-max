#!/usr/bin/env bash
set -e

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root."
  exit 1
fi

mkdir -p build/
cd build
cmake ../
make all