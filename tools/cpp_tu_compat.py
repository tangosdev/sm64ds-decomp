#!/usr/bin/env python3
"""Audit production tooling against one synthetic two-function C++ source.

This is a readiness probe, not a promotion tool.  It creates a disposable repository
shape in a temporary directory and asks the real path-sensitive helpers what they do
with this invariant::

    two ROM function symbols -> one enrolled synthetic source range

The result deliberately distinguishes support from detection.  A tool that notices a
shared source but rewrites it into per-function entries is still a production blocker.
Likewise, a guard that rejects a stale port reference is useful, but the corresponding
port resolver is not TU-compatible until it can resolve a symbol inside a shared file.

The default command reports the current contract and exits zero even while blockers
remain, so it can be used as a living status report.  ``--require-ready`` turns the
known-blocker result into a non-zero gate for the eventual promotion work.
"""
from __future__ import annotations

import argparse
import contextlib
import io
import importlib.util
import json
import pathlib
import sys
import tempfile
from unittest import mock

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

import eligible as EL  # noqa: E402
import enroll as EN  # noqa: E402
import langmode_audit as LM  # noqa: E402
import port_refcheck as PR  # noqa: E402
import rombuild as RB  # noqa: E402
import srcpath as SP  # noqa: E402
import tiers as TI  # noqa: E402
import validate_merge as VM  # noqa: E402


SYMBOLS = ("_ZN4Pair5FirstEv", "_ZN4Pair6SecondEv")
SOURCE = pathlib.PurePosixPath("src", "actors", "Pair.cpp").as_posix()
START = 0x02000000

SYMBOL_TEXT = "".join(
    f"{name} kind:function(arm,size=0x4) addr:0x{START + i * 4:08x}\n"
    for i, name in enumerate(SYMBOLS)
)
DELINKS_TEXT = (
    f"    .text start:0x{START:08x} end:0x{START + 8:08x} kind:code\n\n"
    f"{SOURCE}:\n"
    "    complete\n"
    f"    .text start:0x{START:08x} end:0x{START + 8:08x}\n"
)
SOURCE_TEXT = """//cpp
struct Pair
{
    void First();
    void Second();
};

void Pair::First() {}
void Pair::Second() {}
"""


def _row(surface, status, evidence, consequence):
    return {
        "surface": surface,
        "status": status,
        "evidence": evidence,
        "consequence": consequence,
    }


class _Fixture:
    def __init__(self):
        self._tmp = tempfile.TemporaryDirectory(prefix="cpp-tu-compat-")
        self.repo = pathlib.Path(self._tmp.name)

    def __enter__(self):
        for directory in ("src/actors", "legacy-src", "include", "config/arm9",
                          "port/hal"):
            (self.repo / directory).mkdir(parents=True, exist_ok=True)
        (self.repo / SOURCE).write_text(SOURCE_TEXT, encoding="utf-8")
        # The physical-file metric's before shape.  These files are deliberately
        # outside src/ so they do not contaminate any other surface's merged fixture.
        for i, symbol in enumerate(SYMBOLS):
            method = "First" if i == 0 else "Second"
            (self.repo / "legacy-src" / f"{symbol}.cpp").write_text(
                "//cpp\nstruct Pair { void " + method + "(); };\n"
                "void Pair::" + method + "() {}\n", encoding="utf-8")
        (self.repo / "config/arm9/symbols.txt").write_text(
            SYMBOL_TEXT, encoding="utf-8")
        self.reset_delinks()

        # One valid path manifest plus the empty siblings port_refcheck requires.
        for suffix in PR.SLICE_GATE_SUFFIXES:
            text = SOURCE + "\n" if suffix == PR.SLICE_GATE_SUFFIXES[0] else ""
            (self.repo / "port" / f"slice_gate{suffix}.txt").write_text(
                text, encoding="utf-8")
        (self.repo / "port/CMakeLists.txt").write_text(
            "set(GATE1_SYMS\n  " + "\n  ".join(SYMBOLS) + "\n)\n",
            encoding="utf-8")
        return self

    def __exit__(self, *_exc):
        self._tmp.cleanup()

    def reset_delinks(self):
        (self.repo / "config/arm9/delinks.txt").write_text(
            DELINKS_TEXT, encoding="utf-8")


