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
"""

import bisect
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import romblob_common  # noqa: E402


def overlay_yaml(root, ovid, field):
    """A field out of ONE overlay's record in overlays.yaml.

    The record is found by its own `- id:` line and nothing else. Matching
    "id: NN" anywhere in the file also matches the `file_id: NN` line of a
    DIFFERENT overlay -- ov085's base came back as ov043's that way, and the
    only symptom was the BL guard below firing on a byte that was never code.
    """
    ytxt = (root / "extracted/dsd/arm9_overlays/overlays.yaml").read_text()
    blk = re.search(rf"^  - id: {ovid}$\n(.*?)(?=^  - id: |\Z)", ytxt,
                    re.S | re.M)
    if not blk:
        sys.exit(f"overlays.yaml has no record for overlay {ovid}")
    m = re.search(rf"^    {field}: (\d+)$", blk.group(1), re.M)
    if not m:
        sys.exit(f"overlays.yaml: overlay {ovid} has no {field}")
    return int(m.group(1))


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
    bss = overlay_yaml(root, ovid, "bss_size")
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
    if romblob_common.ROM_CLEAN:
        lines.append("#include <string.h>")
    lines += [
        "typedef unsigned char u8;",
        "",
        "/* 8-byte alignment for the whole-image object. MSVC spells it"
        " __declspec(align),",
        " * GCC/Clang (the Linux port) spell it __attribute__((aligned)); this"
        " macro picks. */",
        "#if defined(_MSC_VER)",
        "#define PORT_ALIGN8 __declspec(align(8))",
        "#else",
        "#define PORT_ALIGN8 __attribute__((aligned(8)))",
        "#endif",
        "",
        f"/* {len(data):#x} bytes of image + {bss:#x} of bss, one object so the"
        " game's",
        " * table walks stay inside it however far past a symbol they step. */",
        f"PORT_ALIGN8 u8 {tag}_image[{total}] = "
        f"{{ {romblob_common.init_body(data)} }};",
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


def cross_mode(out_path, map_paths):
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
    # unambiguous. ov002 (the always-resident shared overlay, 0x020ad660..
    # 0x021111a0) shares its base with ov003 (0x020ad660..0x020b18a0), so a
    # per-overlay drop would strand every ov002 provider -- including the Koopa
    # fileptr at data_ov002_0210da18 that ov062 points at, 0x5c000 past ov003's
    # end. So a target is ambiguous only when it lands inside MORE THAN ONE
    # mount's window; a target covered by exactly one window has one host answer
    # and is resolved against that overlay's provider even where the two windows
    # touch at the base.
    #
    # Overlays that are NOT mounted are not part of the test. ov000 shares
    # ov001's base and dsd cannot separate them (`module:overlays(0,1)` on
    # every icon target), but only ov001 is hosted, so an address in that range
    # has exactly one host answer. config names all five of the disputed
    # symbols in ov001 and both names are DATA at the same address.
    windows = {m["overlay"]: tuple(m["window"]) for m in mounts}

    def window_count(v):
        return sum(1 for (s, e) in windows.values() if s <= v < e)

    # SITES A HAND SEAT ALREADY OWNS.
    #
    # port_ov089_keymodels_fixup() in hal/actor_overlays.cpp closes ov089's six
    # LoadKeyModels cross pointers by hand, checking each word still holds its
    # ROM address before rewriting it. Four of the six point into ov002, which
    # went from contested (whole overlay dropped) to resolvable under the
    # per-target test above -- so the cross pass would now write the host
    # address first and the hand seat's ROM-address check would then abort on
    # its own success. Two of the six point into ov085 and stay contested
    # (ov084/ov085 share a base), so the seat still owns those. Leave all of
    # ov089's key-model words to that seat: it runs after this patch and lands
    # the identical host address. Keyed by (site symbol, byte offset).
    HAND_SEATED = {
        ("data_ov089_02132894", 20), ("data_ov089_02132894", 24),
        ("data_ov089_02132894", 28), ("data_ov089_021328b4", 8),
        ("data_ov089_021328b4", 12), ("data_ov089_021328b4", 16),
    }

    claims = []
    for m in mounts:
        for a, sz, n in m["provides"]:
            claims.append((a, a + sz, n))
    covering = make_covering(claims)

    lines = ["/* GENERATED by port/tools/ovdata.py --cross -- local only. */",
             "typedef unsigned char u8;", ""]
    decls = []
    body = []
    seen = set()
    unresolved = 0
    contested_hits = 0
    for m in mounts:
        hit_here = 0
        for name, off, target in m["wants"]:
            # A word a hand seat re-patches with the same host address after
            # this pass runs; writing it here would trip that seat's check.
            if (name, off) in HAND_SEATED:
                continue
            # A target inside two or more mounted windows is the shared-level-
            # base case: which host copy it names depends on the loaded level,
            # so leave it raw and count it. One window (or none) is unambiguous.
            if window_count(target) > 1:
                unresolved += 1
                contested_hits += 1
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
    print(f"cross: {len(body)} pointers rebased across {len(mounts)} mounts, "
          f"{unresolved} left raw ({contested_hits} into a contested window, "
          f"the rest arm9 or an unmounted overlay) -> {out_path}")


def main():
    root = pathlib.Path(__file__).resolve().parents[2]
    # --rom-clean is a global mode switch (see romblob_common). Strip it out of
    # the positional argv here so the per-mode parsing below is unaffected.
    romblob_common.set_rom_clean(sys.argv)
    argv = [a for a in sys.argv if a != "--rom-clean"]
    if argv[1] == "--cross":
        cross_mode(pathlib.Path(argv[2]),
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
    base = overlay_yaml(root, ovid, "base_address")

    # Read the RAW ndspy overlay (tools/unpack.py output), not the dsd
    # export: the dsd image goes stale against config re-addressings
    # (found the hard way: a +0x12c drift left every non-reloc'd word --
    # jump velocities, anim IDs -- reading a stranger's bytes, while the
    # reloc-patched pointers hid the skew). The raw image is the ROM; the
    # yaml base still holds. Guard: a known arm_call from relocs.txt must
    # decode as BL, or the extraction itself is suspect.
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
    if romblob_common.ROM_CLEAN:
        lines.append("#include <string.h>")
    lines += ["typedef unsigned char u8;", ""]
    # PORT_OVSEC(fullsec, basesec, algn): place a packed symbol so the run keeps
    # its ROM spacing. MSVC uses $-suffix section grouping (__pragma(section) +
    # __declspec(allocate)); GCC/Clang use one base section + declaration order
    # (the build passes -fno-toplevel-reorder), with per-symbol alignment. Same
    # contiguous layout on both; the emitted *_check() verifies the offsets.
    lines += [
        "#if defined(_MSC_VER)",
        "#define PORT_OVSEC(fullsec, basesec, algn) \\",
        "    __pragma(section(fullsec, read, write)) \\",
        "    __declspec(allocate(fullsec)) __declspec(align(algn))",
        "#else",
        "#define PORT_OVSEC(fullsec, basesec, algn) \\",
        "    __attribute__((section(basesec), aligned(algn), used))",
        "#endif",
        "",
    ]
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
        a = addr_of[name]
        nxt = next((s for s, _ in syms if s > a), a + 4)
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
                    f'PORT_OVSEC(".{tag}${slot:04d}", ".{tag}",'
                    f' {rom_align(prev_end)})'
                    f' static u8 {tag}_gap_{prev_end:08x}'
                    f'[{gap}] = {{ 0 }};')
                slot += 1
            lines.append(
                f'PORT_OVSEC(".{tag}${slot:04d}", ".{tag}", {rom_align(a)})'
                f' u8 {name}[{size}] = '
                f'{{ {romblob_common.init_body(blob_of[name])} }};')
            slot += 1
            prev_end = a + size
            checks.append((name, a - first))
        # Tail guard: dsd sizes the LAST packed symbol by its next-symbol delta,
        # so the section ends exactly at that symbol. But a sinit that views a
        # symbol as a struct array can legitimately write a few bytes PAST the
        # last named symbol -- on the DS those bytes are the overlay's own
        # contiguous tail; on the host the next overlay's .pkNNN section starts
        # there and the write stomps it. (ov072's __sinit_02122414 writes
        # data_ov072_02122d6c as S16[6] = 96 bytes though the packed run only
        # reserves 92, clobbering ov094's data_ov094_021369c0.) Reserving 16
        # bytes of zeroed slack keeps any such struct-view spill inside the
        # overlay's own section. It is never read directly (no symbol names it)
        # and it matches the ROM, whose overlay data ran on past the last symbol.
        lines.append(
            f'PORT_OVSEC(".{tag}${slot:04d}", ".{tag}", 4)'
            f' static u8 {tag}_tail_guard[16] = {{ 0 }};')
        slot += 1
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
        runs = []
        for t in sorted(want):
            if runs and t - runs[-1][1] <= 0x100:
                runs[-1][1] = t + 0x18
            else:
                runs.append([t, t + 0x18])
        for s, e in runs:
            name = "port_%s_gap_%08x" % (ov, s)
            blob = data[s - base:e - base]
            if len(blob) < e - s:
                blob = blob + b"\0" * (e - s - len(blob))
            blob = reloc_blob(s, blob)
            # A synthetic gap block DOES carry real ROM bytes (a pointer walk
            # steps through it), so under --rom-clean it zeroes and the apply
            # below fills it like any named symbol.
            lines.append(f"u8 {name}[{e - s}] = "
                         f"{{ {romblob_common.init_body(blob)} }};")
            emitted.append((name, s, e - s, blob))

    # Pointer relocation: emitted data holds DS addresses of OTHER emitted
    # symbols (fileptr tables, dtor-chain nodes). Rewrite any aligned word
    # that lands inside another emitted symbol to the host address, so the
    # game's pointer walks stay on hosted storage. Code pointers and words
    # pointing at un-hosted data are left as-is (they trap loudly).
    covering = make_covering((a, a + sz, n) for n, a, sz, _ in emitted)

    patches = []
    for name, a, size, blob in emitted:
        for off in range(0, size - 3, 4):
            v = int.from_bytes(blob[off:off + 4], "little")
            hit = covering(v)
            if hit is not None:
                patches.append("    *(unsigned int *)(%s + %d) = "
                               "(unsigned int)(%s + %d);"
                               % (name, off, hit[2], v - hit[0]))
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
              (base, base + len(data) + overlay_yaml(root, ovid, "bss_size")),
              [(n, a, sz) for n, a, sz, _ in emitted], wants)
    print(f"{len(wanted)} symbols, {len(patches)} pointer patches, "
          f"{len(wants)} cross-mount candidates -> {out_path}")


if __name__ == "__main__":
    main()
