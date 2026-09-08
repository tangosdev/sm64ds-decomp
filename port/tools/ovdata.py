#!/usr/bin/env python3
"""Emit overlay data symbols as C arrays, bytes from the overlay image.

The Player (and every overlay actor) reads static tables baked into its
overlay's data section: animation-file entries, state objects, tuning
constants. Host symbols need that content, so this reads the overlay
binary plus its symbols.txt (sizes = next-symbol deltas) and emits real
bytes for a requested symbol list. Never committed: Nintendo bytes stay
out of git, same policy as romdata.py.

DS-code POINTERS inside emitted data (state Init/Main/Cleanup slots and
the like) are meaningless on host; callers patch those fields at runtime
with host addresses -- see the gate-10 smoke.

    python tools/ovdata.py <ovNNN> <out.c> <symbol> [symbol...]
    python tools/ovdata.py ov002 out.c --from-list syms.txt
    python tools/ovdata.py ov009 out.c --whole
    python tools/ovdata.py --cross out.c a.c.map b.c.map [...]

--whole emits the ENTIRE overlay as one aligned array plus a reloc-driven
rebase (see whole_mode below). Per-symbol emission is wrong for a level
overlay: the LVL_Overlay's object tables are walked across symbol boundaries
by count, so eight separate host arrays put a neighbour's bytes wherever the
walk steps past the symbol dsd happened to name.

CROSS-MOUNT POINTERS. Each mount is its own invocation (CMakeLists runs nine),
so no run can see another's symbols, and the per-mount pointer pass below can
only rebase a pointer whose target lands inside its OWN emission. A pointer
from one mounted overlay into another was left holding its DS address, which
is not a loud failure: the port reserves DS main RAM at 0x02000000 as zeroed
pages, so the read succeeds and returns zeros. For a sprite-list pointer that
means an OamAttr list with no 0xffff terminator anywhere, and OAM::Render's
cull path does not touch the entry counter, so the walk is unbounded the
moment the sprite goes off screen and the fault lands megabytes from the
cause. See hal/oam_lists.cpp.

So every run also writes a sidecar `<out.c>.map`, and a final --cross pass
reads all of them and emits one patch function for the pointers that cross.
Two phases rather than one all-mounts invocation: the mounts differ in mode
(--whole, --pack, plain), and one invocation per mount keeps a list edit from
rebuilding all nine.

The cross pass drops a pointer whose target could name more than one mount's
copy, and "could" is a RESIDENCY question, not an address question: overlays
that the DS never has loaded at the same time are not two answers. See the
Residency class.
"""

import bisect
import json
import os
import pathlib
import re
import struct
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import link_only  # noqa: E402
import romblob_common  # noqa: E402

_YAML_TEXT = {}
_OVT = {}
_BASE_WARNED = set()
_BSS_WARNED = set()


def overlay_yaml(root, ovid, field):
    """A field out of ONE overlay's record in overlays.yaml.

    The record is found by its own `- id:` line and nothing else. Matching
    "id: NN" anywhere in the file also matches the `file_id: NN` line of a
    DIFFERENT overlay -- ov085's base came back as ov043's that way, and the
    only symptom was the BL guard below firing on a byte that was never code.
    """
    key = str(root)
    if key not in _YAML_TEXT:
        _YAML_TEXT[key] = (
            root / "extracted/dsd/arm9_overlays/overlays.yaml").read_text()
    ytxt = _YAML_TEXT[key]
    blk = re.search(rf"^  - id: {ovid}$\n(.*?)(?=^  - id: |\Z)", ytxt,
                    re.S | re.M)
    if not blk:
        sys.exit(f"overlays.yaml has no record for overlay {ovid}")
    m = re.search(rf"^    {field}: (\d+)$", blk.group(1), re.M)
    if not m:
        sys.exit(f"overlays.yaml: overlay {ovid} has no {field}")
    return int(m.group(1))


def delinks_sections(root, ov):
    """[(start, end)] for the section table at the head of delinks.txt.

    delinks.txt opens with one indented line per section (.text, .rodata,
    .init, .ctor, .data, .bss) and then switches to the per-file listing,
    whose entries start at column 0. Stop there: the per-file lines carry
    start:/end: too and they are ranges INSIDE a section, not sections.
    """
    out = []
    path = root / f"config/arm9/overlays/{ov}/delinks.txt"
    for line in path.read_text().splitlines():
        ms = re.search(
            r"^\s+\.(\w+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)",
            line)
        if ms:
            out.append((int(ms.group(2), 16), int(ms.group(3), 16)))
        elif out and not line.startswith(" "):
            break
    if not out:
        sys.exit(f"{path}: no section table")
    return out


def rom_path(root):
    """The user's .nds, if this checkout has one. None is normal."""
    env = os.environ.get("SM64DS_ROM")
    if env and pathlib.Path(env).is_file():
        return pathlib.Path(env)
    for p in sorted(root.glob("*.nds")):
        return p
    return None


