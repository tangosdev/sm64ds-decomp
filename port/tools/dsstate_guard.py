"""Build-time guard: every hosted DS global must live in the .dsstate section.

The save state (hal/lk6_savestate.cpp) captures the hosted DS main-RAM arena
plus the whole .dsstate section, where every file that hosts DS globals routes
them with the DSSTATE_BEGIN/END markers (hal/dsstate_seg.h). The 0.2.0 restore
crash was a hosted global that was NOT captured -- the ANIM_PTRS SharedFilePtr
table in ov002. This guard is the ratchet that keeps that from recurring: it
reads the linked map and fails the build if a hosted DS symbol landed OUTSIDE
the bracket the snapshot copies.

WHAT COUNTS AS A HOSTED DS SYMBOL
--------------------------------
A public symbol in the read/write data segment whose name is a DS symbol the
port hosts:

    data_0XXXXXXX            a DS main-RAM BSS/data symbol (0x02......)
    data_ovNNN_0XXXXXXX      an overlay data symbol
    LCG_STATE_0XXXXXXX       the particle RNG
    port_ovNNN_image         a whole-overlay host image
    <Class>_SpawnInfo        an actor's spawn record, mounted by name
    _ZNK...E / _ZN...E       a C++-mangled DS data symbol (rare; the heap
                             registry root iterator is one)

Host-only symbols (walk_window's playlog path, ntr_2x window state, the CRT)
never match these patterns, so they are correctly ignored: they must NOT be in
.dsstate and the guard does not ask them to be.

...OR ANY READ/WRITE DATA SYMBOL IN A GENERATED MOUNT OBJECT, WHATEVER ITS NAME
------------------------------------------------------------------------------
The name list cannot be the whole test, and the way it fails is the way that
matters: it goes QUIET rather than red.

dsd spells a symbol data_ovNNN_ADDRESS only when it had nothing better to call
it. Every symbol it did have a name for matched nothing above, and neither did
the synthetic port_ovNNN_gap_ADDRESS blocks tools/ovdata.py invents for
un-symbolized statics. Counted off this build's own map, in the mount objects'
contributions to .dsstate: 285 *_SpawnInfo records, 13 _ZTV vtables and 51
_ZN...E data symbols, 349 in all. All of them are hosted DS storage. The 349
happened to be inside .dsstate and were checked by hand; the gap blocks in
packed mounts were NOT, and 28 of them sat outside the captured span until
tools/ovdata.py was fixed to route them. This guard was the thing that should
have said so and could not.

So the test is also keyed on the OBJECT. tools/ovdata.py emits nothing into
ovNNN_data.c / ovNNN_syms.c but hosted DS storage, so every READ/WRITE data
symbol in one of those objects belongs in .dsstate, whatever it is called and
whether or not anyone has thought of it yet. Read-only data in those objects is
correctly outside and falls out of the test by its segment, not by a name list:
the const DS-window pair port_ovNNN_ds_base/_ds_end and the MSVC string
literals behind the pack check all live in .rdata.

A LEADING UNDERSCORE. 32-bit MSVC decorates C symbols, data included, so a
mount's `_ZTV9PushBlock` is spelled `__ZTV9PushBlock` in the map. The two
C++-mangled exceptions below were written without it and so had never matched
anything; they are spelled with an optional one now. Both are inside .dsstate,
so nothing was stranded by it, but the guard was not proving what it said.

WHAT THE OBJECT TEST ACTUALLY ADDS, since the headline count is easy to
over-read. On the build it landed on it reaches 855 symbols the name test does
not, and they are not 855 more globals:

    462   MSVC-decorated C++ aliases of mount data, ?data_ov002_020ff480@@3PAHA
          and its kind, separate publics at addresses the name test already
          covers under the undecorated spelling
    247   pack padding slots, pkNNN_gap_ADDRESS, which the map lists even though
          they are static, and which an explicit allocate already routed
     64   the mangled mount names, _ZTV and _ZN...E
     48   the synthetic gap blocks, 28 of them the ones this test was added for
     34   the rest, mostly dsd-named func_ADDRESS symbols in mount objects that
          turned out to be referenced as data

So the number that grew is publics checked, not storage discovered. The reason
to keep all of it is the same either way: none of it costs anything to check,
and the test does not depend on anyone having named the shape first.

HOW THE BOUND IS READ
---------------------
hal/dsstate_seg.cpp puts dsstate_lo in .dsstate$aaa and dsstate_hi in
.dsstate$zzz, so [addr(dsstate_lo), addr(dsstate_hi)) is the captured span. A
hosted symbol passes iff its address is inside that half-open range. The two
sentinels themselves are excluded (they are the fence, not payload).

USAGE
    python tools/dsstate_guard.py <path-to-walk_window.map>

Exit 0 if every hosted DS symbol is inside .dsstate; non-zero (with the offending
symbols listed) otherwise.
"""

