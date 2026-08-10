"""Generate the port's asset catalog (build/assets/files.tsv + handles.tsv) from
an already-extracted ROM dump, without needing the .nds.

tools/asset_catalog.py is the canonical generator, but it parses a user-supplied
.nds with ndspy. When only the dsd/dtool extraction is on disk (extracted/), this
builds the same two TSVs from:
  - extracted/dsd/files/files.txt   -> file_id / size / path   (the NitroFS map)
  - extracted/overlays/overlay_0000.bin (decompressed overlay 0, RAM-imaged)
    at base 0x020AA420 -> the game's handle -> path pointer table
    (OV000_HANDLE_TABLE_ADDRESS / COUNT, reused from asset_catalog.py).

Note: extracted/dsd/arm9_overlays/ov000.bin is NOT a clean RAM image (it did not
validate at any single base); the extracted/overlays/overlay_0000.bin dump does,
which is why this uses it and base 0x020AA420 (verified: all 2058 handle pointers
land on ASCII path strings).

  python port/tools/gen_catalog_from_dump.py [repo-root]
"""
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
REPO = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else HERE.parent.parent
sys.path.insert(0, str(REPO / "tools"))
import asset_catalog as ac  # noqa: E402

OV0_FILE = REPO / "extracted" / "overlays" / "overlay_0000.bin"
OV0_RAM = 0x020AA420
FILES_TXT = REPO / "extracted" / "dsd" / "files" / "files.txt"
OUT = REPO / "build" / "assets"


class _Overlay:
    def __init__(self, data, ram):
        self.data = data
        self.ramAddress = ram


def main():
    if not FILES_TXT.is_file():
        sys.exit(f"missing {FILES_TXT} -- extract the ROM first")
    if not OV0_FILE.is_file():
        sys.exit(f"missing {OV0_FILE} -- extract the ROM overlays first")

    assets = []
    for line in FILES_TXT.read_text().splitlines():
        parts = line.split(None, 2)
        if len(parts) < 3:
            continue
        assets.append(ac.Asset(file_id=int(parts[0]), size=int(parts[1]),
                               path=parts[2].strip()))

    ov = _Overlay(OV0_FILE.read_bytes(), OV0_RAM)
    handles = ac.handles_from_overlay(ov, assets)

    OUT.mkdir(parents=True, exist_ok=True)
    ac.write_manifest(OUT / "files.tsv", assets)
    ac.write_handles(OUT / "handles.tsv", handles)
    print(f"files.tsv: {len(assets)} entries")
    print(f"handles.tsv: {len(handles)} handles")


if __name__ == "__main__":
    main()
