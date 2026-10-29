#!/usr/bin/env python3
"""Prove a readability-only edit changed nothing the link consumes.

WHY THIS EXISTS
---------------
`tools/match.py` answers "does this function still reproduce the ROM?" one
function at a time, and it wildcards relocated words while doing it. Neither
property is what a whole-file readability pass needs. Renaming a type, deleting
a shadow struct or unifying sixteen spellings of one `extern` touches hundreds
of functions at once, and the question is not "does function N still match" but
"did ANY of this change ANY byte, relocation or symbol that the linker reads".

Compiling the TU before and after and comparing the two objects answers that
directly, and catches three things a per-function byte check does not:

1. A relocation that moved to a different symbol. `match.compare` forces
   ``match = True`` at relocated offsets (see tools/reloc_audit.py), so a call
   retargeted to the wrong callee passes there. Here every relocation entry is
   compared with its symbol index resolved to a NAME, so the check survives the
   symbol table being renumbered and still means "points at the same thing".
   mwccarm emits one `.text` and one `.rela.text` per function, all sharing a
   section name, so a relocation group is identified by the symbol its target
   section defines rather than by that name.
2. A function the edit changed that nobody thought to re-check.
3. **A new compiler-numbered local symbol.** mwccarm names compiler-local
   entities from one per-TU counter -- `@1024`, `__arraydtor$954`,
   `sEnterTable$170`, `Frame$2583Player_cpp` -- and those names are pinned in
   `config/**/symbols.txt` and in `config/tu_manifest.d/**/*.json`
   (`defined_symbol_renames`). An edit that adds or removes a declaration can
   renumber every one of them, which silently re-points those config rows at
   the wrong constants. This is not hypothetical: giving a local struct a
   non-POD member added `_ZN6Player20Frame$2583Player_cppD1Ev` to Player.cpp,
   and deleting one unused shim in d_s_mg_hanachan_methods.cpp shifted `@1024`
   to `@1023`. Numbered symbols are reported first and on their own line.

WHAT IS DELIBERATELY IGNORED, AND WHY
-------------------------------------
The byte layout of the string tables. mwccarm writes `.strtab` in declaration
order, so deleting a type reorders the strings without changing a single
instruction. The linker resolves by name, so that reordering is cosmetic -- but
a raw sha256 of the object reports it as a divergence, which is a false
positive on precisely the kind of change this tool is for. The symbol table is
folded into the digest keyed by NAME for the same reason: an index shuffle is
not a change. Section contents, section types and every relocation entry are
compared byte for byte.

A clean result here means the object is identical, which is stronger than a
byte match -- but it is a statement about the compiler's output, not about the
ROM. It says "this edit changed nothing", so it is only as good as the baseline
being correct in the first place. Run it against a tree-green base.

USAGE
-----
    python tools/tu_object_diff.py src/game/player/Player.cpp
    python tools/tu_object_diff.py src/game/player/Player.cpp --base origin/main
    python tools/tu_object_diff.py src/game/player/Player.cpp src/fListNode.cpp --quiet
    python tools/tu_object_diff.py src/game/player/Player.cpp --symbol _ZN6Player12CanEnterDoorEh
    python tools/tu_object_diff.py src/game/player/Player.cpp --numbered

Exit 0 when every file's object is identical, 1 when one differs, 2 on error.
"""
import argparse
import hashlib
import pathlib
import re
import struct
import subprocess
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match

SHT_PROGBITS = 1
SHT_SYMTAB = 2
SHT_STRTAB = 3
SHT_RELA = 4
SHT_NOBITS = 8
SHT_REL = 9

# `@1024`, `__arraydtor$954`, `sEnterTable$170`, `Frame$2583Player_cpp`
NUMBERED = re.compile(r"(?:^@\d+$)|\$\d+")


class ObjectDiffError(Exception):
    """A file could not be read, compiled, or parsed."""


