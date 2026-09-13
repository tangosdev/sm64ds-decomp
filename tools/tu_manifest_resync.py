#!/usr/bin/env python3
"""Re-point a TU manifest at the compiler-numbered symbols the current tree emits.

mwccarm names file-local and vague-linkage entities with a POSITIONAL
uniquifier -- ``@344``, ``Bundle$725daMky_c_cpp``, ``@class$432daDsnBase_c_cpp``.
The number is assigned in parse order, so any edit that adds, removes or
reorders a literal / inline / class-scope entity renumbers it -- including an
edit to a shared base header that the manifest's own TU never mentions. The
shifts also compose ADDITIVELY, which is why two independently-correct re-syncs
of one manifest are mutually exclusive: the combined tree emits a third number
that neither branch names. Measured on ov030/daMky_c: main ``$724``, one branch
``$725``, another ``$732``, both together ``$733``, the whole 09-13 wave ``$739``.

So a manifest conflict must never be resolved by picking a side -- ``--ours``
and ``--theirs`` are both wrong. Take either side to get a mergeable tree, then
run this against the merged result.

A rename lands in TWO places and the build only complains about one at a time:
the manifest under ``config/tu_manifest.d/``, and the overlay's
``config/<target>/overlays/<module>/symbols.txt``. Fixing only the manifest
turns ``binding rewrite symbol @344 has 0 entries`` into ``licensed .bss symbol
@356 ... has no independent ov023 symbols.txt home``. This tool does both.

It only ever rewrites a number, never adds or drops a row, because a genuinely
new compiler-only symbol needs a human-written ``reason``. Two pairing rules,
both of which must be unambiguous or the row is reported and left alone:

* mangled names (``$NNN`` inside a longer symbol) pair by SHAPE -- identical
  after blanking the uniquifier digits and the Itanium length prefix.
* bare ``@NNN`` data/bss symbols carry no shape, so they pair by ORDERED
  POSITION and only when the section and size agree entry-for-entry.

    python tools/tu_manifest_resync.py ov030/daMky_c ov100/daStarGate_c
    python tools/tu_manifest_resync.py --from-build build.log
    python tools/tu_manifest_resync.py --check ov030/daMky_c   # exit 1, writes nothing
"""

import argparse
import json
import pathlib
import re
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tu_order_check as T
from elftools.elf.elffile import ELFFile

REPO = pathlib.Path(__file__).resolve().parent.parent
MANIFEST_DIR = REPO / "config" / "tu_manifest.d"

UNIQ = re.compile(r"[$@](\d+)")
LENPREFIX = re.compile(r"^_ZN\d+")
BARE_AT = re.compile(r"^@(\d+)$")
ANY_AT = re.compile(r"@(\d+)\b")
FAIL_LINE = re.compile(
    r"(?:compiler-only symbol\(s\) not defined: \[([^\]]*)\]"
    r"|binding rewrite symbol (\S+) has 0 entries"
    r"|licensed \.\w+ symbol (\S+) at)"
)
# manifest arrays whose rows name a non-text symbol
DATA_KEYS = ("bss", "data", "rodata")
# every array that can hold a human-written sentence, and the keys it spells it under
PROSE_ARRAYS = ("compiler_only_output", "symbol_binding_rewrites", "bss", "data",
                "rodata", "relocations", "functions")
PROSE_KEYS = ("reason", "evidence", "note")


def shape(symbol):
    """The symbol with its uniquifier digits and Itanium length prefix blanked.

    Two symbols share a shape exactly when they differ only in the positional
    number -- the only difference this tool is willing to rewrite.
    """
    return LENPREFIX.sub("_ZN#", UNIQ.sub("#", symbol))


def compile_and_read(entry):
    """(mangled symbol set, ordered bare-@ rows) as this tree actually emits them."""
    with tempfile.TemporaryDirectory(prefix="resync-") as td:
        obj = pathlib.Path(td) / (pathlib.Path(entry["source"]).stem + ".o")
        proc = T.compile_tu(entry["source"], obj)
        if proc.returncode != 0 or not obj.is_file():
            raise SystemExit(f"{entry['id']}: COMPILE FAILED\n   "
                             + (proc.stderr or proc.stdout or "").strip()[:800])
        mangled, bare = set(), []
        with open(obj, "rb") as fh:
            elf = ELFFile(fh)
            names = [s.name for s in elf.iter_sections()]
            for sec in elf.iter_sections():
                if sec.header["sh_type"] != "SHT_SYMTAB":
                    continue
                for sym in sec.iter_symbols():
                    if not sym.name:
                        continue
                    m = BARE_AT.match(sym.name)
                    if m:
                        ix = sym["st_shndx"]
                        home = names[ix] if isinstance(ix, int) and ix < len(names) else str(ix)
                        bare.append((int(m.group(1)), home, sym["st_size"]))
                    else:
                        mangled.add(sym.name)
    return mangled, sorted(set(bare))


