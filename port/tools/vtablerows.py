#!/usr/bin/env python3
"""Read a minigame class's vtable rows out of the RAW overlay images and
price them the way mg_fanout_costs.txt section 3 does: override slots vs
the dScMgBase_c base table, marker-carrying overrides, and no-source
overrides -- so a fan-out lane starts from a checked read instead of a
hand transcription of 36 slots.

WHY THIS EXISTS. The fan-out behind dScMgCurling_c is twenty-nine more
classes, and section 3's per-class table (the ranking the lanes are cut
from) was hand-derived once, for all thirty, by MG1. Its first nosrc
column was WRONG because a name-shaped lookup missed src files not named
after their symbol, and review re-derived it by hand through the delink
records. This tool does the delink-keyed join from the start, and its
acceptance was a RECONSTRUCTION: run over the twenty-nine resolved
vtables, it reproduced section 3's reviewed ovr/mark/nosrc columns row
for row, 29 of 29, and those rows summed to 353/264/4 (the one
divergence in the first reconstruction run was a transcription typo in
the CHECKER's own copy of the hand table, which is the point of the
exercise). This line and the totals print used to quote 353/263/6,
which was section 3's SUMMARY LINE and never the sum of its own rows;
run mg6's merge corrected both ends to the rows.

IT IS THIRTY ROWS NOW, NOT TWENTY-NINE, and the thirtieth is the one
this tool could never have produced on its own. Section 3's row for
0x179 read "NOT FOUND -- see below" because no load relocation inside
MgSnowballSlalom_Spawn points at any signature table: that factory
allocates and DELEGATES, and func_ov006_021295ac is the body that
writes the vtable. Run mg11 lane SNW derived data_ov006_0214000c and
its width 36 BY HAND out of the ROM and filled the row. What this tool
does with it is the useful half and the honest division of labour:
given a vtable and a width it MEASURES ovr/mark/nosrc from the image,
so the hand row 16/14/0 is checked on every build exactly like the
other twenty-nine. The tool still cannot RESOLVE that vtable; it can
and does reconstruct the row. The totals moved to 369/278/4 with the
row, and the twenty-nine that always resolved still sum to 353/264/4.
The unclaimed-table count printed by --reconstruct drops from three to
TWO, and those two are shared bases rather than any id's vtable:
data_ov006_0213e448 (dScMgSingle3DBase_c, 41 load relocations from
fourteen classes) and data_ov006_0213c62c (the bounce/trampoline
family's base, 14 from five).

WHAT IT READS, all of it produced by something that is not this tool:

  extracted/overlays/overlay_000{4,6}.bin   the RAW images. dsd's
                     arm9_overlays exports are REBUILT and twice proven
                     stale (ov085 by 2897 bytes, ov007 by 1248); only
                     extracted/overlays is config-aligned, so that is
                     the only thing this tool will open.
  config .../delinks.txt   the section spans (the overlay BASE is the
                     first section's start -- the T1 rule; yaml
                     base_address is the counterexample) and the
                     address->src-file blocks the join runs on.
  config .../symbols.txt   exact-address symbol names, for the report.
  src/<file>         only to test for the marker string
                     "recovered from vtable slot identity".
  port/mg_fanout_costs.txt   --reconstruct parses section 3's table out
                     of the evidence doc itself, so there is no second
                     copy of the table to drift.

WHAT IT REPORTS per class (--vtable ADDR --width N): every slot 0..N-1,
inherited or OVERRIDE, the override's target address, owning src file
via the delinks join, marker status, and the exact-address symbol name.
--census lists every ActorBase-signature table in ov006's .data (slots
13/14/15 == 0x0204357c/0x0204349c/0x02043494, the spacing no pair run
or file table imitates) plus the base-table uniqueness check in ov004.
--reconstruct re-derives all THIRTY hand rows and exits nonzero on
any divergence, printed as a CONTRADICTION TO INVESTIGATE -- either
side could be the wrong one; the first run's divergence was the
checker's, not the doc's.

WHAT IT DOES NOT DO, stated so nobody reads more into a row than the
code earns:

  * it does NOT adjudicate. A MARKER row still goes to a human with the
    ROM open (inferred_stub_adjudicated.txt is where rulings live);
    this tool counts and names, never rules.
  * it does NOT derive width, but since run link60 lane MGA it REFUSES
    one that cannot fit. 36-vs-37 is still a per-class ruling taken as
    --width input (--reconstruct takes it from the doc's rows), and the
    word at slot 36 is printed either way, labeled candidate, because
    nothing here can tell a 37th slot from the data that follows. What
    is new is a floor: if the vtable address is itself a config symbol,
    a width reaching past the NEXT config symbol is rejected, because
    that slot would be the first word of another object. The span is an
    upper bound (padding inflates it) so the check only ever refuses,
    never promotes 36 to 37. It was added because the doc's width column
    and this tool's --reconstruct were reading each other: twelve of the
    twenty-nine rows said 37 over a 36-word span, and for 0x169 the
    phantom slot lands on a pair word CURLING's constructor copies.
    A SECOND floor sits beside it (spawninfo_tail, run link60 lane MGB)
    for the case the first one names and cannot answer -- "or the config
    is missing a symbol". It reads the ROM instead of the config: if the
    word after slot 36 is a doubled minigame id, slot 36 is the next
    class's SpawnInfo FACTORY word, so the table is 36 and the tool can
    say WHICH class owns the bytes. Fires on 0x16c.
  * it does NOT resolve id -> vtable. The factory load-reloc derivation
    stayed by hand, and MgSnowballSlalom (0x179, no resolving reloc) is
    the standing counterexample. Feed it a vtable address you derived.
  * the census covers ov006's .data and the ActorBase-signature family
    ONLY. A vtable outside that family is invisible to the scan.
  * the join covers .text and .init blocks of arm9, ov004 and ov006. An
    override pointing anywhere else would read as no-owner and inflate
    nosrc; among the 353 reconstructed overrides there were none, and a
    future family that spans other overlays extends DELINKS, not the
    trust boundary.

THE TWO TEST MODES COVER DIFFERENT THINGS AND BOTH ARE WIRED into
build-port.cmd. --selftest exercises the parsing, the slot diff, the
delinks join and every refusal on fabricated fixtures, and cannot see
the REAL-TREE WIRING: the extracted/config paths, the BASE_TABLE
constant, the symbol tables. --reconstruct is the net for exactly that
half (a broken real_setup passes --selftest and fails the 29/29), which
is why the build guard runs both rather than reading selftest green as
coverage of the whole tool. The build therefore requires the extracted
ROM images pre-configure; a tree without them refuses with the
decomp-worktree pointer instead of building binaries that abort at
runtime anyway.

    python port/tools/vtablerows.py --census
    python port/tools/vtablerows.py --vtable 0x0213c304 --width 36
    python port/tools/vtablerows.py --reconstruct
    python port/tools/vtablerows.py --selftest
"""

