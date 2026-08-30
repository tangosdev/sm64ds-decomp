#!/usr/bin/env python3
"""Compile a manifest TU once and report, per licensed function, bytes and order.

The question this answers is the one blocking the promotion program: a TU that
defines its destructor OUT OF LINE makes mwcc emit ``D2, D0, D1``, while the ROM has
``D1`` then ``D0`` and no ``D2`` at all. Production isolation places an object's
``.text`` sections into the spanning delink in emission order, so the order is not
cosmetic -- it decides whether the range links correctly.

Changing the source form to fix the order (an inline in-class destructor plus a real
instantiation) also changes what else the TU emits, so the two questions have to be
asked together: does every function still reproduce its ROM bytes, AND are they
emitted in ROM order. Running the whole ROM build to find out costs minutes; this
costs one compile.

    python tools/tu_order_check.py ov045/PoleLift ov002/Seaweed
"""

from __future__ import annotations

import argparse
import io
import pathlib
import subprocess
import sys
import tempfile

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

from elftools.elf.elffile import ELFFile  # noqa: E402
from elftools.elf.relocation import RelocationSection  # noqa: E402

import rombuild as RB  # noqa: E402
import tu_manifest as TUM  # noqa: E402
import modules as MOD  # noqa: E402
import relocs as RL  # noqa: E402

_IMAGES = {}


def rom_bytes(module, addr, size):
    key = RL.normalize_module(module)
    if key not in _IMAGES:
        hit = next((m for m in MOD.modules()
                    if RL.normalize_module(m["name"]) == key
                    or (key == "arm9" and m["name"] == "main")), None)
        _IMAGES[key] = (hit["bin"].read_bytes(), hit["base"]) if hit else None
    img = _IMAGES[key]
    if img is None:
        return None
    data, base = img
    return data[addr - base:addr - base + size]


def compile_tu(source, out):
    src = REPO / source
    flags = RB.CFLAGS
    if src.read_text(encoding="utf-8", errors="ignore").startswith("//cpp"):
        flags = flags.replace("-lang c99", "-lang c++")
    proc = subprocess.run(
        [*RB.launcher(), str(RB.MW / RB.VERSION / "mwccarm.exe"), *flags.split(),
         "-i", str(REPO / "include"), "-c", str(src), "-o", str(out)],
        capture_output=True, text=True, encoding="utf-8", errors="replace")
    return proc


def sections_of(raw):
    """[(index, name, bytes, {relocated word offsets})] for every .text section."""
    elf = ELFFile(io.BytesIO(raw))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    reloc_at = {}
    for s in secs:
        if isinstance(s, RelocationSection) and s.header["sh_size"]:
            reloc_at.setdefault(s.header["sh_info"], set()).update(
                r["r_offset"] for r in s.iter_relocations())
    out = []
    for i, s in enumerate(secs):
        if not s.name.startswith(".text") or not s.header["sh_size"]:
            continue
        names = sorted({y.name for y in symtab.iter_symbols()
                        if y.name and not y.name.startswith("$")
                        and y["st_info"]["type"] != "STT_SECTION"
                        and y["st_shndx"] == i})
        out.append((i, names, bytes(s.data()), reloc_at.get(i, set())))
    return out


def compare(target, cand, relocated):
    """Words differing, ignoring relocated words -- match.compare's own rule."""
    if len(target) != len(cand):
        return None
    bad = 0
    for off in range(0, len(target), 4):
        if off in relocated:
            continue
        if target[off:off + 4] != cand[off:off + 4]:
            bad += 1
    return bad


def duplicate_row(symbol, homes):
    """A `deadstrip-duplicate` policy row for a vague-linkage symbol with one home."""
    hits = homes.get(symbol) or []
    if len(hits) != 1:
        return None
    module, addr = hits[0]
    return {
        "symbol": symbol,
        "disposition": "deadstrip-duplicate",
        "canonical_module": module,
        "canonical_address": f"0x{addr:08x}",
        "reason": (f"vague-linkage copy of {symbol}, re-emitted by every TU that needs "
                   f"it; the cartridge keeps one at {module}:0x{addr:08x} and another "
                   f"enrolled source owns it. rombuild proves this object's body "
                   f"against the cartridge before discarding it."),
    }


