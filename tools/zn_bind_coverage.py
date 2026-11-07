#!/usr/bin/env python3
"""Equivalence measurement for tools/def_locate.py.

For every src/ file: which owned symbols do `// @symbol` markers bind, and
which does def_locate bind? A marker binds the definition below it; stacked
markers (ctor/dtor variants) all bind the same def. def_locate binds via
demangled-signature matching. Report:
  - marker-bound but locator-unbound  (REGRESSIONS)
  - locator-bound but marker-unbound  (improvements)
"""
import pathlib, re, sys, collections
sys.path.insert(0, str(pathlib.Path(__file__).parent))
import srcpath as SP
import def_locate as DL

ROOT = pathlib.Path(__file__).resolve().parent.parent
SRC = ROOT / "src"
MARK = re.compile(r"^\s*//\s*@symbol\s+(\S+)\s*$", re.M)

regress = collections.Counter()
improve = collections.Counter()
reg_samples = collections.defaultdict(list)
imp_samples = collections.defaultdict(list)
tot_mark = tot_loc = 0
for p in sorted(SRC.rglob("*")):
    if p.suffix not in (".c", ".cpp"):
        continue
    rel = p.relative_to(ROOT).as_posix()
    owned = SP.symbols_for(rel)
    if not owned or owned == [p.stem]:
        continue
    text = p.read_text(encoding="utf-8", errors="replace")
    marks = [m for m in MARK.findall(text) if m in owned]
    try:
        bound, defs = DL.bind(rel, text, owned)
    except Exception as e:
        print(f"EXC {rel}: {e!r}")
        continue
    loc_bound = set(bound)
    tot_mark += len(marks)
    tot_loc += len(loc_bound & set(owned))
    for s in marks:
        if s not in loc_bound:
            regress[rel] += 1
            reg_samples[rel].append(s)
    for s in loc_bound:
        if s not in marks:
            improve[rel] += 1
            imp_samples[rel].append(s)

print(f"marker-bound owned symbols: {tot_mark}")
print(f"locator-bound owned symbols: {tot_loc}")
if tot_mark == 0:
    print("no // @symbol markers remain; the locator outline is the map")
    raise SystemExit(0)
print(f"regressions: {sum(regress.values())} in {len(regress)} files")
print(f"improvements: {sum(improve.values())} in {len(improve)} files")
print("\nworst regressions:")
for f, n in regress.most_common(12):
    print(f"  {n:3d}  {f}")
    for s in reg_samples[f][:4]:
        print(f"       {s}")
print("\nworst improvements:")
for f, n in improve.most_common(8):
    print(f"  {n:3d}  {f}")
