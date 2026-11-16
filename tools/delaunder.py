#!/usr/bin/env python3
"""Delete launder idioms that cost nothing, one site at a time, verified against the ROM.

Five idioms were pasted through the tree as matching crutches:

    (EXPR & 0xFFFFFFFFFFFFFFFFULL)      MASK       -- the u64 identity mask
    (long long)(int)(EXPR)              ROUNDTRIP  -- widen-then-narrow cast pair
    (unsigned long long)(EXPR)          WIDEN      -- a lone 64-bit widening cast,
                                                     EXCEPT a fixed-point multiply
    (s32)(volatile s32)EXPR             CVCAST     -- cv round-trip on a scalar rvalue
    ((const T *)p)->m, p declared volatile
                                        CVSTRIP    -- volatile declared, cast off at use

The last two were added 2026-08-21, after `dBgW_Kc::DetectClsn(dBgCh_SphCrr&)` was
matched using both and NO gate in the tree could see either. They are the cv-qualifier
family, and they are worth separating from the first three because their status is not
in doubt the way a widening cast's is:

  * CVCAST is provably semantic dead weight. Casting a prvalue to a cv-qualified
    non-class type discards the qualifier -- `(s32)(volatile s32)x` and `(s32)x` mean
    exactly the same thing to the language. It emits no code. So it cannot be there for
    any reason except to perturb the compiler, and on that function it is what demotes a
    local out of the declaration chain into the temp pool.
  * CVSTRIP is the matching pair to it: declare a pointee `volatile` so the compiler
    will not CSE the loads, then cast the `volatile` straight back off at every read so
    the object does not actually behave volatile. `sphere.pos` is ordinary RAM.

Genuine hardware volatile is NOT counted. `(volatile u16 *)0x4000208` is a pointer cast
to volatile -- CVCAST requires a non-pointer type, and CVSTRIP only fires on a name the
same file declares volatile and then casts un-volatile. Adding volatile is real; taking
it away again at the point of use is the tell.

Most of them do nothing: the compiler emits the same bytes without them. Some are
load-bearing, and no amount of reading tells you which -- one site in
`func_ov002_020f7020.c` carries a comment confidently explaining why it is required,
and the function matches without it. So this tool does not guess. It removes ONE
site, compiles the result under that file's own pin, and compares against the ROM
with relocation destinations checked. A site is deleted only when the bytes still
reproduce at that strict bar.

Granularity is the whole point. Measured on this tree, 1583 of 1707 sites in enrolled
files are free and 124 are not, but they do not separate by file: `func_ov002_020be3b0.c`
needs all 22 of its sites, `Camera::Behavior` needs 4 of 9, and `St_Climb_Main` -- which
a file-at-a-time probe calls load-bearing, because removing everything at once breaks it
-- turns out to need exactly 1 of its 12. Sites are tried greedily, each on top of the
previously accepted text, because removals interact. Per-file, per-site, in order.

Only ENROLLED files are considered. A file the ROM build does not compile has no
byte gate behind it, so "it still verifies" would prove nothing there.

WHAT THE GATE COVERS, AND WHAT IT STILL DOES NOT
------------------------------------------------
Until 2026-11-06 this tool byte-checked ONE function per file -- module_index() kept the
last enrolled row per path -- which is unsafe on exactly the consolidated translation
units most of src/ now consists of. One --apply run over src/engine changed seven files
and tools/prepush_linkcheck.py called all seven NO-REPRO; on d_a_ply_methods.cpp the tool
accepted 21 of 22 sites where the truth is 4. Three things were wrong, and all three are
fixed here:

  * every enrolled symbol of the file is gated now, not one (GATED_SECTIONS, so
    __sinit_<file> is included);
  * the candidate is written into a private directory under its ORIGINAL BASENAME,
    because mwccarm derives symbol names from the file name (__sinit_<file>, and the
    numbered local-class symbols like Bundle$902d_a_mky_cpp) -- compiled as cand_7.cpp a
    candidate cannot match them, and the old shared cand_<N> namespace also let two
    concurrent sweeps overwrite each other;
  * a byte-identical removal that shifts the per-TU @NNN local numbering is declined,
    via the __sinit fingerprint, because landing it needs defined_symbol_renames re-keyed.

Still NOT seen by this tool, measured the same day and left for a human:

  * MASK needs an enclosing parenthesis group, so `*cur = ip & 0xFFFFFFFFFFFFFFFF;`,
    `& ~0ULL`, and a mask hidden behind a macro are all invisible.
  * ROUNDTRIP removes a pair's two casts together. Dropping ONLY the outer widening cast
    is byte-free at many sites the tool calls load-bearing -- but byte-freeness is not
    sufficient there: where the outer cast carries the arithmetic (isdoverlay, G3i, cstd)
    dropping it silently wraps in 32 bits. A rule for this needs to be restricted to
    address round-trips inside a pointer cast.
  * `(T *)(int)p` -- a pointer round-tripped through int -- matches no rule: ROUNDTRIP and
    WIDEN want a 64-bit outer cast and CVCAST's scalar pattern has no star in it. It was
    97% free where it was swept by hand.
  * the reverse-order `(int)((long long)x)` shape has no rule either.

tools/prepush_linkcheck.py remains the authority after a sweep: it compares at the delink
extent, which this tool cannot do, and that is the difference on func_0206a928, where
config/arm9/symbols.txt records 0x1360 while the function's literal pool runs to 0x1364.

Edits are textual deletions of whole tokens, with the enclosing parenthesis group
rebalanced, so they cannot change the parse of surrounding code. Comment and string
regions are masked out before sites are located: the idiom appears in prose too, and
prose is not the thing being fixed. Files whose comments still describe an idiom that
is now gone are reported separately -- that text is both false and, because the
langmode metric is a regex over source text, the reason the metric would not move.

    python tools/delaunder.py --dry-run
    python tools/delaunder.py --apply
    python tools/delaunder.py --apply --kinds MASK --limit 40 --jobs 8
"""

