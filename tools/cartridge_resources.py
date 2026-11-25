"""Write the arm9 resource tables from the cartridge image at build time.

The tracked sources are placeholders. A build fills those files in place, compiles
them, and restores the placeholders before the process exits. Nothing this module
writes is meant to be committed.
"""
import atexit
import os
import pathlib
import re
import struct
import threading

import modules as MOD

REPO = pathlib.Path(__file__).resolve().parent.parent
OUT_DIR = REPO / "build" / "cartridge"

# (repo-relative placeholder, section words or bytes, symbols in address order)
# Each symbol is (name, start, end, ctype). ctype "u8" is a byte array; anything
# else is a little-endian word array of that C type.
TABLES = (
    ("src/data/arm9/data_02075f14.c", (
        # ' APS10_1' particle archive; IDENTITY_MATRIX4X3 follows at 0x02082128.
        ("data_02075f14", 0x02075F14, 0x02082128, "const u32"),
    )),
    ("src/data/arm9/data_0208a178.c", (
        ("data_0208a178", 0x0208A178, 0x0208C178, "u8"),
    )),
    ("src/data/arm9/data_0209289c.c", (
        ("data_0209289c", 0x0209289C, 0x02092D3C, "int"),
    )),
    ("src/data/arm9/data_02092d3c.c", (
        ("data_02092d3c", 0x02092D3C, 0x0209325C, "int"),
        ("data_0209325c", 0x0209325C, 0x020937BC, "int"),
        ("data_020937bc", 0x020937BC, 0x02093D7C, "int"),
        ("data_02093d7c", 0x02093D7C, 0x0209435C, "int"),
    )),
)
BY_REL = {rel: symbols for rel, symbols in TABLES}

_SYMBOL = re.compile(r"^(\S+)\s+.*?addr:(0x[0-9a-fA-F]+)")
_RELOC = re.compile(r"^from:(0x[0-9a-fA-F]+)\s+kind:load\s+to:(0x[0-9a-fA-F]+)\b")

_image = None
_names = None
_relocs = None
_lock = threading.Lock()
_originals = None
_owner_pid = None


def _arm9():
    global _image
    if _image is None:
        blob = (MOD.EXTRACTED / "arm9_dec.bin").read_bytes()
        _image = (MOD.ARM9_BASE, blob)
    return _image


def _symbol_names():
    global _names
    if _names is None:
        names = {}
        text = (MOD.CFG / "symbols.txt").read_text(encoding="utf-8", errors="replace")
        for line in text.splitlines():
            match = _SYMBOL.match(line.strip())
            if match:
                names.setdefault(int(match.group(2), 16), match.group(1))
        _names = names
    return _names


def _load_relocs():
    global _relocs
    if _relocs is None:
        relocs = {}
        text = (MOD.CFG / "relocs.txt").read_text(encoding="utf-8", errors="replace")
        for line in text.splitlines():
            match = _RELOC.match(line.strip())
            if match:
                relocs[int(match.group(1), 16)] = int(match.group(2), 16)
        _relocs = relocs
    return _relocs


def _words(start, end):
    base, blob = _arm9()
    off = start - base
    size = end - start
    if off < 0 or off + size > len(blob):
        raise RuntimeError(f"arm9 image does not cover 0x{start:08x}..0x{end:08x}")
    raw = blob[off:off + size]
    if size % 4:
        raise RuntimeError(f"0x{start:08x}..0x{end:08x} is not a whole number of words")
    return list(struct.unpack(f"<{size // 4}I", raw))


