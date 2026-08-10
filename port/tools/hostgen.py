#!/usr/bin/env python3
"""Rewrite sm64ds-decomp sources for the host build.

The decomp's `src/` is byte-verified against the ROM and must not change. This
reads it and emits a transformed copy into build/host-src/, so the port never
edits the matched tree. See notes/assessment.md section 0c.

Currently rewrites direct MMIO cast-derefs:

    *(volatile unsigned short *)0x4000280      ->  NTR_MMIO(unsigned short, 0x4000280)

which routes the access through include/ntr/mmio.h so that write-triggered
registers run their side effect.

It also rewrites registers reached through a POINTER BOUND TO A LITERAL:

    volatile int *p = (volatile int *)0x400046c;
    *p = x;                                    ->  NTR_MMIO(int, 0x400046c) = x

The header used to say those were harmless because the host maps real memory
at the DS I/O addresses. They are not. func_0204488c -- the ordinary part
walk, and the one place a model's scale is spent -- writes MTX_SCALE through
exactly that shape, so every scale in the render walk latched into dead memory
and the geometry engine never saw it: the level lost its BMD shift and every
actor lost the Vector3 scale its Render asked for. See MMIO_PTR below.

    python tools/hostgen.py --decomp ../sm64ds-decomp _ZN4cstd3divEii
    python tools/hostgen.py --decomp ../sm64ds-decomp --all
"""

import argparse
import pathlib
import re
import sys

# *(volatile TYPE *)0xADDR   ->  NTR_MMIO(TYPE, 0xADDR)
# TYPE is a plain builtin spelling; the decomp never casts to anything else
# here. `volatile` is optional: matched code also reaches registers through
# plain casts (the material bind stores POLYGON_ATTR/TEXIMAGE without it),
# and any literal-address deref in the 0x04xxxxxx window is MMIO by
# definition.
# Two textual shapes: `*(T *)0xADDR` and `*((T *)0xADDR)`. The conditional
# group consumes the trailing paren only when the extra leading one matched,
# so computed addresses like `*(volatile u32 *)(0xADDR + x)` are left alone
# (they resolve through the mapped latch window, which is correct for reads).
MMIO_DEREF = re.compile(
    r"\*\s*(\()?\s*\(\s*(?:volatile\s+)?"
    r"((?:unsigned|signed|long|short|int|char|u8|u16|u32|u64|s8|s16|s32|s64)"
    r"(?:\s+(?:unsigned|signed|long|short|int|char))*)"
    r"\s*\*\s*\)\s*(0x0?4[0-9A-Fa-f]{6})\b(?(1)\s*\))"
)

# A REGISTER REACHED THROUGH A POINTER, which the cast-deref rewrite above
# cannot see. The decomp writes this whenever mwccarm kept the address in a
# register across a block:
#
#     volatile int *mtxScale = (volatile int *)0x400046c;   func_0204488c
#     ...
#     *mtxScale = c0;                                       MTX_SCALE, x3
#
#     volatile unsigned int *r2b8 = (volatile unsigned int *)0x40002b8;
#     r2b8[0] = 0;  r2b8[1] = n;                            func_02053008
#
# Both forms latch into mapped memory and trigger nothing, which is silent and
# total: the geometry engine simply never receives the command.
#
# The rewrite is deliberately narrow, because a pointer is a pointer and this
# transform is textual. A name qualifies only when
#   * it is bound EXACTLY ONCE, from a literal cast in the 0x04xxxxxx window,
#   * every other mention of it is `*name` or `name[<decimal literal>]`, and
#   * nothing else -- no address-of, no passing it along, no arithmetic.
# Anything outside that and the name is left completely alone; the port keeps
# the behaviour it had (a latch that does not trigger) rather than gaining a
# rewrite the tool cannot justify. The binding statement itself stays: it
# becomes an unused local, which is cheaper to read than a hole in the source.
MMIO_TYPE = (r"(?:unsigned|signed|long|short|int|char|"
             r"u8|u16|u32|u64|s8|s16|s32|s64)"
             r"(?:\s+(?:unsigned|signed|long|short|int|char))*")
