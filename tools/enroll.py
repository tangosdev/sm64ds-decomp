"""Write `src/` file entries into config/**/delinks.txt so the ROM build can use them.

Each entry carves an address range out of its module's gap object and hands it to a
named file. Two modes, and the difference is one keyword:

  rombytes  (default)  the entry has no `complete` line, so **dsd** supplies the object
                       from the original ROM bytes. The build stays byte-identical by
                       construction. This is the scale test: it proves dsd delink/lcf and
                       an ~11k-object mwldarm link hold up before correctness is in play.

  complete             the entry gets `complete`, so **mwccarm** compiles the source file
                       and its output is what lands in the ROM. This is the real thing,
                       and only files that pass the eligibility rules may use it.

Entries already marked `complete` are preserved across runs unless --demote-all is given,
so promoting a file is additive and re-running is idempotent.

Usage:
    python tools/enroll.py --dry-run              # what would change
    python tools/enroll.py                        # rombytes for every candidate
    python tools/enroll.py --complete-list f.txt  # promote the names in f.txt
    python tools/enroll.py --clear                # back to bare global sections

A candidate is a function symbol in config/**/symbols.txt that has a src/<name>.c[pp]
without a `// NONMATCHING` hatch, a nonzero size, an unambiguous module, and an address
fully inside one of that module's declared sections. See notes/rom-build.md.
"""
import argparse
import collections
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG, SRC = REPO / "config", REPO / "src"
sys.path.insert(0, str(REPO / "tools"))
import asm_policy  # noqa: E402
import srcpath as SP  # noqa: E402
# An intentional divergence from the ROM lives in mods/<symbol>.c and takes precedence
# over src/<symbol>.c for that one function. Keeping it a file-existence rule (rather
# than a hand-edited delinks.txt entry) is what makes regeneration idempotent: a mod
# survives every enroll.py run, and deleting the file reverts to stock.
MODS = REPO / "mods"

SYM_RE = re.compile(
    r"^(\S+)\s+kind:function\((arm|thumb),size=0x([0-9a-fA-F]+)\)\s+addr:0x([0-9a-fA-F]+)")
SEC_RE = re.compile(
    r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)\s+kind:(\S+)")


def read_delinks(path):
    """Split a delinks.txt into its global section header and its file entries.

    Header = the leading indented section lines. A file entry is an unindented path
    ending in ':' followed by indented lines; we only care whether `complete` is there.
    """
    header, entries, cur = [], {}, None
    for line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        if not line.strip():
            continue
        if line[0].isspace():
            if cur is None:
                header.append(line.rstrip())
            elif line.strip() == "complete":
                entries[cur] = True
        else:
            cur = line.strip().rstrip(":")
            entries.setdefault(cur, False)
    return header, entries


def sections(header):
    out = []
    for line in header:
        m = SEC_RE.match(line)
        if m:
            out.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return out


EXCLUDE = REPO / "config" / "rombuild-exclude.txt"


def excluded():
    """Symbol names that must stay inside the gap object (see the file's comments)."""
    if not EXCLUDE.is_file():
        return set()
    return {l.strip() for l in EXCLUDE.read_text(encoding="utf-8").splitlines()
            if l.strip() and not l.startswith("#")}


