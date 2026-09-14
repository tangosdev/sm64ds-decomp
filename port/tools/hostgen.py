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
# registers, and the decomp uses them: src/minigames/d_s_mg_single3_d_base.cpp reaches
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
    # gates 64-69, WHOMP. RETIRED 2026-09-13 (main -> port sync, lane SYNC4).
    # decl_common.h declared func_01ffb07c (void*,void*) and func_020396d0
    # (int*,int) while the TU declared them (void*,s32*) and (void*,int) --
    # one register on the ROM, two C2733s to MSVC, so the header's pair was
    # hidden and the TU's own declarations won. main's spelling of the TU
    # declares NEITHER locally any more (it just calls them, lines 138 and
    # 141), so hiding the header now leaves both undeclared: C3861. The
    # collision the entry existed for is gone with the local declarations.
    # run linkw wave 7, lane w7b, MR_I_PROJECTILE (264): decl_common.h:2793
    # declares _ZN8daEyBm_c10HurtPlayerEv `(void*)` inside its extern "C" block while
    # src/game/actors/d_a_ey_bm.cpp defines it `(char*)`. One register on the
    # ROM, C2733 to MSVC -- the func_ov102_0214b248 case exactly.
    "_ZN8daEyBm_c10HurtPlayerEv": "decl_common.h",
    # DROPPED 2026-09-14 (main -> port sync, lane SYNC7), with the REG_RIDE_ARG
    # row it existed to make room for, and for the same reason the
    # func_ov102_0214b248 entry above was dropped: the collision is gone.
    # decl_common.h:1404 now declares func_ov002_020e3f90 `(char*)`, which is
    # what its definition says and what src/func_ov002_020e444c.c:30 calls, so
    # the header no longer contradicts anything and hiding it left the name with
    # no declaration at all (C3861 in the generated TU, which is how it was
    # found). The old text is in git at d9ea8dda4.
    # "func_ov002_020e444c": ("decl_common.h", ("func_ov002_020e3f90",)),
    # lane LINKMG, dScMgBase_c slots 30 and 29: decl_common.h:2352-2353 declare
    # both `(void*)` inside its extern "C" block while src/_ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv.cpp
    # defines `(char*)` and src/_ZN11dScMgBase_c16OnAimedAtWithEggEv.cpp defines `(Obj*)`. One
    # register on the ROM, C2733 to MSVC -- the func_ov102_0214b248 case
    # exactly. unmatched/MgBase_DeclConflict.cpp used to carry both bodies with
    # the parameter retyped; this entry retires that file.
    "_ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv": "decl_common.h",
    "_ZN11dScMgBase_c16OnAimedAtWithEggEv": "decl_common.h",
    # run rel0215 wave 3, lane w3-a2, TTC_ROTATING_CUBE/PRISM (108/109):
    # decl_common.h:2777-2778 declare _ZN15TtcRotatingCube10UpdateClsnEv and
    # _ZN15TtcRotatingCube11UpdateModelEv `(char*)` inside its extern "C" block, while
    # src/_ZN15TtcRotatingCube13InitResourcesEv.cpp re-declares both `(void*)`
    # in its own extern "C" block and calls them with `(void*)this`. One
    # register on the ROM, two C2733s and two C2664s to MSVC -- the
    # _ZN5Whomp13InitResourcesEv case exactly, down to the colliding names
    # being OTHER functions than the emitted symbol. Both are themselves on
    # this cluster's slice (src/_ZN15TtcRotatingCube10UpdateClsnEv.cpp defines it `(void*)`
    # and src/_ZN15TtcRotatingCube11UpdateModelEv.cpp `(void*)`), so the header is the odd one
    # out and shadowing it is what leaves every definition untouched.
    "_ZN15TtcRotatingCube13InitResourcesEv": ("decl_common.h",
                                              ("_ZN15TtcRotatingCube10UpdateClsnEv",
                                               "_ZN15TtcRotatingCube11UpdateModelEv")),
    # Run link100, lane SEAT6, batch B6. src/_ZN6Player16CleanupResourcesEv.cpp
    # re-declares five functions decl_common.h has already declared, `char *`
    # against the header's `void *` (:2207-2209, :2289) and, for func_02073244
    # (:2158), `(int,int,int,void(*)(void))` against `(void*,int,int,
    # void(*)(void*))`. mwcc reads one declaration seen twice; MSVC reads an
    # extern "C" overload and refuses the TU with five C2733s and one C2664 --
    # the func_ov102_0214b248 / _ZN5Whomp13InitResourcesEv case exactly. The
    # TU's own declarations then stand alone and its call to func_02073244
    # type-checks against the `void (*)(void)` it declares _ZN7Vector3D1Ev with,
    # which decl_common.h does not declare at all. This is blocker 2 of the two
    # port/unmatched/Player_CleanupResources.cpp records; blocker 1 (the
    # `p->v1()` model deletes landing on DoSetFile) died with lane SLOT5F's
    # destructor respelling -- hal/model_dtor_seat.cpp seats _ZTV5Model[1],
    # _ZTV9ModelAnim[1] and _ZTV10ModelAnim2[1] with the ROM's own deleting D0
    # today, which is the word the two-virtual shadow's slot 1 means.
    # Retires port/unmatched/Player_CleanupResources.cpp.
    # DROPPED 2026-09-13 (main -> port sync, lane SYNC5), reason gone and the
    # shadow now HARMS. Lane SEAT6 added this row because the TU re-declared
    # five decl_common.h names with `char *` where the header says `void *`.
    # main's own spelling of the TU says the opposite in its own words at
    # line 16: "func_ov002_020bdd2c, _020bdef0, _020bdd9c, _020e032c and
    # func_02073244 come from decl_common.h above ... Declare only what
    # decl_common.h does not." With the header shadowed the TU declares none
    # of the five and MSVC answers C3861, identifier not found, three times.
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
    # THE C2733 A HEADER SHADOW CANNOT REACH, 2026-09-13 (lane SYNC3).
    # src/actors/Player.cpp defines func_ov002_020cfbdc `(char *self)` inside an
    # extern "C" block and then, at block scope inside that same body, declares
    # `extern int func_ov002_020cfbdc(void*);` -- two extern "C" spellings of one
    # name in one TU, which MSVC refuses outright. While the body was its own
    # file the collision was with include/decl_common.h and HEADER_SHADOW hid the
    # header's declaration across the include; the header is not in this TU at
    # all, so the only thing left to hide is the TU's own line. The declaration
    # is dead weight either way: the definition it collides with is four lines
    # above it and says the same thing about the ROM, one register.
    "Player": [
        ("    extern int func_ov002_020cfbdc(void*);\n", ""),
    ],
    # gate 67, BILL_BLASTER: _ZN11BillBlaster8BehaviorEv (daObjBkKillerdai_c::Behavior)
    # RE-DERIVED 2026-09-13 (lane SYNC3). The same five out-of-class member
    # redeclarations, which C++ does not allow and mwccarm accepted; main
    # renamed the classes they name (Platform -> dBgActor_c, through the helper
    # spelling this TU uses, and Actor -> dActor_c) and widened Spawn's last two
    # parameters to their real types. Deleting them is still the whole patch:
    # every one of the five is declared again, correctly, inside its class.
    "_ZN11BillBlaster8BehaviorEv": [
        ("int dBgActor_c_helper::UpdateKillByMegaChar(short, short, short, "
         "FixV);\n", ""),
        ("dActor_c* Actor_s::FindWithID(unsigned int);\n", ""),
        ("dActor_c* Actor_s::ClosestPlayer();\n", ""),
        ("dActor_c* Actor_s::Spawn(unsigned int, unsigned int, const Vector3&, "
         "const Vector3_16*, signed char, short);\n", ""),
        ("int dBgActor_c_helper::IsClsnInRange(FixV, FixV);\n", ""),
    ],
}


# ---- TWO SPELLINGS OF ONE C NAME IN ONE TRANSLATION UNIT --------------------
#
# Run link100, lane HOSTGEN4. The decomp's convention was one function per file,
# and the pret idiom that DECLARATION ORDER controls mwccarm's register
# allocation means every member re-declares the externs it calls AT BLOCK
# SCOPE, in its own spelling, exactly where it recovered them.
# src/actors/daMky_c.cpp states that rule in its own header at lines 148-154.
# main's consolidation then added ONE file-scope extern "C" block with a single
# spelling per name, and the two now sit in one translation unit. mwccarm never
# saw the pair; MSVC refuses it outright:
#
#   daMky_c.cpp(277): error C2556: 'int func_0201267c(int,void *)': overloaded
#       function differs only by return type from 'void func_0201267c(int,void *)'
#   daMky_c.cpp(168): note: see declaration of 'func_0201267c'
#
# eighteen times over, plus the C2568, C2440 and C2264 that fall out of the
# unresolved overload sets. THE DISAGREEMENT IS THE RETURN TYPE AND NOTHING
# ELSE at every one of the eighteen, and every call site ignores the value: a
# site that did not could not have compiled against the `void` spelling. So the
# block-scope copy is deleted and the file-scope one governs. Four of the seven
# names are the TU's OWN definitions being redeclared by a sibling member
# further down the same file, where the redeclaration is pure noise; the
# definition is always above the use, which was checked line by line.
#
# THE SECOND SHAPE IN THE SAME FILE is a function-local class used to declare an
# `extern`, which C++ forbids because a local class has no linkage (C2624). Four
# of the six are a type the member defines for itself and one is a stray
# `struct dActor_c;` forward declaration that SHADOWS the real class from the
# headers -- deleting that line is the whole fix for two of the six, because the
# elaborated specifiers below it then name the real dActor_c. The rest are
# hoisted to file scope, and hoisted INSIDE an extern "C" block: these are ROM
# data references that inherit C linkage from the enclosing member today, and a
# hoist into plain namespace scope would hand them back to the linker decorated,
# which is the defect DATA_C_LINKAGE exists to remove.
#
# ONLY THE TYPES MOVE, and that was measured rather than assumed. Hoisting the
# DATA declarations with them was tried first and produced five new C2040s,
# because the members do not agree on the type of data_ov030_02115d18:
# EnterState10 calls it `struct S { int w[2]; }`, EnterState8
# `struct G { void *a; void *b; }`, and three more members declare it `int []`,
# `void *[]` and `void *` at their own block scope. Those spellings do not fight
# each other while they stay in separate function bodies; ONE file-scope
# declaration makes four of them an error. A block-scope declaration whose TYPE
# has linkage is all C2624 asks for, and it keeps the C linkage of the
# enclosing extern "C" member, which is the spelling the mount emits.
# THE HOISTED BLOCK LANDS ABOVE THE TU'S OWN INCLUDES, so it may use nothing
# but builtin types: `int` where the member wrote `s32`, which is the same type
# under a different spelling and keeps the block readable without a header.
REDECL_CONFLICT_DECL = {
    "daMky_c": """\
/* hostgen REDECL_CONFLICT: the file-scope half of the repair below. ONLY THE
   TYPES move. Each datum keeps its own block-scope declaration, in the member
   that recovered it, because the members do not agree on its type: five
   spellings of data_ov030_02115d18 live in this one file and a single
   file-scope declaration would make four of them a C2040. */
struct daMky_M4x3 { int w[12]; };
struct daMky_S { int w[2]; };
struct daMky_G { void *a; void *b; };
struct daMky_Item16 { int a, b, c, d; };

""",
    "daMip_c": """\
/* hostgen REDECL_CONFLICT: the file-scope half of the repair below. Four
   members define an identical `struct Mtx43 { s32 m[12]; }` for themselves and
   two of them then declare an extern of that type, which C++ refuses because a
   local class has no linkage; one definition up here serves all four and every
   `struct Mtx43` spelling in the file still reads. VObj is the render shadow:
   six virtuals declared and never defined, which is legal at namespace scope
   and C3640 inside a function body. daMip_G carries StateReleasedInit's own
   two-word view of data_ov085_021305c0, which four other members spell four
   other ways at their own block scope. */
struct Mtx43 { int m[12]; };
struct daMip_G { int w[2]; };
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m14(void *arg);
};

""",
}

