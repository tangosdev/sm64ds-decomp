#!/usr/bin/env python3
"""Delete launder idioms that cost nothing, one site at a time, verified against the ROM.

Three idioms were pasted through the tree as matching crutches:

    (EXPR & 0xFFFFFFFFFFFFFFFFULL)      MASK       -- the u64 identity mask
    (long long)(int)(EXPR)              ROUNDTRIP  -- widen-then-narrow cast pair
    (unsigned long long)(EXPR)          WIDEN      -- a lone 64-bit widening cast

Most of them do nothing: the compiler emits the same bytes without them. Some are
load-bearing, and no amount of reading tells you which -- one site in
`func_ov002_020f7020.c` carries a comment confidently explaining why it is required,
and the function matches without it. So this tool does not guess. It removes ONE
site, compiles the result under that file's own pin, and compares against the ROM
with relocation destinations checked. A site is deleted only when the bytes still
reproduce at that strict bar.

Granularity is the whole point. Measured on this tree, 1583 of 1707 sites in enrolled
files are free and 124 are not, but they do not separate by file: `func_ov002_020be3b0.c`
needs all 22 of its sites, `Camera::Behavior` needs 4 of 9, and `St_Climb_Main` -- which
a file-at-a-time probe calls load-bearing, because removing everything at once breaks it
-- turns out to need exactly 1 of its 12. Sites are tried greedily, each on top of the
previously accepted text, because removals interact. Per-file, per-site, in order.

Only ENROLLED files are considered. A file the ROM build does not compile has no
byte gate behind it, so "it still verifies" would prove nothing there.

Edits are textual deletions of whole tokens, with the enclosing parenthesis group
rebalanced, so they cannot change the parse of surrounding code. Comment and string
regions are masked out before sites are located: the idiom appears in prose too, and
prose is not the thing being fixed. Files whose comments still describe an idiom that
is now gone are reported separately -- that text is both false and, because the
langmode metric is a regex over source text, the reason the metric would not move.

    python tools/delaunder.py --dry-run
    python tools/delaunder.py --apply
    python tools/delaunder.py --apply --kinds MASK --limit 40 --jobs 8
"""

import argparse
import concurrent.futures
import json
import pathlib
import re
import sys
import threading

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

# The sweep needs the compiler and the extracted ROM; find_sites() and code_mask()
# need neither -- they are regex over text. langmode_audit.py imports this module for
# find_sites alone and runs in a CI job that installs no build dependencies, so a hard
# import here would make a static metric fail on `No module named 'elftools'`. Import
# what the sweep needs when the sweep actually runs.
try:
    import build_pin as BP           # noqa: E402
    import enroll                    # noqa: E402
    import reloc_audit as RA         # noqa: E402
    import relocs as RL              # noqa: E402
    import rombuild_check as RC      # noqa: E402
except ImportError as exc:           # pragma: no cover - exercised by the ratchet job
    BP = enroll = RA = RL = RC = None
    _SWEEP_IMPORT_ERROR = exc
else:
    _SWEEP_IMPORT_ERROR = None


def require_sweep_deps():
    """Fail loudly, and only for the paths that genuinely need a build environment."""
    if _SWEEP_IMPORT_ERROR is not None:
        raise SystemExit(
            f"tools/delaunder.py needs the build-side modules to sweep: "
            f"{_SWEEP_IMPORT_ERROR}. find_sites() alone does not."
        )

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"

MASK_RE = re.compile(r"&\s*0x[fF]{16}(?:[uUlL]+)?")
ROUNDTRIP_RE = re.compile(
    r"\(\s*(?:long\s+long|s64|u64|unsigned\s+long\s+long)\s*\)\s*"
    r"\(\s*(?:int|s32|u32|unsigned|unsigned\s+int)\s*\)"
)
WIDEN_RE = re.compile(r"\(\s*(?:long\s+long|s64|u64|unsigned\s+long\s+long)\s*\)(?=\s*\()")

KINDS = ("MASK", "ROUNDTRIP", "WIDEN", "PARENS")

# Removing a cast or a mask leaves its parentheses behind: `(((self + 0xb0)))`. That is
# not much of a readability win, so PARENS collapses a group whose entire content is
# itself one balanced group. It is skipped when the outer `(` follows an identifier
# character, because there the parentheses are an argument list and `foo((a, b))` is a
# one-argument call while `foo(a, b)` is a two-argument one -- a silent, compiling,
# byte-changing difference. A cast's `)` before the group is fine and is the common case.
IDENT = re.compile(r"[A-Za-z0-9_]")


