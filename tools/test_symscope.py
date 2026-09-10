"""Toolchain-free coverage for symscope.py's classification and dispatch.

WHY THIS EXISTS. symscope.py had no test module at all. Its central verdict
(WRONG-MODULE vs CROSS-OVERLAY) is pure set/span logic once the compiled
object's undefined-reference list and relocation destinations are known, so
that half is testable on a bare runner by stubbing ``objisolate`` and
``reloc_audit`` the way ``test_tubuild_owned_relocs.py`` stubs
``section_contribution``: no mwccarm, no ``extracted/``, no ``config/``.

THE RACE THIS ALSO PINS. A consolidated TU enrolls many candidate functions
under one source file, and ``main()`` used to submit one ``ThreadPoolExecutor``
task per CANDIDATE, each independently calling ``rombuild.compile_one`` for
its (shared) ``rel``. Two candidates from the same file landing on different
threads raced the identical ``build/<rel>.o`` path -- one thread's cache-fetch
or ``_isolate`` write could land mid-read of another thread's read of the same
bytes, surfacing as ``elftools...ELFError: Magic number does not match`` at
-j8 and -j12 (the tool's own default). Reproduced against the real tree:
single-threaded (-j1) it never happens; -j8/-j12 crash near the same
candidate every time, always a file enrolling many functions. The fix groups
candidate rows by ``rel`` before dispatch (``audit_file``), compiling each
FILE exactly once regardless of how many candidates it enrolls -- correctness
here, not just speed. ``AuditFileCompilesOnce`` pins that a group of N
candidate rows for one file triggers exactly one ``compile_one`` call and one
``Path.read_bytes``, and that every row's finding reaches the caller.
"""

import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import symscope as S  # noqa: E402


class DestModules(unittest.TestCase):
    """``_dest_modules``: dsd's plural ``overlays(a,b,...)`` spelling, and the
    ordinary single-module case that falls through to ``relocs.normalize_module``."""

    def test_plural_expands_every_member(self):
        self.assertEqual(S._dest_modules("overlays(2,7)"), {"ov002", "ov007"})

    def test_plural_single_member(self):
        self.assertEqual(S._dest_modules("overlays(0,1)"), {"ov000", "ov001"})

    def test_singular_normalizes(self):
        self.assertEqual(S._dest_modules("ov018"), {"ov018"})
        self.assertEqual(S._dest_modules("main"), {"arm9"})


class Overlap(unittest.TestCase):
    """``_overlap``: half-open interval intersection, the primitive every
    WRONG-MODULE/CROSS-OVERLAY split is built on."""

    def test_overlapping(self):
        self.assertTrue(S._overlap((0x1000, 0x2000), (0x1800, 0x2800)))

    def test_disjoint(self):
        self.assertFalse(S._overlap((0x1000, 0x2000), (0x2000, 0x3000)))

    def test_nested(self):
        self.assertTrue(S._overlap((0x1000, 0x9000), (0x2000, 0x3000)))


