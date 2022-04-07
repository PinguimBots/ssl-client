#!/usr/bin/env sh
# Echo all programs available.

case "$PATH" in
  (*[!:]:) PATH="$PATH:" ;;
esac

set -f; IFS=:
for dir in $PATH; do
  set +f
  [ -z "$dir" ] && dir="."
  for file in "$dir"/*; do
    if [ -x "$file" ] && ! [ -d "$file" ]; then
      echo $file
    fi
  done
done
