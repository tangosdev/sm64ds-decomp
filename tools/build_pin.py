"""The compiler the ROM build will actually use for a file -- and a verifier pinned to it.

## The false green this exists to stop

`tools/match.py` sweeps every installed mwccarm (`match.SWEEP`, 25 builds at the time of
writing) and calls a function matched when ANY of them reproduces it. That is the right
rule for *matching*: the question there is whether the recovered source is the real
thing, and a service pack that schedules two independent instructions the other way
round does not make it less real.

It is the wrong rule for *verifying an edit*. `tools/rombuild.py` compiles each enrolled
file with exactly ONE compiler -- `rombuild.VERSION`, or the per-file override
`config/rombuild-versions.txt` records -- so a file that reproduces only under some
other sweep member links the wrong bytes. A tool that rewrites a file, byte-checks the
rewrite against the sweep and keeps it has verified nothing the build will honour: the
gate reports success for something the build then rejects.

Observed: `src/unnamed/ov006/020c/func_ov006_020cb030.cpp` was rewritten by a transform that byte-verified
it against the sweep. The check passed. The ROM build then reported the overlay
MISMATCHING by three words -- `module fidelity: 105/106`, `ROM-build analysis: FAIL`.

## The rule

`version_for()` reproduces `rombuild.compile_one`'s lookup exactly, keyed on the FILE
STEM because that is what the build keys on. `verify()` compiles with that one version
and with the build's own flags (`rombuild.CFLAGS`, not `match.DEFAULT_FLAGS`): compiling
the check with different flags than the link uses can bless a version the build then
breaks on, which is the same defect by another route -- `tools/rombuild_versions.py`
already had to say so.

Everything fails CLOSED. An unknown pin, a compiler that is not installed, a compile
error, a symbol missing from the object, a reloc-destination check that could not run --
all report "not verified" rather than falling back to a sweep. A gate that answers
"maybe" with "yes" is worse than no gate at all.

Usage:
    python tools/build_pin.py                      # what the build pins, and why
    python tools/build_pin.py --audit              # sweep vs pinned over every candidate
    python tools/build_pin.py --audit --limit 200
"""
import argparse
import collections
import concurrent.futures
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import match as M                                                    # noqa: E402
from rombuild import CFLAGS, MW, VERSION, VERSIONS_FILE, versions    # noqa: E402

# The build's default, re-exported so a caller never has to name a version literal.
DEFAULT_VERSION = VERSION

_pins = None
_mods = None


def pins():
    """{file stem: mwccarm version} the build overrides, or None if unknowable.

    None when `config/rombuild-versions.txt` is absent: the overrides are then not
    merely empty but *unknown*, and every answer derived from them has to fail closed.
    Cached -- this is read once per file in threaded sweeps of eleven thousand.
    """
    global _pins
    if _pins is None:
        _pins = versions() if VERSIONS_FILE.is_file() else False
    return None if _pins is False else _pins


def modules():
    """{label: module registry entry}, with the ARM9 module under the name config uses."""
    global _mods
    if _mods is None:
        import modules as MOD
        _mods = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}
    return _mods


def version_for(rel, func=None):
    """The mwccarm build `rombuild.py` will compile `rel` with, or None if unknown.

    Keyed on the file stem, because `rombuild.compile_one` keys on the file stem
    (`vers.get(pathlib.Path(rel).stem, VERSION)`) -- not on the symbol. The two
    normally coincide, this corpus being one function per file named after it, but
    where they do not it is the stem that decides what the build runs, so it is the
    stem that has to decide what the verifier runs.

    `func` is not used to look anything up. It is used to detect the one way the two
    keyings can disagree: an override recorded under a symbol name whose file is named
    something else is an override the build silently ignores. That is an inconsistent
    pin, and picking a side would just re-manufacture the false green, so it returns
    None and the caller reports the file unverified.
    """
    table = pins()
    if table is None:
        return None
    stem = pathlib.PurePath(str(rel)).stem
    pinned = table.get(stem)
    if pinned is None and func is not None and func != stem and func in table:
        return None
    return pinned or DEFAULT_VERSION