def candidates():
    """(module_dir, name, srcpath, addr, size, section) for every eligible function."""
    skip_names = excluded()
    name_mods = collections.defaultdict(set)
    mods = {}
    for sym in sorted(CONFIG.rglob("symbols.txt")):
        d = sym.parent
        header, _ = read_delinks(d / "delinks.txt") if (d / "delinks.txt").is_file() else ([], {})
        syms = []
        for line in sym.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = SYM_RE.match(line)
            if m:
                syms.append((m.group(1), m.group(2), int(m.group(3), 16), int(m.group(4), 16)))
                name_mods[m.group(1)].add(d.as_posix())
        mods[d] = (sections(header), syms)

    out, skipped = [], collections.Counter()
    for d, (secs, syms) in mods.items():
        for (name, mode, size, addr) in syms:
            if name in skip_names:
                skipped["on the exclude list"] += 1
                continue
            # mods/ still resolves flat -- it is a handful of deliberate divergences
            # and has no reason to grow directories. src/ goes through srcpath, so a
            # file in a subdirectory is still found; the entry written below is
            # f.relative_to(REPO), so the delinks path follows the file automatically.
            f = next((MODS / (name + e) for e in (".c", ".cpp")
                      if (MODS / (name + e)).is_file()), None)
            if f is None:
                f = SP.path_for(name)
            if f is None:
                skipped["no src file"] += 1
                continue
            is_mod = f.parent == MODS
            if is_mod:
                skipped["intentional mod (mods/)"] += 1
            if size == 0:
                skipped["zero size"] += 1
                continue
            # A carved object's section alignment is inherited from the delinks.txt
            # header and cannot be overridden per entry (dsd rejects `align:` there).
            # dsd gives code sections 4-byte alignment, so a function that does not
            # start 4-aligned - the thumb SDK stubs at 2-byte-aligned addresses - gets
            # padded up by the linker, inserting bytes and shifting the whole module.
            if addr % 4 != 0:
                skipped["not 4-byte aligned (thumb stub)"] += 1
                continue
            # A 4-aligned Thumb function would slip past the alignment gate above and
            # then be compiled as ARM; only the size check would bounce it. Be explicit.
            if mode != "arm":
                skipped["thumb function"] += 1
                continue
            # A mod is *meant* to diverge, so the not-a-byte-match hatch does not apply
            # to it; every structural rule still does, because a mod that changes size
            # or emits data would shift the module just as badly as a bad match would.
            if not is_mod and asm_policy.has_draft_banner(
                    f.read_text(encoding="utf-8", errors="ignore")):
                skipped["NONMATCHING hatch"] += 1
                continue
            if len(name_mods[name]) > 1:
                skipped["name in >1 module"] += 1
                continue
            sec = next((s for (s, lo, hi) in secs if lo <= addr and addr + size <= hi), None)
            if sec is None:
                skipped["no containing section"] += 1
                continue
            out.append((d, name, f.relative_to(REPO).as_posix(), addr, size, sec))
    return out, skipped


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--complete-list", help="file of symbol names to mark `complete`")
    ap.add_argument("--all-complete", action="store_true", help="mark every candidate `complete`")
    ap.add_argument("--demote-all", action="store_true", help="drop existing `complete` marks")
    ap.add_argument("--clear", action="store_true", help="remove all file entries")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    promote = set()
    if args.complete_list:
        promote = {l.strip() for l in pathlib.Path(args.complete_list).read_text().splitlines()
                   if l.strip()}

    cands, skipped = candidates()
    bymod = collections.defaultdict(list)
    for c in cands:
        bymod[c[0]].append(c)

    n_entries = n_complete = 0
    touched = []
    for d in sorted(CONFIG.rglob("symbols.txt")):
        path = d.parent / "delinks.txt"
        if not path.is_file():
            continue
        header, existing = read_delinks(path)
        # Key the carried-over `complete` marks by symbol name, not path: a function
        # that moved between src/ and mods/ is the same function and must keep its mark.
        existing_by_name = {pathlib.Path(p2).stem: v for p2, v in existing.items()}
        lines = list(header)

        if not args.clear:
            # Sorted by address: delink file entries are consumed in link order.
            for (_d, name, rel, addr, size, sec) in sorted(bymod.get(d.parent, []), key=lambda x: x[3]):
                done = (args.all_complete or name in promote
                        or (existing_by_name.get(name, False) and not args.demote_all))
                lines.append(f"{rel}:")
                if done:
                    lines.append("    complete")
                    n_complete += 1
                lines.append(f"    {sec} start:0x{addr:08x} end:0x{addr + size:08x}")
                lines.append("")
                n_entries += 1

        text = "\n".join(lines).rstrip("\n") + "\n"
        if text != path.read_text(encoding="utf-8", errors="ignore"):
            touched.append(path.relative_to(REPO).as_posix())
            if not args.dry_run:
                path.write_text(text, encoding="utf-8", newline="\n")

    print(f"candidates: {len(cands)}")
    for k, v in skipped.most_common():
        print(f"  skipped {v:6d}  {k}")
    print(f"entries written: {n_entries}  ({n_complete} complete / "
          f"{n_entries - n_complete} rom-bytes)")
    print(f"delinks.txt files {'that would change' if args.dry_run else 'changed'}: {len(touched)}")


if __name__ == "__main__":
    main()