import argparse
import bisect
import os
import pathlib
import re
import struct
import sys
import tempfile

SIG = (0x0204357c, 0x0204349c, 0x02043494)
SIG_SLOT = 13          # SIG occupies slots 13, 14, 15
BASE_TABLE = 0x020bc0c0  # dScMgBase_c's own table, in ov004 .data
BASE_WIDTH = 36
MARKER = "recovered from vtable slot identity"

SECTION = re.compile(
    r"^\s+\.(\w+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)\s+"
    r"kind:(\w+)")
SRC_BLOCK = re.compile(r"^(src/\S+):\s*$")
SRC_RANGE = re.compile(
    r"\.(?:text|init)\s+start:0x([0-9a-fA-F]+) end:0x([0-9a-fA-F]+)")
SYMBOL = re.compile(r"^(\S+)\s+kind:\S+\s+addr:0x([0-9a-fA-F]+)")
# section 3's row shape. The spawn column can carry a space
# ("MgShuffleShell (curling)"), so the row is anchored on the 8-hex
# vtable column, not on a \S+ name -- the selftest's fixture caught the
# \S+ version dropping exactly the DONE row. The NOT FOUND row (no hex
# column, no count columns) does not match by construction.
DOC_ROW = re.compile(
    r"^\s+0x(1[0-9a-f]{2})\s+.+?([0-9a-f]{8})\s+(\d+)\s+(\d+)\s+(\d+)"
    r"\s+(\d+)(?:\s+DONE)?\s*$")


