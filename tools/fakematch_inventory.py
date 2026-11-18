#!/usr/bin/env python3
"""Count the fake-matching devices left in src/ (issue #821), per git ref.

A fake match is source that reproduces the ROM bytes by forcing the compiler
rather than by reading like the code EAD wrote: optimiser pragmas wrapped around
one function, volatile pins, cast-through-address launders, `(char *)this + off`
member access, and so on. This tool only COUNTS candidates by pattern; it does
not decide which are fake. Some hits are genuine (MMIO `volatile`, a file-wide
SDK build setting, a varargs pun) -- the per-site classification lives in the
review of each cleanup round, not here.

Counts are over non-library src/*.c and src/*.cpp, with comments stripped, so a
"Kept (#821 rN): ..." note that mentions `volatile` does not count as one.

    python tools/fakematch_inventory.py                     # HEAD
    python tools/fakematch_inventory.py main=origin/main pr=HEAD
    python tools/fakematch_inventory.py --top 10 HEAD       # hottest files per category
    python tools/fakematch_inventory.py --json out.json HEAD

Reads blobs straight from git (`git cat-file --batch`), so any ref works without
a checkout and the working tree is never consulted.
"""
import argparse
import json
import re
import subprocess
import sys

# name -> (regex, one-line meaning). Applied with re.M to comment-stripped text.
CATEGORIES = {
    "volatile": (r"\bvolatile\b",
                 "volatile anywhere in code: pins, fill-value spills, and genuine MMIO"),
    "pragma": (r"#pragma\s+(?:opt|optimiz|auto_inline|dont_inline|scheduling|peephole)",
               "optimiser pragmas: per-function pins and file-wide build settings"),
    "charthis": (r"\(\s*(?:char|u8)\s*\*\s*\)\s*this\b",
                 "(char *)this + offset instead of a named member"),
    "v3cast": (r"\(\s*(?:const\s+)?Vector3\s*\*\s*\)\s*&",
               "(Vector3 *)&x: a field triple or POD vector viewed as a Vector3"),
    "launder": (r"\*\(\s*[A-Za-z_]\w*(?:\s*\*)?\s*\*\s*\)\s*&",
                "*(T *)&x: a cast-through-address read or write"),
    "selfthis": (r"\w+\s*\*\s*self\s*=\s*(?:\(\w+\s*\*\)\s*)?this\s*;",
                 "T *self = this; alias inside a member function"),
    "pmfext": (r"^\s*extern [^(;]*\bpmf_",
               "extern declarations of hand-built pointer-to-member literals"),
}

_COMMENT = re.compile(r"//[^\n]*|/\*.*?\*/|\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'", re.S)


def strip_comments(text):
    """Blank out comments, keeping string/char literals and line structure."""
    def repl(m):
        s = m.group(0)
        if s.startswith(("//", "/*")):
            return re.sub(r"[^\n]", " ", s)
        return s
    return _COMMENT.sub(repl, text)


def is_counted(path):
    """Game and engine sources only: src/lib/ holds the toolchain's MSL and C++
    runtime, which are library code and out of scope for #821."""
    return (path.startswith("src/") and path.endswith((".c", ".cpp"))
            and not path.startswith("src/lib/"))


def count_text(text):
    code = strip_comments(text)
    return {k: len(re.findall(rx, code, re.M)) for k, (rx, _) in CATEGORIES.items()}


def _git(repo, *args, data=None):
    r = subprocess.run(["git", "-C", repo, *args], input=data, capture_output=True)
    if r.returncode:
        sys.exit("git %s failed: %s" % (" ".join(args), r.stderr.decode("utf-8", "replace").strip()))
    return r.stdout


def inventory(repo, ref):
    """Return ({category: total}, {category: {path: n}}) for one ref."""
    names = _git(repo, "ls-tree", "-r", "--name-only", ref, "--", "src").decode("utf-8", "replace")
    files = [f for f in names.split("\n") if f and is_counted(f)]
    totals = dict.fromkeys(CATEGORIES, 0)
    per_file = {k: {} for k in CATEGORIES}
    if not files:
        return totals, per_file
    blob = _git(repo, "cat-file", "--batch",
                data="".join("%s:%s\n" % (ref, f) for f in files).encode())
    i = 0
    for f in files:
        nl = blob.index(b"\n", i)
        header = blob[i:nl].split()
        size = int(header[2])
        body = blob[nl + 1:nl + 1 + size].decode("utf-8", "replace")
        i = nl + 2 + size
        for k, n in count_text(body).items():
            if n:
                totals[k] += n
                per_file[k][f] = n
    return totals, per_file


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("refs", nargs="*", default=["HEAD"],
                    help="git refs, optionally labelled as label=ref (default: HEAD)")
    ap.add_argument("--repo", default=".", help="repository root (default: .)")
    ap.add_argument("--top", type=int, default=0, metavar="N",
                    help="also list the N files with the most hits per category (last ref)")
    ap.add_argument("--json", metavar="PATH", help="write totals and per-file counts as JSON")
    ap.add_argument("--legend", action="store_true", help="print what each category means")
    a = ap.parse_args(argv)

    if a.legend:
        for k, (_, meaning) in CATEGORIES.items():
            print("%-10s %s" % (k, meaning))
        print()

    rows = []
    for spec in a.refs:
        label, _, ref = spec.partition("=") if "=" in spec else (spec, "", spec)
        totals, per_file = inventory(a.repo, ref)
        rows.append({"label": label, "ref": ref, "totals": totals, "files": per_file})

    width = max([len(r["label"]) for r in rows] + [3]) + 2
    print("%-*s" % (width, "ref") + "".join("%10s" % k for k in CATEGORIES))
    for r in rows:
        print("%-*s" % (width, r["label"]) + "".join("%10d" % r["totals"][k] for k in CATEGORIES))

    if a.top and rows:
        last = rows[-1]
        for k in CATEGORIES:
            hot = sorted(last["files"][k].items(), key=lambda kv: (-kv[1], kv[0]))[:a.top]
            if hot:
                print("\n%s (%d in %d files)" % (k, last["totals"][k], len(last["files"][k])))
                for path, n in hot:
                    print("  %4d  %s" % (n, path))

    if a.json:
        with open(a.json, "w", encoding="utf-8", newline="\n") as fh:
            json.dump(rows, fh, indent=1, sort_keys=True)
            fh.write("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
