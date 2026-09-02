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
# THE TYPEDEF SPELLINGS AND THE INTEGER SUFFIX ARE BOTH PART OF THE PATTERN.
# types.h carries vu8/vu16/vu32/vu64 (and the signed row) for exactly these
# registers, and the decomp uses them: src/func_ov006_0210a708.c reaches
# LIGHT_COLOR as `*(vu32 *)0x40004ccu = 0x7fff;`. That line missed the old
# pattern TWICE over -- `vu32` was not in the type list, and the `u` suffix on
# the literal killed the `\b` after seven hex digits -- so the store latched
# into the mapped window and the geometry engine never got a light. See the
# LIGHTING note in port/ntr/gx.cpp. A suffix cannot make the match looser:
# `[uUlL]*` only ever consumes suffix letters, and a longer hex literal still
# fails the word boundary the same way it did before.
MMIO_DEREF = re.compile(
    r"\*\s*(\()?\s*\(\s*(?:volatile\s+)?"
    r"((?:unsigned|signed|long|short|int|char|u8|u16|u32|u64|s8|s16|s32|s64"
    r"|vu8|vu16|vu32|vu64|vs8|vs16|vs32|vs64)"
    r"(?:\s+(?:unsigned|signed|long|short|int|char))*)"
    r"\s*\*\s*\)\s*(0x0?4[0-9A-Fa-f]{6})[uUlL]*\b(?(1)\s*\))"
)

# `NTR_MMIO(vu32, ...)` would instantiate Reg<volatile unsigned>, whose
# operator T() returns a top-level-volatile value. It compiles and it means
# nothing: the proxy is the volatility. The emitted spelling drops the v.
MMIO_DEVOL = {"vu8": "u8", "vu16": "u16", "vu32": "u32", "vu64": "u64",
              "vs8": "s8", "vs16": "s16", "vs32": "s32", "vs64": "s64"}

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
             r"u8|u16|u32|u64|s8|s16|s32|s64|"
             r"vu8|vu16|vu32|vu64|vs8|vs16|vs32|vs64)"
             r"(?:\s+(?:unsigned|signed|long|short|int|char))*")
MMIO_BIND = re.compile(
    r"\b(\w+)\s*=\s*\(\s*(?:volatile\s+)?(?:const\s+)?(" + MMIO_TYPE + r")"
    r"\s*\*\s*\)\s*(0x0?4[0-9A-Fa-f]{6})[uUlL]*\s*;")
MMIO_WIDTH = {
    "char": 1, "signed char": 1, "unsigned char": 1, "u8": 1, "s8": 1,
    "short": 2, "short int": 2, "signed short": 2, "unsigned short": 2,
    "unsigned short int": 2, "u16": 2, "s16": 2,
    "int": 4, "signed": 4, "signed int": 4, "unsigned": 4, "unsigned int": 4,
    "long": 4, "long int": 4, "unsigned long": 4, "u32": 4, "s32": 4,
    "long long": 8, "signed long long": 8, "unsigned long long": 8,
    "unsigned long long int": 8, "u64": 8, "s64": 8,
    "vu8": 1, "vs8": 1, "vu16": 2, "vs16": 2, "vu32": 4, "vs32": 4,
    "vu64": 8, "vs64": 8,
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
            etype = MMIO_DEVOL.get(ctype, ctype)
            edits += [(a, b, f"NTR_MMIO({etype}, {c:#x})") for a, b, c in mine]

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
    # `void* data_0209f394[];` (HUD::InitResources): the star is glued to
    # the keyword, so it is part of the alternative rather than of the
    # trailing `\**` (lane shadow-A)
    r"void\**|"
    r"[A-Z]\w*)[ \t]+)+\**[ \t]*)(data_(?:ov\d+_)?[0-9a-f]{6,8})"
    r"([ \t]*(?:\[[^\];=]*\])?)[ \t]*;",
    re.MULTILINE)


# ENGINE BSS SPELLED BY ROLE: `char gGlobalA;` at file scope.
#
# EXTERN_DATA above catches engine BSS only when the decomp spelled it
# `data_<hex>`, and a delinked TU does not have to. A decompiler who knew what
# the address WAS is free to name it: ActorBase::AfterCleanupResources writes
#
#     char gGlobalA;                    /* 0x020a4b6c, the scene tree     */
#     char gGlobalB;                    /* 0x020a4ba8, the cleanup list   */
#     struct Heap* Memory_gameHeapPtr;  /* 0x020a0eac                     */
#
# which is a perfectly good delinked TU -- the addresses come back from the
# relocation table and the declared width is whatever that one function needed.
# It is also the vacuous-pass shape, and the worst one in this file: the name
# does not look like storage, so --extern-data left it alone, the TU LINKED
# CLEAN, and at run time the teardown unlinked the actor from a private
# one-byte object and freed it against a null heap. No link error, no crash at
# the seam, just a scene tree that quietly never lost its dead nodes.
#
# So the map is by ADDRESS, taken from the decomp's own comment on each line,
# and the rewrite is the same one EXTERN_DATA performs: the definition becomes
# a declaration of the hosted symbol, at the width the src itself declared,
# and the role name is #define'd onto it so every use in the body -- including
# `&gGlobalA`, which is how both list heads are passed -- resolves to the
# engine's storage.
#
# Listed rather than pattern-matched, like HEADER_SHADOW: a role name is a
# human judgement about which address a word means, and the next one has to be
# looked at rather than guessed from its spelling.
ROLE_DATA_HOSTS = {
    "gGlobalA":           "data_020a4b6c",
    "gGlobalB":           "data_020a4ba8",
    "Memory_gameHeapPtr": "data_020a0eac",
}