class Overlay:
    """A raw overlay image, based and span-checked from its delinks file.

    The base is the FIRST section's start and the file must be exactly
    last-in-file-section-end minus base bytes long; anything else is a
    stale or wrong image and the answer is a refusal, not a read.
    """

    def __init__(self, name, image_path, delinks_path):
        self.name = name
        sections = []
        for line in pathlib.Path(delinks_path).read_text(
                encoding="utf-8-sig", errors="replace").splitlines():
            if SRC_BLOCK.match(line):
                break
            m = SECTION.match(line)
            if m:
                sections.append((m.group(1), int(m.group(2), 16),
                                 int(m.group(3), 16), m.group(4)))
        if not sections:
            sys.exit("vtablerows: no section lines in %s" % delinks_path)
        infile = [s for s in sections if s[3] != "bss"]
        self.base = infile[0][1]
        want = infile[-1][2] - self.base
        self.data_span = next(((s[1], s[2]) for s in sections
                               if s[0] == "data"), None)
        self.image = pathlib.Path(image_path).read_bytes()
        if len(self.image) != want:
            sys.exit("vtablerows: %s is %d bytes but %s says %d "
                     "(0x%08x..0x%08x) -- a stale or wrong image, refusing "
                     "to read it" % (image_path, len(self.image),
                                     delinks_path, want, self.base,
                                     infile[-1][2]))

    def word(self, addr):
        off = addr - self.base
        if not 0 <= off <= len(self.image) - 4:
            sys.exit("vtablerows: address 0x%08x is outside %s "
                     "(0x%08x..0x%08x)" % (addr, self.name, self.base,
                                           self.base + len(self.image)))
        return struct.unpack_from("<I", self.image, off)[0]


class Sources:
    """The delinks-keyed address->src join, and the marker test.

    This join exists BECAUSE the name-shaped lookup failed: section 3's
    first nosrc column missed src files not named after their symbol.
    Only .text and .init ranges are indexed; vtable slots point at code.
    """

    def __init__(self, root, delinks_paths):
        self.root = pathlib.Path(root)
        ivs = []
        for dp in delinks_paths:
            cur = None
            for line in pathlib.Path(dp).read_text(
                    encoding="utf-8-sig", errors="replace").splitlines():
                m = SRC_BLOCK.match(line)
                if m:
                    cur = m.group(1)
                    continue
                m = SRC_RANGE.search(line)
                if m and cur:
                    ivs.append((int(m.group(1), 16), int(m.group(2), 16),
                                cur))
        self.ivs = sorted(ivs)
        self.starts = [a for a, _, _ in self.ivs]
        self._marked = {}

    def owner(self, addr):
        i = bisect.bisect_right(self.starts, addr) - 1
        if i >= 0 and self.ivs[i][0] <= addr < self.ivs[i][1]:
            return self.ivs[i][2]
        return None

    def marked(self, src):
        if src not in self._marked:
            p = self.root / src
            self._marked[src] = p.is_file() and \
                MARKER in p.read_text(errors="replace")
        return self._marked[src]


def load_symbols(paths):
    out = {}
    for sp in paths:
        p = pathlib.Path(sp)
        if not p.is_file():
            continue
        for line in p.read_text(encoding="utf-8-sig",
                                errors="replace").splitlines():
            m = SYMBOL.match(line)
            if m:
                out.setdefault(int(m.group(2), 16), m.group(1))
    return out


def find_tables(ov):
    """Every ActorBase-signature table start in the overlay's .data."""
    if ov.data_span is None:
        sys.exit("vtablerows: %s has no .data section to scan" % ov.name)
    lo, hi = ov.data_span
    hits = []
    for a in range(lo, hi - (SIG_SLOT + 3) * 4, 4):
        if (ov.word(a + SIG_SLOT * 4), ov.word(a + (SIG_SLOT + 1) * 4),
                ov.word(a + (SIG_SLOT + 2) * 4)) == SIG:
            hits.append(a)
    return hits


def symbol_span_words(syms, vtable):
    """Words from `vtable` to the next config symbol, or None if `vtable` is
    not itself a symbol. An UPPER BOUND on the table's width, never an exact
    width: unsymbolised padding after a table inflates it. Used only to REFUSE
    a width that cannot fit, which is the direction that is always sound."""
    if not syms or vtable not in syms:
        return None
    later = [a for a in syms if a > vtable]
    if not later:
        return None
    return (min(later) - vtable) // 4