import argparse
import concurrent.futures
import json
import pathlib
import re
import shutil
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

# The sweep needs the compiler and the extracted ROM; find_sites() and code_mask()
# need neither -- they are regex over text. langmode_audit.py imports this module for
# find_sites alone and runs in a CI job that installs no build dependencies, so a hard
# import here would make a static metric fail on `No module named 'elftools'`. Import
# what the sweep needs when the sweep actually runs.
try:
    import build_pin as BP           # noqa: E402
    import enroll                    # noqa: E402
    import reloc_audit as RA         # noqa: E402
    import relocs as RL              # noqa: E402
    import rombuild_check as RC      # noqa: E402
except ImportError as exc:           # pragma: no cover - exercised by the ratchet job
    BP = enroll = RA = RL = RC = None
    _SWEEP_IMPORT_ERROR = exc
else:
    _SWEEP_IMPORT_ERROR = None


def require_sweep_deps():
    """Fail loudly, and only for the paths that genuinely need a build environment."""
    if _SWEEP_IMPORT_ERROR is not None:
        raise SystemExit(
            f"tools/delaunder.py needs the build-side modules to sweep: "
            f"{_SWEEP_IMPORT_ERROR}. find_sites() alone does not."
        )

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"

MASK_RE = re.compile(r"&\s*0x[fF]{16}(?:[uUlL]+)?")
ROUNDTRIP_RE = re.compile(
    r"\(\s*(?:long\s+long|s64|u64|unsigned\s+long\s+long)\s*\)\s*"
    r"\(\s*(?:int|s32|u32|unsigned|unsigned\s+int)\s*\)"
)
WIDEN_RE = re.compile(r"\(\s*(?:long\s+long|s64|u64|unsigned\s+long\s+long)\s*\)(?=\s*\()")

# The cv-qualifier family. `_CV` deliberately does not match a bare `const`: casting to
# `const T` on an rvalue is idiomatic and harmless, and it is `volatile` that steers the
# allocator. `_SCALAR` has no `*` in it, which is what keeps genuine MMIO casts
# (`(volatile u16 *)0x4000208`) out of the count -- those are pointers to volatile, and
# they are real.
_CV = r"(?:const\s+volatile|volatile\s+const|volatile)"
_SCALAR = (r"(?:signed\s+|unsigned\s+)?"
           r"(?:s8|u8|s16|u16|s32|u32|s64|u64|fx16|fx32|bool|char|short|int|float|double"
           r"|long(?:\s+long)?)")
CVCAST_RE = re.compile(r"\(\s*" + _CV + r"\s+" + _SCALAR + r"\s*\)")