class AuditOneVerdicts(unittest.TestCase):
    """``audit_one``'s classification, over a hand-built ctx -- no compiler, no
    real ELF bytes. ``objisolate.referenced_undefined`` and
    ``reloc_audit.object_reloc_dests`` are stubbed; everything downstream of
    their return values is the real code under test."""

    def _ctx(self, spans, name_defs, config_relocs=None, sym_index=None):
        return {
            "spans": spans,
            "name_defs": name_defs,
            "name_index": {},
            "config_relocs": config_relocs or {},
            "sym_index": sym_index or {},
        }

    def test_wrong_module_never_coresident(self):
        # Caller links into ov018 (0x02111000..0x02113000); the only definition
        # of the referenced name lives in ov019, whose span overlaps ov018's --
        # the MotherPenguin/RacingPenguin shape symscope.py exists to catch.
        spans = {"ov018": (0x02111000, 0x02113000),
                 "ov019": (0x02111000, 0x02113000)}
        name_defs = {"_ZN13RacingPenguin16OnPendingDestroyEv":
                     {("ov019", 0x0211235c)}}
        ctx = self._ctx(spans, name_defs)
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"_ZN13RacingPenguin16OnPendingDestroyEv"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)):
            rel, name, module, findings = S.audit_one(
                "src/_ZN7SkiLift8BehaviorEv.cpp", "SkiLift::Behavior",
                0x02111368, 0x34c, "ov018", b"", ctx)
        self.assertEqual(len(findings), 1)
        self.assertEqual(findings[0]["verdict"], "WRONG-MODULE")
        self.assertEqual(findings[0]["symbol"],
                         "_ZN13RacingPenguin16OnPendingDestroyEv")
        self.assertEqual(findings[0]["defined_in"], ["ov019:0x0211235c"])

    def test_wrong_module_prints_configs_own_symbol(self):
        # Same shape, but config records this exact call site's destination --
        # the tool's job is to print what it SHOULD say, not just flag it.
        spans = {"ov018": (0x02111000, 0x02113000),
                 "ov019": (0x02111000, 0x02113000)}
        name_defs = {"_ZN13RacingPenguin16OnPendingDestroyEv":
                     {("ov019", 0x0211235c)}}
        config_relocs = {"ov018": {0x02112488: ("arm_call", 0x0211235c, "ov018")}}
        sym_index = {("ov018", 0x0211235c): "func_ov018_0211235c"}
        ctx = self._ctx(spans, name_defs, config_relocs, sym_index)
        dests = [(0x24, "_ZN13RacingPenguin16OnPendingDestroyEv", "ov018", 0)]
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"_ZN13RacingPenguin16OnPendingDestroyEv"}), \
             mock.patch.object(S.RA, "object_reloc_dests",
                               return_value=(dests, None)):
            _rel, _name, _module, findings = S.audit_one(
                "src/daPgMthr_c.cpp", "daPgMthr_c::Behavior",
                0x02112464, 0x30, "ov018", b"", ctx)
        self.assertEqual(findings[0]["verdict"], "WRONG-MODULE")
        site = findings[0]["sites"][0]
        self.assertEqual(site["should_be"], "func_ov018_0211235c")
        self.assertFalse(site["agrees"])

    def test_cross_overlay_disjoint_spans_can_coload(self):
        # Two overlays with genuinely disjoint spans: co-resident is possible,
        # so a reference between them is reported, not failed.
        spans = {"ov006": (0x03000000, 0x03100000),
                 "ov002": (0x020ad660, 0x021111a0)}
        name_defs = {"func_ov002_020cac30": {("ov002", 0x020cac30)}}
        ctx = self._ctx(spans, name_defs)
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"func_ov002_020cac30"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)):
            _rel, _name, _module, findings = S.audit_one(
                "src/func_ov006_x.cpp", "func_ov006_x",
                0x03000100, 0x10, "ov006", b"", ctx)
        self.assertEqual(findings[0]["verdict"], "CROSS-OVERLAY")

    def test_own_module_and_always_resident_are_invisible(self):
        # A name defined in the caller's own module, or in arm9/itcm/dtcm,
        # produces no finding at all -- these are exactly what "visible where
        # this file links" means.
        spans = {"ov018": (0x02111000, 0x02113000)}
        name_defs = {"OwnHelper": {("ov018", 0x02111500)},
                     "SharedHelper": {("arm9", 0x02001000)}}
        ctx = self._ctx(spans, name_defs)
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"OwnHelper", "SharedHelper"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)):
            _rel, _name, _module, findings = S.audit_one(
                "src/x.cpp", "x", 0x02111400, 0x10, "ov018", b"", ctx)
        self.assertEqual(findings, [])

    def test_undefined_nowhere_is_not_this_tools_problem(self):
        # A name no symbols.txt defines at all: eligible.py rule 5's domain,
        # not a crossing (would otherwise show as a phantom empty verdict).
        ctx = self._ctx({"ov018": (0x02111000, 0x02113000)}, {})
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"NowhereDefined"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)):
            _rel, _name, _module, findings = S.audit_one(
                "src/x.cpp", "x", 0x02111400, 0x10, "ov018", b"", ctx)
        self.assertEqual(findings, [])

    def test_elf_error_is_reported_not_raised(self):
        ctx = self._ctx({}, {})
        with mock.patch.object(S.OI, "referenced_undefined",
                               side_effect=ValueError("bad elf")):
            _rel, _name, _module, findings = S.audit_one(
                "src/x.cpp", "x", 0x02111400, 0x10, "ov018", b"", ctx)
        self.assertEqual(findings, [{"verdict": "ELF-ERROR", "detail": "ValueError"}])