def spawninfo_tail(ov, vtable):
    """The minigame id whose SpawnInfo record starts at vtable+36*4, or None.

    THE CASE symbol_span_words ABOVE CANNOT ANSWER, and its own refusal names
    the hole: "either the width is wrong OR THE CONFIG IS MISSING A SYMBOL".
    The span check needs `vtable` to be a symbol and needs a symbol after it;
    where either is absent it returns None and declines. This reads the ROM
    bytes instead and is independent of config coverage entirely.

    A minigame SpawnInfo is eight bytes -- a factory address, then the actor
    id in BOTH halves of the next word -- and hal/scene_boot.cpp's registry
    cross-checks exactly that doubled id at +4 before installing a row.
    Nothing else in ov006 .data has the shape by accident: the halves must
    agree AND land inside src/IsMinigameActorID.c's 0x169..0x186.

    Run link60 lane MGB. It fires on 0x16c, where the eight bytes a width-37
    reading would call slot 36 are MgBingoBallSlotsShot's spawn record. That
    is the same defect MGA's span check catches on twelve rows, arrived at
    from the other side, and it is kept as its own check for two reasons: it
    survives a config with no symbol there, and it can say WHICH class owns
    the bytes, which a span can never do.
    """
    w1 = ov.word(vtable + 37 * 4)
    lo, hi = w1 & 0xFFFF, (w1 >> 16) & 0xFFFF
    if lo == hi and 0x169 <= lo <= 0x186:
        return lo
    return None


def class_rows(ov, base_slots, vtable, width, syms=None):
    """[(slot, word, override?, owner, marked?)] for one class's table."""
    if width not in (36, 37):
        sys.exit("vtablerows: width %d is outside MG1's ruling set (36 or "
                 "37); derive it by hand first, this tool will not guess"
                 % width)
    if width == 37:
        sid = spawninfo_tail(ov, vtable)
        if sid is not None:
            sys.exit("vtablerows: width 37 does not fit 0x%08x -- slot 36 "
                     "(0x%08x) and the word after it (0x%08x, the doubled "
                     "minigame id 0x%03x) are that id's SPAWNINFO RECORD, so "
                     "the bytes past this table belong to another class and "
                     "a fill would overwrite its factory. This table is 36 "
                     "slots. Read from the ROM rather than the config, so it "
                     "stands even where the symbol span declines."
                     % (vtable, ov.word(vtable + 36 * 4),
                        ov.word(vtable + 37 * 4), sid))
    # THE WIDTH CHECK THAT IS NOT CIRCULAR. Run link60 lane MGA.
    #
    # This tool used to take width purely as input and --reconstruct took it
    # from the doc's own rows, so the gate compared the doc against the ROM
    # USING THE DOC'S NUMBER and could never contradict it. Twelve of the
    # twenty-nine documented widths were 37 where the config's own symbol span
    # is 36 words, and the extra slot is not spare space -- for 0x169 it lands
    # on data_ov006_0213c1e4, the mwcc pair whose code word CURLING's own
    # constructor copies into data_ov006_02141930 slot 2. A 37-slot fill there
    # writes a host thunk over another shipped class's state table, which no
    # byte gate can see because it is port host storage.
    #
    # The span is an UPPER bound (padding inflates it), so it can only ever
    # refuse a width that does not fit -- it never promotes 36 to 37. That is
    # enough: every observed error was in the too-wide direction.
    span = symbol_span_words(syms, vtable)
    if span is not None and width > span:
        sys.exit("vtablerows: width %d does not fit 0x%08x -- the next config "
                 "symbol is %d words on, so slot %d would be the first word "
                 "of the NEXT object, not a slot. Either the width is wrong "
                 "or the config is missing a symbol; derive it from the span "
                 "and from slot 35 (a dScMgBase_c-derived table ends "
                 "0x020ad660 there) before overriding this."
                 % (width, vtable, span, span))
    lo, hi = ov.data_span
    if not (lo <= vtable and vtable + width * 4 <= hi):
        sys.exit("vtablerows: vtable 0x%08x (+%d slots) is not inside %s "
                 ".data 0x%08x..0x%08x" % (vtable, width, ov.name, lo, hi))
    got = tuple(ov.word(vtable + (SIG_SLOT + j) * 4) for j in range(3))
    if got != SIG:
        # The one place this tool used to guess: any in-range address
        # printed a confident table, so a 4-bytes-off address produced 36
        # plausible-looking wrong rows. The signature is the same test the
        # census runs; an address that fails it is not a Scene-family
        # vtable start, whatever else it is.
        sys.exit("vtablerows: 0x%08x does not carry the ActorBase "
                 "signature at slots 13..15 (read %s) -- not a "
                 "Scene-family vtable start, or an address a few bytes "
                 "off one; run --census for the real table starts"
                 % (vtable, " ".join("0x%08x" % g for g in got)))
    rows = []
    for i in range(width):
        w = ov.word(vtable + 4 * i)
        rows.append((i, w, i >= BASE_WIDTH or w != base_slots[i]))
    return rows