# A pointer-to-volatile declared in this file, so CVSTRIP can tell "cast the volatile
# off something that was declared volatile here" from an ordinary pointer cast.
VOLATILE_DECL_RE = re.compile(
    r"\b(?:const\s+)?volatile\s+([A-Za-z_]\w*)\s*\*\s*(?:volatile\s*)?([A-Za-z_]\w*)\s*[;,)=]")

KINDS = ("MASK", "ROUNDTRIP", "WIDEN", "CVCAST", "CVSTRIP", "PARENS")

# Which sections module_index() gates on. `.init` is here for `__sinit_<file>`, and it
# is not an afterthought: a removal can be byte-identical and still shift mwccarm's
# per-TU `@NNN` local numbering, and then `__sinit` relocates against names
# `config/tu_manifest.d/**` no longer maps. That is reported WRONG, not NO-REPRO, and
# while this tool gated `.text` alone it could not see it at all -- four of the twelve
# blocking verdicts one sweep produced were exactly this. enroll.candidates() files
# those rows as `.init` (305 of them), which is why `sec != ".text"` dropped them.
GATED_SECTIONS = (".text", ".init")

# The kinds langmode_audit counts as codegen hacks. PARENS is a cosmetic tidy, not a
# hack, and is excluded there; keeping the list here means the audit and the sweep
# cannot drift apart.
LAUNDER_KINDS = ("MASK", "ROUNDTRIP", "WIDEN")
CV_KINDS = ("CVCAST", "CVSTRIP")

# Removing a cast or a mask leaves its parentheses behind: `(((self + 0xb0)))`. That is
# not much of a readability win, so PARENS collapses a group whose entire content is
# itself one balanced group. It is skipped when the outer `(` follows an identifier
# character, because there the parentheses are an argument list and `foo((a, b))` is a
# one-argument call while `foo(a, b)` is a two-argument one -- a silent, compiling,
# byte-changing difference. A cast's `)` before the group is fine and is the common case.
IDENT = re.compile(r"[A-Za-z0-9_]")


