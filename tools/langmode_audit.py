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

    # CI ratchet: counts may fall, never rise. The baseline lives at the repo root
    # because progress/ is gitignored and CI needs a committed reference point. It holds
    # only the metrics --check reads, so re-banking it stays a small diff.
    python tools/langmode_audit.py --json langmode-baseline.json    # re-bank after work
    python tools/langmode_audit.py --check langmode-baseline.json   # exit 1 if worse
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
import demangle as D  # noqa: E402

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
    return "NONMATCHING" in t


# A local struct/class body, e.g. `struct Actor { char pad[0xd0]; ... };`
LOCAL_BODY = re.compile(r"^\s*(?:struct|class)\s+\w+\s*(?::[^;{]*)?\{", re.M)
PROJECT_INCLUDE = re.compile(r'^\s*#\s*include\s*"', re.M)
PAD_LAYOUT = re.compile(r"char\s+pad\[0x", re.M)
INLINE_ASM = re.compile(r"__asm|(?<![A-Za-z0-9_])asm\s*[({]")
LAUNDER = re.compile(r"launder|force.*(?:reg|codegen)|codegen.*force", re.I)


def tracked_sources():
    """Every git-tracked C/C++ file under src/, as repo-relative posix paths."""
    out = subprocess.run(
        ["git", "-C", str(REPO), "ls-files", "src"],
        capture_output=True, text=True, check=True,
    ).stdout.split()
    return [p for p in out if p.endswith((".c", ".cpp"))]


def classify(path):
    """Return (kind, info) for a mangled-symbol file, or (None, None) if not one.

    `kind` is the ctor/dtor variant (C1/C2/D0/D1/D2) or 'method'.
    """
    stem = path.rsplit("/", 1)[-1].rsplit(".", 1)[0]
    if not MANGLED.match(stem):
        return None, None
    try:
        info = D.demangle(stem)
    except Exception:
        return None, None
    if not info or not info.get("qualified"):
        return None, None
    kind = info.get("variant") or "method"
    return kind, info


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
        kind, info = classify(p)
        if kind is None:
            continue
        stem = p.rsplit("/", 1)[-1].rsplit(".", 1)[0]
        is_c = p.endswith(".c")
        (c_mangled if is_c else cpp_mangled).append(p)

        # Unmigrated means "the source spells the mangled symbol", not "the file is .c".
        # A NONMATCHING draft is not evidence a pattern works, so it counts as neither
        # migrated nor proven -- it is tracked on its own.
        unmigrated = is_c or hand_spells_own_symbol(p, stem)
        draft = is_nonmatching(p)
        if not is_c and unmigrated:
            cpp_handspelled.append(p)
        if draft:
            nonmatching.append(p)

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
                excluded.append({"file": p, "by_value": sorted(set(byval))})
            else:
                cls = info.get("class") or "<free>"
                pc = per_class.setdefault(cls, {"c_left": 0, "kinds": {}})
                pc["c_left"] += 1
                pc["kinds"][kind] = pc["kinds"].get(kind, 0) + 1

    # No include/<Class>.h AND no ctor/dtor => almost certainly an SDK *namespace*
    # (GX, CP15, IRQ...): no `this`, no vtable, no layout, no includers, so a migration
    # cannot shift anyone's offsets. A missing header alone does NOT mean that -- a
    # class like Chuckya has a D0/D1 and therefore a vtable whether a header exists or
    # not, so it carries full layout risk. Keep the two apart.
    CTOR_DTOR = {"C1", "C2", "C3", "D0", "D1", "D2"}
    for cls, pc in per_class.items():
        pc["has_header"] = (REPO / "include" / f"{cls}.h").is_file()
        pc["layout_free"] = (not pc["has_header"]
                             and not (set(pc["kinds"]) & CTOR_DTOR))

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
        if LAUNDER.search(t):
            launder[k] += 1

    tot = lambda d: d["c"] + d["cpp"]
    r["shadow_decls"] = {
        "local_struct_body": tot(local_body),
        "local_struct_body_cpp": local_body["cpp"],
        "local_body_no_include": tot(no_include),
        "local_body_no_include_cpp": no_include["cpp"],
        "pad_layout": tot(pad),
    }
    r["codegen_hacks"] = {"inline_asm": tot(asm_files),
                          "launder_or_forced": tot(launder)}
    return r


# Every metric here is a defect count: lower is better, so CI fails on any increase.
# `unmigrated_total` is the headline: it cannot be gamed by renaming a .c to .cpp,
# because a hand-spelled symbol still counts however the file is named.
RATCHET = [
    ("language_mode", "unmigrated_total"),
    ("language_mode", "cpp_still_handspelled"),
    ("shadow_decls", "local_struct_body"),
    ("shadow_decls", "local_body_no_include"),
    ("shadow_decls", "pad_layout"),
    ("codegen_hacks", "inline_asm"),
    ("codegen_hacks", "launder_or_forced"),
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
    out.append(f"    launder / forced         {ch['launder_or_forced']:6d}")
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
    ap.add_argument("--list", metavar="WHICH", choices=["c-mangled", "cpp-handspelled", "excluded"],
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
            print("\nA C++ symbol was added in a .c file, or a shadow declaration was\n"
                  "introduced. See notes/plan-cpp-language-mode.md section 3.")
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