MMIO_BIND = re.compile(
    r"\b(\w+)\s*=\s*\(\s*(?:volatile\s+)?(?:const\s+)?(" + MMIO_TYPE + r")"
    r"\s*\*\s*\)\s*(0x0?4[0-9A-Fa-f]{6})\s*;")
MMIO_WIDTH = {
    "char": 1, "signed char": 1, "unsigned char": 1, "u8": 1, "s8": 1,
    "short": 2, "short int": 2, "signed short": 2, "unsigned short": 2,
    "unsigned short int": 2, "u16": 2, "s16": 2,
    "int": 4, "signed": 4, "signed int": 4, "unsigned": 4, "unsigned int": 4,
    "long": 4, "long int": 4, "unsigned long": 4, "u32": 4, "s32": 4,
    "long long": 8, "signed long long": 8, "unsigned long long": 8,
    "unsigned long long int": 8, "u64": 8, "s64": 8,
}


def mmio_ptr(text):
    """Rewrite derefs of names bound to a literal register address."""
    binds = {}
    for m in MMIO_BIND.finditer(text):
        name = m.group(1)
        ctype = " ".join(m.group(2).split())
        binds[name] = None if name in binds else (ctype, int(m.group(3), 16))

    edits = []
    for name, info in binds.items():
        if info is None or info[0] not in MMIO_WIDTH:
            continue
        ctype, addr = info
        width = MMIO_WIDTH[ctype]
        # spans this pass must not touch: the declarator and the binding.
        skip = [m.span() for m in re.finditer(
            r"(?:(?:volatile|const)\s+)*(?:" + MMIO_TYPE + r")\s*\*\s*" +
            re.escape(name) + r"\b", text)]
        skip += [m.span() for m in MMIO_BIND.finditer(text)
                 if m.group(1) == name]
        mine = []
        ok = True
        for m in re.finditer(r"\b" + re.escape(name) + r"\b", text):
            if any(a <= m.start() < b for a, b in skip):
                continue
            d = re.match(r"\s*\[\s*(\d+)\s*\]", text[m.end():])
            if d:
                mine.append((m.start(), m.end() + d.end(),
                             addr + int(d.group(1)) * width))
                continue
            s = re.search(r"(?<![)\]\w])\*\s*$", text[:m.start()])
            if s:
                mine.append((s.start(), m.end(), addr))
                continue
            ok = False
            break
        if ok and mine:
            edits += [(a, b, f"NTR_MMIO({ctype}, {c:#x})") for a, b, c in mine]

    for a, b, rep in sorted(edits, reverse=True):
        text = text[:a] + rep + text[b:]
    return text, len(edits)


HEADER = """// GENERATED by tools/hostgen.py from {src}
// Do not edit. The source of truth is the byte-verified decomp; edit the
// transform or the host I/O layer instead.
#include "ntr/mmio.h"

"""


# mwccarm allows void*+int (byte arithmetic, the pret void*-arith idiom);
# MSVC C++ rejects it. Retype the pointee to char* only where the deref
# feeds an addition -- the value is identical, the arithmetic becomes legal.
# Two spellings reach here and they park their parentheses differently:
#     (*((void **) obj)) + 0x18                    -- func_02044b30
#     *(void**)(c + id * 4 + 0xdc) + 0x50          -- func_ov002_020c897c
# The first wraps the deref and leaves the operand bare; the second wraps
# the operand and leaves the deref bare. Same rewrite either way, and the
# operand is substituted VERBATIM, parens and all -- dropping them turns
# *(void**)(c + 0x160) into *((char **) c + 0x160), which is a scaled
# index, not the same address.
#
# The second spelling needs the lookbehind. Casting the deref before the
# addition is already legal C++, and the decomp writes that far more often
# than the bare form:
#     (char*)*(void**)(c + 0x160) + 0x58     legal, leave it alone
#            *(void**)(c + 0x160) + 0x58     void* arithmetic, rewrite
# The only textual difference is what sits in front of the `*`, so a cast's
# closing paren (and a preceding identifier or subscript, which would make
# the `*` a multiply) blocks the match.
VOIDPP_ARITH = re.compile(
    r"\(\s*\*\s*\(\s*\(\s*void\s*\*\*\s*\)\s*([^()]+?)\s*\)\s*\)\s*\+"
    r"|(?<![)\]\w])\*\s*\(\s*void\s*\*\*\s*\)\s*(\([^()]+?\))\s*\+")


