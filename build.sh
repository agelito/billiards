#!/bin/sh

CFLAGS="-Wall -Wextra -g"
LDLIBS="-lGL -lm -lX11"

SRC_FILES="src/main.c src/window_x11.c src/gl_extensions.c src/platform_posix.c src/keyboard_x11.c src/mesh.c src/shader.c"

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
