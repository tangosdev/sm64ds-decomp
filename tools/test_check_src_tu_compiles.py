"""Regression tests for tools/check_src_tu_compiles.py.

Every case here plants the breakage and asserts the gate goes red. Two of them are the
shapes this gate exists for and that `check_src_tu.py` provably cannot see:

  * a header retypes a method the way #1583 retyped three `dActor_c` virtuals
    `int` -> `void`. Every `#include` still resolves and every mangled symbol still
    exists, so the reference gate passes; the compiler does not.
  * the pinned compiler is not installed. `tools/test_build_pin.py` and
    `tools/test_tubuild.py` both `return` here and report green having compiled
    nothing. This asserts the opposite.

NO TEST IN THIS FILE SKIPS ITSELF. `test_the_committed_tree_compiles` needs mwccarm and
fails, loudly, if it is missing -- because a compile gate whose test passes without a
compiler is the exact false green the gate is for. Nothing in .github/workflows runs
pytest, so this only ever asks a box that has the toolchain.
"""
import json
import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import build_pin as BP              # noqa: E402
import check_src_tu_compiles as CC  # noqa: E402
import match as M                   # noqa: E402
import rombuild as RB               # noqa: E402

# A header and a TU that agree. `void` both sides.
HEADER_OK = """\
#ifndef PROBE_H
#define PROBE_H
struct Probe {
\tvoid Behavior();
\tint mField;
};
#endif
"""
# The #1583 shape: the header now says `void`, the translation unit still says `int`.
# Nothing is renamed, nothing moves, no include breaks -- only the type drifts.
HEADER_RETYPED = HEADER_OK.replace("\tvoid Behavior();", "\tint Behavior();")
UNIT = """\
//cpp
#include "Probe.h"
void Probe::Behavior() { mField = 1; }
"""


class Scratch:
    """A throwaway TU plus its own manifest, inside build/ so the paths the gate reports
    are expressible as repo-relative -- the same reason test_check_src_tu.py does it."""

    def __enter__(self):
        (REPO / "build").mkdir(exist_ok=True)
        self.dir = pathlib.Path(tempfile.mkdtemp(prefix="src_tu_compiles_",
                                                 dir=REPO / "build"))
        self.rel = self.dir.relative_to(REPO).as_posix()
        return self

    def __exit__(self, *exc):
        shutil.rmtree(self.dir, ignore_errors=True)

    def write(self, name, body):
        (self.dir / name).write_text(body, encoding="utf-8")
        return self

    def manifest(self, *entries):
        p = self.dir / "manifest.json"
        p.write_text(json.dumps({"schema_version": 1, "entries": list(entries)}),
                     encoding="utf-8")
        return p

    def entry(self, name="Probe.cpp", tu_id="probe/Probe"):
        # No `functions`, so resolve_tu_version takes the build default -- which is the
        # right pin for a fixture that corresponds to no ROM address.
        return {"id": tu_id, "source": f"{self.rel}/{name}", "functions": []}

    def check(self, *entries, **kw):
        # The scratch header sits beside the scratch source, and mwccarm does not
        # search a source's own directory -- see compile_one's note on why the real
        # run must not either. A fixture says so explicitly instead.
        kw.setdefault("include_dirs", [self.dir])
        return CC.check(self.manifest(*entries), self.dir, **kw)


def failures_of(report, kind):
    return [f for f in report["failures"] if f["check"] == kind]


class TypeDriftTests(unittest.TestCase):
    """The gap #1667 named and left open."""

    def test_a_tu_whose_header_agrees_with_it_compiles(self):
        """Positive control. Without this the test below could be passing because the
        fixture never compiled in the first place."""
        with Scratch() as s:
            s.write("Probe.h", HEADER_OK).write("Probe.cpp", UNIT)
            report = s.check(s.entry())
            self.assertTrue(report["ok"], report["failures"])
            self.assertEqual(report["checked"]["compiled"], 1)

    def test_a_header_that_retypes_a_method_makes_the_gate_red(self):
        with Scratch() as s:
            s.write("Probe.h", HEADER_RETYPED).write("Probe.cpp", UNIT)
            report = s.check(s.entry())
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "compile")), 1)
            self.assertEqual(report["checked"]["compiled"], 0)

    def test_the_retyped_header_still_passes_the_reference_gate(self):
        """The claim that this gate is not redundant, checked rather than asserted:
        check_src_tu.py sees a resolvable include and no mangled reference at all, so it
        reports the same tree clean. Only the compiler disagrees."""
        import check_src_tu as CR
        with Scratch() as s:
            s.write("Probe.h", HEADER_RETYPED).write("Probe.cpp", UNIT)
            refs = CR.check(s.dir)
            self.assertEqual(failures_of(refs, "includes"), [])
            self.assertEqual(failures_of(refs, "symbols"), [])
            self.assertTrue(CC.check(s.manifest(s.entry()), s.dir,
                                     include_dirs=[s.dir])["failed"])


