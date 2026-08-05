"""Confirm every field in a struct header lands on the offset its comment claims.

The generated headers encode layout twice: once as a sequence of declarations and
pads, and once as a `/* 0x0a4 */` comment per field. Retyping a field silently
breaks the agreement between them unless the following pad is shrunk to match, and
nothing else in the build would notice -- a header is not compiled on its own, and
the byte gate cannot see a field no source file happens to read.

This walks the declarations, applies natural alignment, and compares. Used as the
first gate on any header edit; see notes/plan-scalar-markers.md 4.

    python tools/check_header_offsets.py include/Enemy.h include/Camera.h
    python tools/check_header_offsets.py $(git diff --name-only include/)
"""
import re, sys, pathlib
SZ = {"u8":1,"s8":1,"char":1,"u16":2,"s16":2,"short":2,"u32":4,"s32":4,"int":4,
      "unsigned":4,"long":4,"Fix12i":4,"float":4,"u64":8,"s64":8,"double":8}
# `void *p;` / `Model* m;` -- any pointer is 4 bytes on this target
DECL = re.compile(r"^\s*([A-Za-z_]\w*)\s*(\**)\s*(\w+)\s*"
                  r"(?:\[\s*(0x[0-9a-fA-F]+|\d+)\s*\])?\s*;"
                  r"(?:\s*/\*\s*(0x[0-9a-fA-F]+))?")
# lines inside a struct body that are legitimately not declarations
IGNORABLE = re.compile(r"^\s*($|/\*|\*|//|\}|#)")

rc = 0
for path in sys.argv[1:]:
    txt = pathlib.Path(path).read_text(errors="replace")
    off, bad, n, skipped = 0, 0, 0, []
    started = in_comment = unmodelled = False
    for lineno, line in enumerate(txt.splitlines(), 1):
        if not started:
            if re.match(r"^\s*struct \w+ \{", line):
                started = True
            continue
        if in_comment:
            if "*/" in line:
                in_comment = False
            continue
        if re.match(r"^\s*(#|\}|/\* methods)", line):
            break
        if IGNORABLE.match(line):
            if "/*" in line and "*/" not in line:
                in_comment = True
            continue
        # A polymorphic C++ struct carries an implicit vptr at offset 0 that no
        # declaration mentions, so the running offset cannot be derived from the text.
        # Say the struct is unmodelled rather than emit a mismatch per field.
        if re.match(r"^\s*(virtual\b|[A-Za-z_][\w:<>, &*]*\([^;]*\)\s*(const)?\s*;)", line):
            unmodelled = True
            break
        m = DECL.match(line)
        typ = m.group(1) if m else None
        w = 4 if (m and m.group(2)) else SZ.get(typ)
        if w is None:
            # An unrecognised declaration is NOT harmless: skipping it leaves the
            # running offset short, so every later field silently "matches" at the
            # wrong place. Say so rather than quietly carrying on.
            skipped.append(f"{lineno}: {line.strip()}")
            continue
        _, _, name, arr, decl = m.groups()
        if off % w:                       # compiler would insert padding here
            off += w - (off % w)
        if decl is not None:
            n += 1
            if int(decl, 16) != off:
                print(f"  MISMATCH {path}:{lineno} {name}: comment {decl}, "
                      f"computed 0x{off:03x}")
                bad += 1
        off += w * (int(arr, 0) if arr else 1)
    if unmodelled:
        # not a failure: this gate is for the generated flat headers, and a
        # hand-written polymorphic one has layout the text does not determine
        print(f"{path}: skipped -- polymorphic C++ struct, implicit vptr not modelled")
        continue
    for s in skipped:
        print(f"  UNPARSED {path}:{s}")
    print(f"{path}: {n} commented fields, {bad} mismatched, "
          f"{len(skipped)} unparsed, struct spans 0x{off:x}")
    rc |= bool(bad or skipped)
sys.exit(rc)
