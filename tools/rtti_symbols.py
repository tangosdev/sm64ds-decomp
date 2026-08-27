#!/usr/bin/env python3
"""Phase 1: give the ROM's type_info records and name strings their real symbols.

Every one of the 429 records found by `tools/rtti_extract.py` sits at an address that
`config/**/symbols.txt` already knows about -- as an anonymous `data_<addr>` placeholder.
So does every one of the 429 name strings.  This pass renames those 858 placeholders to
the Itanium symbols they actually are:

    record at 0x0208e9e0  ->  _ZTI8dFader_c        (the type_info object)
    string at 0x0208ea48  ->  _ZTS8dFader_c        (its name string)

The mangled suffix is the raw string the ROM stores, verbatim -- that string *is* the
Itanium type name, which is why `_ZTS` + it is the right symbol and no re-mangling is
needed.  Nested names (`N8dGraph_c10callback_cE`) and the `St` substitution
(`St9type_info`) come out correct for free.

Nothing about the ROM image changes.  Relocations in this tree are address-based
(`from:0x… kind:… to:0x… module:…`), so a symbol name is presentation over an address
that is not moving.  A bad rename here cannot silently alter bytes; it can only break
the link loudly, which is what the gate below is for.

What is deliberately NOT renamed
--------------------------------
`tools/import_symbols.py` established the rule this pass follows: a data symbol that is
**referenced by name from a matched source** is deferred, because renaming it means
rewriting those sources, which drags every referencing file into the PR and turns
`validate` red even though the rename is byte-safe.

Ten of the 858 are referenced (see `--report`).  For these, deferral is not merely
procedural -- some of the references are *overlay-ambiguous* and renaming them would
assert something false.  `0x0211396c` exists in both ov018 and ov032;
`src/unnamed/ov018/0211/__sinit_ov018_02112c80.cpp` -- an ov018 file -- refers to it by ov032's spelling,
`data_ov032_0211396c`.  Renaming that to `_ZTS14daObjTdWater_c` would state that an
ov018 reference points at ov032's typeinfo string.  It may; nothing here proves it.
See notes/overlay-ambiguous-references.md and tools/overlay_residency.py.

Do those ten only after settling the attribution, one at a time, in a quiet window.

Usage
-----
    python tools/rtti_symbols.py                # dry run: what would change
    python tools/rtti_symbols.py --report       # + the deferral list and why
    python tools/rtti_symbols.py --apply        # rewrite config/**/symbols.txt
    python tools/rtti_symbols.py --revert       # undo, by name, exactly
"""

from __future__ import annotations

import argparse
import collections
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
RTTI = REPO / "build" / "rtti.json"
SYM_RE = re.compile(r"^(?P<lead>\s*)(?P<name>\S+)(?P<rest>\s+kind:\S+\s+addr:(?P<addr>0x[0-9a-fA-F]+).*)$")
PLACEHOLDER = re.compile(r"^data_(?:ov\d+_)?[0-9a-fA-F]{8}$")
DATA_REF = re.compile(r"\bdata_(?:ov\d+_)?[0-9a-fA-F]{8}\b")

# Roots whose contents feed the compiler or the link.  A reference from notes/ or
# progress/ (training corpora, worklists) does not affect the build and is not a
# reason to defer.
#
# `mods/` is here because rombuild.py --profile mods compiles it.  There is no
# extension whitelist: an earlier one covered .c/.cpp/.h/.hpp/.py/.txt/.cmake and so
# would have missed a reference from a .s, a .lcf, a Makefile, or a .yaml.  Nothing
# fell through it in practice, but "the scan happened to cover it" is not a property
# worth relying on when the cost of scanning everything is one pass over a tree we
# already walk.  Binary files are skipped by decode failure, not by extension.
BUILD_ROOTS = ("src", "include", "port", "tools", "config", "mods")
SKIP_DIRS = {".git", "__pycache__", ".vs"}


def symbol_files():
    return sorted((REPO / "config").rglob("symbols.txt"))


def load_symbols():
    """module -> addr -> [(name, path, lineno)]"""
    out = collections.defaultdict(lambda: collections.defaultdict(list))
    for p in symbol_files():
        mod = p.parent.name
        for i, line in enumerate(p.read_text(encoding="utf-8", errors="replace").splitlines()):
            m = SYM_RE.match(line)
            if m:
                out[mod][int(m.group("addr"), 16)].append((m.group("name"), p, i))
    return out


def all_symbol_names():
    names = set()
    for p in symbol_files():
        for line in p.read_text(encoding="utf-8", errors="replace").splitlines():
            m = SYM_RE.match(line)
            if m:
                names.add(m.group("name"))
    return names