class ToolchainTests(unittest.TestCase):
    def test_a_missing_compiler_is_a_failure_not_a_skip(self):
        """test_build_pin.py and test_tubuild.py return green here. This must not."""
        with Scratch() as s:
            s.write("Probe.h", HEADER_OK).write("Probe.cpp", UNIT)
            saved = M.MW
            try:
                M.MW = REPO / "build" / "no-such-compiler-root"
                report = s.check(s.entry())
            finally:
                M.MW = saved
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "toolchain")), 1)
            self.assertIn("not installed", failures_of(report, "toolchain")[0]["message"])

    def test_a_missing_licence_is_reported_once_and_up_front(self):
        with Scratch() as s:
            s.write("Probe.h", HEADER_OK).write("Probe.cpp", UNIT)
            saved = M.LICENSE
            try:
                M.LICENSE = REPO / "build" / "no-such-licence.dat"
                report = s.check(s.entry())
            finally:
                M.LICENSE = saved
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "toolchain")), 1)

    def test_the_flags_are_the_builds_flags_not_the_sweeps(self):
        """build_pin.py exists because a check compiled with different flags than the
        link can bless a version the build then breaks on."""
        src = REPO / "src_tu" / "actors" / "PoleLift.cpp"
        flags = BP.flags_for(src)
        self.assertEqual(flags, RB.CFLAGS.replace("-lang c99", "-lang c++"))
        self.assertNotEqual(flags, M.DEFAULT_FLAGS)
        self.assertIn("-Cpp_exceptions off", flags)


class WorkListTests(unittest.TestCase):
    """An empty check is not a pass, and neither is a check over the wrong list."""

    def test_no_translation_units_fails(self):
        with Scratch() as s:
            report = s.check()
            self.assertFalse(report["ok"])
            self.assertTrue(failures_of(report, "worklist"))

    def test_a_source_on_disk_the_manifest_does_not_declare_fails(self):
        """Nothing knows its pin, so nothing compiles it -- and the gate's own count
        would say "1/1 compiled" over a directory holding two."""
        with Scratch() as s:
            s.write("Probe.h", HEADER_OK).write("Probe.cpp", UNIT)
            s.write("Stray.cpp", "//cpp\nint stray() { return 0; }\n")
            report = s.check(s.entry())
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "coverage")), 1)
            self.assertIn("Stray.cpp", failures_of(report, "coverage")[0]["file"])

    def test_a_manifest_entry_with_no_source_on_disk_fails(self):
        with Scratch() as s:
            s.write("Probe.h", HEADER_OK).write("Probe.cpp", UNIT)
            report = s.check(s.entry(), s.entry("Gone.cpp", "probe/Gone"))
            self.assertFalse(report["ok"])
            self.assertTrue(failures_of(report, "coverage"))

    def test_a_declared_source_outside_the_scan_root_is_not_stranded(self):
        """A TU PROMOTED out of src_tu into the real build tree (status "promoted",
        source under src/) is absent from a walk of src_tu while being perfectly
        present -- and the gate compiles it either way, so "one fewer TU checked"
        was false as well. The scan root is a place to look, not the definition of
        existence."""
        with Scratch() as s:
            s.write("Probe.h", HEADER_OK).write("Probe.cpp", UNIT)
            (s.dir / "elsewhere").mkdir()
            report = CC.check(s.manifest(s.entry()), s.dir / "elsewhere",
                              include_dirs=[s.dir])
            self.assertEqual(failures_of(report, "coverage"), [])
            self.assertTrue(report["ok"])
            self.assertEqual(report["checked"]["compiled"], 1)

    def test_a_missing_manifest_fails(self):
        report = CC.check(REPO / "build" / "no-such-manifest.json", REPO / "src_tu")
        self.assertFalse(report["ok"])
        self.assertTrue(failures_of(report, "worklist"))

    def test_an_unknown_id_fails(self):
        with Scratch() as s:
            s.write("Probe.h", HEADER_OK).write("Probe.cpp", UNIT)
            report = s.check(s.entry(), only=("probe/NoSuch",))
            self.assertFalse(report["ok"])
            self.assertTrue(failures_of(report, "worklist"))


class TreeTests(unittest.TestCase):
    def test_the_committed_tree_compiles(self):
        """The baseline. If this is red, a src_tu TU has been stranded by a type change
        -- fix the tree, not the test. It needs mwccarm and does not skip without it:
        see the module docstring."""
        report = CC.check()
        self.assertTrue(report["ok"],
                        [f["message"] for f in report["failures"]][:5])
        self.assertEqual(report["checked"]["units"], report["checked"]["compiled"])
        self.assertGreaterEqual(report["checked"]["units"], 41)

    def test_the_cli_exits_zero_on_the_committed_tree(self):
        r = subprocess.run([sys.executable, str(TOOLS / "check_src_tu_compiles.py"),
                            "--quiet"], capture_output=True, text=True, cwd=REPO)
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)

    def test_the_cli_writes_a_json_report(self):
        with Scratch() as s:
            out = s.dir / "report.json"
            r = subprocess.run([sys.executable, str(TOOLS / "check_src_tu_compiles.py"),
                                "--id", "ov045/PoleLift", "--json", str(out)],
                               capture_output=True, text=True, cwd=REPO)
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
            report = json.loads(out.read_text())
            self.assertTrue(report["ok"])
            self.assertEqual(report["checked"]["units"], 1)
            self.assertEqual(report["results"][0]["version"], BP.DEFAULT_VERSION)


if __name__ == "__main__":
    unittest.main()
