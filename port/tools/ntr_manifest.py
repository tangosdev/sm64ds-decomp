#!/usr/bin/env python3
"""Write the files.txt manifest ntr::fs_mount_dir expects.

The ntr fs backend mounts an extracted directory through a "<id> <size> <path>"
manifest so FAT ids survive extraction. Our extraction already has the same
facts in build/assets/files.tsv (tools/asset_catalog.py); this just re-emits
them at the mount root, extracted/dsd/files/files.txt.
"""

import csv
import pathlib
import sys

root = pathlib.Path(__file__).resolve().parents[2]
tsv = root / "build/assets/files.tsv"
out = root / "extracted/dsd/files/files.txt"

if not tsv.exists():
    sys.exit(f"missing {tsv} -- run tools/asset_catalog.py generate <rom> first")

lines = []
with open(tsv, newline="") as f:
    for row in csv.DictReader(f, delimiter="\t"):
        lines.append(f'{row["file_id"]} {row["size"]} {row["path"]}')

out.write_text("\n".join(lines) + "\n", encoding="ascii")
print(f"{len(lines)} entries -> {out}")
