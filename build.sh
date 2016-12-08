#!/bin/sh

CFLAGS="-Wall -Wextra -g"
LDLIBS="-lGL -lX11"

SRC_FILES="src/main.c src/window.c"

OUTPUT_FILE=aren

cd $(dirname "$0")

( flock -w 5 999 || exit 1
  case "$1" in
      "run") ./bin/$OUTPUT_FILE;;
      *)
	  gcc $CFLAGS $LDLIBS $SRC_FILES -o bin/$OUTPUT_FILE
	  rsync -aruthz assets/ bin/
	  ;;
  esac
) 999>/tmp/$OUTPUT_FILE-compile-lock