@contextlib.contextmanager
def _redirect_modules(repo):
    """Point imported helpers at the disposable fixture, then restore every cache."""
    old_repo, _old_src = SP.set_root(repo)
    VM._ENROLMENT_CACHE.clear()
    try:
        with contextlib.ExitStack() as stack:
            values = {
                EN: {"REPO": repo, "CONFIG": repo / "config", "SRC": repo / "src",
                     "MODS": repo / "mods", "EXCLUDE": repo / "config/rombuild-exclude.txt"},
                EL: {"REPO": repo, "INCLUDE": repo / "include", "BUILD": repo / "build",
                     "MW": repo / "tools/mwccarm", "LICENSE": repo / "tools/mwccarm/license.dat"},
                RB: {"REPO": repo, "CONFIG_ROOT": repo / "config"},
                VM: {"REPO": repo},
                LM: {"REPO": repo},
                PR: {"REPO": repo, "PORT": repo / "port",
                     "SRC_DIRS": [repo / "src", repo / "include"]},
            }
            for module, attrs in values.items():
                for name, value in attrs.items():
                    stack.enter_context(mock.patch.object(module, name, value))
            yield
    finally:
        VM._ENROLMENT_CACHE.clear()
        SP.set_root(old_repo)


def _probe_srcpath(repo):
    owners = {
        symbol: SP.path_for(symbol).relative_to(repo).as_posix()
        for symbol in SYMBOLS
    }
    owned = SP.symbols_for(repo / SOURCE)
    ok = set(owners.values()) == {SOURCE} and owned == list(SYMBOLS)
    return _row(
        "srcpath", "ready" if ok else "blocked",
        {"owners": owners, "symbolsForSource": owned},
        "Both symbols resolve through the enrollment table to the shared source."
        if ok else "At least one symbol is invisible once the filename convention stops applying.")


def _probe_enroll(fixture):
    candidates, skipped = EN.candidates()
    shared = [(name, rel) for _mod, name, rel, _addr, _size, _sec in candidates]

    # Exercise the real renderer in the disposable config.  Starting from one complete
    # 8-byte TU range, a safe round trip would preserve one entry and its complete mark.
    with mock.patch.object(sys, "argv", ["enroll.py"]), \
            contextlib.redirect_stdout(io.StringIO()):
        EN.main()
    rendered = (fixture.repo / "config/arm9/delinks.txt").read_text(encoding="utf-8")
    entries = sum(line == f"{SOURCE}:" for line in rendered.splitlines())
    completes = sum(line.strip() == "complete" for line in rendered.splitlines())
    fixture.reset_delinks()

    ok = (len(shared) == 2 and len({rel for _name, rel in shared}) == 1
          and entries == 1 and completes == 1)
    return _row(
        "enroll", "ready" if ok else "blocked",
        {"candidateOwners": shared, "roundTripEntries": entries,
         "roundTripCompleteMarks": completes, "skipped": dict(skipped)},
        "Current candidate discovery sees both members, but regeneration splits the "
        "single TU range and drops its complete mark." if not ok else
        "Candidate discovery and delinks regeneration preserve the shared range.")


class _FakeSection:
    def __init__(self, name, section_type, size):
        self.name = name
        self.header = {"sh_type": section_type, "sh_size": size}


class _FakeSymbol:
    def __init__(self, name):
        self.name = name
        self._data = {
            "st_info": {"bind": "STB_GLOBAL", "type": "STT_FUNC"},
            "st_shndx": 1,
            "st_size": 4,
        }

    def __getitem__(self, key):
        return self._data[key]


class _FakeSymtab(_FakeSection):
    def __init__(self, name):
        super().__init__(".symtab", "SHT_SYMTAB", 1)
        self._symbol = _FakeSymbol(name)

    def iter_symbols(self):
        return iter([self._symbol])


class _FakeElf:
    def __init__(self, name):
        self.text = _FakeSection(".text", "SHT_PROGBITS", 4)
        self.symtab = _FakeSymtab(name)

    def iter_sections(self):
        return iter([self.text, self.symtab])

    def get_section_by_name(self, name):
        return self.symtab if name == ".symtab" else None


def _probe_eligible():
    candidates, _skipped = EN.candidates()
    expected = [name for _mod, name, _rel, _addr, _size, _sec in candidates]
    elf_names = iter(expected)
    isolated = []

    def compile_ok(cmd, **_kwargs):
        out = pathlib.Path(cmd[cmd.index("-o") + 1])
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_bytes(b"ELF")
        return type("Result", (), {"returncode": 0})()

    def isolate(_obj, name):
        isolated.append(name)
        return {}

    with mock.patch.object(EL.subprocess, "run", side_effect=compile_ok), \
            mock.patch.object(EL.OI, "isolate", side_effect=isolate), \
            mock.patch.object(EL.OI, "referenced_undefined", return_value=set()), \
            mock.patch.object(EL, "ELFFile", side_effect=lambda _data: _FakeElf(next(elf_names))):
        results = [EL.classify((rel, name, addr, size, sec, set(SYMBOLS),
                                "2004/b56", True))
                   for _mod, name, rel, addr, size, sec in candidates]

    per_symbol_passes = [name for _rel, name, reason, _missing in results
                         if reason is None]
    mode = EL.placement_mode(expected, isolate=True)
    ready = (isolated == expected and len(per_symbol_passes) == len(SYMBOLS)
             and mode == "derived")
    return _row(
        "eligible", "ready" if ready else "gap",
        {"perSymbolPasses": per_symbol_passes, "isolatedSymbols": isolated,
         "intactObjectVerdicts": 0, "objectMode": mode},
        "Eligibility does not state whether the shared source is derived or intact."
        if not ready else
        "Every member passes and the report declares derived-object placement explicitly.")