def code_mask(text):
    """True for every offset that is real code -- comments and string literals are False.

    The idiom appears inside explanatory comments, and rewriting prose is not this
    tool's job; a site found in a comment would also produce a no-op edit that still
    reads as an accepted removal.
    """
    keep = bytearray(b"\x01") * len(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            j = n if j < 0 else j
            keep[i:j] = b"\x00" * (j - i)
            i = j
        elif c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            keep[i:j] = b"\x00" * (j - i)
            i = j
        elif c in "\"'":
            q, j = c, i + 1
            while j < n and text[j] != q:
                j += 2 if text[j] == "\\" else 1
            j = min(j + 1, n)
            keep[i:j] = b"\x00" * (j - i)
            i = j
        else:
            i += 1
    return keep


def _enclosing_group(text, at):
    """(open, close) of the innermost parenthesis group containing offset `at`."""
    depth, i = 0, at - 1
    while i >= 0:
        if text[i] == ")":
            depth += 1
        elif text[i] == "(":
            if depth == 0:
                break
            depth -= 1
        i -= 1
    if i < 0:
        return None
    depth, j = 0, at
    while j < len(text):
        if text[j] == "(":
            depth += 1
        elif text[j] == ")":
            if depth == 0:
                return i, j
            depth -= 1
        j += 1
    return None


def _balanced(s):
    if not (s.startswith("(") and s.endswith(")")):
        return False
    depth = 0
    for k, ch in enumerate(s):
        depth += (ch == "(") - (ch == ")")
        if depth == 0 and k != len(s) - 1:
            return False
    return depth == 0


def find_sites(text, kinds=KINDS):
    """Every removable launder site, as (kind, start, end, replacement), left to right."""
    keep = code_mask(text)
    out = []

    if "MASK" in kinds:
        for m in MASK_RE.finditer(text):
            if not keep[m.start()]:
                continue
            grp = _enclosing_group(text, m.start())
            if not grp:
                continue
            o, c = grp
            if text[m.end():c].strip():
                continue          # something else trails the mask; leave the site alone
            inner = text[o + 1:m.start()].strip()
            if not inner:
                continue
            out.append(("MASK", o, c + 1, inner if _balanced(inner) else f"({inner})"))

    for kind, rx in (("ROUNDTRIP", ROUNDTRIP_RE), ("WIDEN", WIDEN_RE)):
        if kind not in kinds:
            continue
        for m in rx.finditer(text):
            if keep[m.start()]:
                out.append((kind, m.start(), m.end(), ""))

    if "PARENS" in kinds:
        for o, ch in enumerate(text):
            if ch != "(" or not keep[o]:
                continue
            prev = text[:o].rstrip()
            if prev and IDENT.match(prev[-1]):
                continue                      # argument list, not a grouping paren
            grp = _enclosing_group(text, o + 1)
            if not grp or grp[0] != o:
                continue
            inner = text[o + 1:grp[1]].strip()
            if _balanced(inner):
                out.append(("PARENS", o, grp[1] + 1, inner))

    # ROUNDTRIP and WIDEN start at the same offset whenever the widening cast is the
    # first half of a round trip: `(long long)(int)` is also a `(long long)` followed by
    # `(`. One syntactic thing, so keep the longer (more specific) match and drop the
    # other -- otherwise every round trip is removed once but counted twice, which
    # silently doubles any metric built on this list.
    out.sort(key=lambda s: (s[1], -(s[2] - s[1])))
    deduped, claimed = [], set()
    for site in out:
        if site[1] in claimed:
            continue
        claimed.add(site[1])
        deduped.append(site)
    return deduped


def apply_site(text, site):
    _, start, end, rep = site
    return text[:start] + rep + text[end:]


def enrolled_paths():
    """Every src path the ROM build actually compiles -- a delinks entry marked `complete`.

    NOT the same set as enroll.candidates(), which is everything that *could* be
    enrolled. The difference matters here: a non-enrolled file has no byte gate behind
    it, so a removal that "still verifies" there proves only that the file is
    self-consistent, not that the ROM agrees. Several of them do not even reproduce
    at baseline.
    """
    out = set()
    for dl in CONFIG.rglob("delinks.txt"):
        cur = None
        for line in dl.read_text(errors="ignore").splitlines():
            if not line.startswith((" ", "\t")) and line.strip().endswith(":"):
                cur = line.strip()[:-1]
            elif cur and line.strip() == "complete":
                out.add(cur.replace("\\", "/"))
    return out


def module_index():
    """src-relative path -> (symbol, addr, size, module label), enrolled functions only."""
    live = enrolled_paths()
    idx = {}
    rows = enroll.candidates()
    rows = rows[0] if isinstance(rows, tuple) else rows
    for d, sym, rel, addr, size, sec in rows:
        rel = rel.replace("\\", "/")
        if sec != ".text" or rel not in live:
            continue
        idx[rel] = (sym, addr, size, RC.module_label(d, CONFIG))
    return idx


_TMP_LOCK = threading.Lock()
_TMP_N = [0]


def read_source(p):
    """Decode without translating newlines -- this tree mixes CRLF and LF per file.

    pathlib.read_text() normalises line endings on the way in, so a read/edit/write
    round trip silently rewrites every line of a CRLF file. That is invisible in a
    byte-match check (the compiler does not care) and very visible in review.
    """
    return p.read_bytes().decode("utf-8", errors="ignore")


def write_source(p, text):
    p.write_bytes(text.encode("utf-8"))


def _tmp_path(ext):
    with _TMP_LOCK:
        _TMP_N[0] += 1
        n = _TMP_N[0]
    d = REPO / "build" / "delaunder"
    d.mkdir(parents=True, exist_ok=True)
    return d / f"cand_{n}{ext}"


def verify_text(text, ext, sym, addr, size, label, version, strict):
    p = _tmp_path(ext)
    write_source(p, text)
    try:
        return BP.verify(p, sym, addr, size, label, strict=strict, version=version)
    finally:
        try:
            p.unlink()
        except OSError:
            pass


def greedy(cur, kinds, verify, rec):
    """Remove sites one at a time, keeping only those the ROM still agrees with.

    On acceptance the site is gone and the next one slides into its index, so `i`
    only advances past sites that earned their place.
    """
    i = 0
    while True:
        sites = find_sites(cur, kinds)
        if i >= len(sites):
            return cur
        cand = apply_site(cur, sites[i])
        if verify(cand):
            cur = cand
            rec["accepted"] += 1
            rec["by_kind"][sites[i][0]] = rec["by_kind"].get(sites[i][0], 0) + 1
        else:
            rec["rejected"] += 1
            rec["rejected_kinds"][sites[i][0]] = rec["rejected_kinds"].get(sites[i][0], 0) + 1
            i += 1


def sweep_file(rel, meta, kinds, strict, tidy=True):
    """Greedy per-site removal. Returns a record; never raises."""
    sym, addr, size, label = meta
    path = REPO / rel
    rec = {"file": rel, "symbol": sym, "module": label, "accepted": 0, "rejected": 0,
           "tidied": 0, "by_kind": {k: 0 for k in kinds},
           "rejected_kinds": {k: 0 for k in kinds},
           "status": "", "text": None, "stale_prose": False}
    try:
        base = read_source(path)
    except OSError as e:
        rec["status"] = f"unreadable: {e}"
        return rec

    sites = find_sites(base, kinds)
    if not sites:
        rec["status"] = "no-sites"
        return rec

    version, why = BP.compiler_for(path, sym)
    if version is None:
        rec["status"] = f"no pin: {why}"
        return rec

    ok, detail = verify_text(base, path.suffix, sym, addr, size, label, version, strict)
    if not ok:
        rec["status"] = f"baseline does not verify: {detail}"
        return rec
    rec["pin"] = version

    def ok_(text):
        return verify_text(text, path.suffix, sym, addr, size, label, version, strict)[0]

    cur = greedy(base, kinds, ok_, rec)

    # Tidy only what we disturbed. PARENS matches every redundant grouping paren in the
    # tree -- tens of thousands of sites, one compile each -- so it is not a kind you
    # sweep for. Run it after a removal lands, to collapse the `(((x)))` it left behind.
    # Counted separately: it is cosmetic, and folding it into the headline would inflate
    # the number that is supposed to mean "launder sites that were doing nothing".
    if tidy and rec["accepted"]:
        scratch = {"accepted": 0, "rejected": 0, "by_kind": {}, "rejected_kinds": {}}
        cur = greedy(cur, ("PARENS",), ok_, scratch)
        rec["tidied"] = scratch["accepted"]

    rec["status"] = "clean" if rec["rejected"] == 0 else "partial"
    if rec["accepted"]:
        rec["text"] = cur
        low = cur.lower()
        rec["stale_prose"] = ("launder" in low or "u64-mask" in low
                              or "u64 mask" in low or "forces the base" in low)
    return rec


def main():
    require_sweep_deps()
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument("--dry-run", action="store_true", help="report only; write nothing")
    g.add_argument("--apply", action="store_true", help="rewrite files whose removals verified")
    ap.add_argument("--kinds", default="MASK,ROUNDTRIP",
                    help=f"comma-separated subset of {','.join(KINDS)} (default MASK,ROUNDTRIP)")
    ap.add_argument("--limit", type=int, default=0, help="stop after N files (0 = all)")
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--filter", default="", help="only files whose path contains this")
    ap.add_argument("--no-tidy", action="store_true",
                    help="leave the redundant parentheses a removal exposes")
    ap.add_argument("--json", default="", help="write the full record set here")
    args = ap.parse_args()

    kinds = tuple(k.strip().upper() for k in args.kinds.split(",") if k.strip())
    bad = [k for k in kinds if k not in KINDS]
    if bad:
        ap.error(f"unknown kind(s): {', '.join(bad)}")

    idx = module_index()
    targets = []
    for rel, meta in sorted(idx.items()):
        p = REPO / rel
        if args.filter and args.filter not in rel:
            continue
        if p.suffix not in (".c", ".cpp") or not p.is_file():
            continue
        try:
            t = read_source(p)
        except OSError:
            continue
        if find_sites(t, kinds):
            targets.append((rel, meta))
    if args.limit:
        targets = targets[:args.limit]

    print(f"delaunder: {len(targets)} enrolled file(s) carry {'/'.join(kinds)} sites, "
          f"-j{args.jobs}, {'DRY RUN' if args.dry_run else 'APPLY'}")

    strict = (RA, RA.build_name_index(), RA.build_config_relocs(), RL.load_all_syms())

    recs = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        futs = {ex.submit(sweep_file, rel, meta, kinds, strict, not args.no_tidy): rel
                for rel, meta in targets}
        for n, f in enumerate(concurrent.futures.as_completed(futs), 1):
            rec = f.result()
            recs.append(rec)
            if rec["accepted"] or rec["rejected"]:
                flag = "!" if rec["rejected"] else " "
                print(f"  [{n}/{len(targets)}]{flag} {rec['file']}  "
                      f"-{rec['accepted']} kept {rec['rejected']}")
            elif not rec["status"].startswith(("no-sites", "clean")):
                print(f"  [{n}/{len(targets)}]? {rec['file']}  {rec['status']}")

    if args.apply:
        for rec in recs:
            if rec["text"] is not None:
                write_source(REPO / rec["file"], rec["text"])

    acc = sum(r["accepted"] for r in recs)
    rej = sum(r["rejected"] for r in recs)
    load_bearing = [r for r in recs if r["rejected"]]
    stale = [r for r in recs if r["stale_prose"]]
    skipped = [r for r in recs if r["status"].startswith(("no pin", "baseline", "unreadable"))]

    print("\n--- delaunder summary ---")
    print(f"sites removed (verified free) : {acc}")
    print(f"parens collapsed after removal: {sum(r.get('tidied', 0) for r in recs)}")
    print(f"sites kept (load-bearing)     : {rej}")
    if acc + rej:
        print(f"free rate                     : {100.0 * acc / (acc + rej):.1f}%")
    for k in kinds:
        a = sum(r["by_kind"].get(k, 0) for r in recs)
        d = sum(r["rejected_kinds"].get(k, 0) for r in recs)
        if a + d:
            print(f"  {k:<10} removed {a:>4}  kept {d:>3}  ({100.0 * a / (a + d):.1f}% free)")
    print(f"files fully cleaned           : {sum(1 for r in recs if r['status'] == 'clean' and r['accepted'])}")
    print(f"files with load-bearing sites : {len(load_bearing)}")
    for r in load_bearing:
        print(f"    {r['file']}  kept {r['rejected']}")
    if skipped:
        print(f"files skipped                 : {len(skipped)}")
        for r in skipped[:10]:
            print(f"    {r['file']}  {r['status']}")
    if stale:
        print(f"\nprose still describes a removed idiom ({len(stale)} file(s)) --")
        print("these need a human sentence, and until they get one the langmode metric")
        print("will not move even though the code is clean:")
        for r in stale:
            print(f"    {r['file']}")

    if args.json:
        pathlib.Path(args.json).write_text(
            json.dumps([{k: v for k, v in r.items() if k != "text"} for r in recs], indent=2),
            encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
