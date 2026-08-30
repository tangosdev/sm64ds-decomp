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
# run rel0215 wave 3 (lane w3-e) adds the SECOND entry, and its caveat is
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
    text, _ = asm_excision_patch(text, sym)
    text, _ = ds_div_patch(text, sym)
    text, _ = mmio_extern_patch(text, sym)
    text, _ = falls_off_return_patch(text, sym)
    text, _ = virtual_call_patch(text, sym)
    text, _ = member_redecl_patch(text, sym)
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