def manifest_bare_rows(entry):
    """Ordered bare-@ rows the manifest declares, as (number, section, size)."""
    rows = []
    for key in DATA_KEYS:
        for r in entry.get(key) or []:
            m = BARE_AT.match(str(r.get("symbol", "")))
            if m:
                rows.append((int(m.group(1)), "." + key, int(str(r.get("size", "0")), 16)))
    return sorted(set(rows))


def plan_mangled(entry, emitted):
    """Pair each stale compiler-only row with the symbol it was renumbered to."""
    rows = entry.get("compiler_only_output") or []
    named = {r.get("symbol") for r in rows}
    unclaimed = {s for s in emitted if s not in named and UNIQ.search(s)}
    renames, manual = {}, []
    for row in rows:
        old = row.get("symbol")
        if not old or old in emitted:
            continue
        if not UNIQ.search(old):
            manual.append((old, "no positional uniquifier in the name"))
            continue
        cands = sorted(s for s in unclaimed if shape(s) == shape(old))
        if len(cands) == 1:
            renames[old] = cands[0]
            unclaimed.discard(cands[0])
        else:
            manual.append((old, "no emitted symbol shares its shape" if not cands
                           else f"ambiguous, {len(cands)} candidates: {cands}"))
    return renames, manual


def plan_bare(entry, emitted_bare):
    """Pair bare @NNN rows by position, but only if section and size agree."""
    want = manifest_bare_rows(entry)
    stale = [w for w in want if w[0] not in {e[0] for e in emitted_bare}]
    if not stale:
        return {}, []
    if len(want) != len(emitted_bare):
        return {}, [(f"@{n}", f"manifest declares {len(want)} bare-@ symbol(s), the "
                              f"object emits {len(emitted_bare)} -- pair them by hand")
                    for n, _, _ in stale]
    mismatched = [(w, e) for w, e in zip(want, emitted_bare) if w[1:] != e[1:]]
    if mismatched:
        w, e = mismatched[0]
        return {}, [("@%d" % w[0], f"positional pairing rejected: @{w[0]} is "
                     f"{w[1]}/{w[2]}B but @{e[0]} is {e[1]}/{e[2]}B")]
    return {f"@{w[0]}": f"@{e[0]}" for w, e in zip(want, emitted_bare) if w[0] != e[0]}, []


def substitute(text, renames):
    """One simultaneous pass -- the old and new number sets overlap in practice.

    Sequential replacement double-applies: ov100 maps @438->@444 and @444->@450,
    so replacing in order would land @438 at @450.
    """
    bare = {int(k[1:]): v for k, v in renames.items() if BARE_AT.match(k)}
    long = {k: v for k, v in renames.items() if not BARE_AT.match(k)}
    hits = {}
    for old, new in long.items():
        n = text.count(old)
        if n:
            hits[old] = hits.get(old, 0) + n
            text = text.replace(old, new)
    if bare:
        def sub(m):
            n = int(m.group(1))
            if n in bare:
                key = f"@{n}"
                hits[key] = hits.get(key, 0) + 1
                return bare[n]
            return m.group(0)
        text = ANY_AT.sub(sub, text)
    return text, hits


def prose_of(entry):
    """Every human-written sentence in the manifest, as one string.

    Three spellings, because the schema uses a different one per array:
    `compiler_only_output` rows carry `reason` (and twice `note`), while `bss`,
    `data`, `relocations` and `symbol_binding_rewrites` rows carry `evidence`.
    Scanning only `reason` -- which this did until the keys were counted -- sees
    the mangled-symbol prose and none of the bare-@ prose, which is the half the
    data-row renames touch.
    """
    out = []
    for key in PROSE_ARRAYS:
        for row in entry.get(key) or []:
            if isinstance(row, dict):
                out += [str(row[k]) for k in PROSE_KEYS if row.get(k)]
    out += [str(n) for n in entry.get("notes") or []]
    return " ".join(out)


def stale_prose(before, after, renames):
    """Uniquifier numbers a human-written sentence spells out. Never auto-fixed.

    A `reason` is the whole point of the schema -- it argues WHY a symbol is
    compiler-only -- and ov030/daMky_c's narrates its own renumbering history:
    "this row read _ZN21Bundle$701daMky_c_cppD1Ev until stage 3b ... at which
    point the same struct came back as $724". A substitution inside that
    sentence turns a true history into a false one, which is not hypothetical:
    a blanket `.replace("$725", "$739")` over that file produced "$739 with
    SharedFilePtr.h included" for a step that really was $725.

    Which pass touched a sentence is not something to reason about -- the
    mangled pass replaces whole symbols and the bare pass replaces tokens, and
    a sentence can spell either form. So compare the prose BEFORE and AFTER and
    report what actually happened:

    * still there afterwards -> STALE, the sentence describes a number the
      policy row no longer holds.
    * there before and gone after -> REWRITTEN, the pass edited the sentence
      and the claim it makes may now be false.

    Measured across all 177 manifests on 5a6e76995: exactly one prose site
    names a uniquifier at all. Both arms are reported anyway, because the cost
    of finding out the hard way is a plausible sentence that is a lie.

    Old and new number sets overlap (ov100 maps @438->@444 and @444->@450), so
    a number that is also somebody's NEW value is not evidence of anything.
    """
    was, now = prose_of(json.loads(before)), prose_of(json.loads(after))
    # prose names the bare uniquifier ("shifted it to $432"), not the full symbol
    fresh = {t for new in renames.values() for t in UNIQ.findall(new)}
    rewritten, stale = [], []
    for old in sorted(renames):
        for tok in UNIQ.findall(old):
            if tok in fresh:
                continue
            # (?!\d) and not \b: prose spells the number both bare ("came back
            # as $724") and inside the full symbol ("read
            # _ZN21Bundle$701daMky_c_cppD1Ev until"), and \b sees no boundary
            # between the 1 and the d, so it misses every full-symbol mention --
            # which is the spelling the substitution actually rewrites.
            pattern = r"[$@]" + tok + r"(?!\d)"
            if re.search(pattern, now):
                stale.append(old)
            elif re.search(pattern, was):
                rewritten.append(old)
            break
    return rewritten, stale