def voidpp_char(m):
    """Both alternatives land on the same char** deref."""
    return "((*((char **) %s))) +" % (m.group(1) or m.group(2))

# GCC-style attributes (long_call, target("thumb")) mean nothing to MSVC.
# One nesting level inside the (( )) is enough for every use in src/.
ATTRIBUTE = re.compile(r"__attribute__\s*\(\((?:[^()]|\([^()]*\))*\)\)")

# ENGINE BSS: `int data_0209b458;` at file scope.
#
# mwccarm and MSVC's C front end both make that a TENTATIVE definition, which
# the linker merges with whoever really owns the storage -- which is how the
# port's .c slices coexist with hal/auto_bss.cpp. MSVC's C++ front end does
# not: in a .cpp it is a strong definition, and the same symbol in the HAL is
# LNK2005. The port's rule is that engine BSS is the HAL's (real sizes live
# there; a src file's declared width is whatever that one function needed --
# Actor::BeforeBehavior spells the 0x5c-byte Clipper `char data_0209f43c;`),
# so the src-side definition becomes a declaration and the HAL keeps the
# storage. Opt-in per file: --extern-data, because a src file CAN be the
# intended owner and this rewrite would silently unhome it.
#
# Only `data_<hex>` names are touched, and only a plain scalar/array
# definition with no initializer.
EXTERN_DATA = re.compile(
    r"^([ \t]*)((?:(?:unsigned|signed|volatile|const|struct|long|short|int|char|"
    r"float|double|u8|u16|u32|u64|s8|s16|s32|s64|bool|Vector3|Matrix4x3|"
    r"[A-Z]\w*)[ \t]+)+\**[ \t]*)(data_(?:ov\d+_)?[0-9a-f]{6,8})"
    r"([ \t]*(?:\[[^\];=]*\])?)[ \t]*;",
    re.MULTILINE)


# C99's `_Bool` keyword, which MSVC's C++ front end does not have. Everything
# hostgen emits is compiled as C++, so the spelling has to become `bool` --
# same width and same argument passing under cdecl on x86, so no ABI change.
# Exactly one file in the decomp writes it (Particle::Callback::OnUpdate's
# `_Bool active`), but the rewrite is general because the next one will not be
# announced.
CBOOL = re.compile(r"\b_Bool\b")


# `extern "C" extern TYPE name;` -- MSVC and mwcc accept the redundant `extern`
# inside a linkage spec; g++ rejects it ("invalid use of extern in linkage
# specification"). Dropping the second `extern` is semantically identical and
# valid on every compiler. (Three //cpp src files spell it: ShadowModel::
# InitCylinder, Model::LoadTextureToVram, and one more.)
EXTERN_C_EXTERN = re.compile(r'(extern\s+"C"\s+)extern\b')


def transform(text, extern_data=False):
    """Return (new_text, n_rewrites)."""
    text, n3 = ATTRIBUTE.subn("", text)
    text, n7 = EXTERN_C_EXTERN.subn(r"\1", text)
    text, n2 = VOIDPP_ARITH.subn(voidpp_char, text)
    text, n1 = MMIO_DEREF.subn(lambda m: f"NTR_MMIO({m.group(2).strip()}, {m.group(3)})", text)
    text, n5 = mmio_ptr(text)
    text, n6 = CBOOL.subn("bool", text)
    n4 = 0
    if extern_data:
        text, n4 = EXTERN_DATA.subn(r"\1extern \2\3\4;", text)
    return text, n1 + n2 + n3 + n4 + n5 + n6 + n7


