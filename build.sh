#!/bin/sh

CFLAGS="-std=c99 -Wall -Wextra -g"
LDLIBS="-lGL -lm -lX11 -lXi"

SRC_FILES="src/linux_main.c src/racera_game.c src/opengl.c"

OUTPUT_FILE=racera

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
