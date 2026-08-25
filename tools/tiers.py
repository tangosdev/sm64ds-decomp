#!/usr/bin/env python3
"""The three tiers: how much of the game is MATCHED, CONVERTED, and LINKED.

One percentage cannot describe this project, because "done" means three
different things and they move independently:

  MATCHED    the C compiles to the ROM's exact bytes. Provably the original
             logic. This is the number the README bar and the treemap show.
  CONVERTED  a person can read that C without the ROM open next to them.
             Matching does not require readable code, so this tier lags far
             behind MATCHED and does not move on its own.
  LINKED     the matched translation unit actually reaches the PC port's
             binary, replacing the host stand-in that was there before.

They are deliberately not a funnel: a function can be MATCHED and LINKED while
still being unreadable, and CONVERTED work never changes the matched bytes.
Reporting only MATCHED overstates how finished the project is, which is the
whole reason this file exists.

Denominators differ and that is not a bug. MATCHED counts functions against the
dsd config's function universe. CONVERTED counts source-owned FUNCTIONS. Most
physical files still own one function, but reconstructed translation units own
several; weighting by their enrolled symbols keeps the metric invariant when the
same readable behavior is consolidated into its original file. LINKED counts
matched translation units, not all functions.

Usage:
    python tools/tiers.py                 # human-readable table
    python tools/tiers.py --json          # machine-readable, for chaos-db
    python tools/tiers.py --write-readme  # rewrite the block in README.md

MATCHED is read from chaos-db.json when it is present, which is the same file
the README bar and treemap read, so every surface reports one number. Without
it, the tiers that need no ROM are still reported and MATCHED is omitted rather
than guessed.
"""
import json
import os
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import delaunder  # noqa: E402  (code_mask only -- no compiler, no ROM; see _code_only)
import demangle  # noqa: E402  (pure string work, no compiler, no ROM; see _reader_name)
import srcpath  # noqa: E402  (enrolled source -> function ownership)

SRC = REPO / "src"
README = REPO / "README.md"
README_START = "<!-- tiers:start -->"
README_END = "<!-- tiers:end -->"
LINKED_STAMP = REPO / "config" / "port_linkage.json"


# ---------------------------------------------------------------- CONVERTED
#
# Five independent checks, each one a thing a reader needs. A file is CONVERTED
# only if it passes all five. These regexes are the pinned definition of the
# tier: changing one silently redefines the published percentage, so change them
# in a PR that says what moved and why, never as a drive-by.
#
# Three of them run over `_code_only(text)`, not raw text, so a comment that names
# a defect no longer scores as the defect. See _code_only for the measurement.
#
# The bar was set on 2026-08-07 and is deliberately strict. Softer readings were
# considered and rejected: "includes a shared header" scores 28.9% but measures
# include hygiene rather than readability, and dropping to the two criteria that
# actually block a reader scores 6.6%. Both are reported below the headline so
# the strictness is visible instead of being an unexplained low number.

# The symbol this file exists to produce. The `@symbol` marker names it where
# present; otherwise the filename stem is the same name by construction.
SYMBOL = re.compile(r"//\s*@symbol\s+(\S+)")
PLACEHOLDER = re.compile(r"^func_(ov\d+_)?0[0-9a-f]{7}$")
MANGLED = re.compile(r"^_Z[0-9NK]")

# One component of a reader-visible name that is still a placeholder even when the
# rest of the name is real. `Player::Unk_020c9e5c`, `CommonModel::Func_020160AC`
# and `ActorBase::Virtual38` all wear a genuine class prefix over a method half
# that is the ROM address or a vtable slot index, so the pair reads no better than
# the bare `func_020c9e5c` did. `FUN_<addr>` is Ghidra's default for a function it
# never analysed. The trailing `_Z` catches a mangled name that reached this test
# without demangling.
#
# Note the old MANGLED above is deliberately kept but no longer decides the tier:
# `^_Z[0-9NK]` misses `_ZT` and `_Zd`/`_Zn`, so vtable symbols, thunks and the
# operator new/delete pair scored as REAL names. Six `_ZThn80_*` thunk files and
# six `FUN_<addr>` files passed `real_name` on that hole before this change.
PLACEHOLDER_PART = re.compile(
    r"^(?:func|data)_(?:ov\d+_)?0[0-9a-f]{7}$"
    r"|^(?:Unk|Func|Method|Sub)_[0-9a-fA-F]{6,8}$"
    r"|^Virtual\d+$"
    r"|^FUN_[0-9a-fA-F]+$"
    r"|^_Z", re.IGNORECASE)