# ~110 files in the decomp are ARM assembly blocks -- CP15 cache ops, the CRT0,
# SWI wrappers, context switches. They are matched and correct, and they are also
# the one thing a host compiler can never consume. They are not transformed; they
# are reimplemented by hand in src/port/runtime.cpp. This is the port's shim
# surface, and keeping it enumerable is the point.
ASM_BLOCK = re.compile(r"^\s*(?:extern\s+\"C\"\s+)?(?:__)?asm\s", re.MULTILINE)


def is_asm(text):
    return bool(ASM_BLOCK.search(text))


# A shared header can declare a symbol with a different pointer parameter type
# than the TU that defines it uses. That was invisible while the decl_*.h
# headers were C++-mangled, because the two spellings were simply two different
# symbols; once main gave those headers C linkage (the 2026-08-03 sweep) both
# become the same extern "C" name and MSVC rejects the file outright:
#
#   include/decl_common.h  extern int func_ov002_020cfbdc(void*);
#   src/..._020cfbdc.cpp   extern "C" int func_ov002_020cfbdc(char *self)
#   -> error C2733: you cannot overload a function with 'extern "C"' linkage
#
# Neither side is wrong about the ROM -- void* and char* are one register --
# and neither src/ nor include/ may be edited for the port. So the emitted copy
# shadows the NAME across the header's include, which leaves the header
# declaring a dead alias and the TU's own definition untouched. One entry per
# symbol, listed rather than pattern-matched, so a new collision has to be
# looked at rather than silently absorbed.
#
# THE COLLIDING NAME IS NOT ALWAYS THE EMITTED ONE. func_ov078_02124cf4 defines
# itself compatibly and re-declares two OTHER functions with a different
# pointer type than decl_common.h gives them, so the entry names those two
# instead. The value is (header, names to shadow); a bare string means "shadow
# the emitted symbol itself", which is what the first three want.
HEADER_SHADOW = {
    "func_ov002_020cfbdc": "decl_common.h",   # header void*, TU char*
    "func_ov002_020d6c60": "decl_common.h",   # header (char*, void*), TU (char*, char*)
    "func_ov102_0214b248": "decl_common.h",   # header void*, TU char*  (gate 32)
    # gate 32, KING_BOB_OMB: header (char*,int)/(void*,int), TU (void*,int) for
    # both
    "func_ov078_02124cf4": ("decl_common.h",
                            ("func_ov078_02125c24", "func_0200fa8c")),
    # gate 29: decl_Particle.h declares Initialise as (void*) with C++
    # linkage, the TU defines it extern "C" over Particle__SysTracker*, and
    # MSVC reads that as overloading a function with C linkage (C2733).
    "_ZN8Particle10SysTracker10InitialiseEv": "decl_Particle.h",
    # gates 64-69, WHOMP: decl_common.h declares func_01ffb07c (void*,void*)
    # and func_020396d0 (int*,int), the TU declares them (void*,s32*) and
    # (void*,int) -- one register on the ROM, two C2733s to MSVC.
    "_ZN5Whomp13InitResourcesEv": ("decl_common.h",
                                   ("func_01ffb07c", "func_020396d0")),
}

