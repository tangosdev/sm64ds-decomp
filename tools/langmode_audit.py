#!/usr/bin/env python3
"""Audit the C/C++ language-mode gap: C++ symbols that still build in C mode.

Issue #821 ("this is 100% fakematch") is largely about *description*, not bytes: the
tree defines C++ symbols in files that compile as C, with the mangled name spelled by
hand and `this` typed as `int*`. The bytes are right -- every enrolled function
reproduces the ROM -- but the source claims something false. See
`notes/plan-cpp-language-mode.md` for the plan this tool instruments, and
`notes/runbook-type-reconstruction.md` for how a migration is actually done.

Nothing here compiles anything. It is pure static analysis over git-tracked files, so
it runs on a fresh clone with no compilers and no extracted ROM -- which is the point:
the gap must be measurable before any of it can be fixed, and measurable in CI so it
cannot silently grow.

Four families are counted:

  1. LANGUAGE MODE   mangled-symbol files that are not migrated. Two populations, and
                     conflating them flatters the tree: a `.c` file defining `_ZN...`,
                     AND a `.cpp` file that merely renamed the extension while still
                     hand-spelling its own mangled symbol in the definition. The second
                     is what #821 meant by "your cute little //cpp changes nothing" --
                     a real migration lets the compiler mangle the name for you. Split
                     by symbol kind, because feasibility differs sharply: destructors
                     are proven, constructors are not.
  2. SHADOW DECLS    files that re-declare a struct locally instead of including the
                     real header. Migrating a dtor the quick way *creates* these, which
                     is how the tree accumulated hundreds. Counted so that quick wins
                     show up as the debt they are.
  3. CODEGEN HACKS   inline `asm`, and pointer-laundering macros that exist only to
                     force address materialization.
  4. EXCLUSIONS      symbols that cannot be migrated and are therefore not backlog:
                     any method taking a class BY VALUE. mwccarm homes r0-r3 to the
                     stack for those (+0x14) on every one of the 25 sweep versions at
                     every optimization level. See runbook section 7.

Usage:
    python tools/langmode_audit.py                    # human summary
    python tools/langmode_audit.py --by-class         # per-class backlog table
    python tools/langmode_audit.py --list c-mangled   # file list, for picking work
    python tools/langmode_audit.py --list cpp-handspelled
    python tools/langmode_audit.py --list excluded    # the un-migratable ones

    # CI ratchet: counts may fall, never rise. It holds only the metrics --check reads,
    # so the banked file stays small and readable.
    #
    # The baseline lives on the `chaos-data` BRANCH, not in this tree -- it is a generated
    # counter with no meaningful hand-merge, and re-banking it by hand in every migration
    # PR made each merge to main move it under every other PR in flight. To read it:
    #     git show origin/chaos-data:langmode-baseline.json > /tmp/base.json
    #     python tools/langmode_audit.py --check /tmp/base.json
    #
    # Do NOT re-bank a slice that LOWERS counts: update-chaos-data.yml banks it on merge.
    # Only a PR that RAISES a count banks its own, as a root langmode-baseline.json that
    # langmode-ratchet.yml prefers over the branch copy -- see the RATCHET block below and
    # notes/plan-cpp-language-mode.md (Phase 0). Delete that root file once the rise lands.
    python tools/langmode_audit.py --json langmode-baseline.json    # only to book a RISE
    python tools/langmode_audit.py --check <banked>.json            # exit 1 if worse
    python tools/langmode_audit.py --json out.json --full           # + per-class, lists
"""
import argparse
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import asm_policy  # noqa: E402
import delaunder  # noqa: E402
import demangle as D  # noqa: E402
import srcpath as SP  # noqa: E402

# A mangled basename, with or without the .c/.cpp extension.
MANGLED = re.compile(r"^_Z[A-Za-z0-9_]+$")

# Scalar and builtin spellings demangle.py can emit. Anything else that is not a
# pointer or reference is a class passed BY VALUE -- the runbook section 7 dead end.
SCALARS = {
    "void", "bool", "char", "signed char", "unsigned char", "short",
    "unsigned short", "int", "unsigned int", "long", "unsigned long",
    "long long", "unsigned long long", "float", "double", "long double",
    "wchar_t", "...",
}

