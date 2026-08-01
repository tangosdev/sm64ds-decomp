"""Split a large `src/` change into PR-sized branches the validate bot can chew.

`validate` compiles every changed `src/*.c|*.cpp` on the build box and compares relocated
bytes to the ROM. It is polled for at most ~20 minutes (`.github/workflows/pr-validate.yml`
`120 * 10s`), so a batch has to finish inside that. Roughly 150-200 files is the working
size.

The split follows three rules, in priority order:

  1. **One transformation per PR.** The rationale for a change is what a human reviews;
     the per-file table validate posts is what checks it. Mixing a behavioural fix in with
     a mechanical rename means the reviewer has to evaluate two arguments at once.
  2. **Disjoint file sets.** Every file belongs to exactly one branch, each branched from
     the base, so batches can be opened in any order and never conflict.
  3. **Grouped by module, then chunked.** A red batch is then diagnosable and lines up
     with how CLAIMS.md carves work. Small modules are packed together to avoid a long
     tail of tiny PRs.

Nothing is pushed. Branches are created locally; the `gh pr create` commands are printed
for you to run.

Usage:
    python tools/split_prs.py                     # show the plan
    python tools/split_prs.py --create            # create the local branches
    python tools/split_prs.py --max-files 150
"""
import argparse
import collections
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"

# Which commit introduced which kind of change, and how to describe it in a PR.
TRANSFORMS = [
    ("fixes", "Fix wrong relocation destinations",
     """These change where a relocation points, not just what a name is spelled.
Each was found by linking the function into the ROM and comparing the linked bytes:
`tools/match.py` wildcards every relocated word, so a call to the wrong function with the
right shape passes the byte gate. Each file still byte-matches after the fix."""),
    ("names", "Make each file define the symbol its filename claims",
     """Each file carried `// @symbol X` and `// @emits Y` with X != Y, so it defined a
different symbol than its name. Nothing in the repo consumes either annotation, and
`match.py --func <stem>` reports "symbol not found in object" — meaning the byte oracle
could not verify these files at all, while `progress.py` counted them as matched because
a `src/<name>.c` existed. The emitted function is renamed to the `@symbol` name and the
recovered name kept as `// recovered name: Y`. Produced by `tools/reconcile_names.py`,
byte-verified per file."""),
    ("blind", "Resolve references to symbols that carry no address",
     """Each file referenced a name `config/**/symbols.txt` does not define — the BLIND
matches of `notes/link-verification.md`. The destination is recoverable: the function
byte-matches, so the ROM's own linked word at the relocation slot is the answer. Decoded
per file and per relocation, which matters because placeholders like `G0` mean a
different address in almost every file. Produced by `tools/resolve_blind.py`,
byte-verified per file."""),
]

SYMRE = re.compile(r"^(\S+)\s+kind:function")


def module_of_stems():
    out = {}
    for sp in sorted(CONFIG.rglob("symbols.txt")):
        rel = sp.parent.relative_to(CONFIG).as_posix()
        label = "arm9" if rel == "arm9" else rel.split("/")[-1]
        for line in sp.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = SYMRE.match(line)
            if m:
                out.setdefault(m.group(1), label)
    return out