def rom_overlay_table(root):
    """{id: (ram_address, ram_size, bss_size)} from the ROM, or {} with no ROM.

    The NDS header's word at +0x50 is the arm9 overlay table's file offset and
    +0x54 its byte size; entries are 32 bytes of (id, ram_address, ram_size,
    bss_size, static_init_start, static_init_end, file_id, flags).
    """
    key = str(root)
    if key in _OVT:
        return _OVT[key]
    path = rom_path(root)
    if path is None:
        _OVT[key] = {}
        return _OVT[key]
    rom = path.read_bytes()
    off, size = struct.unpack_from("<II", rom, 0x50)
    table = {}
    for i in range(size // 32):
        ovid, ram, ramsz, bsssz = struct.unpack_from("<IIII", rom,
                                                     off + i * 32)
        table[ovid] = (ram, ramsz, bsssz)
    _OVT[key] = table
    return table


def overlay_base(root, ovid):
    """Where the overlay loads: the LOWEST section start in its delinks.txt.

    THE BASE COMES FROM THE DELINK CONFIG, NOT FROM overlays.yaml. The yaml is
    a dsd export and its base_address is wrong for 22 of the 103 overlays --
    the eight level overlays that read 0x021111c0 instead of 0x021111a0, and
    fourteen more (ov061, ov067, ov068, ov069, ov076, ov082, ov083, ov086,
    ov087, ov088, ov093, ov097, ov099, ov101) that are wrong by as much as
    0x9000. config/arm9/overlays/<ov>/delinks.txt is the config the rest of
    this file already trusts for section ends and symbol sizes, and its lowest
    section start equals the ROM's own overlay-table ram_address for 103 of
    103. That is the number this build measured, not a claim inherited from a
    note.

    WHAT READING A BASE WRONG COSTS. It does not fail, it lies: a level
    overlay read 32 bytes off returns the wrong LVL_Overlay record, so its
    four file handles resolve to unrelated art or to nothing. Run linkw wave 8
    declined five real levels that way -- 0, 30, 32, 34 and 46 (test_map, the
    Secret Aquarium, Vanish Cap Under the Moat, Wing Mario Over the Rainbow,
    Luigi's key course), all mounted now.

    Two checks, and they are deliberately asymmetric:

      the yaml is EXPECTED to disagree, so a disagreement prints once per
      overlay per run and carries on. Silence would let the yaml quietly come
      back as the source of truth for someone reading this later.

      the ROM overlay table is ground truth, so a disagreement REFUSES. If
      this checkout has no .nds (the normal case on a build box) the check is
      skipped -- absent, not passed.
    """
    base = min(s for s, _ in delinks_sections(root, f"ov{ovid:03d}"))

    ovt = rom_overlay_table(root)
    if ovid in ovt and ovt[ovid][0] != base:
        sys.exit(f"ov{ovid:03d}: delinks.txt says base {base:#010x}, the ROM "
                 f"overlay table says {ovt[ovid][0]:#010x}. The ROM is ground "
                 f"truth and the config disagrees with it; refusing to emit.")

    # LINK-ONLY: overlays.yaml is a dsd export and lives under extracted/, so a
    # cartridge-free run has no yaml to disagree with. The check is a WARNING
    # about a source this function already refuses to trust, so skipping it
    # loses nothing the emission depends on. See port/tools/link_only.py.
    if link_only.LINK_ONLY:
        return base

    yaml_base = overlay_yaml(root, ovid, "base_address")
    if yaml_base != base and ovid not in _BASE_WARNED:
        _BASE_WARNED.add(ovid)
        print(f"ov{ovid:03d}: overlays.yaml says base {yaml_base:#010x}, "
              f"delinks.txt says {base:#010x}; using delinks. The yaml is a "
              f"dsd export and is wrong on 22 bases -- see overlay_base().")
    return base


def overlay_bss(root, ovid, image_len):
    """How much zeroed space the loader leaves past the overlay's image.

    THE BSS SIZE COMES FROM THE DELINK CONFIG, NOT FROM overlays.yaml, for the
    same reason and with the same evidence as overlay_base() above. The yaml's
    bss_size is short for four overlays -- ov004 by 256, ov007 by 64, ov075 by
    256 and ov099 by 1024 -- while delinks.txt's HIGHEST section end equals the
    ROM overlay table's ram_address + ram_size + bss_size for 103 of 103. That
    is measured on a checkout that has the .nds, not inherited from a note.

    WHAT READING IT SHORT COSTS, since none of the four was mounted until now
    and the defect was therefore invisible. The number feeds the overlay's
    FOOTPRINT, and the footprint is the window cross_mode() contests over and
    the span Residency reasons about. A short window silently disowns the
    overlay's own top-of-bss storage: an ov007 per-symbol mount taking the yaml
    at its word declares six of its own bss symbols to be somebody else's
    memory, and cross_mode's out-of-footprint filter then refuses to offer them
    to anyone. No mount wants those six addresses today, so nothing misbinds;
    this is so that the first one to want them gets an answer.

    The two checks mirror overlay_base()'s and are asymmetric for the same
    reason: the yaml is expected to disagree and only prints, the ROM overlay
    table is ground truth and refuses. A checkout with no .nds skips the ROM
    check -- absent, not passed.
    """
    ov = f"ov{ovid:03d}"
    base = overlay_base(root, ovid)
    bss = max(e for _, e in delinks_sections(root, ov)) - base - image_len
    if bss < 0:
        sys.exit(f"{ov}: delinks.txt's last section ends before the image "
                 f"does ({image_len} bytes from {base:#010x}); the config and "
                 f"the extracted overlay disagree about the image length.")

    ovt = rom_overlay_table(root)
    if ovid in ovt and ovt[ovid][2] != bss:
        sys.exit(f"{ov}: delinks.txt implies bss {bss}, the ROM overlay table "
                 f"says {ovt[ovid][2]}. The ROM is ground truth and the config "
                 f"disagrees with it; refusing to emit.")

    if link_only.LINK_ONLY:
        return bss                       # no yaml to warn against -- see above

    yaml_bss = overlay_yaml(root, ovid, "bss_size")
    if yaml_bss != bss and ovid not in _BSS_WARNED:
        _BSS_WARNED.add(ovid)
        print(f"ov{ovid:03d}: overlays.yaml says bss {yaml_bss}, delinks.txt "
              f"says {bss}; using delinks. The yaml is a dsd export and is "
              f"short on four -- see overlay_bss().")
    return bss


def load_relocs(root, ov):
    """{site address: target address} for every kind:load reloc."""
    relocs = {}
    rel_path = root / f"config/arm9/overlays/{ov}/relocs.txt"
    if rel_path.exists():
        for line in rel_path.read_text().splitlines():
            mm = re.search(r"from:0x([0-9a-fA-F]+)\s+kind:load\s+to:0x([0-9a-fA-F]+)",
                           line)
            if mm:
                relocs[int(mm.group(1), 16)] = int(mm.group(2), 16)
    return relocs


# A synthetic gap block, as named below. Matched generically because the cross
# pass sees blocks from every mount at once, not just one overlay's.
IS_GAP = re.compile(r"^port_ov\d+_gap_")


def make_covering(ivals):
    """(start, end, name) intervals -> a lookup that picks ONE for an address.

    SYMBOLS OVERLAP, so the nearest preceding start is not the answer.
    A synthetic gap block is one run covering every un-symbolized target in
    a 0x100 neighbourhood, and named symbols sit inside it: ov002's
    port_ov002_gap_0210c548 is 1452 bytes and swallows six of them, the
    first being OAM::MM_STAR_MARKERS at 0x0210c5b8. A lookup that stopped at
    the nearest start answered "MM_STAR_MARKERS" for every target past
    0x0210c5b8, found it did not reach, and returned NOTHING -- so those
    pointers stayed raw DS addresses.

    What that costs: HUD::RenderHealthMeter reads its sprite list out of
    data_ov002_0210c230, whose nine entries all point at 0x0210c5d8..0x678.
    Left un-rebased they point into the port's reserved DS main RAM, which
    is mapped and ZEROED, so OAM::Render's list walk never meets the
    0xffff terminator and runs the full 2.9 MB to 0x02400000 -- one byte
    past the region -- and takes the process with it.

    So scan back over every candidate that could still reach v and pick
    among all of them, in this order:

      1. A NAMED SYMBOL BEATS A GAP BLOCK. Named symbols are the storage the
         game reaches BY NAME, and a pointer that aliases one has to land on
         the same bytes or a write through the name is invisible through the
         pointer. A gap block is only ever reached through a patched pointer,
         so it never has that obligation.
      2. Among named symbols, the SMALLEST -- the most specific.
      3. Among gap blocks, the one that REACHES FURTHEST past v. Gap blocks
         overlap each other: the run builder merges neighbours within 0x100
         and later iterations lay short (t, t + 0x18) stubs over runs an
         earlier iteration already covered, so ov002 has a 24-byte
         port_ov002_gap_0210c158 sitting across a 212-byte
         port_ov002_gap_0210c168. Both hold the same ROM bytes, but a walk
         that starts at 0x0210c168 gets eight bytes from the first and 212
         from the second, and running out of array is the failure this whole
         pass exists to prevent. Take the one with the bytes.
    """
    ivals = sorted(ivals)
    starts = [iv[0] for iv in ivals]
    widest = max((e - s for s, e, _ in ivals), default=0)

    def covering(v):
        best = None
        best_key = None
        i = bisect.bisect_right(starts, v) - 1
        while i >= 0 and v - ivals[i][0] <= widest:
            s, e, n = ivals[i]
            if s <= v < e:
                is_gap = bool(IS_GAP.match(n))
                key = (is_gap, e - s if not is_gap else -(e - v))
                if best_key is None or key < best_key:
                    best, best_key = ivals[i], key
            i -= 1
        return best

    return covering


def overlay_footprint(root, ovid):
    """(base, base + image + bss): every DS address the loaded overlay owns.

    The same arithmetic write_map() uses for a mount's window, so the two are
    comparable and cross_mode() checks that they agree. Image length comes off
    the raw ndspy dump (its byte length is the ROM overlay table's ram_size);
    bss is past the image and the loader zeroes it, so an address-containment
    test has to include it.
    """
    ov = f"ov{ovid:03d}"
    base = overlay_base(root, ovid)
    if link_only.LINK_ONLY:
        # The footprint is base .. highest delinks section end either way; with
        # a cartridge the length comes off the dump and the bss is the
        # remainder, without one both halves come off the same section table.
        # image_span() documents why .bss's start IS the file image length.
        image_len = link_only.image_span(root, f"arm9/overlays/{ov}")[1]
    else:
        img = root / f"extracted/overlays/overlay_{ovid:04d}.bin"
        if not img.exists():
            img = root / f"extracted/dsd/arm9_overlays/{ov}.bin"
        image_len = img.stat().st_size
    return (base, base + image_len + overlay_bss(root, ovid, image_len))


def _src_defining(root, func):
    """The matched TU that defines `func`. Named after it by convention."""
    for ext in (".c", ".cpp"):
        p = root / "src" / (func + ext)
        if p.exists():
            return p
    hits = [p for p in sorted((root / "src").glob(f"*{func}*"))
            if p.suffix in (".c", ".cpp")]
    if len(hits) == 1:
        return hits[0]
    sys.exit(f"residency: cannot find the src TU defining {func}(). The model "
             f"is read out of the loader's own code and will not be guessed "
             f"at; if the function was renamed, update _src_defining()'s "
             f"caller in port/tools/ovdata.py.")


class Residency:
    """Which mounted overlays can be LIVE AT THE SAME TIME.

    The cross pass has to decide whether two overlays whose windows both
    contain a DS address are really two candidate answers. Purely by address
    they always are, and that is wrong often enough to matter: ov002 and ov006
    both cover 0x0210da20 and the DS never has both loaded, so a pointer out
    of a level overlay into that address has exactly one meaning.

    CO-RESIDENCY IS NOT AN EQUIVALENCE RELATION, so this is a relation and not
    a partition into groups. ov009 and ov014 both co-reside with ov002 and
    never with each other; ov004 co-resides with ov006 and with nothing else.
    Any code that tries to bucket overlays into disjoint groups gets one of
    those two wrong.

    Three rules. Two are derived, one is asserted, and this docstring says
    which is which because a residency claim nobody can re-derive is exactly
    the folklore this replaces.

    RULE 1, OVERLAP (derived, arithmetic). Two overlays whose footprints
    intersect can never both be loaded: the second would write over the first.
    No loader knowledge needed. This alone covers every overlay that shares a
    base -- the fifty-odd level overlays at 0x021111a0, the alternate actor
    pairs ov084/ov085 and ov089/ov091 -- and it covers ov002 against ov005 and
    ov006, which start at 0x020bfec0, inside ov002's span.

    RULE 2, THE SCENE SLOT (derived, read out of the loader's own matched TUs).
    src/GetSceneOverlayID.c maps every scene actor id to its overlay, and the
    set of overlays it can return IS the set of scene occupants. func_0201a694
    installs one: it calls func_0201a754(old) before func_0201a798(new), so at
    most one is loaded and they are pairwise exclusive. func_0201a798 also
    loads a PASSENGER -- `if (id == &overlay_6) LoadOverlay(&overlay_4)` -- and
    func_0201a754 unloads the same pair, so ov004 is live exactly when ov006
    is. All three facts are parsed from those files rather than restated here;
    if the files stop parsing the tool refuses instead of guessing.

    Rule 2 is what rule 1 cannot see. ov007 (0x020ad660..0x02104c40) does not
    reach a level overlay's base, so rule 1 alone would call the two
    co-resident and let an ov007 window contest ov002's providers.

    RULE 3, LEVEL TERRITORY (ASSERTED, not derived). Every overlay based at or
    above the scene slot's top -- the address where the level and object
    overlays stack, 0x021111a0 -- is loaded by the in-level machinery
    (LoadLevelOverlays and LoadOrUnloadObjectOverlays,
    src/_Z17LoadLevelOverlaysi.cpp) and so is live only while the in-level
    scene is. Which scene that is IS derived: the scene overlay whose
    footprint ENDS exactly at that base, which resolves uniquely to ov002 and
    is checked below. What is asserted is the premise that a level or object
    overlay never loads under any other scene, and the evidence for it is the
    load code (a level runs out of ov002, and func_0201a694 unloads the scene
    overlay it is running on before switching) plus the fact that no other
    scene occupant's footprint reaches this base, so the level overlays would
    sit above a hole under any of them. It is not proved here, and a reader
    who wants it proved should read the callers of LoadLevelOverlays.

    Overlays BELOW the slot get no rule 3. ov000 and ov001 load before any
    scene and are not scene occupants: ov001 stays resident across scene
    switches, so it is co-resident with all of them, and rule 1 already keeps
    it exclusive of ov000.

    Neither do STICKY overlays, which sit in territory but outlive the scene
    switch. ov075 is loaded by func_0201a694 at scene id 6 and released by
    func_0201a614 at scene id 1, so it is live across everything in between.
    Rule 3 would call it exclusive of ov003 and be wrong the permissive way.
    The set is derived the same way the rest is -- see __init__.
    """

    def __init__(self, root, footprints):
        self.foot = dict(footprints)

        scene_src = _src_defining(root, "GetSceneOverlayID").read_text()
        self.scene = {int(n) for n in
                      re.findall(r"return \(int\)&overlay_(\d+);", scene_src)}
        if not self.scene:
            sys.exit("residency: GetSceneOverlayID's source names no "
                     "overlay_N -- refusing to guess the scene set.")

        # THE PASSENGER PARSE FAILING OPEN IS THE DANGEROUS DIRECTION, so it
        # refuses like the scene parse does. An empty map does not merely lose
        # the ov004/ov006 pairing: it drops ov004 out of self.slot, and a
        # non-slot ov004 stops being covered by rule 3, so coresident(ov004,
        # every level overlay) flips from False to True. That is the
        # over-permissive direction with every cross want behind it, and three
        # ordinary edits to this one file reach it -- renaming the consts,
        # changing the cast style, adding braces round the if.
        load_src = _src_defining(root, "func_0201a798").read_text()
        self.passenger = {
            int(a): int(b) for a, b in re.findall(
                r"if \(id == \(int\)&overlay_(\d+)\)\s*"
                r"LoadOverlay\(\(int\)&overlay_(\d+)\);", load_src)}
        if not self.passenger:
            sys.exit(f"residency: {_src_defining(root, 'func_0201a798')} names "
                     f"no `if (id == (int)&overlay_A) "
                     f"LoadOverlay((int)&overlay_B);` pair. The scene slot's "
                     f"passenger rule is read out of that line and will not be "
                     f"guessed at -- an empty map silently makes the rider "
                     f"co-resident with every level overlay. If the function "
                     f"was rewritten, re-derive the rule here.")

        # Match the UNLOAD CALL, not the identifier: `extern int overlay_4;`
        # sits at the top of the file, so a substring test passes on the
        # declaration alone and a gutted UnloadOverlay() goes unnoticed.
        unload_path = _src_defining(root, "func_0201a754")
        unload_body = "\n".join(
            ln for ln in unload_path.read_text().splitlines()
            if not ln.lstrip().startswith("extern"))
        for host, rider in self.passenger.items():
            if not (re.search(r"UnloadOverlay\([^;]*\boverlay_%d\b" % rider,
                              unload_body)
                    and re.search(r"\boverlay_%d\b" % host, unload_body)):
                sys.exit(f"residency: func_0201a798 loads ov{rider:03d} with "
                         f"ov{host:03d} but {unload_path} has no "
                         f"UnloadOverlay() call on ov{rider:03d} keyed off "
                         f"ov{host:03d} -- the load and unload halves "
                         f"disagree, so the pair is not a passenger pair.")

        # The exclusivity premise, checked rather than assumed: the switch
        # unloads whatever is in the slot before it loads the replacement.
        # Ignore the calls that name an overlay LITERALLY -- those are the
        # STICKY overlays below, which are not the slot.
        switch_src = _src_defining(root, "func_0201a694").read_text()
        body = switch_src[switch_src.index("int func_0201a694("):]
        seq = [m.group(1) for m in re.finditer(
            r"(func_0201a754|func_0201a798)\((?!\(int\)&overlay_)", body)]
        if seq[:2] != ["func_0201a754", "func_0201a798"]:
            sys.exit("residency: func_0201a694 does not unload the previous "
                     "scene overlay before loading the next -- the scene slot "
                     "is not exclusive and this model is wrong.")

        # STICKY OVERLAYS: loaded THROUGH the slot helpers but not the slot.
        # func_0201a694 calls func_0201a798((int)&overlay_75) when the scene id
        # is 6, and func_0201a614 calls func_0201a754((int)&overlay_75) when it
        # is 1, both behind the data_0209d4e0 latch. Loaded at one scene id and
        # released at a different one, so its lifetime spans scene switches by
        # construction and it is not exclusive with anything the slot does. Any
        # overlay named literally in those two functions and not returned by
        # GetSceneOverlayID is one of these; ov075 is the only one today.
        #
        # It has to be exempt from rule 3 or the model is wrong in the
        # over-permissive direction: ov075's footprint (0x02113ee0..0x0211d8c0)
        # lands in level territory, so rule 3 would call it exclusive of ov003,
        # and it is not -- a level-to-star-select switch keeps ov075 loaded.
        # Nothing misbinds today because ov075 is not mounted; this is so that
        # the first mount of it does not bind wrong and uncontested.
        self.sticky = set()
        for func in ("func_0201a694", "func_0201a614"):
            src = _src_defining(root, func).read_text()
            for n in re.findall(
                    r"func_0201a(?:754|798)\(\(int\)&overlay_(\d+)\)", src):
                if int(n) not in self.scene:
                    self.sticky.add(int(n))

        self.slot = set(self.scene) | set(self.passenger.values())
        if self.slot & self.sticky:
            sys.exit(f"residency: ov{min(self.slot & self.sticky):03d} is both "
                     f"a scene occupant and loaded literally by the switch -- "
                     f"the two readings contradict each other.")
        for ovid in self.slot | self.sticky:
            if ovid not in self.foot:
                self.foot[ovid] = overlay_footprint(root, ovid)

        # RULE 1 AND RULE 2 CAN CONTRADICT EACH OTHER, AND RULE 1 WINS SILENTLY.
        #
        # coresident() tests overlap before it tests the passenger relation, so
        # if a passenger pair's footprints ever intersect the answer flips to
        # False for two overlays the loader's own code always loads together --
        # in the direction that drops bindings, with nothing printed.
        #
        # This is not hypothetical margin. ov004's footprint ends at 0x020bfec0
        # and ov006's base IS 0x020bfec0, so the pair passes only because
        # _overlaps() uses a strict `<`. There is exactly zero slack, and it was
        # 256 bytes until overlay_bss() started reading the ROM's bss instead of
        # the yaml's short one. One more byte of ov004 from any future
        # re-derivation and the model would quietly contradict itself.
        for host, rider in self.passenger.items():
            if self._overlaps(host, rider):
                (s1, e1), (s2, e2) = self.foot[host], self.foot[rider]
                sys.exit(f"residency: ov{rider:03d} rides ov{host:03d} per "
                         f"func_0201a798, but their footprints overlap "
                         f"({s2:#010x}..{e2:#010x} against "
                         f"{s1:#010x}..{e1:#010x}). Rule 1 is tested first and "
                         f"would report the pair NOT co-resident, which is the "
                         f"direction that drops bindings. One of the two "
                         f"footprints is wrong, or the passenger rule no longer "
                         f"holds; either way this must not be decided by "
                         f"whichever rule runs first.")

        # The level and object overlays stack on top of the in-level scene.
        # Derive WHICH scene that is rather than naming ov002: it is the slot
        # occupant whose footprint ends where they start. Unique, or refuse.
        top_of_slot = max(self.foot[i][0] for i in self.slot)
        self.territory = min((s for i, (s, _) in self.foot.items()
                              if i not in self.slot and i not in self.sticky
                              and s >= top_of_slot),
                             default=None)
        hosts = [i for i in self.slot if self.foot[i][1] == self.territory]
        if self.territory is None or len(hosts) != 1:
            sys.exit(f"residency: the level-overlay base {self.territory} is "
                     f"the top of {len(hosts)} scene overlays, not exactly "
                     f"one -- cannot say which scene a level rides. Add a "
                     f"level overlay to the mount set or work out why the "
                     f"slot's top moved.")
        self.level_scene = hosts[0]

    def describe(self):
        return ("residency: scene slot %s, passengers %s, sticky %s, level "
                "territory from %#010x on the ov%03d scene"
                % (", ".join("ov%03d" % i for i in sorted(self.slot)),
                   ", ".join("ov%03d rides ov%03d" % (r, h)
                             for h, r in sorted(self.passenger.items()))
                   or "none",
                   ", ".join("ov%03d" % i for i in sorted(self.sticky))
                   or "none",
                   self.territory, self.level_scene))

    def _overlaps(self, a, b):
        (s1, e1), (s2, e2) = self.foot[a], self.foot[b]
        return s1 < e2 and s2 < e1

    def _territory(self, ovid):
        """A level or object overlay: stacked above the slot, and not sticky."""
        return (ovid not in self.slot and ovid not in self.sticky
                and self.foot[ovid][0] >= self.territory)

    def coresident(self, a, b):
        """Can overlays a and b be loaded at the same instant?"""
        if a == b:
            return True
        if a not in self.foot or b not in self.foot:
            sys.exit(f"residency: no footprint for ov{a:03d}/ov{b:03d}")
        if self._overlaps(a, b):
            return False                                   # rule 1
        a_slot, b_slot = a in self.slot, b in self.slot
        if a_slot and b_slot:                              # rule 2
            return self.passenger.get(a) == b or self.passenger.get(b) == a
        if a_slot and self._territory(b):                  # rule 3
            return a == self.level_scene
        if b_slot and self._territory(a):                  # rule 3
            return b == self.level_scene
        return True                    # includes every sticky pair: a sticky
                                       # overlay outlives the scene switch, so
                                       # only rule 1 constrains it


# SITES A HAND SEAT ALREADY OWNS.
#
# port_ov089_keymodels_fixup() in hal/actor_overlays.cpp closes ov089's six
# LoadKeyModels cross pointers by hand, checking each word still holds its ROM
# address before rewriting it. Four of the six point into ov002, which went
# from contested (whole overlay dropped) to resolvable under the per-target
# test in cross_mode -- so the cross pass would now write the host address
# first and the hand seat's ROM-address check would then abort on its own
# success. Two of the six point into ov085 and stay contested (ov084 and ov085
# both cover the target and both can be live alongside ov089), so the seat
# still owns those. Leave all of ov089's key-model words to that seat: it runs
# after this patch and lands the identical host address. Keyed by (site symbol,
# byte offset).
#
# Module scope, not cross_mode's, because tools/ovsweep.py checks the RESULT of
# this pass and has to know which words were left raw on purpose.
HAND_SEATED = {
    ("data_ov089_02132894", 20), ("data_ov089_02132894", 24),
    ("data_ov089_02132894", 28), ("data_ov089_021328b4", 8),
    ("data_ov089_021328b4", 12), ("data_ov089_021328b4", 16),
}


def write_map(out_path, ov, mode, window, provides, wants):
    """The sidecar the --cross pass reads. See the module docstring.

    window:   the overlay's whole DS footprint, image + bss. The cross pass
              needs this and not just the provided ranges -- see cross_mode.
    provides: (name, ds_addr, size) for host storage this mount OWNS.
    wants:    (name, byte offset, ds target) for a pointer word this mount
              emitted and could NOT rebase, because the target is not its own.
    """
    doc = {"overlay": ov, "mode": mode, "window": list(window),
           "provides": [[a, sz, n] for n, a, sz in provides],
           "wants": [[n, off, t] for n, off, t in wants]}
    path = out_path.parent / (out_path.name + ".map")
    path.write_text(json.dumps(doc, indent=1), encoding="ascii")
    return path


def whole_mode(root, ov, ovid, base, data, out_path):
    """Emit the overlay image as ONE array + a reloc-driven internal rebase.

    A DS overlay is linked at a fixed base and loaded there unrelocated, so
    the ROM image already holds absolute addresses. relocs.txt is a DELINKING
    artefact: it records which words are addresses and where they point. That
    is exactly the information a host mount needs -- every word the table says
    targets inside this overlay gets rewritten to the host array, and nothing
    else is touched. No value-range guessing: a data word that merely happens
    to look like an overlay address stays the number it is.

    bss lives past the file image (the loader zeroes it), so the host array is
    code_size + bss_size and pointers into bss land inside it too.
    """
    bss = overlay_bss(root, ovid, len(data))
    total = len(data) + bss
    win = (base, base + total)
    relocs = load_relocs(root, ov)

    tag = f"port_{ov}"
    patches = []
    wants = []
    desync = 0
    for site in sorted(relocs):
        target = relocs[site]
        off = site - base
        if not (0 <= off < len(data) - 3):
            continue                      # site itself is not file-backed
        word = int.from_bytes(data[off:off + 4], "little")
        if not (win[0] <= target < win[1]):
            # Points at arm9 or ANOTHER OVERLAY. If that overlay is mounted
            # too, the --cross pass rebases it; if it is not, the word keeps
            # its DS address exactly as before. Unlike the internal case this
            # does not exit on a mismatch: a word outside the window is not
            # this overlay's to vouch for, so a desync there is counted and
            # skipped rather than allowed to fail the build.
            if word == target:
                wants.append((f"{tag}_image", off, target))
            else:
                desync += 1
            continue
        if word != target:
            sys.exit(f"{ov}: reloc site {site:#010x} holds {word:#010x} but "
                     f"relocs.txt says {target:#010x} -- image/config desync")
        patches.append((off, target - base))

    # Report (do not act on) aligned words that LOOK like internal pointers
    # but carry no reloc. A real one would mean the delink table is short and
    # some host pointer walk will step off the array; guessing instead would
    # corrupt ordinary data that happens to sit in the window.
    known = {off for off, _ in patches}
    unlisted = 0
    for off in range(0, len(data) - 3, 4):
        if off in known:
            continue
        v = int.from_bytes(data[off:off + 4], "little")
        if win[0] <= v < win[1]:
            unlisted += 1

    # Under --rom-clean the image bytes zero here and load at boot from
    # romdata.bin (port_<tag>_apply below); bss stays zero either way, so the
    # blob part is just the file image. Baked output is byte-identical to before.
    lines = [
        f"/* GENERATED by port/tools/ovdata.py --whole from {ov}.bin"
        " -- local only. */"]
    if link_only.LINK_ONLY:
        lines.append(link_only.banner(f"{ov}.bin"))
    if romblob_common.ROM_CLEAN:
        lines.append("#include <string.h>")
    lines += [
        "typedef unsigned char u8;",
        "",
        # The whole-overlay image is mutable game state (SharedFilePtr caches,
        # actor tables) that a save state has to roll back, so route it into the
        # .dsstate section the snapshot captures. The section declaration must
        # come first: without it a bss-flavoured contribution gets different
        # characteristics from the sentinels' section and the linker silently
        # splits .dsstate in two, leaving these bytes outside the captured span
        # (LNK4078). See hal/dsstate_seg.h.
        '#pragma section(".dsstate$mmm", read, write)',
        '#pragma data_seg(".dsstate$mmm")',
        '#pragma bss_seg(".dsstate$mmm")',
        f"/* {len(data):#x} bytes of image + {bss:#x} of bss, one object so the"
        " game's",
        " * table walks stay inside it however far past a symbol they step. */",
        f"__declspec(align(8)) u8 {tag}_image[{total}] = "
        f"{{ {romblob_common.init_body(data)} }};",
        "#pragma data_seg()",
        "#pragma bss_seg()",
        f"const unsigned {tag}_ds_base = {base:#010x}u;",
        f"const unsigned {tag}_ds_end = {base + total:#010x}u;",
        "",
        "/* (word offset, target offset) for every reloc that points inside"
        " this overlay */",
        f"static const unsigned {tag}_rebase[][2] = {{",
    ]
    for off, tgt in patches:
        lines.append("    { %du, %du }," % (off, tgt))
    lines += [
        "};",
        "",
        f"void {tag}_patch(void)",
        "{",
        "    unsigned i;",
        f"    for (i = 0; i < sizeof {tag}_rebase / sizeof {tag}_rebase[0];"
        " ++i)",
        f"        *(unsigned *)({tag}_image + {tag}_rebase[i][0]) =",
        f"            (unsigned)(unsigned long){tag}_image + {tag}_rebase[i][1];",
        "}",
        "",
        "/* DS address -> host address inside the mounted image. Returns 0 for"
        " an",
        " * address outside the overlay so a caller can fail loudly rather"
        " than",
        " * hand the game a wild pointer. */",
        f"void *{tag}_at(unsigned ds)",
        "{",
        f"    if (ds < {tag}_ds_base || ds >= {tag}_ds_end) return 0;",
        f"    return {tag}_image + (ds - {tag}_ds_base);",
        "}",
    ]
    # The apply/part tag is the bare overlay name (port_ov009_apply), NOT the
    # port_-prefixed image tag, so it matches the ov<NNN> convention the
    # per-symbol mounts use and does not double the prefix. Baked build: no apply.
    apply = romblob_common.emit_apply(ov, [(f"{tag}_image", data)])
    if apply:
        lines += [
            "",
            "/* ROM-CLEAN: fill the image from romdata.bin (already read +"
            " verified).",
            f" * bss stays zero. Runs before {tag}_patch(), which then rebases"
            " in place. */",
        ]
        lines.extend(apply)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines) + "\n", encoding="ascii")
    romblob_common.write_part(out_path, ov, [(f"{tag}_image", data)])

    # A whole-image mount PROVIDES NOTHING to the cross pass: the image spans
    # .text, and resolving a cross function pointer onto DS code bytes would be
    # a worse answer than leaving it raw. It still declares its WINDOW, which
    # is what keeps the level overlays honest -- see cross_mode.
    write_map(out_path, ov, "whole", win, [], wants)
    print(f"{ov} whole image {total:#x} bytes, {len(patches)} internal "
          f"pointers rebased ({unlisted} unlisted candidates, {len(wants)} "
          f"cross-mount candidates, {desync} skipped desync) -> {out_path}")