def symbols_txt_for(entry):
    """The overlay symbol table that must carry the same rename."""
    module = entry.get("module", "")
    hits = sorted(REPO.glob(f"config/*/overlays/{module}/symbols.txt"))
    if not hits:
        hits = sorted(REPO.glob(f"config/{module}/symbols.txt"))
    return hits[0] if hits else None


def ids_from_build(log):
    """Manifest ids named by the FAIL lines of a rombuild or tubuild log."""
    text = pathlib.Path(log).read_text(encoding="utf-8", errors="replace")
    wanted = set()
    for m in FAIL_LINE.finditer(text):
        listed, binding, licensed = m.groups()
        if listed:
            wanted.update(s.strip().strip("'\"") for s in listed.split(","))
        wanted.update(x for x in (binding, licensed) if x)
    wanted.discard("")
    if not wanted:
        return []
    return [f"{p.parent.name}/{p.stem}" for p in sorted(MANIFEST_DIR.rglob("*.json"))
            if any(w in p.read_text(encoding="utf-8", errors="replace") for w in wanted)]


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("ids", nargs="*", help="manifest entry id(s), e.g. ov030/daMky_c")
    ap.add_argument("--from-build", metavar="LOG",
                    help="also take every id named by a FAIL line in this build log")
    ap.add_argument("--check", action="store_true",
                    help="report and exit 1 if anything is stale; write nothing")
    args = ap.parse_args(argv)

    ids = list(args.ids)
    if args.from_build:
        found = ids_from_build(args.from_build)
        print(f"{args.from_build}: names {len(found)} manifest(s)")
        ids += [i for i in found if i not in ids]
    if not ids:
        ap.error("give at least one manifest id, or --from-build")

    stale_total = written = 0
    for ident in ids:
        path = MANIFEST_DIR / f"{ident}.json"
        entry = json.loads(path.read_text(encoding="utf-8"))
        entry.setdefault("id", ident)
        mangled, bare = compile_and_read(entry)
        renames, manual = plan_mangled(entry, mangled)
        more, more_manual = plan_bare(entry, bare)
        renames.update(more)
        manual += more_manual

        for old, why in manual:
            print(f"{ident}: MANUAL  {old}\n            {why}")
        if not renames:
            if not manual:
                print(f"{ident}: in sync")
            continue
        stale_total += 1
        if args.check:
            for old, new in sorted(renames.items()):
                print(f"{ident}: STALE   {old} -> {new}")
            continue

        targets = [path]
        table = symbols_txt_for(entry)
        if table:
            targets.append(table)
        rewritten_prose = stale_prose_rows = ()
        for target in targets:
            before = target.read_text(encoding="utf-8")
            body, hits = substitute(before, renames)
            if not hits:
                continue
            if target.suffix == ".json":
                json.loads(body)  # refuse to write a file we just broke
                # while both texts are in hand -- reading the file back after the
                # write finds only the new numbers and reports nothing, every time
                rewritten_prose, stale_prose_rows = stale_prose(before, body, renames)
            target.write_text(body, encoding="utf-8", newline="")
            rel = target.relative_to(REPO).as_posix()
            print(f"{ident}: {rel}")
            for old, new in sorted(renames.items()):
                if hits.get(old):
                    print(f"            {old} -> {new}  ({hits[old]} occurrence(s))")
        written += 1
        for old in stale_prose_rows:
            print(f"{ident}: NOTE    a sentence still spells {old} out in prose -- "
                  f"the policy row is correct now, the sentence is not")
        for old in rewritten_prose:
            print(f"{ident}: NOTE    a sentence spelled {old} out and the substitution "
                  f"REWROTE it -- re-read that sentence, it may now be false")

    if args.check:
        if stale_total:
            print(f"\n{stale_total} manifest(s) out of sync with this tree")
            return 1
        print(f"\n{len(ids)} manifest(s) in sync with this tree")
        return 0
    print(f"\n{written} manifest(s) rewritten")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
