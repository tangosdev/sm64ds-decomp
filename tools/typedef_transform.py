"""Replace inline fixed-width scalar typedefs (the ones include/types.h provides) with
a single #include "types.h". Byte-neutral; the gate reverts anything that doesn't match.
Usage: python typedef_transform.py <worktree> <slice-file>
"""
import re, sys

WT, SLICE = sys.argv[1], sys.argv[2]
PROV = r'(u8|u16|u32|s8|s16|s32|u64|s64|Fix12|Fix12i|f32|f64|vu8|vs8|vu16|vs16|vu32|vs32|OnYoshiEatReturnVal|bool)'
TD = re.compile(r'^\s*typedef\s+[^;]*\b' + PROV + r'\s*;\s*$', re.M)

for f in [l.strip() for l in open(SLICE) if l.strip()]:
    p = f"{WT}/{f}"
    t = open(p, encoding="utf8", errors="ignore").read()
    if '#include "types.h"' in t:
        continue
    marked = TD.sub("__DROP__", t)
    if "__DROP__" not in marked:
        continue
    lines = [l for l in marked.split("\n") if l.strip() != "__DROP__"]
    ins = 1 if lines and lines[0].strip() == "//cpp" else 0
    lines.insert(ins, '#include "types.h"')
    open(p, "w", encoding="utf8").write("\n".join(lines))