# Raw object-layout arithmetic: a cast-and-offset into an object, in the forms
# the tree actually uses. The single biggest thing holding this tier down.
#
# KNOWN HOLE, same PR as the one above: `(int)this + 0x154` is the same defect --
# a field address computed by hand -- but the redundant integer cast makes it read
# as arithmetic on an int rather than a cast into an object, so it passes. 4 files
# use it. Left here rather than fixed silently, because widening what the tier
# counts while leaving a known evasion in place is how a metric goes soft.
RAW_OFFSET = re.compile(r"""
    \(\s*(?:unsigned\s+|signed\s+)?\w+\s*\*\s*\)\s*\(\s*\w+\s*\+\s*0x   # *(u32*)(c + 0x74)
  | \(\s*char\s*\*\s*\)\s*[\w\)\(]+\s*\+\s*0x                            # (char*)self + 0x74
""", re.VERBOSE)
UNK_FIELD = re.compile(r"\bunk_[0-9a-fA-F]+\b")

# Codegen steering that exists only to make the compiler emit given bytes.
LAUNDER = re.compile(r"&\s*0xFFFFFFFFFFFFFFFF")
VOLATILE = re.compile(r"\bvolatile\b")
ASM = re.compile(r"\b(__asm|asm\s*\()")

# A call to another function under its mangled name: readable code calls
# Player::SpinBounce, not _ZN6Player10SpinBounceE5Fix12IiE.
#
# KNOWN HOLE, deliberately left for its own PR: the label says "calls things by
# real names" but only the mangled half is tested, so a file that calls nothing but
# `func_020c9e5c` placeholders passes. Closing it is correct and was measured --
# tree-wide this criterion falls 6,521 -> 1,431 and it becomes the tier's binding
# constraint instead of real_name -- but it de-credits 204 of the currently banked
# CONVERTED files, nearly all plain C files with no bearing on the real_name defect
# this PR fixes. Bundling a 204-file backslide into a correction about C++ method
# names would make one number answer two questions. See notes/converted-tier.md.
MANGLED_REF = re.compile(r"\b_Z[0-9NK]\w+")
SHARED_HEADER = re.compile(r'#include\s+"(decl_|common\.h|[A-Z])')

CRITERIA = ("real_name", "no_raw_offset", "no_unk_field", "no_codegen_trick",
            "no_mangled_refs")
CRITERION_LABEL = {
    "real_name": "Real function name (not func_<addr>, Unk_<addr>, Virtual<n>)",
    "no_raw_offset": "No raw offset arithmetic (*(u32*)(c + 0x74))",
    "no_unk_field": "No unk_<off> fields",
    "no_codegen_trick": "No codegen tricks (launder mask, volatile, asm)",
    "no_mangled_refs": "Calls things by real names, not mangled _Z",
}


def _defined_symbol(path, text):
    m = SYMBOL.search(text)
    if m:
        return m.group(1)
    return os.path.splitext(os.path.basename(path))[0]


def _code_only(text):
    """`text` with comments and string/char literals blanked to spaces.

    Three of the five criteria are questions about CODE, and asking them of raw file
    text answers a different question: whether the subject is MENTIONED anywhere. A
    comment that explains why an offset cast was needed, or names the mangled symbol a
    function used to be called, scored exactly like the defect it describes -- so
    documenting your own cleanup made the file look dirtier, and the cheapest way to
    improve the tier was to delete the explanation. That is the same inversion
    langmode_audit.py already had to fix for its launder metric (see the comment at its
    delaunder.find_sites call), and it is why that metric now reads code too.

    Measured on this tree, masking flips 1,310 file-criterion readings, every one of
    them fail -> pass: 1,291 no_mangled_refs, 14 no_unk_field, 3 no_raw_offset, 2
    no_codegen_trick. Blanking only ever REMOVES text, so it can turn a failure into a
    pass but never the reverse, which was checked rather than assumed. The headline
    CONVERTED count does not move at all (426 before, 426 after) because every affected
    file already fails a different criterion -- usually real_name, since a file whose
    stem is `_ZN...` is the very thing that also mentions mangled names in prose. So
    this fixes the published per-criterion breakdown without redefining the published
    percentage, which is the only shape of change this file's header permits quietly.

    Two criteria deliberately keep reading RAW text: `real_name` looks for the
    `// @symbol` marker, which is a comment by construction, and `shared_header` looks
    for `#include "..."`, whose argument is a string literal.

    delaunder.code_mask is the masker rather than a second copy of one; it is imported
    for that alone and needs no compiler or ROM (langmode_audit.py imports the same
    module in a CI job that installs no build dependencies).
    """
    keep = delaunder.code_mask(text)
    return "".join(c if k else " " for c, k in zip(text, keep))