def measure(rows, sources):
    ovr = [(i, w) for i, w, is_ovr in rows if is_ovr]
    mark = nosrc = 0
    detail = []
    for i, w in ovr:
        o = sources.owner(w)
        m = bool(o and sources.marked(o))
        if o is None:
            nosrc += 1
        elif m:
            mark += 1
        detail.append((i, w, o, m))
    return len(ovr), mark, nosrc, detail


def real_setup(root):
    root = pathlib.Path(root)
    for need in ("extracted/overlays/overlay_0004.bin",
                 "extracted/overlays/overlay_0006.bin",
                 "config/arm9/overlays/ov004/delinks.txt",
                 "config/arm9/overlays/ov006/delinks.txt"):
        if not (root / need).is_file():
            sys.exit("vtablerows: %s is missing under %s -- this tree has "
                     "no extracted ROM images; copy extracted/ from a live "
                     "worktree (the decomp-worktree setup step) before "
                     "measuring" % (need, root))
    ov4 = Overlay("ov004", root / "extracted/overlays/overlay_0004.bin",
                  root / "config/arm9/overlays/ov004/delinks.txt")
    ov6 = Overlay("ov006", root / "extracted/overlays/overlay_0006.bin",
                  root / "config/arm9/overlays/ov006/delinks.txt")
    sources = Sources(root, [root / "config/arm9/delinks.txt",
                             root / "config/arm9/overlays/ov004/delinks.txt",
                             root / "config/arm9/overlays/ov006/delinks.txt"])
    syms = load_symbols([root / "config/arm9/symbols.txt",
                         root / "config/arm9/overlays/ov004/symbols.txt",
                         root / "config/arm9/overlays/ov006/symbols.txt"])
    base_slots = [ov4.word(BASE_TABLE + 4 * i) for i in range(BASE_WIDTH)]
    return ov4, ov6, sources, syms, base_slots


def cmd_census(root):
    ov4, ov6, sources, syms, base_slots = real_setup(root)
    own = find_tables(ov4)
    if own != [BASE_TABLE]:
        print("vtablerows: NOTE -- ov004's signature census is %s, not "
              "exactly the base table; investigate before trusting rows"
              % ["0x%08x" % a for a in own])
    hits = find_tables(ov6)
    print("ov006 ActorBase-signature tables: %d" % len(hits))
    for a in hits:
        print("    0x%08x" % a)
    print("(the base table 0x%08x is ov004's only signature table: %s)"
          % (BASE_TABLE, "yes" if own == [BASE_TABLE] else "NO"))
    return 0


def cmd_rows(root, vtable, width):
    ov4, ov6, sources, syms, base_slots = real_setup(root)
    rows = class_rows(ov6, base_slots, vtable, width, syms)
    n_ovr, n_mark, n_nosrc, detail = measure(rows, sources)
    det = {i: (o, m) for i, w, o, m in detail}
    print("vtable 0x%08x, width %d (width is MG1's ruling, not this "
          "tool's)" % (vtable, width))
    for i, w, is_ovr in rows:
        if not is_ovr:
            print("  slot %2d  inherited   0x%08x" % (i, w))
            continue
        o, m = det[i]
        print("  slot %2d  OVERRIDE    0x%08x  %-12s %s%s" % (
            i, w, syms.get(w, "(no exact-address symbol)"),
            o or "NO SOURCE (no delinks block owns this address)",
            "  MARKER: needs a ROM adjudication before seating" if m
            else ""))
    if width == 36:
        cand = ov6.word(vtable + 36 * 4)
        print("  slot 36 word (CANDIDATE only, width says it is not a "
              "slot): 0x%08x" % cand)
    print("ovr %d, mark %d, nosrc %d" % (n_ovr, n_mark, n_nosrc))
    return 0


def parse_doc_table(doc_path):
    rows = []
    for line in pathlib.Path(doc_path).read_text(
            errors="replace").splitlines():
        m = DOC_ROW.match(line)
        if m:
            rows.append((int(m.group(1), 16), int(m.group(2), 16),
                         int(m.group(3)), int(m.group(4)),
                         int(m.group(5)), int(m.group(6))))
    return rows


