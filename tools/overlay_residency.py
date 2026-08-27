"""Which overlays can be resident at the same time.

`dsd` resolves a relocation to a target address and an owning module. Where several
overlays cover the address and dsd cannot tell them apart it says so --
`module:overlays(0,4)` -- and `resolve_placeholders.py` refuses rather than guess,
because a wrong pick still byte-matches (`match.compare` wildcards every relocated
word) and both candidates share the address, so even the ROM link cannot tell.

That shortlist is only unanswerable if you ask the symbol table. Ask the *loader*
instead. Overlays are not free to coexist: the game keeps a table of resident ones
and panics if a new one would overlap. So a candidate that could not have been in
memory while the referring code ran is not a candidate at all, and the shortlist
usually collapses to one.

Every rule below is a fact about this ROM, read out of the game's own code and data
or out of dsd's own unambiguous verdicts. None is an inference from a symbol name.

    E2  overlapping ranges are mutually exclusive
    E1  ov000 is a boot overlay: loaded, entered, unloaded, before anything else
    E3  loading ov006 always loads ov004
    E5  an unambiguous CALL relocation proves its target was resident
    E6  the level -> overlay tables say which overlays a level loads

`possible()` is deliberately one-sided: it can rule a candidate *out*, never in.
When the data is missing (a cold clone has no `extracted/`) it rules nothing out,
so the caller falls back to refusing -- which is the safe direction here.

Validated by falsification: of the 2322 cross-module relocations dsd resolved
unambiguously to a *call*, this model forbids none. Of the 926 unambiguous
cross-module *loads* it forbids 10 -- pointer-shaped data words, no control flow.
Re-run that check with:

    python tools/overlay_residency.py --check
"""
import collections
import pathlib
import re
import struct

REPO = pathlib.Path(__file__).resolve().parent.parent
EXTRACTED = REPO / "extracted" / "dsd"


# --------------------------------------------------------------------------- E2
def _load_ranges():
    """{ov000: (start, end)} covering code *and* bss.

    bss is part of it: `LoadOverlay` adds both before testing for overlap, so two
    overlays whose code fits but whose bss collides are still exclusive."""
    p = EXTRACTED / "arm9_overlays" / "overlays.yaml"
    if not p.is_file():
        return {}
    out, cur = {}, None
    for line in p.read_text(encoding="utf-8", errors="ignore").splitlines():
        m = re.match(r"\s*-\s*id:\s*(\d+)", line)
        if m:
            cur = {"id": int(m.group(1))}
            out[cur["id"]] = cur
            continue
        m = re.match(r"\s+(\w+):\s*(.+)", line)
        if m and cur is not None:
            try:
                cur[m.group(1)] = int(m.group(2).strip())
            except ValueError:
                pass
    return {f"ov{o['id']:03d}": (o["base_address"],
                                 o["base_address"] + o["code_size"] + o.get("bss_size", 0))
            for o in out.values() if "base_address" in o}


RANGE = _load_ranges()


def conflict(a, b):
    """Can `a` and `b` be resident together?  No, if their ranges intersect.

    `LoadOverlay` (src/runtime/filesystem/LoadOverlay.c) walks the 12-entry resident table
    `data_0209d3c4` and calls `Crash()` when the incoming overlay's
    [start, start + code + bss) intersects one already there. The game enforces
    this itself; it is not an assumption about the linker.

    `arm9`, `itcm` and `dtcm` are always resident and conflict with nothing."""
    if a == b or a not in RANGE or b not in RANGE:
        return False
    (s1, e1), (s2, e2) = RANGE[a], RANGE[b]
    return s1 < e2 and s2 < e1


# --------------------------------------------------------------------------- E1
# ov000 is loaded, entered at its own base address, and unloaded again inside one
# function -- src/unnamed/arm9/0201/func_0201a2f8.c:41-43 --
#
#     LoadOverlay((int)&overlay_0);
#     func_020aa420();                 /* == ov000's base address */
#     UnloadOverlay((int)&overlay_0);
#     ...
#     LoadOverlay((int)&overlay_1);    /* line 49, and never unloaded */
#
# Those are the ROM's only LoadOverlay(0)/UnloadOverlay(0) sites: all 17
# Load/UnloadOverlay call sites in the ROM are decompiled in src/ and only this one
# names overlay 0. So ov000 is never resident alongside another overlay, and the
# only arm9 code that can see it is func_0201a2f8 itself.
OV000_MODULE = "arm9"
OV000_WINDOW = "func_0201a2f8"

# ov001 is loaded on line 49 above and there is no UnloadOverlay(1) anywhere in the
# ROM, so it is resident from boot onwards. Nothing needs excluding for it -- it
# overlaps only ov000, which E1 has already dealt with.
ALWAYS_RESIDENT = {"ov001"}


# --------------------------------------------------------------------------- E5
_RELOC = re.compile(r"^from:0x[0-9a-fA-F]+\s+kind:(\S+)\s+to:0x[0-9a-fA-F]+\s+"
                    r"module:overlay\((\d+)\)$")


def _proven_calls():
    """{module: overlays it demonstrably calls}.

    A relocation dsd resolved to a single overlay *and* classified as a call is a
    BL that transfers control there. The target has to have been in memory, so the
    two modules were resident together. Loads do not count: a pointer-shaped word
    can name an address nothing ever dereferences."""
    out = collections.defaultdict(set)
    for p in (REPO / "config").rglob("relocs.txt"):
        src = p.parent.name
        for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = _RELOC.match(ln.strip())
            if m and "call" in m.group(1):
                t = f"ov{int(m.group(2)):03d}"
                if t != src:
                    out[src].add(t)
    return out