def _reader_name(path, text):
    """The name a READER sees, which is not always the linker symbol.

    For a plain C function the two are the same string, and this returns the symbol
    unchanged. For a C++ method they are not, and the difference used to decide the
    tier the wrong way round.

    mwccarm emits `_ZN10KoopaShell13OnYoshiTryEatEv` for `KoopaShell::OnYoshiTryEat()`,
    and eligible.py REQUIRES a file's defined symbol to equal its config symbol (it
    rejects with `defines X, expected Y`). So a converted method cannot carry any
    other symbol, and asking "is the symbol a placeholder or mangled?" asked a
    question no method could answer well: the only way to pass was to un-convert the
    method back into a flat extern "C" function. Measured before this change, 0 of
    the 426 CONVERTED files had a mangled symbol -- the tier structurally excluded
    every C++ method in the tree, which inverts what it exists to reward, and is why
    CONVERTED read as "furthest behind and not moving" while 2,034 files had already
    been converted into real methods.

    Demangling the file's own symbol answers the question the tier actually means.
    That is done with the repo's demangler rather than by matching `Class::Method(`
    in the body, because the text search was tried first and is wrong four ways:
    it finds a DEPENDENCY's stub or a bare forward declaration instead of the file's
    own function (src/_ZN5Stage13UpdateMessageEv.cpp reported `Message::UpdateWindow`;
    src/_ZN6Coffin13InitResourcesEv.cpp matched a `MeshCollider::LoadFile` prototype),
    it lets an unrelated call override a genuinely-unidentified `func_ov*` filename,
    it drops outer qualifiers (`Sound::Player::SetPlayableSeqCount` -> `Player::...`,
    colliding with the unrelated real `Player` class), and it cannot match a ctor or
    dtor at all, since those have no return type -- which would have excluded the 749
    already-converted destructor files that motivated the change in the first place.

    Returns (name, ok). `ok` is False for a symbol that starts `_Z` but does not
    demangle, and for a compiler-emitted thunk: `_ZThn80_N9ModelAnimD0Ev` demangles
    to a real class and method, but no one wrote that function and its name encodes
    an adjustment offset, so it is not a name a reader benefits from.
    """
    sym = _defined_symbol(path, text)
    return _reader_name_for_symbol(sym)


def _reader_name_for_symbol(sym):
    """The reader-visible name for one symbol owned by a source file."""
    d = demangle.demangle(sym)
    if d is None:
        return sym, not sym.startswith("_Z")
    return d["qualified"], not d.get("thunk")


def _real_name(path, text):
    name, ok = _reader_name(path, text)
    if not ok or not name:
        return False
    return all(not PLACEHOLDER_PART.match(part.lstrip("~").strip())
               for part in name.split("::") if part.strip())


def _real_name_for_symbol(sym):
    name, ok = _reader_name_for_symbol(sym)
    if not ok or not name:
        return False
    return all(not PLACEHOLDER_PART.match(part.lstrip("~").strip())
               for part in name.split("::") if part.strip())


def score_file(path, text):
    """The five criteria for one source file, plus the header reading."""
    code = _code_only(text)
    return {
        "real_name": _real_name(path, text),
        "no_raw_offset": not RAW_OFFSET.search(code),
        "no_unk_field": not UNK_FIELD.search(code),
        "no_codegen_trick": not (LAUNDER.search(code) or VOLATILE.search(code)
                                 or ASM.search(code)),
        "no_mangled_refs": not MANGLED_REF.search(code),
        "shared_header": bool(SHARED_HEADER.search(text)),
    }


