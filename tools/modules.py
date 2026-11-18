"""Module registry: main ARM9, both TCM autoloads, and every overlay.

Each module has its own binary, RAM base address, symbols, and relocs. Overlays
can share a base address (they occupy the same memory slot at different times),
so anything that records matches must key by (module, addr), never addr alone.

The autoloads (itcm, dtcm) were missing here until 2026-08-01, and their absence
was invisible rather than noisy: linkcheck's `_ranges()` never contained them, so
a slot at 0x01ff.... resolved to no module and pr_linkcheck reported NONE -- zero
slots checked, which reads exactly like "clean". The whole of arm9/itcm sat at
0 of 41 matched partly because nothing in the verification stack could see it.
See notes/itcm.md.

Their images come from build/build/, not extracted/, because that is where
config/arm9/config.yaml points dsd at them (`object: ../../build/build/itcm.bin`);
there is no extracted/itcm.bin. Both directories are dsd extract output and both
are gitignored, so neither is more canonical than the other -- but config.yaml is
the source of truth for which file is which module, so follow it. Missing files
are skipped rather than raising, so a checkout with no build/ still gets a usable
registry, just without the autoloads.
"""
import pathlib
import re

REPO = pathlib.Path(__file__).resolve().parent.parent
EXTRACTED = REPO / "extracted"
BUILD = REPO / "build" / "build"
CFG = REPO / "config" / "arm9"
ARM9_BASE = 0x02004000


_SECTION_START = re.compile(r"start:(0x[0-9a-fA-F]+)")


def _overlay_image_base(overlay_dir):
    """The configured start of an overlay's image: its lowest delinks `start:`.

    A data-only overlay loads an empty `.ctor` section ahead of its first
    symbol, up to 0x2c bytes. dsd places that section at the image base; taking
    the lowest symbol instead shifts every read. The overlay's own
    `relocs.txt` is the proof: at `data_ov054_021111c0` the lowest-symbol base
    reads the word `0`, while the delinks base reads `2`, and the
    `from:0x021111c4 kind:load to:0x02111258` row matches the latter.
    """
    delinks = overlay_dir / "delinks.txt"
    if not delinks.is_file():
        return None
    starts = [int(m.group(1), 16)
              for line in delinks.read_text(errors="ignore").splitlines()
              if (m := _SECTION_START.search(line))]
    return min(starts) if starts else None


def _overlay_base(symbols):
    """An overlay's image base, from its config directory.

    Prefer the lowest `delinks.txt` section start, fall back to the lowest
    symbol when the delinks cannot supply it. Both come from config, the source
    of truth; `extracted/dsd/arm9_overlays/overlays.yaml` is not read. The
    overlay binaries are identity-indexed: overlay_NNNN.bin == config ovNNN.
    """
    lo = None
    for line in symbols.read_text(errors="ignore").splitlines():
        m = re.search(r"addr:0x([0-9a-fA-F]+)", line)
        if m:
            a = int(m.group(1), 16)
            lo = a if lo is None else min(lo, a)
    base = _overlay_image_base(symbols.parent)
    if base is not None and (lo is None or base <= lo):
        return base
    return lo


def modules():
    """All modules as dicts: {name, syms, relocs, bin, base}."""
    mods = [{"name": "main", "syms": CFG / "symbols.txt", "relocs": CFG / "relocs.txt",
             "bin": EXTRACTED / "arm9_dec.bin", "base": ARM9_BASE}]
    for name in ("itcm", "dtcm"):
        d = CFG / name
        syms = d / "symbols.txt"
        # config.yaml points dsd at build/build/, but that is a BUILD output: a
        # contributor who has only run tools/unpack.py does not have it, and the
        # autoload then dropped out of the registry silently, so `match.py
        # --module itcm` answered "module itcm not found". dsd also writes the
        # same image to extracted/dsd/arm9/, which unpack.py does produce, so
        # fall back to it. Same bytes, same base (itcm.yaml base_address
        # 33521664 = 0x01ff8000, code_size 24384), just the copy that exists
        # without a build.
        b = next((p for p in (BUILD / f"{name}.bin",
                              EXTRACTED / "dsd" / "arm9" / f"{name}.bin")
                  if p.is_file()), None)
        if not (b and syms.is_file()):
            continue
        base = _overlay_base(syms)      # same derivation: the module's lowest symbol
        if base is None:
            continue
        mods.append({"name": name, "syms": syms, "relocs": d / "relocs.txt",
                     "bin": b, "base": base})
    for d in sorted((CFG / "overlays").glob("ov*")):
        syms = d / "symbols.txt"
        b = EXTRACTED / "overlays" / f"overlay_{int(d.name[2:]):04d}.bin"
        if not (d.is_dir() and b.is_file() and syms.is_file()):
            continue
        base = _overlay_base(syms)
        if base is None:
            continue
        mods.append({"name": d.name, "syms": syms, "relocs": d / "relocs.txt",
                     "bin": b, "base": base})
    return mods


def read_bytes(mod, addr, size):
    """Bytes of [addr, addr+size) from a module's binary."""
    off = addr - mod["base"]
    return mod["bin"].read_bytes()[off:off + size]
