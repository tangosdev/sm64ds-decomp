"""Report every class whose name the cartridge's own RTTI contradicts.

WHY THIS EXISTS. A coined class name is not only a readability debt -- it is a hard
block on TU promotion, and the block does not announce itself as a naming problem.
Promoting a translation unit consolidates the class's RTTI, so the compiler emits
`_ZTI<OurName>` and `_ZTS<OurName>`. The cartridge holds those records under the
*real* name, at a different length, so there is no ROM symbol to license them to and
`tubuild` refuses:

    HOMELESS     _ZTS8PoleLift  STB_LOPROC .data size=0xa
    HOMELESS     _ZTI8PoleLift  STB_LOPROC .data size=0xc
    COLLIDES-GAP _ZTV8PoleLift  STB_GLOBAL .data size=0x84 already at ov045:0x02112dbc

`PoleLift` (#2066) is `daObjKm2_Ami_Bou_c` in the ROM. The destructor work that PR was
about was fine; the promotion could never have landed, and no amount of TU-boundary
evidence would have changed that. That is the whole point of running this first.

The evidence is direct, not inferred. From a `_ZTV` address `V`, the word at `V-4` is
the typeinfo pointer; a `__si_class_type_info` record is `[vptr][name ptr][base ptr]`,
so word 1 leads to the `_ZTS` string, which is length-prefixed and therefore
self-checking -- `18daObjKm2_Ami_Bou_c` has exactly 18 characters after the prefix.

TWO WAYS TO GET THIS WRONG, both of which produced wrong numbers before they were
caught:

1. **Resolve each address in its own overlay first.** Many overlays share a base
   address -- ov045, ov046 and ov047 are all at 0x021111a0 -- so a reader that falls
   back to arm9, or that picks an overlay by address alone, silently reads a different
   image and reports confident nonsense. This walks `config/arm9/**/symbols.txt` and
   takes the overlay from the *path* of the file the symbol came from.

2. **Use the decompressed arm9.** `extracted/dsd/arm9/arm9.bin` is compressed;
   `extracted/arm9_dec.bin` is the one with readable strings at their link addresses.

A class this reports as DISAGREE is not promotable until it is renamed. A class it
reports as unreadable is *unmeasured*, not agreeing -- six of them are, and they are
reported separately for exactly that reason.

    python tools/rtti_name_audit.py                # summary + the disagreements
    python tools/rtti_name_audit.py --json out.json
    python tools/rtti_name_audit.py --class PoleLift    # one class, with the chain
"""

import argparse
import json
import pathlib
import re
import struct
import sys

import yaml

REPO = pathlib.Path(__file__).resolve().parent.parent

VTABLE_SYMBOL = re.compile(r"^(_ZTV\S+)\s+.*?addr:(0x[0-9a-fA-F]+)")
OVERLAY_IN_PATH = re.compile(r"overlays/ov(\d+)/")
LENGTH_PREFIXED = re.compile(r"^(\d+)(.*)$")


def _load_images():
    """Return (arm9_base, arm9_bytes, {overlay id: (base, bytes)})."""
    arm9 = (REPO / "extracted/arm9_dec.bin").read_bytes()
    with open(REPO / "extracted/dsd/arm9/arm9.yaml") as fh:
        arm9_base = yaml.safe_load(fh)["base_address"]

    with open(REPO / "extracted/dsd/arm9_overlays/overlays.yaml") as fh:
        overlays = yaml.safe_load(fh)
    if isinstance(overlays, dict):
        overlays = overlays.get("overlays", overlays)

    images = {}
    for entry in overlays:
        path = REPO / "extracted/dsd/arm9_overlays" / entry["file_name"]
        if path.exists():
            images[int(entry["id"])] = (int(entry["base_address"]), path.read_bytes())
    return arm9_base, arm9, images


def _reader(overlay_id, arm9_base, arm9, images):
    """Resolve an address to bytes, preferring this overlay over arm9.

    The preference is the point: overlays share base addresses, so an
    address-only lookup is ambiguous and an arm9-first lookup is simply wrong.
    """
    candidates = []
    if overlay_id is not None and overlay_id in images:
        candidates.append(images[overlay_id])
    candidates.append((arm9_base, arm9))

    def get(addr, count):
        for base, data in candidates:
            offset = addr - base
            if 0 <= offset and offset + count <= len(data):
                return data[offset:offset + count]
        return None

    return get


def _u32(get, addr):
    raw = get(addr, 4)
    return struct.unpack("<I", raw)[0] if raw else None


def _cstr(get, addr, limit=96):
    raw = get(addr, limit)
    if not raw:
        return None
    end = raw.find(b"\0")
    return raw[:end].decode("ascii", "replace") if end >= 0 else None