def hand_spells_own_symbol(path, stem):
    """True if the file DEFINES its own mangled symbol by hand.

    A migrated file writes `void FaderColor::AdvanceFade()` and lets mwccarm mangle it.
    An unmigrated one writes `int _ZN10FaderColorD1Ev(int *self) {`, which is the same
    C-shaped lie regardless of whether the extension says .c or .cpp.

    What separates a definition from a declaration is the trailing `{`, NOT the word
    `extern`: `extern "C" void _ZN5ActorC1Ev(Actor *self) {` is a definition with C
    linkage, and an earlier version of this function wrongly discarded those, reporting
    five constructors as migrated when none are. Declarations end in `;` and so never
    match the pattern in the first place.

    A CALL is not a definition, and the trailing-`{` rule alone cannot tell them apart:
    in `if (_ZN6Player12FinishedAnimEv(self)) {` the `[^;{]*` swallows the inner `)`,
    the outer `)` closes the group and the if-body's `{` completes the match. This never
    fired while every file owned exactly one symbol -- a call to some OTHER class's
    member is never asked about -- so it stayed invisible until a TU promotion put many
    members and their mangled cross-calls in one file. Measured on the 301-member
    ov002/Player TU: 8 members written as real `Player::Method()` definitions were
    reported hand-spelled purely because the file also CALLS them through the mangled
    name, which raised the ratchet by 8 with no source regression behind it.

    `DEF_LINE_PREFIX` is the fix, and it already existed: `ANY_MANGLED_DEF` has carried
    it since 537 files were mis-swept in exactly this way. Applying it here changes
    those 8 classifications and nothing else across every `.cpp` in `src/` -- a pointer
    return type like `int *_ZN14daObjWc_Mizu_cD0Ev(int *t) {` still counts, because `*`
    is a declarator token and `(` is not.
    """
    try:
        t = (REPO / path).read_text(errors="ignore")
    except OSError:
        return False
    for m in re.finditer(re.escape(stem) + r"\s*\([^;{]*\)\s*\{", t, re.S):
        line_start = t.rfind("\n", 0, m.start()) + 1
        line = t[line_start:m.start()]
        if line.lstrip().startswith(("//", "*", "/*")):
            continue
        if not DEF_LINE_PREFIX.match(line):
            continue        # a call inside an expression, not a definition
        return True
    return False


def is_nonmatching(path):
    """`// NONMATCHING` marks a draft whose bytes do not reproduce the ROM.

    The runbook treats these as non-authoritative -- their codegen and often their
    signatures are guesses -- so they must not be counted as proof that a pattern works.
    """
    try:
        t = (REPO / path).read_text(errors="ignore")
    except OSError:
        return False
    return asm_policy.has_draft_banner(t)


# A local struct/class body, e.g. `struct Actor { char pad[0xd0]; ... };`
LOCAL_BODY = re.compile(r"^\s*(?:struct|class)\s+\w+\s*(?::[^;{]*)?\{", re.M)
PROJECT_INCLUDE = re.compile(r'^\s*#\s*include\s*"', re.M)
PAD_LAYOUT = re.compile(r"char\s+pad\[0x", re.M)
INLINE_ASM = re.compile(r"__asm|(?<![A-Za-z0-9_])asm\s*[({]")
# (The launder metric used to live here as a regex over prose. It now comes from
#  delaunder.find_sites, which reads code. See the call site for why.)


def tracked_sources():
    """Every git-tracked C/C++ file under src/, as repo-relative posix paths."""
    out = subprocess.run(
        ["git", "-C", str(REPO), "ls-files", "src"],
        capture_output=True, text=True, check=True,
    ).stdout.split()
    return [p for p in out if p.endswith((".c", ".cpp"))]


# Any mangled symbol DEFINED in the body -- same shape as hand_spells_own_symbol,
# but not anchored to the file's own stem. It used to say that costs "two guards
# that function does not need"; the DEF_LINE_PREFIX one it DOES need, and a
# 301-member TU promotion is what finally showed it.
#
# The lookbehind is not decoration: without it `_Z` matches inside a longer
# identifier, and `void START_INTRO_MINIMAP_ZOOM(char* c){` was read as defining a
# symbol called `_ZOOM`.
ANY_MANGLED_DEF = re.compile(r"(?<![A-Za-z0-9_])(_Z[A-Za-z0-9_]+)\s*\([^;{]*\)\s*\{", re.S)