def compiler_for(rel, func=None):
    """(version, None) for the build's compiler, or (None, why not)."""
    v = version_for(rel, func)
    if v is None:
        if pins() is None:
            return None, (f"no pinned build version: {VERSIONS_FILE.name} is missing, so "
                          f"the per-file overrides cannot be read")
        return None, (f"inconsistent pin: {VERSIONS_FILE.name} overrides symbol {func!r} "
                      f"but the build keys on the file stem "
                      f"{pathlib.PurePath(str(rel)).stem!r} and would ignore it")
    exe = MW / v / "mwccarm.exe"
    if not exe.is_file():
        return None, f"pinned compiler {v} is not installed ({exe})"
    return v, None


def flags_for(path, text=None):
    """The build's flags for this file: `rombuild.CFLAGS`, C++ when the file says so.

    The `//cpp` first-line marker is the whole test, matching `rombuild.compile_one`.
    Deliberately NOT `suffix == ".cpp" or ...` as the calling tools used to do: the
    build does not look at the extension, so neither may a check that claims to predict
    the build. (Every committed .cpp does carry the marker, and 53 committed .c files
    carry it too and are compiled as C++ because of it.)
    """
    p = pathlib.Path(str(path))
    if text is None:
        try:
            text = p.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            text = ""
    return CFLAGS.replace("-lang c99", "-lang c++") if text.startswith("//cpp") else CFLAGS


def target_bytes(label, addr, size):
    """The ROM's bytes for a function, given its config module label."""
    if label == "arm9":
        return M.target_bytes(addr, size)
    mod = modules().get(label)
    if mod is None:
        return None
    return M.target_bytes(addr, size, mod["bin"], mod["base"])


def verify(src, func, addr, size, label, strict=None, version=None):
    """Does `src` reproduce `func` under the compiler the ROM build will use?

    Returns (ok, detail). `ok` is True only when the pinned compiler exists, compiles
    the file, emits `func`, reproduces every non-relocated word, and -- when `strict`
    is supplied -- every relocation lands on the destination `config/**/relocs.txt`
    records. Every other outcome is False with a reason; there is no path that returns
    True on a question it could not answer.

    `strict`, when given, is the tuple `(reloc_audit, name_index, config_relocs,
    sym_index)` the callers already build. `version` forces a build for callers that
    are deliberately measuring a different one (the audit below); leave it None.
    """
    src = pathlib.Path(str(src))
    if version is None:
        # Absolute or relative is immaterial -- the lookup is on the stem either way.
        version, why = compiler_for(src, func)
        if version is None:
            return False, why
    tgt = target_bytes(label, addr, size)
    if tgt is None:
        return False, f"no module binary for {label!r}"
    if len(tgt) != size:
        return False, f"ROM bytes for {func} are out of range for {label}"
    obj = M.compile_c(src, version, flags_for(src))
    if obj is None:
        return False, f"does not compile under {version}"
    code, relocs = M.extract_func(obj, func)
    if code is None:
        return False, f"{func} is not in the object {version} produced"
    ok, ndiff = M.compare(tgt, code, relocs, verbose=False)
    if not ok:
        return False, f"{ndiff} word(s) differ under {version}"
    if strict is None:
        return True, version
    RA, name_index, config_relocs, sym_index = strict
    try:
        rows, missing = RA.check_destinations(obj, func, addr, size, label,
                                              name_index, config_relocs, sym_index)
    except Exception as e:                                  # noqa: BLE001
        rows, missing = None, f"reloc-destination check raised: {e}"
    if rows is None:
        return False, f"bytes match under {version} but the reloc-destination check " \
                      f"could not run: {missing}"
    bad = [r for r in rows if r["verdict"] == "WRONG-DEST"]
    if bad:
        return False, (f"bytes match under {version} but {len(bad)} reloc destination(s) "
                       f"are wrong (first: {bad[0]['cand']} != {bad[0]['cfg']})")
    return True, version


def sweep_version(src, func, addr, size, label, versions_=None):
    """The first sweep member that reproduces `func`, or None. The OLD rule.

    Kept because it is what the audit below has to compare against: the number that
    matters is how many files answer "yes" here and "no" to `verify`.
    """
    tgt = target_bytes(label, addr, size)
    if tgt is None:
        return None
    for v in (versions_ or M.SWEEP):
        obj = M.compile_c(src, v, flags_for(src))
        if obj is None:
            continue
        code, relocs = M.extract_func(obj, func)
        if code is None:
            continue
        if M.compare(tgt, code, relocs, verbose=False)[0]:
            return v
    return None