def cmd_reconstruct(root):
    ov4, ov6, sources, syms, base_slots = real_setup(root)
    doc = pathlib.Path(root) / "port" / "mg_fanout_costs.txt"
    rows = parse_doc_table(doc)
    if len(rows) != 30:
        sys.exit("vtablerows: parsed %d rows out of %s section 3, expected "
                 "30 -- the doc's table shape moved, re-pin DOC_ROW before "
                 "trusting a reconstruction. It was 29 until run mg11 lane "
                 "SNW filled 0x179's row by hand; a reader seeing 29 here "
                 "has lost that row rather than found an older doc"
                 % (len(rows), doc))
    hits = set(find_tables(ov6))
    missing = [vt for _, vt, *_ in rows if vt not in hits]
    if missing:
        sys.exit("vtablerows: hand vtables %s are not in the signature "
                 "census -- wrong image or wrong doc, refusing"
                 % ["0x%08x" % v for v in missing])
    print("signature census: %d tables, all 30 hand vtables among them, "
          "%d unclaimed (both shared bases: dScMgSingle3DBase_c at "
          "0x0213e448 and the bounce/trampoline base at 0x0213c62c)"
          % (len(hits), len(hits) - 30))
    diverged = []
    tot = [0, 0, 0]
    for cid, vt, width, h_ovr, h_mark, h_nosrc in rows:
        n_ovr, n_mark, n_nosrc, _ = measure(
            class_rows(ov6, base_slots, vt, width, syms), sources)
        tot[0] += n_ovr
        tot[1] += n_mark
        tot[2] += n_nosrc
        tag = ""
        if (n_ovr, n_mark, n_nosrc) != (h_ovr, h_mark, h_nosrc):
            tag = "  DIVERGES from the doc's %d/%d/%d" % (h_ovr, h_mark,
                                                          h_nosrc)
            diverged.append(cid)
        print("  0x%03x  vt 0x%08x  w%d  ovr %2d  mark %2d  nosrc %d%s"
              % (cid, vt, width, n_ovr, n_mark, n_nosrc, tag))
    print("totals ovr/mark/nosrc: %d/%d/%d (section 3's table: 369/278/4; "
          "the 29 that always resolved are 353/264/4)" % tuple(tot))
    if diverged:
        print("vtablerows: %d row(s) DIVERGE. A divergence is a "
              "CONTRADICTION TO INVESTIGATE, not automatically the doc's "
              "error -- the first reconstruction's divergence was the "
              "checker's own transcription." % len(diverged))
        return 1
    print("reconstruction EXACT, 30/30")
    return 0


# ---------------------------------------------------------------------------

def _wr(p, b):
    """Write the fixture image and hand back the path, for one-line arms."""
    p.write_bytes(bytes(b))
    return p