def cross_mode(root, out_path, map_paths):
    """Rebase the pointers that leave their own mount.

    Reads every mount's sidecar and emits ONE patch function for the words no
    single invocation could place. The writes are into host arrays and the
    values are host addresses, so nothing here depends on another patch having
    run first, and the sites are disjoint from every per-mount pass by
    construction: a target is either inside its own mount or it is not.
    """
    mounts = [json.loads(p.read_text()) for p in map_paths]

    # AN AMBIGUOUS DS RANGE IS DROPPED, NOT GUESSED AT.
    #
    # Every level overlay is linked at the SAME base, because the DS only ever
    # has one loaded: ov009 and ov014 both run 0x021111a0..0x021149e0. So a
    # pointer into that range names whichever level is currently loaded, and
    # relocs.txt says so in as many words -- ov102's word at 0x0214e190 is
    # `to:0x02113ccc module:overlays(9,14,15,16,20,21,22,26,27,29,36)`, eleven
    # candidates. Binding it to the ov009 copy would be right on the castle
    # grounds and a walk over another level's bytes everywhere else, which is
    # the class of bug this pass exists to remove. Resolving it correctly needs
    # a seat that re-patches per loaded level; until there is one, raw is the
    # honest answer and the sweep keeps it visible.
    #
    # The test is on WINDOWS, not on the provided ranges. ov014 is mounted
    # whole and provides nothing, so comparing provided ranges alone would find
    # ov009's per-symbol mount unopposed and quietly bind the pointer.
    #
    # THE TEST IS PER-TARGET, not per-overlay. Two overlays that share a base
    # overlap only over the SHORTER one's span; addresses past that overlap are
    # unambiguous. ov002 (the in-level engine, 0x020ad660..0x021111a0) shares
    # its base with ov003 (0x020ad660..0x020b18a0), so a per-overlay drop would
    # strand every ov002 provider -- including the Koopa fileptr at
    # data_ov002_0210da18 that ov062 points at, 0x5c000 past ov003's end.
    #
    # AND THE TEST IS PER-RESIDENCY, not per-address. Two windows covering one
    # address are two candidate answers only if both overlays can be LOADED
    # while the pointer's owner is. They usually cannot: ov002 and ov006 both
    # cover 0x0210da20 and the DS never has both, so a level overlay's pointer
    # there means ov002's copy and nothing else. Without this the tool is a
    # trap for the next mount rather than a check -- an ov006 window would have
    # taken 194 of this build's resolved pointers back to raw DS addresses, and
    # an ov007 window three, none of them ambiguous in the machine. See
    # Residency above for the model and for which of its rules are derived.
    #
    # Overlays that are NOT mounted are not part of the test. ov000 shares
    # ov001's base and dsd cannot separate them (`module:overlays(0,1)` on
    # every icon target), but only ov001 is hosted, so an address in that range
    # has exactly one host answer. config names all five of the disputed
    # symbols in ov001 and both names are DATA at the same address.
    windows = {m["overlay"]: tuple(m["window"]) for m in mounts}

    # A sidecar's window and the config's footprint are the same arithmetic
    # from the same files, so they have to agree. If they do not, one of the
    # two is stale and the residency model below would be reasoning about a
    # different overlay than the mount emitted.
    foot = {}
    for ov, win in sorted(windows.items()):
        ovid = int(ov[2:])
        got = overlay_footprint(root, ovid)
        if got != win:
            sys.exit(f"{ov}: sidecar window {win[0]:#010x}..{win[1]:#010x} but "
                     f"the config says {got[0]:#010x}..{got[1]:#010x} -- one "
                     f"of them is stale, rebuild the mount.")
        foot[ovid] = got
    res = Residency(root, foot)
    print(res.describe())

    def window_count(src, v):
        """Mounted windows holding v that could be live alongside src."""
        return sum(1 for ov, (s, e) in windows.items()
                   if s <= v < e and res.coresident(src, int(ov[2:])))

    # A CLAIM OUTSIDE ITS OWN MOUNT'S WINDOW IS NOT THAT OVERLAY'S STORAGE.
    #
    # The per-symbol pass hunts un-symbolized statics out to base + image +
    # 0x1000, which can run past the overlay's real footprint: ov022 ends at
    # 0x021146a0 and carries a port_ov022_gap_02114874 whose bytes are read
    # past the end of the image and come out ZERO. That block is not ov022's
    # memory -- 0x02114874 belongs to whatever object overlay stacks above it
    # -- and binding anyone's pointer to it swaps an honest raw DS address for
    # 24 bytes of host zeros with live host data behind them. The gap generator
    # over-reaching is a separate defect and is not fixed here.
    #
    # HOW THIS RELATES TO THE ZERO-WINDOW TEST BELOW, precisely, because the
    # first version of this comment implied the wrong thing. They are two
    # INDEPENDENT guards and either one alone stops the ov022 case: 0x02114874
    # is in no window ov022 can be co-resident with, so the zero-window test
    # drops it, and the claim is outside ov022's footprint, so this filter
    # never offers it. Removing BOTH is what puts the junk row back -- measured
    # as +1 against the pre-change baseline. This filter is still not
    # redundant: a stray claim landing INSIDE a co-resident window passes the
    # zero-window test, and only its own overlay's footprint says it is not
    # real storage.
    claims_by_ov = {}
    outside = 0
    for m in mounts:
        s, e = windows[m["overlay"]]
        for a, sz, n in m["provides"]:
            if not (s <= a < e):
                outside += 1
                continue
            claims_by_ov.setdefault(m["overlay"], []).append((a, a + sz, n))

    _covering_cache = {}

    def covering_for(src):
        """Providers a mount of `src` could legitimately be pointing at."""
        key = tuple(sorted(ov for ov in claims_by_ov
                           if res.coresident(src, int(ov[2:]))))
        if key not in _covering_cache:
            _covering_cache[key] = make_covering(
                [c for ov in key for c in claims_by_ov[ov]])
        return _covering_cache[key]

    lines = ["/* GENERATED by port/tools/ovdata.py --cross -- local only. */",
             "typedef unsigned char u8;", ""]
    decls = []
    body = []
    seen = set()
    unresolved = 0
    contested_hits = 0
    unowned = 0
    for m in mounts:
        hit_here = 0
        src = int(m["overlay"][2:])
        covering = covering_for(src)
        for name, off, target in m["wants"]:
            # A word a hand seat re-patches with the same host address after
            # this pass runs; writing it here would trip that seat's check.
            if (name, off) in HAND_SEATED:
                continue
            n_windows = window_count(src, target)
            # Two or more co-resident windows is the shared-level-base case:
            # which host copy it names depends on the loaded level, so leave
            # it raw and count it.
            if n_windows > 1:
                unresolved += 1
                contested_hits += 1
                continue
            # ZERO co-resident windows means no mount that could be live
            # alongside this one owns the address -- arm9, an unmounted
            # overlay, or an overlay this one can never see. There is no host
            # answer, and a provider claim reaching there anyway is the
            # out-of-window case above.
            if n_windows == 0:
                unresolved += 1
                unowned += 1
                continue
            hit = covering(target)
            if hit is None:
                unresolved += 1
                continue
            for sym in (name, hit[2]):
                if sym not in seen:
                    seen.add(sym)
                    decls.append(f"extern u8 {sym}[];")
            body.append("    *(unsigned int *)(%s + %d) = "
                        "(unsigned int)(%s + %d);"
                        % (name, off, hit[2], target - hit[0]))
            hit_here += 1
        if hit_here:
            print(f"  cross: {m['overlay']} {hit_here} pointers rebased onto "
                  f"another mount")
    lines.extend(sorted(decls))
    lines.append("")
    lines.append("void port_cross_patch(void)")
    lines.append("{")
    lines.extend(body)
    lines.append("}")
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines) + "\n", encoding="ascii")
    if outside:
        print(f"cross: {outside} provider claims sit outside their own "
              f"overlay's footprint and were not offered to anyone")
    print(f"cross: {len(body)} pointers rebased across {len(mounts)} mounts, "
          f"{unresolved} left raw ({contested_hits} into a window contested by "
          f"a co-resident mount, {unowned} into a range no co-resident mount "
          f"owns, the rest arm9 or an unmounted overlay) -> {out_path}")


