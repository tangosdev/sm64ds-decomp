#!/usr/bin/env python3
"""Which ROM images a derived RTTI file was built from, and whether they still hold.

build/rtti_vtables.json (and the build/tu_map.json made from it) is derived data. It
is read out of extracted/dsd/arm9_overlays/ovNNN.bin, a dsd extraction, while every
byte gate compares against the ROM's own images: extracted/arm9_dec.bin and
extracted/overlays/overlay_NNNN.bin (tools/modules.py, tools/match.py, build_pin.py).
On a healthy extraction the two are the same bytes. On a stale one they are not, and
nothing said so: the shared extraction on one box held 89 of 103 overlays that
differed from the ROM images, and a vtable file built from it put 364 of 426 tables
wrong (daBar_c: 21 slots from 0x02010ecc where the ROM's table holds 31 from
0x020b067c) while every later tool read it as fact.

This module is the one place that answers the question.

    extraction_mismatches()   which overlays the extraction reads differently from
                              the ROM images
    require_matching_extraction()
                              the builder's gate: refuse to derive from those
    fingerprint()             one sha256 over the ROM images, recorded inside the
                              derived file under "_rom_fingerprint"
    guard()                   the reader's gate: refuse when the recorded fingerprint
                              is not the ROM on this box, warn loudly when the file
                              predates the field

A mismatch is refused, not warned, because the data is known wrong. A file with no
fingerprint is warned about, not refused, because it is only unknown, and every
existing build/ directory on every box is in that state until it is rebuilt once.

ITCM and DTCM are not covered: they have no separate ROM image to differ from (they
come out of the same arm9 extraction), so there is nothing independent to compare.

    python tools/rom_fingerprint.py              # print the fingerprint and any mismatch
"""
import hashlib
import json
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
KEY = "_rom_fingerprint"
SCHEME = 1
SOURCE = "extracted/arm9_dec.bin + extracted/overlays/overlay_NNNN.bin"


def _repo(repo):
    return pathlib.Path(repo) if repo is not None else REPO


def _sha(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def rom_images(repo=None):
    """[(module, path)] for the ROM images the byte gates compare against, ROM order."""
    ex = _repo(repo) / "extracted"
    out = [("arm9", ex / "arm9_dec.bin")]
    for p in sorted((ex / "overlays").glob("overlay_*.bin")):
        stem = p.stem.split("_", 1)[1]
        if stem.isdigit():
            out.append(("ov%03d" % int(stem), p))
    return out


def extraction_path(module, repo=None):
    """The dsd extraction file tools/rtti_vtables.py reads for an overlay."""
    return _repo(repo) / "extracted" / "dsd" / "arm9_overlays" / ("%s.bin" % module)


def fingerprint(repo=None):
    """{"scheme", "source", "modules", "sha256"}: one hash over every ROM image.

    Raises SystemExit when there are no ROM images to fingerprint."""
    images = rom_images(repo)
    if not images or not images[0][1].is_file():
        raise SystemExit("no ROM images under extracted/ (arm9_dec.bin, overlays/): "
                         "run tools/unpack.py")
    h = hashlib.sha256()
    n = 0
    for name, path in images:
        h.update(("%s %d %s\n" % (name, path.stat().st_size, _sha(path))).encode())
        n += 1
    return {"scheme": SCHEME, "source": SOURCE, "modules": n, "sha256": h.hexdigest()}


def extraction_mismatches(repo=None):
    """[(module, rom_bytes, extraction_bytes or None)] for overlays whose dsd
    extraction is missing or differs from the ROM image. Empty means they agree."""
    bad = []
    for name, path in rom_images(repo)[1:]:
        ext = extraction_path(name, repo)
        if not ext.is_file():
            bad.append((name, path.stat().st_size, None))
        elif ext.stat().st_size != path.stat().st_size or _sha(ext) != _sha(path):
            bad.append((name, path.stat().st_size, ext.stat().st_size))
    return bad


def _describe(bad):
    rows = ["    %-6s ROM image %d bytes, extraction %s"
            % (m, r, "MISSING" if e is None else "%d bytes" % e) for m, r, e in bad[:6]]
    if len(bad) > 6:
        rows.append("    ... and %d more" % (len(bad) - 6))
    return "\n".join(rows)


def require_matching_extraction(what, repo=None):
    """The builder's gate: SystemExit unless every overlay extraction is the ROM image."""
    bad = extraction_mismatches(repo)
    if bad:
        raise SystemExit(
            "%s: extracted/dsd/arm9_overlays does not match the ROM images the byte "
            "gates use (%d of %d overlays differ).\n%s\nA stale extraction put 364 of "
            "426 vtables wrong once. Re-extract (python tools/unpack.py, or copy a "
            "fresh extracted/ into this tree) and run this again."
            % (what, len(bad), len(rom_images(repo)) - 1, _describe(bad)))


def split(data):
    """(tables, recorded fingerprint or None) from a parsed derived file.

    Consumers that iterate the file as {class: table} must go through this, so the
    metadata key is never read as a class."""
    recorded = data.get(KEY) if isinstance(data, dict) else None
    tables = {k: v for k, v in data.items() if not k.startswith("_")}
    return tables, recorded


def stamp(tables, repo=None):
    """`tables` with this tree's fingerprint first, ready to write."""
    return {KEY: fingerprint(repo), **tables}


_warned = set()


def guard(recorded, what, rebuild, repo=None):
    """The reader's gate. Returns "match" or "absent"; SystemExit on "mismatch".

    `recorded` is the fingerprint stored in the file, or None. `what` names the file,
    `rebuild` is the command that remakes it."""
    if not isinstance(recorded, dict) or not recorded.get("sha256"):
        if what not in _warned:
            _warned.add(what)
            print("WARNING: %s has no ROM fingerprint (built before it was recorded), so "
                  "it cannot be checked against this tree's ROM images. Rebuild it: %s"
                  % (what, rebuild), file=sys.stderr)
        return "absent"
    current = fingerprint(repo)
    if recorded.get("scheme") != current["scheme"] or recorded["sha256"] != current["sha256"]:
        raise SystemExit(
            "%s was built from different ROM images than this tree has (recorded %s, "
            "now %s). Its vtables describe another extraction. Rebuild it: %s"
            % (what, str(recorded["sha256"])[:12], current["sha256"][:12], rebuild))
    return "match"


def recorded_in(path):
    """The fingerprint stored in a derived JSON file, or None."""
    p = pathlib.Path(path)
    if not p.is_file():
        return None
    try:
        data = json.loads(p.read_text(encoding="utf-8", errors="ignore"))
    except ValueError:
        return None
    return data.get(KEY) if isinstance(data, dict) else None


def main():
    fp = fingerprint()
    print("%s: %d modules, sha256 %s" % (fp["source"], fp["modules"], fp["sha256"]))
    bad = extraction_mismatches()
    if bad:
        print("extraction differs from the ROM images in %d overlay(s):" % len(bad))
        print(_describe(bad))
        return 1
    print("extracted/dsd/arm9_overlays matches the ROM images")
    return 0


if __name__ == "__main__":
    sys.exit(main())