def selftest():
    """The whole read path against a fabricated overlay and delinks.

    One fake module: .text 0x1000..0x2000 (a marked src at 0x1100, an
    unmarked one at 0x1200, a gap at 0x1300), .data 0x2000..0x3000
    holding a 36-slot base table at 0x2000 and one width-37 class table
    at 0x2100 overriding slots 5 (marked), 6 (unmarked), 7 (the gap)
    plus its own slot 36. Expected: census finds exactly the two
    tables; ovr/mark/nosrc = 4/2/1 (slot 36's target is the marked TU
    again, so mark counts it twice the way section 3's columns do);
    width 35 refuses; a truncated image refuses on the span check.
    """
    ok = True

    def expect(cond, what, got):
        nonlocal ok
        if not cond:
            print("FAIL: %s: %s" % (what, got))
            ok = False

    with tempfile.TemporaryDirectory() as td:
        tdp = pathlib.Path(td)
        (tdp / "src").mkdir()
        # The marker text is a LITERAL here on purpose: a fixture written
        # from the MARKER constant would mutate in lockstep with it and
        # the arm would test nothing (the first mutation battery proved
        # exactly that).
        (tdp / "src" / "f_1100.c").write_text(
            "/* recovered from vtable slot identity */\n")
        (tdp / "src" / "f_1200.c").write_text("/* plain */\n")
        delinks = tdp / "delinks.txt"
        delinks.write_text(
            "    .text       start:0x1000 end:0x2000 kind:code align:32\n"
            "    .data       start:0x2000 end:0x3000 kind:data align:32\n"
            "    .bss        start:0x3000 end:0x3100 kind:bss align:32\n"
            "src/f_1100.c:\n"
            "    .text start:0x1100 end:0x1200\n"
            "src/f_1200.c:\n"
            "    .text start:0x1200 end:0x1300\n")
        img = bytearray(0x2000)
        base_slots = [0x8000 + 4 * i for i in range(36)]
        # Signature words as LITERALS, same reason as the marker above:
        # the arm must catch a mutated SIG constant, so the fixture must
        # not be built from it.
        base_slots[13], base_slots[14], base_slots[15] = \
            0x0204357c, 0x0204349c, 0x02043494
        for i, s in enumerate(base_slots):
            struct.pack_into("<I", img, 0x2000 - 0x1000 + 4 * i, s)
        cls = list(base_slots) + [0x1100]   # width 37
        cls[5], cls[6], cls[7] = 0x1100, 0x1200, 0x1300
        for i, s in enumerate(cls):
            struct.pack_into("<I", img, 0x2100 - 0x1000 + 4 * i, s)
        (tdp / "fake.bin").write_bytes(img)

        ov = Overlay("fake", tdp / "fake.bin", delinks)
        expect(ov.base == 0x1000 and ov.data_span == (0x2000, 0x3000),
               "base and data span from delinks", (ov.base, ov.data_span))
        hits = find_tables(ov)
        expect(hits == [0x2000, 0x2100], "signature census", hits)

        sources = Sources(tdp, [delinks])
        rows = class_rows(ov, base_slots, 0x2100, 37)
        n_ovr, n_mark, n_nosrc, detail = measure(rows, sources)
        expect((n_ovr, n_mark, n_nosrc) == (4, 2, 1),
               "ovr/mark/nosrc on the fixture", (n_ovr, n_mark, n_nosrc))
        expect([i for i, w, o, m in detail] == [5, 6, 7, 36],
               "override slot list", detail)
        expect(sources.owner(0x1300) is None and
               sources.owner(0x1250) == "src/f_1200.c",
               "delinks join edges", (sources.owner(0x1300),
                                      sources.owner(0x1250)))

        # refusals: width outside the ruling set, a signature-less address
        # (4 bytes off the real table -- the confident-wrong-table shape),
        # and a truncated image
        try:
            class_rows(ov, base_slots, 0x2100, 35)
            expect(False, "width 35 was ACCEPTED", "no SystemExit")
        except SystemExit as e:
            expect("ruling set" in str(e), "width refusal message", str(e))
        try:
            class_rows(ov, base_slots, 0x2104, 36)
            expect(False, "an off-by-4 vtable address was ACCEPTED",
                   "no SystemExit")
        except SystemExit as e:
            expect("ActorBase" in str(e) and "census" in str(e),
                   "signature refusal message", str(e))
        # THE SYMBOL SPAN. Arms added by run link60 lane MGB; the check
        # itself is MGA's. It shipped with no fixture arm and --reconstruct
        # cannot cover it either, because once the doc is corrected no row
        # trips the refusal -- so both `if False:` and `>=` mutants of it
        # survived a full selftest run. An unexercised guard is exactly the
        # failure this whole correction was about, one level up.
        far = {0x2100: "cls", 0x219c: "next_far"}      # span 39
        exact = {0x2100: "cls", 0x2194: "next_exact"}  # span 37
        near = {0x2100: "cls", 0x2190: "next_near"}    # span 36
        expect(symbol_span_words(far, 0x2100) == 39,
               "span from the next symbol", symbol_span_words(far, 0x2100))
        expect(symbol_span_words(far, 0x2104) is None,
               "an address that is not itself a symbol declines",
               symbol_span_words(far, 0x2104))
        # UPPER BOUND, NOT EQUALITY: 39 words of room, 37 declared, allowed.
        # This is 0x16d's real shape (152-byte span, genuinely 36 slots).
        expect(measure(class_rows(ov, base_slots, 0x2100, 37, far),
                       sources)[0] == 4,
               "a width BELOW the span is allowed", "refused")
        # EXACTLY at the span must be allowed too, and this arm is the one
        # that matters: 0x173 is the single genuine width-37 class and its
        # span is exactly 37, so a `>=` here would refuse the only row the
        # correction leaves alone.
        expect(measure(class_rows(ov, base_slots, 0x2100, 37, exact),
                       sources)[0] == 4,
               "a width EQUAL to the span is allowed", "refused")
        try:
            class_rows(ov, base_slots, 0x2100, 37, near)
            expect(False, "a width ABOVE the span was ACCEPTED",
                   "no SystemExit")
        except SystemExit as e:
            expect("does not fit" in str(e) and "36 words on" in str(e),
                   "span refusal message", str(e))

        # THE SPAWNINFO TAIL, run link60 lane MGB. A third signature table at
        # 0x2200 whose 36 slots are followed by an eight-byte SpawnInfo
        # record -- 0x16c's real shape. It is deliberately given NO symbols
        # at all, so symbol_span_words declines and only this check can
        # answer: that is the config-coverage hole MGA's own refusal message
        # names ("or the config is missing a symbol").
        tail = list(base_slots)
        tail[5] = 0x1100
        for i, s in enumerate(tail):
            struct.pack_into("<I", img, 0x2200 - 0x1000 + 4 * i, s)
        struct.pack_into("<I", img, 0x2200 - 0x1000 + 36 * 4, 0x1200)
        struct.pack_into("<I", img, 0x2200 - 0x1000 + 37 * 4, 0x01780178)
        (tdp / "tail.bin").write_bytes(img)
        ovt = Overlay("tail", tdp / "tail.bin", delinks)
        expect(symbol_span_words(None, 0x2200) is None,
               "the span check declines with no symbol table",
               symbol_span_words(None, 0x2200))
        expect(spawninfo_tail(ovt, 0x2200) == 0x178,
               "spawninfo tail detected", spawninfo_tail(ovt, 0x2200))
        try:
            class_rows(ovt, base_slots, 0x2200, 37)
            expect(False, "a SpawnInfo tail was ACCEPTED as slot 36",
                   "no SystemExit")
        except SystemExit as e:
            expect("SPAWNINFO" in str(e) and "0x178" in str(e),
                   "spawninfo-tail refusal message", str(e))
        n = measure(class_rows(ovt, base_slots, 0x2200, 36), sources)[0]
        expect(n == 1, "the same table reads as 1 override at width 36", n)
        # doubled halves OUTSIDE the minigame range are not a SpawnInfo, and
        # halves that disagree are not either. Without both tests this would
        # be a doubled-halves heuristic rather than a positive ROM read.
        struct.pack_into("<I", img, 0x2200 - 0x1000 + 37 * 4, 0x01000100)
        (tdp / "tail2.bin").write_bytes(img)
        expect(spawninfo_tail(Overlay("t2", _wr(tdp / "tail2.bin", img),
                                      delinks), 0x2200) is None,
               "a doubled id outside 0x169..0x186 is not a SpawnInfo", None)
        struct.pack_into("<I", img, 0x2200 - 0x1000 + 37 * 4, 0x01780179)
        expect(spawninfo_tail(Overlay("t3", _wr(tdp / "tail3.bin", img),
                                      delinks), 0x2200) is None,
               "mismatched halves are not a SpawnInfo", None)
        struct.pack_into("<I", img, 0x2200 - 0x1000 + 37 * 4, 0)

        (tdp / "cut.bin").write_bytes(img[:-4])
        try:
            Overlay("cut", tdp / "cut.bin", delinks)
            expect(False, "truncated image was ACCEPTED", "no SystemExit")
        except SystemExit as e:
            expect("stale or wrong image" in str(e),
                   "span refusal message", str(e))

        # the doc-table parser against a fabricated section 3 shape
        doc = tdp / "doc.txt"
        doc.write_text(
            "  id     spawn symbol   vtable     width  ovr  mark  nosrc\n"
            "  0x172  MgSortOrSplode              0213bbb4      36    6"
            "     5      0\n"
            "  0x176  MgShuffleShell (curling)    0213c304      36    6"
            "     5      0   DONE\n"
            "  0x179  MgSnowballSlalom            NOT FOUND -- see below\n")
        got = parse_doc_table(doc)
        expect(got == [(0x172, 0x0213bbb4, 36, 6, 5, 0),
                       (0x176, 0x0213c304, 36, 6, 5, 0)],
               "doc-table rows (DONE tolerated, NOT FOUND skipped)", got)

    print("selftest %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


def default_root():
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


def main():
    ap = argparse.ArgumentParser(
        description="vtable override rows from the raw overlays; see the "
                    "docstring")
    ap.add_argument("--root", default=default_root())
    ap.add_argument("--census", action="store_true")
    ap.add_argument("--vtable", type=lambda x: int(x, 0))
    ap.add_argument("--width", type=int)
    ap.add_argument("--reconstruct", action="store_true")
    ap.add_argument("--selftest", action="store_true")
    args = ap.parse_args()

    if args.selftest:
        sys.exit(selftest())
    if args.census:
        sys.exit(cmd_census(args.root))
    if args.reconstruct:
        sys.exit(cmd_reconstruct(args.root))
    if args.vtable is not None:
        if args.width is None:
            sys.exit("pass --width 36 or 37 (MG1's ruling for the class; "
                     "this tool does not derive it)")
        sys.exit(cmd_rows(args.root, args.vtable, args.width))
    sys.exit("pass --census, --vtable ADDR --width N, --reconstruct, or "
             "--selftest")


if __name__ == "__main__":
    main()
