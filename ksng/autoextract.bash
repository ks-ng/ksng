#!/bin/bash
for file in *.zip; do
    [ -f "$file" ] || break
    unzip -o "$file" -d "${file%.zip}"
    rm "$file"
done