# One character class and one quantifier, deliberately: the obvious spelling
# of "a type is a run of words and stars" nests quantifiers and backtracks
# exponentially on every non-matching line in the file.
ROLE_DATA = re.compile(
    r"^([ \t]*)(?!extern\b|static\b|typedef\b|return\b)"
    r"([A-Za-z_][\w \t*]*[ \t*])(" + "|".join(ROLE_DATA_HOSTS) + r")"
    r"([ \t]*(?:\[[^\];=]*\])?)[ \t]*;",
    re.MULTILINE)


def role_data_extern(m):
    """One role-named engine global -> a declaration of its hosted symbol."""
    indent, typ, role, arr = m.group(1), m.group(2), m.group(3), m.group(4)
    host = ROLE_DATA_HOSTS[role]
    return ("%sextern %s%s%s;  /* hostgen ROLE_DATA: %s lives here */\n"
            "%s#define %s %s"
            % (indent, typ, host, arr, role, indent, role, host))


# C99's `_Bool` keyword, which MSVC's C++ front end does not have. Everything
# hostgen emits is compiled as C++, so the spelling has to become `bool` --
# same width and same argument passing under cdecl on x86, so no ABI change.
# Exactly one file in the decomp writes it (Particle::Callback::OnUpdate's
# `_Bool active`), but the rewrite is general because the next one will not be
# announced.
CBOOL = re.compile(r"\b_Bool\b")


def transform(text, extern_data=False):
    """Return (new_text, n_rewrites)."""
    text, n3 = ATTRIBUTE.subn("", text)
    text, n2 = VOIDPP_ARITH.subn(voidpp_char, text)
    text, n1 = MMIO_DEREF.subn(
        lambda m: "NTR_MMIO(%s, %s)"
                  % (MMIO_DEVOL.get(m.group(2).strip(), m.group(2).strip()),
                     m.group(3)),
        text)
    text, n5 = mmio_ptr(text)
    text, n6 = CBOOL.subn("bool", text)
    n4 = n7 = 0
    if extern_data:
        text, n4 = EXTERN_DATA.subn(r"\1extern \2\3\4;", text)
        text, n7 = ROLE_DATA.subn(role_data_extern, text)
    return text, n1 + n2 + n3 + n4 + n5 + n6 + n7


# ~110 files in the decomp are ARM assembly blocks -- CP15 cache ops, the CRT0,
# SWI wrappers, context switches. They are matched and correct, and they are also
# the one thing a host compiler can never consume. They are not transformed; they
# are reimplemented by hand in src/port/runtime.cpp. This is the port's shim
# surface, and keeping it enumerable is the point.
ASM_BLOCK = re.compile(r"^\s*(?:extern\s+\"C\"\s+)?(?:__)?asm\s", re.MULTILINE)


def is_asm(text):
    return bool(ASM_BLOCK.search(text))


# ---- INLINE ASM EXCISION -----------------------------------------------------
#
# The rule above is right about the ~110 whole-file ARM shims: a CP15 cache op
# or a context switch has no C semantics to substitute, so it is reimplemented
# by hand and stays enumerable. It is wrong about one narrower shape -- a C
# function with ONE small asm block in the middle of it, where the block exists
# purely to make mwccarm emit a particular instruction sequence and the thing
# it computes is plain C.
#
# Player::InitResources is the whole class. Its match note says why the block
# is there:
#
#     Heap zero of operator_new(0x14) at +0x588 uses mwccarm CLEAR shape
#     (4x strb post-inc). Pure C never emits that for a heap pointer under
#     -O4,p (only stack u8[N]={0} does); small asm block reproduces it.
#
# So the asm is a CODEGEN instruction, not a computation: five iterations of
# four post-incrementing byte stores over the fresh 0x14-byte allocation, which
# is memset(q, 0, 0x14) and nothing else. The host has no reason to care which
# instructions a DS compiler picked to reach that state, and refusing the file
# over it costs the whole function -- which is why the hand-written host copy
# in port/unmatched/ exists, byte-identical to the matched source apart from
# these seven lines.
#
# Excision is exact-text and per-symbol, through the same apply_patches that
# hard-errors when a patch stops matching. That is deliberate: an asm block
# whose text drifted is an asm block nobody has re-read, and substituting C for
# instructions you have not looked at is exactly the vacuous pass this file
# keeps trying to avoid. Adding an entry here is a claim that someone read the
# instructions and the C says the same thing.
#
# The other three asm-block files in src/ are NOT candidates and must not be
# added without the same reading: ov091/ov009 are NONMATCHING hand-asm hatches
# (there is no C semantics to recover -- the asm IS the decomp) and
# func_0205950c writes CPSR (a host has no such register).
ASM_EXCISION = {
    "_ZN6Player13InitResourcesEv": [
        ("""    asm {
      mov r1, #5
      mov r0, #0
    Lzero:
      strb r0, [q], #1
      strb r0, [q], #1
      strb r0, [q], #1
      strb r0, [q], #1
      subs r1, r1, #1
      bne Lzero
    }""",
         "    /* hostgen ASM_EXCISION: the matched source zeroes this fresh\n"
         "       0x14-byte allocation with mwccarm's CLEAR shape (5 x 4 strb\n"
         "       post-inc) because pure C will not emit that for a heap\n"
         "       pointer under -O4,p. The state it reaches is a 20-byte\n"
         "       zero. */\n"
         "    for (int zi_ = 0; zi_ < 0x14; ++zi_) q[zi_] = 0;"),
    ],
}