def _probe_rombuild(fixture):
    enrolled = RB.enrolled(fixture.repo / "config")
    mapping = RB.enrolled_symbols()
    selected = []
    with mock.patch.object(RB.OI, "isolate_many",
                           side_effect=lambda _obj, name: selected.append(name) or {}):
        RB._isolate(fixture.repo / "Pair.o", SOURCE, mapping)
    ok = (enrolled == [SOURCE] and mapping.get(SOURCE) == list(SYMBOLS)
          and selected == [list(SYMBOLS)])
    return _row(
        "rombuild", "ready" if ok else "blocked",
        {"compiledSources": enrolled, "enrolledSymbolMap": mapping,
         "symbolsKeptByIsolation": selected},
        "The source list or multi-symbol isolation loses at least one shared member."
        if not ok else
        "Compilation groups the members in ROM order and fail-closed isolation preserves "
        "the exact text-only object.")


@contextlib.contextmanager
def _mock_revision(repo):
    paths = ["config/arm9/symbols.txt", "config/arm9/delinks.txt", SOURCE]

    def tree_paths(_rev, prefix=None):
        return [p for p in paths if prefix is None or p.startswith(prefix)]

    def git_text(_rev, path):
        return (repo / path).read_text(encoding="utf-8")

    with mock.patch.object(VM, "tree_paths", side_effect=tree_paths), \
            mock.patch.object(VM, "git_text", side_effect=git_text), \
            mock.patch.object(VM, "_git", return_value=""):
        VM._ENROLMENT_CACHE.clear()
        try:
            yield
        finally:
            VM._ENROLMENT_CACHE.clear()


def _probe_validate_merge(repo):
    with _mock_revision(repo):
        functions = VM.function_snapshot("fixture")
        enrollment = VM.enrollment_snapshot("fixture")
    rows = [functions["functions"][f"arm9:0x{START + i * 4:08x}"]
            for i in range(2)]
    ok = (all(row["matched"] and row["srcPath"] == SOURCE for row in rows)
          and enrollment["stats"]["sourceFunctions"] == 2
          and len(enrollment["source"]) == 1)
    return _row(
        "validate_merge", "ready" if ok else "blocked",
        {"functionOwners": [row["srcPath"] for row in rows],
         "matchedFunctions": functions["stats"]["matchedFunctions"],
         "completeRanges": len(enrollment["source"]),
         "sourceFunctions": enrollment["stats"]["sourceFunctions"]},
        "Merge validation counts both symbols and both functions from one complete range."
        if ok else "Merge validation loses function or source-built coverage after consolidation.")


def _probe_tiers(repo):
    before = TI.converted(repo / "legacy-src")
    after = TI.converted(repo / "src")
    ok = (before["functions"] == after["functions"]
          and before["converted"] == after["converted"])
    return _row(
        "tiers", "ready" if ok else "gap",
        {"legacyFiles": before["source_files"], "legacyFunctions": before["functions"],
         "legacyConverted": before["converted"],
         "mergedFiles": after["source_files"], "mergedFunctions": after["functions"],
         "mergedConverted": after["converted"],
         "fixtureFunctions": len(SYMBOLS)},
        "Physical-file scoring drops both numerator and denominator from two readable "
        "functions to one readable file, so consolidation changes progress without "
        "changing recovered behavior." if not ok else
        "The readability metric preserves function units across source consolidation.")


def _probe_langmode():
    with mock.patch.object(LM, "tracked_sources", return_value=[SOURCE]):
        result = LM.audit()
    mode = result["language_mode"]
    ok = mode["mangled_total"] == len(SYMBOLS)
    return _row(
        "langmode_audit", "ready" if ok else "gap",
        {"cppFiles": result["totals"]["cpp_extension"],
         "mangledDefinitionsCounted": mode["mangled_total"],
         "fixtureFunctions": len(SYMBOLS)},
        "A non-symbol TU filename makes both migrated C++ methods disappear from the "
        "mangled-symbol inventory." if not ok else
        "Every method in the shared source remains represented in language-mode metrics.")


