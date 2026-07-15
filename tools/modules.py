"""Module registry: main ARM9 + every overlay.

Each module has its own binary, RAM base address, symbols, and relocs. Overlays
can share a base address (they occupy the same memory slot at different times),
so anything that records matches must key by (module, addr), never addr alone.
"""
import pathlib
import re

REPO = pathlib.Path(__file__).resolve().parent.parent
EXTRACTED = REPO / "extracted"
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
