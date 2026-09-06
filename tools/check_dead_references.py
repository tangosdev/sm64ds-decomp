"""Does the tree's prose name files that no longer exist?

    python tools/check_dead_references.py            # gate: fail on NEW dead references
    python tools/check_dead_references.py --list      # print every dead reference found
    python tools/check_dead_references.py --update    # re-bank the prose baseline
    python tools/check_dead_references.py --update-code  # re-bank C/C++ comment debt

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

This is a REFERENCE-INTEGRITY gate for prose, including prose embedded in C/C++
comments. `check_references.py` covers dangling code symbols; this covers the sentences
around them.

WHAT IT CHECKS
--------------
Prose surfaces only:
  - `tools/**/*.py`   module/function/class docstrings, `#` comments, and the strings
                      passed to argparse as help/epilog/description/usage
  - `notes/**/*.md`, top-level `*.md`, `docs/**/*.md`, `port/docs/*.md`
  - `.github/workflows/*.yml`  comments and `name:` / `run:` lines
  - `.claude/skills/**/*.md`
  - comments in repository `.c`, `.cc`, `.cpp`, `.h`, and `.hpp` files

In those it finds repo-rooted path references -- a token whose first segment is a real
top-level directory of this repo (`tools/`, `notes/`, `include/`, `src/`, ...) -- and
asks whether the path resolves in the working tree.

AND, SEPARATELY, MARKDOWN LINKS THAT POINT AT THE WRONG PLACE
-------------------------------------------------------------
The scan above reads a path out of prose and resolves it FROM THE REPO ROOT. A markdown
link does not work that way: a renderer resolves `[x](config/<name>.json)` against the
directory of the file the link is written in. So a link inside `notes/` naming
`config/<name>.json` points at that same path UNDER `notes/` -- a 404 -- while the
repo-rooted scan resolves `config/<name>.json`, finds it, and reports a clean tree.

Not hypothetical. #2036 turned 106 bare paths in `notes/ead-debug-name-crossref.md`
into markdown links; 105 carried the `../` and the one at line 192 did not. All four of
that PR's checks were green. It was found by hand and fixed forward in #2049; the blind
spot itself is #2037, which `broken_links` below closes by resolving every markdown
link against `os.path.dirname()` of its own file.

FAIL-CLOSED, NO BASELINE. Measured over all 106 markdown prose files at `e6ede02d7`:
183 relative links, of which exactly ONE did not resolve -- `port/docs/opie-assessment.md`
naming `roadmap.md`, which lives at `notes/roadmap.md`. One offender is a fix, not a
ratchet, so it was corrected in the same change and this check carries no accepted-offender
list. The repo-rooted scan needs one because its false-positive population is large;
this one does not, and a baseline nobody needs is a place for real breakage to hide.

HOW IT AVOIDS CRYING WOLF
-------------------------
Two mechanisms, because a noisy gate gets switched off and then protects nothing:

1. Generated and gitignored roots (`build/`, `extracted/`, `progress/`) are skipped
   outright -- a reference to `build/tu_map.json` is correct even on a fresh clone
   where the file is absent.

2. Documentation debt is measured against `config/dead-reference-baseline.json`, while
   the larger pre-existing C/C++ comment debt has its own grouped ratchet at
   `tools/dead-code-reference-baseline.json`. Only a reference that is dead AND absent
   from both baselines fails. Keeping the comment ratchet separate matters: the familiar
   prose `--update` command cannot accidentally bank hundreds of source-comment entries.
   Exact `(citing file, referenced path)` pairs allow removals but prevent one cleanup
   from hiding one addition.

FAILING LOUDLY RATHER THAN SKIPPING GREEN
-----------------------------------------
This tree has a documented history of gates that pass by doing nothing (an Allman
reformat took `check_header_offsets` from 5 matches to 0 and printed a pass). So this
tool refuses to report success unless the scan actually happened: if it inspects fewer
than `MIN_FILES` prose files, extracts fewer than `MIN_REFS` candidate references, or
resolves fewer than `MIN_LINKS` markdown links, it exits non-zero with SCAN TOO SMALL
rather than announcing a clean tree. Those floors sit far below today's numbers; they
trip when the extractor breaks, not when prose changes.
"""
import argparse
import ast
import io
import json
import os
import pathlib
import re
import subprocess
import sys
import tokenize

