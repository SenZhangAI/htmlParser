#!/bin/bash

has_installed() {
  echo "Check if [$1] installed..."
  found=$(which $1 2>/dev/null)
  if [ -z "$found" ]; then
    echo "                           ------- No"
    return 1
  else
    echo "                           ------- Yes"
    return 0
  fi
}

if has_installed clang++; then
  compiler=clang++
else
  echo "[clang++] has not installed, check if g++ has installed"

  if has_installed g++; then
    compiler=g++
  else
    echo "[clang++] or [g++] not installed, exit..."
    exit 1
  fi
fi

echo "make clean..."
make clean

echo "make..."
make CC=$compiler

echo "run test..."
./bin/htmlparser.exe ./test/About-OperaSoftware.html

echo "open index.html..."
echo "this may take a moment..."

if has_installed explorer; then
  explorer index.html
elif has_installed gnome-open; then
  gnome-open index.html
elif has_installed xdg-open; then
  xdg-open index.html
fi