# What may precede a definition on its own line: a return type, storage class,
# pointer/reference tokens, `extern "C"`. Anything else -- an open paren above all
# -- means this is a CALL, not a definition. `if (_ZN4Heap6IntactEv(x)) {` matches
# the shape exactly and is not a definition; 537 files were mis-swept before this
# guard, nearly all of them calls inside an `if`.
DEF_LINE_PREFIX = re.compile(r'^[\w\s\*&:"]*$')


def _brace_depth(text, pos):
    """Nesting depth at `pos`, ignoring braces inside strings, chars and comments.

    This is the guard that actually decides definition vs call. A definition sits at
    depth 0; a call sits inside some function body. The line-prefix check alone is
    not enough, because a call continued onto its own line --

        if (a &&
            _ZN9Animation8FinishedEv(c + 0x3b0)) {

    -- presents a prefix of pure whitespace and passes it. 15 files got through on
    exactly that shape.

    `extern "C" { ... }` and `namespace X { ... }` do NOT count as nesting. Most of
    this tree's hand-spelled definitions sit inside an `extern "C"` block --
    `src/_ZN5ActorD2Ev.cpp` is the type -- and treating that brace as depth would
    leave exactly those files reachable by the rename game this guard exists to stop.
    """
    depth = 0
    i = 0
    n = min(pos, len(text))
    linkage_depths = set()
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n:
            nxt = text[i + 1]
            if nxt == "/":
                j = text.find("\n", i)
                if j < 0:
                    break
                i = j
                continue
            if nxt == "*":
                end = text.find("*/", i + 2)
                i = n if end < 0 else end + 2
                continue
        elif ch in "\"'":
            quote, i = ch, i + 1
            while i < n:
                if text[i] == "\\":
                    i += 2
                    continue
                if text[i] == quote:
                    break
                i += 1
        elif ch == "{":
            head = text[max(0, i - 80):i]
            depth += 1
            if re.search(r'(?:extern\s*"C(?:\+\+)?"|namespace(?:\s+\w+)?)\s*$', head):
                linkage_depths.add(depth)
        elif ch == "}":
            linkage_depths.discard(depth)
            depth -= 1
        i += 1
    return depth - len(linkage_depths)


def defined_mangled_symbols(path):
    """Every mangled symbol this file defines by hand, regardless of filename.

    The counters used to key entirely on the basename, which made the headline
    metric trivially gameable by `git mv` -- and not only in theory:

      * renaming `_ZN5FaderD1Ev.c` to `func_0201786c.c`, content untouched, LOWERED
        `unmigrated_total`. The hand-spelled symbol was still there; the metric
        improved because the tool stopped looking at the file.
      * renaming every mangled-stem file to `func_fake_N` drove the headline to
        "0 of 1258 (0.0%)" without editing one byte of source.

    That inverts the incentive the ratchet exists to create: it rewarded making the
    symbol table LESS true. Reading the body closes it -- a file that defines a
    mangled symbol is a mangled-symbol file wherever it lives.
    """
    try:
        t = (REPO / path).read_text(errors="ignore")
    except OSError:
        return []
    found = []
    for m in ANY_MANGLED_DEF.finditer(t):
        line_start = t.rfind("\n", 0, m.start()) + 1
        line = t[line_start:m.start()]
        if line.lstrip().startswith(("//", "*", "/*")):
            continue
        if not DEF_LINE_PREFIX.match(line):
            continue
        if _brace_depth(t, m.start()) != 0:
            continue
        if m.group(1) not in found:
            found.append(m.group(1))
    return found


def defined_mangled_symbol(path):
    """Backward-compatible first hand-spelled mangled definition, if any."""
    found = defined_mangled_symbols(path)
    return found[0] if found else None


# `extern int _ZTV6ToxBox[];`, `extern u32 _ZTV11ShadowModel[];`, `extern void
# *_ZTV6Eyerok[];`, `extern int _ZTV11ChiefChilly;` -- the declaration that lets a
# non-polymorphic struct store a vptr by hand.
#
# BOTH SPELLINGS COUNT, and requiring either one alone undercounts by tens of files.
# The type is `int`, `u32`, `void *` or `unsigned int` depending on who wrote it, and
# the array brackets are optional because the scalar form is taken by address instead.
# What actually pins the idiom is `extern` + a `_ZTV` name + a declarator terminator,
# so that is all this matches.
EXTERN_VTABLE = re.compile(
    r"\bextern\s+[A-Za-z_][\w\s*]*?\b(_ZTV[0-9A-Za-z_]+)\s*(?:\[\s*\])?\s*;")