REPO = pathlib.Path(__file__).resolve().parent.parent
BASELINE = REPO / "config" / "dead-reference-baseline.json"
CODE_BASELINE = REPO / "tools" / "dead-code-reference-baseline.json"

# The scan must be at least this big or the tool is broken, not the tree.
MIN_FILES = 150
MIN_REFS = 1000
# A broken C/C++ target selector must not quietly reduce the new coverage to zero. This
# is deliberately far below the current tree, so normal shard consolidation cannot trip
# it while removing files.
MIN_CODE_FILES = 1000
# 183 relative markdown links at e6ede02d7, 106 of them in one note. The floor sits
# below what survives that note being deleted outright, so it trips on a broken
# extractor and not on prose churn.
MIN_LINKS = 50

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
CODE_SUFFIXES = (".c", ".cc", ".cpp", ".h", ".hpp")

PATH_RE = re.compile(r"(?<![\w./\\-])((?:[A-Za-z0-9_.\-]+/)+[A-Za-z0-9_.\-]+)(?![\w-])")
# Templates, globs and format placeholders are not paths anyone can resolve.
GLOBBY = re.compile(r"[*?\[\]{}<>]|\.\.\.|%s|\$\(|::")
TRAILING = ".,;:)\"'`!?"
HELP_KWARGS = {"help", "epilog", "description", "usage"}

# NUL separator for `git ... -z` pipes; spelled this way so no editor or heredoc
# can turn it into a literal control byte in this source file.
NUL = bytes([0])


# --------------------------------------------------------------------------- scan

def _prose_targets(root=None):
    root = REPO if root is None else pathlib.Path(root)
    out = []
    for base, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        rel_base = pathlib.Path(base).relative_to(root).as_posix()
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
            elif rel.endswith(CODE_SUFFIXES):
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


def _c_prose(text):
    """Yield comments from C/C++ without mistaking strings or includes for prose.

    This is a small lexer rather than a comment regex: URLs and path-shaped strings are
    common in host code, and `//` or `/*` inside them must not start a comment. Raw C++
    strings are skipped as one token for the same reason.
    """
    i = 0
    line = 1
    size = len(text)
    while i < size:
        ch = text[i]
        if ch == "\n":
            line += 1
            i += 1
            continue

        if text.startswith("//", i):
            end = text.find("\n", i + 2)
            if end < 0:
                end = size
            yield line, text[i + 2:end]
            i = end
            continue

        if text.startswith("/*", i):
            i += 2
            while i < size:
                end = text.find("*/", i)
                stop = size if end < 0 else end
                segment = text[i:stop]
                parts = segment.split("\n")
                for offset, part in enumerate(parts):
                    yield line + offset, part
                line += len(parts) - 1
                if end < 0:
                    i = size
                    break
                i = end + 2
                break
            continue

        # C++ raw string: R"delimiter(contents)delimiter". Prefixes such as u8R are
        # harmless because the scanner reaches the R before the quote.
        if text.startswith('R"', i):
            open_paren = text.find("(", i + 2, min(size, i + 20))
            if open_paren >= 0:
                delimiter = text[i + 2:open_paren]
                if not re.search(r"[\s\\()]", delimiter):
                    close = ")" + delimiter + '"'
                    end = text.find(close, open_paren + 1)
                    if end >= 0:
                        token_end = end + len(close)
                        line += text.count("\n", i, token_end)
                        i = token_end
                        continue

        if ch in ('"', "'"):
            quote = ch
            i += 1
            while i < size:
                if text[i] == "\\":
                    if i + 1 < size and text[i + 1] == "\n":
                        line += 1
                    i += 2
                    continue
                if text[i] == quote:
                    i += 1
                    break
                if text[i] == "\n":
                    # Invalid/uncontinued literals should not hide the rest of a file.
                    line += 1
                    i += 1
                    break
                i += 1
            continue

        i += 1


def _normalise(ref):
    ref = ref.strip().rstrip(TRAILING).lstrip("(\"'`[").replace("\\", "/")
    return ref.lstrip("./").rstrip("/")


def collect(root=REPO):
    """[(file, line, ref)] for every repo-rooted path reference in prose/comments."""
    refs = []
    files = _prose_targets(root)
    for rel in files:
        text = (root / rel).read_text(encoding="utf-8", errors="replace")
        if rel.endswith(".py"):
            regions = _py_prose(text)
        elif rel.endswith((".yml", ".yaml")):
            regions = _yaml_prose(text)
        elif rel.endswith(CODE_SUFFIXES):
            regions = _c_prose(text)
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
    """Paths PRESENT ON DISK. Worktree-dependent -- see `_git_ignored`."""
    paths = set()
    for base, dirs, names in os.walk(root):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        rel_base = pathlib.Path(base).relative_to(root).as_posix()
        if rel_base != ".":
            paths.add(rel_base)
        for n in names:
            paths.add(n if rel_base == "." else f"{rel_base}/{n}")
    return paths