# ---- REDUNDANT OUT-OF-LINE MEMBER REDECLARATIONS ---------------------------
#
# mwccarm tolerates re-declaring a class member out of line with no body (a
# harmless forward declaration); MSVC rejects it as C2761 "redeclaration of
# member is not allowed". A handful of recovered TUs declare their local
# struct's methods both inside the struct AND again just before the function
# body. The out-of-line copies are pure noise -- the in-struct declarations
# already give MSVC everything it needs -- so they are deleted here. Exact
# strings, hard-errored by apply_patches if the source moves.
MEMBER_REDECL = {
    # gate 67, BILL_BLASTER: func_ov079_02126f8c (daObjBkKillerdai_c::Behavior)
    "func_ov079_02126f8c": [
        ("int Platform::UpdateKillByMegaChar(short, short, short, Fix12);\n", ""),
        ("Actor* Actor_s::FindWithID(unsigned int);\n", ""),
        ("Actor* Actor_s::ClosestPlayer();\n", ""),
        ("Actor* Actor_s::Spawn(unsigned int, unsigned int, const Vector3&, "
         "const Vector3_16*, int, int);\n", ""),
        ("int Platform::IsClsnInRange(Fix12, Fix12);\n", ""),
    ],
}


# ---- DS INTEGER DIVISION -----------------------------------------------------
#
# mwccarm compiles `/` and `%` on ints to a call to __aeabi_idiv (ITCM
# 0x01ffabe4), which on a ZERO DIVISOR returns quotient = numerator and
# remainder = 0 and does not fault. x86's idiv raises instead. That is not a
# corner case in this game: 181 of the 321 particle definitions ship a zero
# emission interval, and func_0204a730 gates emission on
# `counter % interval == 0` -- which the ROM reads as "every frame".
#
# The operands are rerouted through ds_idiv/ds_imod in hal/cstd_div.c, which
# carry the ROM's semantics with the disassembly that proves them.
#
# WHY A TABLE OF EXACT STRINGS rather than a rewrite of the `/` and `%`
# tokens: finding an operator's operands in C text means matching parens,
# casts and precedence, and a regex that gets that subtly wrong would produce
# code that compiles and computes the wrong number -- the worst possible
# failure for a byte-matched decomp. Each site is named instead, and emit()
# HARD ERRORS if a listed patch no longer matches its file, so this cannot rot
# quietly: it breaks the build the moment the source moves.
#
# Only sites with a RUNTIME divisor are listed. Constant divisors (`/ 0xff`,
# `/ 255`, `/ 4`) can never be zero and are left alone.
DS_DIV = {
    "func_0204a730": [
        # the emission-interval gate, the one that crashes first
        ("(u16)(*(u16 *)((char *)self + 0x38)) % (u8)(*(u8 *)((char *)self + 0x58))",
         "ds_imod((u16)(*(u16 *)((char *)self + 0x38)), (u8)(*(u8 *)((char *)self + 0x58)))"),
        ("(r0 >> 0xc) % (u8)(*(u8 *)(cfg + 0xe))",
         "ds_imod((r0 >> 0xc), (u8)(*(u8 *)(cfg + 0xe)))"),
        ("((s32)(e2e << 8)) / (s32)e2c",
         "ds_idiv((s32)(e2e << 8), (s32)e2c)"),
    ],
    "func_0204c304": [
        ("0xffff / (int)((u32)self->h2c >> 1)",
         "ds_idiv(0xffff, (int)((u32)self->h2c >> 1))"),
        ("0xffff / self->h2c", "ds_idiv(0xffff, self->h2c)"),
    ],
    "func_0204c584": [
        ("d10 / count", "ds_idiv(d10, count)"),
        # the per-particle setup at the tail: a palette index, and the two
        # 0xffff reciprocals that become the particle's fade and scale rates.
        # b34 and +0x2c are both routinely zero in the ROM's own definitions.
        ("pal[(s >> 0x14) % pal[8]]",
         "pal[ds_imod((int)(s >> 0x14), (int)pal[8])]"),
        ("0xffff / (*(Def **)emitter)->b34",
         "ds_idiv(0xffff, (*(Def **)emitter)->b34)"),
        ("0xffff / *(u16 *)(p + 0x2c)",
         "ds_idiv(0xffff, *(u16 *)(p + 0x2c))"),
    ],
    "func_0204d294": [
        ("blueMul / denom", "ds_idiv(blueMul, denom)"),
        ("redMul / denom", "ds_idiv(redMul, denom)"),
        ("greenMul / denom", "ds_idiv(greenMul, denom)"),
    ],
    "_ZN8Particle6Jitter4FuncERNS_10EffectDataEPcR7Vector3": [
        ("(int)*(u16*)(p + 0x2e) % (int)self->unk_006",
         "ds_imod((int)*(u16*)(p + 0x2e), (int)self->unk_006)"),
    ],
}