import re
import sys


# Publics-by-value rows look like:
#   0003:000002e8       _data_ov002_020ff480       005852e8     ov002_data.c.obj
# and, for a symbol the linker flagged as code, with an extra one-letter column:
#   0001:000936f0       _data_ov000_020ab3c4       004946f0 f   _ZN10dCapIcon_cC1Ev.c.obj
# groups: seg:off, name, rva+base, flag ('f' = function, 'i' = import, or none),
# object. The flag matters: dsd names some FUNCTIONS `data_*` (a symbol it saw
# referenced as data that turned out to be code), and those live in .text. They
# are not save state and must not be asked to move into .dsstate.
ROW = re.compile(
    r"^\s*([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})\s+"
    r"(?:([fi])\s+)?(\S+)")

# The map's leading section table. Rows look like:
#   0005:00000008 0002b8c8H .dsstate$mmm            DATA
# groups: seg, offset, length, name, class
SECTION_ROW = re.compile(
    r"^\s*([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+([0-9a-fA-F]+)H\s+(\.\S+)\s+(\S+)\s*$")

# A hosted DS symbol by name. Leading underscore is MSVC's cdecl decoration on
# C symbols; C++-mangled names (_ZN..) keep their own leading underscore-Z.
HOSTED = re.compile(
    r"^_?("
    r"data_0[0-9a-fA-F]{7}"          # DS main-RAM data/bss
    r"|data_ov\d+_0[0-9a-fA-F]{7}"   # overlay data
    r"|LCG_STATE_0[0-9a-fA-F]{7}"    # particle RNG
    r"|port_ov\d+_image"             # whole-overlay host image
    r"|[A-Za-z_][A-Za-z0-9_]*_SpawnInfo"   # an actor's spawn record
    r")$"
    # plus the handful of C++-mangled DS data symbols the port hosts; matched
    # loosely by the trailing E on a data name we know lands in rw data. The
    # inner underscore is MSVC's C decoration -- see the header.
    r"|^__?_ZN6Memory16rootHeapIteratorE$"
    r"|^__?_ZN6Memory25isRootHeapIterInitializedE$")

# A GENERATED OVERLAY-MOUNT OBJECT. tools/ovdata.py writes ovNNN_data.c for a
# plain or whole mount and ovNNN_syms.c for a packed one, and it puts nothing
# into either but hosted DS storage, so a read/write data symbol in one of them
# is save state by construction. This is the half of the test that does not
# depend on anybody having named the symbol shape first.
MOUNT_OBJ = re.compile(r"^ov\d+_(?:data|syms)\.c\.obj$")

# Sections that hold read-only data. A segment carrying one of these is not
# storage the game writes, so the object test above does not reach into it:
# that is what keeps the const port_ovNNN_ds_base/_ds_end pair and the pack
# check's string literals out of the result without naming them.
RO_SECTIONS = (".rdata", ".CRT", ".idata", ".edata", ".gfids", ".00cfg",
               ".rsrc", ".reloc", ".didat", ".tls")

SENTINELS = {"_dsstate_lo", "dsstate_lo", "_dsstate_hi", "dsstate_hi"}

