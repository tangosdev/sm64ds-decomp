#!/usr/bin/env python3
"""LINK-ONLY: build the port's REFERENCE GRAPH with no cartridge present.

WHAT THIS IS FOR. The port's throughput is bottlenecked on one machine-wide
build lock (port/tools/build_lock.py says why: 5m22s alone, 72-85 minutes when
four lanes race). Every lane's cycle has two halves -- "does it build, does it
link, what is the linkage number" (tools/linkage.py over walk_window.map), and
behaviour (tools/battery.py, the proofs, the opening gate). The second half
needs this box, a window and the real cartridge data. The FIRST half does not
depend on the cartridge's VALUES at all: tools/linkage.py counts which matched
src/ TUs survived /OPT:REF, and /OPT:REF keeps a function when something
REFERENCES it. The reference graph is already in config/, so the first half can
run anywhere -- a hosted CI runner included -- and the lock stops being the
queue everything waits in.

WHAT IT DOES. The ROM emitters (tools/romdata.py, tools/ovdata.py) read the
extracted images for two different things, and only one of them is Nintendo's:

  SHAPE   which symbols exist, where they start, how big they are, which words
          are addresses and where those addresses point. All of that is in
          config/**/symbols.txt, delinks.txt and relocs.txt, which are
          committed, because they are the delinking config and not ROM content.

  VALUES  every other word -- the tuning constants, the tables, the art. That
          is the cartridge, it can never enter a public repo, and NOTHING in
          the reference graph depends on it.

So this synthesises an image of exactly the right length with zeros for the
values and the real address in every reloc site, and hands it to the emitters
UNCHANGED. That is the whole design, and the reason it is that shape rather
than a second code path inside each emitter: this lane touches the measuring
instrument, so the emitters' own logic -- which symbols get emitted, how they
are sized, which pointers are rebased, which cross-mount candidates are offered
-- must not fork on the mode. Only the bytes going in differ.

WHY THE SYNTHESIS IS EXACT WHERE IT MATTERS. Measured on this checkout, over
all 103 overlays: 40,469 kind:load relocs are file-backed, and the image word
at the site equals the reloc's stated target for 40,469 of them -- ZERO
disagreements. So every word the reference graph is built out of comes back
byte-identical, and the emitters' own desync refusals (ovdata's "image/config
desync" exit, whose whole job is to catch a config that has drifted from the
image) pass because they are true, not because they were bypassed.

WHAT IT CANNOT REPRODUCE, stated because a mode that quietly differs is worse
than one that fails. ovdata's per-symbol mounts also scan for a RAW word that
merely looks like a data pointer and give it a synthetic gap block. A raw word
is a value, so a synthesised image has a zero there and the block is not
emitted. Measured across every packed mount in port/ov*_syms.txt: 4,955 such
candidates come from reloc sites (reproduced exactly) and ONE comes from a raw
value (ov007). That one block is a `static u8` array in a generated host file,
referenced only by that file's own pointer-patch pass, so it names no src/ TU
and cannot move the linkage number in either direction. The build-to-build
comparison in out/CILINK is the evidence, not this paragraph.

THE EXE IS DEAD, ON PURPOSE. Every table it needs is zero, so it must never be
mistaken for a real build:

  * the 13 archive mounts get the path "LINK-ONLY-BUILD-NOT-A-GAME" instead of
    their narc paths, so the fs seam cannot open a single file and the string
    is greppable in the binary,
  * build/port/LINK_ONLY.stamp is written next to the exe,
  * every generated file's header comment says so.

TWO GATES, BOTH REQUIRED. `--link-only` on the emitter's argv AND
SM64DS_LINK_ONLY=1 in the environment. The flag alone REFUSES -- a build that
has not deliberately set the environment variable cannot reach this mode by
picking up a stray argument, and a stray environment variable cannot reach it
without the flag. CMake's PORT_LINK_ONLY option sets both together and defaults
OFF, so every existing build is byte-identical to before.
"""

import os
import pathlib
import re
import struct
import sys

MARKER = "LINK-ONLY-BUILD-NOT-A-GAME"

# Set once per emitter run by set_link_only(). Default OFF: a plain
# `python tools/romdata.py out.c` reads the cartridge exactly as it always did.
LINK_ONLY = False

_RELOC = re.compile(
    r"from:0x([0-9a-fA-F]+)\s+kind:(\S+)\s+to:0x([0-9a-fA-F]+)")


class NoCartridge(Exception):
    """Raised where an emitter check needs a real image and there is none."""