DS_DIV_DECL = """/* hostgen: DS integer-division semantics, see hal/cstd_div.c */
#ifdef __cplusplus
extern "C" {
#endif
int ds_idiv(int, int);
int ds_imod(int, int);
#ifdef __cplusplus
}
#endif
"""


# ---- CALLING CONVENTION: C++ VIRTUAL CALLS ON C VTABLES ----------------------
#
# On ARM a virtual call and a plain function-pointer call are the SAME thing:
# `this` goes in r0 and the argument in r1 either way. MSVC splits them --
# __thiscall puts `this` in ECX and leaves the argument on the stack, cdecl
# puts both on the stack -- so a vtable that the decomp reaches BOTH ways
# cannot be served by one host function.
#
# The particle Callback vtables are exactly that. Slot 1 (OnUpdate) is
# dispatched cdecl through a typedef'd pointer in func_02021bec:
#
#     e->f10->vt->m[1](e->f10, e->fc, m)
#
# and slot 0 is dispatched cdecl in Particle::System::New:
#
#     o->vtable[0](o, p)
#
# but func_02021d1c declares a local shadow `struct Callback { virtual void
# Run(void *); }` and calls `p6->Run(...)`, which MSVC compiles as a thiscall
# virtual. Hosted unpatched, the callback reads its System argument off the
# stack slot that held `this` and dereferences null on the first landing puff.
#
# The whole subsystem has exactly ONE such call, so the fix is to make that
# one site dispatch the way its twenty-five siblings do -- explicitly, through
# vptr[0], cdecl -- rather than to give slot 0 a second calling convention.
# The local `virtual` declaration stays; only the call changes.
VIRTUAL_CALL = {
    "func_02021d1c": [
        ("p6->Run(*(void **)(self + 0xc));",
         "(*(void (***)(void *, void *))p6)[0]"
         "((void *)p6, *(void **)(self + 0xc));"),
    ],
}


def apply_patches(text, sym, table, what, decl=""):
    """Exact-string patches, with a hard error if one stops matching."""
    pats = table.get(sym)
    if not pats:
        return text, 0
    n = 0
    for old, new in pats:
        if old not in text:
            sys.exit(
                "hostgen: %s: %s patch no longer matches:\n  %s\n"
                "The source moved. Re-derive the expression -- do NOT drop "
                "the patch, it is load-bearing." % (sym, what, old))
        n += text.count(old)
        text = text.replace(old, new)
    return (decl + text) if decl else text, n


def ds_div_patch(text, sym):
    """Reroute the named divisions through the DS-semantics helpers."""
    return apply_patches(text, sym, DS_DIV, "DS_DIV", DS_DIV_DECL)


def virtual_call_patch(text, sym):
    """Make a C++ virtual call on a C vtable dispatch cdecl, like its peers."""
    return apply_patches(text, sym, VIRTUAL_CALL, "VIRTUAL_CALL")


def member_redecl_patch(text, sym):
    """Delete redundant out-of-line member redeclarations (mwcc-tolerated,
    C2761 to MSVC)."""
    return apply_patches(text, sym, MEMBER_REDECL, "MEMBER_REDECL")


def shadow_header_decl(text, sym, spec):
    """Hide the shared header's declaration of one or more names across its
    include. `spec` is a header name, or (header, names)."""
    if isinstance(spec, tuple):
        header, names = spec
    else:
        header, names = spec, (sym,)
    inc = '#include "%s"' % header
    if inc not in text:
        return text, 0
    pre = "".join("#define %s %s__hdrshadow\n" % (n, n) for n in names)
    post = "".join("\n#undef %s" % n for n in names)
    return text.replace(inc, pre + inc + post, 1), 1


