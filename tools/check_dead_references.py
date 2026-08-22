"""Does the tree's PROSE name files that no longer exist?

    python tools/check_dead_references.py            # gate: fail on NEW dead references
    python tools/check_dead_references.py --list      # print every dead reference found
    python tools/check_dead_references.py --update    # re-bank the baseline

WHY THIS EXISTS
---------------
Renames in this tree are heavy -- `Actor` -> `dActor_c`, `MeshCollider` -> `dBgW_Kc`,
`ClsnResult` -> `dBgPi`, and dozens more -- and a rename moves the FILE while leaving
every sentence that names it untouched. Nothing in the build reads a docstring, so a
citation to a header like `Enemy.h` survives indefinitely after that header stops
existing. Three of those were found by hand in two days (`check_header_offsets.py`'s
usage line, `check_src_tu.py`'s "does not compile" list, and a whole family of
`plan-gen-header.md` citations that had outlived that note's move into the notes
archive), and each one had already been repeated as fact by a reader who trusted it.

This is a REFERENCE-INTEGRITY gate for documentation, not for code. `check_references.py`
covers dangling code symbols; this covers the sentences around them.

WHAT IT CHECKS
--------------
Prose surfaces only:
  - `tools/**/*.py`   module/function/class docstrings, `#` comments, and the strings
                      passed to argparse as help/epilog/description/usage
  - `notes/**/*.md`, top-level `*.md`, `docs/**/*.md`, `port/docs/*.md`
  - `.github/workflows/*.yml`  comments and `name:` / `run:` lines
  - `.claude/skills/**/*.md`

In those it finds repo-rooted path references -- a token whose first segment is a real
top-level directory of this repo (`tools/`, `notes/`, `include/`, `src/`, ...) -- and
asks whether the path resolves in the working tree.

HOW IT AVOIDS CRYING WOLF
-------------------------
Two mechanisms, because a noisy gate gets switched off and then protects nothing:

1. Generated and gitignored roots (`build/`, `extracted/`, `progress/`) are skipped
   outright -- a reference to `build/tu_map.json` is correct even on a fresh clone
   where the file is absent.

2. Everything else is measured against `config/dead-reference-baseline.json`, the same
   ratchet shape this repo already uses for byte, layout and langmode known-issues.
   Only a reference that is dead AND not in the baseline fails the run. That admits
   the whole existing false-positive population -- synthetic example paths in test
   fixtures (`src/f.c`), paths belonging to other repositories (`include/nitro/...`,
   the n64 `src/game/mario.c` cross-reference), paths that live only on an unlanded
   branch, and prose that deliberately names something historical -- without anyone
   having to enumerate those categories by pattern.

FAILING LOUDLY RATHER THAN SKIPPING GREEN
-----------------------------------------
This tree has a documented history of gates that pass by doing nothing (an Allman
reformat took `check_header_offsets` from 5 matches to 0 and printed a pass). So this
tool refuses to report success unless the scan actually happened: if it inspects fewer
than `MIN_FILES` prose files or extracts fewer than `MIN_REFS` candidate references, it
exits non-zero with SCAN TOO SMALL rather than announcing a clean tree. Those floors sit
far below today's numbers; they trip when the extractor breaks, not when prose changes.
"""
import argparse
import ast
import io
import json
import os
import pathlib
import re
import sys
import tokenize

REPO = pathlib.Path(__file__).resolve().parent.parent
BASELINE = REPO / "config" / "dead-reference-baseline.json"

# The scan must be at least this big or the tool is broken, not the tree.
MIN_FILES = 150
MIN_REFS = 1000

# Directories that are real parts of the repo layout. A path reference only counts
# when its first segment is one of these -- that is what separates `tools/foo.py`
# from prose like `if/else`, `106/106` or `ctor/dtor`.
TOPDIRS = {"tools", "notes", "src", "src_tu", "include", "config", "config_tu",
           "docs", "port", "mods", "audit", "symbols", "nearmiss", ".github",
           "asm", "assets", "lib"}

