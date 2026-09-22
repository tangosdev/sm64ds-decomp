"""Give ROM symbol declarations C linkage in C++ translation units.

A C++ TU that declares a ROM symbol like this:

    extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);

gets it mangled *again*, because the declaration has C++ linkage. mwccarm emits a
reference to `_Z35_ZN5Model8LoadFileER13SharedFilePtrPv`, which exists nowhere. Plain
names suffer the same way: `void Vec3_Sub(Vec3*, Vec3*, Vec3*)` becomes
`_Z8Vec3_SubP4Vec3S0_S0_`.

The file still byte-matches, which is why this survives. `match.py` compares the
relocated word as a wildcard, so the *name* a call resolves to never enters the byte
verdict -- only the full ROM link cares, and until a function is enrolled the link
never sees it either. So these read as finished work while referencing symbols that
do not exist.

## What this wraps, and how it decides

Not by pattern-matching declarations, which is guesswork. `eligible.py` already
records, per file, the exact undefined symbols that failed to resolve. A mangled ROM
declaration leaves a fingerprint that can be read back:

    _Z8Vec3_SubP4Vec3S0_S0_
     ^^ ^        ^
     |  |        `-- argument encoding, ignored
     |  `----------- 8 characters of name
     `-------------- length prefix

Decode the length prefix, take that many characters, and ask whether the result is a
symbol the ROM actually defines. If it is, the declaration wanted C linkage and did
not get it. That name -- and only that name -- is wrapped.

Three consequences worth stating, because they are what the earlier regex version got
wrong:

- **Data is never touched.** mwccarm does not mangle data references: `extern int
  data_x[]` emits `UND:data_x` from C++ with or without the guard, checked by
  compiling. Only function declarations can have this defect, so only they are
  considered.
- **A name absent from `symbols.txt` is left alone.** Wrapping it would not resolve
  anything; the reference is broken for a different reason and needs a human.
- **A file that already contains an `extern "C"` block is still processed.** Brace
  depth already prevents wrapping *inside* one. Skipping such files wholesale, as the
  first version did, hid most of the remaining work.

Every file is verified after the edit and reverted if it stops reproducing, so a
declaration that genuinely wanted C++ linkage cannot be broken silently. That check is
pinned to the compiler `tools/rombuild.py` will build the file with -- not a sweep of
every installed mwccarm, which blesses versions the link never runs (see
tools/build_pin.py) -- and it reads the relocation DESTINATION, because a byte compare
cannot see the very thing this tool changes.

Usage:
    python tools/extern_c_wrap.py                 # report only
    python tools/extern_c_wrap.py --apply
    python tools/extern_c_wrap.py --apply -j 16
    python tools/extern_c_wrap.py --apply --limit 128    # cap for the PR size gate
"""
import argparse
import collections
import concurrent.futures
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import build_pin as BP     # noqa: E402
import eligible as E       # noqa: E402
from enroll import candidates  # noqa: E402

ELIGIBILITY = REPO / "build" / "rombuild-eligibility.json"

# _Z<len><name><argument encoding>.  Nested-name forms (_ZN...) and special names
# (_ZTV...) do not carry a bare length prefix and are correctly not matched here:
# those are genuine C++ references, not a ROM name that got mangled a second time.
MANGLED = re.compile(r"^_Z(\d+)(.+)$")


def demangled_head(sym):
    """The ROM name inside a doubly-mangled symbol, or None."""
    m = MANGLED.match(sym)
    if not m:
        return None
    n, rest = int(m.group(1)), m.group(2)
    return rest[:n] if len(rest) >= n else None


def mangled_targets(known):
    """{path: {rom names that were mangled}} for files whose *every* missing symbol is
    a mangled ROM name. A file with any other unresolved reference is left out:
    wrapping would not make it enrollable, so touching it is churn."""
    if not ELIGIBILITY.exists():
        sys.exit(f"missing {ELIGIBILITY} -- run tools/eligible.py first")
    out = {}
    for r in E.load_report(ELIGIBILITY)[0]:
        if not (r.get("reason") or "").startswith("unresolvable"):
            continue
        # `missing`, not `reason` -- the reason string is truncated to three symbols
        # for display, and wrapping only those leaves a file with four still broken.
        syms = r.get("missing")
        if syms is None:
            sys.exit("eligibility report predates the `missing` field -- "
                     "re-run tools/eligible.py")
        heads = {s: demangled_head(s) for s in syms}
        if all(h and h in known for h in heads.values()):
            out[r["file"].replace("\\", "/")] = set(heads.values())
    return out