class AuditFileCompilesOnce(unittest.TestCase):
    """``audit_file``: one ``compile_one`` and one ``read_bytes`` per FILE, no
    matter how many candidate rows the file enrolls -- the fix for the
    ThreadPoolExecutor race described at the top of this module. Uses a real
    temp directory for ``RB.BUILD`` (not a mocked ``pathlib.Path``) so the
    real ``obj.is_file()`` / ``obj.read_bytes()`` calls in ``audit_file`` run
    unmodified; only ``compile_one`` and ``audit_one`` are stubbed."""

    def setUp(self):
        self._tmpdir = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmpdir.cleanup)
        self.build = pathlib.Path(self._tmpdir.name)
        self._build_patch = mock.patch.object(S.RB, "BUILD", self.build)
        self._build_patch.start()
        self.addCleanup(self._build_patch.stop)

    def test_group_of_three_shares_one_compile(self):
        calls = []
        obj_path = self.build / "src/consolidated.o"
        obj_path.parent.mkdir(parents=True, exist_ok=True)
        obj_path.write_bytes(b"\x7fELFDUMMYBYTES")

        def fake_compile_one(rel, vers, cache, init_srcs, syms):
            calls.append(rel)
            return rel, None, "hit"

        seen_raw = []

        def fake_audit_one(rel, name, addr, size, module, raw, ctx):
            seen_raw.append(raw)
            return rel, name, module, [{"verdict": "WRONG-MODULE", "symbol": name}]

        group = [
            ("func_A", 0x100, 0x10, "ov018"),
            ("func_B", 0x110, 0x10, "ov018"),
            ("func_C", 0x120, 0x10, "ov018"),
        ]
        ctx = {"vers": {}, "cache": None, "init_srcs": {}, "syms": {}}

        with mock.patch.object(S.RB, "compile_one", side_effect=fake_compile_one), \
             mock.patch.object(S, "audit_one", side_effect=fake_audit_one):
            results = S.audit_file("src/consolidated.cpp", group, ctx)

        self.assertEqual(calls, ["src/consolidated.cpp"],
                         "compile_one must run exactly once for the whole group")
        self.assertEqual(len(results), 3)
        self.assertEqual([r[1] for r in results], ["func_A", "func_B", "func_C"])
        # Every candidate audited the SAME bytes -- one read, not three.
        self.assertTrue(all(r == b"\x7fELFDUMMYBYTES" for r in seen_raw))
        self.assertEqual(len(seen_raw), 3)

    def test_compile_failure_reports_no_object_for_every_candidate(self):
        def fake_compile_one(rel, vers, cache, init_srcs, syms):
            return rel, "mwccarm: syntax error", "error"

        group = [("func_A", 0x100, 0x10, "ov018"), ("func_B", 0x110, 0x10, "ov018")]
        ctx = {"vers": {}, "cache": None, "init_srcs": {}, "syms": {}}
        # No object written at build/src/broken.o: compile_one's error stands
        # and audit_file must not try to read a file that was never produced.

        with mock.patch.object(S.RB, "compile_one", side_effect=fake_compile_one):
            results = S.audit_file("src/broken.cpp", group, ctx)

        self.assertEqual(len(results), 2)
        for rel, name, module, findings in results:
            self.assertEqual(findings[0]["verdict"], "NO-OBJECT")
            self.assertIn("syntax error", findings[0]["detail"])


if __name__ == "__main__":
    unittest.main()
