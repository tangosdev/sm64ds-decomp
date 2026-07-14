"""addr -> symbol-name index, built from config symbols + the nearmiss DB.

Used to resolve a captured `lr` back to a caller name. Best-effort: returns the
name of the function whose [addr, addr+size) range contains the query, else None.
"""
import json
import pathlib
import re

REPO = pathlib.Path(__file__).resolve().parents[2]

_SYM_RE = re.compile(r"^(\S+)\s+kind:function\([^)]*size=(0x[0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)")


def _iter_symbol_files():
    yield REPO / "config" / "arm9" / "symbols.txt"
    ovdir = REPO / "config" / "arm9" / "overlays"
    if ovdir.is_dir():
        for p in sorted(ovdir.glob("ov*/symbols.txt")):
            yield p


class SymIndex:
    def __init__(self):
        # list of (start, end, name), kept sorted for bisect-free small scans
        self.ranges = []
        self._load()

    def _add(self, addr, size, name):
        if addr is None:
            return
        self.ranges.append((addr, addr + (size or 4), name))

    def _load(self):
        for sf in _iter_symbol_files():
            if not sf.is_file():
                continue
            for line in sf.read_text(encoding="utf-8", errors="replace").splitlines():
                m = _SYM_RE.match(line.strip())
                if m:
                    self._add(int(m.group(3), 16), int(m.group(2), 16), m.group(1))
        db = REPO / "nearmiss" / "db.jsonl"
        if db.is_file():
            for line in db.read_text(encoding="utf-8", errors="replace").splitlines():
                if not line.strip():
                    continue
                try:
                    r = json.loads(line)
                except json.JSONDecodeError:
                    continue
                self._add(int(str(r["addr"]), 16), r.get("size"), r["name"])
        self.ranges.sort()

    def resolve(self, addr):
        if addr is None:
            return None
        # ARM lr carries return address; the call site is lr-4 (or -2 thumb).
        for probe in (addr, addr - 4, addr - 2):
            for start, end, name in self.ranges:
                if start <= probe < end:
                    return name
        return None


_INSTANCE = None


def get():
    global _INSTANCE
    if _INSTANCE is None:
        _INSTANCE = SymIndex()
    return _INSTANCE