def converted(src_root=None):
    """Score every source-owned function. Returns CONVERTED plus its breakdown.

    Reads committed source only - no ROM, no build, no local state - so it
    reproduces on a fresh checkout, which is what CI needs. File-wide criteria are
    applied to every function the enrollment table assigns to that source; the name
    criterion is evaluated per symbol.
    """
    root = pathlib.Path(src_root or SRC)
    files = sorted(str(p) for p in root.rglob("*")
                   if p.suffix in (".c", ".cpp") and p.is_file())
    counts = dict.fromkeys(list(CRITERIA) + ["shared_header"], 0)
    passes_hist = {}
    readable = core = 0
    ownership = srcpath.source_ownership_index()
    repo_root = srcpath.REPO.absolute()

    source_readable = 0
    total = 0
    for p in files:
        with open(p, errors="replace") as f:
            text = f.read()
        file_score = score_file(p, text)
        path = pathlib.Path(p)
        try:
            rel = path.absolute().relative_to(repo_root).as_posix()
        except ValueError:
            rel = None
        members = ownership.get(rel) or [path.stem]
        all_readable = True
        for sym in members:
            s = dict(file_score)
            s["real_name"] = _real_name_for_symbol(sym)
            total += 1
            for k in counts:
                counts[k] += s[k]
            n_pass = sum(s[k] for k in CRITERIA)
            passes_hist[n_pass] = passes_hist.get(n_pass, 0) + 1
            if n_pass == len(CRITERIA):
                readable += 1
            else:
                all_readable = False
            # The two criteria that genuinely block a reader, reported as context
            # for how strict the headline is.
            if s["real_name"] and s["no_raw_offset"]:
                core += 1
        if members and all_readable:
            source_readable += 1

    return {
        "functions": total,
        "source_files": len(files),
        "converted_source_files": source_readable,
        "converted": readable,
        "pct": round(100.0 * readable / total, 2) if total else 0.0,
        "criteria": {k: counts[k] for k in CRITERIA},
        "distribution": {str(k): passes_hist.get(k, 0)
                         for k in range(len(CRITERIA) + 1)},
        "alt_core_two": core,
        "alt_shared_header": counts["shared_header"],
    }


# ------------------------------------------------------------------- LINKED
#
# LINKED cannot be computed here and must not pretend otherwise. port/tools/
# linkage.py reads build/port/walk_window.map, which only exists after an MSVC
# build of the port, and the port branches are not merged to main. So the number
# is STAMPED: measured by hand, recorded with the branch and commit it came from,
# and refreshed when someone runs a port build.
#
# A stamp with no provenance is worse than no number, so a stamp missing its
# branch/commit/date is treated as absent.