def plan(stats):
    rtti = json.loads(RTTI.read_text(encoding="utf-8"))
    syms = load_symbols()
    existing = all_symbol_names()

    proposals = {}          # old name -> (new name, module, addr, what, class)
    blocked = []
    for key, r in sorted(rtti["records"].items()):
        for addr_s, what in ((r["addr"], "_ZTI"), (r["name_addr"], "_ZTS")):
            addr = int(addr_s, 16)
            here = syms.get(r["module"], {}).get(addr, [])
            new = what + r["mangled"]
            if len(here) != 1:
                blocked.append((r["module"], addr_s, new,
                                "address carries %d symbols, not 1" % len(here)))
                continue
            old = here[0][0]
            if not PLACEHOLDER.match(old):
                blocked.append((r["module"], addr_s, new,
                                "already named %s -- never clobber" % old))
                continue
            if new in existing:
                blocked.append((r["module"], addr_s, new, "name already used in config"))
                continue
            if old in proposals:
                blocked.append((r["module"], addr_s, new,
                                "placeholder %s claimed twice" % old))
                continue
            proposals[old] = (new, r["module"], addr_s, what, r["name"])
    stats["proposed"] = len(proposals)
    stats["blocked"] = len(blocked)

    # collision check across the whole proposal set
    news = [v[0] for v in proposals.values()]
    dupes = [n for n, c in collections.Counter(news).items() if c > 1]
    stats["proposed_names_not_unique"] = len(dupes)

    # who references a placeholder by name, in build-relevant files only
    refs = collections.defaultdict(list)
    for root in BUILD_ROOTS:
        base = REPO / root
        if not base.is_dir():
            continue
        for f in base.rglob("*"):
            if not f.is_file() or SKIP_DIRS & set(f.parts):
                continue
            if f.name == "symbols.txt":
                continue          # the file we are rewriting; not a reference
            if f.resolve() == pathlib.Path(__file__).resolve():
                continue          # this file names one in its own docstring
            try:
                text = f.read_bytes().decode("utf-8")
            except (OSError, UnicodeDecodeError):
                continue          # binary, or unreadable: cannot hold a name
            for n in set(DATA_REF.findall(text)):
                if n in proposals:
                    refs[n].append(f.relative_to(REPO).as_posix())
    stats["deferred_referenced_by_name"] = len(refs)

    clean = {k: v for k, v in proposals.items() if k not in refs}
    stats["clean"] = len(clean)
    return clean, {k: sorted(v) for k, v in refs.items()}, proposals, blocked, dupes


def placeholder_name(module, addr_s):
    """The dsd placeholder this address would carry: data_<addr> / data_ov<NN>_<addr>."""
    bare = addr_s[2:].rjust(8, "0").lower()
    return "data_%s" % bare if module == "arm9" else "data_%s_%s" % (module, bare)


def revert_plan(stats):
    """new name -> old placeholder, derived from the ROM data, not from the tree.

    `plan()` finds work by looking for `data_` placeholders, so once --apply has run
    it correctly reports nothing left to do.  Reusing it to build the undo map
    therefore produced an undo that silently did nothing -- the worst kind, since it
    exits 0.  The reverse map is reconstructed from build/rtti.json plus dsd's
    placeholder naming convention, and every entry is checked against what is
    actually in config before it is offered.
    """
    rtti = json.loads(RTTI.read_text(encoding="utf-8"))
    syms = load_symbols()
    out = {}
    for key, r in sorted(rtti["records"].items()):
        for addr_s, what in ((r["addr"], "_ZTI"), (r["name_addr"], "_ZTS")):
            new = what + r["mangled"]
            here = syms.get(r["module"], {}).get(int(addr_s, 16), [])
            if [n for n, _p, _i in here] == [new]:
                out[new] = (placeholder_name(r["module"], addr_s), r["module"],
                            addr_s, what, r["name"])
    stats["revertible"] = len(out)
    return out


def rewrite(mapping, apply, reverse=False):
    """Rename in place.  Only a line whose name matches exactly is touched."""
    pairs = {k: v[0] for k, v in mapping.items()}
    changed = collections.Counter()
    for p in symbol_files():
        raw = p.read_bytes()
        # Preserve the file's own line ending.  These files are CRLF in the working
        # tree; git normalizes on the way in, so writing LF produces an identical
        # commit -- but it flips the file on disk, and the next checkout flips it
        # back.  Churn with no diff to show for it.
        nl = "\r\n" if raw.count(b"\r\n") > raw.count(b"\n") - raw.count(b"\r\n") else "\n"
        lines = raw.decode("utf-8", "replace").splitlines()
        out, hit = [], 0
        for line in lines:
            m = SYM_RE.match(line)
            if m and m.group("name") in pairs:
                out.append(m.group("lead") + pairs[m.group("name")] + m.group("rest"))
                hit += 1
            else:
                out.append(line)
        if hit:
            changed[p.parent.name] = hit
            if apply:
                with open(p, "w", encoding="utf-8", newline="") as fh:
                    fh.write(nl.join(out) + nl)
    return changed


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--revert", action="store_true")
    ap.add_argument("--report", action="store_true")
    ap.add_argument("--include-referenced", action="store_true",
                    help="also rename the deferred ones (does NOT rewrite src -- "
                         "you must fix the referencing files yourself)")
    a = ap.parse_args()

    stats = collections.Counter()
    if a.revert:
        target = revert_plan(stats)
        refs, proposals, blocked, dupes = {}, target, [], []
    else:
        clean, refs, proposals, blocked, dupes = plan(stats)
        target = proposals if a.include_referenced else clean

    if a.report:
        print("== populations ==")
        for k in sorted(stats):
            print("  %-38s %d" % (k, stats[k]))
        print("\n== deferred: referenced by name from a build input (%d) ==" % len(refs))
        for n in sorted(refs):
            new, mod, addr, what, cls = proposals[n]
            print("  %-26s %s  would become %s" % (n, mod, new))
            for f in refs[n]:
                print("        %s" % f)
        if blocked:
            print("\n== blocked (%d) ==" % len(blocked))
            for b in blocked[:20]:
                print("  %s %s %s: %s" % b)
        if dupes:
            print("\n== PROPOSED NAMES NOT UNIQUE (%d) ==" % len(dupes))
            for d in dupes:
                print("  %s" % d)

    if dupes:
        print("refusing: proposed names are not unique")
        return 1

    changed = rewrite(target, a.apply, reverse=a.revert)
    verb = "renamed" if a.apply else "would rename"
    if a.revert:
        verb = "reverted" if a.apply else "would revert"
    print("%s %d symbols across %d module(s)%s"
          % (verb, sum(changed.values()), len(changed),
             "" if a.apply else "  (dry run -- pass --apply)"))
    if a.report:
        for mod in sorted(changed):
            print("   %-8s %d" % (mod, changed[mod]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