def _demangle_length_prefixed(name):
    """`18daObjKm2_Ami_Bou_c` -> `daObjKm2_Ami_Bou_c`, and only if the count agrees."""
    match = LENGTH_PREFIXED.match(name or "")
    if not match:
        return name or ""
    body = match.group(2)
    return body if len(body) == int(match.group(1)) else (name or "")


def audit():
    """Yield one dict per `_ZTV` symbol in config, with the ROM's name beside ours."""
    arm9_base, arm9, images = _load_images()
    rows = []
    for symbols in sorted(REPO.glob("config/arm9/**/symbols.txt")):
        match = OVERLAY_IN_PATH.search(symbols.as_posix())
        overlay_id = int(match.group(1)) if match else None
        get = _reader(overlay_id, arm9_base, arm9, images)

        for line in symbols.read_text(encoding="utf-8", errors="replace").splitlines():
            found = VTABLE_SYMBOL.match(line.strip())
            if not found:
                continue
            vtable, addr = found.group(1), int(found.group(2), 16)

            typeinfo = _u32(get, addr - 4)
            name_ptr = _u32(get, typeinfo + 4) if typeinfo else None
            rom = _cstr(get, name_ptr) if name_ptr else None

            rows.append({
                "module": "ov%03d" % overlay_id if overlay_id is not None else "arm9",
                "symbols": symbols.relative_to(REPO).as_posix(),
                "vtable": vtable,
                "addr": "0x%08x" % addr,
                "typeinfo": "0x%08x" % typeinfo if typeinfo else None,
                "ours": _demangle_length_prefixed(vtable[4:]),
                "rom": _demangle_length_prefixed(rom) if rom else None,
                "rom_raw": rom,
            })
    return rows


def _classify(rows):
    agree = [r for r in rows if r["rom"] is not None and r["rom"] == r["ours"]]
    disagree = [r for r in rows if r["rom"] is not None and r["rom"] != r["ours"]]
    unreadable = [r for r in rows if r["rom"] is None]
    return agree, disagree, unreadable


def _report_one(rows, wanted):
    for row in rows:
        if row["ours"] != wanted and row["vtable"] != wanted:
            continue
        print("%s @ %s  (%s)" % (row["vtable"], row["addr"], row["module"]))
        print("  typeinfo   %s" % row["typeinfo"])
        print("  ROM name   %s" % (row["rom_raw"] or "<unreadable>"))
        print("  our name   %s" % row["ours"])
        print("  verdict    %s" % (
            "unreadable -- UNMEASURED, not an agreement" if row["rom"] is None
            else "AGREES" if row["rom"] == row["ours"]
            else "DISAGREES -- not promotable until renamed"))
        return 0
    print("no _ZTV symbol in config for %r" % wanted, file=sys.stderr)
    return 1


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--json", metavar="PATH", help="write every row as JSON")
    parser.add_argument("--class", dest="cls", metavar="NAME",
                        help="report one class and its typeinfo chain")
    parser.add_argument("--all", action="store_true",
                        help="list every disagreement, not the first 25")
    args = parser.parse_args(argv)

    rows = audit()
    if args.cls:
        return _report_one(rows, args.cls)

    agree, disagree, unreadable = _classify(rows)
    print("total _ZTV symbols in config: %d" % len(rows))
    print("  RTTI name AGREES with our class name : %d" % len(agree))
    print("  RTTI name DISAGREES                  : %d" % len(disagree))
    print("  typeinfo/name unreadable             : %d" % len(unreadable))
    print()
    print("A DISAGREE row cannot be TU-promoted until it is renamed: consolidation")
    print("emits _ZTI/_ZTS under our name and the ROM has no home for them.")
    print("An unreadable row is UNMEASURED. It is not an agreement.")
    print()

    shown = disagree if args.all else disagree[:25]
    print("--- %s ---" % ("all disagreements" if args.all
                          else "first %d of %d disagreements" % (len(shown), len(disagree))))
    for row in sorted(shown, key=lambda r: (r["module"], r["ours"])):
        print("  %-6s %-34s ROM: %s" % (row["module"], row["ours"], row["rom"]))

    if unreadable:
        print()
        print("--- unreadable (%d) ---" % len(unreadable))
        for row in unreadable:
            print("  %-6s %s" % (row["module"], row["vtable"]))

    if args.json:
        with open(args.json, "w") as fh:
            json.dump(rows, fh, indent=1)
        print()
        print("wrote %s" % args.json)
    return 0


if __name__ == "__main__":
    sys.exit(main())