def _git_tracked(root=REPO):
    """Paths git TRACKS, plus every ancestor directory of one.

    Deterministic where `_tree_paths` is not: identical in a wired worktree, a bare
    CI checkout and a `git archive` extraction.
    """
    out = subprocess.run(["git", "ls-files", "-z"], cwd=root,
                         capture_output=True)
    if out.returncode != 0:
        return set()
    paths = set()
    for raw in out.stdout.split(NUL):
        rel = raw.decode("utf-8", "replace").strip()
        if not rel:
            continue
        paths.add(rel)
        parts = rel.split("/")
        for i in range(1, len(parts)):
            paths.add("/".join(parts[:i]))
    return paths


def _git_ignored(candidates, root=REPO):
    """Which of `candidates` .gitignore covers -- ONE `git check-ignore` process.

    This is the load-bearing distinction. `tools/mwccarm/` (the pinned compiler),
    `tools/bin/` (dsd.exe) and `extracted/` (the ROM dump) are gitignored inputs that
    every wired worktree junctions in. Prose naming them is CORRECT -- the tools really
    do read those paths -- but git does not track them and a clean CI checkout does not
    have them on disk. Resolving against the filesystem alone therefore passes locally
    and fails in CI, which is exactly how this gate broke on its own first PR.

    Each candidate is asked TWICE, bare and with a trailing slash. `.gitignore` spells
    these rules `tools/mwccarm/`, and a directory-only pattern matches only when git
    knows the path is a directory -- which it infers from the filesystem. On a wired
    worktree the junction exists, so the bare form matches; on a clean CI checkout it
    does not, so the bare form MISSES and only `tools/mwccarm/` matches. Asking both is
    what makes the answer independent of whether the input happens to be present.

    `-v`, and a hit is kept only when git names a NON-EMPTY pattern. That is not
    decoration. On git 2.50.1.windows.1 a trailing-slash probe matches a BLANK LINE in
    .gitignore -- asking it about ANY path with a trailing slash answers with the line
    number of a blank line and an empty pattern -- so every probe came back ignored, `_git_ignored` returned the
    whole candidate set, and the gate reported 0 dead references on any tree. It was
    green locally and red in CI for months, which is how two stale paths landed. An
    empty pattern is never a real rule, so dropping it costs nothing and restores the
    answer git actually means.

    LIMIT, stated rather than papered over: `git check-ignore` answers from the ignore
    RULES, not from the filesystem, so it reports a path under `tools/mwccarm/` as
    ignored whether or not that file exists. A genuinely dead reference INSIDE an
    ignored directory is therefore invisible to this gate. That is not a fixable
    oversight: on a clean checkout there is no ground truth for what should exist under
    a directory whose contents were never committed. Checking the disk when it happens
    to be populated would restore the worktree-dependence this function exists to remove.
    """
    cands = sorted(set(candidates))
    if not cands:
        return set()
    probes = [c for c in cands] + [c + "/" for c in cands]
    # BYTES, and -z on both sides. With text=True Python rewrites \\n to \\r\\n
    # on Windows, git takes the CR as part of the filename, and every answer
    # comes back quoted with a stray carriage return -- so nothing matches and every
    # gitignored input looks dead again. -z sidesteps the newline question entirely.
    out = subprocess.run(["git", "check-ignore", "-v", "-z", "--stdin"], cwd=root,
                         input=NUL.join(c.encode("utf-8") for c in probes),
                         capture_output=True)
    # exit 0 = at least one ignored, 1 = none ignored; both are normal. Anything
    # else (128: not a repo) means we could not classify, so claim nothing.
    if out.returncode not in (0, 1):
        return set()
    # -v -z emits four NUL-separated fields per hit: source, line, pattern, pathname.
    fields = out.stdout.split(NUL)
    hits = set()
    for i in range(0, len(fields) - 3, 4):
        pattern = fields[i + 2].decode("utf-8", "replace").strip()
        if not pattern:
            continue          # a blank .gitignore line matches nothing; see the docstring
        name = fields[i + 3].decode("utf-8", "replace").strip().replace("\\", "/")
        if name:
            hits.add(name.rstrip("/"))
    return hits


