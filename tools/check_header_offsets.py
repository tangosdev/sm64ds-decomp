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
# padding before every one.
#
# Scalars are self-aligned only up to 4. mwccarm 2004/b56 -proc arm946e aligns
# `long long` and `double` to 4, not 8 -- measured, not assumed:
#     struct { char c; long long v; };   /* sizeof == 12, not 16 */
#     struct { char c; double d; };      /* sizeof == 12, not 16 */
# (Asserting 16 instead is rejected by the compiler, so the probe discriminates.)
# This is what lets `s64 unk_004;` sit at 0x004 in include/ClsnResult.h.
ALIGN = {t: min(w, 4) for t, w in SZ.items()}

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

# `void *p;` / `Model* m;` -- any pointer is 4 bytes on this target.
# The leading `struct`/`union`/`class`/`enum` is an elaborated type specifier and
# names the same type as the bare tag: `struct Matrix4x3 mat4x3;` is a Matrix4x3.
# Without it these were the ONLY unparsed declarations left in include/, and an
# unparsed one stops the gate reporting mismatches for the rest of the header --
# so a handful of `struct X y;` lines were suppressing the check on 8 files.
# An unknown tag still fails: the keyword makes the shape parseable, not the size.
DECL = re.compile(r"^\s*(?:(?:struct|union|class|enum)\s+)?([A-Za-z_]\w*)\s*(\**)\s*(\w+)\s*"
                  r"(?:\[\s*(0x[0-9a-fA-F]+|\d+)\s*\])?\s*;"
                  r"(?:\s*/\*\s*(0x[0-9a-fA-F]+))?")
# lines inside a struct body that are legitimately not declarations
IGNORABLE = re.compile(r"^\s*($|/\*|\*|//|\}|#)")

# Class sizes come from the tree's own compile-time assertions:
#   typedef char ModelAnim_size_must_be_0x64[...]
# Without these an embedded member is an unknown type, which this checker skips --
# and skipping without advancing the offset makes every later field mismatch.
# rglob, not glob: Matrix4x3 and Matrix3x3 assert their sizes in include/math/,
# so a non-recursive scan missed exactly the two that Model.h and friends embed.
CLASS_SIZES = {}
for _h in pathlib.Path(__file__).resolve().parents[1].joinpath("include").rglob("*.h"):
    for _m in re.finditer(r"(\w+)_size_must_be_(0x[0-9a-fA-F]+)",
                          _h.read_text(errors="replace")):
        CLASS_SIZES[_m.group(1)] = int(_m.group(2), 16)
SZ.update(CLASS_SIZES)

rc = 0
for path in sys.argv[1:]:
    txt = pathlib.Path(path).read_text(errors="replace")
    off, bad, n, skipped, pending = 0, 0, 0, [], []
    trusted = True
    started = in_comment = unmodelled = nested = False
    unknown_base = derived_from = None
    for lineno, line in enumerate(txt.splitlines(), 1):
        if not started:
            # `struct X {` or `struct X : Base {`. Without the second form this
            # tool never started on a derived struct at all, and reported
            # "0 commented fields ... struct spans 0x0" -- which reads exactly
            # like a pass. Every class this repo reconstructs from here on is a
            # derived struct, so that silence covered the growing majority.
            m0 = re.match(r"^\s*struct (\w+)\s*(?::\s*(?:public\s+)?(\w+)\s*)?\{", line)
            if m0:
                started = True
                base = m0.group(2)
                if base is not None:
                    # A derived struct's own fields do NOT start at 0 -- the base
                    # sub-object is there. Guessing 0 would mismatch every field,
                    # so refuse unless the base states its own size.
                    if base not in SZ:
                        unknown_base = base
                        break
                    off = SZ[base]
                    derived_from = base
            continue
        if in_comment:
            if "*/" in line:
                in_comment = False
            continue
        # A NESTED type definition -- `struct State { ... };` inside Player -- is a
        # type, not a field: it occupies no space and advances no offset. Consume it
        # whole, before the checks below can misread it.
        #
        # Two of those checks would, and both fail silently. Its closing `};` matches
        # the outer-struct terminator on the next line, and a pointer-to-member
        # declaration (`int (Player::*mInit)();`) matches the member-function pattern
        # that ends the field list. Either way the walk stopped at the nested type,
        # and for Player.h that meant "0 commented fields ... struct spans 0xd0" --
        # the base's size, no fields checked, exit status 0. A header with 177
        # checkable fields reported exactly like a clean pass.
        if nested:
            if re.match(r"^\s*\};", line):
                nested = False
            continue
        # A nested FORWARD declaration -- `struct State;` inside Camera -- is a type
        # too, and occupies no space either. It shows up when a class handles its
        # nested type ONLY by pointer: `Camera::ChangeState` is mangled `PNS_5StateE`,
        # which is what forces State to be nested, while nothing in the tree
        # dereferences one, so there is no layout to write down.
        #
        # DECL wants two identifiers (`struct X y;`), so a bare tag matched nothing
        # and fell through to UNPARSED -- and per the note above, ONE unparsed line
        # suppresses the mismatch check for the whole header. That is the same
        # silent-no-op shape this gate has already been bitten by twice, arriving a
        # third way; here it at least exits non-zero rather than reporting a pass.
        if re.match(r"^\s*(?:struct|union|class)\s+\w+\s*;\s*$", line):
            continue
        if re.match(r"^\s*struct \w+\s*(?::\s*(?:public\s+)?\w+\s*)?\{", line):
            nested = True
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
            # ...unless we started from a base whose size is asserted. A derived
            # class places no vptr of its own -- it inherits the base's, and the
            # base's asserted size already counts it. The running offset is sound,
            # so the member functions merely end the field list.
            if derived_from is None:
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
        # Alignment is the strictest MEMBER alignment, never the type's size: a
        # 100-byte ModelAnim aligns to 4, not to 100. Aligning to size padded 0xd4
        # out to 0x12c and made every later field in the header mismatch.
        #
        # Prefer a computed alignment when we have one: learn_aggregates knows a
        # Vector3 is 4-aligned because its members are. Fall back to min(w, 4) for
        # a type known only by its `_size_must_be_` assertion -- the size alone
        # cannot give the alignment, and no member of any struct here exceeds 4.
        a = 4 if (m and m.group(2)) else ALIGN.get(typ, min(w, 4))
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
    if unknown_base:
        # Not a pass and not a failure: a statement of what is missing. Adding
        # `typedef char X_size_must_be_0xN[sizeof(X) == 0xN ? 1 : -1];` to the
        # base's header makes this header checkable AND makes the base's own size
        # a claim the compiler enforces.
        print(f"{path}: skipped -- derives from {unknown_base}, whose size is asserted "
              f"nowhere (add {unknown_base}_size_must_be_0x.. to its header)")
        continue
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