REDECL_CONFLICT = {
    "daMky_c": [
        # The eighteen block-scope redeclarations, in file order. Each names a
        # symbol the TU already declares at lines 155-169 or defines above.
        ("    extern int func_0201267c(int a, void *b);\n", ""),
        ("    extern void *_ZN8dActor_c13ClosestPlayerEv(void *a);\n", ""),
        ("    extern void func_ov030_02111dd0(char *c);\n", ""),
        ("    extern void func_ov030_02111ea4(char *c);\n", ""),
        ("    extern void func_ov030_02111bc4(void* c);\n", ""),
        ("    void *_ZN8dActor_c13ClosestPlayerEv(void *self);\n", ""),
        ("    void _ZN5dCc_c5ClearEv(void *thiz);\n", ""),
        ("    extern void func_ov030_02111bc4(void *a);\n", ""),
        ("    extern void _ZN5dCc_c5ClearEv(void *p);\n", ""),
        ("    extern int func_0201267c(int a, void* b);\n", ""),
        ("    extern void _ZN5dCc_c5ClearEv(void* self);\n", ""),
        ("    extern void _ZN5dCc_c5ClearEv(void *self);\n", ""),
        ("    extern void *_ZN8dActor_c13ClosestPlayerEv(void *c);\n", ""),
        ("    extern void func_ov030_02111bc4(void *c);\n", ""),
        ("    extern void _ZN5dCc_c5ClearEv(void *c);\n", ""),
        ("    extern void func_ov030_02111a00(char* c);\n", ""),
        ("    extern void func_ov030_02111dd0(char* c);\n", ""),
        ("    extern void func_ov030_02111ea4(char* c);\n", ""),
        # The stray forward declaration that makes a LOCAL dActor_c out of the
        # two elaborated specifiers under it. The real class is visible at file
        # scope in this TU: func_ov030_021122b0 takes a dActor_c * at line 679.
        ("    struct dActor_c;\n", ""),
        # The four local types, re-pointed at the file-scope ones in
        # REDECL_CONFLICT_DECL above. The two that are typedefs keep their
        # names, so every use in the member reads unchanged; the two that are
        # elaborated inside the extern itself lose the elaboration, because
        # `struct S` is not a spelling a typedef-name answers to.
        ("    typedef struct M4x3 { int w[12]; } M4x3;\n",
         "    typedef daMky_M4x3 M4x3;\n"),
        ("    struct S { int w[2]; };\n"
         "    extern struct S data_ov030_02115d18;\n",
         "    extern daMky_S data_ov030_02115d18;\n"),
        ("    struct G { void *a; void *b; };\n"
         "    extern struct G data_ov030_02115d18;\n",
         "    extern daMky_G data_ov030_02115d18;\n"),
        ("    typedef struct { int a, b, c, d; } Item16;\n",
         "    typedef daMky_Item16 Item16;\n"),
    ],
    # daMip_c (MIPS the rabbit). Nine errors, both classes of them the local-type
    # shape and no return-type disagreement anywhere: the TU's own header at line
    # 61 already records that its members spell data_ov085_021305c0 five
    # different ways. Four members define the SAME `struct Mtx43` for themselves
    # (identical text, checked), two of those then declare an extern of it, and
    # Render's VObj declares six virtuals it never defines. All three go to file
    # scope; the Mtx43 definition is deleted from all four members at once and
    # the file-scope one carries every `struct Mtx43` spelling that is left.
    "daMip_c": [
        ("    struct G { int w[2]; };\n"
         "    extern struct G data_ov085_021305c0;\n",
         "    extern daMip_G data_ov085_021305c0;\n"),
        ("    struct Mtx43 { s32 m[12]; };\n", ""),
        ("    struct VObj {\n"
         "        virtual void v0();\n"
         "        virtual void v1();\n"
         "        virtual void v2();\n"
         "        virtual void v3();\n"
         "        virtual void v4();\n"
         "        virtual void m14(void* arg);\n"
         "    };\n", ""),
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
    # RE-KEYED AND RE-DERIVED 2026-09-13 (lane SYNC3). main folded
    # _ZN19daPropeller_Heyho_c13InitResourcesEv into
    # src/game/actors/d_a_propeller_heyho.cpp and rewrote the declaration block,
    # so the old exact strings no longer matched -- which apply_patches would
    # have turned into a hard error rather than a silent drop, and did. main's
    # spelling declares all SEVEN SharedFilePtr views plus the state-table
    # pointer above the body's own `extern "C" {`, so this moves the opener up
    # over the whole run instead of over one name. The block is balanced: the
    # opener is moved, not added, so the TU's brace count does not change.
    # CleanupResources further down the same TU releases the same seven and has
    # no declarations of its own, so it is covered by the same move.
    # RE-DERIVED AGAIN 2026-09-14 (main -> port sync, lane SYNC7). One line of
    # the eight moved: main retyped the state table from `extern char
    # data_ov070_0212359c[];` to `extern daPropeller_Heyho_c::State
    # data_ov070_0212359c;`. The move itself is unchanged and so is what it is
    # for: a C++ type declared inside an extern "C" block still gets C linkage,
    # which is the whole point of the row.
    "d_a_propeller_heyho": [
        ("extern SharedFilePtr data_ov070_02123530;\n"
         "extern SharedFilePtr data_ov070_02123520;\n"
         "extern SharedFilePtr data_ov070_02123518;\n"
         "extern SharedFilePtr data_ov070_02123510;\n"
         "extern SharedFilePtr data_ov070_02123528;\n"
         "extern SharedFilePtr data_ov070_02123508;\n"
         "extern SharedFilePtr data_ov070_02123500;\n"
         "extern daPropeller_Heyho_c::State data_ov070_0212359c;\n"
         'extern "C" {\n',
         'extern "C" {\n'
         "extern SharedFilePtr data_ov070_02123530;\n"
         "extern SharedFilePtr data_ov070_02123520;\n"
         "extern SharedFilePtr data_ov070_02123518;\n"
         "extern SharedFilePtr data_ov070_02123510;\n"
         "extern SharedFilePtr data_ov070_02123528;\n"
         "extern SharedFilePtr data_ov070_02123508;\n"
         "extern SharedFilePtr data_ov070_02123500;\n"
         "extern daPropeller_Heyho_c::State data_ov070_0212359c;\n"),
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
        # RE-DERIVED 2026-09-13 (lane SYNC3): main gave both operands real
        # names. Same modulo, same zero-divisor exposure -- the divisor is
        # still the emission interval 181 of the 321 particle definitions ship
        # as zero.
        ("(int)state.age % (int)effect.jitter.period",
         "ds_imod((int)state.age, (int)effect.jitter.period)"),
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
    # Lane shadow-A: G3i::PerspectiveW_. The register traffic itself needs no
    # entry -- the sixteen MTX_LOAD_4x4 stores go through a pointer bound once
    # to 0x4000458 and MMIO_PTR rewrites them, the u64 DIV_NUMER/DENOM stores
    # are literal derefs. What the table carries is ONE DELIBERATE HOST
    # DIVERGENCE: on the DS this function inherits 64/32 divide mode from the
    # cstd::fdiv it opens with (fdiv_async writes DIVCNT); the port's
    # cstd::fdiv is a pure C divide (hal/cstd_div.c) that touches no register,
    # so without this write the two divisions run 32/32 and the near/far row
    # comes out garbage. The retired host copy carried the same line at the
    # same spot. Anchored on the first DIV_NUMER store so it lands after the
    # scaleW adjust and before the numerator is latched.
    "_ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3": [
        ("    *(volatile u64 *)0x4000290 = (u64)(unsigned)tmp << 32;",
         "    NTR_MMIO(unsigned short, 0x4000280) = 1;  /* hostgen: DIVCNT 64/32, "
         "the DS inherits it from fdiv_async; see MMIO_EXTERN */\n"
         "    *(volatile u64 *)0x4000290 = (u64)(unsigned)tmp << 32;"),
    ],
    "func_02053130": [
        # the busy-bit spin: SQRTCNT through the proxy; the local pointer
        # binding goes with it (keeping it would keep the &SQRTCNT reference)
        # RE-DERIVED 2026-09-13 (main -> port sync, lane SYNC3): main spells
        # the two registers REG_SQRTCNT and REG_SQRT_RESULT and the busy bit
        # SQRT_CONTROL_BUSY now. Same two registers, same two addresses; only
        # the names in the source moved.
        ("    sqrtcnt = &REG_SQRTCNT;\n"
         "    while (*sqrtcnt & SQRT_CONTROL_BUSY)\n        ;",
         "    while (NTR_MMIO(u16, 0x40002b0) & SQRT_CONTROL_BUSY)\n        ;"),
        # the result read, at the width the ROM read it
        ("(s64)REG_SQRT_RESULT", "(s64)(s32)NTR_MMIO(s32, 0x40002b4)"),
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
# _ZN8daGmch_c21ApplySlopeToVertSpeedEPv is the SAME BODY one overlay over (the ov081/ov096
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
#     (src/actors/daGmch_c.cpp:46, src/actors/daGmch_c.cpp:31,
#     src/actors/daGmch_c.cpp:16) each call it as a bare statement.
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
    # RE-KEYED 2026-09-13 (main -> port sync, lane SYNC3). main folded this
    # body into src/game/actors/d_a_propeller_heyho.cpp, so the key is the TU's
    # stem now; the patch string is unchanged and still matches exactly once in
    # main's own spelling of the body, which was checked before the key moved.
    # RE-KEYED AGAIN 2026-09-14 (main -> port sync, lane SYNC7). Same body,
    # same address (ov070 0x0211f0a4), same falls-off-the-end defect: main
    # retyped the parameter from `void *c` with a local `dActor_c *a` to a
    # typed `daPropeller_Heyho_c *c` and dropped the local, so the anchor's
    # receiver is spelled `c` now. The string still matches exactly once in
    # the TU; the file's other KillAndTrackInDeathTable call is the implicit
    # one inside a void member and is not matched by it.
    "d_a_propeller_heyho": [
        ("    c->KillAndTrackInDeathTable();\n}",
         "    c->KillAndTrackInDeathTable();\n"
         "    return 1;  /* hostgen FALLS_OFF_RETURN: see the table's note */\n"
         "}"),
    ],
    "_ZN8daGmch_c21ApplySlopeToVertSpeedEPv": [
        ("_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, n1);\n    }\n}",
         "_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, n1);\n    }\n"
         "    return 0;  /* hostgen FALLS_OFF_RETURN: see the table's note */\n"
         "}"),
    ],
    # The anchor is the WALL branch's CopyNormalTo, the one that writes n1; the
    # floor branch above it writes n0 and is not matched by this text.
    "func_ov019_0211140c": [
        ("        _ZNK11SurfaceInfo12CopyNormalToER7Vector3("
         "(char*)_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, n1);\n    }\n}",
         "        _ZNK11SurfaceInfo12CopyNormalToER7Vector3("
         "(char*)_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, n1);\n    }\n"
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
# dispatched cdecl through a typedef'd pointer in _ZN8Particle10SysTracker8Contents6UpdateEv:
#
#     e->f10->vt->m[1](e->f10, e->fc, m)
#
# and slot 0 is dispatched cdecl in Particle::System::New:
#
#     o->vtable[0](o, p)
#
# but _ZN8Particle10SysTracker8Contents5Entry10InitialiseEjjR7Vector3PK11Vector3_16fPN5dPa_c7level_c10callback_cE declares a local shadow `struct Callback { virtual void
# Run(void *); }` and calls `p6->Run(...)`, which MSVC compiles as a thiscall
# virtual. Hosted unpatched, the callback reads its System argument off the
# stack slot that held `this` and dereferences null on the first landing puff.
#
# The whole subsystem has exactly ONE such call, so the fix is to make that
# one site dispatch the way its twenty-five siblings do -- explicitly, through
# vptr[0], cdecl -- rather than to give slot 0 a second calling convention.
# The local `virtual` declaration stays; only the call changes.
VIRTUAL_CALL = {
    "_ZN8Particle10SysTracker8Contents5Entry10InitialiseEjjR7Vector3PK11Vector3_16fPN5dPa_c7level_c10callback_cE": [
        # RE-DERIVED 2026-09-13 (main -> port sync, lane SYNC3). main retired
        # the local shadow `struct Callback { virtual void Run(void *); }` and
        # calls the real class: dPa_c::level_c::callback_c's FIRST virtual,
        # SpawnParticles, which is the same slot 0 this patch always meant.
        # Nothing about the reason changed -- MSVC would still compile the call
        # __thiscall while every other dispatch of that table in this build is
        # cdecl -- so the rewrite is the same explicit slot-0 call with the new
        # receiver and argument names. The System& argument is passed as the
        # pointer it already is on both sides.
        ("        newCallback->SpawnParticles(*system);",
         "        (*(void (***)(void *, void *))newCallback)[0]"
         "((void *)newCallback, (void *)system);"),
    ],
    # Lane shadow-A: Model::LoadAndSetFile's middle. The matched source
    # dispatches DoSetFile through a LOCAL shadow class with three virtuals,
    # `self->v2(file, c, d)`: slot 2 in ROM/Itanium numbering, where a
    # destructor takes TWO slots (D1, D0).
    #
    # THE SLOT MOVED BACK, and this is the history. The comment used to read:
    # "MSVC gives a destructor one slot, so the host's _ZTV5Model has DoSetFile
    # at 1 and UpdateVerts at 2, and the shadow's slot 2 walked a fresh Model's
    # null vertex list inside the first Tree's InitResources. The double-fill
    # trick (Render, slots 4 and 5) cannot serve a slot that means two live
    # things, so the call is resolved at the caller: DoSetFile is slot 1 in
    # every host model table." The respelling in include/ModelBase.h stopped
    # MSVC folding the pair, so every host model table is ROM-numbered now and
    # the shadow's slot 2 IS DoSetFile -- the word the ROM's own _ZTV5Model
    # holds at 0x0208e90c+8.
    #
    # The rewrite itself stays, because the shadow's `v2` still has to be
    # entered __fastcall (ecx = this, the dummy edx absorbs fastcall's second
    # register), the convention hal/actor_vtables.cpp set. Only the index
    # changes, 1 -> 2. LEFT AT 1 THIS WOULD DISPATCH Model's DELETING
    # DESTRUCTOR on every model load.
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
         "(*(void ***)self)[2])(self, 0, file, c, d);"),
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
    # src/_ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE.cpp, unchanged: before,
    # then main, then after(code)), through the same __fastcall thunk
    # convention as func_02016ff4 above. Retires
    # port/unmatched/func_0204335c_hostcopy.cpp and func_02043288_hostcopy.cpp.
    # The other two wrappers (func_0204322c render 9/10/11, func_020432e4
    # cleanup 3/4/5) keep their host copies: the render one carries the slot-5
    # Virtual18 ruling and is not this lane's.
    "func_0204335c": [
        ("    return _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(\n"
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
        ("    return _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(\n"
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
    # Run link100, lane EXCEPT. dScMgSnowball_c's Render, ROM vtable slot 9,
    # and the last of lane FOLD2's thirty ov006 rows. Its divergence is NOT
    # the destructor fold: the vtable read is already right. The ROM is
    #
    #     02127e50  add  r0,sb,r0        ; the Model at this+0xaba4
    #     02127e54  ldr  r2,[r0]         ; its vptr
    #     02127e58  ldr  r2,[r2,#0x14]   ; BYTE +0x14 = word 5
    #     02127e5c  blx  r2
    #
    # and word 5 of _ZTV5Model is Render on the host too, since lane SLOT5F's
    # respelling of include/ModelBase.h made every model table ROM-numbered
    # (hal/cxxname_bridge.cpp:522 writes mv_render there). The problem is the
    # CONVENTION, measured by a run: the matched source reads the word into a
    # `void (*)(void*, int*)` and calls it cdecl, while what is seated is
    #
    #     void __fastcall mv_render(void *self, void *unused, const void *s)
    #
    # so the receiver arrived on the stack instead of ecx, `s` read the model
    # pointer, and the callee cleaned four bytes the caller had also counted.
    # The retired host copy recorded the fault that finding came from: scene
    # 377, code c0000005, eip == the actor's own address, because the
    # double-cleaned frame returned into the object.
    #
    # This patch keeps the ROM's word index and the ROM's arguments and only
    # spells the call the way the host slot has to be entered. The two
    # geometry-register stores in the same body need no row here: transform()
    # runs after this and MMIO_DEREF already matches both
    # `*(volatile int*)0x040004c8` and `*(int*)0x040004cc`.
    # Retires port/unmatched/MgSnowball_ModelRender.cpp.
    # Run link100, lane HOSTGEN2. dScMgRoulette_c's wheel update,
    # func_ov006_02107db8, calls ROM vtable slot 3 through a FOUR-VIRTUAL local
    # shadow (`struct ObjV`) on the Model at the wheel object's +0x10. MSVC
    # numbers that shadow's own table 0..3 with no destructor slot, so slot 3 of
    # the shadow is not slot 3 of the host Model, and the reading that settles it
    # is the one unmatched/MgRoulette_WheelDispatch.cpp wrote out of the ROM
    # table: (0 D1, 1 D0, 2 DoSetFile, 3 UpdateVerts, 4 Virtual10, 5 Render), so
    # ROM slot 3 is Model::UpdateVerts. The patch spells the qualified method the
    # ROM means, which is exactly what that host copy's DELTA 2 did. The TU's own
    # header chain does not reach Model.h -- dScMgRoulette_c.h pulls in
    # dScMgSingle3DBase_c.h and stops -- so the include rides in as the first
    # pair, next to the TU's own.
    "dScMgRoulette_c": [
        ('#include "dScMgRoulette_c.h"',
         '#include "dScMgRoulette_c.h"\n'
         '/* hostgen VIRTUAL_CALL: for the Model at the wheel object + 0x10. */\n'
         '#include "Model.h"'),
        ("    c->obj.v3();",
         "    /* hostgen VIRTUAL_CALL: ROM slot 3 of the Model at +0x10 is\n"
         "       UpdateVerts; the local four-virtual shadow numbers its own\n"
         "       table differently. See the table. */\n"
         "    ((Model *)((char *)c + 0x10))->UpdateVerts();"),
    ],
    "_ZN15dScMgSnowball_c6RenderEv": [
        ("            void (*fn)(void*, int*) = "
         "*(void(**)(void*, int*))((char*)(*(void**)self) + 0x14);\n"
         "            fn(self, vecArr);",
         "            /* hostgen VIRTUAL_CALL: byte +0x14 is word 5 of the\n"
         "               Model vptr -- Render -- and the host seats a\n"
         "               __fastcall thunk there. See the table. */\n"
         "            ((void (__fastcall *)(void *, void *, int *))\n"
         "             (*(void ***)self)[5])(self, 0, vecArr);"),
    ],
    # ------------------------------------------------------------------
    # Run link100, lane SEAT6, batch B6 (families E and F, the residue lanes
    # SLOT5F and FACEF left standing). Every row below is the SAME mechanism
    # as _ZN15dScMgSnowball_c6RenderEv above: the matched TU reads a vtable word raw and
    # calls it __cdecl (receiver PUSHED, caller cleans), while the word the
    # host seats in that slot is entered with the receiver in ECX. On ARM the
    # two spellings are the same three instructions, which is why byte-locked
    # src carries the cdecl one; on x86 they are different calls. Each patch
    # keeps the ROM's own word index and the ROM's own arguments and changes
    # only how the call is entered.
    #
    # RabbitKey::Render (ov085), the Model at this+0x110. ROM:
    #   ldr r2,[r0] / mov r1,#0 / ldr r2,[r2,#0x14] / blx r2
    # byte +0x14 = word 5 = Render, scale argument null. The matched TU's
    # shadow field is `void (*fn14)(void *self, int)`, a PLAIN cdecl pointer,
    # and _ZTV5Model[5] holds `void __fastcall mv_render(void*, void*,
    # const void*)` (hal/cxxname_bridge.cpp:522): the receiver never arrived
    # and four bytes were cleaned twice (measured: the spawn-assisted key
    # crashed frame 1, ecx=13811ba3, DEP-exec fault). Retires the
    # _ZN9RabbitKey6RenderEv body in port/unmatched/Ov085_Renders.cpp.
    "_ZN9RabbitKey6RenderEv": [
        ("        s->vt->fn14(s, 0);",
         "        /* hostgen VIRTUAL_CALL: byte +0x14 is word 5 of the Model\n"
         "           vptr -- Render -- and the host seats a __fastcall thunk\n"
         "           there. See the table. */\n"
         "        ((void (__fastcall *)(void *, void *, const void *))\n"
         "         (*(void ***)s)[5])(s, 0, (const void *)0);"),
    ],
    # Actor::OnKicked's dispatcher (ov002), vtable byte +0x60 = word 24. All
    # thirty seated slot-24 bodies are `static int __fastcall ac_kicked(void*,
    # void*, void*)` ret-4 veneers (hal/actor_classes.cpp:169,403), while the
    # raw src emits `push ebx / push ecx / call [eax+0x60]` with the caller
    # cleaning sixteen bytes. The two port_actor_interaction_* calls are the
    # quarantine net's receiver latch, carried over verbatim from the host
    # copy this retires: without it an access violation inside the receiver's
    # slot freezes the PLAYER driving the interaction instead of the receiver,
    # which presents as a permanent soft-lock rather than a frozen enemy. It
    # is a HOST SEAM, the func_02016ff4 port_model_shrink_enabled case, not
    # part of the convention fix. Retires
    # port/unmatched/Actor_OnKickedDispatch.cpp.
    "func_ov002_020eeeb8": [
        ("extern short data_02082214[];",
         "extern short data_02082214[];\n"
         "/* hostgen VIRTUAL_CALL host seam: the quarantine net's receiver\n"
         "   latch (port/unmatched/func_02043fdc_hostcopy.cpp). */\n"
         "void *port_actor_interaction_begin(void *receiver);\n"
         "void port_actor_interaction_end(void *prev);"),
        ("                    (*(void (**)(void *, char *))(*(int *)a + 0x60))(\n"
         "                        a, actor);",
         "                    /* hostgen VIRTUAL_CALL: byte +0x60 is word 24 --\n"
         "                       Actor::OnKicked -- and every seated slot-24\n"
         "                       body is a __fastcall ret-4 veneer. See the\n"
         "                       table. */\n"
         "                    void *port_prev_recv =\n"
         "                        port_actor_interaction_begin(a);\n"
         "                    ((void (__fastcall *)(void *, void *, char *))\n"
         "                     (*(void ***)a)[24])(a, 0, actor);\n"
         "                    port_actor_interaction_end(port_prev_recv);"),
    ],
    # Actor::OnAttacked2's dispatcher (ov002), vtable byte +0x5c = word 23.
    # Same shape, same seats (hal/actor_classes.cpp:167,402 `ac_atk2`).
    # Retires port/unmatched/Actor_OnAttacked2Dispatch.cpp.
    "func_ov002_020ef070": [
        ("extern short data_02082214[];",
         "extern short data_02082214[];\n"
         "/* hostgen VIRTUAL_CALL host seam: the quarantine net's receiver\n"
         "   latch (port/unmatched/func_02043fdc_hostcopy.cpp). */\n"
         "void *port_actor_interaction_begin(void *receiver);\n"
         "void port_actor_interaction_end(void *prev);"),
        ("                    (*(void (**)(void *, char *))"
         "(*(int *)a + 0x5c))(a, actor);",
         "                    /* hostgen VIRTUAL_CALL: byte +0x5c is word 23 --\n"
         "                       Actor::OnAttacked2 -- and every seated slot-23\n"
         "                       body is a __fastcall ret-4 veneer. See the\n"
         "                       table. */\n"
         "                    void *port_prev_recv =\n"
         "                        port_actor_interaction_begin(a);\n"
         "                    ((void (__fastcall *)(void *, void *, char *))\n"
         "                     (*(void ***)a)[23])(a, 0, actor);\n"
         "                    port_actor_interaction_end(port_prev_recv);"),
    ],
    # Actor::OnHitFromUnderneath's dispatcher (ov002), vtable byte +0x70 =
    # word 28 (hal/actor_classes.cpp:177,407 `ac_headbonk`). Slot 28's OTHER
    # byte-locked dispatcher, src/func_ov002_020eeca8.cpp, already spells the
    # same call as a C++ virtual, which is why one vtable word could not be
    # both conventions and why this site had to move rather than the seats.
    # Retires port/unmatched/Player_HeadBonk.cpp.
    # RE-KEYED 2026-09-13 (main -> port sync, lane SYNC5). main folded this
    # dispatcher into src/actors/Player.cpp, which is already a whole-TU
    # substitution (PORT_HOSTGEN_TU), so the key is that TU's stem and the
    # separate per-symbol generation is gone -- it would have compiled the
    # same body twice. ONE patch now, not two: main's TU spells
    # `extern int data_02099368;` three times over three functions, so the
    # old declaration anchor is no longer unique and text.replace would
    # have written the seam's declarations into two unrelated bodies. The
    # call site is unique, and a block-scope extern declaration is what the
    # TU itself uses everywhere else.
    "Player": [
        ("                if (a)\n"
         "                    (*(void (**)(void *, char *))"
         "(*(int *)a + 0x70))(a, self);",
         "                if (a) {\n"
         "                    /* hostgen VIRTUAL_CALL: byte +0x70 is word 28 --\n"
         "                       dActor_c::OnHitFromUnderneath -- and every\n"
         "                       seated slot-28 body is a __fastcall ret-4\n"
         "                       veneer. See the table. */\n"
         "                    void *port_actor_interaction_begin(void *recv);\n"
         "                    void port_actor_interaction_end(void *prev);\n"
         "                    void *port_prev_recv =\n"
         "                        port_actor_interaction_begin(a);\n"
         "                    ((void (__fastcall *)(void *, void *, char *))\n"
         "                     (*(void ***)a)[28])(a, 0, self);\n"
         "                    port_actor_interaction_end(port_prev_recv);\n"
         "                }"),
    ],
    # dScMgBase_c's two per-frame framework ticks (ov004), vtable slot 0x13 =
    # byte +0x4c. The ROM at 0x020b6cf4 is `ldr r2,[r0] / ldr r1,[r4,#0x18] /
    # ldr r2,[r2,#0x4c] / blx r2`, so slot 19 takes (this, int); the host
    # thunk hal/scene_mg.cpp's `static int __fastcall mb_v19(void*, void*,
    # int)` cleans the one stack parameter itself (`ret 4`) while the raw
    # src's plain C pointer cleans it again -- four bytes cleaned twice, which
    # walked func_ov004_020b6c9c's own epilogue one slot high and executed a
    # word out of the live scene object on scene 387. The two src TUs are
    # byte-identical apart from their names, so the two patches are the same
    # text. Retires port/unmatched/MgBase_ShadowSlot19.cpp.
    "func_ov004_020b6b40": [
        ("        int (*fn)(void *, int) = "
         "(int (*)(void *, int)) (*(void ***)g)[0x13];\n"
         "        if (fn(g, c->f18) == 0)",
         "        /* hostgen VIRTUAL_CALL: slot 0x13 (byte +0x4c) is seated as\n"
         "           a __fastcall thunk with one stack parameter it cleans\n"
         "           itself. See the table. */\n"
         "        int (__fastcall *fn)(void *, void *, int) =\n"
         "            (int (__fastcall *)(void *, void *, int)) "
         "(*(void ***)g)[0x13];\n"
         "        if (fn(g, 0, c->f18) == 0)"),
    ],
    "func_ov004_020b6c9c": [
        ("        int (*fn)(void *, int) = "
         "(int (*)(void *, int)) (*(void ***)g)[0x13];\n"
         "        if (fn(g, c->f18) == 0)",
         "        /* hostgen VIRTUAL_CALL: slot 0x13 (byte +0x4c) is seated as\n"
         "           a __fastcall thunk with one stack parameter it cleans\n"
         "           itself. See the table. */\n"
         "        int (__fastcall *fn)(void *, void *, int) =\n"
         "            (int (__fastcall *)(void *, void *, int)) "
         "(*(void ***)g)[0x13];\n"
         "        if (fn(g, 0, c->f18) == 0)"),
    ],
    # The +0x4f38 sub-object's fader gate (ov006), shared by dScMgCup_c and
    # dScMgSound_c. ROM 0x020c2938: `ldr r1,[r0] / ldr r1,[r1,#0x18] / blx r1`
    # with r0 the object -- byte +0x18, word 6. data_0209f5bc is a HOST object
    # (hal/fader_wipes.cpp's HalFaderWipe, laid out in ROM byte order) whose
    # virtuals MSVC compiles __thiscall, so the raw `int (**vt)(void*)` read
    # left the receiver on the stack and IsAtEnd read currInterp off whatever
    # ECX held. The stack balances either way, which is why nothing faulted
    # and nothing saw it. Retires
    # port/unmatched/MgShared4f38_ShadowFader_020c2924.cpp.
    "func_ov006_020c2924": [
        ("  int (**vt)(void*)=*(int(***)(void*))obj;\n"
         "  if(vt[6](obj)==0) return;",
         "  /* hostgen VIRTUAL_CALL: byte +0x18 is word 6 of the fader vptr --\n"
         "     IsAtEnd -- and hal/fader_wipes.cpp compiles HalFaderWipe's\n"
         "     virtuals __thiscall. See the table. */\n"
         "  if (((int (__fastcall *)(void *, void *))"
         "(*(void ***)obj)[6])(obj, 0) == 0) return;"),
    ],
    # ArrowSignRight::OnAttacked1's inner self-kill (ov098), vtable byte +0x7c
    # = word 31. The raw src loads the vtable pointer into ECX to reach the
    # slot and PUSHES `this`, while the seated veneer `static int __fastcall
    # as_kill(void *s, void *)` (hal/actor_classes_bob_world.cpp:1139,1190)
    # reads `this` from ECX: a WRONG RECEIVER, not a lost word -- the sign
    # would have run its Kill against _ZTV14ArrowSignRight. The stack balances
    # (one push, one pop, `ret 0`), so nothing faults. Retires
    # port/unmatched/ArrowSign_OnAttacked1.cpp.
    # DROPPED 2026-09-13 (main -> port sync, lane SYNC5), reason gone: main's
    # own spelling of this body calls Kill() as a C++ member instead of
    # indexing a raw 64-entry vtable at word 0x7c/4, so the one disagreeing
    # call site this patch respelled no longer exists. The TU is quarantined
    # for C2561 in the same pass -- main's body takes a bare `return;` out of
    # an int function -- so no copy of it is generated either.
    #
    # WATERFALL MIST, run link100 lane HOSTGEN4. daObjMarioCap_c::Behavior
    # calls the cap's own ModelAnim through a LOCAL four-virtual shadow,
    # `((VObj *)(c + 0x300))->v03()`, and the ROM's ModelAnim vtable is
    # [D1, D0, DoSetFile, UpdateVerts, Virtual10, Render, Virtual18], so +0x0c
    # is UpdateVerts() with no arguments. The port's _ZTV9ModelAnim is filled
    # in MSVC numbering, where the destructor is ONE slot, so +0x0c lands on
    # Virtual10(Matrix4x3 &): a one-argument __thiscall whose `ret 4` pops a
    # slot the frameless caller never pushed. Lane CAPSHOW measured it on VS
    # map 2 -- the return site after `call [eax+0Ch]` comes back four bytes
    # high on the frame a collected cap first animates, and Behavior's own ret
    # lands on 0x300387b0.
    #
    # That is the whole reason port/unmatched/WaterfallMist_Behavior_HostSites
    # .cpp exists, and before main's consolidation the src side was simply not
    # on a slice: slice_gate33's loop skips the row by name. main then folded
    # the body into the class TU, which slice_gate204 and slice_gate51 both
    # name, so the matched body now arrives whether the port wants it or not
    # and the two collide -- one of the six duplicate-symbol rows on
    # walk_window's link. The substitution carries CAPSHOW's one-line
    # correction into the class TU, which retires the host copy: the call is
    # the ROM's slot-3 UpdateVerts, spelled qualified so nothing dispatches
    # through the host table's numbering at all. The local VObj shadow stays;
    # only the call changes.
    # RE-ANCHORED 2026-09-14 at the fold (lane INT3). HOSTGEN4 derived the
    # from-text against the pre-merge src/actors/daObjMarioCap_c.cpp, which
    # reached the cap's ModelAnim through a local four-virtual `struct VObj`
    # shadow and called slot 3 by byte offset. Today's main rewrote that body
    # (286 lines in, 306 out in the sync merge): the VObj shadow is gone and
    # the member is spelled `mModelAnim.UpdateVerts()`, so the old from-text
    # does not occur and hostgen hard-errored the whole build. The site is the
    # same one, at line 251, and it occurs exactly once. The patch is KEPT and
    # re-anchored rather than dropped, because the reason is unchanged: the
    # port's _ZTV9ModelAnim is MSVC-numbered, so the correction pins the call
    # to the ROM's slot-3 UpdateVerts by naming it, instead of leaving MSVC to
    # decide whether to devirtualise a member call it is allowed to dispatch.
    "daObjMarioCap_c": [
        ("        mModelAnim.UpdateVerts();",
         "        /* hostgen VIRTUAL_CALL: the ROM's +0x0c on the cap's own\n"
         "           ModelAnim is UpdateVerts(); the port's table is MSVC-\n"
         "           numbered and puts Virtual10(Matrix4x3 &) there. Spelled\n"
         "           qualified so no numbering is consulted. */\n"
         "        mModelAnim.ModelAnim::UpdateVerts();"),
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
extern "C" void port_mg_wiggler_call0(void *, unsigned, int);
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
    # dScMgPanel_c, RE-KEYED AND RE-DERIVED 2026-09-13 (main -> port sync, lane
    # SYNC3). main folded every ov006 panel body into src/actors/dScMgPanel_c.cpp,
    # so the two entries that used to be keyed on their own file names are keyed
    # on the TU's stem and live in one list. There are now THREE dispatch sites
    # rather than two, because the TU also carries func_ov006_02106bc0, a body
    # the port never had a file for:
    #
    #   dScMgPanel_c::Behavior   slot 6, table 02142888, arity 0, open-coded
    #   func_ov006_02106bc0      table 02142840, arity 1, open-coded
    #   func_ov006_02106ca4      table 02142840, arity 1, a REAL mwcc
    #                            pointer-to-member on an INCOMPLETE class
    #
    # The first two are the shapes this table already knew; the strings moved
    # (main spells the cast `(void (*)(void *))` with spaces and indents the
    # Behavior call four) and were re-derived against main's own text rather
    # than adapted from the old ones.
    #
    # THE THIRD IS THE NEW ONE AND IT IS TWO EDITS, NOT ONE. main's
    # func_ov006_02106ca4 declares `typedef void (PanelC_ca4::*PanelPmf_ca4)(int)`
    # over a forward-declared `struct PanelC_ca4;` and indexes the ROM table with
    # it. MSVC sizes a member pointer to an INCOMPLETE class at its most general
    # representation, so `data_ov006_02142840[*p]` would stride over the table at
    # the wrong width and read a record that is not there -- before the call is
    # even reached, and with nothing said about it. The ROM's record is two
    # words, a code word and an adjustment, which is what the sibling body above
    # spells by hand as `struct PanelPmfRec_bc0 { int off; int adj; }`. So the
    # declaration is retyped to the ROM's own two words and the call goes through
    # the same seam the other two use, which re-does the decode, applies the
    # ROM's null-code guard and reports an address nothing hosts. The port
    # branch's own src/func_ov006_02106ca4.c, an open-coded transcription of the
    # same body, is retired by this: one definition, out of the decomp's TU.
    "dScMgPanel_c": [
        ("    ((void (*)(void *))fn)(obj);",
         "    port_mg_panel_call0(obj, (unsigned)e->a, adj);"),
        ("        fn(thisp, i);",
         "        port_mg_panel_call1(thisp, (unsigned)e->off, adj, i);"),
        ("    extern PanelPmf_ca4 data_ov006_02142840[];",
         "    /* hostgen MG_PMF_CALL: the ROM record is two words, a code word\n"
         "       and an adjustment; an MSVC member pointer to an incomplete\n"
         "       class is not that width. */\n"
         "    extern int data_ov006_02142840[];"),
        ("            (((PanelC_ca4 *)c)->*data_ov006_02142840[*p])(i);",
         "            port_mg_panel_call1(c + (data_ov006_02142840[*p * 2 + 1] >> 1),\n"
         "                                (unsigned)data_ov006_02142840[*p * 2],\n"
         "                                data_ov006_02142840[*p * 2 + 1], i);"),
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
    # dScMgHanachan_c's THIRD SHAPE, run link100 lane HOSTGEN2.
    # func_ov006_020eb610 open-codes the ARM Itanium sequence in plain ints over
    # the pair at the sub-object's +0x00, which its sibling func_ov006_020eb31c
    # copies down from +0x10. There is no member-pointer type anywhere in it, so
    # neither the link nor a `::*` source sweep can see it; only a run convicts
    # it, and the first symptom is eip on a raw DS address. The whole decode goes,
    # the way unmatched/MgWiggler_StateDispatch.cpp's copy of this body wrote it:
    # the seam re-does the decode, applies the ROM's own null guard on the CODE
    # word (`cmp r2,#0 / beq` at 0x020eb71c, which is the src's own test and
    # stays) and reports an address nothing hosts instead of jumping to it.
    "dScMgHanachan_c": [
        ("    if (*(int *)c != 0) {\n"
         "        int off = *(int *)(c + 4);\n"
         "        char *base = c + (off >> 1);\n"
         "        void (*fn)(char *);\n"
         "        if (off & 1)\n"
         "            fn = *(void (**)(char *))(*(char **)base + *(int *)c);\n"
         "        else\n"
         "            fn = *(void (**)(char *))c;\n"
         "        fn(base);\n"
         "    }",
         "    if (*(int *)c != 0) {\n"
         "        /* hostgen MG_PMF_CALL: the record is two words at +0x00, a\n"
         "           code word and an adjustment. The code word is a DS address;\n"
         "           the class's own switch decodes it, guards it and calls it. */\n"
         "        port_mg_wiggler_call0(c, (unsigned)*(int *)c, *(int *)(c + 4));\n"
         "    }"),
    ],
    "func_02008550": [
        ("    return (obj->*data_0209b138[msg[6]])(msg + 7, a2, a3);",
         "    return ((int (*)(void *, unsigned char *, int, int))"
         "*(void **)&data_0209b138[msg[6]])(obj, msg + 7, a2, a3);"),
    ],
}


# ---- STATE-MACHINE mwcc MEMBER-POINTER DISPATCH (PLAYER, CAMERA) -----------
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
CALL_STATE_FN_DECL = ('extern "C" int hal_call_state_fn(void *, unsigned);\n'
                      'extern "C" int hal_call_camera_state_fn(void *, unsigned);\n')
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
    # RE-KEYED 2026-09-13 (lane SYNC3): main folded St_NoControl_Init into
    # src/actors/Player.cpp. St_Jump_Main and St_WallJump_Main still have files
    # of their own and keep their own keys above. The patch string is unchanged
    # and matches exactly once in the TU.
    "Player": [
        ("  void (*f)(void*);\n  if(fn & 1){\n"
         "    f=*(void(**)(void*))(*(int*)obj + m->adj);\n"
         "  } else {\n    f=(void(*)(void*))m->adj;\n  }\n  f(obj);",
         "  if(fn & 1){\n"
         "    hal_call_state_fn(obj, (unsigned)(*(int*)obj + m->adj));\n"
         "  } else {\n    hal_call_state_fn(obj, (unsigned)m->adj);\n  }"),
    ],
    # Lane shadow-A: the camera state machine's two dispatchers, the same
    # shape on a second seam. The State objects at 0x0209b008.. are mwcc
    # member-function pairs {code address, this-delta} that __sinit_02073a24
    # copies from relocated data; on the host the code word is a DS address
    # and hal/camera_states.cpp's hal_call_camera_state_fn translates it
    # through the address switch in camera_states.inc (every delta in the
    # ROM's pairs is 0). Each entry swaps the one PMF call for that seam,
    # reading the same word the src reads; the seam's declaration rides in
    # CALL_STATE_FN_DECL at file scope (the sources are C++, so a block-scope
    # declaration would take C++ linkage). onEnter is word 0 of the State
    # (func_0200cae4); main is word 2 (func_0200ca50, the src's `obj + 8`).
    # Retires port/unmatched/func_0200cae4_hostcopy.cpp and
    # func_0200ca50_hostcopy.cpp.
    "func_0200cae4": [
        ("  return (c->**p)();",
         "  return hal_call_camera_state_fn(c, *(unsigned *)p);"),
    ],
    "func_0200ca50": [
        ("            r5 = (self->**pp)();",
         "            r5 = hal_call_camera_state_fn(self, *(unsigned *)pp);"),
    ],
}


# ---- A TYPED POINTER-TO-MEMBER DISPATCH, ROUTED AT THE SITE ----------------
#
# Run link100, lane HOSTGEN2. MG_PMF_CALL above handles the sites that OPEN-CODE
# the ARM Itanium sequence in plain ints. This table handles the sites that
# spell a real member-pointer TYPE and let the compiler do the decode:
#
#     (((PanelC_eb8 *)c)->*data_ov006_02142840[idx])(i);
#
# WHAT IS WRONG WITH ONE OF THOSE ON THE HOST IS ONE THING, AND IT IS NOT THE
# WIDTH. Every PORT_HOST_ABI ruling that names this shape says "MSVC's 4-byte
# member pointer cannot express the ROM's 8-byte {code, this-adjust} pair", and
# that half of every one of them went obsolete when port/CMakeLists.txt put
# `/vmg /vmm` on add_compile_options (run link100, lane PMF; the measurement
# table is in that block). Under the pair an MSVC pointer to member function is
# EIGHT bytes in every inheritance shape, complete or forward-declared, word 0
# the code address and word 1 the this-adjust. The stride is the ROM's, the
# field order is the ROM's, and a table the ROM's own __sinit fills is read at
# the width it was written.
#
# What is left is the CODE WORD: it is a DS address, and MSVC's call sequence
# calls it. On the host that lands inside the mounted overlay's DATA image and
# jumps into mapped data. The port owns the answer -- the per-class address
# switches in port/unmatched/Mg*_StateDispatch.cpp -- so the correction is to
# read the two words the pointer already holds and hand them to the class's
# seam. port/hal/pmf_dispatch.h is that read, with a static_assert on the width
# so a build that loses /vmg /vmm stops here instead of reading half a pointer.
#
# PER SITE, NOT PER TU, AND THAT IS MEASURED RATHER THAN CAUTIOUS. Inside ONE
# translation unit some of a class's tables are SEATED with host addresses at
# boot and some still hold the ROM's DS words. dScMgPanel_c has both:
# data_ov006_021427bc and data_ov006_02142860 are seated by
# port_mg_panel_states_seat in unmatched/MgPanel_StateDispatch.cpp, so their two
# dispatches are already correct, while 021427ec, 02142820 and 02142840 are not
# seated and route through the switch. Routing a SEATED table would be a
# regression: the switch is keyed on DS addresses, so it would read a host
# address as unknown and swallow the call. A blanket per-TU rewrite cannot tell
# the two apart. So each site is named, and each seated site it must NOT touch
# is named too, in PMF_SEAM_ALLOW.
#
# THE RESIDUE CHECK IS WHAT MAKES THIS REPEATABLE. After the patches run, emit()
# counts the `->*` operators left in a listed TU and refuses the file unless
# every one of them is an allowed line. A dispatch site that appears in a TU
# this table covers -- a new one from main, or one whose text moved -- fails the
# build with its own name instead of shipping a jump into mapped data.
_PMF_SEAM_INCLUDE = '#include "hal/pmf_dispatch.h"\n'
PMF_SEAM_DECL = {
    "dScMgPanel_c": _PMF_SEAM_INCLUDE +
        'extern "C" void port_mg_panel_call0(void *, unsigned, int);\n'
        'extern "C" void port_mg_panel_call1(void *, unsigned, int, int);\n',
    "dScMgHanachan_c": _PMF_SEAM_INCLUDE +
        'extern "C" void port_mg_wiggler_call0(void *, unsigned, int);\n',
    "dScMgRoulette_c": _PMF_SEAM_INCLUDE +
        'extern "C" void port_mg_roulette_call0(void *, unsigned, int);\n',
}
PMF_SEAM = {
    # dScMgPanel_c. SIX sites over FIVE strings (the 02106fdc and 0210709c
    # loops are textually identical and take the same rewrite), closing six of
    # the seven-row LNK2005 block against unmatched/MgPanel_StateDispatch.cpp;
    # the seventh, func_ov006_02106bc0, is an open-coded site MG_PMF_CALL above
    # already routes.
    #
    #   func_ov006_02104c60   table 021427ec   arity 0   not seated
    #   func_ov006_021057f0   table 02142820   arity 0   not seated
    #   func_ov006_02106eb8   table 02142840   arity 1   not seated
    #   func_ov006_02106f44   table 02142840   arity 1   not seated
    #   func_ov006_02106fdc   table 02142840   arity 1   not seated
    #   func_ov006_0210709c   table 02142840   arity 1   not seated
    #
    # The receiver handed to the seam is the src's OWN receiver expression,
    # unadjusted, and the adjustment word rides along raw: the seam applies the
    # ROM's `code != 0 && adj == 0` rule and reports anything else, which is
    # what all seven host copies did and all the ROM's own pairs support.
    "dScMgPanel_c": [
        ("        (cc->*data_ov006_021427ec[*(u8 *)(c + 0x4000 + 0x686)])();",
         "        PORT_PMF_CALL0(port_mg_panel_call0, cc,\n"
         "                       data_ov006_021427ec[*(u8 *)(c + 0x4000 + 0x686)]);"),
        ("    (c->*data_ov006_02142820[c->idx])();",
         "    PORT_PMF_CALL0(port_mg_panel_call0, c, data_ov006_02142820[c->idx]);"),
        ("        (((PanelC_eb8 *)c)->*data_ov006_02142840[idx])(i);",
         "        PORT_PMF_CALL1(port_mg_panel_call1, c,\n"
         "                       data_ov006_02142840[idx], i);"),
        ("        (((PanelC_f44 *)c)->*data_ov006_02142840[idx])(i);",
         "        PORT_PMF_CALL1(port_mg_panel_call1, c,\n"
         "                       data_ov006_02142840[idx], i);"),
        ("        (c->*data_ov006_02142840[idx])(i);",
         "        PORT_PMF_CALL1(port_mg_panel_call1, c,\n"
         "                       data_ov006_02142840[idx], i);"),
    ],
    # dScMgHanachan_c. FOUR sites over THREE strings (func_ov006_020eb018 and
    # func_ov006_020eb31c spell the same two lines and take the same rewrite),
    # closing four of the five rows against unmatched/MgWiggler_StateDispatch.cpp;
    # the fifth, func_ov006_020eb610, open-codes its decode and is routed by
    # MG_PMF_CALL above.
    #
    # NO TABLE HERE: every one of this class's pairs is a FIELD -- at +0x10 of
    # the per-Wiggler sub-object for three of them and at scene +0x4660 for the
    # Behavior -- filled from the ROM's own .data. Nothing seats them, so the
    # code word is a DS address at every site and all four route.
    #
    #   func_ov006_020eb018        field +0x10          arity 0
    #   func_ov006_020eb0c8        field +0x10 (`cb`)   arity 0
    #   func_ov006_020eb31c        field +0x10          arity 0
    #   dScMgHanachan_c::Behavior  field +0x4660        arity 0, vtable slot 6
    "dScMgHanachan_c": [
        ("    (o->*(o->pmf))();",
         "    PORT_PMF_CALL0(port_mg_wiggler_call0, o, o->pmf);"),
        ("        (((Self *)self)->*(((Self *)self)->cb))();",
         "        PORT_PMF_CALL0(port_mg_wiggler_call0, self,\n"
         "                       ((Self *)self)->cb);"),
        ("    (((CB *)c)->*(*(PMF *)(c + 0x4660)))();",
         "    PORT_PMF_CALL0(port_mg_wiggler_call0, c,\n"
         "                   *(PMF *)(c + 0x4660));"),
    ],
    # dScMgRoulette_c. ONE site: the wheel object's pair at offset 0, dispatched
    # by func_ov006_02107db8 every frame, with a universe of exactly two code
    # words -- both in unmatched/MgRoulette_WheelDispatch.cpp's switch. The same
    # body's OTHER defect, a shadow-class call of ROM vtable slot 3 on a host
    # Model, is a VIRTUAL_CALL row and not a seam row.
    "dScMgRoulette_c": [
        ("    (c->*(c->pmf))();",
         "    PORT_PMF_CALL0(port_mg_roulette_call0, c, c->pmf);"),
    ],
}

# The dispatch sites a listed TU is allowed to keep RAW, each with the reason
# it is correct as it stands. Anything else left holding `->*` after the
# patches is a refusal.
PMF_SEAM_ALLOW = {
    "dScMgPanel_c": [
        # data_ov006_021427bc: seated with three one-argument __fastcall faces
        # by port_mg_panel_states_seat (lane PMFB5). The words are host
        # addresses by the time any state runs.
        "    (c->*data_ov006_021427bc[j].pmf[0])(0);",
        # data_ov006_02142860: seated with five plain cdecl bodies by the same
        # installer (lane PMFB3).
        "    (c->*(data_ov006_02142860[c->idx].pmf))();",
    ],
}


# ---- AN UNINITIALISED LOCAL THAT mwcc'S REGISTER ALLOCATION MADE HARMLESS --
#
# Run link100, lane HOSTGEN2. src/actors/dScMgCup_c.cpp's func_ov006_020def80
# declares `int cup;`, never assigns it on two of its paths, and reaches the
# epilogue through a goto that uses it. On ARM mwcc allocated it to r1, which
# still held the function's own `i`, so the byte-matched body works and the TU's
# own header lists that spelling as one of five that MUST NOT BE TIDIED: hoisting
# the assignment takes the function from MATCH to DIFF. That is a decomp
# constraint, not a port one. MSVC's allocation makes the same statement a wild
# write, which is the whole reason
# port/unmatched/MgCup_UninitEpilogue_020def80.cpp exists.
#
# So the host build initialises it to the value the ROM's register held, and the
# matched tree keeps its byte match. One exact string, with the line above it for
# context because `int cup;` alone appears more than once in the file.
UNINIT_LOCAL = {
    "dScMgCup_c": [
        ("    char *row = c + i;\n    int cup;",
         "    char *row = c + i;\n"
         "    /* hostgen UNINIT_LOCAL: mwcc left this in r1, which still held\n"
         "       `i`; the gotos below reach the epilogue without assigning it,\n"
         "       and MSVC's allocation makes that a wild write. See the table. */\n"
         "    int cup = i;"),
    ],
}


def uninit_local_patch(text, sym):
    """Give a local the value the ROM's register happened to hold."""
    return apply_patches(text, sym, UNINIT_LOCAL, "UNINIT_LOCAL")


def pmf_seam_patch(text, sym):
    """Route a TYPED member-pointer dispatch through the class's seam."""
    return apply_patches(text, sym, PMF_SEAM, "PMF_SEAM",
                         PMF_SEAM_DECL.get(sym, ""))


def pmf_seam_residue(text, sym, suffix):
    """Refuse a listed TU that still carries an unlisted `->*` dispatch."""
    if sym not in PMF_SEAM:
        return
    if suffix == ".c":
        sys.exit("hostgen: %s: PMF_SEAM covers C++ translation units only -- "
                 "the header it includes declares templates and a .c source is "
                 "emitted inside an extern \"C\" wrap." % sym)
    left = text.count("->*")
    allowed = 0
    for line in PMF_SEAM_ALLOW.get(sym, ()):
        if line not in text:
            sys.exit("hostgen: %s: PMF_SEAM_ALLOW line no longer matches:\n  %s\n"
                     "The seated dispatch moved. Re-read it against the seat "
                     "before changing this list." % (sym, line))
        allowed += text.count(line) * line.count("->*")
    if left != allowed:
        sys.exit(
            "hostgen: %s: %d pointer-to-member dispatch(es) left unrouted after "
            "PMF_SEAM (%d allowed raw). A `->*` site in this TU either routes "
            "through the class seam or is listed in PMF_SEAM_ALLOW with the "
            "seat that makes it correct. Calling the stored word raw jumps into "
            "the mounted overlay's data image." % (sym, left - allowed, allowed))


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
    # Run link100, lane SEAT6, batch B6. The same disagreement on a RETURN
    # rather than a parameter. src/func_0200ee8c.c declares
    # `extern s32 GetStarCameraSetting(s32 star);` and indexes the six-entry
    # table data_020876e4 with the whole of it, while src/GetStarCameraSetting.c
    # DEFINES it `unsigned char GetStarCameraSetting(int idx)` returning
    # `(data_02092134 >> (idx*4)) & 0xf` -- so the ROM's `bl` leaves a 4-bit
    # value in r0 with the high bits already cleared by the AND, and the
    # s8/s32 declaration mismatch is harmless on ARM. Under MSVC the cdecl
    # caller reads the whole of EAX, whose high bytes the callee never wrote:
    # a call that should return 1 came back 0x1001 and indexed 0x4000 bytes
    # past the table (measured on the king-defeat star path, the c0000005 in
    # ProcessKuppaScript that port/hal/star_flow.cpp's block records).
    # Declaring the return the width the definition really has makes MSVC
    # read AL alone, which is what ARM did. Retires the func_0200ee8c body in
    # port/hal/star_flow.cpp.
    "func_0200ee8c": [
        ("extern s32 GetStarCameraSetting(s32 star);",
         "extern unsigned char GetStarCameraSetting(s32 star);"
         "  /* hostgen ARG_WIDTH: the definition returns u8, see the table */"),
    ],
}


# ---- CALLEE SEAM: ONE TU'S LOADER CALLEES ROUTED TO A HOST SEAM ------------
#
# Lane HUD-INITRES. HUD::InitResources (src/_ZN3HUD13InitResourcesEv.cpp) is
# the ROM's own loader shape twenty-five times over:
#
#     h = LoadFile(0x229);                  the file, STILL COMPRESSED
#     DecompressLZ16(h, vram_b + 0x2000);   expanded into VRAM by the caller
#     Deallocate(h);
#
# and, for the palettes, LoadFile then GX::LoadOBJPltt / GXS::LoadOBJPltt on
# the raw bytes. Two things stop that body linking and running as it stands,
# and neither is in the body. The port's LoadFile (hal/level_boot.cpp) is
# SharedFilePtr-backed and hands back DECODED bytes, so the DecompressLZ16
# that follows would LZ-decode an already-decoded buffer straight into VRAM;
# the ROM-faithful spelling of that load is the port's raw loader
# func_0201817c (hal/fs.cpp). And ten of the handles the body names (the
# ARCHIVE members 0xa003/a007/a00a/a00e, 0xac03, 0xb00e, 0x8000..0x8003,
# 0x980f) are not in this extraction, so the raw loader returns null for them
# and DecompressLZ16 / LoadOBJPltt would read it.
#
# Both live BEHIND the callee names, so the reroute is spelled at the NAME
# rather than at the twenty-five call sites: one `#define` per callee, at file
# scope ahead of the TU's own declarations, so the TU's `void* LoadFile(int)`
# declares the seam and every call in the body resolves to it with the body
# verbatim. The seams are hal/hud_load.cpp: LoadFile is the raw loader plus
# the extraction's null path (say so once, hand back null); the two consumers
# skip a null and otherwise call the port's own DecompressLZ16 / LoadOBJPltt
# unchanged. Deallocate is not routed: src/Deallocate.c tolerates a null.
#
# Per symbol, and hard-errored like the exact-string tables: a listed callee
# the TU no longer calls is a src that moved, and the entry has to be re-read
# rather than left inert.
CALLEE_SEAM = {
    "_ZN3HUD13InitResourcesEv": [
        ("LoadFile",       "port_hud_loadfile"),
        ("DecompressLZ16", "port_hud_lz16"),
        ("LoadOBJPltt",    "port_hud_objpltt"),   # GX:: and GXS:: alike
    ],
}


def callee_seam_patch(text, sym):
    """Route a TU's named callees to host seams, by #define at file scope."""
    seams = CALLEE_SEAM.get(sym)
    if not seams:
        return text, 0
    lines = ["/* hostgen CALLEE_SEAM: this TU's loader callees resolve to",
             "   hal/hud_load.cpp -- see the table */"]
    for callee, seam in seams:
        if not re.search(r"\b%s\s*\(" % re.escape(callee), text):
            sys.exit(
                "hostgen: %s: CALLEE_SEAM names %s but the source no longer "
                "calls it.\nThe source moved. Re-read the entry -- do NOT drop "
                "it, it is load-bearing." % (sym, callee))
        lines.append("#define %s %s" % (callee, seam))
    return "\n".join(lines) + "\n" + text, len(seams)


def arg_width_patch(text, sym):
    """Retype one parameter to the width its matched caller declares."""
    return apply_patches(text, sym, ARG_WIDTH, "ARG_WIDTH")


# ---- AN ARM REGISTER RIDE-THROUGH THE HOST HAS TO SPELL ----------------------
#
# Lane RIDE-020E444C. Two matched TUs can disagree about a function's ARITY the
# same way ARG_WIDTH's pair disagree about a parameter's width, and both still
# be byte matches: on ARM the argument is already in r0 and the caller does not
# have to put it there. include/decl_common.h:1461 declares
#
#     extern int func_ov002_020e3f90(void);
#
# and src/func_ov002_020e444c.c calls it through that prototype with no
# argument, while src/func_ov002_020e3f90.c:15 DEFINES it `int
# func_ov002_020e3f90(char* self)` and dereferences self on its first line
# (`*(char**)(self + 0x35c)`). The ROM says the two agree -- 0x020e444c's
# prologue is `push {r4,r5,r6,lr}` / `sub sp,#0x28` / `mov r4,r0` /
# `bl 0x020e3f90`, so r0 is still the incoming Player at the branch and nothing
# between entry and the call touches it. mwccarm emitted no `mov r0,..` because
# there was nothing to move, and the byte gate is satisfied.
#
# Under x86 cdecl the caller pushes nothing and the callee reads its `self`
# slot out of whatever the frame happened to leave there. That is the function
# that seats the player's head-model world matrix
# (`*(Matrix4x3*)(head+0x1c) = data_020a0e68`), so the failure is a wrong head
# transform rather than a fault.
#
# The patch spells the argument the ROM rode through r0 and nothing else. Its
# HEADER_SHADOW entry hides decl_common.h's `(void)` declaration for this TU
# only, and REG_RIDE_ARG_DECL puts the definition's own signature back, so the
# call site type-checks against the arity src/func_ov002_020e3f90.c really has.
# Neither src/ nor include/ moves. Exact strings, hard-errored by
# apply_patches: if the decomp ever settles the two declarations on one arity,
# the entry stops matching and says so rather than going quietly inert.
# Retires port/unmatched/func_ov002_020e444c_hostcopy.c, which was this whole
# hundred-line body transcribed to change this one call.
#
# THE GO/NO-GO TEST, ANSWERED. CALLEE_SEAM's disqualifier is a receiver that
# exists only in a register with no in-scope expression naming it. That is NOT
# this case: the receiver is `c`, func_ov002_020e444c's own first parameter,
# live and named at the call site (the next statement reads `*(int *)(c +
# 0x690)`). Nothing is guessed and nothing is reached for. So the retirement is
# admissible.
#
# It is spelled here rather than through CALLEE_SEAM for two reasons. First,
# CALLEE_SEAM emits an OBJECT-LIKE `#define callee seam`, which can send a call
# somewhere else but cannot change what the call PASSES: `func_ov002_020e3f90()`
# stays argless whatever name it resolves to. The missing thing here is an
# argument, not a destination. Second, there is nothing to seam TO. The correct
# destination is the matched body src/func_ov002_020e3f90.c, which already
# takes `char* self` and is already linked in this slice; interposing a host
# seam function would ADD a stand-in to retire one. The rewrite therefore lands
# on the call expression, and hard-errors the same way CALLEE_SEAM does if the
# source moves.
# THE INJECTED DECLARATION IS PER SYMBOL (run link100, lane SEAT6). It used to
# be one module-level string prepended to whatever TU this table touched, which
# was correct while the table had one row and would have put func_ov002_020e3f90
# 's declaration at the top of four unrelated generated TUs the moment it had
# five. A row that patches the TU's OWN declaration -- which every row added
# below does -- needs no injection at all and gets none.
REG_RIDE_ARG_DECL = {
    # DROPPED 2026-09-14 with its patch below (main -> port sync, lane SYNC7).
}
REG_RIDE_ARG = {
    # DROPPED 2026-09-14 (main -> port sync, lane SYNC7), reason gone rather
    # than moved. This row existed because the matched C spelled the call with
    # empty parentheses and relied on ARM r0 still holding the player across
    # the branch. main's commit 826d762a0, "func_ov002_020e3f90 takes
    # the player it works on: shared declaration and its caller", gave the
    # shared declaration in include/decl_common.h:1404 the parameter and
    # rewrote the one call site, so src/func_ov002_020e444c.c:30 now reads
    # `if (func_ov002_020e3f90(c) == 0) {` on its own. The patch text stopped
    # matching in this merge, which is the tool saying so out loud, and the
    # injected declaration would now be a second, identical declaration.
    # "func_ov002_020e444c": [
    #     ("    if (func_ov002_020e3f90() == 0) {",
    #      "    if (func_ov002_020e3f90(c) == 0) {"),
    # ],
    # ------------------------------------------------------------------
    # Run link100, lane SEAT6, batch B6. Four more of the same shape, each
    # patching the DECLARATION the TU itself carries plus the call sites that
    # use it, so nothing is injected and neither src/ nor include/ moves.
    #
    # ModelComponents::Render (arm9). src declares
    # `extern void func_02044b30(ModelComponents* self);` and calls it
    # `func_02044b30(this)` at both ends of the module walk, while
    # src/func_02044b30.c DEFINES it `void func_02044b30(char *obj, int idx)`
    # and indexes `obj+4`'s module array by `idx * 0x30` on its first line. On
    # ARM the material index is already in r1 from the loop's own arithmetic
    # and mwccarm had nothing to move; under cdecl the callee reads its `idx`
    # off a stack slot the caller never wrote. `idx` is the loop's own
    # `u8 idx` and is live and named at both call sites. Retires
    # port/unmatched/ModelComponents_Render.cpp (a host REIMPLEMENTATION, and
    # its banner says so: "not byte-verified against the ROM").
    "_ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3": [
        ("extern void func_02044b30(ModelComponents* self);",
         "extern void func_02044b30(ModelComponents* self, int idx);"
         "  /* hostgen REG_RIDE_ARG: the material index rides r1, see the "
         "table */"),
        ("                        func_02044b30(this);",
         "                        func_02044b30(this, idx);"),
    ],
    # Bubba's chase gate (ov032). src declares
    # `extern char* _ZN8dActor_c13ClosestPlayerEv(void);` and calls it with no
    # argument; Actor::ClosestPlayer is a __thiscall method that reads
    # `this + 0x5c`. ROM 0x02111350 is `push {r4,lr} / mov r4,r0 /
    # bl 0x02010ad8`, so r0 still holds the receiver at the branch. The body's
    # own first parameter `c` is that value. port/tools/closestplayer_guard.py
    # refuses this TU in any slice for exactly this reason, and it is right to:
    # what it scans is the RAW source, and the generated TU below is the one
    # that gets compiled. Retires port/unmatched/Bubba_ChaseGate.cpp.
    # DROPPED 2026-09-13 (main -> port sync, lane SYNC5), reason gone rather
    # than moved. main folded Bubba's chase gate into
    # src/game/actors/d_a_bakubaku.cpp, and that TU already declares
    # `extern char* _ZN8dActor_c13ClosestPlayerEv(char* c);` (line 78) and
    # calls it `(c)` (line 874) -- the TU merge resolved the two legacy
    # declarations and kept the one with the receiver, which is exactly what
    # this patch used to write. The TU is on four live slices, so the body is
    # compiled from main's own spelling and a generated copy would only be a
    # duplicate symbol.
    # The two TextureSequence::Prepare callers, the SHORT-1 argsweep row.
    # Prepare is a real non-static C++ method, Prepare(BMD_File &model,
    # BTP_File &animFile), so it consumes THREE ARM registers (r0 this, r1
    # model, r2 animFile) and its ROM body is a 0xc tail-call veneer into
    # func_02046d50 that touches none of them -- whatever the caller has
    # loaded rides straight through. Both matched TUs declare it as a
    # TWO-argument free function. The host bridge (hal/player_bridges.cpp) is
    # the real three-parameter face and DEREFERENCES its third argument
    # (`*(BTP_File*)btp`) before func_02046d50, which never reads it, runs. The
    # third argument is therefore the caller's own second value again -- a
    # known-valid BTP_File pointer -- which is the shape the two host copies
    # this retires already ship and which MotherPenguin_InitResources.cpp
    # derived. Retires port/unmatched/TexSeq_Caller_ov002_020e5948.cpp and
    # port/unmatched/Snowman_InitResources.cpp.
    "func_ov002_020e5948": [
        ("extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File"
         "(void* bmd, void* btp);",
         "extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File"
         "(void* self, void* bmd, void* btp);"
         "  /* hostgen REG_RIDE_ARG: the third register rides through, see "
         "the table */"),
        ("                _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File"
         "((void*)((s32*)data_ov002_0210a69c[k])[1], (void*)((s32*)entry)[1]);",
         "                _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File"
         "((void*)((s32*)data_ov002_0210a69c[k])[1], (void*)((s32*)entry)[1], "
         "(void*)((s32*)entry)[1]);"),
        ("    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File"
         "((void*)data_ov002_0210ebb8[1], (void*)data_ov002_0210e8d0[1]);",
         "    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File"
         "((void*)data_ov002_0210ebb8[1], (void*)data_ov002_0210e8d0[1], "
         "(void*)data_ov002_0210e8d0[1]);"),
        ("    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File"
         "((void*)data_ov002_0210eb20[1], (void*)data_ov002_0210ebd8[1]);",
         "    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File"
         "((void*)data_ov002_0210eb20[1], (void*)data_ov002_0210ebd8[1], "
         "(void*)data_ov002_0210ebd8[1]);"),
    ],
    # RE-KEYED 2026-09-13 (main -> port sync, lane SYNC5). Lane SEAT6 keyed
    # this on the per-function TU src/func_ov072_02120a44.c; main has since
    # folded that body into src/game/actors/d_a_bg_snwmn.cpp, so the key is
    # the TU stem now and the TU is declared in PORT_HOSTGEN_TU. Both patch
    # strings are re-read from main's own spelling of the body -- the
    # declaration and the call are each wrapped over two lines there, which
    # the old strings were not -- and apply_patches hard-errors if either
    # stops matching.
    # DROPPED 2026-09-14 (main -> port sync, lane SYNC7). The anchor is gone,
    # not moved: this row rewrote a FLAT two-argument declaration and its one
    # call into a three-argument one, and main has since given the class a
    # real header. include/TextureSequence.h:75 declares
    # `static void Prepare(BMD_File &model, BTP_File &animFile);` with a
    # comment that says "Prepare is static, for the reason set out below", and
    # src/game/actors/d_a_bg_snwmn.cpp:106 calls it
    # `TextureSequence::Prepare(**bmd, **btp)`. There is no flat declaration
    # left in the TU to widen.
    #
    # WHAT THAT COSTS IS NOT NOTHING, and it is recorded rather than patched
    # here. The ROM body is a method and consumes three ARM registers; the
    # port's face takes three and dereferences the third. A two-argument
    # static call cannot feed it, so the ov072 snowman site is now the same
    # open question as every other C++-spelled Prepare caller main has, and
    # closing it means teaching hostgen to rewrite a MEMBER call into the flat
    # three-argument one, which is lane HOSTGEN4's rule to write and not a row
    # in this table. out/SYNC7/bugs.md carries it.
    # "d_a_bg_snwmn": [ ... the two patches, in git history at d9ea8dda4 ... ],
    # ADDED 2026-09-13 (main -> port sync, lane SYNC6), and it retires
    # port/unmatched/Player_ReleaseHeldDispatch.cpp, which was this whole body
    # transcribed to change one call.
    #
    # func_ov002_020bdb50 is Player's "let go of the held object" path. For one
    # held-object kind it hands the object to func_ov002_020d5cec, and the ROM
    # leaves that object in r0 across the branch:
    #
    #     020bdbb8: ldr  r0, [r4, #0x360]    ; r0 = the held object
    #     020bdbbc: ldrh r1, [r0, #0xc]      ; r0 kept
    #     020bdbd4: bl   func_ov002_020d5cec ; r0 still the held object
    #
    # so the matched C spells the call with no argument and byte-matches. Under
    # cdecl nothing is pushed and the callee dereferences whatever the frame
    # left in that slot. main's own consolidation already spells the OTHER call
    # of the same function with the object (line 5556 passes
    # *(char**)((char*)&mObjInMouth)); this site is the one it did not reach, so
    # the two declarations in the TU disagree with each other today.
    #
    # The receiver is named and live at the call: obj is the enclosing block's
    # own char* obj = *(char**)(c+0x360), the same expression the host copy
    # passed. Both anchors match exactly once in the TU (measured), and
    # apply_patches hard-errors if either stops matching.
    "Player": [
        ("    extern void func_ov002_020d5cec(void);",
         "    extern void func_ov002_020d5cec(void*);"
         "  /* hostgen REG_RIDE_ARG: the held object rides r0, see the table */"),
        ("            func_ov002_020d5cec();",
         "            func_ov002_020d5cec(obj);"),
    ],
}


def reg_ride_arg_patch(text, sym):
    """Spell an argument the ROM's caller left riding in a register."""
    return apply_patches(text, sym, REG_RIDE_ARG, "REG_RIDE_ARG",
                         REG_RIDE_ARG_DECL.get(sym, ""))


# ---- A VTABLE EXTERN THAT MSVC DECORATED -----------------------------------
#
# PORT_HOST_ABI, run link100, lane VPTR item 1 of the ruling: nine of the
# seventy vtable stores name their table with an MSVC-DECORATED symbol, because
# the declaration the translation unit sees is a plain C++ `extern int
# _ZTV<X>[];` rather than one inside an extern "C" block. mwccarm has one
# spelling and does not care; MSVC emits `?_ZTV7daBmb_c@@3PAHA`, the port's host
# array and the /alternatename that binds it are the C name `__ZTV7daBmb_c`, and
# an /alternatename is a NAME bridge that cannot cross a decoration. So the row
# sits on the unresolved wall:
#
#   daBmb_c.cpp.obj : error LNK2019: unresolved external symbol
#       "int * _ZTV7daBmb_c" (?_ZTV7daBmb_c@@3PAHA) referenced in function
#       _daBmb_c_classInit
#
# THE DECLARATION IS IN A HEADER, NOT THE TU (include/daBmb_c.h:158,
# include/daEyBm_c.h:83; only d_a_mc_flag.cpp spells its own at line 66), and
# hostgen rewrites translation units, not headers. What works instead is a
# declaration with C linkage AHEAD of the include: C++ says a redeclaration
# that carries no linkage-specification keeps the linkage already specified, so
# the header's plain `extern int _ZTV7daBmb_c[];` inherits it rather than
# fighting it. Measured on this toolchain rather than read off the standard --
# cl /c on `extern "C" { extern int _ZTV7daBmb_c[]; }` followed by the header
# and a use emits one undefined symbol, `__ZTV7daBmb_c`.
#
# The element type has to match the header's, so it is written out per row
# rather than assumed: dScMgSound_c's table is `void *`, not `int`.
#
# Retiring the decoration also brings the store itself back to the address
# point, because VPTR_ADDRESS_POINT below runs over the same emitted text: the
# row leaves tools/vptr_addend_baseline.txt in the same change that closes it
# on the wall.
ZTV_C_LINKAGE = {
    "daBmb_c": [("int", "_ZTV7daBmb_c")],
    # DROPPED 2026-09-14 (main -> port sync, lane SYNC7), reason gone rather
    # than moved, and the same reason for both. Lane HOSTGEN3 wrote these two
    # rows the day before because each TU stored its own table by name, and
    # MSVC spelled that reference decorated, which no /alternatename can bind.
    # main's cleanup of both classes replaced the raw store with a real
    # constructor call (daEyBm_c_classInit is `return new daEyBm_c();` now),
    # so neither TU names its vtable symbol at all: outside decl_common.h no
    # file under src/ or include/ mentions _ZTV8daEyBm_c or _ZTV10daMcFlag_c,
    # and hostgen hard-errors on a row whose name is absent rather than write
    # a declaration nothing uses. What those two classes dispatch through is
    # MSVC's own vtable now rather than the port's hosted array, which is the
    # vptr address-point question and is lane HOSTGEN4's work, not this row's.
    # "d_a_ey_bm": [("int", "_ZTV8daEyBm_c")],
    # "d_a_mc_flag": [("int", "_ZTV10daMcFlag_c")],
    # FOLD NOTE (lane INT3, 2026-09-14): the two rows above stay dropped.
    # Lane HOSTGEN4's branch predates the main -> port sync and still
    # carries them live; in the folded tree the names they ask for are
    # gone, and ztv_c_linkage hard-errors on a row whose name is absent.
    # HOSTGEN4's own new row, daPkn_c, is taken unchanged.
    # daPkn_c, added with its seat (lane HOSTGEN4). The TU's declaration of
    # its own table is a plain C++ `extern int _ZTV7daPkn_c[];`, so MSVC
    # asks for ?_ZTV7daPkn_c@@3PAHA; under C linkage it asks for
    # __ZTV7daPkn_c, which hal/actor_classes_l7.cpp line 585 already
    # bridges to __ZTV12PiranhaPlant.
    # DROPPED AT THE FOLD (lane INT3, 2026-09-14), for the same reason and in
    # the same words as the two rows above. Lane HOSTGEN4 wrote this row
    # against the PRE-SYNC daPkn_c.cpp, whose daPkn_c_classInit stored the
    # vtable by name: `p[0] = (int)(_ZTV7daPkn_c + 2);`. main has rewritten
    # that factory as `return new daPkn_c();`, so the translation unit does not
    # name its vtable symbol at all and ztv_c_linkage hard-errors on a row
    # whose name is absent. What the class dispatches through is MSVC's own
    # vtable now rather than the port's hosted array, which is the vptr
    # address-point question and not this row's.
    # "daPkn_c": [("int", "_ZTV7daPkn_c")],
    # daGmch_c (MONEYBAG, ov081), added with its seat (run link100, lane
    # SEATS3). The same defect as daBmb_c above, and NOT the one the three
    # dropped rows have: this factory still stores its table by name.
    # src/actors/daGmch_c.cpp line 1111 is
    #     p[0] = (int)&_ZTV8daGmch_c[2];
    # and the declaration it reaches is include/daGmch_c.h:180, a plain C++
    # `extern int _ZTV8daGmch_c[];`, so MSVC asks the linker for
    # ?_ZTV8daGmch_c@@3PAHA. The /alternatename in
    # port/hal/actor_classes_ov081.cpp:747 carries __ZTV8daGmch_c to the host
    # array __ZTV8Moneybag, and an alias is a NAME bridge that cannot cross a
    # decoration, so without this row the seat trades two closed rows for one
    # fresh decorated one. Under C linkage the reference is the name the alias
    # already binds. The +8 address-point bias on the same line is dropped by
    # VPTR_ADDRESS_POINT below, which is the other half of why this TU is
    # substituted rather than sliced raw: compiled raw the bias would be a new
    # nonzero addend and vptr_addend_guard refuses any row that is not in
    # tools/vptr_addend_baseline.txt.
    "daGmch_c": [("int", "_ZTV8daGmch_c")],
    # daMip_c (MIPS the rabbit, ov085), added with its seat (run link100, lane
    # SEATS3). Identical defect to daGmch_c above, one line down:
    # src/actors/daMip_c.cpp line 1883 is
    #     p[0] = (int)(_ZTV7daMip_c + 2);
    # under a plain C++ `extern` the file's own note at line 1867 explains, so
    # MSVC asks for ?_ZTV7daMip_c@@3PAHA while the live alias in
    # port/hal/actor_classes.cpp:1584 carries __ZTV7daMip_c to the host array
    # __ZTV6Rabbit. Measured on the generated object rather than predicted:
    # ?_ZTV7daMip_c@@3PAHA came back as one of its undefined externals and
    # nothing in this link defines it.
    "daMip_c": [("int", "_ZTV7daMip_c")],
}


def ztv_c_linkage(text, sym):
    """Give a TU's vtable externs C linkage, ahead of every include."""
    rows = ZTV_C_LINKAGE.get(sym)
    if not rows:
        return text, 0
    decls = []
    for ctype, name in rows:
        if name not in text:
            sys.exit("hostgen: %s: ZTV_C_LINKAGE names %s, which is not in the "
                     "translation unit any more. The row was written against a "
                     "reference that has moved; re-read the TU rather than "
                     "dropping the row, because without it the reference comes "
                     "out decorated and the /alternatename cannot bind it."
                     % (sym, name))
        decls.append("extern %s %s[];" % (ctype, name))
    block = ("/* hostgen ZTV_C_LINKAGE: the header below declares these "
             "without a\n"
             "   linkage-specification, and a redeclaration keeps the linkage "
             "already\n"
             "   specified. See the table for why the decorated name cannot "
             "link. */\n"
             'extern "C" {\n'
             + "".join("    %s\n" % d for d in decls)
             + "}\n\n")
    return block + text, len(rows)


# ---- A MEMBER THE FACE LEDGER ALREADY DEFINES ------------------------------
#
# Run link100, lane HOSTGEN4. A class TU that has never been on a slice can have
# ONE of its members already defined in the link, because port/faces_sync.txt
# derived a REVERSE face for it: a definition of the decorated member that
# forwards to the flat ROM name. Seating the TU then trades every row it closes
# for one duplicate symbol.
#
# THE CLEAN END STATE IS THE LEDGER'S, not the substitution's: the row flips to
# a FORWARD face, the flat ROM name forwards to the class TU's real member, and
# the host copy that supplies the flat body today is retired. That is one edit
# per row in port/faces_sync.txt, which is another lane's file.
#
# Until then the member is PARKED: the generated copy brackets the body in
# `#if 0`, so the TU seats, the face goes on serving the vtable exactly as it
# does today, and no behaviour changes at all -- the face's forward target is
# the same ROM body the host copy holds. A call to the member from inside the TU
# still binds, to the face, under the same name. Parking is written as a bracket
# rather than a deletion so the body stays readable next to its own ROM
# adjudication, and so retiring the park is one line.
LEDGER_PARK = {
    # dScMgD3DBase_c::OnKicked, ROM ov006:0x020e6e78, vtable slot 24.
    # faces_sync_gen.cpp defines ?OnKicked@dScMgD3DBase_c@@UAEHXZ and forwards
    # it to _ZN14dScMgD3DBase_c8OnKickedEv, whose body is
    # unmatched/MgD3DBase_DeclConflict.cpp. That host copy's stated reason is a
    # C2733 between include/decl_common.h:1516 and this TU's definition, and
    # THAT REASON IS OBSOLETE: the TU compiles clean today. So the retirement
    # this park is waiting on is a pair -- flip the ledger row to a forward face
    # and retire the host copy in the same change -- and neither half is this
    # lane's to make alone.
    "dScMgD3DBase_c": [
        ("int dScMgD3DBase_c::OnKicked()\n{\n    char *self = (char *)this;\n",
         "/* hostgen LEDGER_PARK: port/faces_sync.txt already defines this\n"
         "   member as a reverse face forwarding to the flat ROM name, so a\n"
         "   second definition here is a duplicate symbol. Parked, not deleted.\n"
         "   See the LEDGER_PARK table in tools/hostgen.py. */\n"
         "#if 0\n"
         "int dScMgD3DBase_c::OnKicked()\n{\n    char *self = (char *)this;\n"),
        ("    return 1;\n}\n\n"
         "/* ---------------------------------------------------------------"
         "----------- */\n"
         "/* ROM ordinal 11 -- _ZN14dScMgD3DBase_c8OnPushedEv, 0x020e6e54, "
         "size 0x24 */\n",
         "    return 1;\n}\n"
         "#endif  /* hostgen LEDGER_PARK: dScMgD3DBase_c::OnKicked */\n\n"
         "/* ---------------------------------------------------------------"
         "----------- */\n"
         "/* ROM ordinal 11 -- _ZN14dScMgD3DBase_c8OnPushedEv, 0x020e6e54, "
         "size 0x24 */\n"),
    ],
    # daSanbo_c (Pokey). Five members port/faces_sync.txt already defines as
    # reverse faces, whose flat targets are all defined in this link by
    # their own per-function TUs, plus func_ov096_02135e2c, which is not a
    # face at all but a PORT_HOST_ABI host copy
    # (unmatched/Pokey_HostSites.cpp). Parking that one keeps the host
    # copy's correction on the call and asks nobody to re-adjudicate the
    # ruling tonight.
    "daSanbo_c": [
        # UNPARKED AT THE FOLD (lane INT3, 2026-09-14), half of the recursion
        # fix the coordinator ruled on. The ledger rows these parks were
        # waiting on are retired in port/faces_sync.txt in the same commit, so
        # the member's real ROM body is what defines it now and the hand face
        # in port/hal/ calls a body instead of calling the face that calls it.
        # Retiring the row and deleting the park are ONE change: either half
        # alone is a broken link.
        # func_ov096_02135e2c
        ("/* ROM ordinal 11 -- func_ov096_02135e2c, 0x02135e2c, size 0xd0 */\n",
         "/* ROM ordinal 11 -- func_ov096_02135e2c, 0x02135e2c, size 0xd0 */\n#if 0  /* hostgen LEDGER_PARK */\n"),
        ("/* ROM ordinal 10 -- func_ov096_02135948, 0x02135948, size 0x4e4 */\n",
         "#endif  /* hostgen LEDGER_PARK: func_ov096_02135e2c */\n/* ROM ordinal 10 -- func_ov096_02135948, 0x02135948, size 0x4e4 */\n"),
    ],
    # daPkn_c (Piranha Plant). Three ledger members plus
    # func_ov084_0212f204, whose host copy
    # (unmatched/Actor_ClosestPlayer_OverlayReaders.cpp) carries a receiver
    # ruling lane SYNC6 flagged as worth re-checking against main's text.
    # Parking leaves that ruling exactly where it is.
    "daPkn_c": [
        # UNPARKED AT THE FOLD (lane INT3, 2026-09-14), half of the recursion
        # fix the coordinator ruled on. The ledger rows these parks were
        # waiting on are retired in port/faces_sync.txt in the same commit, so
        # the member's real ROM body is what defines it now and the hand face
        # in port/hal/ calls a body instead of calling the face that calls it.
        # Retiring the row and deleting the park are ONE change: either half
        # alone is a broken link.
        # func_ov084_0212f204
        # RE-ANCHORED AT THE FOLD (lane INT3, 2026-09-14). This park was
        # written against the PRE-SYNC daPkn_c.cpp, which carried a
        # "/* ROM ordinal N -- <name>, <addr>, size <n> */" banner over each
        # body. main's rewrite of this translation unit replaced all of them
        # with "// @symbol <name>" lines, so both anchors named text that is
        # no longer anywhere in the file and hostgen refused the build. The
        # park is KEPT, not dropped: func_ov084_0212f204 is host-copied by
        # port/unmatched/Actor_ClosestPlayer_OverlayReaders.cpp under a
        # standing PORT_HOST_ABI receiver ruling, and nothing tonight
        # re-adjudicates that against ROM bytes.
        #
        # main wraps each retained C-linkage helper in its own extern "C"
        # block, so this bracket opens on the definition line and closes on
        # the body's own last brace, the way daWanwan_c_classInit's does. A
        # banner-to-banner bracket here would leave the extern "C" block's
        # closing brace inside the #if 0.
        ("void func_ov084_0212f204(char* r4){\n",
         "#if 0  /* hostgen LEDGER_PARK: func_ov084_0212f204 */\n"
         "void func_ov084_0212f204(char* r4){\n"),
        ("      *(short*)(r4 + 0x468) = *(short*)(r4 + 0x8e);\n    }\n  }\n}\n",
         "      *(short*)(r4 + 0x468) = *(short*)(r4 + 0x8e);\n    }\n  }\n}\n"
         "#endif  /* hostgen LEDGER_PARK: func_ov084_0212f204 */\n"),
    ],
    # daWanwan_c (Chain Chomp). Three ledger members plus daWanwan_c_classInit,
    # which is a FACTORY host copy (unmatched/ChainChomp_Spawn_hostcopy.cpp) held
    # by the VPTR ruling's sequencing.
    #
    # THE BRACKETS HERE ARE PER BODY, not banner to banner like the others,
    # and that was learned from a build. These four members sit at the TOP of
    # the file, and a banner-to-banner bracket swallowed an
    # `#include "decl_common.h"` and the file-scope declarations of
    # data_ov014_02114968, 02114978, 02114980 and 02114970, which
    # CleanupResources further down still needs: four C2065s on a member
    # nobody was parking. So each bracket opens on the member's own
    # definition line and closes at the next ROM banner, which leaves every
    # include and declaration between the banner and the body outside it.
    # classInit is the exception to the exception: it lives INSIDE an
    # `extern "C" {` block, so its bracket closes on its own last line
    # instead, or the block's closing brace would end up inside the #if 0.
    # d_s_mg_roulette (MG_ROULETTE, scene 383), run link100 lane SEATS3. This
    # is the LEDGER_PARK bracket used for a reason that is not a ledger row:
    # the TU holds exactly two bodies, func_ov006_0210a4ac (an empty ROM stub,
    # the one row the seat closes) and dScMgRoulette_c_classInit, which
    # port/unmatched/MgRoulette_Factory.cpp already host-copies. That host copy
    # is a PORT_HOST_ABI repair, not a convenience: the matched factory calls
    # _ZN11dScMgBase_cC2Ev() with NO argument, relying on r0 still holding the
    # object, one of exactly two such calls in the ROM. port/CMakeLists.txt
    # refuses this TU on slice_rlt.txt by name for that duplicate. The bracket
    # removes the duplicate instead of the slice row, so the host copy and its
    # repair are untouched, the refusal's premise is gone, and the assertion
    # itself is left in place and still guards slice_rlt.txt.
    "d_s_mg_roulette": [
        ('extern "C" void* dScMgRoulette_c_classInit(void){\n',
         "#if 0  /* hostgen LEDGER_PARK: dScMgRoulette_c_classInit -- "
         "port/unmatched/MgRoulette_Factory.cpp hosts this factory for the\n"
         "          dropped-receiver _ZN11dScMgBase_cC2Ev call */\n"
         'extern "C" void* dScMgRoulette_c_classInit(void){\n'),
        ("    _ZN5ModelC1Ev(m + 0x60);\n  }\n  return c;\n}\n",
         "    _ZN5ModelC1Ev(m + 0x60);\n  }\n  return c;\n}\n"
         "#endif  /* hostgen LEDGER_PARK: dScMgRoulette_c_classInit */\n"),
    ],
    "d_a_wanwan": [
        ("void* daWanwan_c_classInit(void){\n",
         "#if 0  /* hostgen LEDGER_PARK: daWanwan_c_classInit */\n"
         "void* daWanwan_c_classInit(void){\n"),
        ("    __cxa_vec_ctor(c+0x578, 7, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);\n  }\n  return c;\n}\n",
         "    __cxa_vec_ctor(c+0x578, 7, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);\n  }\n  return c;\n}\n"
         "#endif  /* hostgen LEDGER_PARK: daWanwan_c_classInit */\n"),
        # UNPARKED AT THE FOLD (lane INT3, 2026-09-14), half of the recursion
        # fix the coordinator ruled on. The ledger rows these parks were
        # waiting on are retired in port/faces_sync.txt in the same commit, so
        # the member's real ROM body is what defines it now and the hand face
        # in port/hal/ calls a body instead of calling the face that calls it.
        # Retiring the row and deleting the park are ONE change: either half
        # alone is a broken link.
    ],
}


def ledger_park(text, sym):
    """Bracket a member port/faces_sync.txt already defines."""
    return apply_patches(text, sym, LEDGER_PARK, "LEDGER_PARK")


# ---- A BODY A PORT_HOST_ABI HOST COPY ALREADY DEFINES ----------------------
#
# Run link100, lane DUPES. The sibling of LEDGER_PARK above, for the other
# reason a seated translation unit arrives with a body the link already has.
# LEDGER_PARK's reason is port/faces_sync.txt: a REVERSE face defines the
# decorated member and the flat ROM name is its forward target, so that park is
# waiting on a ledger row to flip. These five wait on nothing in the ledger.
# Each is a body port/unmatched/ host-copies under a standing PORT_HOST_ABI
# ruling BECAUSE THE MATCHED BODY IS WRONG ON THE HOST, and main's 2026-09
# consolidation folded that body into a class translation unit the port already
# compiles for its other members, so the two collide at the link. They are the
# five LNK2005 rows walk_window carried at port/l7-int4 52c4b9a3b.
#
# The rulings are one per row, in runs/link100/out/ALIAS2/host_abi_conflicts.txt
# and runs/link100/out/HOSTGEN4/needs_main.md section 3, and every one of them
# says THE HOST COPY IS THE BODY THAT RUNS. So the park keeps the host copy and
# its ruling exactly where they are and removes the second definition, which is
# the only end of this that leaves dispatch untouched: the flat name stays
# defined, by the same object that defines it today, and a caller inside the
# generated copy still binds to it under the same name through the declaration
# the translation unit already carries.
#
# NOT a retirement of a host copy and not an edit to src/. If the decomp side
# fixes the source the host copy retires and the row here goes with it, in a
# change gated by a captured scene-to-level transition, because THAT change
# moves what runs. FOUR of these five defects are already absent from main's
# text in the tree this was written against, checked line by line and site by
# site: only func_ov078_02123f1c's dropped GetHealth receiver still reads as
# the ruling describes it. That is written up in runs/link100/out/DUPES/bugs.md
# as the decomp side's finding, and it is not a reason for a port lane to swap
# the bodies tonight.
#
# ANCHORED ON THIS TREE'S TEXT, which is the fold rule lane INT3 wrote after
# four defects of exactly one shape. Every anchor below was taken out of the
# file it patches at 52c4b9a3b and checked to occur exactly once. Each bracket
# opens on the body's own definition line and closes on its own last brace,
# never banner to banner, so an `extern "C" {` block's closing brace or a
# file-scope declaration sitting between a banner and a body can never fall
# inside the #if 0.
HOST_COPY_PARK = {
    # func_ov006_0211ba88, ov006 0x0211ba88, host-copied by
    # port/unmatched/MgSound_ShadowSlot35.cpp, whose header lists four measured
    # defects in the matched translation unit (the dropped receiver at the
    # vtable slot-35 call, the modulo approximated by one subtraction, an int
    # store where the ROM stores a byte, and the n == 0 test nested inside the
    # idx >= limit arm).
    "dScMgSound_c": [
        ("void func_ov006_0211ba88(char *base, int idx)\n{\n",
         "/* hostgen HOST_COPY_PARK: port/unmatched/MgSound_ShadowSlot35.cpp\n"
         "   defines this ROM name under a standing PORT_HOST_ABI ruling, so a\n"
         "   second definition here is a duplicate symbol. Parked, not deleted;\n"
         "   the host copy goes on defining the name and nothing dispatches\n"
         "   differently. See the HOST_COPY_PARK table in tools/hostgen.py. */\n"
         "#if 0  /* hostgen HOST_COPY_PARK: func_ov006_0211ba88 */\n"
         "void func_ov006_0211ba88(char *base, int idx)\n{\n"),
        ("    ctx->ent[idx].b0d = 0;\n    ctx->ent[idx].b0e = 0;\n}\n",
         "    ctx->ent[idx].b0d = 0;\n    ctx->ent[idx].b0e = 0;\n}\n"
         "#endif  /* hostgen HOST_COPY_PARK: func_ov006_0211ba88 */\n"),
    ],
    # func_ov078_02123f1c, ov078 0x02123f1c, host-copied by
    # port/unmatched/KingBobOmb_HeldHealthCheck.cpp. The ruling is an implicit
    # register argument: Player::GetHealth's receiver is c->field_494 and rode
    # r0 on ARM, which is why the matched body calls it with no argument at all.
    # That spelling is still here in main's text, three lines below the anchor.
    "daBombking_c": [
        ('extern "C" int func_ov078_02123f1c(CView* c)\n{\n',
         "/* hostgen HOST_COPY_PARK: port/unmatched/KingBobOmb_HeldHealthCheck.cpp\n"
         "   defines this ROM name under a standing PORT_HOST_ABI ruling (the\n"
         "   GetHealth receiver rides r0), so a second definition here is a\n"
         "   duplicate symbol. Parked, not deleted. See the HOST_COPY_PARK\n"
         "   table in tools/hostgen.py. */\n"
         "#if 0  /* hostgen HOST_COPY_PARK: func_ov078_02123f1c */\n"
         'extern "C" int func_ov078_02123f1c(CView* c)\n{\n'),
        ("        KingBobOmb_SetState(c, data_ov078_0212709c);\n"
         "    }\n    return 1;\n}\n",
         "        KingBobOmb_SetState(c, data_ov078_0212709c);\n"
         "    }\n    return 1;\n}\n"
         "#endif  /* hostgen HOST_COPY_PARK: func_ov078_02123f1c */\n"),
    ],
    # daObjBSwdoor_c_classInit, ov014, host-copied by
    # port/unmatched/ShutterBob_Spawn_hostcopy.cpp, which carries a measured
    # fault: the matched factory emitted its two pooled vtable constants in
    # POOL address order rather than STORE order, the base table won, slot 0
    # came up null and the object faulted before its InitResources ran. Four
    # lines, so the bracket is one pair around the whole body.
    "d_a_obj_b_swdoor": [
        ('extern "C" daObjBSwdoor_c *daObjBSwdoor_c_classInit()\n'
         "{\n    return new daObjBSwdoor_c();\n}\n",
         "/* hostgen HOST_COPY_PARK: port/unmatched/ShutterBob_Spawn_hostcopy.cpp\n"
         "   defines this factory under a standing ruling with a measured fault\n"
         "   behind it, so a second definition here is a duplicate symbol.\n"
         "   Parked, not deleted. See the HOST_COPY_PARK table in\n"
         "   tools/hostgen.py. */\n"
         "#if 0  /* hostgen HOST_COPY_PARK: daObjBSwdoor_c_classInit */\n"
         'extern "C" daObjBSwdoor_c *daObjBSwdoor_c_classInit()\n'
         "{\n    return new daObjBSwdoor_c();\n}\n"
         "#endif  /* hostgen HOST_COPY_PARK: daObjBSwdoor_c_classInit */\n"),
    ],
    # dScMgTrampoline2_c_classInit, ov006 0x0212471c, host-copied by
    # port/unmatched/MgTrampolineTerror_Factory.cpp, which is the bluntest of
    # the set: that file is this translation unit with SIX ARGUMENT PAIRS put
    # back in the ROM's order and nothing else changed, and it pastes the fault
    # the wrong order shipped, a call through a null pointer in
    # ModelBase::SetFile before frame 0 of scene 385. The profile table at the
    # top of this file still names the factory, and that reference binds to the
    # host copy's definition through the declaration above it.
    "d_s_mg_trampoline2": [
        ('extern "C" void *dScMgTrampoline2_c_classInit(void)\n{\n',
         "/* hostgen HOST_COPY_PARK: port/unmatched/MgTrampolineTerror_Factory.cpp\n"
         "   defines this factory with six argument pairs put back in the ROM's\n"
         "   order, so a second definition here is both a duplicate symbol and\n"
         "   the wrong body. Parked, not deleted. See the HOST_COPY_PARK table\n"
         "   in tools/hostgen.py. */\n"
         "#if 0  /* hostgen HOST_COPY_PARK: dScMgTrampoline2_c_classInit */\n"
         'extern "C" void *dScMgTrampoline2_c_classInit(void)\n{\n'),
        ("        __cxa_vec_ctor(scene + 0x7ad0, 5, 0x24,\n"
         "                      (void *)func_ov006_021225a8,\n"
         "                      (void *)func_ov006_02120938);\n"
         "    }\n    return scene;\n}\n",
         "        __cxa_vec_ctor(scene + 0x7ad0, 5, 0x24,\n"
         "                      (void *)func_ov006_021225a8,\n"
         "                      (void *)func_ov006_02120938);\n"
         "    }\n    return scene;\n}\n"
         "#endif  /* hostgen HOST_COPY_PARK: dScMgTrampoline2_c_classInit */\n"),
    ],
    # dScMgCup_c_classInit, ov006 0x020e0574, host-copied by
    # port/unmatched/MgCup_Factory.cpp. The ruling is the base constructor call
    # spelled _ZN11dScMgBase_cC2Ev() with no argument, one of exactly two such
    # calls in the ROM, relying on r0 already holding the object; the host copy
    # passes the object explicitly so the callee does not store three vtable
    # words through an uninitialised stack slot. This translation unit is
    # already substituted for its UNINIT_LOCAL row, so the park is the only
    # thing added for it here.
    "dScMgCup_c": [
        ('extern "C" void *dScMgCup_c_classInit()\n{\n',
         "/* hostgen HOST_COPY_PARK: port/unmatched/MgCup_Factory.cpp defines\n"
         "   this factory under a standing PORT_HOST_ABI ruling (the base\n"
         "   constructor call with no argument), so a second definition here is\n"
         "   a duplicate symbol. Parked, not deleted. See the HOST_COPY_PARK\n"
         "   table in tools/hostgen.py. */\n"
         "#if 0  /* hostgen HOST_COPY_PARK: dScMgCup_c_classInit */\n"
         'extern "C" void *dScMgCup_c_classInit()\n{\n'),
        ("        __cxa_vec_ctor(scene + 0x53e8, 3, 8,\n"
         "                      (void *)func_0203d738,\n"
         "                      (void *)NullDestructor_0203d47c);\n"
         "    }\n    return scene;\n}\n",
         "        __cxa_vec_ctor(scene + 0x53e8, 3, 8,\n"
         "                      (void *)func_0203d738,\n"
         "                      (void *)NullDestructor_0203d47c);\n"
         "    }\n    return scene;\n}\n"
         "#endif  /* hostgen HOST_COPY_PARK: dScMgCup_c_classInit */\n"),
    ],
}


def host_copy_park(text, sym):
    """Bracket a body port/unmatched/ host-copies under a standing ruling."""
    return apply_patches(text, sym, HOST_COPY_PARK, "HOST_COPY_PARK")


# ---- A DECORATED ROM DATUM A FILE-SCOPE DECLARATION CANNOT REACH -----------
#
# Run link100, lane HOSTGEN4. DATA_C_LINKAGE below is the right fix whenever one
# file-scope declaration can govern the whole translation unit. Sometimes it
# cannot: src/actors/daMky_c.cpp declares data_ov030_02115d18 five different
# ways in five different C++ MEMBERS -- `int []`, `void *[]`, `void *` and two
# local two-word structs -- and those spellings only coexist because each lives
# in its own function body. One file-scope declaration makes four of them a
# C2040, so there is no single type to give C linkage to.
#
# The port's own answer to a name that means one thing under two spellings is
# the /alternatename bridge, and it is already used for exactly this shape:
# hal/actor_classes.cpp:870 carries
# `/alternatename:?data_ov002_0210d9a8@@3DA=_data_ov002_0210d9a8`, a decorated
# LHS bridged to the C name the mount emits. The directive is a NAME bridge and
# nothing else, which is why it is admissible here and not for a receiver-shape
# mismatch: a datum has no calling convention.
#
# THESE ARE DATA ROWS ONLY, deliberately. The same trick would close function
# rows whose decorated twin is defined, and this lane measured eight of those in
# this TU and did NOT write them: port/faces_sync.txt is another lane's file and
# facegen derives exactly those rows, so an alias here would be DEFEATED the day
# a face defines its LHS and alternatename_guard would refuse the build. The
# eight are named in the lane's handoff instead. facegen emits no data rows, so
# a data alias cannot collide with it.
ALTNAME = {
    "daMky_c": [
        # The five members that declare a ROM datum at block scope inside a C++
        # member rather than inside the TU's own extern "C" region. Each RHS is
        # defined in this link: romdata.c for 02082214, ov030_syms.c for the
        # rest.
        ("?data_02082214@@3PAFA", "_data_02082214"),
        ("?data_ov030_02115ce0@@3PADA", "_data_ov030_02115ce0"),
        ("?data_ov030_02115ce0@@3PAHA", "_data_ov030_02115ce0"),
        ("?data_ov030_02115d08@@3PAPAXA", "_data_ov030_02115d08"),
        ("?data_ov030_02115d08@@3PAXA", "_data_ov030_02115d08"),
        ("?data_ov030_02115d18@@3UdaMky_G@@A", "_data_ov030_02115d18"),
        ("?data_ov030_02115d18@@3UdaMky_S@@A", "_data_ov030_02115d18"),
    ],
    # daPkn_c, added with its seat. Seven ROM data references declared
    # inside C++ members, where a linkage-specification is not allowed, so
    # each reaches the linker decorated while ov002_syms.c and ov084_syms.c
    # emit the C name. Measured against the whole link before the rows were
    # written: every RHS below is defined.
    "daPkn_c": [
        ("?data_ov002_0210da38@@3UPknSharedFile@@A", "_data_ov002_0210da38"),
        ("?data_ov084_021302f4@@3PAPAUSharedFilePtr@@A", "_data_ov084_021302f4"),
        ("?data_ov084_02130df4@@3UPknSharedFile@@A", "_data_ov084_02130df4"),
        ("?data_ov084_02130dfc@@3UPknSharedFile@@A", "_data_ov084_02130dfc"),
        ("?data_ov084_02130e0c@@3UPknSharedFile@@A", "_data_ov084_02130e0c"),
        ("?data_ov084_02130e14@@3UPknSharedFile@@A", "_data_ov084_02130e14"),
        ("?data_ov084_02130e24@@3UPknSharedFile@@A", "_data_ov084_02130e24"),
    ],
}


def altname(text, sym):
    """Bridge a decorated ROM datum to the C name the mount emits."""
    rows = ALTNAME.get(sym)
    if not rows:
        return text, 0
    block = ("/* hostgen ALTNAME: each of these is one ROM datum under two\n"
             "   spellings. The declaration that produces the LHS is inside a\n"
             "   C++ member, where a linkage-specification is not allowed and a\n"
             "   file-scope declaration would collide with the other members'\n"
             "   spellings of the same name, so the bridge is a linker\n"
             "   directive. Every RHS is defined in this link. */\n"
             + "".join('#pragma comment(linker, "/alternatename:%s=%s")\n'
                       % (lhs, rhs) for lhs, rhs in rows)
             + "\n")
    return block + text, len(rows)


# ---- A ROM DATA EXTERN THAT MSVC DECORATED ---------------------------------
#
# PORT_HOST_ABI, run link100, lane HOSTGEN4: the same defect as ZTV_C_LINKAGE
# above, one level out. A recovered translation unit declares a ROM DATUM with
# a plain C++ `extern <T> data_<hex>;` -- at file scope above the TU's own
# `extern "C"` region, or at block scope inside a member that has no such
# region -- so MSVC asks the linker for the decorated name while every mount in
# the port emits that datum with C linkage:
#
#   dMgJump3DMario_c.cpp.obj : error LNK2001: unresolved external symbol
#       "struct PmfRecord data_ov006_0213b020" (?data_ov006_0213b020@@3UPmfRecord@@A)
#
# and ov006_syms.c defines `_data_ov006_0213b020` two feet away. mwccarm did
# not care: the ROM carries one spelling and the decomp author's placement was
# a matched-source detail, not a linkage decision.
#
# EXTERN_C_DATA above fixes the file-scope case by MOVING the TU's own
# `extern "C" {` opener up over the run. That does not reach the block-scope
# case at all -- a linkage-specification is namespace-scope only, and MSVC says
# so outright (C2598: "linkage specification must be at global scope"), which
# this lane measured before writing the rule. So this rule is purely ADDITIVE:
# it inserts a file-scope `extern "C"` declaration of the same datum, with the
# TU's own spelling of the type, after a named anchor, and the TU's later
# declaration -- file scope or block scope -- inherits the linkage already
# specified rather than fighting it. That is the same C++ rule ZTV_C_LINKAGE
# rests on, and it was measured on this toolchain the same way: cl /c on
#
#     struct Pair;
#     extern "C" { extern Pair data_test_2; }
#     struct Pair { int a, b; };
#     int g() { extern Pair data_test_2; return data_test_2.a; }
#
# emits ONE undefined symbol, `_data_test_2`, with no diagnostic.
#
# The anchor is named per row so the inserted block lands where the type is
# already known: an incomplete class type is enough for an extern declaration,
# but the TU's own typedefs and includes are not, so the rows anchor after the
# include or after the struct the declarations use. A row whose anchor or whose
# datum has left the translation unit is a HARD ERROR, never a silent drop:
# without the insertion the reference comes out decorated again and there is no
# /alternatename that can bridge a decoration.
# THE ADDRESS OF A ROM FUNCTION, TAKEN AS DATA. dBgW's two collision-callback
# bodies are never CALLED by these translation units: func_020393d4 is handed
# their address and the mesh collider calls them later, so the decomp declares
# each one as a DATUM under its flat Itanium name. Fourteen translation units
# do it and they do not agree on the type -- `int`, `char`, `void *`, `int[]`,
# each one the width its own author happened to write -- so MSVC emits FOUR
# decorated spellings of UpdatePosAndAngs and four of UpdatePosWithTransform
# against the one ROM body. Under C linkage all seven collapse onto the flat
# name __ZN4dBgW16UpdatePosAndAngs... (and its Transform twin), which is the
# name the ROM itself carries and the one port/faces_sync.txt already has on
# the wall as an ordinary face row. Nothing new arrives; seven wrong spellings
# of one missing thing become the one right spelling.
_DBGW_ANGS = ("_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPi"
              "R7Vector3P10Vector3_16S8_")
_DBGW_XFRM = ("_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPi"
              "R7Vector3P10Vector3_16S8_")


def _c_linkage_line(line, name):
    """One file-scope declaration, re-declared with C linkage just above it."""
    return (line + "\n", [(line, name)], "before")


DATA_C_LINKAGE = {
    # Nine +0x3c state installs and two argument pairs, every one declared at
    # BLOCK SCOPE inside a member that carries no extern "C" region of its own.
    # The types are the TU's own two-word shadows at lines 233 and 234, so the
    # anchor is the second of them.
    "dMgJump3DMario_c": ("struct Pair { int a, b; };\n", [
        ("extern PmfRecord data_ov006_0213b020;", "data_ov006_0213b020"),
        ("extern PmfRecord data_ov006_0213b028;", "data_ov006_0213b028"),
        ("extern PmfRecord data_ov006_0213b030;", "data_ov006_0213b030"),
        ("extern PmfRecord data_ov006_0213b038;", "data_ov006_0213b038"),
        ("extern PmfRecord data_ov006_0213b060;", "data_ov006_0213b060"),
        ("extern Pair data_ov006_0213b058;", "data_ov006_0213b058"),
        ("extern Pair data_ov006_0213b068;", "data_ov006_0213b068"),
        ("extern Pair data_ov006_0213b070;", "data_ov006_0213b070"),
        ("extern Pair data_ov006_0213b078;", "data_ov006_0213b078"),
    ]),
    # File scope, above this TU's own extern "C" region: the two file views the
    # step mounts and the collision-parameter block it hands dBgW_Kc.
    "_ZN9TowerStep13InitResourcesEv": ('#include "dBgCh_Gnd.h"\n', [
        ("extern SharedFilePtr data_ov015_02114a64;", "data_ov015_02114a64"),
        ("extern SharedFilePtr data_ov015_02114a5c;", "data_ov015_02114a5c"),
        ("extern CLPS_Block data_ov015_02113594;", "data_ov015_02113594"),
    ]),
    # File scope: the player array the gate indexes and the rotation vector it
    # passes to Vec3_RotateYAndTranslate.
    "d_a_star_gate": ("extern u8 data_0209f250;\n", [
        ("extern Player *data_0209f394[];", "data_0209f394"),
        ("extern char data_020a0ebc[];", "data_020a0ebc"),
    ]),
    "_ZN10LavaSeesaw16CleanupResourcesEv": ('#include "decl_common.h"\n', [
        ("extern int data_ov022_021145a0[];", "data_ov022_021145a0"),
    ]),
    # The head of the live-shadow list, hosted in hal/actor_vtables.cpp.
    "_ZN11ShadowModelD1Ev": ('#include "ShadowModel.h"\n', [
        ("extern ShadowModel *data_0209cef4;", "data_0209cef4"),
    ]),
    # The dBgW collision-callback addresses, one row per translation unit and
    # per spelling. See the block above the table.
    "func_ov002_020b5e58":
        _c_linkage_line("extern char %s;" % _DBGW_ANGS, _DBGW_ANGS),
    "_ZN13TTC_MovingBar13InitResourcesEv":
        _c_linkage_line("extern int %s;" % _DBGW_ANGS, _DBGW_ANGS),
    "_ZN16RotatingCogSmall13InitResourcesEv":
        _c_linkage_line("extern int %s;" % _DBGW_ANGS, _DBGW_ANGS),
    "func_ov002_020b676c":
        _c_linkage_line("extern int %s;" % _DBGW_ANGS, _DBGW_ANGS),
    "_ZN25RotatingUpDownPlatformUtm13InitResourcesEv":
        _c_linkage_line("extern void *%s;" % _DBGW_ANGS, _DBGW_ANGS),
    "func_ov091_02133254":
        _c_linkage_line("extern void *%s;" % _DBGW_ANGS, _DBGW_ANGS),
    "_ZN6ToxBox13InitResourcesEv":
        _c_linkage_line("extern char %s;" % _DBGW_XFRM, _DBGW_XFRM),
    "_ZN11PyramidLift13InitResourcesEv":
        _c_linkage_line("extern int %s;" % _DBGW_XFRM, _DBGW_XFRM),
    "_ZN11PyramidStep13InitResourcesEv":
        _c_linkage_line("extern int %s;" % _DBGW_XFRM, _DBGW_XFRM),
    "d_a_obj_wc_obj04":
        _c_linkage_line("extern int %s;" % _DBGW_XFRM, _DBGW_XFRM),
    "_ZN8CccArena13InitResourcesEv":
        _c_linkage_line("extern int %s[];" % _DBGW_XFRM, _DBGW_XFRM),
    "_ZN9SeesawBob13InitResourcesEv":
        _c_linkage_line("extern int %s[];" % _DBGW_XFRM, _DBGW_XFRM),
    "_ZN6ShipUp13InitResourcesEv":
        _c_linkage_line("extern void* %s;" % _DBGW_XFRM, _DBGW_XFRM),
}


def data_c_linkage(text, sym):
    """Give a TU's ROM data externs C linkage, ahead of its own declaration."""
    row = DATA_C_LINKAGE.get(sym)
    if not row:
        return text, 0
    if len(row) == 3:
        anchor, decls, where = row
    else:
        anchor, decls = row
        where = "after"
    if anchor not in text:
        sys.exit("hostgen: %s: DATA_C_LINKAGE's anchor is not in the "
                 "translation unit any more:\n  %s\nRe-read the TU and move "
                 "the anchor -- do NOT drop the row. Without it every datum "
                 "below comes out decorated and no /alternatename can bridge "
                 "a decoration." % (sym, anchor.rstrip("\n")))
    for decl, name in decls:
        if name not in text:
            sys.exit("hostgen: %s: DATA_C_LINKAGE names %s, which is not in "
                     "the translation unit any more. Re-read the TU rather "
                     "than dropping the row." % (sym, name))
    block = ("/* hostgen DATA_C_LINKAGE: this TU declares these ROM data "
             "symbols\n"
             "   without a linkage-specification, so MSVC asks for the "
             "decorated\n"
             "   name while every mount emits the C one. A redeclaration "
             "keeps the\n"
             "   linkage already specified, so the TU's own declaration "
             "below\n"
             "   inherits this one. */\n"
             'extern "C" {\n'
             + "".join("    %s\n" % d for d, _ in decls)
             + "}\n")
    if where == "before":
        return text.replace(anchor, block + anchor, 1), len(decls)
    return text.replace(anchor, anchor + block, 1), len(decls)


# ---- THE VTABLE ADDRESS-POINT BIAS ------------------------------------------
#
# PORT_HOST_ABI: mwcc's own vtable symbol denotes the OBJECT START and the
# Itanium address point is two words past it, so a translation unit that EMITS
# its class's vtable writes the vptr as `&_ZTV<X>[2]` or `_ZTV<X> + 2`.
# src/actors/daObjLava_c.cpp states the rule in its own header: "The addend-0
# spelling is correct only for a TU that IMPORTS its vtable, where the linker's
# symbol already denotes the address point; this TU EMITS the vtable, so mwcc's
# own symbol denotes the object start and the +8 bias to the address point has
# to be written out."
#
# ON THE PORT THE SAME NAME ALREADY MEANS THE ADDRESS POINT. Every vtable a
# class dispatches through here is a host array in port/hal/actor_classes_*.cpp
# (or a mounted ROM image) whose slot 0 IS the first virtual: there are no two
# header words for the bias to skip. So the +8 spelling puts the object's vptr
# two slots high, and slot 0 -- InitResources, the first call on the spawn path
# -- enters slot 2 instead, which is a different function with a different
# arity. Nothing sees it: the byte gate compares the matched object, the linker
# resolves the name and applies the addend without comment, and linkage.py
# counts the row.
#
# MEASURED, run link100 lane VPTR (out/VPTR/report.md, sweep3.txt): over the 461
# objects on walk_window's link line that mention a _ZTV name, 340 (file, table)
# pairs store addend 0 and 70 store addend 8, with no other value. The port's
# own live convention was read off a running binary -- vptr equals the table
# symbol plus zero -- and the ROM's own factory pool word points at table+0 for
# every one of the five classes that lane disassembled. 61 of the 70 would link
# clean and dispatch silently wrong; the other 9 name the table with an
# MSVC-decorated extern and are on the unresolved wall already.
#
# So the rewrite is unconditional and pattern-keyed rather than a per-site
# table: there is no TU in this tree for which the +8 spelling is right on the
# host, and a class main promotes to a key-function TU tomorrow gets the same
# treatment without anyone writing a row for it. Both spellings the decomp uses
# are covered; the four-word form `(int)&_ZTV<X>[2]` is the first with a cast in
# front of it and needs nothing of its own.
#
# THIS RULE ONLY REACHES A TU HOSTGEN EMITS. The port compiles most of src/
# straight, so the net for everything else is port/tools/vptr_addend_guard.py,
# which reads the applied value of every DIR32 relocation against a _ZTV symbol
# out of the built objects and refuses a value it has not been told about.
VPTR_ADDRESS_POINT = re.compile(
    r"&\s*(_ZTV\w+)\s*\[\s*2\s*\]"
    r"|(?<![\w.>])(_ZTV\w+)\s*\+\s*2(?![\w.])")


def vptr_address_point(text):
    """Drop the Itanium +8 address-point bias from a vptr store."""
    return VPTR_ADDRESS_POINT.subn(lambda m: m.group(1) or m.group(2), text)


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


def redecl_conflict_patch(text, sym):
    """Delete the block-scope redeclarations that disagree with the TU's own
    file-scope spelling, and hoist the function-local types the ROM data
    declarations need."""
    return apply_patches(text, sym, REDECL_CONFLICT, "REDECL_CONFLICT",
                         REDECL_CONFLICT_DECL.get(sym, ""))


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


QUIET_VPTR = False


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
    text, _ = pmf_seam_patch(text, sym)
    text, _ = uninit_local_patch(text, sym)
    text, _ = member_redecl_patch(text, sym)
    text, _ = redecl_conflict_patch(text, sym)
    text, _ = ledger_park(text, sym)
    text, _ = host_copy_park(text, sym)
    text, _ = extern_c_data_patch(text, sym)
    text, _ = call_state_fn_patch(text, sym)
    text, _ = arg_width_patch(text, sym)
    text, _ = callee_seam_patch(text, sym)
    text, _ = reg_ride_arg_patch(text, sym)
    text, nztv = ztv_c_linkage(text, sym)
    if nztv and not QUIET_VPTR:
        print("  %s: %d vtable extern(s) given C linkage" % (sym, nztv))
    text, ndata = data_c_linkage(text, sym)
    if ndata and not QUIET_VPTR:
        print("  %s: %d ROM data extern(s) given C linkage" % (sym, ndata))
    text, nalt = altname(text, sym)
    if nalt and not QUIET_VPTR:
        print("  %s: %d decorated ROM datum(s) bridged by /alternatename"
              % (sym, nalt))
    text, nvptr = vptr_address_point(text)
    if nvptr and not QUIET_VPTR:
        print("  %s: %d vtable address-point bias(es) dropped" % (sym, nvptr))
    pmf_seam_residue(text, sym, src_path.suffix)
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

    global QUIET_VPTR
    QUIET_VPTR = args.all
    if args.all:
        targets = [p for p in src.rglob("*") if p.suffix in (".c", ".cpp")]
    else:
        targets = []
        for name in args.symbols:
            # A name may be given as a repo-relative path (src/minigames/X.cpp)
            # or as a bare stem. The bare-stem form used to assume src/ was
            # flat, which stopped being true when the decomp began merging
            # one-function sources into translation units and filing them under
            # src/<area>/: the flat probe missed them and the tool exited
            # "not found" on a file that is right there. Try the path first,
            # then the flat convention, then one recursive scan by stem.
            cand = (src.parent / name) if "/" in name or "\\" in name else None
            if cand is not None and cand.is_file():
                targets.append(cand)
                continue
            hit = next((p for ext in (".c", ".cpp")
                        if (p := src / f"{name}{ext}").exists()), None)
            if hit is None:
                found = [p for p in src.rglob(f"{name}.c")] +                         [p for p in src.rglob(f"{name}.cpp")]
                if len(found) == 1:
                    hit = found[0]
                elif len(found) > 1:
                    sys.exit(f"ambiguous in decomp src/: {name} -> "
                             + ", ".join(str(x) for x in found))
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