def asm_excision_patch(text, sym):
    """Substitute C semantics for the codegen-only asm blocks we have read."""
    return apply_patches(text, sym, ASM_EXCISION, "ASM_EXCISION")


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
    # run linkw wave 7, lane w7b, MR_I_PROJECTILE (264): decl_common.h:2793
    # declares func_ov071_02121ba4 `(void*)` inside its extern "C" block while
    # src/func_ov071_02121ba4.cpp defines it `(char*)`. One register on the
    # ROM, C2733 to MSVC -- the func_ov102_0214b248 case exactly.
    "func_ov071_02121ba4": "decl_common.h",
    # lane LINKMG, dScMgBase_c slots 30 and 29: decl_common.h:2352-2353 declare
    # both `(void*)` inside its extern "C" block while src/func_ov004_020aeed8.cpp
    # defines `(char*)` and src/func_ov004_020af094.cpp defines `(Obj*)`. One
    # register on the ROM, C2733 to MSVC -- the func_ov102_0214b248 case
    # exactly. unmatched/MgBase_DeclConflict.cpp used to carry both bodies with
    # the parameter retyped; this entry retires that file.
    "func_ov004_020aeed8": "decl_common.h",
    "func_ov004_020af094": "decl_common.h",
    # run rel0215 wave 3, lane w3-a2, TTC_ROTATING_CUBE/PRISM (108/109):
    # decl_common.h:2777-2778 declare func_ov065_021198a0 and
    # func_ov065_0211990c `(char*)` inside its extern "C" block, while
    # src/_ZN15TtcRotatingCube13InitResourcesEv.cpp re-declares both `(void*)`
    # in its own extern "C" block and calls them with `(void*)this`. One
    # register on the ROM, two C2733s and two C2664s to MSVC -- the
    # _ZN5Whomp13InitResourcesEv case exactly, down to the colliding names
    # being OTHER functions than the emitted symbol. Both are themselves on
    # this cluster's slice (src/func_ov065_021198a0.cpp defines it `(void*)`
    # and src/func_ov065_0211990c.c `(void*)`), so the header is the odd one
    # out and shadowing it is what leaves every definition untouched.
    "_ZN15TtcRotatingCube13InitResourcesEv": ("decl_common.h",
                                              ("func_ov065_021198a0",
                                               "func_ov065_0211990c")),
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