def git(*a, check=True):
    r = subprocess.run(["git", *a], capture_output=True, text=True, cwd=REPO)
    if check and r.returncode != 0:
        sys.exit(f"git {' '.join(a)} failed:\n{r.stderr}")
    return r.stdout.strip()


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--base", default="origin/main")
    ap.add_argument("--max-files", type=int, default=175)
    ap.add_argument("--prefix", default="rombuild")
    ap.add_argument("--create", action="store_true", help="create the local branches")
    args = ap.parse_args()

    head = git("rev-parse", "HEAD")
    changed = [f for f in git("diff", "--name-only", args.base, "HEAD", "--", "src/").splitlines() if f]
    if not changed:
        sys.exit("no src/ changes vs " + args.base)

    # Attribute each file to the transformation that last touched it, so the sets are
    # disjoint and each branch carries that file's final content. Walk commits
    # newest-first; the first one that touched a file owns it.
    owner = {}

    def kind_of(subject):
        s = subject.lower()
        if "vtable" in s or "repoint" in s or "wrong relocation" in s:
            return "fixes"
        if "reconcile" in s or "filename" in s:
            return "names"
        if "blind" in s:
            return "blind"
        if "source-built" in s or "compile" in s:
            return "fixes"   # the 9 repoints rode with a coverage commit
        return None

    for line in git("log", "--format=%H\x01%s", f"{args.base}..HEAD").splitlines():
        sha, subject = line.split("\x01", 1)
        k = kind_of(subject)
        if not k:
            continue
        for f in git("show", "--name-only", "--format=", sha, "--", "src/").splitlines():
            if f and f not in owner:
                owner[f] = k

    # A file changed vs the base that NONE of these commits touched came from something
    # else on this branch - here, the fader-hierarchy .c -> .cpp promotions that arrived
    # via the merge. That is separate work with its own review; it must not ride along.
    foreign = [f for f in changed if f not in owner]
    if foreign:
        print(f"excluding {len(foreign)} file(s) changed by other work on this branch "
              f"(e.g. {pathlib.Path(foreign[0]).name})\n")
        changed = [f for f in changed if f in owner]

    mods = module_of_stems()
    plan = []
    for key, title, body in TRANSFORMS:
        files = sorted(f for f in changed if owner.get(f) == key)
        if not files:
            continue
        bymod = collections.defaultdict(list)
        for f in files:
            bymod[mods.get(pathlib.Path(f).stem, "misc")].append(f)
        # Big modules get their own batches; small ones are packed together.
        batches, cur, curmods, pool = [], [], [], []
        for mod, fs in sorted(bymod.items(), key=lambda kv: -len(kv[1])):
            if len(fs) >= args.max_files:
                # Full batches for the bulk; the remainder joins the packing pool rather
                # than becoming a lopsided 20-file PR of its own.
                whole = (len(fs) // args.max_files) * args.max_files
                for i in range(0, whole, args.max_files):
                    batches.append(([mod], fs[i:i + args.max_files]))
                if fs[whole:]:
                    pool.append((mod, fs[whole:]))
                continue
            pool.append((mod, fs))
        for mod, fs in sorted(pool, key=lambda kv: -len(kv[1])):
            if len(cur) + len(fs) > args.max_files and cur:
                batches.append((curmods, cur))
                cur, curmods = [], []
            cur.extend(fs)
            curmods.append(mod)
        if cur:
            batches.append((curmods, cur))
        for i, (bmods, fs) in enumerate(batches, 1):
            plan.append((key, title, body, i, len(batches), bmods, fs))

    print(f"base: {args.base}   changed src files: {len(changed)}   max per PR: {args.max_files}")
    print(f"total PRs: {len(plan)} (plus one for tools/config/notes, which must land first)\n")
    for (key, title, _body, i, n, bmods, fs) in plan:
        label = bmods[0] if len(bmods) == 1 else f"{len(bmods)} modules"
        print(f"  {args.prefix}/{key}-{i:02d}  {len(fs):4d} files  {label:22s} {title}")

    if not args.create:
        print("\n(plan only — pass --create to make the local branches)")
        return

    start = git("rev-parse", "--abbrev-ref", "HEAD")
    made = []
    for (key, title, body, i, n, bmods, fs) in plan:
        br = f"{args.prefix}/{key}-{i:02d}"
        git("checkout", "-q", "-B", br, args.base)
        # Take each file's final content from the working branch.
        subprocess.run(["git", "checkout", head, "--", *fs], cwd=REPO, check=True,
                       capture_output=True)
        label = bmods[0] if len(bmods) == 1 else f"{len(bmods)} modules"
        msg = (f"{title} ({len(fs)} files, {label}) [{i}/{n}]\n\n{body}\n\n"
               f"Batch {i} of {n} for this transformation; the batches are disjoint and each\n"
               f"branches from {args.base}, so they can be reviewed in any order.\n\n"
               "Co-Authored-By: Claude Opus 5 <noreply@anthropic.com>\n"
               "Claude-Session: https://claude.ai/code/session_01GzZipJbjwrhPze4qsoKGyi\n")
        subprocess.run(["git", "commit", "-q", "-m", msg], cwd=REPO, check=True,
                       capture_output=True)
        made.append((br, title, len(fs)))
    git("checkout", "-q", start)

    print(f"\ncreated {len(made)} local branches. Nothing pushed. To open them:\n")
    for (br, title, n) in made:
        print(f"  git push -u origin {br} && gh pr create --base main --head {br} \\\n"
              f"    --title \"{title} ({n} files)\" --body-file -")


if __name__ == "__main__":
    main()