def dead_references(root=REPO):
    """(files_scanned, refs_seen, sorted [(file, ref)] that do not resolve).

    A reference is ALIVE when it is tracked by git, OR present on disk, OR gitignored.
    The three cover different ground: tracked is deterministic across checkouts, on-disk
    catches untracked-but-real files, and gitignored covers the junctioned build inputs
    that are legitimately absent from a clean clone.
    """
    files, refs = collect(root)
    paths = _tree_paths(root) | _git_tracked(root)
    # Most references are repo-rooted and hit `paths` directly. The historical
    # basename-relative allowance used a full paths scan for every miss; grouping by
    # final component preserves that behavior without turning C/C++ coverage into an
    # O(references * repository-files) gate.
    suffix_candidates = {}
    for path in paths:
        suffix_candidates.setdefault(path.rsplit("/", 1)[-1], []).append(path)
    unresolved = set()
    for rel, _lineno, ref in refs:
        if ref in paths:
            continue
        if any(p.endswith("/" + ref)
               for p in suffix_candidates.get(ref.rsplit("/", 1)[-1], ())):
            continue
        unresolved.add((rel, ref))
    ignored = _git_ignored({ref for _rel, ref in unresolved}, root)
    dead = {(rel, ref) for rel, ref in unresolved if ref not in ignored}
    return files, refs, sorted(dead)


# ------------------------------------------------------------------- relative links

# `[text](target)`, and the reference-style `[label]: target` definition. The target
# group stops at the first `)`, which is why a target containing parentheses must be
# written in the angle-bracket form -- that is also markdown's own rule.
LINK_RE = re.compile(r"\[[^\]\n]*\]\(([^()\n]*)\)")
LINKDEF_RE = re.compile(r"^\s{0,3}\[[^\]\n]+\]:[ \t]*(\S+)")
# `https:`, `mailto:`, and anything else with a URL scheme is not ours to resolve.
SCHEME_RE = re.compile(r"^[A-Za-z][A-Za-z0-9+.\-]*:")
FENCE_RE = re.compile(r"^\s{0,3}(?:```|~~~)")
BACKTICKS_RE = re.compile(r"`+")
# `path "title"` / `path 'title'` -- markdown's optional link title.
TITLED_RE = re.compile(r"^(\S+)[ \t]+[\"'].*$")


def _code_spans(line):
    """(start, end) of every inline `code` span, so a link inside one is not a link.

    Needed because this tree writes ARM and C in prose constantly, and
    `(*(fn**)this)[N](this, ...)` in `notes/archive/pret-idioms.md` reads exactly like
    a markdown link to `this, ...`. Four such false positives came out of the tree-wide
    measurement; all four sit inside backticks. Runs are paired by LENGTH, as markdown
    itself pairs them, so ``a`b`` closes on the double tick and not the single.
    """
    runs = [(m.start(), m.end()) for m in BACKTICKS_RE.finditer(line)]
    spans, i = [], 0
    while i < len(runs):
        width = runs[i][1] - runs[i][0]
        j = i + 1
        while j < len(runs) and runs[j][1] - runs[j][0] != width:
            j += 1
        if j >= len(runs):
            break                 # unclosed run: everything after it is ordinary text
        spans.append((runs[i][0], runs[j][1]))
        i = j + 1
    return spans


def _link_target(raw):
    """The path part of a markdown link target: angle brackets and title stripped."""
    target = raw.strip()
    if target.startswith("<") and target.endswith(">"):
        return target[1:-1].strip()
    titled = TITLED_RE.match(target)
    return titled.group(1) if titled else target


