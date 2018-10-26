#!/usr/bin/env bash
set -e

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root."
  exit 1
fi

mkdir -p lib/
cd lib/

if [ "$1" != "-nopull" ]; then
  if ! [ -x liblogicalaccess/ ]; then
    git clone https://github.com/islog/liblogicalaccess.git
    cd liblogicalaccess
  else
    cd liblogicalaccess
    git fetch origin
    git reset --hard origin/master
  fi
else
  cd liblogicalaccess
fi

mkdir -p install/
cd install/
cmake ../
make install
cd /usr/local/include/logicalaccess/
grep --include=*.hpp -rnl './' -e "def UNIX" | xargs -d \n -i@ sed -i 's/def UNIX/def linux/g' @
grep --include=*.hpp -rnl './' -e "defined(UNIX)" | xargs -d \n -i@ sed -i 's/defined(UNIX)/defined(linux)/g' @