def code_mask(text):
    """True for every offset that is real code -- comments and string literals are False.

    The idiom appears inside explanatory comments, and rewriting prose is not this
    tool's job; a site found in a comment would also produce a no-op edit that still
    reads as an accepted removal.
    """
    keep = bytearray(b"\x01") * len(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            j = n if j < 0 else j
            keep[i:j] = b"\x00" * (j - i)
            i = j
        elif c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            keep[i:j] = b"\x00" * (j - i)
            i = j
        elif c in "\"'":
            q, j = c, i + 1
            while j < n and text[j] != q:
                j += 2 if text[j] == "\\" else 1
            j = min(j + 1, n)
            keep[i:j] = b"\x00" * (j - i)
            i = j
        else:
            i += 1
    return keep


def _enclosing_group(text, at):
    """(open, close) of the innermost parenthesis group containing offset `at`."""
    depth, i = 0, at - 1
    while i >= 0:
        if text[i] == ")":
            depth += 1
        elif text[i] == "(":
            if depth == 0:
                break
            depth -= 1
        i -= 1
    if i < 0:
        return None
    depth, j = 0, at
    while j < len(text):
        if text[j] == "(":
            depth += 1
        elif text[j] == ")":
            if depth == 0:
                return i, j
            depth -= 1
        j += 1
    return None


def _balanced(s):
    if not (s.startswith("(") and s.endswith(")")):
        return False
    depth = 0
    for k, ch in enumerate(s):
        depth += (ch == "(") - (ch == ")")
        if depth == 0 and k != len(s) - 1:
            return False
    return depth == 0


def _cv_strip_sites(text, keep):
    """`((const T *)p)->m` where THIS file declares `p` as a pointer to volatile.

    Adding volatile is a real thing to do; taking it away again at the point of use is
    the laundering. Requiring the declaration keeps ordinary pointer casts out -- a cast
    to `(const T *)` only counts when the name it is applied to was declared
    `volatile T *` a few lines up, so the cast provably strips a qualifier the same
    author just added.

    The site is the cast tokens alone, so deleting it leaves `(p)->m`, which parses the
    same. Whether the bytes survive is the sweep's problem, not this function's.
    """
    volatiles = {}
    for m in VOLATILE_DECL_RE.finditer(text):
        if keep[m.start()]:
            volatiles[m.group(2)] = m.group(1)
    if not volatiles:
        return []
    sites = []
    for name, ty in volatiles.items():
        # a cast to the same pointee type WITHOUT volatile, applied to that name
        rx = re.compile(r"\(\s*(?:const\s+)?" + re.escape(ty) + r"\s*\*\s*\)\s*(?=" +
                        re.escape(name) + r"\b)")
        for m in rx.finditer(text):
            if keep[m.start()] and "volatile" not in text[m.start():m.end()]:
                sites.append(("CVSTRIP", m.start(), m.end(), ""))
    return sites



def _is_fixed_point_widen(text, end, keep):
    """True when the widened value is an operand of a multiply that is shifted back down.

    That is a Q-format multiply -- `(s64)(a) * (b) >> 12` -- and the cast is REQUIRED:
    without it the product is 32x32->32 and overflows, so the cast changes the value, not
    merely the codegen. A crutch does not change the value; that is the whole difference,
    and it is why this shape is excluded from the launder count rather than merely
    annotated.

    Scans forward from the end of the cast to the end of the enclosing expression. A `;`
    or `{` ends it, and so does a `,` once the scan is back outside the group the cast
    opened -- otherwise `f((u64)(x), y >> 2)` would read the shift from the NEXT
    argument and excuse itself.
    """
    depth, seen_mul, i, n = 0, False, end, len(text)
    while i < n:
        if not keep[i]:
            i += 1
            continue
        ch = text[i]
        if ch in ";{}":
            return False
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
            if depth < -4:                       # far outside; stop looking
                return False
        elif ch == ",":
            if depth <= 0:
                return False
        elif ch == "*":
            seen_mul = True
        elif text.startswith(">>", i):
            return seen_mul
        i += 1
    return False


def find_sites(text, kinds=KINDS):
    """Every removable launder site, as (kind, start, end, replacement), left to right."""
    keep = code_mask(text)
    out = []

    if "MASK" in kinds:
        for m in MASK_RE.finditer(text):
            if not keep[m.start()]:
                continue
            grp = _enclosing_group(text, m.start())
            if not grp:
                continue
            o, c = grp
            if text[m.end():c].strip():
                continue          # something else trails the mask; leave the site alone
            inner = text[o + 1:m.start()].strip()
            if not inner:
                continue
            out.append(("MASK", o, c + 1, inner if _balanced(inner) else f"({inner})"))

    for kind, rx in (("ROUNDTRIP", ROUNDTRIP_RE), ("WIDEN", WIDEN_RE),
                     ("CVCAST", CVCAST_RE)):
        if kind not in kinds:
            continue
        for m in rx.finditer(text):
            if not keep[m.start()]:
                continue
            # A widening cast that feeds a multiply which is then shifted back down is a
            # fixed-point multiply, not a crutch: it changes the value. See
            # _is_fixed_point_widen.
            if kind == "WIDEN" and _is_fixed_point_widen(text, m.end(), keep):
                continue
            out.append((kind, m.start(), m.end(), ""))

    if "CVSTRIP" in kinds:
        out.extend(_cv_strip_sites(text, keep))

    if "PARENS" in kinds:
        for o, ch in enumerate(text):
            if ch != "(" or not keep[o]:
                continue
            prev = text[:o].rstrip()
            if prev and IDENT.match(prev[-1]):
                continue                      # argument list, not a grouping paren
            grp = _enclosing_group(text, o + 1)
            if not grp or grp[0] != o:
                continue
            inner = text[o + 1:grp[1]].strip()
            if _balanced(inner):
                out.append(("PARENS", o, grp[1] + 1, inner))

    # ROUNDTRIP and WIDEN start at the same offset whenever the widening cast is the
    # first half of a round trip: `(long long)(int)` is also a `(long long)` followed by
    # `(`. One syntactic thing, so keep the longer (more specific) match and drop the
    # other -- otherwise every round trip is removed once but counted twice, which
    # silently doubles any metric built on this list.
    out.sort(key=lambda s: (s[1], -(s[2] - s[1])))
    deduped, claimed = [], set()
    for site in out:
        if site[1] in claimed:
            continue
        claimed.add(site[1])
        deduped.append(site)
    return deduped


def apply_site(text, site):
    _, start, end, rep = site
    return text[:start] + rep + text[end:]


def enrolled_paths():
    """Every src path the ROM build actually compiles -- a delinks entry marked `complete`.

    NOT the same set as enroll.candidates(), which is everything that *could* be
    enrolled. The difference matters here: a non-enrolled file has no byte gate behind
    it, so a removal that "still verifies" there proves only that the file is
    self-consistent, not that the ROM agrees. Several of them do not even reproduce
    at baseline.
    """
    out = set()
    for dl in CONFIG.rglob("delinks.txt"):
        cur = None
        for line in dl.read_text(errors="ignore").splitlines():
            if not line.startswith((" ", "\t")) and line.strip().endswith(":"):
                cur = line.strip()[:-1]
            elif cur and line.strip() == "complete":
                out.add(cur.replace("\\", "/"))
    return out


def module_index():
    """src-relative path -> [(symbol, addr, size, module label), ...], enrolled only.

    EVERY enrolled `.text` symbol the file owns, not one of them. This used to keep a
    single row per path (`idx[rel] = ...` in a loop, so the last row won) and that
    made the tool unsafe on exactly the files most of `src/` now consists of: a
    consolidated translation unit was byte-checked on ONE of its functions, and a
    removal anywhere else in the file was "verified" by comparing a function the edit
    cannot affect.

    Measured on three slices before the fix: a sweep of `src/engine` changed seven
    files and all seven came back NO-REPRO from `tools/prepush_linkcheck.py`; on
    `d_a_ply_methods.cpp` the tool accepted 21 of 22 sites where the true answer is 4;
    and the symbol it happened to pick was often a one-line `*_classInit` factory
    containing no launder site at all, or a symbol defined in a DIFFERENT translation
    unit, which made the whole file skip with "baseline does not verify".

    The section filter is GATED_SECTIONS, not `.text`, so a file's `__sinit_<file>` is
    gated too -- see the note on that constant for why that one matters on its own.
    """
    live = enrolled_paths()
    idx = {}
    rows = enroll.candidates()
    rows = rows[0] if isinstance(rows, tuple) else rows
    for d, sym, rel, addr, size, sec in rows:
        rel = rel.replace("\\", "/")
        if sec not in GATED_SECTIONS or rel not in live:
            continue
        idx.setdefault(rel, []).append((sym, addr, size, RC.module_label(d, CONFIG)))
    for rel in idx:
        idx[rel].sort(key=lambda r: r[1])
    return idx


def read_source(p):
    """Decode without translating newlines -- this tree mixes CRLF and LF per file.

    pathlib.read_text() normalises line endings on the way in, so a read/edit/write
    round trip silently rewrites every line of a CRLF file. That is invisible in a
    byte-match check (the compiler does not care) and very visible in review.
    """
    return p.read_bytes().decode("utf-8", errors="ignore")


def write_source(p, text):
    p.write_bytes(text.encode("utf-8"))


def _tmp_copy(path, text):
    """Write `text` as a candidate that compiles to the SAME symbols as `path`.

    Two reasons it is a private directory holding the ORIGINAL BASENAME rather than a
    numbered `cand_N` file:

      * mwccarm derives symbol names from the source file name. `__sinit_<file>` is the
        obvious one, and so are the compiler-numbered local-class symbols like
        `Bundle$902d_a_mky_cpp` (with their mangled length prefix). Compiled as
        `cand_7.cpp` the candidate emits `__sinit_cand_7.cpp`, so the real symbol is
        absent from the object and no gate on it can ever pass.
      * the old name came from a per-PROCESS counter, so two sweeps running at once
        overwrote each other's candidates: `mwccarm.exe: Specified file 'cand_32.cpp'
        not found`, files reported as "does not compile", and free sites reported
        load-bearing. Never a false ACCEPT, but measurably lost sites.
    """
    root = REPO / "build" / "delaunder"
    root.mkdir(parents=True, exist_ok=True)
    d = pathlib.Path(tempfile.mkdtemp(dir=str(root)))
    cand = d / path.name
    write_source(cand, text)
    return d, cand


def split_targets(targets):
    """(destination-checked, fingerprinted). `__sinit_<file>` goes in the second.

    Its relocations name compiler-numbered locals (`@1633`) that only
    `config/tu_manifest.d/**` maps, so a destination check against
    `config/**/relocs.txt` reports eleven wrong destinations on an UNMODIFIED file. The
    sweep compares the candidate's destinations with the original's instead.
    """
    fp = [x for x in targets if x[0].startswith("__sinit")]
    return [x for x in targets if not x[0].startswith("__sinit")], fp


def verify_text(text, path, targets, fp_targets, version, strict):
    """Does this candidate text reproduce EVERY function the file owns?

    `targets` is module_index()'s list for the file. One compile, N comparisons, via
    build_pin.verify_many -- asking verify() per symbol would recompile the file once
    per function, and on a 600-function TU that is the entire cost. Returns
    `(ok, detail, fingerprint)`; see build_pin.verify_many for the fingerprint.
    """
    d, cand = _tmp_copy(path, text)
    try:
        return BP.verify_many(cand, targets, strict=strict, version=version,
                              fp_targets=fp_targets)
    finally:
        shutil.rmtree(d, ignore_errors=True)


def greedy(cur, kinds, verify, rec):
    """Remove sites one at a time, keeping only those the ROM still agrees with.

    On acceptance the site is gone and the next one slides into its index, so `i`
    only advances past sites that earned their place.
    """
    i = 0
    while True:
        sites = find_sites(cur, kinds)
        if i >= len(sites):
            return cur
        cand = apply_site(cur, sites[i])
        if verify(cand):
            cur = cand
            rec["accepted"] += 1
            rec["by_kind"][sites[i][0]] = rec["by_kind"].get(sites[i][0], 0) + 1
        else:
            rec["rejected"] += 1
            rec["rejected_kinds"][sites[i][0]] = rec["rejected_kinds"].get(sites[i][0], 0) + 1
            i += 1


def sweep_file(rel, targets, kinds, strict, tidy=True):
    """Greedy per-site removal, gated on every function the file owns. Never raises."""
    sym, addr, size, label = targets[0]
    path = REPO / rel
    rec = {"file": rel, "symbol": sym, "symbols": len(targets), "module": label,
           "accepted": 0, "rejected": 0,
           "tidied": 0, "by_kind": {k: 0 for k in kinds},
           "rejected_kinds": {k: 0 for k in kinds},
           "status": "", "text": None, "stale_prose": False}
    try:
        base = read_source(path)
    except OSError as e:
        rec["status"] = f"unreadable: {e}"
        return rec

    sites = find_sites(base, kinds)
    if not sites:
        rec["status"] = "no-sites"
        return rec

    version, why = BP.compiler_for(path, sym)
    if version is None:
        rec["status"] = f"no pin: {why}"
        return rec

    gated, fp_targets = split_targets(targets)
    ok, detail, fp0 = verify_text(base, path, gated, fp_targets, version, strict)
    if not ok:
        rec["status"] = f"baseline does not verify: {detail}"
        return rec
    rec["pin"] = version

    def ok_(text):
        """Byte-and-destination clean on every function AND no @NNN renumbering.

        The second half is why fp0 is captured above. A removal can reproduce every
        byte and still move mwccarm's per-TU local numbering, which leaves
        `__sinit_<file>` relocating against names the manifest no longer maps: the real
        link gate calls that WRONG, and a byte-only gate cannot see it at all. Landing
        such a site means re-keying `defined_symbol_renames`, so it is not free and the
        sweep declines it.
        """
        ok_t, _, fp = verify_text(text, path, gated, fp_targets, version, strict)
        return ok_t and fp == fp0

    cur = greedy(base, kinds, ok_, rec)

    # Tidy only what we disturbed. PARENS matches every redundant grouping paren in the
    # tree -- tens of thousands of sites, one compile each -- so it is not a kind you
    # sweep for. Run it after a removal lands, to collapse the `(((x)))` it left behind.
    # Counted separately: it is cosmetic, and folding it into the headline would inflate
    # the number that is supposed to mean "launder sites that were doing nothing".
    if tidy and rec["accepted"]:
        scratch = {"accepted": 0, "rejected": 0, "by_kind": {}, "rejected_kinds": {}}
        cur = greedy(cur, ("PARENS",), ok_, scratch)
        rec["tidied"] = scratch["accepted"]

    rec["status"] = "clean" if rec["rejected"] == 0 else "partial"
    if rec["accepted"]:
        rec["text"] = cur
        low = cur.lower()
        rec["stale_prose"] = ("launder" in low or "u64-mask" in low
                              or "u64 mask" in low or "forces the base" in low)
    return rec


def main():
    require_sweep_deps()
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument("--dry-run", action="store_true", help="report only; write nothing")
    g.add_argument("--apply", action="store_true", help="rewrite files whose removals verified")
    ap.add_argument("--kinds", default="MASK,ROUNDTRIP",
                    help=f"comma-separated subset of {','.join(KINDS)} (default MASK,ROUNDTRIP)")
    ap.add_argument("--limit", type=int, default=0, help="stop after N files (0 = all)")
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--filter", default="", help="only files whose path contains this")
    ap.add_argument("--no-tidy", action="store_true",
                    help="leave the redundant parentheses a removal exposes")
    ap.add_argument("--json", default="", help="write the full record set here")
    args = ap.parse_args()

    kinds = tuple(k.strip().upper() for k in args.kinds.split(",") if k.strip())
    bad = [k for k in kinds if k not in KINDS]
    if bad:
        ap.error(f"unknown kind(s): {', '.join(bad)}")

    idx = module_index()
    targets = []
    for rel, syms in sorted(idx.items()):
        p = REPO / rel
        if args.filter and args.filter not in rel:
            continue
        if p.suffix not in (".c", ".cpp") or not p.is_file():
            continue
        try:
            t = read_source(p)
        except OSError:
            continue
        if find_sites(t, kinds):
            targets.append((rel, syms))
    if args.limit:
        targets = targets[:args.limit]

    gated = sum(len(s) for _, s in targets)
    print(f"delaunder: {len(targets)} enrolled file(s) carry {'/'.join(kinds)} sites, "
          f"-j{args.jobs}, {'DRY RUN' if args.dry_run else 'APPLY'}")
    print(f"           every removal is gated on all {gated} enrolled "
          f"symbol(s) of its file")

    strict = (RA, RA.build_name_index(), RA.build_config_relocs(), RL.load_all_syms())

    recs = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        futs = {ex.submit(sweep_file, rel, syms, kinds, strict, not args.no_tidy): rel
                for rel, syms in targets}
        for n, f in enumerate(concurrent.futures.as_completed(futs), 1):
            rec = f.result()
            recs.append(rec)
            if rec["accepted"] or rec["rejected"]:
                flag = "!" if rec["rejected"] else " "
                print(f"  [{n}/{len(targets)}]{flag} {rec['file']}  "
                      f"-{rec['accepted']} kept {rec['rejected']}")
            elif not rec["status"].startswith(("no-sites", "clean")):
                print(f"  [{n}/{len(targets)}]? {rec['file']}  {rec['status']}")

    if args.apply:
        for rec in recs:
            if rec["text"] is not None:
                write_source(REPO / rec["file"], rec["text"])

    acc = sum(r["accepted"] for r in recs)
    rej = sum(r["rejected"] for r in recs)
    load_bearing = [r for r in recs if r["rejected"]]
    stale = [r for r in recs if r["stale_prose"]]
    skipped = [r for r in recs if r["status"].startswith(("no pin", "baseline", "unreadable"))]

    print("\n--- delaunder summary ---")
    print(f"sites removed (verified free) : {acc}")
    print(f"parens collapsed after removal: {sum(r.get('tidied', 0) for r in recs)}")
    print(f"sites kept (load-bearing)     : {rej}")
    if acc + rej:
        print(f"free rate                     : {100.0 * acc / (acc + rej):.1f}%")
    for k in kinds:
        a = sum(r["by_kind"].get(k, 0) for r in recs)
        d = sum(r["rejected_kinds"].get(k, 0) for r in recs)
        if a + d:
            print(f"  {k:<10} removed {a:>4}  kept {d:>3}  ({100.0 * a / (a + d):.1f}% free)")
    print(f"files fully cleaned           : {sum(1 for r in recs if r['status'] == 'clean' and r['accepted'])}")
    print(f"files with load-bearing sites : {len(load_bearing)}")
    for r in load_bearing:
        print(f"    {r['file']}  kept {r['rejected']}")
    if skipped:
        print(f"files skipped                 : {len(skipped)}")
        for r in skipped[:10]:
            print(f"    {r['file']}  {r['status']}")
    if stale:
        print(f"\nprose still describes a removed idiom ({len(stale)} file(s)) --")
        print("these need a human sentence, and until they get one the langmode metric")
        print("will not move even though the code is clean:")
        for r in stale:
            print(f"    {r['file']}")

    if args.json:
        pathlib.Path(args.json).write_text(
            json.dumps([{k: v for k, v in r.items() if k != "text"} for r in recs], indent=2),
            encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