def collect_links(root=REPO):
    """[(file, line, raw_target, resolved)] for every markdown link that names a path.

    MARKDOWN FILES ONLY. Link syntax means nothing in a python docstring or a workflow
    comment, and applying it there is actively harmful: a regex character class followed
    by a group -- `cand\\.c(?:pp)?`, `[a-z]+(...)` -- is indistinguishable from a link,
    and seven of the twelve hits in the first tree-wide pass were exactly that.

    Fenced blocks are skipped. The repo-rooted scan above does NOT skip them, and that
    difference is deliberate: a path named in a fenced `python tools/<x>.py` example is a
    real claim about the tree, whereas a link written inside a fence is being SHOWN, not
    followed. It costs nothing today either way -- all 183 links at `e6ede02d7` sit
    outside fences.
    """
    links = []
    for rel in _prose_targets(root):
        if not rel.endswith(".md"):
            continue
        text = (root / rel).read_text(encoding="utf-8", errors="replace")
        fenced = False
        for lineno, line in enumerate(text.splitlines(), 1):
            if FENCE_RE.match(line):
                fenced = not fenced
                continue
            if fenced:
                continue
            spans = _code_spans(line)
            found = [(m.group(1), m.start(1)) for m in LINK_RE.finditer(line)]
            definition = LINKDEF_RE.match(line)
            if definition:
                found.append((definition.group(1), definition.start(1)))
            for raw, col in found:
                if any(a <= col < b for a, b in spans):
                    continue
                target = _link_target(raw)
                # `#anchor` is same-page; a scheme is someone else's to resolve.
                if not target or target.startswith("#") or SCHEME_RE.match(target):
                    continue
                # A raw space cannot appear in a bare target -- it is prose we misread.
                if re.search(r"\s", target):
                    continue
                path = target.split("#", 1)[0].split("?", 1)[0].replace("\\", "/")
                if not path:
                    continue      # was `#frag` after all
                if path.startswith("/"):
                    resolved = os.path.normpath(path.lstrip("/"))
                else:
                    resolved = os.path.normpath(
                        os.path.join(os.path.dirname(rel), path))
                links.append((rel, lineno, target, resolved.replace("\\", "/")))
    return links


def broken_links(root=REPO, dead=()):
    """(links_seen, sorted [(file, line, target, resolved)] that resolve to nothing).

    Aliveness is judged exactly as `dead_references` judges it -- tracked by git, or on
    disk, or gitignored -- so `[x](../build/tu_map.json)` is fine on a clean clone for
    the same reason `build/tu_map.json` in a docstring is.

    `dead` is the repo-rooted scan's own findings. A link written `[x](../include/<Gone>.h)`
    is dead by both readings and would otherwise be printed twice, so anything the
    repo-rooted scan already owns for that file is left to it -- including the entries
    it has baselined. What survives is the case only this check can see: a link whose
    target names a REAL file by a path that is wrong from where the link is written,
    which is the #2036 shape.
    """
    links = collect_links(root)
    paths = _tree_paths(root) | _git_tracked(root)
    owned = set(dead)
    unresolved = []
    for rel, lineno, target, resolved in links:
        if (rel, _normalise(target)) in owned:
            continue
        if resolved in paths or resolved in (".", ""):
            continue
        # A directory target is fine when anything tracked lives under it.
        if not resolved.startswith("..") and any(
                p.startswith(resolved + "/") for p in paths):
            continue
        unresolved.append((rel, lineno, target, resolved))
    ignored = _git_ignored({r for _f, _l, _t, r in unresolved}, root)
    return links, sorted(b for b in unresolved if b[3] not in ignored)


# ------------------------------------------------------------------------ baseline

def load_baseline():
    known = set()
    for path in (BASELINE, CODE_BASELINE):
        if not path.exists():
            continue
        data = json.loads(path.read_text(encoding="utf-8"))
        entries = data["known"]
        if isinstance(entries, dict):
            known.update((file, ref)
                         for file, refs in entries.items() for ref in refs)
        else:
            known.update((e["file"], e["ref"]) for e in entries)
    return known


def _write_baseline(path, dead, comment):
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "_comment": comment,
        "known": [{"file": f, "ref": r} for f, r in sorted(dead)],
    }
    path.write_text(json.dumps(payload, indent=1) + "\n",
                    encoding="utf-8", newline="\n")


def write_baseline(dead):
    prose = {(f, r) for f, r in dead if not f.endswith(CODE_SUFFIXES)}
    _write_baseline(
        BASELINE,
        prose,
        (
            "Dead path references in prose that are known and accepted: synthetic "
            "example paths in tests, paths belonging to other repositories, paths "
            "that exist only on an unlanded branch, and prose that deliberately "
            "names something historical. tools/check_dead_references.py fails on any "
            "dead reference NOT listed here. Regenerate with --update, but read the "
            "diff: a new entry is usually a rename someone forgot to follow through "
            "into the docs."))