def _render_words(name, start, end, ctype):
    relocs = _load_relocs()
    names = _symbol_names()
    externs = []
    pieces = []
    for index, word in enumerate(_words(start, end)):
        addr = start + index * 4
        target = relocs.get(addr)
        if target is None:
            pieces.append(f"0x{word:08x}")
            continue
        target_name = names.get(target)
        if target_name is None:
            pieces.append(f"0x{word:08x}")
            continue
        pieces.append(f"(u32)&{target_name}")
        externs.append(target_name)
    lines = []
    for row in range(0, len(pieces), 8):
        lines.append("    " + ", ".join(pieces[row:row + 8]) + ",")
    count = (end - start) // 4
    body = "\n".join(lines)
    return externs, f"{ctype} {name}[{count}] = {{\n{body}\n}};\n"


def _render_bytes(name, start, end):
    base, blob = _arm9()
    off = start - base
    size = end - start
    if off < 0 or off + size > len(blob):
        raise RuntimeError(f"arm9 image does not cover 0x{start:08x}..0x{end:08x}")
    raw = blob[off:off + size]
    pieces = [f"0x{byte:02x}" for byte in raw]
    lines = []
    for row in range(0, len(pieces), 16):
        lines.append("    " + ", ".join(pieces[row:row + 16]) + ",")
    body = "\n".join(lines)
    return f"u8 {name}[0x{size:x}] = {{\n{body}\n}};\n"


def render(rel):
    """C text for one placeholder, or None when rel is not one of these tables."""
    symbols = BY_REL.get(rel.replace("\\", "/"))
    if symbols is None:
        return None
    externs = []
    bodies = []
    for name, start, end, ctype in symbols:
        if ctype == "u8":
            bodies.append(_render_bytes(name, start, end))
            continue
        found, text = _render_words(name, start, end, ctype)
        externs.extend(found)
        bodies.append(text)
    header = [
        "/* Filled from the cartridge image by tools/cartridge_resources.py. */",
        "#include \"types.h\"",
        "",
    ]
    for name in dict.fromkeys(externs):
        header.append(f"extern const u32 {name}[];")
    if externs:
        header.append("")
    return "\n".join(header) + "\n" + "\n".join(bodies)


def _filled(path):
    try:
        with path.open(encoding="utf-8", errors="replace") as handle:
            return "Filled from the cartridge image" in handle.read(240)
    except OSError:
        return False


def install():
    """Fill the tracked placeholders from the cartridge image.

    A second call, and a process that finds the files already filled, does
    nothing. No cartridge image means there is nothing to fill; callers that
    are not building a ROM keep going. The process that wrote the files
    restores the placeholders at exit. A forked child inherits that
    registration and must not restore early.
    """
    global _originals, _owner_pid
    with _lock:
        if _originals is not None or os.getpid() == _owner_pid:
            return
        if not (MOD.EXTRACTED / "arm9_dec.bin").is_file():
            return
        paths = [REPO / rel for rel, _symbols in TABLES]
        if not all(path.is_file() for path in paths) or all(_filled(path) for path in paths):
            return
        saved = [(path, path.read_bytes()) for path in paths]
        try:
            for rel, path in zip((rel for rel, _symbols in TABLES), paths):
                path.write_text(render(rel), encoding="utf-8", newline="\n")
        except Exception:
            for path, blob in saved:
                path.write_bytes(blob)
            raise
        _originals = saved
        _owner_pid = os.getpid()
        atexit.register(restore)


def restore():
    """Put the placeholders back. A forked child leaves that to the parent."""
    global _originals, _owner_pid
    with _lock:
        if _originals is None or os.getpid() != _owner_pid:
            return
        saved = _originals
        _originals = None
        _owner_pid = None
    for path, blob in saved:
        path.write_bytes(blob)


def source_override(rel):
    """Path of a materialized compile input, or None for an ordinary source.

    The ROM build fills the tracked file instead of compiling this copy. The
    copy remains for inspection.
    """
    text = render(rel)
    if text is None:
        return None
    dest = OUT_DIR / rel
    dest.parent.mkdir(parents=True, exist_ok=True)
    current = dest.read_text(encoding="utf-8") if dest.is_file() else None
    if current != text:
        dest.write_text(text, encoding="utf-8", newline="\n")
    return dest
