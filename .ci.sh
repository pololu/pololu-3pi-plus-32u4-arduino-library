#!/usr/bin/env bash
set -ueo pipefail

BOARDS=(arduino:avr:leonardo)
PLATFORMS=(arduino:avr)

arduino-cli core install "${PLATFORMS[@]}"

export ARDUINO_DIRECTORIES_USER="$PWD/out"
LIB="$ARDUINO_DIRECTORIES_USER/libraries"
rm -rf "$LIB"
mkdir -p "$LIB"
ln -s "$PWD" "$LIB/OurLibrary"
arduino-cli lib update-index
mapfile -t DEPS < <((grep 'depends=' library.properties || echo "") | cut -c9- | tr ',' '\n' | sed -r "/^$/d")
if [ ${#DEPS[@]} -ne 0 ]; then
  arduino-cli lib install "${DEPS[@]}"
fi

for e in examples/*; do
  for b in ${BOARDS[*]}; do
    echo "Compiling $e for $b"
    arduino-cli compile --warnings all --fqbn "$b" "$e" --output-dir "./out/$e"
  done
done