# ---- EXTERNAL DATA LEFT OUTSIDE THE TU'S OWN extern "C" BLOCK -----------------
#
# A recovered TU can declare its external data symbols ABOVE its own
# `extern "C"` block, so MSVC gives them C++ linkage and asks the linker for
# ?data_ov070_02123530@@3USharedFilePtr@@A and its siblings, while the port's
# mount emits those names with C linkage as every other consumer spells them --
# so each such declaration is an LNK2019. mwccarm did not care: the ROM carries
# these names under a C++ mangling of its own, and the decomp author's placement
# was a matched-source detail, not a linkage decision. The fix moves the
# `extern "C" {` opener up so the data declarations fall inside it and take C
# linkage. The body is untouched; this is purely the declaration-linkage
# spelling that unmatched/FlyGuy_InitResources.cpp used to carry by hand (the
# Coffin_InitResources.cpp shape). Exact strings, hard-errored by apply_patches
# if the source moves.
EXTERN_C_DATA = {
    "_ZN6FlyGuy13InitResourcesEv": [
        ("typedef struct PMF PMF;\nextern SharedFilePtr data_ov070_02123530;",
         'typedef struct PMF PMF;\nextern "C" {\n'
         "extern SharedFilePtr data_ov070_02123530;"),
        ('extern PMF data_ov070_0212359c;\nextern "C" {\n',
         "extern PMF data_ov070_0212359c;\n"),
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
    # PathLift's path-follow tick: dv = cstd::fdiv(len2, speed) / 0x1000 is 0
    # whenever a path segment is shorter than one frame of travel, and the two
    # angle-step divisions below then divide by it. The ROM's AEABI idiv answers
    # x/0 = 0, x86 idiv faults c0000094 (measured on the VS castle grounds,
    # level 51, actor 0x1f, reproduced with SM64DS_LEVEL=51). `/ 0x1000` above
    # is a constant divisor and stays. This retires unmatched/VS_PathDivGuard.cpp.
    "func_ov002_020ef670": [
        ("ad2 / dv", "ds_idiv(ad2, dv)"),
        ("ad1 / dv", "ds_idiv(ad1, dv)"),
    ],
    # Lane shadow-A: Animation::Advance's looping arm, `(frame + speed + len)
    # % len`, and len is ZERO on a ModelAnim that carries a model and no
    # animation (the VS cap: WaterfallMist seats only the BMD for mType 4 and
    # calls Advance every visible frame). The ROM's `bl 0x01ffabe4` answers
    # n % 0 = 0 and parks the frame at 0; x86 idiv faults (measured:
    # c0000094 at Advance+0x1e, VS map 2 level 43, lane CAPSHOW). This entry
    # retires port/unmatched/Animation_AdvanceDivGuard.cpp, which was the
    # matched source with exactly this one modulus rerouted.
    "_ZN9Animation7AdvanceEv": [
        ("currFrame = (currFrame + speed + (int)len) % (int)len;",
         "currFrame = ds_imod(currFrame + speed + (int)len, (int)len);"),
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


# ---- MMIO REACHED THROUGH ROLE-NAMED EXTERNS ---------------------------------
#
# One register-access shape the two passes above cannot see: a TU that declares
# the register as a C extern BY ROLE NAME and lets the linker bind the address.
#
#     extern volatile u16 SQRTCNT;     /* 0x040002b0 */
#     extern volatile s32 SQRT_RESULT; /* 0x040002b4 */
#     sqrtcnt = &SQRTCNT;
#     while (*sqrtcnt & 0x8000);
#     ... (s64)SQRT_RESULT ...
#
# On the ROM those externs were resolved to the I/O addresses at link time.
# The host cannot do that (no image section lives at 0x04000000; the pages are
# VirtualAlloc'd at runtime by ntr/io.cpp), so the access is rewritten to the
# same NTR_MMIO proxy the literal shapes get, register address taken from the
# TU's own comment and re-checked against GBATEK (SQRTCNT 0x40002b0,
# SQRT_RESULT 0x40002b4). Exact strings, hard-errored like DS_DIV, because a
# role name is program text and a regex over it would be guessing.
#
# src/func_02053130.c is the only file in src/ with this shape today (swept
# 2026-08-13: `extern volatile` over the 0x04xxxxxx window comments). Its
# sibling unit drivers func_02053008 / func_020531a4 / func_02052fdc bind the
# registers by literal cast or bound pointer and need no entry here.
MMIO_EXTERN = {
    "func_02053130": [
        # the busy-bit spin: SQRTCNT through the proxy; the local pointer
        # binding goes with it (keeping it would keep the &SQRTCNT reference)
        ("    sqrtcnt = &SQRTCNT;\n    while (*sqrtcnt & 0x8000)\n        ;",
         "    while (NTR_MMIO(u16, 0x40002b0) & 0x8000)\n        ;"),
        # the result read, at the width the ROM read it
        ("(s64)SQRT_RESULT", "(s64)(s32)NTR_MMIO(s32, 0x40002b4)"),
    ],
}


def mmio_extern_patch(text, sym):
    """Route role-named register externs through the NTR_MMIO proxy."""
    return apply_patches(text, sym, MMIO_EXTERN, "MMIO_EXTERN")


# ---- INT BODIES THAT FALL OFF THE END ----------------------------------------
#
# mwccarm lets a non-void function end without a return (the value is whatever
# r0 held); MSVC makes that C4716, a hard error. The recovered TU below ends on
# a call to a face the port defines VOID (Actor::KillAndTrackInDeathTable, a
# real void method in matched src), so the ROM's r0 at that point is the leftover
# of an interior call -- unrecoverable without giving the whole KATIDT chain int
# faces. The patch returns 1, the value every sibling state handler in ov070
# returns, and the caveat is real but bounded: the one reader of this return is
# FlyGuy's state dispatcher, and FlyGuy is unregistered (no bootable level
# spawns it), so the value is dead until a FlyGuy lane lands -- which should
# re-derive it before trusting it.
#
# THE SECOND ENTRY IS THE CASE THIS TABLE WAS ADVERTISED FOR AND THEN NOT USED.
# port/slice_ov096.txt section D and port/unmatched/Pokey_HostSites.cpp both
# record a fall-off-the-end body (func_ov096_02135e2c) that lane w5-c's
# mechanism could have kept in the linkage count, and both say out loud that
# the lane host-copied it instead only because hostgen.py was outside its
# append scope -- "the only one given up for a reason that is not a real wall".
# func_ov081_02126950 is the SAME BODY one overlay over (the ov081/ov096
# collision helper: func_02038414, then a floor-normal slope correction, then
# a wall-normal read), so lane w3-b takes the cheap route rather than making
# the same trade a second time.
#
# THE RETURNED VALUE IS UNOBSERVABLE, checked rather than assumed, both ends:
#   * THE ROM SETS NO r0 ON THE WAY OUT. The tail at 0x02126a08..0x02126a1c is
#     `add r1,sp,#0xc / add r0,r0,#4 / bl 0x02037dcc (SurfaceInfo::CopyNormalTo,
#     void) / add sp,sp,#0x1c / pop {r4-r7,lr} / bx lr`, and the conditional
#     early exit at 0x021269f4 leaves whatever the IsOnWall test returned.
#     Neither path computes a result.
#   * ALL THREE CALLERS DISCARD IT. The ov081 relocs name exactly three call
#     sites -- 0x021270f8, 0x021272e8, 0x02127478 -- and their matched sources
#     (src/func_ov081_02127070.cpp:46, src/func_ov081_02127240.cpp:31,
#     src/func_ov081_02127440.cpp:16) each call it as a bare statement.
# So 0 is as faithful as any other value, and the `int` in the TU's own
# signature is the recovery's placeholder rather than something the ROM
# produces. A later lane that owns port/slice_ov096.txt should give the twin
# the same treatment and delete its host copy; this lane does not edit another
# lane's slice.
# run rel0215 wave 3 (lane w3-e) adds the ov019 entry (third in the table
# as merged), and its caveat is
# smaller than the first's because the deadness is measured on both sides.
# func_ov019_0211140c is RacingPenguin's ground/wall normal helper (ov019
# 0x0211140c). On the ROM it ends `add sp,#0x1c; pop {r4-r7,lr}; bx lr` with r0
# holding whatever SurfaceInfo::CopyNormalTo -- a void function -- left behind,
# and ALL FOUR of its call sites discard the result: func_ov019_02111dec:31,
# func_ov019_0211197c:44 and :79, func_ov019_021117a8:33. So there is no reader
# to re-derive it for, on the host or on the DS.
FALLS_OFF_RETURN = {
    "func_ov070_0211f0a4": [
        ("    a->KillAndTrackInDeathTable();\n}",
         "    a->KillAndTrackInDeathTable();\n"
         "    return 1;  /* hostgen FALLS_OFF_RETURN: see the table's note */\n"
         "}"),
    ],
    "func_ov081_02126950": [
        ("_ZNK12WithMeshClsn13GetWallResultEv(clsn)+4, n1);\n    }\n}",
         "_ZNK12WithMeshClsn13GetWallResultEv(clsn)+4, n1);\n    }\n"
         "    return 0;  /* hostgen FALLS_OFF_RETURN: see the table's note */\n"
         "}"),
    ],
    # The anchor is the WALL branch's CopyNormalTo, the one that writes n1; the
    # floor branch above it writes n0 and is not matched by this text.
    "func_ov019_0211140c": [
        ("        _ZNK11SurfaceInfo12CopyNormalToER7Vector3("
         "(char*)_ZNK12WithMeshClsn13GetWallResultEv(clsn)+4, n1);\n    }\n}",
         "        _ZNK11SurfaceInfo12CopyNormalToER7Vector3("
         "(char*)_ZNK12WithMeshClsn13GetWallResultEv(clsn)+4, n1);\n    }\n"
         "    return 1;  /* hostgen FALLS_OFF_RETURN: see the table's note */\n"
         "}"),
    ],
}


def falls_off_return_patch(text, sym):
    """Give a falls-off-the-end int body the documented return."""
    return apply_patches(text, sym, FALLS_OFF_RETURN, "FALLS_OFF_RETURN")


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
    # Lane shadow-A: Model::LoadAndSetFile's middle. The matched source
    # dispatches DoSetFile through a LOCAL shadow class with three virtuals,
    # `self->v2(file, c, d)`: slot 2 in ROM/Itanium numbering, where a
    # destructor takes TWO slots (D1, D0). MSVC gives a destructor one slot,
    # so the host's _ZTV5Model has DoSetFile at 1 and UpdateVerts at 2, and
    # the shadow's "slot 2" walked a fresh Model's null vertex list inside the
    # first Tree's InitResources. hal/cxxname_bridge.cpp's double-fill trick
    # (Render, slots 4 and 5) cannot serve a slot that means two live things,
    # so the call is resolved at the caller: DoSetFile is slot 1 in every
    # host model table (_ZTV5Model, _ZTV9ModelAnim, _ZTV10ModelAnim2), and the
    # slots are __fastcall thunks (ecx = this, the dummy edx absorbs
    # fastcall's second register), the convention hal/actor_vtables.cpp set.
    # The third patch is a HOST SEAM and not part of the slot fold: the tail
    # is the ROM's shrink-to-fit (func_02017060 -> Heap::Reallocate), which the
    # port declines by default on every model path (hal/level_boot.cpp's
    # port_model_shrink_enabled, SM64DS_MODEL_SHRINK=1 turns it back on), and
    # the retired host copy carried the same switch. Retires
    # port/unmatched/func_02016ff4_hostcopy.cpp.
    "func_02016ff4": [
        ('extern "C" int func_02017060(BMD_File *f);',
         'extern "C" int func_02017060(BMD_File *f);\n'
         'extern "C" int port_model_shrink_enabled(void);'),
        ("ret = self->v2(file, c, d);",
         "ret = ((int (__fastcall *)(void *, void *, BMD_File *, int, int))"
         "(*(void ***)self)[1])(self, 0, file, c, d);"),
        ("if (ret) func_02017060(file);",
         "if (ret && port_model_shrink_enabled()) func_02017060(file);"),
    ],
    # Lane shadow-A: two of the four ActorBase::Process wrappers. Each passes
    # three mwcc pointer-to-member-functions, static {vtable byte offset, 1}
    # records at 0x02099e74..0x02099ecc, into
    #
    #     ActorBase::Process(self, <main>, <before>, <after>)
    #
    # and on the host those three globals are zeroed storage
    # (hal/player_bridges.cpp) because MSVC has no representation for the
    # mwcc pair. The records are static and the ROM's bytes name the slots:
    # 0x02099ebc/ec4/e94 = {0x00,1} {0x04,1} {0x08,1} -> InitResources,
    # BeforeInitResources, AfterInitResources (func_0204335c, the init
    # Process); 0x02099ea4/eac/eb4 = {0x18,1} {0x1c,1} {0x20,1} -> Behavior,
    # BeforeBehavior, AfterBehavior (func_02043288, the per-frame tick). The
    # patch inlines Process's own control flow over those slots (the body of
    # src/_ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE.cpp, unchanged: before,
    # then main, then after(code)), through the same __fastcall thunk
    # convention as func_02016ff4 above. Retires
    # port/unmatched/func_0204335c_hostcopy.cpp and func_02043288_hostcopy.cpp.
    # The other two wrappers (func_0204322c render 9/10/11, func_020432e4
    # cleanup 3/4/5) keep their host copies: the render one carries the slot-5
    # Virtual18 ruling and is not this lane's.
    "func_0204335c": [
        ("    return _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE(\n"
         "        self, data_02099ebc, data_02099ec4, data_02099e94);",
         "    /* hostgen VIRTUAL_CALL: Process over slots 1/0/2, see the table */\n"
         "    void **vt = *(void ***)self;\n"
         "    int r = ((int (__fastcall *)(void *, void *))vt[1])(self, 0);\n"
         "    unsigned code;\n"
         "    if (r != 0) {\n"
         "        r = ((int (__fastcall *)(void *, void *))vt[0])(self, 0);\n"
         "        code = r == -1 ? 3u : r == 1 ? 2u : 1u;\n"
         "    } else {\n"
         "        code = 0;\n"
         "    }\n"
         "    ((void (__fastcall *)(void *, void *, unsigned))vt[2])(self, 0, code);\n"
         "    return r;"),
    ],
    "func_02043288": [
        ("    return _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE(\n"
         "        self, data_02099ea4, data_02099eac, data_02099eb4);",
         "    /* hostgen VIRTUAL_CALL: Process over slots 7/6/8, see the table */\n"
         "    void **vt = *(void ***)self;\n"
         "    int r = ((int (__fastcall *)(void *, void *))vt[7])(self, 0);\n"
         "    unsigned code;\n"
         "    if (r != 0) {\n"
         "        r = ((int (__fastcall *)(void *, void *))vt[6])(self, 0);\n"
         "        code = r == -1 ? 3u : r == 1 ? 2u : 1u;\n"
         "    } else {\n"
         "        code = 0;\n"
         "    }\n"
         "    ((void (__fastcall *)(void *, void *, unsigned))vt[8])(self, 0, code);\n"
         "    return r;"),
    ],
}


# ---- OPEN-CODED mwcc MEMBER-POINTER CALLS -----------------------------------
#
# Lane LINKMG. Six minigame TUs spell the mwcc pointer-to-member pair as two
# plain ints (`struct Ent { int a; int b; }`, `int e[2]`) and decode it by hand:
# obj = this + (adj >> 1), virtual bit in adj & 1, code word either an address
# or a vtable byte offset. That is the ROM's five-instruction sequence
# transcribed, and MSVC compiles every line of it as it stands -- there is no
# member-pointer TYPE anywhere, so the 4-byte-vs-8-byte stride problem the
# host-copied dispatchers exist for does not arise. The ONE line MSVC cannot
# make behave is the call through the decoded word, because the word is a DS
# code address. The port already owns the answer to that: the per-class
# address switches in port/unmatched/Mg*_StateDispatch.cpp, entered through
# port_mg_<class>_call0/1, which apply the ROM's null-code guard, refuse a
# nonzero adjustment nobody has measured, and report an unknown address.
#
# So each entry swaps exactly that call line for a call into the seam and
# leaves the src decode (and the by-hand adjustment arithmetic, which the seam
# re-checks) untouched. The receiver is the src's own adjusted `obj`/`thisp`,
# the code word is the src's own `e->a`/`e[0]`/`e->off`, and the argument is
# the src's own loop counter. Exact-string, like VIRTUAL_CALL: a src edit that
# moves the line is a hard error here, not a silent fall-through to the DS
# address. The declarations ride in front of the text; a .c source gets them
# inside hostgen's extern "C" wrap and a .cpp source already spells its own,
# so both are extern "C" explicitly.
MG_PMF_CALL_DECL = """extern "C" void port_mg_pachinko_call1(void *, unsigned, int, int);
extern "C" void port_mg_bomroom_opencoded_call0(void *, unsigned, int);
extern "C" void port_mg_panel_call0(void *, unsigned, int);
extern "C" void port_mg_panel_call1(void *, unsigned, int, int);
"""
MG_PMF_CALL = {
    # dScMgPachinko_c, tables 02142624 / 02142644 / 02142694, arity 1
    "func_ov006_020fc7d0": [
        ("      ((void(*)(void*,int))fn)(obj,i);",
         "      port_mg_pachinko_call1(obj, (unsigned)e->a, adj, i);"),
    ],
    "func_ov006_020fe248": [
        ("      ((void(*)(void*,int))fn)(obj,i);",
         "      port_mg_pachinko_call1(obj, (unsigned)e->a, adj, i);"),
    ],
    "func_ov006_020fda7c": [
        ("      ((void(*)(void*,int))fn)(obj,i);",
         "      port_mg_pachinko_call1(obj, (unsigned)e->a, adj, i);"),
    ],
    # dScMgBomroom_c, table 021416a0, arity 0 -- the counting entry, so
    # hal/scene_mg_bomroom.cpp's census still says how often this shape ran
    "func_ov006_020d8f98": [
        ("  f(obj);",
         "  port_mg_bomroom_opencoded_call0(obj, (unsigned)e[0], off);"),
    ],
    # dScMgPanel_c: slot 6 Behavior over 02142888 (arity 0) and the round-end
    # state over 02142840 (arity 1)
    "func_ov006_02107358": [
        ("  ((void(*)(void*))fn)(obj);",
         "  port_mg_panel_call0(obj, (unsigned)e->a, adj);"),
    ],
    "func_ov006_02106ca4": [
        ("        fn(thisp, i);",
         "        port_mg_panel_call1(thisp, (unsigned)e->off, adj, i);"),
    ],
    # Lane shadow-A: the camera half of the kuppa script's command dispatch.
    # The matched source seeds data_0209b138[39] from 39 static
    # pointer-to-member records and calls
    #
    #     (obj->*data_0209b138[msg[6]])(msg + 7, a2, a3)
    #
    # On the host those 39 records are hal/ptr_tables.cpp's {host handler,
    # 0} pairs (real host function pointers, the ROM's two-word record
    # layout), data_0209b138 is hal/auto_bss.cpp's 0x138-byte span, and the
    # handlers are C-linkage cdecl functions that take the camera as an
    # ordinary first argument. `Obj` is a complete single-inheritance struct,
    # so MSVC's PMF for it is one code word and every one of the 39 seeding
    # assignments copies word 0 of a record -- the host handler -- into the
    # table at the host stride. The ONE line that cannot behave is the call:
    # MSVC would dispatch it __thiscall. The patch reads the seeded word back
    # and calls it cdecl with the receiver first, which is what the ROM's own
    # call sequence does in r0. Retires port/unmatched/func_02008550_hostcopy.cpp,
    # which was the same body with the 39 assignments spelled as a loop.
    "func_02008550": [
        ("    return (obj->*data_0209b138[msg[6]])(msg + 7, a2, a3);",
         "    return ((int (*)(void *, unsigned char *, int, int))"
         "*(void **)&data_0209b138[msg[6]])(obj, msg + 7, a2, a3);"),
    ],
    # Lane shadow-A: the camera state machine's two dispatchers. The State
    # objects at 0x0209b008.. are mwcc member-function pairs {code address,
    # this-delta} that __sinit_02073a24 copies from relocated data; on the
    # host the code word is a DS address and hal/camera_states.cpp's
    # hal_call_camera_state_fn translates it through the address switch in
    # camera_states.inc (every delta in the ROM's pairs is 0). Each entry
    # swaps the one PMF call for that seam, reading the same word the src
    # reads, and adds the seam's declaration at file scope (the sources are
    # C++, so a block-scope declaration would take C++ linkage). onEnter is
    # word 0 of the State (func_0200cae4); main is word 2 (func_0200ca50,
    # the src's `obj + 8`). Retires port/unmatched/func_0200cae4_hostcopy.cpp
    # and func_0200ca50_hostcopy.cpp.
    "func_0200cae4": [
        ("extern \"C\" int func_0200cae4(C* c){",
         "extern \"C\" int hal_call_camera_state_fn(void *self, unsigned ds_addr);\n"
         "extern \"C\" int func_0200cae4(C* c){"),
        ("  return (c->**p)();",
         "  return hal_call_camera_state_fn(c, *(unsigned *)p);"),
    ],
    "func_0200ca50": [
        ("extern \"C\" int func_0200ca50(C *self)\n{",
         "extern \"C\" int hal_call_camera_state_fn(void *self, unsigned ds_addr);\n"
         "extern \"C\" int func_0200ca50(C *self)\n{"),
        ("            r5 = (self->**pp)();",
         "            r5 = hal_call_camera_state_fn(self, *(unsigned *)pp);"),
    ],
}


# ---- PLAYER STATE-MACHINE mwcc MEMBER-POINTER DISPATCH ----------------------
#
# Three Player state functions read a per-character/per-kind row that the ROM's
# sinit copied out of a code-pointer table, and dispatch it as an mwcc member
# pointer: obj = this + (word1 >> 1), the virtual bit is word1 & 1, and the code
# word (word0) is either a DS code address or a DS vtable BYTE OFFSET. Hosted
# raw, the non-virtual branch calls a ROM code address that on the host lands
# inside the mounted ov002 DATA image and jumps into mapped data -- that is the
# walljump crash St_WallJump_Main used to take (row0 = 0x020e200c for Mario).
# The port owns the answer: hal_call_state_fn(self, ds_addr) maps the DS code
# word to the hosted body, applies the ROM's null guard, and reports an unhosted
# address as a loud no-op instead of a wild jump. Each entry swaps exactly the
# dispatch for that seam call, the Player_St_*.cpp host copies' treatment, and
# leaves the row decode untouched. Exact-string, like VIRTUAL_CALL / MG_PMF_CALL.
#
# St_Jump_Main keeps its VIRTUAL branch a raw host-vtable walk (its table's ptr
# words are filled and the host vtables are runtime-filled with host bodies).
# St_NoControl_Init and St_WallJump_Main route the virtual branch through the
# seam too: their ptr words are all zero, so the branch is dead in the ROM's own
# data and word0 there would be a DS vtable byte offset, meaningless against a
# host vtable pointer.
CALL_STATE_FN_DECL = 'extern "C" int hal_call_state_fn(void *, unsigned);\n'
CALL_STATE_FN = {
    "_ZN6Player12St_Jump_MainEv": [
        ("      int (*f)(void*);\n      if (v & 1) {\n"
         "        f = *(int (**)(void*))((char*)(*(int**)p2) + row[0]);\n"
         "      } else {\n        f = (int (*)(void*))row[0];\n      }\n"
         "      f(p2);",
         "      if (v & 1) {\n        int (*f)(void*) =\n"
         "            *(int (**)(void*))((char*)(*(int**)p2) + row[0]);\n"
         "        f(p2);\n      } else {\n"
         "        /* PORT: row[0] is a DS code address (mwcc PMF); route through the\n"
         "           state-fn mapper instead of calling it raw */\n"
         "        hal_call_state_fn(p2, (unsigned)row[0]);\n      }"),
    ],
    "_ZN6Player16St_WallJump_MainEv": [
        ("      int (*f)(void*);\n      if (v & 1) {\n"
         "        f = *(int(**)(void*))((char*)(*(int**)p) + row[0]);\n"
         "      } else {\n        f = (int(*)(void*))row[0];\n      }\n"
         "      f(p);",
         "      if (v & 1) {\n"
         "        /* PORT: row[0] is a DS vtable byte offset; route it, do not walk a\n"
         "           host vtable with it */\n"
         "        hal_call_state_fn(p, (unsigned)(*(int*)p + row[0]));\n"
         "      } else {\n"
         "        /* PORT: row[0] is a DS code address (mwcc PMF); route through the\n"
         "           state-fn mapper instead of calling it raw */\n"
         "        hal_call_state_fn(p, (unsigned)row[0]);\n      }"),
    ],
    "_ZN6Player17St_NoControl_InitEv": [
        ("  void (*f)(void*);\n  if(fn & 1){\n"
         "    f=*(void(**)(void*))(*(int*)obj + m->adj);\n"
         "  } else {\n    f=(void(*)(void*))m->adj;\n  }\n  f(obj);",
         "  if(fn & 1){\n"
         "    hal_call_state_fn(obj, (unsigned)(*(int*)obj + m->adj));\n"
         "  } else {\n    hal_call_state_fn(obj, (unsigned)m->adj);\n  }"),
    ],
}


# ---- ARGUMENT WIDTH ACROSS TWO MATCHED TUs -----------------------------------
#
# Lane shadow-A. Two src TUs can disagree about the width of one parameter and
# both still be byte matches: on ARM the value rides in r0 either way, and the
# caller that widened a byte to a register has done the callee's work for it.
# Under x86 cdecl the two spellings are NOT interchangeable. MSVC stores an s8
# argument as ONE byte into the outgoing four-byte slot and pushes the slot,
# so a callee that reads the slot as `int` sees one meaningful byte and three
# stale ones -- and CollectStar's body is `data_0209cab4[a] |= 1 << b;`, an
# unbounded indexed read-modify-write. Measured: the first star the port ever
# collected faulted at data_0209cab4 + 0x60e95014.
#
#     src/CollectStarInLevel.c   extern void CollectStar(s8 courseID, s32);
#     src/CollectStar.c          void CollectStar(int a, int b);
#
# The patch retypes the one parameter in the DEFINITION to the width its
# caller declares, so the callee extends the byte itself. Nothing else in the
# body changes. src/IsStarCollected.c, the READ half of the same pair, already
# spells its parameter s8 in src, which is why NumStars needs no entry here
# and links plain. Exact strings, hard-errored like the tables above: if the
# decomp ever settles the two declarations on one width, the entry stops
# matching and this table says so rather than silently going inert.
ARG_WIDTH = {
    "CollectStar": [
        ("void CollectStar(int a, int b){",
         "void CollectStar(signed char a, int b){"),
    ],
}


def arg_width_patch(text, sym):
    """Retype one parameter to the width its matched caller declares."""
    return apply_patches(text, sym, ARG_WIDTH, "ARG_WIDTH")


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


def mg_pmf_call_patch(text, sym):
    """Route an open-coded mwcc member-pointer call through the class seam."""
    return apply_patches(text, sym, MG_PMF_CALL, "MG_PMF_CALL", MG_PMF_CALL_DECL)


def member_redecl_patch(text, sym):
    """Delete redundant out-of-line member redeclarations (mwcc-tolerated,
    C2761 to MSVC)."""
    return apply_patches(text, sym, MEMBER_REDECL, "MEMBER_REDECL")


def extern_c_data_patch(text, sym):
    """Bring external data declarations a TU left above its own extern "C"
    block into C linkage, so the mount's C-linkage symbols resolve them."""
    return apply_patches(text, sym, EXTERN_C_DATA, "EXTERN_C_DATA")


def call_state_fn_patch(text, sym):
    """Route a Player state row's mwcc member-pointer dispatch through
    hal_call_state_fn on the DS code word, instead of calling it raw."""
    return apply_patches(text, sym, CALL_STATE_FN, "CALL_STATE_FN",
                         CALL_STATE_FN_DECL)


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
    text, _ = asm_excision_patch(text, sym)
    text, _ = ds_div_patch(text, sym)
    text, _ = mmio_extern_patch(text, sym)
    text, _ = falls_off_return_patch(text, sym)
    text, _ = virtual_call_patch(text, sym)
    text, _ = mg_pmf_call_patch(text, sym)
    text, _ = member_redecl_patch(text, sym)
    text, _ = extern_c_data_patch(text, sym)
    text, _ = call_state_fn_patch(text, sym)
    text, _ = arg_width_patch(text, sym)
    new, n = transform(text, extern_data)
    # An excision that left an asm block behind would emit a file MSVC cannot
    # read, and the file was only let past the skip in main() on the promise
    # that its blocks were all accounted for. Say so here rather than at the
    # compiler.
    if sym in ASM_EXCISION and is_asm(new):
        sys.exit("hostgen: %s: ASM_EXCISION ran but an asm block is still "
                 "present. Every block in the file has to be read and listed "
                 "before it can be emitted." % sym)
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
        if is_asm(text) and path.stem not in ASM_EXCISION:
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