def main():
    root = pathlib.Path(__file__).resolve().parents[2]
    # --rom-clean is a global mode switch (see romblob_common). Strip it out of
    # the positional argv here so the per-mode parsing below is unaffected.
    romblob_common.set_rom_clean(sys.argv)
    # LINK-ONLY is the same shape of global mode switch, and is stripped here
    # for the same reason. Both gates are checked inside set_link_only().
    link_only.set_link_only(sys.argv)
    argv = link_only.strip([a for a in sys.argv if a != "--rom-clean"])
    if argv[1] == "--cross":
        cross_mode(root, pathlib.Path(argv[2]),
                   [pathlib.Path(p) for p in argv[3:]])
        return
    ov = argv[1]
    out_path = pathlib.Path(argv[2])
    whole = argv[3] == "--whole"
    pack = "--pack" in argv
    if whole:
        wanted = []
    elif argv[3] == "--from-list":
        text = "\n".join(
            ln for ln in pathlib.Path(argv[4]).read_text().splitlines()
            if not ln.lstrip().startswith("#"))
        wanted = list(dict.fromkeys(w for w in text.split() if w != "--pack"))
    else:
        wanted = [a for a in argv[3:] if a != "--pack"]

    ovid = int(ov[2:])
    base = overlay_base(root, ovid)

    # Read the RAW ndspy overlay (tools/unpack.py output), not the dsd
    # export: the dsd image goes stale against config re-addressings
    # (found the hard way: a +0x12c drift left every non-reloc'd word --
    # jump velocities, anim IDs -- reading a stranger's bytes, while the
    # reloc-patched pointers hid the skew). The raw image is the ROM; the
    # yaml base still holds. Guard: a known arm_call from relocs.txt must
    # decode as BL, or the extraction itself is suspect.
    if link_only.LINK_ONLY:
        # No cartridge: stand an image of exactly the config's length in for
        # the dump, holding the real address in every reloc site and zero
        # everywhere else. Everything below -- the BL probe, the size deltas,
        # the reloc rebase, the cross-mount offer -- then runs on it unchanged.
        data = link_only.synth_image(root, f"arm9/overlays/{ov}", base=base)
        link_only.stamp(root)
    else:
        img = root / f"extracted/overlays/overlay_{ovid:04d}.bin"
        if not img.exists():
            img = root / f"extracted/dsd/arm9_overlays/{ov}.bin"
        data = img.read_bytes()
    rl = (root / f"config/arm9/overlays/{ov}/relocs.txt").read_text()
    mc = re.search(r"from:0x([0-9a-f]{8}) kind:arm_call", rl)
    if mc:
        probe = int(mc.group(1), 16) - base
        word = int.from_bytes(data[probe:probe + 4], "little")
        assert (word >> 24) == 0xEB, (
            f"{img}: first arm_call site 0x{mc.group(1)} is not a BL "
            f"(0x{word:08x}) -- image/config desync, re-extract")

    if whole:
        whole_mode(root, ov, ovid, base, data, out_path)
        return

    # every symbol in the overlay, sorted by address, for size deltas
    syms = []
    for line in (root / f"config/arm9/overlays/{ov}/symbols.txt").read_text().splitlines():
        mm = re.search(r"^(\S+)\s+kind:\S+\s+addr:0x([0-9a-fA-F]+)", line)
        if mm:
            syms.append((int(mm.group(2), 16), mm.group(1)))
    syms.sort()
    addr_of = {n: a for a, n in syms}

    # THE LAST SYMBOL OF A SECTION HAS NO NEXT SYMBOL, and four bytes is not a
    # safe guess for it.
    #
    # Sizes below come from next-symbol deltas, which is right everywhere
    # except at the end of the list: dsd names a symbol wherever code
    # referenced one, and the final one in an overlay's table owns everything
    # from its address to the end of its SECTION. ov018's
    # data_ov018_02113c98 is the whole class -- last line of its symbols.txt,
    # so the delta fell through to a+4 and the host gave a 40-byte ROM global
    # (0x02113c98 to the .bss end at 0x02113cc0) four bytes of storage.
    #
    # That is the ICE_SLIDE_MANAGER trample, and it is why it was
    # LAYOUT-DEPENDENT. MSVC merges grouped sections by the text after the
    # `$`, so .dsstate$pk018_0041 lands immediately before .dsstate$pk019_0000
    # -- and pk019_0000 is IceSlideManager_SpawnInfo. Writing 40 bytes into a
    # 4-byte host object put bytes 8 and 9 straight onto SpawnInfo+4, the id
    # halfword the registry cross-checks, which is why the symptom was
    # "SpawnInfo says id 21932, registry says 356" before registration ever
    # ran. In a build where some other pack sorts between the two the same
    # overrun lands on an unwatched neighbour and says nothing at all.
    #
    # So the fallback is the containing section's end, from delinks.txt, and
    # every size is clamped to it: a symbol may run to the end of its own
    # section and never past it.
    sections = delinks_sections(root, ov)

    def section_end(a):
        """End of the delinks section containing `a`, or None if unlisted."""
        for s, e in sections:
            if s <= a < e:
                return e
        return None

    # The DS applies the overlay's relocation table at load: the static image
    # holds pre-reloc junk where a pointer will live, and relocs.txt records
    # word-address -> target. Apply those before emitting, or every baked
    # pointer is garbage (build-time leftovers, ASCII scraps).
    relocs = load_relocs(root, ov)

    def reloc_blob(a, blob):
        b = bytearray(blob)
        for off in range(0, len(b) - 3, 4):
            t = relocs.get(a + off)
            if t is not None:
                b[off:off + 4] = t.to_bytes(4, "little")
        return bytes(b)

    # Under --rom-clean every ROM-content array below is emitted ZEROED and
    # filled at boot from romdata.bin by port_<ov>[_syms]_apply (built at the end
    # from `emitted`). The reloc-applied bytes each array holds become the blob
    # part; the pointer-rebase pass (port_<ov>[_syms]_patch) then runs over the
    # loaded bytes exactly as before -- see out/rom-clean/DESIGN.md. Baked output
    # is byte-identical to the pre-ROM-CLEAN emitter.
    lines = [f"/* GENERATED by port/tools/ovdata.py from {ov}.bin"
             " -- local only. */"]
    if link_only.LINK_ONLY:
        lines.append(link_only.banner(f"{ov}.bin"))
    if romblob_common.ROM_CLEAN:
        lines.append("#include <string.h>")
    lines += ["typedef unsigned char u8;", ""]
    # Overlay data is mutable game state (SharedFilePtr caches, actor and anim
    # tables) a save state must roll back. The plain per-symbol arrays below
    # come out in the default rw data unless routed, so set the .dsstate default
    # segment before them and clear it after. Packed mounts (--pack) place each
    # symbol in its own .dsstate$pkNNN slot explicitly, so they do not lean on
    # this default. The section declaration comes first so a bss-flavoured array
    # here carries the same characteristics as the sentinels' section; without it
    # the linker splits .dsstate in two and these bytes fall outside the captured
    # span (LNK4078). See hal/dsstate_seg.h.
    if not pack:
        lines.append('#pragma section(".dsstate$mmm", read, write)')
        lines.append('#pragma data_seg(".dsstate$mmm")')
        lines.append('#pragma bss_seg(".dsstate$mmm")')
    # --pack: THE SYMBOLS KEEP THEIR ROM SPACING.
    #
    # dsd names a symbol wherever code happened to reference one, so a single
    # ROM object comes back as a run of neighbours -- ov009's water spawn
    # points are three seven-element Vector3 arrays split into twenty-one
    # symbols, and the loader walks each one by index. Separate host arrays
    # put element 1 wherever the linker felt like it.
    #
    # Grouped sections put the run back: MSVC sorts contributions by the part
    # after the `$`, so emitting every symbol in address order with explicit
    # padding for the gaps reproduces the ROM's own relative offsets exactly.
    # The generator refuses a layout it cannot express (a symbol whose size
    # would overlap its successor) and the emitted check asserts the result
    # at runtime rather than trusting the linker.
    pack_rows = []
    emitted = []          # (name, ds_addr, size)
    for name in wanted:
        # "name:0xNNN" pins the size -- for tables the game indexes past an
        # interior symbol (next-symbol deltas under-size those; on the DS the
        # memory is contiguous, on host separate arrays would break it).
        override = None
        if ":" in name:
            name, _, ov_sz = name.partition(":")
            override = int(ov_sz, 0)
        if name not in addr_of:
            sys.exit(f"{name} not in {ov} symbols")
        # A ROM NAME IS NOT ALWAYS A C NAME. dsd names each .ctor word after
        # the sinit it points at with a `.p` prefix -- `.p__sinit_ov004_
        # 020b948c` -- and there are 308 of those across the config, one per
        # .ctor word in every overlay that has constructors. Emitted verbatim
        # the array declaration is not C and the compile fails several minutes
        # later with a parse error nobody will trace back to a symbol list.
        # This is not a silent failure, but it is a confusing one, and the
        # rule "mount every data-kind symbol dsd names" reaches it the first
        # time an overlay with a non-empty .ctor is mounted whole (ov004 and
        # ov006 are the first two). Refuse here, where the list is.
        if not re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", name):
            sys.exit(f"{ov}: {name!r} is not a valid C identifier, so it "
                     f"cannot be emitted as an array name. dsd gives every "
                     f".ctor word a `.p` prefix; those entries hold DS "
                     f"addresses of .init code no data mount hosts, so leave "
                     f"them out of the list and let --pack's padding stand in "
                     f"for the table.")
        a = addr_of[name]
        end = section_end(a)
        # No next symbol means the end of the section owns the rest (see the
        # note where `sections` is built); a next symbol past the section end
        # belongs to the section after this one and must not stretch this
        # symbol across the boundary.
        nxt = next((s for s, _ in syms if s > a), end if end else a + 4)
        if end:
            nxt = min(nxt, end)
        size = override if override else max(4, nxt - a)
        off = a - base
        blob = data[off:off + size]
        if len(blob) < size:
            blob = blob + b"\0" * (size - len(blob))   # tail runs into bss
        blob = reloc_blob(a, blob)
        if pack:
            pack_rows.append((a, name, size))
        else:
            lines.append(f"u8 {name}[{size}] = "
                         f"{{ {romblob_common.init_body(blob)} }};")
        emitted.append((name, a, size, blob))

    if pack:
        tag = f"pk{ovid:03d}"
        # A packed mount keeps each symbol in its own section slot so the run
        # reproduces the ROM's spacing. Route those slots INTO the .dsstate
        # section the save state captures: one base section .dsstate, grouped
        # (and ordered) by a per-mount, per-slot suffix after the single `$`.
        # MSVC merges grouped sections by the text after the first `$`, so
        # pkNNN_SSSS sorts this mount's slots in order and drops the whole run
        # between the .dsstate$aaa/$zzz sentinels. See hal/dsstate_seg.h.
        def pksect(s):
            return f".dsstate$pk{ovid:03d}_{s:04d}"
        pack_rows.sort()
        slot = 0
        prev_end = None
        checks = []
        first = pack_rows[0][0] if pack_rows else 0
        # blob per name, for the baked-mode initialiser (ROM-clean uses "0").
        blob_of = {n: b for n, _, _, b in emitted}

        # EACH SYMBOL KEEPS ITS OWN ROM ALIGNMENT, not a blanket 4.
        # dsd names symbols wherever code referenced one, and some of those
        # land mid-word -- ov085 has three (a string interior and two
        # sub-fields). Declaring those align(4) makes MSVC pad the run and
        # every symbol after them shifts, which is exactly what the emitted
        # check caught: "data_ov085_0212ffff at +3460, ROM says +3459", and
        # then a 4-byte skew through the rest of the overlay's data.
        def rom_align(addr):
            return 4 if addr % 4 == 0 else (2 if addr % 2 == 0 else 1)

        for a, name, size in pack_rows:
            if prev_end is not None and a < prev_end:
                sys.exit(f"{ov}: --pack: {name} at {a:#010x} overlaps the "
                         f"symbol ending at {prev_end:#010x}")
            if prev_end is not None and a > prev_end:
                gap = a - prev_end
                # A ROM-CLEAN pack gap stays zeroed and needs no load: it exists
                # only to pad the run so the named symbols keep their ROM
                # spacing, and its bytes are never read directly.
                lines.append(
                    f'__pragma(section("{pksect(slot)}", read, write))'
                    f' __declspec(allocate("{pksect(slot)}"))'
                    f' __declspec(align({rom_align(prev_end)}))'
                    f' static u8 {tag}_gap_{prev_end:08x}'
                    f'[{gap}] = {{ 0 }};')
                slot += 1
            lines.append(
                f'__pragma(section("{pksect(slot)}", read, write))'
                f' __declspec(allocate("{pksect(slot)}"))'
                f' __declspec(align({rom_align(a)}))'
                f' u8 {name}[{size}] = '
                f'{{ {romblob_common.init_body(blob_of[name])} }};')
            slot += 1
            prev_end = a + size
            checks.append((name, a - first))
        lines.append("")
        lines.append("#include <stdio.h>")
        lines.append(f"void port_{ov}_pack_check(void)")
        lines.append("{")
        lines.append(f"    const u8 *base = {pack_rows[0][1]};")
        for name, off in checks:
            lines.append(f"    if ({name} - base != {off})")
            lines.append(
                '        fprintf(stderr, "  [%s] PACK BROKEN: %s at +%%d, '
                'ROM says +%d\\n", (int)(%s - base));' % (ov, name, off, name))
        lines.append("}")

    # Un-symbolized statics: mwcc pools some SharedFilePtr objects and small
    # tables in the overlay tail (0x0210xxxx) where dsd attributed the range
    # to the giant sinit functions. Any pointer in emitted data landing there
    # gets a synthetic gap block (real overlay bytes) so the walks stay on
    # hosted storage. Iterated: gap blocks can reference further gaps.
    #
    # Held back and emitted together below, because where they land depends on
    # whether this is a packed mount and the two branches route data
    # differently. See the block after the loop.
    gap_lines = []
    DATA_WIN = (0x02100000, base + len(data) + 0x1000)
    if pack:
        # A LEVEL overlay's data holds its own CODE addresses -- the Bird's
        # four behaviour states are {function, 0} pairs sitting in .data -- and
        # mounting the text section as a synthetic data block would rebase them
        # onto host bytes that are not the host function. The seat that patches
        # those pairs cross-checks the stored word against the ROM address, so
        # the wrong answer would be caught, but it must not be produced in the
        # first place. Start the window at the end of .text.
        dl = (root / f"config/arm9/overlays/{ov}/delinks.txt").read_text()
        mt = re.search(r"\.text\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)",
                       dl)
        if mt:
            DATA_WIN = (int(mt.group(2), 16), DATA_WIN[1])
    for _ in range(5):
        cov = sorted((a, a + sz) for _, a, sz, _ in emitted)

        def covered(v):
            for s, e in cov:
                if s <= v < e:
                    return True
            return False

        want = set()
        for _, _, _, blob in emitted:
            for off in range(0, len(blob) - 3, 4):
                v = int.from_bytes(blob[off:off + 4], "little")
                if DATA_WIN[0] <= v < DATA_WIN[1] and not covered(v):
                    want.add(v)
        if not want:
            break
        # How far past a discovered target a gap must reach. The fixed +0x18
        # guess truncates any record longer than 24 bytes when it is the LAST
        # target in a run -- interior targets get covered incidentally by the
        # next target's extension, so the shortfall only ever bites the tail.
        # That cost the Bowser puzzle its 14th piece: the per-piece state
        # scripts at ov064 0x0211bfd8 are 28-byte records whose -1 terminator
        # sits at +26, so the last one (0x0211c144) lost its terminator, the
        # script cursor ran off the end of the array, and Manager::Behavior got
        # an out-of-range state index. Size each target by its own symbol
        # extent (symbols.txt next-symbol delta, the rule named mounts already
        # use); keep +0x18 as a floor so nothing that fit before shrinks.
        sym_addrs = [a for a, _ in syms]

        def target_end(t):
            i = bisect.bisect_right(sym_addrs, t)
            nxt = sym_addrs[i] if i < len(sym_addrs) else None
            return max(t + 0x18, nxt) if nxt is not None else t + 0x18

        runs = []
        for t in sorted(want):
            if runs and t - runs[-1][1] <= 0x100:
                runs[-1][1] = max(runs[-1][1], target_end(t))
            else:
                runs.append([t, target_end(t)])
        for s, e in runs:
            name = "port_%s_gap_%08x" % (ov, s)
            blob = data[s - base:e - base]
            if len(blob) < e - s:
                blob = blob + b"\0" * (e - s - len(blob))
            blob = reloc_blob(s, blob)
            # A synthetic gap block DOES carry real ROM bytes (a pointer walk
            # steps through it), so under --rom-clean it zeroes and the apply
            # below fills it like any named symbol.
            gap_lines.append(f"u8 {name}[{e - s}] = "
                             f"{{ {romblob_common.init_body(blob)} }};")
            emitted.append((name, s, e - s, blob))

    # THE SYNTHETIC GAP BLOCKS ARE SAVE STATE, IN BOTH KINDS OF MOUNT.
    #
    # They are not padding. The patch pass below rebases pointers onto them,
    # the cross pass rebases pointers from other mounts onto them, and the
    # game's walks read and write through those pointers -- so they are
    # mutable DS state by the same argument that puts every other mounted byte
    # into .dsstate, and a restore that skips them leaves them stale. That is
    # the 0.2.0 ANIM_PTRS bug, which is the bug the section exists for.
    #
    # A PLAIN mount got this right by accident: it sets .dsstate as the default
    # data segment before its per-symbol arrays and only closes it after these
    # blocks, so they rode along. A PACKED mount never opens that default,
    # because it places each named symbol and each padding run with an explicit
    # __declspec(allocate), and these blocks are ordinary file-scope arrays
    # with no allocate of their own. They landed in the default read/write data
    # and no save state captured them: 28 blocks and 5368 bytes across eleven
    # mounts on the build where this was found. Eight of the 28 are cross-mount
    # pointer targets on that build, so other overlays' data points into them
    # too. port/tools/gapaudit.py counts them and checks the side of the
    # captured span they land on.
    #
    # WHAT WAS ACTUALLY OBSERVED, so nobody reads more into this than was
    # measured: ONE of the 28 was watched through a real save and load and seen
    # to change and then fail to roll back (port_ov009_gap_0211222c+12, the
    # reproducer under SM64DS_SS_WATCH_FLAG in tests/walk_window.cpp). The other
    # 27 are routed on the same argument, not on 27 more observations. Routing
    # them costs 5368 captured bytes and does not depend on which of them turns
    # out to be written; auditing them one at a time would.
    #
    # WHAT THIS DOES TO THE SAVE STATE AND TO THE SELFTEST BMP, for whoever
    # writes the release note. The captured span both GROWS and MOVES: on the
    # build this landed on, 934896 bytes at 0xb28000 became 940280 at 0xb26000,
    # so +5384 (the 5368 plus 16 of section alignment) and a base 8192 lower.
    # The size is what makes an older disk state refuse rather than misread.
    # The BASE is what invalidates a BMP comparison: per the rule in
    # port/tools/battery.py, walk_window_selftest.bmp carries a dependence on
    # the absolute addresses of the hosted globals and is only comparable at an
    # equal .dsstate base, so a byte-compare of the selftest BMP across this
    # change is meaningless and a diff must not be read as a render regression.
    # Neither of that note's exemptions applies here: the base moved, and the
    # change is not interior to .dsstate either, since 5368 bytes left the
    # ordinary read/write data to get here. dsstate_guard is the check with
    # meaning across this commit.
    #
    # Route them the way the plain branch does rather than into the pack
    # family: they have no ROM-relative spacing obligation to the named
    # symbols (nothing reaches them by arithmetic from a name, only through a
    # patched pointer), so keeping them out of .dsstate$pkNNN_SSSS leaves the
    # packed run's slot ordering, and the offsets port_ovNNN_pack_check
    # asserts, exactly as they were. The section declaration comes first for
    # the same reason it does everywhere else: without it a bss-flavoured
    # array here gets different characteristics from the sentinels' section
    # and the linker silently splits .dsstate in two (LNK4078), which would
    # put these bytes back outside the capture by another route.
    if gap_lines:
        if pack:
            lines.append('#pragma section(".dsstate$mmm", read, write)')
            lines.append('#pragma data_seg(".dsstate$mmm")')
            lines.append('#pragma bss_seg(".dsstate$mmm")')
        lines.extend(gap_lines)
        if pack:
            lines.append('#pragma data_seg()')
            lines.append('#pragma bss_seg()')

    # Pointer relocation: emitted data holds DS addresses of OTHER emitted
    # symbols (fileptr tables, dtor-chain nodes). Rewrite any word the delink
    # table calls an address to the host address, so the game's pointer walks
    # stay on hosted storage. Code pointers and words pointing at un-hosted
    # data are left as-is (they trap loudly).
    #
    # THE EVIDENCE IS relocs.txt, NOT A VALUE-RANGE SCAN, and it has to be.
    # This pass used to rebase any 4-byte window whose VALUE landed inside the
    # emission, on the reasoning that inside one mount the coverage is near
    # enough to proof. It is not, once a mount carries a large block that dsd
    # named at an address which is not word-aligned: the scan then steps along
    # a grid the ROM never used, so four consecutive bytes of an ordinary byte
    # table can read as an in-mount address by coincidence, and the patch
    # writes a host pointer over four table entries.
    #
    # ov007 is the mount that found it. It has sixteen non-4-aligned symbols
    # including blocks of 3830, 3570 and 1828 bytes, and it produced exactly
    # one such coincidence: data_ov007_020ef022 + 1488 is DS 0x020ef5f2, two
    # bytes off the word grid, where the byte run 08 0e 0f 02 reads as
    # 0x020f0e08 and lands inside data_ov007_020f020e.
    #
    # The gate costs nothing that was ever real: across the 2753 patches the
    # 45 per-symbol mounts in the current build emit, every single site is both
    # 4-aligned and listed in relocs.txt, so none of them changes. It is the
    # same standard of evidence the cross pass below already holds itself to,
    # and the reason given there -- a constant can fall in the window -- turns
    # out to apply within one mount too.
    covering = make_covering((a, a + sz, n) for n, a, sz, _ in emitted)

    patches = []
    for name, a, size, blob in emitted:
        for off in range(0, size - 3, 4):
            if relocs.get(a + off) is None:
                continue
            v = int.from_bytes(blob[off:off + 4], "little")
            hit = covering(v)
            if hit is not None:
                patches.append("    *(unsigned int *)(%s + %d) = "
                               "(unsigned int)(%s + %d);"
                               % (name, off, hit[2], v - hit[0]))
    # Close the .dsstate default segment opened for the plain mount so the
    # apply/patch code and anything after it stays in the ordinary segments.
    # (data_seg does not affect functions, but the synthetic gap arrays above
    # are data and had to land in .dsstate too, which they did while it was
    # open.) A packed mount never opened this one; it opens and closes its own
    # around the gap blocks, which is the only data it emits without an
    # explicit allocate.
    if not pack:
        lines.append('#pragma data_seg()')
        lines.append('#pragma bss_seg()')
    lines.append("")
    lines.append("void port_%s_%spatch(void)\n{"
                 % (ov, "syms_" if pack else ""))
    lines.extend(patches)
    lines.append("}")

    # Under --rom-clean, fill every zeroed array from romdata.bin, in `emitted`
    # order, BEFORE the patch pass above rebases interior pointers. The apply tag
    # is ov<NNN>_syms for a packed mount and ov<NNN> for a plain one, so it never
    # collides with the same overlay's whole-image apply (port_ov<NNN>_apply).
    apply_tag = f"{ov}_syms" if pack else ov
    apply_entries = [(n, blob) for n, _, _, blob in emitted]
    apply = romblob_common.emit_apply(apply_tag, apply_entries)
    if apply:
        lines.append("")
        lines.append("/* ROM-CLEAN: load the ROM bytes, then port_%s_%spatch()"
                     " rebases. */" % (ov, "syms_" if pack else ""))
        lines.extend(apply)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines) + "\n", encoding="ascii")
    romblob_common.write_part(out_path, apply_tag, apply_entries)

    # Everything the pass above could NOT place, for the --cross pass.
    #
    # The candidates come from relocs.txt, not from the value-range scan the
    # internal pass uses. Inside one mount a value landing in the mount's own
    # coverage is near enough to proof, because the coverage IS the emission.
    # Across mounts the window is nine overlays wide and an ordinary constant
    # could fall in it, so the only acceptable evidence is the delink table
    # saying in as many words that this word is an address and where it points.
    # reloc_blob already wrote those targets into the blobs, so the word at a
    # reloc site holds the target by construction.
    #
    # Every emitted copy gets its own entry: a named symbol and the gap block
    # that swallows it are two host arrays holding the same ROM bytes, and a
    # walk through either one has to find hosted pointers.
    wants = []
    for name, a, size, _ in emitted:
        for off in range(0, size - 3, 4):
            t = relocs.get(a + off)
            if t is not None and covering(t) is None:
                wants.append((name, off, t))
    write_map(out_path, ov, "pack" if pack else "syms",
              (base, base + len(data) + overlay_bss(root, ovid, len(data))),
              [(n, a, sz) for n, a, sz, _ in emitted], wants)
    print(f"{len(wanted)} symbols, {len(patches)} pointer patches, "
          f"{len(wants)} cross-mount candidates -> {out_path}")


if __name__ == "__main__":
    main()