# Named boot-constant exclusions. These ARE hosted DS globals but are written
# exactly once at boot and never during play, so they cannot diverge between a
# save and a load and are deliberately outside .dsstate. Each entry names every
# map spelling of the symbol (the real storage AND its /alternatename aliases:
# an alias appears in the map as its own public at the same address, so the
# guard must know both or it flags the alias). Keep the rationale next to the
# definition too -- data_020a0eac's is in hal/cxxname_bridge.cpp.
BOOT_CONSTANT = {
    # the game heap pointer: Heap::InitializeGameHeap writes it once off the
    # main.c boot spine; the port seeds it once before the frame loop.
    "_data_020a0eac", "data_020a0eac", "_data_020a0eac_c", "data_020a0eac_c",
    "?data_020a0eac@@3PAUHeap@@A",
    # THE VS STAR SPAWN ORDERS, and this one is stricter than the bar above:
    # it is not written once at boot, it is never written at all. Six 12-byte
    # rows of ROM constant at arm9 0x02075720, hosted by name as
    # VS_STAR_SPAWN_ORDERS in hal/bob_enemy_bridges.cpp, read as
    # data_0209f344[data_0209f208] and by the ov002/ov084 star-progress checks.
    #
    # It appears here rather than in the file that defines it because of the
    # SPELLING, which is the case this set's header was written for. The
    # storage has always been outside .dsstate and the guard never looked at
    # it: VS_STAR_SPAWN_ORDERS matches none of the HOSTED patterns. Run link60
    # Stage 4 added `/alternatename:_data_02075720=_VS_STAR_SPAWN_ORDERS` so
    # Stage::InitResources' address spelling reaches the same array instead of
    # a second copy of the bytes, and the alias is a public at the same address
    # under a data_ name -- so the guard saw the array for the first time and
    # correctly reported that a DS global is not captured. Nothing writes it,
    # so there is nothing to roll back, and moving a pure constant into the
    # snapshot would grow the captured span for no recoverable state.
    "_data_02075720", "data_02075720",
}

