#!/bin/sh

CFLAGS="-std=c99 -Wall -Wextra -g"
LDLIBS="-lGL -lm -lX11 -lXi"

SRC_FILES="src/platform/linux/linux_main.c src/game.c src/rendering/opengl.c"

OUTPUT_FILE=billiard

AP_OUTPUT_FILE=asset_pack
AP_LDLIBS="-lm"
AP_SRC_FILES="src/assets/assets_pack.c"

cd $(dirname "$0")

( flock -w 5 999 || exit 1
  case "$1" in
      "run") ./bin/$OUTPUT_FILE;;
      *)
	  gcc $CFLAGS $AP_LDLIBS $AP_SRC_FILES -o bin/$AP_OUTPUT_FILE
	  bin/$AP_OUTPUT_FILE assets bin/assets_archive.bin
	  
	  gcc $CFLAGS $LDLIBS $SRC_FILES -o bin/$OUTPUT_FILE
	  rsync -aruthz assets/ bin/
	  ;;
  esac
) 999>/tmp/$OUTPUT_FILE-compile-lock