CALLS = _proven_calls()

# A module whose own proven set contradicts itself is loaded in more than one
# configuration, so the set is a union over configurations and excludes nothing.
# `arm9` is the obvious case: it is always resident and calls both ov002 and ov004,
# which are mutually exclusive.
_SINGLE_CONFIG = {s: not any(conflict(a, b) for a in ts for b in ts if a < b)
                  for s, ts in CALLS.items()}


# --------------------------------------------------------------------------- E6
def _level_tables():
    """The three tables `LoadLevelOverlays` and `LoadOrUnloadObjectOverlays` read.

    src/game/stages/_Z17LoadLevelOverlaysi.cpp        ov = data_020758c8[level]
    src/runtime/filesystem/_Z26LoadOrUnloadObjectOverlaysPFviEi.cpp
                                          fn(data_02075804[i].t[data_02075998[idx][i]])

    data_02075804 is 7 groups of 7 overlay ids and each group is exactly one tier of
    the overlay layout, so at most one member of a group is ever loaded. That is the
    same exclusion E2 gives, stated by the game."""
    # The DECOMPRESSED arm9, not `extracted/dsd/arm9/arm9.bin`. The ROM's arm9 is
    # BLZ-compressed and dsd keeps it that way (its own arm9.yaml says
    # `compressed: true`), so reading it at base 0x02004000 lands in packed bytes:
    # the three tables come back as noise and `group[i][idx[L][i]]` walks off the
    # end of a 7-entry row. tools/unpack.py writes the unpacked image beside it.
    p = REPO / "extracted" / "arm9_dec.bin"
    if not p.is_file():
        return None
    b, base = p.read_bytes(), 0x02004000

    def w(a):
        return struct.unpack_from("<I", b, a - base)[0]

    group = [[w(0x02075804 + g * 28 + i * 4) for i in range(7)] for g in range(7)]
    level_ov = [w(0x020758c8 + i * 4) for i in range(52)]
    idx = [[b[0x02075998 + l * 7 + i - base] for i in range(7)] for l in range(52)]
    resident = []
    for L in range(52):
        s = {"ov002", f"ov{level_ov[L]:03d}"}
        for i in range(7):
            if idx[L][i]:
                s.add(f"ov{group[i][idx[L][i]]:03d}")
        # LoadOrUnloadObjectOverlays' hard-coded tail.
        s |= {"ov060"} if L in (0x24, 0x26, 0x28) else {"ov098", "ov102"}
        resident.append(s)
    system = {f"ov{v:03d}" for g in group for v in g if v}
    system |= {f"ov{v:03d}" for v in level_ov} | {"ov060", "ov098", "ov102"}
    reach = {m: set().union(*[r for r in resident if m in r] or [set()]) for m in system}
    return system, reach


_LEVELS = _level_tables()
LEVEL_SYSTEM, _REACH = _LEVELS if _LEVELS else (set(), {})


# ---------------------------------------------------------------------------
def possible(cand, module, func=None):
    """Could a reference from `module` (function `func`) mean `cand`?

    False only when something in the ROM says it could not. Never True as a
    positive claim -- the caller must still find that exactly one candidate
    survives before it has an answer."""
    if cand == "ov000" and not (module == OV000_MODULE and func == OV000_WINDOW):
        # E1. Every other referrer runs after ov000 was unloaded.
        return False
    if module == "ov000" and cand in RANGE and cand != "ov000":
        return False                                            # E1, the other way
    if conflict(cand, module):
        return False                                            # E2
    if module == "ov006" and conflict(cand, "ov004"):
        return False                                            # E3
    if _SINGLE_CONFIG.get(module, False) and any(conflict(cand, t) for t in CALLS[module]):
        return False                                            # E5
    if (module in LEVEL_SYSTEM and cand in LEVEL_SYSTEM
            and cand not in _REACH[module]):
        return False                                            # E6
    return True


def settle(candidates, module, func=None):
    """The one candidate residency leaves, or None."""
    left = [c for c in candidates if possible(c, module, func)]
    return left[0] if len(left) == 1 else None


def _check():
    """Falsify the model against every unambiguous verdict dsd produced."""
    bad, tot = collections.Counter(), collections.Counter()
    for p in (REPO / "config").rglob("relocs.txt"):
        src = p.parent.name
        for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = _RELOC.match(ln.strip())
            if not m:
                continue
            t = f"ov{int(m.group(2)):03d}"
            if t == src:
                continue
            kind = "call" if "call" in m.group(1) else "load"
            tot[kind] += 1
            # func is unknown here, so the ov000 window cannot apply; skip ov000.
            if t != "ov000" and not possible(t, src):
                bad[(src, t, kind)] += 1
    print(f"cross-module unambiguous verdicts: {dict(tot)}")
    print(f"forbidden by this model: {sum(bad.values())}")
    for k, v in bad.most_common():
        print(f"   {k}: {v}")
    calls = sum(v for k, v in bad.items() if k[2] == "call")
    print(f"\ncalls contradicted: {calls}  <- must be 0")
    return 0 if calls == 0 else 1


if __name__ == "__main__":
    import argparse
    import sys
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true",
                    help="falsify the model against dsd's unambiguous verdicts")
    a = ap.parse_args()
    sys.exit(_check() if a.check else print(__doc__) or 0)