# Gitignored or generated at build time: absent from a clean checkout by design.
GENERATED = {"build", "extracted", "progress"}

SKIP_DIRS = {".git", "build", "extracted", "__pycache__", ".mypy_cache",
             ".pytest_cache", "node_modules", ".venv", "venv"}

PATH_RE = re.compile(r"(?<![\w./\\-])((?:[A-Za-z0-9_.\-]+/)+[A-Za-z0-9_.\-]+)(?![\w-])")
# Templates, globs and format placeholders are not paths anyone can resolve.
GLOBBY = re.compile(r"[*?\[\]{}<>]|\.\.\.|%s|\$\(|::")
TRAILING = ".,;:)\"'`!?"
HELP_KWARGS = {"help", "epilog", "description", "usage"}


# --------------------------------------------------------------------------- scan

def _prose_targets():
    out = []
    for base, dirs, files in os.walk(REPO):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        rel_base = pathlib.Path(base).relative_to(REPO).as_posix()
        for f in files:
            rel = f if rel_base == "." else f"{rel_base}/{f}"
            if rel.startswith("tools/") and rel.endswith(".py"):
                out.append(rel)
            elif rel.startswith("notes/") and rel.endswith(".md"):
                out.append(rel)
            elif rel.startswith(("docs/", "port/docs/")) and rel.endswith(".md"):
                out.append(rel)
            elif rel.startswith(".claude/skills/") and rel.endswith(".md"):
                out.append(rel)
            elif rel.startswith(".github/workflows/") and rel.endswith((".yml", ".yaml")):
                out.append(rel)
            elif "/" not in rel and rel.endswith(".md"):
                out.append(rel)
    return sorted(out)


def _py_prose(text):
    """(lineno, snippet) for docstrings, comments and argparse help in a python file."""
    try:
        for tok in tokenize.generate_tokens(io.StringIO(text).readline):
            if tok.type == tokenize.COMMENT:
                yield tok.start[0], tok.string
    except (tokenize.TokenError, IndentationError, SyntaxError):
        pass
    try:
        tree = ast.parse(text)
    except SyntaxError:
        return
    for node in ast.walk(tree):
        if isinstance(node, (ast.Module, ast.FunctionDef, ast.AsyncFunctionDef,
                             ast.ClassDef)):
            doc = ast.get_docstring(node, clean=False)
            if doc:
                ln = node.body[0].lineno if node.body else getattr(node, "lineno", 1)
                yield ln, doc
        elif isinstance(node, ast.Call):
            for kw in node.keywords:
                if (kw.arg in HELP_KWARGS and isinstance(kw.value, ast.Constant)
                        and isinstance(kw.value.value, str)):
                    yield kw.value.lineno, kw.value.value


def _yaml_prose(text):
    for i, line in enumerate(text.splitlines(), 1):
        stripped = line.strip()
        if stripped.startswith("#") or "name:" in stripped or "run:" in stripped:
            yield i, line


def _text_prose(text):
    for i, line in enumerate(text.splitlines(), 1):
        yield i, line


def _normalise(ref):
    ref = ref.strip().rstrip(TRAILING).lstrip("(\"'`[").replace("\\", "/")
    return ref.lstrip("./").rstrip("/")


def collect(root=REPO):
    """[(file, line, ref)] for every repo-rooted path reference in prose."""
    refs = []
    files = _prose_targets()
    for rel in files:
        text = (root / rel).read_text(encoding="utf-8", errors="replace")
        if rel.endswith(".py"):
            regions = _py_prose(text)
        elif rel.endswith((".yml", ".yaml")):
            regions = _yaml_prose(text)
        else:
            regions = _text_prose(text)
        for lineno, snippet in regions:
            for m in PATH_RE.finditer(snippet):
                ref = _normalise(m.group(1))
                if not ref or "/" not in ref:
                    continue
                head = ref.split("/", 1)[0]
                if head in GENERATED or head not in TOPDIRS:
                    continue
                if GLOBBY.search(ref):
                    continue
                refs.append((rel, lineno, ref))
    return files, refs