# Object files whose data_* symbols are READ-ONLY ROM constants, not mutable
# game state: the game never writes them, so they never diverge between a save
# and a load and do not need to roll back. romdata.py emits only such constants
# (its own header says so, and it explicitly excludes the symbols that ARE
# written at runtime, which live in model_host.cpp / auto_bss.cpp and so are in
# .dsstate). Capturing them would just bloat the snapshot by the ROM's constant
# tables. They are allowed to sit outside .dsstate.
CONST_OBJS = ("romdata.c.obj",)


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: dsstate_guard.py <walk_window.map>")
    try:
        text = open(sys.argv[1], encoding="ascii", errors="replace").read()
    except OSError as e:
        sys.exit(f"dsstate_guard: cannot read map: {e}")

    lo = hi = None
    rows = []
    dsstate_segs = {}
    seg_sections = {}
    seg_class = {}
    for line in text.splitlines():
        s = SECTION_ROW.match(line)
        if s:
            sseg, soff, slen, sname, sclass = s.groups()
            seg_sections.setdefault(sseg, set()).add(sname)
            seg_class[sseg] = sclass
            if sname.startswith(".dsstate"):
                dsstate_segs.setdefault(sseg, []).append(sname)
            continue
        m = ROW.match(line)
        if not m:
            continue
        seg, off, name, rva, flag, obj = m.groups()
        if flag == "f":       # code, not data -- see the ROW comment
            continue
        rva = int(rva, 16)
        if name in ("_dsstate_lo", "dsstate_lo"):
            lo = rva
        elif name in ("_dsstate_hi", "dsstate_hi"):
            hi = rva
        rows.append((seg, name, rva, obj))

    # The segments the object test is allowed to reach into: DATA-class ones
    # holding no read-only section. On this build that is .bss/.data, .dsstate
    # and the handful of custom grouped families, and it excludes the one
    # carrying .rdata, which is where a mount's const DS-window pair and every
    # string literal behind its pack check live.
    rw_segs = {sseg for sseg, names in seg_sections.items()
               if seg_class.get(sseg) == "DATA"
               and not any(n.startswith(RO_SECTIONS) for n in names)}

    # The split-section trap. MSVC will not merge two same-named sections whose
    # characteristics differ, so a contribution routed in with a bare bss_seg
    # (CNT_UNINITIALIZED_DATA) forms a SECOND output section also called
    # .dsstate, placed outside the sentinels, and every hosted global in it is
    # silently uncaptured. The compiler says only `LNK4078` about it, which is a
    # warning nobody reads in a 2000-line build log. Catch it here, by name,
    # because the symptom otherwise reads as "hundreds of files forgot their
    # markers" and sends the reader off fixing files that are already correct.
    if len(dsstate_segs) > 1:
        print(f"dsstate_guard: .dsstate was SPLIT across {len(dsstate_segs)} "
              f"segments -- the linker refused to merge them because their "
              f"section attributes differ (this is what LNK4078 means). Only "
              f"the segment holding the sentinels is captured; the rest is "
              f"silently lost on a restore.", file=sys.stderr)
        for sseg, names in sorted(dsstate_segs.items()):
            shown = ", ".join(sorted(set(names))[:4])
            more = "" if len(set(names)) <= 4 else f" (+{len(set(names)) - 4} more)"
            print(f"  segment {sseg}: {shown}{more}", file=sys.stderr)
        print("Fix: every file routing into .dsstate must declare the section's "
              "attributes BEFORE routing, so both bss- and data-flavoured "
              "contributions agree: `#pragma section(\".dsstate$mmm\", read, "
              "write)`. hal/dsstate_seg.h does this for anything that includes "
              "it; a generator emitting raw pragmas must emit it too.",
              file=sys.stderr)
        sys.exit(1)

    if lo is None or hi is None:
        sys.exit("dsstate_guard: .dsstate sentinels (dsstate_lo/dsstate_hi) not "
                 "found in the map -- hal/dsstate_seg.cpp is not linked into this "
                 "target, or the section collapsed. The save state cannot "
                 "capture the hosted DS globals without it.")
    if hi <= lo:
        sys.exit(f"dsstate_guard: .dsstate span is empty or inverted "
                 f"(lo={lo:#x} hi={hi:#x}); the section did not gather anything.")

    outside = []
    seen = set()
    by_name = by_obj = 0
    for seg, name, rva, obj in rows:
        if name in SENTINELS or name in BOOT_CONSTANT:
            continue
        named = bool(HOSTED.match(name))
        mounted = bool(MOUNT_OBJ.match(obj)) and seg in rw_segs
        if not (named or mounted):
            continue
        if name in seen:      # a symbol can appear under several C++ aliases
            continue
        seen.add(name)
        if named:
            by_name += 1
        else:
            by_obj += 1
        if obj.endswith(CONST_OBJS):   # read-only ROM constants, never diverge
            continue
        if not (lo <= rva < hi):
            outside.append((name, rva, obj))

    if outside:
        print(f"dsstate_guard: {len(outside)} hosted DS symbol(s) are OUTSIDE "
              f".dsstate [{lo:#x}, {hi:#x}) and would NOT be captured by a save "
              f"state:", file=sys.stderr)
        for name, rva, obj in sorted(outside, key=lambda r: r[0])[:60]:
            print(f"  {name}  @ {rva:#010x}  ({obj})", file=sys.stderr)
        if len(outside) > 60:
            print(f"  ... and {len(outside) - 60} more", file=sys.stderr)
        print("Fix, in the file that DEFINES each symbol above:\n"
              "  - a plain global: put DSSTATE_BEGIN before and DSSTATE_END "
              "after it (hal/dsstate_seg.h).\n"
              "  - a global placed by hand into its own grouped section to keep "
              "ROM spacing (the COMM/MMBLK/SAVEBLK-style macros): the segment "
              "default does NOT reach it, because __declspec(allocate) wins. "
              "Rename its section into the captured family instead, "
              "`.dsstate$<family><NNNN>`, the way tools/ovdata.py names packed "
              "overlay mounts `.dsstate$pkNNN_SSSS`.\n"
              "  - a generated overlay file (ovNNN_data.c / ovNNN_syms.c): the "
              "symbol above is not hand-written, so fix the EMITTER. Every "
              "array tools/ovdata.py writes needs either an explicit "
              "__declspec(allocate(\".dsstate$...\")) or the .dsstate default "
              "data_seg open around it; a packed mount opens no default, so an "
              "ordinary file-scope array there falls out of the capture. "
              "port/tools/gapaudit.py lists the blocks and which side of the "
              "span they land on.", file=sys.stderr)
        sys.exit(1)

    print(f"dsstate_guard: OK -- {len(seen)} hosted DS symbols all inside "
          f".dsstate [{lo:#x}, {hi:#x}), {(hi - lo)} bytes captured "
          f"({by_name} matched by name, {by_obj} by mount object).")


if __name__ == "__main__":
    main()