def is_cpp(path, text):
    return path.suffix == ".cpp" or text.startswith("//cpp")


def wrap(text, names):
    """Wrap file-scope declarations of `names` in extern "C". Returns None if nothing
    to do.

    Brace depth keeps this to file scope, which also means a declaration already
    inside an `extern "C" { ... }` block is never wrapped twice."""
    # A declaration of NAME is a statement that calls it out as a function: the
    # identifier followed by `(`. `= NAME(` or `return NAME(` would be a *use*, but
    # neither can appear at brace depth 0.
    pats = {n: re.compile(r"(^|[^\w])" + re.escape(n) + r"\s*\(") for n in names}
    lines = text.splitlines()
    out, run, depth, changed = [], [], 0, False

    def flush():
        nonlocal changed
        if run:
            out.append('extern "C" {')
            out.extend(run)
            out.append("}")
            run.clear()
            changed = True

    for ln in lines:
        s = ln.strip()
        decl = (depth == 0 and s.endswith(";") and "{" not in s
                and any(p.search(s) for p in pats.values()))
        if decl:
            run.append(ln)
            continue
        flush()
        out.append(ln)
        depth += ln.count("{") - ln.count("}")
    flush()
    return "\n".join(out) + "\n" if changed else None


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=10)
    ap.add_argument("--limit", type=int, help="stop after N files were wrapped")
    args = ap.parse_args()

    known = E.defined_symbols()
    targets_by_file = mangled_targets(known)

    info = {}
    for (d, name, rel, addr, size, sec) in candidates()[0]:
        info[rel.replace("\\", "/")] = (name, addr, size, d)

    # This tool exists because a reference resolves to the wrong name, and a byte
    # compare cannot see a name -- the relocated word is a wildcard. So the check that
    # decides whether a wrap worked has to be the one that reads the relocation's
    # DESTINATION. Without it the verification is blind to the only thing being changed.
    strict = None
    try:
        import reloc_audit as RA
        import relocs as RL
        strict = (RA, RA.build_name_index(), RA.build_config_relocs(), RL.load_all_syms())
    except Exception as e:                                          # noqa: BLE001
        print(f"  (reloc-destination check unavailable: {e}; verification is byte-only)")

    targets, skipped = [], collections.Counter()
    for rel, names in sorted(targets_by_file.items()):
        f = REPO / rel
        if not f.is_file():
            skipped["file missing"] += 1
            continue
        t = f.read_text(encoding="utf-8", errors="ignore")
        if not is_cpp(f, t):
            skipped["not a C++ TU"] += 1
            continue
        if wrap(t, names) is None:
            skipped["no file-scope declaration found"] += 1
            continue
        targets.append((f, names))

    print(f"files blocked only by mangled ROM declarations: {len(targets_by_file)}")
    print(f"  wrappable                                   : {len(targets)}")
    for k, v in skipped.most_common():
        print(f"  skipped, {k:<35}: {v}")
    if not args.apply:
        print("\n(report only -- pass --apply to wrap and byte-verify)")
        return 0

    def one(job):
        f, names = job
        rel = f.relative_to(REPO).as_posix()
        original = f.read_text(encoding="utf-8", errors="ignore")
        new = wrap(original, names)
        if new is None:
            return rel, "unchanged"
        f.write_text(new, encoding="utf-8", newline="\n")
        if rel not in info:
            return rel, "wrapped (not enrolled, unverified)"
        name, addr, size, d = info[rel]
        label = d.relative_to(REPO / "config").as_posix()
        label = "arm9" if label == "arm9" else label.split("/")[-1]
        # Pinned to the compiler tools/rombuild.py will build this file with. This
        # used to accept a match under any member of match.SWEEP, so a wrap could be
        # kept on the strength of a compiler the link never runs -- see
        # tools/build_pin.py. Anything the pinned build cannot answer is a revert.
        ok, why = BP.verify(f, name, addr, size, label, strict=strict)
        if ok:
            return rel, "wrapped"
        f.write_text(original, encoding="utf-8", newline="\n")
        return rel, f"reverted: {why}"

    if args.limit:
        targets = targets[:args.limit]
    counts = collections.Counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, verdict in ex.map(one, targets):
            # Bucket on the verdict, not on its explanation: every revert now carries
            # the reason it was rejected, and counting those separately would turn the
            # summary into one line per file.
            counts[verdict.split(":")[0]] += 1
            if verdict.startswith("reverted"):
                print(f"  {rel}: {verdict}")
    print()
    for k, v in counts.most_common():
        print(f"  {v:5d}  {k}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