def _tree_paths(root=REPO):
    paths = set()
    for base, dirs, names in os.walk(root):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        rel_base = pathlib.Path(base).relative_to(root).as_posix()
        if rel_base != ".":
            paths.add(rel_base)
        for n in names:
            paths.add(n if rel_base == "." else f"{rel_base}/{n}")
    return paths


def dead_references(root=REPO):
    """(files_scanned, refs_seen, sorted [(file, ref)] that do not resolve)."""
    files, refs = collect(root)
    paths = _tree_paths(root)
    dead = set()
    for rel, _lineno, ref in refs:
        if ref in paths:
            continue
        if any(p.endswith("/" + ref) for p in paths):
            continue
        dead.add((rel, ref))
    return files, refs, sorted(dead)


# ------------------------------------------------------------------------ baseline

def load_baseline():
    if not BASELINE.exists():
        return set()
    data = json.loads(BASELINE.read_text(encoding="utf-8"))
    return {(e["file"], e["ref"]) for e in data["known"]}


def write_baseline(dead):
    BASELINE.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "_comment": (
            "Dead path references in prose that are known and accepted: synthetic "
            "example paths in tests, paths belonging to other repositories, paths "
            "that exist only on an unlanded branch, and prose that deliberately "
            "names something historical. tools/check_dead_references.py fails on any "
            "dead reference NOT listed here. Regenerate with --update, but read the "
            "diff: a new entry is usually a rename someone forgot to follow through "
            "into the docs."),
        "known": [{"file": f, "ref": r} for f, r in sorted(dead)],
    }
    BASELINE.write_text(json.dumps(payload, indent=1) + "\n",
                        encoding="utf-8", newline="\n")


# ---------------------------------------------------------------------------- cli

def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--list", action="store_true",
                    help="print every dead reference, baselined or not")
    ap.add_argument("--update", action="store_true",
                    help="re-bank config/dead-reference-baseline.json")
    args = ap.parse_args(argv)

    files, refs, dead = dead_references()

    # A scan that inspected nothing must not report a clean tree.
    if len(files) < MIN_FILES or len(refs) < MIN_REFS:
        print(f"check_dead_references: SCAN TOO SMALL -- {len(files)} prose file(s), "
              f"{len(refs)} path reference(s); expected at least {MIN_FILES} and "
              f"{MIN_REFS}.")
        print("  The extractor is broken, or the tool is being run outside the repo.")
        print("  Refusing to report a pass on a scan this size.")
        return 2

    print(f"check_dead_references: {len(files)} prose file(s), {len(refs)} repo-rooted "
          f"path reference(s), {len(dead)} that do not resolve")

    if args.list:
        for f, r in dead:
            print(f"  {f}: {r}")

    if args.update:
        write_baseline(dead)
        print(f"  wrote {BASELINE.relative_to(REPO).as_posix()} "
              f"({len(dead)} known reference(s))")
        return 0

    known = load_baseline()
    new = [d for d in dead if d not in known]
    healed = sorted(known - set(dead))

    if healed:
        print(f"  {len(healed)} baselined reference(s) now resolve -- "
              f"run --update to shrink the baseline (not a failure)")

    if not new:
        print("  no new dead references")
        return 0

    print(f"\nFAIL: {len(new)} prose reference(s) name a path that does not exist:\n")
    for f, r in new:
        print(f"  {f}\n      names `{r}`, which is not in the tree")
    print("\nThis is almost always a rename that did not carry into the prose. Fix the")
    print("sentence to name the file's current path -- or, if the reference is")
    print("deliberately historical or belongs to another repository, add it with")
    print("`python tools/check_dead_references.py --update`.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
