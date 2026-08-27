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
ENTRY_SEC_RE = re.compile(
    r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")


class EnrollmentError(ValueError):
    """A delinks rewrite would lose or ambiguously expand source ownership."""


def read_delinks_full(path):
    """Return ``(header, [(source_path, body_lines), ...])`` without losing bodies.

    The old boolean view in :func:`read_delinks` is enough for readers that only ask
    whether a path is complete.  A writer needs the full entry: one translation unit
    may own several functions and several sections, none of which can be reconstructed
    safely from a function list alone.
    """
    header, entries = [], []
    cur, body = None, []
    for raw in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        if not raw.strip():
            continue
        line = raw.rstrip()
        if line[0].isspace():
            if cur is None:
                header.append(line)
            else:
                body.append(line)
            continue
        if cur is not None:
            entries.append((cur, body))
        cur, body = line.strip().rstrip(":"), []
    if cur is not None:
        entries.append((cur, body))
    return header, entries


def read_delinks(path):
    """Split a delinks.txt into its global section header and its file entries.

    Header = the leading indented section lines. A file entry is an unindented path
    ending in ':' followed by indented lines; we only care whether `complete` is there.
    """
    header, full = read_delinks_full(path)
    entries = {}
    for rel, body in full:
        entries[rel] = any(line.strip() == "complete" for line in body)
    return header, entries


def _body_with_complete(body, done):
    """Toggle only ``complete`` while preserving every other structured line."""
    out, found = [], False
    for line in body:
        if line.strip() != "complete":
            out.append(line)
        elif done and not found:
            out.append(line)
            found = True
    if done and not found:
        out.insert(0, "    complete")
    return out


def _body_ranges(body):
    """Structured ``(section, start, end)`` rows in one entry body."""
    ranges = []
    for line in body:
        m = ENTRY_SEC_RE.match(line)
        if m:
            ranges.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return ranges


def _body_covers(body, rows):
    """Whether every candidate's full range is licensed by the preserved body."""
    ranges = _body_ranges(body)
    return all(any(sec == row[5] and lo <= row[3] and row[3] + row[4] <= hi
                   for sec, lo, hi in ranges)
               for row in rows)


def render_entries(header, existing_entries, module_candidates, promote=(),
                   all_complete=False, demote_all=False):
    """Render one delinks entry per source, preserving existing ownership bodies.

    ``module_candidates`` remains one row per function because eligibility is still a
    per-symbol question.  Rendering is a per-object question: several rows may point at
    one source, including overlapping entry-point aliases inside a single asm routine.
    Existing bodies are authoritative for their ranges and non-text sections.  A new
    shared source without such an entry is refused because a function list cannot prove
    the object's licensed span.

    Returns ``(text, entry_count, complete_count)``.
    """
    promote = set(promote)
    existing = {}
    for rel, body in existing_entries:
        if rel in existing:
            raise EnrollmentError(f"duplicate existing delinks entry for {rel}")
        existing[rel] = list(body)

    grouped = collections.defaultdict(list)
    for row in module_candidates:
        grouped[row[2]].append(row)

    ordered = sorted(grouped.items(), key=lambda item: min(row[3] for row in item[1]))
    rendered, consumed, n_complete = [], set(), 0
    for rel, rows in ordered:
        rows.sort(key=lambda row: row[3])
        names = {row[1] for row in rows}
        requested = names & promote
        if len(names) > 1 and requested and requested != names:
            missing = ", ".join(sorted(names - requested))
            raise EnrollmentError(
                f"partial --complete-list for shared source {rel}; also require: {missing}")

        body = existing.get(rel)
        if body is not None:
            consumed.add(rel)
        if body is None and len(rows) == 1:
            # Preserve the historical move contract: a same-symbol source that moved
            # between src/ and mods/ keeps its full range/body and complete state.
            name = rows[0][1]
            moved = [(old_rel, old_body) for old_rel, old_body in existing.items()
                     if pathlib.PurePosixPath(old_rel).stem == name]
            if len(moved) > 1:
                raise EnrollmentError(
                    f"ambiguous prior delinks entries for moved symbol {name}: "
                    + ", ".join(old_rel for old_rel, _body in moved))
            if moved:
                old_rel, body = moved[0]
                consumed.add(old_rel)
        if body is None:
            if len(rows) > 1:
                raise EnrollmentError(
                    f"shared source {rel} has no existing delinks body; refusing to "
                    "infer its object range from individual functions")
            row = rows[0]
            body = [f"    {row[5]} start:0x{row[3]:08x} "
                    f"end:0x{row[3] + row[4]:08x}"]
        elif not _body_covers(body, rows):
            raise EnrollmentError(
                f"existing delinks body for {rel} does not cover every candidate range")

        was_complete = any(line.strip() == "complete" for line in body)
        selected = all_complete or bool(names) and names <= promote
        done = selected or (was_complete and not demote_all)
        rendered.append((min(row[3] for row in rows), rel,
                         _body_with_complete(body, done)))
        n_complete += int(done)

    # An incomplete entry is an intentional ROM-byte placeholder: dsd does not open
    # its path, and dropping it is an unrelated ownership mutation. Carry it through
    # verbatim even when candidate policy currently rejects its source (for example a
    # NONMATCHING banner). A complete unmatched entry is different: preserving it may
    # compile a missing/draft source while dropping it would silently fall back to ROM
    # bytes, so neither rewrite is safe and the operator must resolve it explicitly.
    for rel, body in existing.items():
        if rel in consumed:
            continue
        if any(line.strip() == "complete" for line in body):
            raise EnrollmentError(
                f"complete existing source {rel} has no candidate; refusing to demote "
                "or preserve an unverified build input implicitly")
        ranges = _body_ranges(body)
        if not ranges:
            raise EnrollmentError(
                f"existing ROM-byte entry {rel} has no section range to preserve")
        rendered.append((min(lo for _sec, lo, _hi in ranges), rel, list(body)))

    lines = list(header)
    for _addr, rel, body in sorted(rendered, key=lambda row: (row[0], row[1])):
        lines.append(f"{rel}:")
        lines.extend(body)
        lines.append("")

    text = "\n".join(lines).rstrip("\n") + "\n"
    return text, len(rendered), n_complete


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
                # A partitioned production TU is the real source owner, but tracked
                # delinks intentionally keeps its one-function comparison selectors.
                # Preserve those selectors here; rombuild swaps in the freshly
                # derived compiler output through its generated production profile.
                legacy = SP.production_legacy_path_for(name)
                f = REPO / legacy if legacy else SP.path_for(name)
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
    touched, rewrites = [], []
    for d in sorted(CONFIG.rglob("symbols.txt")):
        path = d.parent / "delinks.txt"
        if not path.is_file():
            continue
        header, existing = read_delinks_full(path)
        if args.clear:
            text = "\n".join(header).rstrip("\n") + "\n"
            module_entries = module_complete = 0
        else:
            try:
                text, module_entries, module_complete = render_entries(
                    header, existing, bymod.get(d.parent, []), promote,
                    all_complete=args.all_complete, demote_all=args.demote_all)
            except EnrollmentError as exc:
                print(f"enroll: {path.relative_to(REPO).as_posix()}: {exc}",
                      file=sys.stderr)
                return 2
        n_entries += module_entries
        n_complete += module_complete
        if text != path.read_text(encoding="utf-8", errors="ignore"):
            touched.append(path.relative_to(REPO).as_posix())
            rewrites.append((path, text))

    # Rendering every module first makes every refusal fail closed: a partial shared
    # complete-list or malformed preserved body cannot leave earlier modules rewritten.
    if not args.dry_run:
        for path, text in rewrites:
            path.write_text(text, encoding="utf-8", newline="\n")

    print(f"candidates: {len(cands)}")
    for k, v in skipped.most_common():
        print(f"  skipped {v:6d}  {k}")
    print(f"entries written: {n_entries}  ({n_complete} complete / "
          f"{n_entries - n_complete} rom-bytes)")
    print(f"delinks.txt files {'that would change' if args.dry_run else 'changed'}: {len(touched)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
