"""Make each `src/` file define the symbol its filename claims.

Some files carry a pair of annotations:

    // @symbol func_ov091_02132a0c        <- the symbol at this ROM address
    // @emits  daDsn_c_OnAimedAtWithEgg   <- what the C actually defines

The second name is recovered knowledge (usually from a vtable slot), but nothing in the
repo consumes either annotation, and the divergence breaks two things:

  * `tools/match.py --func <filename stem>` reports "symbol not found in object", so the
    byte oracle cannot verify the file at all - while `tools/progress.py` still counts it
    as matched, because that only checks a `src/<name>.c` exists;
  * the ROM link cannot use it. Gap objects import the carved-out function by its
    `symbols.txt` name as a *weak* undefined, so a differently-named definition leaves
    every caller binding to address 0 rather than erroring.

This renames the emitted function to the `@symbol` name, preserving the recovered name
in a comment, and verifies each file byte-for-byte against the ROM before keeping the
edit. A file that stops matching is reverted and reported.

It deliberately does NOT do the opposite (adopt the recovered names into
`config/**/symbols.txt`). That is the better long-term direction, but it changes the
canonical symbol table and every reference to those names, so it is a maintainer's call.

Usage:
    python tools/reconcile_names.py             # report what would change
    python tools/reconcile_names.py --apply
"""
import argparse
import collections
import concurrent.futures
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402
import build_pin as BP      # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
CONFIG = REPO / "config"

SYMBOL_RE = re.compile(r"^// @symbol\s+(\S+)\s*$", re.M)
EMITS_RE = re.compile(r"^// @emits\s+(\S+)\s*$", re.M)
CFG_RE = re.compile(
    r"^(\S+)\s+kind:function\((arm|thumb),size=0x([0-9a-fA-F]+)\)\s+addr:0x([0-9a-fA-F]+)")


def symbol_info():
    info = {}
    for sp in sorted(CONFIG.rglob("symbols.txt")):
        rel = sp.parent.relative_to(CONFIG).as_posix()
        label = "arm9" if rel == "arm9" else rel.split("/")[-1]
        for line in sp.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = CFG_RE.match(line)
            if m:
                info.setdefault(m.group(1),
                                (label, int(m.group(4), 16), int(m.group(3), 16)))
    return info


def rewrite(text, emits, symbol):
    """Rename the emitted function, keeping the recovered name visible."""
    new = re.sub(r"\b" + re.escape(emits) + r"\b", symbol, text)
    # The @emits line now says the same thing as @symbol, which is noise. Replace it
    # with an explicit note so the recovered identity is not lost.
    new = EMITS_RE.sub(f"// recovered name: {emits}", new, count=1)
    if emits not in new:
        new = new.replace(f"// @symbol {symbol}",
                          f"// @symbol {symbol}\n// recovered name: {emits}", 1)
    return new


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=10)
    args = ap.parse_args()

    info = symbol_info()

    jobs = []
    skipped = collections.Counter()
    for f in SP.iter_sources():
        head = f.read_text(encoding="utf-8", errors="ignore")[:400]
        ms, me = SYMBOL_RE.search(head), EMITS_RE.search(head)
        if not ms or not me:
            continue
        symbol, emits = ms.group(1), me.group(1)
        if symbol == emits:
            skipped["already agree"] += 1
            continue
        if f.stem != symbol:
            skipped["filename != @symbol"] += 1
            continue
        if symbol not in info:
            skipped["@symbol not in config"] += 1
            continue
        text = f.read_text(encoding="utf-8", errors="ignore")
        if not re.search(r"\b" + re.escape(emits) + r"\b", text):
            skipped["@emits name absent from file"] += 1
            continue
        if re.search(r"\b" + re.escape(symbol) + r"\b", text.replace(f"@symbol {symbol}", "")):
            skipped["@symbol already used in file"] += 1
            continue
        jobs.append((f, symbol, emits))

    print(f"candidates: {len(jobs)}")
    for k, v in skipped.most_common():
        print(f"  skipped {v:5d}  {k}")

    def work(job):
        f, symbol, emits = job
        label, addr, size = info[symbol]
        original = f.read_text(encoding="utf-8", errors="ignore")
        new = rewrite(original, emits, symbol)
        f.write_text(new, encoding="utf-8", newline="\n")
        # Verified under the compiler the ROM build will use for THIS file, and no
        # other. The loop this replaces tried the build default first and then the rest
        # of match.SWEEP, and kept the rename on whichever version happened to answer --
        # so a file that only reproduced under, say, 1.2/sp4 was applied and reported as
        # verified while the link would emit different bytes for it. Reporting which
        # version won does not make that safe: the rename is already on disk by then.
        # See tools/build_pin.py.
        ok, why = BP.verify(f, symbol, addr, size, label)
        verdict = BP.version_for(f, symbol) if ok else f"no match: {why}"
        if not ok or not args.apply:
            f.write_text(original, encoding="utf-8", newline="\n")
        return f.name, symbol, emits, verdict

    results = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for r in ex.map(work, jobs):
            results.append(r)

    by_verdict = collections.Counter(r[3] for r in results)
    print()
    for k, v in by_verdict.most_common():
        print(f"{v:6d}  {k}")
    bad = [r for r in results if r[3].startswith("no ")]
    if bad:
        print(f"\n{len(bad)} did not verify and were left untouched:")
        for (n, s, e, v) in bad[:10]:
            print(f"  {n}: {e} -> {s} ({v})")
    print("\n(dry run — pass --apply to keep the edits)" if not args.apply
          else f"\napplied to {len(results) - len(bad)} file(s)")


if __name__ == "__main__":
    main()