def write_policy(entry, extra, homes):
    """Add the compiler_only_output rows this TU's unlicensed output needs.

    Only the two shapes with a mechanical justification are written: a homeless
    variant (deadstrip) and a vague-linkage duplicate with exactly one configured home
    (deadstrip-duplicate). Anything else is reported and left for a human, because the
    reason column is the whole point of the schema.
    """
    rows = list(entry.get("compiler_only_output") or [])
    have = {r.get("symbol") for r in rows}
    added, skipped = [], []
    for symbol in sorted(set(extra)):
        if symbol in have:
            continue
        if homes.get(symbol):
            row = duplicate_row(symbol, homes)
            if row is None:
                skipped.append(f"{symbol}: {len(homes[symbol])} configured homes")
                continue
        elif symbol.endswith("D2Ev"):
            row = {"symbol": symbol, "disposition": "deadstrip",
                   "reason": "compiler-generated D2 variant; the retail link discarded "
                             "it, no ROM symbol names it, and nothing here references it"}
        else:
            skipped.append(f"{symbol}: no mechanical justification, write the row by hand")
            continue
        rows.append(row)
        added.append(symbol)
    if added:
        entry["compiler_only_output"] = rows
        path = REPO / "config" / "tu_manifest.d" / f"{entry['id']}.json"
        import json
        body = json.dumps(entry, indent=2, ensure_ascii=False) + chr(10)
        path.write_text(body, encoding="utf-8", newline="")
    return added, skipped


def check(entry, tmp, policy=False, homes=None):
    ident = entry["id"]
    source = entry.get("source")
    obj = tmp / (pathlib.Path(source).stem + ".o")
    proc = compile_tu(source, obj)
    if proc.returncode != 0 or not obj.is_file():
        print(f"{ident}: COMPILE FAILED")
        print("   " + (proc.stderr or proc.stdout or "").strip()[:800].replace("\n", "\n   "))
        return False

    module = entry["module"]
    licensed = [(f["symbol"], int(f["address"], 16), int(f["size"], 16))
                for f in entry["functions"]]
    want = [s for s, _a, _z in licensed]
    secs = sections_of(obj.read_bytes())
    emitted, extra = [], []
    body = {}
    for _i, names, data, rel in secs:
        for n in names:
            (emitted if n in set(want) else extra).append(n)
            body[n] = (data, rel)

    print(f"{ident}:")
    ok = True
    for symbol, addr, size in licensed:
        if symbol not in body:
            print(f"   {'MISSING':9s} {symbol}")
            ok = False
            continue
        data, rel = body[symbol]
        target = rom_bytes(module, addr, size)
        if target is None:
            print(f"   {'NO IMAGE':9s} {symbol}")
            ok = False
            continue
        if len(data) != size:
            print(f"   {'SIZE':9s} {symbol}  rom 0x{size:x}, emitted 0x{len(data):x}")
            ok = False
            continue
        bad = compare(target, data, rel)
        if bad:
            print(f"   {'DIFF':9s} {symbol}  {bad} word(s)")
            ok = False
    if emitted != want:
        print(f"   ORDER     emitted {emitted}")
        print(f"             wanted  {want}")
        ok = False
    if extra:
        print(f"   EXTRA     {sorted(set(extra))}")
        if policy:
            added, skipped = write_policy(entry, extra, homes or {})
            for symbol in added:
                print(f"   POLICY    wrote a row for {symbol}")
            for why in skipped:
                print(f"   POLICY    skipped {why}")
            if skipped:
                ok = False
            extra = [x for x in extra if x not in set(added)]
    if ok and not extra:
        print("   ALL MATCH, ROM order, nothing unlicensed")
    elif ok:
        print("   all match in ROM order; the extras above need a compiler-only policy")
    return ok


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("ids", nargs="+", help="manifest entry id(s)")
    ap.add_argument("--write-policy", action="store_true",
                    help="add the compiler_only_output rows the unlicensed output needs")
    args = ap.parse_args()
    homes = RL.load_symbol_homes() if args.write_policy else {}
    by_id = {e["id"]: e for e in TUM.load()["entries"]}
    tmp = pathlib.Path(tempfile.mkdtemp())
    bad = 0
    for ident in args.ids:
        entry = by_id.get(ident)
        if entry is None:
            print(f"{ident}: no such manifest entry")
            bad += 1
            continue
        if not check(entry, tmp, args.write_policy, homes):
            bad += 1
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