def parse_sections(blob):
    """Return the ELF section headers as a list of dicts, with names resolved."""
    if len(blob) < 0x34 or blob[:4] != b"\x7fELF":
        raise ObjectDiffError("not an ELF object")
    shoff, = struct.unpack_from("<I", blob, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", blob, 0x2E)
    if shnum == 0 or shstrndx >= shnum:
        raise ObjectDiffError("ELF has no usable section table")
    sections = []
    for i in range(shnum):
        off = shoff + i * shentsize
        nameoff, typ, _flags, addr, data_off, size, link, info, _align, _entsize = \
            struct.unpack_from("<IIIIIIIIII", blob, off)
        sections.append(dict(nameoff=nameoff, typ=typ, addr=addr, off=data_off,
                             size=size, link=link, info=info))
    strbase = sections[shstrndx]["off"]
    for sec in sections:
        sec["name"] = read_cstr(blob, strbase, sec["nameoff"])
    return sections


def read_cstr(blob, base, offset):
    """Read a NUL-terminated string at base+offset."""
    start = base + offset
    end = blob.find(b"\0", start)
    if end < 0:
        raise ObjectDiffError("unterminated string in table")
    return blob[start:end].decode("utf-8", "replace")


def symbol_table(blob, sections):
    """Return (by_name, by_index). by_name maps name -> (size, info, section, bytes)."""
    by_name, by_index = {}, []
    for sec in sections:
        if sec["typ"] != SHT_SYMTAB:
            continue
        strbase = sections[sec["link"]]["off"]
        for i in range(sec["size"] // 16):
            off = sec["off"] + i * 16
            nameoff, value, size, info, _other, shndx = \
                struct.unpack_from("<IIIBBH", blob, off)
            name = read_cstr(blob, strbase, nameoff)
            by_index.append(name)
            if not name:
                continue
            home = sections[shndx]["name"] if 0 < shndx < len(sections) else str(shndx)
            body = b""
            if 0 < shndx < len(sections) and sections[shndx]["typ"] == SHT_PROGBITS and size:
                start = sections[shndx]["off"] + value
                body = blob[start:start + size]
            by_name[name] = (size, info, home, body)
    return by_name, by_index


def section_owners(blob, sections):
    """Map section index -> the name of a symbol defined in it, when there is one.

    mwccarm emits one `.text` and one `.rela.text` per function, all sharing a
    section name, so a section's identity is the symbol it carries, not its name.
    """
    owners = {}
    for sec in sections:
        if sec["typ"] != SHT_SYMTAB:
            continue
        strbase = sections[sec["link"]]["off"]
        for i in range(sec["size"] // 16):
            off = sec["off"] + i * 16
            nameoff, value, size, _info, _other, shndx = \
                struct.unpack_from("<IIIBBH", blob, off)
            name = read_cstr(blob, strbase, nameoff)
            if name and size and value == 0 and shndx not in owners:
                owners[shndx] = name
    return owners


def relocations(blob, sections, by_index, owners):
    """Map a relocation group's owning symbol -> its entries, symbols resolved.

    Each entry is (offset, type, target symbol NAME, addend). Resolving the
    index to a name is the point: adding one symbol renumbers the table, so an
    index comparison reports every relocation in the object as changed while
    the destinations are in fact untouched.
    """
    out = {}
    for n, sec in enumerate(sections):
        if sec["typ"] not in (SHT_REL, SHT_RELA):
            continue
        width = 12 if sec["typ"] == SHT_RELA else 8
        entries = []
        for i in range(sec["size"] // width):
            off = sec["off"] + i * width
            if sec["typ"] == SHT_RELA:
                offset, info, addend = struct.unpack_from("<III", blob, off)
            else:
                offset, info = struct.unpack_from("<II", blob, off)
                addend = None
            index = info >> 8
            target = by_index[index] if index < len(by_index) else "#%d" % index
            entries.append((offset, info & 0xFF, target, addend))
        key = owners.get(sec["info"]) or "%s#%d" % (sec["name"], n)
        out["%s (%s)" % (key, sec["name"])] = entries
    return out


def inventory(blob):
    """Everything the link consumes, in a form that ignores table ordering."""
    sections = parse_sections(blob)
    by_name, by_index = symbol_table(blob, sections)
    owners = section_owners(blob, sections)
    content, seen = {}, {}
    for n, sec in enumerate(sections):
        # symbol/string tables are ordering only; relocations are compared
        # separately with their symbol indices resolved to names
        if sec["typ"] in (SHT_SYMTAB, SHT_STRTAB, SHT_REL, SHT_RELA):
            continue
        seen[sec["name"]] = seen.get(sec["name"], -1) + 1
        key = owners.get(n) or "%s#%d" % (sec["name"], seen[sec["name"]])
        key = "%s (%s)" % (key, sec["name"])
        if sec["typ"] == SHT_NOBITS:
            content[key] = (sec["typ"], b"", sec["size"])
        else:
            content[key] = (sec["typ"], blob[sec["off"]:sec["off"] + sec["size"]], None)
    return dict(symbols=by_name, relocs=relocations(blob, sections, by_index, owners),
                sections=content)


def digest(inv):
    """A short stable hash of an inventory."""
    h = hashlib.sha256()
    for name in sorted(inv["symbols"]):
        h.update(repr((name,) + inv["symbols"][name]).encode())
    for name in sorted(inv["sections"]):
        typ, body, nobits = inv["sections"][name]
        h.update(("%s:%d:%s:" % (name, typ, nobits)).encode())
        h.update(body)
    for name in sorted(inv["relocs"]):
        h.update(("%s:" % name).encode())
        h.update(repr(inv["relocs"][name]).encode())
    return h.hexdigest()[:16]


def numbered_symbols(inv):
    """The compiler-numbered local symbols, whose names config pins."""
    return sorted(n for n in inv["symbols"] if NUMBERED.search(n))


def compare(before, after):
    """Describe every difference between two inventories, worst first."""
    findings = []

    gone = sorted(set(numbered_symbols(before)) - set(numbered_symbols(after)))
    added = sorted(set(numbered_symbols(after)) - set(numbered_symbols(before)))
    for name in added:
        findings.append(("numbered-symbol", "NEW compiler-numbered symbol %s "
                                            "-- config rows key on these numbers" % name))
    for name in gone:
        findings.append(("numbered-symbol", "compiler-numbered symbol %s disappeared "
                                            "-- config rows key on these numbers" % name))

    for name in sorted(set(before["symbols"]) - set(after["symbols"])):
        findings.append(("symbol-gone", "symbol %s is no longer defined" % name))
    for name in sorted(set(after["symbols"]) - set(before["symbols"])):
        if name in added:
            continue
        findings.append(("symbol-new", "symbol %s appeared" % name))

    for name in sorted(set(before["symbols"]) & set(after["symbols"])):
        a, b = before["symbols"][name], after["symbols"][name]
        if a == b:
            continue
        what = []
        if a[0] != b[0]:
            what.append("size 0x%x -> 0x%x" % (a[0], b[0]))
        if a[1] != b[1]:
            what.append("binding/type %d -> %d" % (a[1], b[1]))
        if a[2] != b[2]:
            what.append("section %s -> %s" % (a[2], b[2]))
        if a[3] != b[3]:
            differing = sum(1 for x, y in zip(a[3], b[3]) if x != y)
            what.append("%d of %d byte(s) differ" % (differing, min(len(a[3]), len(b[3]))))
        findings.append(("symbol-changed", "%s: %s" % (name, ", ".join(what))))

    for name in sorted(set(before["relocs"]) | set(after["relocs"])):
        a, b = before["relocs"].get(name, []), after["relocs"].get(name, [])
        if a == b:
            continue
        if len(a) != len(b):
            findings.append(("reloc", "%s: %d -> %d entries" % (name, len(a), len(b))))
            continue
        retargeted = [(x, y) for x, y in zip(a, b) if x[2] != y[2]]
        if retargeted:
            off, _t, was, _ad = retargeted[0][0]
            now = retargeted[0][1][2]
            findings.append(("reloc-destination",
                             "%s: %d relocation(s) point somewhere else, e.g. +0x%x %s -> %s"
                             % (name, len(retargeted), off, was, now)))
        else:
            moved = sum(1 for x, y in zip(a, b) if x != y)
            findings.append(("reloc", "%s: %d of %d entries changed (same destinations)"
                             % (name, moved, len(a))))

    for name in sorted(set(before["sections"]) | set(after["sections"])):
        a, b = before["sections"].get(name), after["sections"].get(name)
        if a == b:
            continue
        if a is None or b is None:
            findings.append(("section", "%s: %s" % (name, "added" if a is None else "removed")))
        elif a[2] != b[2]:
            findings.append(("section", "%s: nobits size %s -> %s" % (name, a[2], b[2])))
        elif len(a[1]) != len(b[1]):
            findings.append(("section", "%s: %d -> %d bytes" % (name, len(a[1]), len(b[1]))))
        else:
            differing = sum(1 for x, y in zip(a[1], b[1]) if x != y)
            findings.append(("section", "%s: %d of %d byte(s) differ"
                             % (name, differing, len(a[1]))))

    order = ["reloc-destination", "numbered-symbol", "symbol-gone", "symbol-new",
             "reloc", "section", "symbol-changed"]
    findings.sort(key=lambda f: order.index(f[0]) if f[0] in order else len(order))
    return findings


def word_diff(before, after, name):
    """Word-by-word diff of one symbol's bytes; ARM instructions are 4 bytes."""
    if name not in before["symbols"] or name not in after["symbols"]:
        raise ObjectDiffError("symbol %s is not defined on both sides" % name)
    a, b = before["symbols"][name][3], after["symbols"][name][3]
    rows = []
    for i in range(0, min(len(a), len(b)), 4):
        wa, = struct.unpack_from("<I", a, i)
        wb, = struct.unpack_from("<I", b, i)
        if wa != wb:
            rows.append((i, wa, wb))
    return len(a), len(b), rows


def build(src, rev, version, workdir):
    """Compile `src` from `rev` (None = working tree). Keeps the TU's basename.

    The basename is load-bearing: mwccarm spells it into `__sinit_<file>` and
    into numbered local-class symbols such as `Frame$2583Player_cpp`.
    """
    staged = pathlib.Path(workdir) / src.name
    if rev is None:
        staged.write_bytes(src.read_bytes())
    else:
        proc = subprocess.run(["git", "show", "%s:%s" % (rev, src.as_posix())],
                              capture_output=True)
        if proc.returncode != 0:
            raise ObjectDiffError("git show %s:%s failed: %s"
                                  % (rev, src.as_posix(), proc.stderr.decode().strip()))
        staged.write_bytes(proc.stdout)
    flags = match.resolve_cpp_flags(src, match.DEFAULT_FLAGS)
    obj = match.compile_c(staged, version, flags)
    if not obj:
        raise ObjectDiffError("%s did not compile at %s" % (src.as_posix(), version))
    return obj


def run_one(src, base, version, args):
    """Compare one TU. Returns True when the object is identical."""
    with tempfile.TemporaryDirectory() as before_dir, \
            tempfile.TemporaryDirectory() as after_dir:
        before = inventory(build(src, base, version, before_dir))
        after = inventory(build(src, None, version, after_dir))

    db, da = digest(before), digest(after)
    same = db == da
    rel = src.as_posix()

    if args.numbered:
        for name in numbered_symbols(after):
            print("  numbered: %s" % name)
        if not numbered_symbols(after):
            print("  numbered: none -- this TU has no config-pinned numbered symbols")

    if args.symbol:
        len_a, len_b, rows = word_diff(before, after, args.symbol)
        print("  %s: %d -> %d bytes, %d word(s) differ" % (args.symbol, len_a, len_b, len(rows)))
        for off, wa, wb in rows:
            print("    +0x%03x  base %08x  tree %08x" % (off, wa, wb))

    if same:
        if not args.quiet:
            print("%s\tIDENTICAL\t%s" % (rel, da))
        return True

    print("%s\tDIFFERS\tbase %s  tree %s" % (rel, db, da))
    findings = compare(before, after)
    for kind, text in findings[:args.limit]:
        print("  [%s] %s" % (kind, text))
    if len(findings) > args.limit:
        print("  ... %d more (raise --limit)" % (len(findings) - args.limit))
    return False


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("files", nargs="+", help="source files under src/")
    ap.add_argument("--base", default="HEAD",
                    help="git revision to compare against (default HEAD)")
    ap.add_argument("--version", default="2004/b56", help="mwccarm version (default 2004/b56)")
    ap.add_argument("--symbol", help="also print a word-level diff of this symbol")
    ap.add_argument("--numbered", action="store_true",
                    help="list the TU's compiler-numbered local symbols")
    ap.add_argument("--limit", type=int, default=40, help="max findings to print per file")
    ap.add_argument("--quiet", action="store_true", help="print nothing for identical files")
    args = ap.parse_args(argv)

    ok = True
    for name in args.files:
        src = pathlib.Path(name)
        try:
            if not run_one(src, args.base, args.version, args):
                ok = False
        except ObjectDiffError as exc:
            print("%s\tERROR\t%s" % (src.as_posix(), exc), file=sys.stderr)
            return 2
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