def write_code_baseline(dead):
    code = {(f, r) for f, r in dead if f.endswith(CODE_SUFFIXES)}
    grouped = {}
    for file, ref in sorted(code):
        grouped.setdefault(file, []).append(ref)
    payload = {
        "_comment": (
            "Dead repo-rooted paths already present in C/C++ comments when comment "
            "coverage was introduced. tools/check_dead_references.py allows these "
            "exact pairs, reports healed entries, and fails on additions. Regenerate "
            "only from a clean, pinned main checkout with --update-code; never use it "
            "to make a feature branch green."),
        "known": grouped,
    }
    CODE_BASELINE.write_text(json.dumps(payload, indent=1) + "\n",
                             encoding="utf-8", newline="\n")


# ---------------------------------------------------------------------------- cli

def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--list", action="store_true",
                    help="print every dead reference and broken link, baselined or not")
    updates = ap.add_mutually_exclusive_group()
    updates.add_argument("--update", action="store_true",
                         help="re-bank config/dead-reference-baseline.json only")
    updates.add_argument("--update-code", action="store_true",
                         help="re-bank tools/dead-code-reference-baseline.json only")
    args = ap.parse_args(argv)

    files, refs, dead = dead_references()
    links, broken = broken_links(REPO, dead)
    code_files = [f for f in files if f.endswith(CODE_SUFFIXES)]

    # A scan that inspected nothing must not report a clean tree.
    if (len(files) < MIN_FILES or len(code_files) < MIN_CODE_FILES
            or len(refs) < MIN_REFS or len(links) < MIN_LINKS):
        print(f"check_dead_references: SCAN TOO SMALL -- {len(files)} scan target(s), "
              f"{len(code_files)} C/C++ file(s), {len(refs)} path reference(s), "
              f"{len(links)} markdown link(s); expected at least {MIN_FILES}, "
              f"{MIN_CODE_FILES}, {MIN_REFS} and {MIN_LINKS}.")
        print("  The extractor is broken, or the tool is being run outside the repo.")
        print("  Refusing to report a pass on a scan this size.")
        return 2

    print(f"check_dead_references: {len(files)} scan target(s), {len(code_files)} C/C++ "
          f"file(s), {len(refs)} repo-rooted "
          f"path reference(s), {len(dead)} that do not resolve; {len(links)} markdown "
          f"link(s), {len(broken)} that do not resolve relative to their own file")

    if args.list:
        for f, r in dead:
            print(f"  {f}: {r}")
        for f, ln, target, resolved in broken:
            print(f"  {f}:{ln}: {target} -> {resolved}")

    if args.update:
        write_baseline(dead)
        print(f"  wrote {BASELINE.relative_to(REPO).as_posix()} "
              f"({len(dead)} known reference(s))")
        if broken:
            print(f"  NOTE: {len(broken)} broken markdown link(s) remain -- relative "
                  f"links have no baseline and must be fixed, not banked")
        return 1 if broken else 0

    if args.update_code:
        write_code_baseline(dead)
        code_dead = sum(1 for f, _r in dead if f.endswith(CODE_SUFFIXES))
        print(f"  wrote {CODE_BASELINE.relative_to(REPO).as_posix()} "
              f"({code_dead} known C/C++ comment reference(s))")
        return 1 if broken else 0

    known = load_baseline()
    new = [d for d in dead if d not in known]
    healed = sorted(known - set(dead))

    if healed:
        print(f"  {len(healed)} baselined reference(s) now resolve -- "
              f"run --update to shrink the baseline (not a failure)")

    if not new:
        print("  no new dead references")
    else:
        print(f"\nFAIL: {len(new)} prose reference(s) name a path that does not exist:\n")
        for f, r in new:
            print(f"  {f}\n      names `{r}`, which is not in the tree")
        print("\nThis is almost always a rename that did not carry into the prose. Fix the")
        print("sentence to name the file's current path -- or, if the reference is")
        print("deliberately historical or belongs to another repository, add it with")
        print("`python tools/check_dead_references.py --update`.")

    if not broken:
        print("  no broken markdown links")
        return 1 if new else 0

    print(f"\nFAIL: {len(broken)} markdown link(s) do not resolve from the file they "
          f"are written in:\n")
    for f, ln, target, resolved in broken:
        print(f"  {f}:{ln}\n      links to `{target}`, which resolves to "
              f"`{resolved}` -- not in the tree")
    print("\nA markdown link is resolved against the DIRECTORY OF ITS OWN FILE, not the")
    print("repo root, so a note in `notes/` must write `../config/x.json` and not")
    print("`config/x.json`. There is no baseline for these: fix the link.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