def emit(src_path, out_dir, decomp_root, extern_data=False):
    text = src_path.read_text(encoding="utf-8", errors="replace")
    # The decomp marks C++ files with a leading `//cpp` line; the host build
    # compiles everything as C++ anyway, so drop it.
    text = re.sub(r"\A//cpp[^\n]*\n", "", text)
    sym = src_path.stem
    if sym in HEADER_SHADOW:
        text, _ = shadow_header_decl(text, sym, HEADER_SHADOW[sym])
    text, _ = ds_div_patch(text, sym)
    text, _ = virtual_call_patch(text, sym)
    text, _ = member_redecl_patch(text, sym)
    new, n = transform(text, extern_data)
    # Everything is emitted as C++ (NTR_MMIO expands to a template proxy), but
    # a .c source's symbols must keep C linkage: the port's other slices
    # compile the decomp's .c files as real C, and mixing linkages per-symbol
    # is exactly the bug class the HAL bridges exist to catch. The decomp's
    # own headers are C-only typedefs, so including them inside the wrap is
    # sound.
    if src_path.suffix == ".c":
        new = 'extern "C" {\n' + new + '\n}  /* extern "C" (hostgen: .c source) */\n'
    rel = src_path.relative_to(decomp_root)
    out = out_dir / rel.with_suffix(".cpp")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(HEADER.format(src=rel.as_posix()) + new, encoding="utf-8")
    return out, n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("symbols", nargs="*", help="symbol names to transform (no extension)")
    # This tool lives at port/tools/hostgen.py inside the decomp itself.
    ap.add_argument("--decomp",
                    default=str(pathlib.Path(__file__).resolve().parents[2]))
    ap.add_argument("--out", default="build/host-src")
    ap.add_argument("--all", action="store_true", help="transform every file in src/")
    ap.add_argument("--extern-data", action="store_true",
                    help="turn file-scope data_XXXXXXXX definitions into "
                         "declarations (engine BSS is the HAL's -- see "
                         "EXTERN_DATA)")
    args = ap.parse_args()

    decomp = pathlib.Path(args.decomp).expanduser().resolve()
    src = decomp / "src"
    if not src.is_dir():
        sys.exit(f"no src/ under {decomp} -- pass --decomp")
    out_dir = pathlib.Path(args.out)

    if args.all:
        targets = [p for p in src.rglob("*") if p.suffix in (".c", ".cpp")]
    else:
        targets = []
        for name in args.symbols:
            hit = next((p for ext in (".c", ".cpp")
                        if (p := src / f"{name}{ext}").exists()), None)
            if hit is None:
                sys.exit(f"not found in decomp src/: {name}")
            targets.append(hit)
        if not targets:
            sys.exit("nothing to do -- pass symbol names or --all")

    total = 0
    skipped = []
    for path in targets:
        text = path.read_text(encoding="utf-8", errors="replace")
        if is_asm(text):
            skipped.append(path.stem)
            if not args.all:
                print(f"{path.name:<44}   ARM asm -- needs a host implementation")
            continue
        out, n = emit(path, out_dir, decomp, args.extern_data)
        total += n
        if not args.all:
            print(f"{path.name:<44} {n:>3} MMIO rewrite(s) -> {out}")

    if args.all:
        print(f"{len(targets) - len(skipped):,} files transformed, "
              f"{total:,} MMIO rewrites -> {out_dir}")
        print(f"{len(skipped):,} ARM asm files skipped (the port's shim surface)")
        manifest = out_dir / "asm-shims.txt"
        manifest.parent.mkdir(parents=True, exist_ok=True)
        manifest.write_text("\n".join(sorted(skipped)) + "\n", encoding="utf-8")
        print(f"  -> {manifest}")


if __name__ == "__main__":
    main()