def linked(stamp_path=None):
    """The stamped LINKED measurement, or None if there is no usable stamp."""
    p = pathlib.Path(stamp_path or LINKED_STAMP)
    if not p.is_file():
        return None
    try:
        d = json.loads(p.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None
    if not all(d.get(k) for k in ("measuredAt", "branch", "commit")):
        return None
    tot = d.get("matchedTus") or 0
    got = d.get("linkedTus") or 0
    if not tot:
        return None
    return {
        "linked": got,
        "matchedTus": tot,
        "pct": round(100.0 * got / tot, 2),
        "measuredAt": d["measuredAt"],
        "branch": d["branch"],
        "commit": d["commit"],
        "stale": bool(d.get("stale", False)),
    }


# ------------------------------------------------------------------ MATCHED


def matched(db_path=None):
    """MATCHED from chaos-db.json, the file the bar and treemap also read.

    Returns None when the db is absent rather than recomputing it: a second
    implementation of the matched rule is a second source of truth, and this
    repo has already paid for one of those (the itcm undercount).
    """
    p = pathlib.Path(db_path) if db_path else (REPO / "chaos-db.json")
    if not p.is_file():
        return None
    try:
        db = json.loads(p.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None
    st = db.get("stats") or {}
    tot, got = st.get("totalFunctions"), st.get("matchedFunctions")
    if not tot:
        return None
    return {
        "matched": got,
        "functions": tot,
        "pct": round(100.0 * got / tot, 2),
        "matchedBytes": st.get("matchedBytes"),
        "totalBytes": st.get("totalBytes"),
        "bytePct": (round(100.0 * st["matchedBytes"] / st["totalBytes"], 2)
                    if st.get("totalBytes") else None),
    }


# ------------------------------------------------------------------- output


def collect(db_path=None):
    return {"matched": matched(db_path), "converted": converted(), "linked": linked()}


def bar(done, tot, width=30):
    filled = round(done / tot * width) if tot else 0
    if done and filled == 0:
        filled = 1
    return "█" * filled + "░" * (width - filled)


def bar_block(t, preserved_matched=None):
    """The fenced block shown under the README "## The three tiers" heading."""
    rows = []
    m, c, k = t["matched"], t["converted"], t["linked"]
    if m:
        rows.append(f"MATCHED    {bar(m['matched'], m['functions'])}  "
                    f"{m['pct']:4.1f}%   {m['matched']:,} / {m['functions']:,} functions")
    elif preserved_matched:
        # A worktree may have the ROM inputs needed for CONVERTED/LINKED but not
        # chaos-db.json, which supplies MATCHED.  Updating the live rows must not
        # silently erase the last independently generated MATCHED measurement.
        rows.append(preserved_matched)
    rows.append(f"CONVERTED  {bar(c['converted'], c['functions'])}  "
                f"{c['pct']:4.1f}%   {c['converted']:,} / {c['functions']:,} functions")
    if k:
        rows.append(f"LINKED     {bar(k['linked'], k['matchedTus'])}  "
                    f"{k['pct']:4.1f}%   {k['linked']:,} / {k['matchedTus']:,} matched TUs")
    return "```\n" + "\n".join(rows) + "\n```"


def write_readme(t):
    text = README.read_text(encoding="utf-8")
    start = text.index(README_START) + len(README_START)
    end = text.index(README_END)
    current = text[start:end]
    preserved_matched = next(
        (line.strip() for line in current.splitlines()
         if line.strip().startswith("MATCHED")),
        None,
    )
    new_text = text[:start] + "\n" + bar_block(t, preserved_matched) + "\n" + text[end:]
    if new_text != text:
        README.write_text(new_text, encoding="utf-8")
        return True
    return False


def report(t):
    m, c, k = t["matched"], t["converted"], t["linked"]
    out = []
    if m:
        out.append(f"MATCHED    {m['matched']:,} / {m['functions']:,} functions  "
                   f"{m['pct']:.1f}%   ({m['bytePct']:.1f}% by code bytes)")
    else:
        out.append("MATCHED    (no chaos-db.json; run tools/chaos_db_ci.py first)")
    out.append(f"CONVERTED  {c['converted']:,} / {c['functions']:,} functions  "
               f"{c['pct']:.1f}%   ({c['source_files']:,} physical source files)")
    if k:
        flag = "  STALE" if k["stale"] else ""
        out.append(f"LINKED     {k['linked']:,} / {k['matchedTus']:,} matched TUs  "
                   f"{k['pct']:.1f}%   (stamped {k['measuredAt']} from {k['branch']}"
                   f" @ {k['commit']}){flag}")
    else:
        out.append("LINKED     (no usable stamp in config/port_linkage.json)")

    out.append("")
    out.append("CONVERTED breaks down as:")
    for key in CRITERIA:
        n = c["criteria"][key]
        out.append(f"  {CRITERION_LABEL[key]:<52} {n:>6,}  {100.0*n/c['functions']:5.1f}%")
    out.append(f"  {'ALL FIVE':<52} {c['converted']:>6,}  {c['pct']:5.1f}%")
    out.append("")
    out.append("Criteria passed, distribution:")
    for i in range(len(CRITERIA) + 1):
        n = c["distribution"][str(i)]
        out.append(f"  {i}/5 : {n:>6,}  ({100.0*n/c['functions']:.1f}%)")
    out.append("")
    out.append("Other readings of the same tree, for context on how strict the bar is:")
    out.append(f"  name + no raw offsets only : {c['alt_core_two']:>6,}  "
               f"({100.0*c['alt_core_two']/c['functions']:.1f}%)")
    out.append(f"  includes a shared header   : {c['alt_shared_header']:>6,}  "
               f"({100.0*c['alt_shared_header']/c['functions']:.1f}%)")
    return "\n".join(out)


def main():
    db = None
    if "--from-db" in sys.argv:
        i = sys.argv.index("--from-db")
        if i + 1 < len(sys.argv):
            db = sys.argv[i + 1]
    t = collect(db)

    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

    if "--json" in sys.argv:
        print(json.dumps(t, indent=2))
        return
    if "--write-readme" in sys.argv:
        print(f"README.md {'updated' if write_readme(t) else 'already up to date'}")
        return
    if "--bar" in sys.argv:
        print(bar_block(t))
        return
    print(report(t))


if __name__ == "__main__":
    sys.exit(main())