def _probe_attribution(repo):
    with _mock_revision(repo):
        functions = VM.function_snapshot("fixture")
        with mock.patch.object(VM.CHAOS, "first_matchers",
                               return_value={SOURCE: "maintainer"}), \
                mock.patch.object(VM.CHAOS, "match_finishers", return_value={}), \
                mock.patch.object(VM, "_json_at", return_value={
                    "overrides": {SOURCE: "alice", f"{SOURCE}#{SYMBOLS[1]}": "bob"}
                }):
            attribution = VM.attribution_snapshot("fixture", functions)
    authors = {key: row["author"] for key, row in attribution["byFunction"].items()}
    ok = set(authors.values()) == {"alice", "bob"}
    return _row(
        "attribution", "ready" if ok else "gap",
        {"requestedAuthors": list(("alice", "bob")), "resolvedByFunction": authors},
        "Overrides are keyed by the surviving source path, so one override credits every "
        "member and cannot preserve distinct legacy authors." if not ok else
        "Attribution can preserve a distinct author for each TU member.")


def _probe_port_refs(fixture):
    report = PR.build_report()
    cmake = report["checks"]["cmake-symbols"]
    manifests = report["checks"]["manifests"]
    hostgen_path = TOOLS.parent / "port" / "tools" / "hostgen.py"
    spec = importlib.util.spec_from_file_location("cpp_tu_compat_hostgen", hostgen_path)
    hostgen = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(hostgen)
    out_root = fixture.repo / "host-out"
    with mock.patch.object(sys, "argv", [
            "hostgen.py", "--decomp", str(fixture.repo), "--out", str(out_root),
            SYMBOLS[1]]), contextlib.redirect_stdout(io.StringIO()):
        hostgen.main()
    generated = out_root / pathlib.PurePosixPath(SOURCE).with_suffix(".cpp")
    generated_text = generated.read_text(encoding="utf-8") if generated.is_file() else ""
    definitions = sum(name in generated_text for name in ("Pair::First", "Pair::Second"))
    ok = (not cmake["failures"] and not manifests["failures"]
          and generated.is_file() and definitions == 2)
    return _row(
        "port_refcheck", "ready" if ok else "blocked",
        {"manifestFailures": len(manifests["failures"]),
         "cmakeSymbolFailures": len(cmake["failures"]),
         "hostgenOutput": generated.relative_to(fixture.repo).as_posix(),
         "hostgenDefinitions": definitions,
         "messages": [f["message"] for f in cmake["failures"]]},
        "Literal manifests can move to the TU path, but CMake hostgen still resolves each "
        "symbol only as src/<symbol>.c|.cpp." if not ok else
        "Both literal paths and symbol lists resolve through the shared source.")


def audit():
    """Return the deterministic compatibility report as JSON-serializable data."""
    with _Fixture() as fixture, _redirect_modules(fixture.repo):
        rows = [
            _probe_srcpath(fixture.repo),
            _probe_enroll(fixture),
            _probe_eligible(),
            _probe_rombuild(fixture),
            _probe_validate_merge(fixture.repo),
            _probe_tiers(fixture.repo),
            _probe_langmode(),
            _probe_attribution(fixture.repo),
            _probe_port_refs(fixture),
        ]
    blockers = [row["surface"] for row in rows if row["status"] == "blocked"]
    gaps = [row["surface"] for row in rows if row["status"] == "gap"]
    return {
        "schemaVersion": 1,
        "fixture": {"source": SOURCE, "symbols": list(SYMBOLS),
                    "completeRange": f"0x{START:08x}-0x{START + 8:08x}"},
        "productionCompatible": not blockers,
        "allSurfacesReady": not blockers and not gaps,
        "blockers": blockers,
        "policyAndMetricGaps": gaps,
        "surfaces": rows,
    }


def render(report):
    lines = [
        "C++ two-function-TU compatibility: "
        + ("BLOCKED" if report["blockers"] else
           "GAPS REMAIN" if report["policyAndMetricGaps"] else "READY"),
        f"fixture: {report['fixture']['source']} owns "
        f"{', '.join(report['fixture']['symbols'])}",
        "",
    ]
    for row in report["surfaces"]:
        lines.append(f"[{row['status'].upper():7}] {row['surface']}: "
                     f"{row['consequence']}")
    if report["blockers"]:
        lines += ["", "hard production blockers: " + ", ".join(report["blockers"])]
    if report["policyAndMetricGaps"]:
        lines += ["policy/metric gaps: " + ", ".join(report["policyAndMetricGaps"])]
    return "\n".join(lines)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json", action="store_true", help="write the full report as JSON")
    parser.add_argument("--require-ready", action="store_true",
                        help="exit 1 while any compatibility blocker or gap remains")
    args = parser.parse_args(argv)
    report = audit()
    print(json.dumps(report, indent=2) if args.json else render(report))
    return 1 if args.require_ready and not report["allSurfacesReady"] else 0


if __name__ == "__main__":
    raise SystemExit(main())
