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
# Alignment is NOT the same as width once aggregates are in play: a Vector3 is 12
# bytes wide but 4-aligned, and using the width for the padding test would invent
# padding before every one. Scalars are self-aligned, so this starts as a copy.
ALIGN = dict(SZ)

REPO = pathlib.Path(__file__).resolve().parent.parent
# A simple aggregate body: no nested braces, no methods.
AGG = re.compile(r"(?:typedef\s+)?struct\s+(\w+)\s*\{([^{}]*)\}", re.S)
# `Fix12i x, y, z;` -- one type, several declarators, optional array bounds.
MEMBERS = re.compile(r"^\s*([A-Za-z_]\w*)\s+([^;]+);", re.M)


def learn_aggregates(*paths):
    """Teach SZ/ALIGN the widths of simple project aggregates.

    Without this a `Vector3 pos;` field is unrecognised, and the tool's own rule --
    an unrecognised declaration leaves the running offset short, so every later
    field silently matches at the wrong place -- means it cannot gate ANY header
    that uses one. include/RaycastGround.h has carried `Vector3 pos;` since it was
    hand-extended and has been unparseable that whole time.

    Deliberately narrow: only brace-free bodies whose members are already-known
    types, so nothing is guessed. Anything else stays unknown and still reports
    UNPARSED rather than being silently mis-sized.
    """
    for path in paths:
        try:
            txt = pathlib.Path(path).read_text(errors="replace")
        except OSError:
            continue
        for name, body in AGG.findall(txt):
            if name in SZ:
                continue
            off = align = 0
            for typ, decls in MEMBERS.findall(body):
                ptr = "*" in decls
                w = 4 if ptr else SZ.get(typ)
                a = 4 if ptr else ALIGN.get(typ)
                if w is None:
                    off = None
                    break
                for d in decls.split(","):
                    arr = re.search(r"\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]", d)
                    if off % a:
                        off += a - (off % a)
                    off += w * (int(arr.group(1), 0) if arr else 1)
                    align = max(align, a)
            if off and align:
                if off % align:
                    off += align - (off % align)   # tail padding
                SZ[name], ALIGN[name] = off, align


learn_aggregates(REPO / "include" / "types.h")

# `void *p;` / `Model* m;` -- any pointer is 4 bytes on this target
DECL = re.compile(r"^\s*([A-Za-z_]\w*)\s*(\**)\s*(\w+)\s*"
                  r"(?:\[\s*(0x[0-9a-fA-F]+|\d+)\s*\])?\s*;"
                  r"(?:\s*/\*\s*(0x[0-9a-fA-F]+))?")
# lines inside a struct body that are legitimately not declarations
IGNORABLE = re.compile(r"^\s*($|/\*|\*|//|\}|#)")

rc = 0
for path in sys.argv[1:]:
    txt = pathlib.Path(path).read_text(errors="replace")
    off, bad, n, skipped, pending = 0, 0, 0, [], []
    trusted = True
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
        # A declaration can carry a trailing comment that runs onto later lines:
        #     u8 unk_010;   /* 0x010 - first byte of the 0x28-byte ClsnResult
        #                              the hit is written into ... */
        # Those continuation lines are prose, not declarations. Without this they
        # were reported UNPARSED and the header failed the gate on its own comments.
        if line.count("/*") > line.count("*/"):
            in_comment = True
        m = DECL.match(line)
        typ = m.group(1) if m else None
        w = 4 if (m and m.group(2)) else SZ.get(typ)
        if w is None:
            # An unrecognised declaration is NOT harmless: skipping it leaves the
            # running offset short, so every later field silently "matches" at the
            # wrong place. Say so rather than quietly carrying on.
            skipped.append(f"{lineno}: {line.strip()}")
            # ...and stop claiming MISMATCH from here on. The running offset is now
            # known-wrong, so every later comparison is against a meaningless number:
            # `struct CylinderClsn base;` is unparseable, which made the very next
            # field report "comment 0x30, computed 0x000". Those are artifacts of the
            # skip, not defects in the header. UNPARSED already fails the gate, so
            # nothing is being hidden -- the difference is that what it prints is true.
            trusted = False
            continue
        _, _, name, arr, decl = m.groups()
        a = 4 if (m and m.group(2)) else ALIGN.get(typ, w)
        if off % a:                       # compiler would insert padding here
            off += a - (off % a)
        if decl is not None and trusted:
            n += 1
            if int(decl, 16) != off:
                # Buffered, not printed: a polymorphic struct declares its virtuals
                # AFTER its fields, so we only learn the layout is unmodelled once
                # every field has already been compared against an offset that is
                # short by the implicit vptr. Printing as we went emitted a screen of
                # MISMATCHes for a header the tool then correctly skipped.
                pending.append(f"  MISMATCH {path}:{lineno} {name}: comment {decl}, "
                               f"computed 0x{off:03x}")
                bad += 1
        off += w * (int(arr, 0) if arr else 1)
    if unmodelled:
        # not a failure: this gate is for the generated flat headers, and a
        # hand-written polymorphic one has layout the text does not determine
        print(f"{path}: skipped -- polymorphic C++ struct, implicit vptr not modelled")
        continue
    for msg in pending:
        print(msg)
    for s in skipped:
        print(f"  UNPARSED {path}:{s}")
    print(f"{path}: {n} commented fields, {bad} mismatched, "
          f"{len(skipped)} unparsed, struct spans 0x{off:x}")
    rc |= bool(bad or skipped)
sys.exit(rc)