def _extern_vtable_classes(text):
    """The `_ZTV` symbols this file declares as an extern array, from CODE only.

    Masked through `delaunder.code_mask` for the reason the launder metric is: the
    idiom gets explained in comments, and a metric a reword can move is a metric that
    rewards rewording. Measured when this landed: 397 files, 183 classes, and masking
    suppresses exactly 0 of each -- nothing is being scored from prose today. The mask
    is here so that stays true, not because it is currently load-bearing.
    """
    keep = delaunder.code_mask(text)
    return {m.group(1) for m in EXTERN_VTABLE.finditer(text)
            if all(keep[i] for i in range(m.start(), m.end()))}


def classify(path):
    """Return (kind, info, symbol) for a mangled-symbol file, or (None, None, None).

    `kind` is the ctor/dtor variant (C1/C2/D0/D1/D2) or 'method'. `symbol` is the
    name the classification is based on: normally the file's stem, but for a file
    whose stem is not mangled it is the symbol found in the body -- see
    `defined_mangled_symbol` for why that second path has to exist.
    """
    stem = path.rsplit("/", 1)[-1].rsplit(".", 1)[0]
    sym = stem if MANGLED.match(stem) else defined_mangled_symbol(path)
    if not sym:
        return None, None, None
    try:
        info = D.demangle(sym)
    except Exception:
        return None, None, None
    if not info or not info.get("qualified"):
        return None, None, None
    kind = info.get("variant") or "method"
    return kind, info, sym


def classifications(path):
    """Every evidenced C++ function owned by ``path``.

    Production enrollment is the inventory authority for a reconstructed TU whose
    filename is not any one member symbol.  Unenrolled drafts fall back to symbols
    hand-spelled in the body, then to the legacy mangled basename.
    """
    full = REPO / path
    symbols = [sym for sym in SP.symbols_for(full) if MANGLED.match(sym)]
    for sym in defined_mangled_symbols(path):
        if sym not in symbols:
            symbols.append(sym)
    stem = path.rsplit("/", 1)[-1].rsplit(".", 1)[0]
    if MANGLED.match(stem) and stem not in symbols:
        symbols.insert(0, stem)
    rows = []
    for sym in symbols:
        try:
            info = D.demangle(sym)
        except Exception:
            continue
        if not info or not info.get("qualified"):
            continue
        rows.append((info.get("variant") or "method", info, sym))
    return rows


def by_value_class_args(info):
    """Class-typed parameters passed by value -- the un-migratable ones."""
    out = []
    for a in info.get("args") or []:
        t = a.strip()
        if t.endswith("*") or t.endswith("&"):
            continue
        base = t.replace("const ", "").replace("volatile ", "").strip()
        if base and base not in SCALARS:
            out.append(t)
    return out