def set_link_only(argv):
    """Read the two gates. Returns True only when BOTH are set.

    The asymmetry is deliberate. `--link-only` without the environment
    variable is a hard refusal, because that is the shape an accident takes:
    an argument leaking into a normal build. The environment variable without
    the flag is silently ignored, because that is the shape a leftover shell
    export takes and it must not turn a real build into a fake one.
    """
    global LINK_ONLY
    flag = "--link-only" in argv
    env = os.environ.get("SM64DS_LINK_ONLY") == "1"
    if flag and not env:
        sys.exit(
            "--link-only was passed but SM64DS_LINK_ONLY=1 is not in the "
            "environment. This mode emits a DEAD executable with zeroed ROM "
            "tables; it exists to measure link coverage on a machine with no "
            "cartridge, and it refuses to run unless the build asked for it "
            "in both places. See port/tools/link_only.py.")
    LINK_ONLY = flag
    return LINK_ONLY


def strip(argv):
    """argv without --link-only, for emitters that parse positionally."""
    return [a for a in argv if a != "--link-only"]


def stamp(root):
    """Drop build/port/LINK_ONLY.stamp beside the exe. Idempotent."""
    p = pathlib.Path(root) / "build" / "port" / "LINK_ONLY.stamp"
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(
        MARKER + "\n"
        "Every ROM table in this build is ZERO. It links and it measures;\n"
        "it does not run. Generated by port/tools/link_only.py.\n",
        encoding="ascii")
    return p


def banner(what):
    """The header comment line every synthesised emission carries."""
    return (f"/* {MARKER}: {what} was SYNTHESISED from config/ with no "
            "cartridge present.\n"
            " * Reloc sites hold their real targets; every other word is "
            "zero. Do not run. */")


def sections(root, module):
    """[(name, start, end)] for the section table at a delinks.txt head.

    Same table tools/ovdata.py's delinks_sections() reads, kept separate here
    because this needs the section NAMES (to find .bss) and that one does not.
    """
    path = pathlib.Path(root) / "config" / module / "delinks.txt"
    out = []
    for line in path.read_text().splitlines():
        m = re.match(
            r"^\s+\.(\w+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)",
            line)
        if m:
            out.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
        elif out and not line.startswith(" "):
            break
    if not out:
        sys.exit(f"{path}: no section table")
    return out


def image_span(root, module):
    """(base, file_image_length) for a module, from delinks.txt alone.

    The base is the lowest section start -- the rule tools/ovdata.py's
    overlay_base() already trusts over the yaml, and which it measured equal to
    the ROM's own overlay-table ram_address for 103 of 103 overlays.

    The FILE IMAGE ends where .bss begins: .bss has no file content, the
    loader zeroes it, and it is always the last section. Checked against the
    real extraction on the tree this was written on: ov009's .bss starts at
    0x02113c20 over base 0x021111a0, which is 10880 bytes, and
    extracted/overlays/overlay_0009.bin is 10880 bytes. arm9's .bss starts at
    0x0209b000 over base 0x02004000, and romdata.py's own BSS_START constant is
    that same address -- it already refuses to emit anything past it, so a
    synthesised arm9 that stops there is as long as anything reads.

    A module with no .bss section (none today) runs to its highest section end.
    """
    secs = sections(root, module)
    base = min(s for _, s, _ in secs)
    bss = [s for n, s, _ in secs if n == "bss"]
    end = bss[0] if bss else max(e for _, _, e in secs)
    return base, end - base


def synth_image(root, module, base=None, length=None):
    """A config-only stand-in for one module's ROM image.

    Zeros everywhere, and at every file-backed reloc site the word the DS would
    hold there:

      kind:load        the target address, which is what the word IS -- a DS
                       overlay is linked at a fixed base and loaded there
                       unrelocated, so the image already holds absolutes and
                       relocs.txt is the delinker's record of which words those
                       are. This is the half the reference graph is built from.
      kind:arm_call    a real ARM BL to the target. Not needed by any emission,
                       but ovdata.py probes the first arm_call site and asserts
                       it decodes as BL before it will trust an extraction.
                       Synthesising a genuine BL keeps that guard doing its job
                       instead of being skipped for this mode.
      kind:arm_call_thumb  the ARM BLX(1) form of the same.

    Sites outside the file image (a reloc into .bss, or into another module)
    are skipped, exactly as the emitters skip them.
    """
    if base is None or length is None:
        b, ln = image_span(root, module)
        base = b if base is None else base
        length = ln if length is None else length
    buf = bytearray(length)
    path = pathlib.Path(root) / "config" / module / "relocs.txt"
    if not path.exists():
        return bytes(buf)
    for line in path.read_text().splitlines():
        m = _RELOC.search(line)
        if not m:
            continue
        site = int(m.group(1), 16)
        kind = m.group(2)
        target = int(m.group(3), 16)
        off = site - base
        if not (0 <= off <= length - 4):
            continue
        if kind == "load":
            word = target
        elif kind in ("arm_call", "arm_call_thumb"):
            disp = (target - site - 8) >> 2
            op = 0xEB000000 if kind == "arm_call" else 0xFA000000
            word = op | (disp & 0x00FFFFFF)
        else:
            continue
        struct.pack_into("<I", buf, off, word)
    return bytes(buf)