def _audit(args):
    """Every candidate source, pinned-verified, and swept only where that fails.

    The sweep is not run up front: it costs 25 compiles per file and its answer is only
    interesting for a file the pinned build already rejected. A file that fails BOTH is
    not a false green -- it is just an unfinished function, and this says so separately.
    """
    from enroll import candidates
    import rombuild as RB

    class _Sink:
        """match.compile_c narrates every failed compile, and a sweep of eleven
        thousand candidates produces tens of thousands of those lines. Progress goes
        to the real stdout below; this eats the rest."""
        def write(self, _s):
            return 0

        def flush(self):
            pass

    real, sys.stdout = sys.stdout, _Sink()
    try:
        return _audit_inner(args, candidates, RB, real)
    finally:
        sys.stdout = real


def _audit_inner(args, candidates, RB, out):
    enrolled = set(RB.enrolled())
    rows = []
    for (d, name, rel, addr, size, _sec) in candidates()[0]:
        rel = rel.replace("\\", "/")
        label = d.relative_to(REPO / "config").as_posix()
        label = "arm9" if label == "arm9" else label.split("/")[-1]
        rows.append((rel, name, addr, size, label))
    rows.sort()
    if args.limit:
        rows = rows[:args.limit]
    print(f"auditing {len(rows)} candidate source file(s) against the build's pin "
          f"(default {DEFAULT_VERSION}, {len(pins() or {})} override(s))",
          flush=True, file=out)

    def one(row):
        rel, name, addr, size, label = row
        ok, detail = verify(REPO / rel, name, addr, size, label)
        if ok:
            return rel, name, label, "pinned-ok", detail
        alt = sweep_version(REPO / rel, name, addr, size, label)
        if alt is None:
            return rel, name, label, "no-version", detail
        return rel, name, label, "sweep-only", alt

    counts, false_greens = collections.Counter(), []
    done = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, name, label, verdict, detail in ex.map(one, rows):
            counts[verdict] += 1
            if verdict == "sweep-only":
                false_greens.append((rel, name, label, detail, rel in enrolled))
            done += 1
            if done % 500 == 0:
                print(f"  {done}/{len(rows)}  ({counts['sweep-only']} sweep-only so far)",
                      flush=True, file=out)

    print(file=out)
    for k in ("pinned-ok", "sweep-only", "no-version"):
        print(f"  {counts[k]:6d}  {k}", file=out)
    print("", file=out)
    print(f"FALSE GREENS -- reproduce under a sweep version the build will never "
          f"use: {len(false_greens)}", file=out)
    for rel, name, label, v, is_enrolled in sorted(false_greens):
        print(f"  {v:12} {label:7} "
              f"{'ENROLLED' if is_enrolled else 'not enrolled':13} {rel}", file=out)
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--audit", action="store_true",
                    help="compile every candidate source under its pinned build version "
                         "and sweep only the ones that fail, to size the false-green surface")
    ap.add_argument("--limit", type=int)
    ap.add_argument("-j", "--jobs", type=int, default=10)
    ap.add_argument("files", nargs="*", help="report the pin for these source paths")
    args = ap.parse_args()

    if args.audit:
        return _audit(args)
    table = pins()
    print(f"default              : {DEFAULT_VERSION}")
    print(f"overrides            : {VERSIONS_FILE.relative_to(REPO).as_posix()}"
          f" ({'MISSING -- every pin is unknown' if table is None else f'{len(table)} entries'})")
    print(f"build flags          : {CFLAGS}")
    print(f"build flags (//cpp)  : {CFLAGS.replace('-lang c99', '-lang c++')}")
    print(f"sweep (NOT the pin)  : {len(M.SWEEP)} version(s)")
    for f in args.files:
        rel = pathlib.Path(f)
        rel = rel.relative_to(REPO).as_posix() if rel.is_absolute() else rel.as_posix()
        v, why = compiler_for(rel, pathlib.PurePath(rel).stem)
        print(f"  {rel}: {v or 'UNVERIFIABLE -- ' + why}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
