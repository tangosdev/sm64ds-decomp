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


def _overlay_base(symbols):
    """An overlay's base = its lowest symbol address, read from the config's symbols.txt.

    dsd relocates each overlay to a unique address in the delinked config space so that
    (module, addr) is well-defined even though overlays share RAM slots at runtime; that
    base is the overlay's lowest symbol. We derive it from config (the source of truth)
    rather than `extracted/dsd/arm9_overlays/overlays.yaml`, whose `base_address` is the
    raw ROM overlay-TABLE ramAddress -- correct for the ROM image but NOT the config's
    relocated space, so reading it makes `addr - base` land outside the overlay binary.
    (The overlay binaries are identity-indexed: overlay_NNNN.bin == config ovNNN.)"""
    lo = None
    for line in symbols.read_text(errors="ignore").splitlines():
        m = re.search(r"addr:0x([0-9a-fA-F]+)", line)
        if m:
            a = int(m.group(1), 16)
            lo = a if lo is None else min(lo, a)
    return lo


def modules():
    """All modules as dicts: {name, syms, relocs, bin, base}."""
    mods = [{"name": "main", "syms": CFG / "symbols.txt", "relocs": CFG / "relocs.txt",
             "bin": EXTRACTED / "arm9_dec.bin", "base": ARM9_BASE}]
    for name in ("itcm", "dtcm"):
        d = CFG / name
        syms = d / "symbols.txt"
        b = BUILD / f"{name}.bin"
        if not (b.is_file() and syms.is_file()):
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
