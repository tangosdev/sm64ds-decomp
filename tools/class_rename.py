"""Rename one class to the name the cartridge's RTTI gives it.

WHY A TOOL AND NOT A SUBSTITUTION
---------------------------------
The obvious approach -- replace `CannonHatch` with `daObjCannonShutter_c`
everywhere -- silently corrupts every mangled symbol it touches, because an
Itanium identifier carries its own LENGTH:

    _ZN11CannonHatch8BehaviorEv   ->  _ZN20daObjCannonShutter_c8BehaviorEv
        ^^                                ^^
        11 must become 20; a text substitution leaves it reading `11daObj...`,
        which is a different, nonexistent symbol that still looks plausible.

A prior sweep of this shape produced 489 corruptions by getting the order and the
boundaries wrong. So the rules here are ordered mangled-first, every plain-name
rule is word-bounded, and every rule is IDEMPOTENT -- `--verify-idempotent` (on by
default) re-runs the whole plan against its own output and fails if a second pass
would change anything, which is what catches a rule that eats its own result.

WHAT IT REWRITES
----------------
1. mangled names   -- only inside a `_Z...` token, `<len><Old>` -> `<newlen><New>`
2. the class name  -- word-bounded `\\bOld\\b`
3. derived C symbols -- `\\bOld_` (e.g. `CannonHatch_Spawn`, `..._SpawnInfo`),
                      only with --with-derived, because those names are coined at
                      both ends: the ROM says nothing about them, so renaming them
                      is a convention choice, not evidence. Off by default.
4. include guards  -- `\\bOLD_H\\b` -> `\\bNEW_H\\b`, uppercased
5. FILENAMES       -- via `git mv`, so history follows

WHAT IT DOES NOT TOUCH
----------------------
`docs/` is generated (rtti_extract -> evidence_hierarchy -> rtti_reconcile -> page,
in that order -- notes: generator-dependency-order). Rewriting it by hand would
produce a report that disagrees with its own generator on the next run. Regenerate
it after the rename instead.

`config/arm9/**/symbols.txt` IS rewritten, and that is the load-bearing edit: it is
what makes the build emit the new symbol. Watch for the alias case -- 125 classes
already carry BOTH the coined and the ROM name at one address, and for those the
right operation is to DELETE the coined alias, not rename it onto a name that is
already there. This tool refuses rather than create a duplicate (notes:
symbol-rename-shadowing: a duplicate at size=0x0 fakes "lost N matched function(s)").

Usage:
  python tools/class_rename.py --class CannonHatch --to daObjCannonShutter_c
  python tools/class_rename.py --class CannonHatch --to daObjCannonShutter_c --apply
  python tools/class_rename.py --from-census ov002 --dry-run     # plan every naked class
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent

# Swept by default. `docs/` is generated and deliberately absent; `build/` is
# gitignored output; `extracted/` is the cartridge and must never be edited.
DEFAULT_PATHS = ("src", "src_tu", "include", "config", "symbols", "notes")
SKIP_DIRS = {".git", "build", "extracted", "docs", "__pycache__", "tools/mwccarm"}
TEXT_SUFFIXES = {".c", ".cpp", ".h", ".hpp", ".txt", ".json", ".tsv", ".md", ".yaml", ".yml"}

MANGLED_TOKEN = re.compile(r"_Z[A-Za-z0-9_]+")


def git(*args, check=True):
    r = subprocess.run(["git", *args], cwd=REPO, capture_output=True, text=True)
    if check and r.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)} failed:\n{r.stdout}\n{r.stderr}")
    return r


def rules(old, new, with_derived):
    """Ordered, idempotent rewrite rules. Mangled first -- always."""
    out = []

    old_tag, new_tag = f"{len(old)}{old}", f"{len(new)}{new}"

    def mangled(text):
        # Confined to `_Z...` tokens so a plain identifier that happens to spell
        # `11CannonHatch` in prose is never touched.
        return MANGLED_TOKEN.sub(lambda m: m.group(0).replace(old_tag, new_tag), text)

    out.append(("mangled", mangled))
    out.append(("class-name", lambda t: re.sub(rf"\b{re.escape(old)}\b", new, t)))
    if with_derived:
        out.append(("derived-symbols",
                    lambda t: re.sub(rf"\b{re.escape(old)}_", f"{new}_", t)))
    out.append(("include-guard",
                lambda t: re.sub(rf"\b{re.escape(old.upper())}_H\b", f"{new.upper()}_H", t)))
    return out


def apply_rules(text, rs):
    for _, fn in rs:
        text = fn(text)
    return text


def tracked_files(paths):
    r = git("ls-files", "--", *paths)
    out = []
    for line in r.stdout.splitlines():
        p = pathlib.Path(line)
        if any(str(p).replace("\\", "/").startswith(s) for s in SKIP_DIRS):
            continue
        if p.suffix in TEXT_SUFFIXES:
            out.append(line)
    return out


def plan(old, new, paths, with_derived):
    rs = rules(old, new, with_derived)
    edits, renames, unchanged_hits = [], [], []

    for rel in tracked_files(paths):
        p = REPO / rel
        try:
            text = p.read_text(encoding="utf-8")
        except (UnicodeDecodeError, OSError):
            continue
        if old not in text:
            continue
        new_text = apply_rules(text, rs)
        if new_text != text:
            edits.append((rel, text, new_text))
        else:
            # `old` occurs but no rule fires: a substring hit inside a longer
            # identifier. Reported, never silently ignored -- it is exactly where
            # a hand sweep would have guessed.
            unchanged_hits.append(rel)

        base = pathlib.PurePosixPath(rel).name
        new_base = apply_rules(base, rs)
        if new_base != base:
            renames.append((rel, str(pathlib.PurePosixPath(rel).parent / new_base)))

    return rs, edits, renames, unchanged_hits


def check_collision(new, paths):
    """Refuse to rename onto a name some other class already defines."""
    bad = []
    for rel in tracked_files(paths):
        if not rel.endswith("symbols.txt"):
            continue
        text = (REPO / rel).read_text(encoding="utf-8", errors="replace")
        if re.search(rf"^_ZTV{len(new)}{re.escape(new)}\s", text, re.M):
            bad.append(rel)
    return bad


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0],
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--class", dest="old", required=True)
    ap.add_argument("--to", dest="new", required=True)
    ap.add_argument("--paths", nargs="*", default=list(DEFAULT_PATHS))
    ap.add_argument("--with-derived", action="store_true",
                    help="also rename Old_Spawn / Old_SpawnInfo style C symbols")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--allow-collision", action="store_true")
    args = ap.parse_args(argv)

    old, new = args.old, args.new
    if old == new:
        sys.exit("--class and --to are the same name")

    clash = check_collision(new, args.paths)
    if clash and not args.allow_collision:
        print(f"REFUSING: _ZTV{len(new)}{new} is already defined in: {clash}")
        print("  A class that already carries the ROM name needs the COINED ALIAS")
        print("  DELETED, not this rename -- see the module docstring.")
        return 2

    rs, edits, renames, unchanged = plan(old, new, args.paths, args.with_derived)

    print(f"=== {old}  ->  {new}   (mangled {len(old)}{old} -> {len(new)}{new})")
    print(f"rules: {', '.join(n for n, _ in rs)}")
    print(f"\n{len(edits)} file(s) edited:")
    for rel, a, b in edits:
        n = sum(1 for x, y in zip(a.splitlines(), b.splitlines()) if x != y)
        print(f"   {rel}   ({n} line(s))")
    print(f"\n{len(renames)} file(s) renamed:")
    for a, b in renames:
        print(f"   {a}\n     -> {b}")
    if unchanged:
        print(f"\n{len(unchanged)} file(s) MENTION {old} but no rule fired "
              f"(substring inside a longer identifier) -- review:")
        for rel in unchanged:
            print(f"   {rel}")

    # Idempotency: a second pass over the produced text must be a no-op.
    bad = [rel for rel, _, b in edits if apply_rules(b, rs) != b]
    if bad:
        print(f"\n!! NOT IDEMPOTENT -- a second pass would change: {bad}")
        return 1
    print("\nidempotent: a second pass changes nothing.")

    if not args.apply:
        print("\n(dry run -- pass --apply to write)")
        return 0

    for rel, _, b in edits:
        (REPO / rel).write_text(b, encoding="utf-8", newline="\n")
    for a, b in renames:
        (REPO / b).parent.mkdir(parents=True, exist_ok=True)
        git("mv", a, b)
    print(f"\napplied: {len(edits)} edit(s), {len(renames)} rename(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