def audit():
    srcs = tracked_sources()
    r = {
        "totals": {"src_files": len(srcs)},
        "language_mode": {},
        "shadow_decls": {},
        "codegen_hacks": {},
        "exclusions": {},
    }

    c_ext = [p for p in srcs if p.endswith(".c")]
    cpp_ext = [p for p in srcs if p.endswith(".cpp")]
    r["totals"]["c_extension"] = len(c_ext)
    r["totals"]["cpp_extension"] = len(cpp_ext)

    c_mangled, cpp_mangled = [], []
    cpp_handspelled = []      # .cpp extension, but still defines the mangled name itself
    kinds = {}
    per_class = {}
    excluded = []
    nonmatching = []

    for p in srcs:
        owned = classifications(p)
        if not owned:
            continue
        stem = p.rsplit("/", 1)[-1].rsplit(".", 1)[0]
        is_c = p.endswith(".c")
        draft = is_nonmatching(p)
        for kind, info, sym in owned:
            identity = p if len(owned) == 1 else f"{p}#{sym}"
            (c_mangled if is_c else cpp_mangled).append(identity)

            # A real migration lets C++ emit the mangled name. The basename is no
            # longer evidence either way once a TU owns several functions.
            unmigrated = is_c or hand_spells_own_symbol(p, sym)
            if not is_c and unmigrated:
                cpp_handspelled.append(identity)
            if draft:
                nonmatching.append(identity)

            slot = kinds.setdefault(kind, {"unmigrated": 0, "migrated": 0, "draft": 0})
            if draft:
                slot["draft"] += 1
            if unmigrated:
                slot["unmigrated"] += 1
            elif not draft:
                slot["migrated"] += 1

            if unmigrated:
                byval = by_value_class_args(info)
                if byval:
                    excluded.append({"file": identity, "by_value": sorted(set(byval))})
                else:
                    cls = info.get("class") or "<free>"
                    pc = per_class.setdefault(cls, {"c_left": 0, "kinds": {}, "files": [],
                                                    "nested": False, "const_member": False})
                    pc["c_left"] += 1
                    pc["files"].append(identity)
                    pc["kinds"][kind] = pc["kinds"].get(kind, 0) + 1
                    if (info.get("qualified") or "").count("::") > 1:
                        pc["nested"] = True
                    if sym.startswith("_ZNK"):
                        pc["const_member"] = True

    # No include/<Class>.h AND no ctor/dtor => almost certainly an SDK *namespace*
    # (GX, CP15, IRQ...): no `this`, no vtable, no layout, no includers, so a migration
    # cannot shift anyone's offsets. A missing header alone does NOT mean that -- a
    # class like Chuckya has a D0/D1 and therefore a vtable whether a header exists or
    # not, so it carries full layout risk. Keep the two apart.
    #
    # THOSE TWO TESTS ARE NOT SUFFICIENT, and the way they fail is worth stating,
    # because this set is handed to contributors as "safe to migrate, zero blast
    # radius" and it was over-reported by a third.
    #
    # `class` from the demangler is the INNERMOST enclosing name.
    # _ZN8Particle12ClipCallback8OnUpdateERNS_6SystemEb keys on `ClipCallback', so
    # has_header looks for include/ClipCallback.h -- which does not exist and never
    # would, because the type is nested inside Particle -- and ClipCallback declares
    # no ctor or dtor of its own. Both tests pass and the answer is still wrong:
    # Particle::Callback is a polymorphic base, its SpawnParticles/OnUpdate pair sits
    # at arm9:0x0208f3b4/0x0208f3b8, and the pair repeats per derived class across
    # 0x0208f3a8..0x0208f464 for nine subclasses. So a qualified name with more than
    # one `::' is nested, and nesting means the enclosing scope has to be declared
    # whatever the layout turns out to be.
    #
    # A `_ZNK...' symbol is a CONST MEMBER function. It has a `this' by definition,
    # so it can never be layout-free no matter what its class looks like.
    #
    # What is deliberately NOT checked here: whether the function's address is stored
    # in a vtable. That needs the ROM, and this tool is scoped to run on a bare clone
    # with no compiler and no extracted ROM (plan phase 0). tools/check_layout_free.py
    # does that pass separately.
    CTOR_DTOR = {"C1", "C2", "C3", "D0", "D1", "D2"}
    for cls, pc in per_class.items():
        pc["has_header"] = (REPO / "include" / f"{cls}.h").is_file()
        pc["layout_free"] = (not pc["has_header"]
                             and not (set(pc["kinds"]) & CTOR_DTOR)
                             and not pc.get("nested")
                             and not pc.get("const_member"))

    r["language_mode"] = {
        "mangled_total": len(c_mangled) + len(cpp_mangled),
        "mangled_c": len(c_mangled),
        "mangled_cpp": len(cpp_mangled),
        "cpp_still_handspelled": len(cpp_handspelled),
        "unmigrated_total": len(c_mangled) + len(cpp_handspelled),
        "nonmatching_drafts": len(nonmatching),
        "by_kind": kinds,
    }
    r["_lists"] = {"cpp_handspelled": sorted(cpp_handspelled)}
    r["exclusions"] = {
        "by_value_class_param": len(excluded),
        "files": sorted(e["file"] for e in excluded),
    }
    r["per_class"] = per_class
    r["_lists"]["c_mangled"] = sorted(c_mangled)

    # Families 2 and 3 read file contents once. Shadow declarations are split by
    # extension: the .cpp side is the one the destructor migrations created, and the
    # one Phase 4 of the plan retires, so a single tree-wide number hides the trend.
    z = lambda: {"c": 0, "cpp": 0}
    local_body, no_include, pad, asm_files, launder = z(), z(), z(), z(), z()
    launder_sites = z()
    cv_launder, cv_launder_sites = z(), z()
    extern_vtable, extern_vtable_classes = z(), set()
    for p in srcs:
        try:
            t = (REPO / p).read_text(errors="ignore")
        except OSError:
            continue
        k = "c" if p.endswith(".c") else "cpp"
        if LOCAL_BODY.search(t):
            local_body[k] += 1
            if not PROJECT_INCLUDE.search(t):
                no_include[k] += 1
        if PAD_LAYOUT.search(t):
            pad[k] += 1
        if INLINE_ASM.search(t):
            asm_files[k] += 1
        # Counted from CODE, not prose. The old rule was LAUNDER.search(t) over whole
        # file text, which meant a comment *explaining* an idiom scored exactly like the
        # idiom, deleting an idiom scored nothing while its comment survived, and the
        # cheapest way to improve the number was to reword -- which a merged file
        # (now src/game/actors/d_a_obj_bc_switch.cpp) documents doing on purpose.
        # delaunder.find_sites masks comments and string literals and locates the real
        # idiom, so this number now moves only when code does.
        # The launder kinds only. delaunder's default kind set also carries PARENS,
        # which is a cosmetic tidy for the parentheses a removal exposes, not a codegen
        # hack -- counting it here would inflate this number by thousands.
        sites = delaunder.find_sites(t, delaunder.LAUNDER_KINDS)
        if sites:
            launder[k] += 1
            launder_sites[k] += len(sites)
        # THE CV-QUALIFIER FAMILY, counted separately and for a reason. The three
        # idioms above are sometimes real: `(s64)a * b >> 12` is how you write a
        # Q12 multiply, and the metric cannot tell that from a crutch. These two
        # can be told. `(s32)(volatile s32)x` casts a prvalue to a cv-qualified
        # non-class type, which the language says discards the qualifier -- it
        # emits nothing and means nothing, so it exists only to steer the
        # allocator. Its partner declares a pointee volatile so loads are not CSEd
        # and then casts the volatile back off at every read, which is the same
        # trick wearing a hat. Both were used to match
        # dBgW_Kc::DetectClsn(dBgCh_SphCrr&) and NO gate in this tree could see
        # either; that is what this family is here to fix. Genuine hardware
        # volatile is not counted -- see delaunder's module docstring.
        cv = delaunder.find_sites(t, delaunder.CV_KINDS)
        if cv:
            cv_launder[k] += 1
            cv_launder_sites[k] += len(cv)
        # THE FAKE VTABLE. `extern int _ZTV6ToxBox[]; *(int**)p = _ZTV6ToxBox;` stores a
        # vptr by hand because no polymorphic class exists to emit one. It byte-matches
        # permanently -- storing the address of the ROM's own vtable is exactly what the
        # ROM does -- so no byte gate can ever object, and `romdata_check` finds nothing
        # to compare because the compiler emits no vtable at all. Invisible by
        # construction to every other check in this tree, which is why it is counted
        # here. Masked like the launder metric above: a comment naming `_ZTV` is prose,
        # and rewriting prose must not move a defect count.
        found = _extern_vtable_classes(t)
        if found:
            extern_vtable[k] += 1
            extern_vtable_classes |= found

    tot = lambda d: d["c"] + d["cpp"]
    r["shadow_decls"] = {
        "local_struct_body": tot(local_body),
        "local_struct_body_cpp": local_body["cpp"],
        "local_body_no_include": tot(no_include),
        "local_body_no_include_cpp": no_include["cpp"],
        "pad_layout": tot(pad),
    }
    r["codegen_hacks"] = {"inline_asm": tot(asm_files),
                          "launder_or_forced": tot(launder),
                          "launder_sites": tot(launder_sites),
                          "cv_launder": tot(cv_launder),
                          "cv_launder_sites": tot(cv_launder_sites),
                          "extern_vtable": tot(extern_vtable),
                          # Distinct classes, not files: many files fake the SAME class
                          # (`_ZTV8Platform` alone appears in 161 of them), and modelling
                          # that one class retires every one of its sites at once. The
                          # class count is therefore the size of the remaining WORK; the
                          # file count is the size of the debt. Both ratchet, because
                          # they can move independently -- a new file faking an
                          # already-faked class raises only the first.
                          "extern_vtable_classes": len(extern_vtable_classes)}
    return r


