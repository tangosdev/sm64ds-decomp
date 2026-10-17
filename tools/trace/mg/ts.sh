#!/bin/sh
# ts.sh X Y [wait] [name]: tap the bottom screen at DS (X, Y), wait, then save
# a window capture to build/melonds/shots/<name>.png.
here=$(cd "$(dirname "$0")" && pwd)
shots="$here/../../../build/melonds/shots"
mkdir -p "$shots"
powershell -NoProfile -File "$here/input.ps1" -Action tap -X "$1" -Y "$2" -Ms 100
sleep "${3:-1.5}"
powershell -NoProfile -File "$here/shot.ps1" -Out "$shots/${4:-t}.png"