# Every metric here is a defect count: lower is better, so CI fails on any increase.
# `unmigrated_total` is the headline. It cannot be gamed by renaming a file --
# neither the extension nor the basename decides anything, because `classify` reads
# the body when the name does not carry a mangled symbol.
#
# That second half was NOT true until it was tested. Keying on the basename alone
# meant `git mv _ZN5FaderD1Ev.c func_0201786c.c`, content untouched, LOWERED the
# count, and renaming every mangled-stem file to `func_fake_N` drove the headline to
# "0 of 1258 (0.0%)" without editing a byte. The metric rewarded making the symbol
# table less true, which is the opposite of its purpose.
#
# WHEN A COUNT MAY RISE. It may not, silently -- that is the whole gate. It may rise
# in a PR that commits a root `langmode-baseline.json` booking the increase (the banked
# copy lives on the chaos-data branch; a root file overrides it, so the +N still lands
# as a reviewable diff in the PR), and only when the increase
# is exactly a set of renames giving previously-unnamed (`func_*`) functions their
# evidenced C++ symbols. The PR must name the evidence -- a vtable slot, or
# `tools/dtor_variant_audit.py --discover`. Naming a function the tree had never
# identified genuinely mints new language-mode debt, and the honest way to book that
# is a loud, reviewable +N diff to a committed file, not an exemption. Any other
# increase is a regression.
RATCHET = [
    ("language_mode", "unmigrated_total"),
    ("language_mode", "cpp_still_handspelled"),
    ("shadow_decls", "local_struct_body"),
    ("shadow_decls", "local_body_no_include"),
    ("shadow_decls", "pad_layout"),
    ("codegen_hacks", "inline_asm"),
    ("codegen_hacks", "launder_or_forced"),
    ("codegen_hacks", "launder_sites"),
    ("codegen_hacks", "cv_launder"),
    ("codegen_hacks", "cv_launder_sites"),
    ("codegen_hacks", "extern_vtable"),
    ("codegen_hacks", "extern_vtable_classes"),
]


def summary(r):
    lm, sd, ch = r["language_mode"], r["shadow_decls"], r["codegen_hacks"]
    t = r["totals"]
    tot = lm["mangled_total"]
    pct = 100.0 * lm["unmigrated_total"] / tot if tot else 0.0
    out = []
    out.append(f"src files                    {t['src_files']:6d}"
               f"   ({t['c_extension']} .c / {t['cpp_extension']} .cpp)")
    out.append("")
    out.append(f"NOT MIGRATED                 {lm['unmigrated_total']:6d}"
               f"   of {tot} mangled-symbol files ({pct:.1f}%)")
    out.append(f"    .c extension             {lm['mangled_c']:6d}")
    out.append(f"    .cpp, symbol hand-spelt  {lm['cpp_still_handspelled']:6d}"
               "   renamed, never migrated")
    out.append("")
    out.append("  by symbol kind (unmigrated / genuinely migrated)")
    order = ["method", "D0", "D1", "D2", "C1", "C2", "C3"]
    for k in order + [k for k in sorted(lm["by_kind"]) if k not in order]:
        v = lm["by_kind"].get(k)
        if not v:
            continue
        note = "UNPROVEN -- 0 migrated" if v["migrated"] == 0 else \
               f"{v['migrated']} migrated"
        out.append(f"    {k:<8s}               {v['unmigrated']:6d}   ({note})")
    out.append(f"  excluded (by-value class)  {r['exclusions']['by_value_class_param']:6d}"
               f"   not backlog -- runbook section 7")
    out.append("")
    lf = [c for c in r["per_class"].values() if c["layout_free"]]
    lff = sum(c["c_left"] for c in lf)
    out.append(f"  layout-free candidates     {lff:6d}   across {len(lf)} namespaces"
               " -- no this/vtable/header")
    out.append("")
    out.append("shadow declarations")
    out.append(f"    local struct body        {sd['local_struct_body']:6d}"
               f"   ({sd['local_struct_body_cpp']} in .cpp)")
    out.append(f"    ...and no include        {sd['local_body_no_include']:6d}"
               f"   ({sd['local_body_no_include_cpp']} in .cpp)"
               "  invisible to affected_src.py")
    out.append(f"    char pad[0x..] layout    {sd['pad_layout']:6d}")
    out.append("")
    out.append("codegen hacks")
    out.append(f"    inline asm               {ch['inline_asm']:6d}")
    out.append(f"    extern _ZTV vptr store   {ch['extern_vtable']:6d}"
               f"   ({ch['extern_vtable_classes']} class(es) unmodelled)")
    out.append(f"    launder / forced         {ch['launder_or_forced']:6d}"
               f"   ({ch['launder_sites']} site(s) in code)")
    out.append(f"    cv-qualifier launder     {ch['cv_launder']:6d}"
               f"   ({ch['cv_launder_sites']} site(s) in code)")
    return "\n".join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--json", metavar="PATH", help="write the full report as JSON")
    ap.add_argument("--check", metavar="PATH",
                    help="compare against a banked report; exit 1 if any count rose")
    ap.add_argument("--full", action="store_true",
                    help="with --json: include per-class detail and file lists")
    ap.add_argument("--by-class", action="store_true",
                    help="per-class backlog, worst first")
    ap.add_argument("--list", metavar="WHICH",
                    choices=["c-mangled", "cpp-handspelled", "excluded", "layout-free"],
                    help="print a file list and nothing else")
    args = ap.parse_args()

    r = audit()

    if args.list == "c-mangled":
        print("\n".join(r["_lists"]["c_mangled"]))
        return 0
    if args.list == "cpp-handspelled":
        print("\n".join(r["_lists"]["cpp_handspelled"]))
        return 0
    if args.list == "excluded":
        print("\n".join(r["exclusions"]["files"]))
        return 0
    if args.list == "layout-free":
        # The Phase 1 work list: namespaces with no header and no ctor/dtor, so a
        # migration cannot shift anyone's offsets. Same predicate and the same files
        # the headline "layout-free candidates" count is summed from.
        print("\n".join(sorted(f for c in r["per_class"].values()
                               if c["layout_free"] for f in c["files"])))
        return 0

    if args.json:
        p = pathlib.Path(args.json)
        p.parent.mkdir(parents=True, exist_ok=True)
        banked = {k: v for k, v in r.items() if k != "_lists"}
        if not args.full:
            # The committed baseline carries only what --check reads. per_class is ~2,950
            # lines and the ratchet never looks at it, so banking it would make every
            # legitimate improvement a 3,000-line diff. --full when you want the lot.
            banked.pop("per_class", None)
            banked.get("exclusions", {}).pop("files", None)
            banked["_note"] = ("ratchet baseline; regenerate with "
                              "`python tools/langmode_audit.py --json <this file>`. "
                              "Use --full for per-class detail and file lists.")
        p.write_text(json.dumps(banked, indent=2, sort_keys=True) + "\n")
        print(f"wrote {args.json}" + ("" if args.full else " (ratchet metrics only)"))

    if args.check:
        old = json.loads(pathlib.Path(args.check).read_text())
        regressions = []
        for fam, key in RATCHET:
            was = old.get(fam, {}).get(key)
            now = r[fam][key]
            if was is not None and now > was:
                regressions.append(f"  {fam}.{key}: {was} -> {now}  (+{now - was})")
        if regressions:
            print("langmode ratchet FAILED -- these counts may fall, never rise:")
            print("\n".join(regressions))
            print("\nA C++ symbol was added in a .c file, a shadow declaration was\n"
                  "introduced, or a vptr was stored through an `extern _ZTV` array\n"
                  "instead of a real polymorphic class (extern_vtable*). The last one\n"
                  "byte-matches permanently and no other gate in this tree can see it.\n"
                  "See notes/plan-cpp-language-mode.md section 3.")
            return 1
        print("langmode ratchet PASS")
        return 0

    print(summary(r))

    if args.by_class:
        print("\nper-class backlog (worst first)")
        print(f"  {'class':24s} {'.c left':>7s} {'header':>7s}  kinds")
        rows = sorted(r["per_class"].items(),
                      key=lambda kv: -kv[1]["c_left"])
        for cls, pc in rows[:25]:
            kinds = ", ".join(f"{k}:{v}" for k, v in sorted(pc["kinds"].items()))
            hdr = "yes" if pc["has_header"] else "--"
            print(f"  {cls:24s} {pc['c_left']:7d} {hdr:>7s}  {kinds}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
