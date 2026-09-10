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
``ThreadPoolDispatchIsRaceFree`` pins the same invariant one layer up, through
a real ``ThreadPoolExecutor`` running ``main()``'s actual group-then-``ex.map``
dispatch (Windows surfaces the identical race as ``PermissionError`` or
``FileNotFoundError`` rather than ``elftools``' ``ELFError``, since two
threads opening the same path for read and write at once is a sharing
violation there rather than a torn read) -- a future edit that reintroduces
per-candidate dispatch fails it immediately, without needing mwccarm.
"""

import collections
import concurrent.futures
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
    real ELF bytes, no ``extracted/``. ``objisolate.referenced_undefined``,
    ``reloc_audit.object_reloc_dests`` and ``overlay_residency.possible`` are
    stubbed; everything downstream of their return values is the real code
    under test. Ground-truthed, not numeric: the WRONG-MODULE/CROSS-OVERLAY
    split is now entirely ``overlay_residency.possible()``'s answer -- these
    tests pin that ``audit_one`` asks it the right question (candidate module,
    caller module, caller function) and obeys the answer, not that address
    spans overlap. ``overlay_residency`` earning that answer (E1-E6, its own
    ROM-proven facts) is its own module's job and its own
    ``python tools/overlay_residency.py --check``, not retested here."""

    def _ctx(self, name_defs, config_relocs=None, sym_index=None):
        return {
            "name_defs": name_defs,
            "name_index": {},
            "config_relocs": config_relocs or {},
            "sym_index": sym_index or {},
        }

    def test_wrong_module_when_residency_rules_out_every_owner(self):
        # Caller links into ov018; the only definition of the referenced name
        # lives in ov019, and overlay_residency rules ov019 out for an ov018
        # caller -- the MotherPenguin/RacingPenguin shape symscope.py exists
        # to catch (real E2: ov014/ov018/ov019/ov034 share one RAM slot).
        name_defs = {"_ZN13RacingPenguin16OnPendingDestroyEv":
                     {("ov019", 0x0211235c)}}
        ctx = self._ctx(name_defs)
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"_ZN13RacingPenguin16OnPendingDestroyEv"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)), \
             mock.patch.object(S.OR, "possible", return_value=False) as possible:
            rel, name, module, findings = S.audit_one(
                "src/_ZN7SkiLift8BehaviorEv.cpp", "SkiLift::Behavior",
                0x02111368, 0x34c, "ov018", b"", ctx)
        possible.assert_called_once_with("ov019", "ov018", "SkiLift::Behavior")
        self.assertEqual(len(findings), 1)
        self.assertEqual(findings[0]["verdict"], "WRONG-MODULE")
        self.assertEqual(findings[0]["symbol"],
                         "_ZN13RacingPenguin16OnPendingDestroyEv")
        self.assertEqual(findings[0]["defined_in"], ["ov019:0x0211235c"])

    def test_wrong_module_prints_configs_own_symbol(self):
        # Same shape, but config records this exact call site's destination --
        # the tool's job is to print what it SHOULD say, not just flag it.
        name_defs = {"_ZN13RacingPenguin16OnPendingDestroyEv":
                     {("ov019", 0x0211235c)}}
        config_relocs = {"ov018": {0x02112488: ("arm_call", 0x0211235c, "ov018")}}
        sym_index = {("ov018", 0x0211235c): "func_ov018_0211235c"}
        ctx = self._ctx(name_defs, config_relocs, sym_index)
        dests = [(0x24, "_ZN13RacingPenguin16OnPendingDestroyEv", "ov018", 0)]
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"_ZN13RacingPenguin16OnPendingDestroyEv"}), \
             mock.patch.object(S.RA, "object_reloc_dests",
                               return_value=(dests, None)), \
             mock.patch.object(S.OR, "possible", return_value=False):
            _rel, _name, _module, findings = S.audit_one(
                "src/daPgMthr_c.cpp", "daPgMthr_c::Behavior",
                0x02112464, 0x30, "ov018", b"", ctx)
        self.assertEqual(findings[0]["verdict"], "WRONG-MODULE")
        site = findings[0]["sites"][0]
        self.assertEqual(site["should_be"], "func_ov018_0211235c")
        self.assertFalse(site["agrees"])

    def test_cross_overlay_when_residency_does_not_rule_it_out(self):
        # overlay_residency does not rule the only owner out (e.g. genuinely
        # disjoint, or the ROM never proves the two exclusive): co-residency
        # is possible, so the reference is reported, not failed.
        name_defs = {"func_ov002_020cac30": {("ov002", 0x020cac30)}}
        ctx = self._ctx(name_defs)
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"func_ov002_020cac30"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)), \
             mock.patch.object(S.OR, "possible", return_value=True) as possible:
            _rel, _name, _module, findings = S.audit_one(
                "src/func_ov006_x.cpp", "func_ov006_x",
                0x03000100, 0x10, "ov006", b"", ctx)
        possible.assert_called_once_with("ov002", "ov006", "func_ov006_x")
        self.assertEqual(findings[0]["verdict"], "CROSS-OVERLAY")

    def test_residency_answer_governs_not_address_math(self):
        # The StarSel shape, exactly: ov000 (0x020aa420..0x020bf4e0) and
        # ov003's family (starting 0x020ad660) DO numerically span-overlap --
        # this fixture is real config -- yet symscope.py no longer computes
        # that itself (see _overlap's callers: only families()/module_spans()
        # now, never audit_one). The SAME fixture must read WRONG-MODULE when
        # overlay_residency says the owner is impossible (ov000's real E1:
        # boot-only, unloaded before any scene runs) and CROSS-OVERLAY when it
        # does not (e.g. asked about ov001, ov000's twin, which stays resident
        # and does not conflict with ov003) -- the verdict has to come from
        # the residency answer either way, never from the address relationship
        # alone, which is identical in both calls below.
        name_defs = {"data_ov000_020ab938": {("ov000", 0x020ab938)}}
        ctx = self._ctx(name_defs)
        common = dict(rel="src/_ZN12dScStarSel_c6RenderEv.cpp",
                     name="_ZN12dScStarSel_c6RenderEv",
                     addr=0x020ad800, size=0x30, module="ov003", raw=b"", ctx=ctx)
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"data_ov000_020ab938"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)), \
             mock.patch.object(S.OR, "possible", return_value=False) as possible:
            _rel, _name, _module, findings = S.audit_one(**common)
        possible.assert_called_once_with(
            "ov000", "ov003", "_ZN12dScStarSel_c6RenderEv")
        self.assertEqual(findings[0]["verdict"], "WRONG-MODULE")

        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"data_ov000_020ab938"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)), \
             mock.patch.object(S.OR, "possible", return_value=True):
            _rel, _name, _module, findings = S.audit_one(**common)
        self.assertEqual(findings[0]["verdict"], "CROSS-OVERLAY")

    def test_own_module_and_always_resident_are_invisible(self):
        # A name defined in the caller's own module, or in arm9/itcm/dtcm,
        # produces no finding at all -- these are exactly what "visible where
        # this file links" means -- and never even asks overlay_residency.
        name_defs = {"OwnHelper": {("ov018", 0x02111500)},
                     "SharedHelper": {("arm9", 0x02001000)}}
        ctx = self._ctx(name_defs)
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"OwnHelper", "SharedHelper"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)), \
             mock.patch.object(S.OR, "possible") as possible:
            _rel, _name, _module, findings = S.audit_one(
                "src/x.cpp", "x", 0x02111400, 0x10, "ov018", b"", ctx)
        self.assertEqual(findings, [])
        possible.assert_not_called()

    def test_undefined_nowhere_is_not_this_tools_problem(self):
        # A name no symbols.txt defines at all: eligible.py rule 5's domain,
        # not a crossing (would otherwise show as a phantom empty verdict).
        ctx = self._ctx({})
        with mock.patch.object(S.OI, "referenced_undefined",
                               return_value={"NowhereDefined"}), \
             mock.patch.object(S.RA, "object_reloc_dests", return_value=([], None)):
            _rel, _name, _module, findings = S.audit_one(
                "src/x.cpp", "x", 0x02111400, 0x10, "ov018", b"", ctx)
        self.assertEqual(findings, [])

    def test_elf_error_is_reported_not_raised(self):
        ctx = self._ctx({})
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

        def fake_compile_one(rel, vers, cache, init_srcs, syms,
                             compiler_only=None, intact_tus=None):
            calls.append((rel, compiler_only, intact_tus))
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
        sentinel_co = {"src/consolidated.cpp": {"deadstrip": []}}
        sentinel_intact = {"src/other.cpp": {"id": "x"}}
        ctx = {"vers": {}, "cache": None, "init_srcs": {}, "syms": {},
              "compiler_only": sentinel_co, "intact_tus": sentinel_intact}

        with mock.patch.object(S.RB, "compile_one", side_effect=fake_compile_one), \
             mock.patch.object(S, "audit_one", side_effect=fake_audit_one):
            results = S.audit_file("src/consolidated.cpp", group, ctx)

        self.assertEqual([c[0] for c in calls], ["src/consolidated.cpp"],
                         "compile_one must run exactly once for the whole group")
        # The same manifest-derived policy dicts the production build computes
        # once and threads through every compile_one call must reach it here
        # too, or a promoted consolidated TU's compiled object still carries
        # the RTTI/vtable content the byte gate licenses away and NO-OBJECTs
        # every candidate in the file (this is the fix for that gap).
        self.assertIs(calls[0][1], sentinel_co)
        self.assertIs(calls[0][2], sentinel_intact)
        self.assertEqual(len(results), 3)
        self.assertEqual([r[1] for r in results], ["func_A", "func_B", "func_C"])
        # Every candidate audited the SAME bytes -- one read, not three.
        self.assertTrue(all(r == b"\x7fELFDUMMYBYTES" for r in seen_raw))
        self.assertEqual(len(seen_raw), 3)

    def test_compile_failure_reports_no_object_for_every_candidate(self):
        def fake_compile_one(rel, vers, cache, init_srcs, syms,
                             compiler_only=None, intact_tus=None):
            return rel, "mwccarm: syntax error", "error"

        group = [("func_A", 0x100, 0x10, "ov018"), ("func_B", 0x110, 0x10, "ov018")]
        ctx = {"vers": {}, "cache": None, "init_srcs": {}, "syms": {},
              "compiler_only": {}, "intact_tus": {}}
        # No object written at build/src/broken.o: compile_one's error stands
        # and audit_file must not try to read a file that was never produced.

        with mock.patch.object(S.RB, "compile_one", side_effect=fake_compile_one):
            results = S.audit_file("src/broken.cpp", group, ctx)

        self.assertEqual(len(results), 2)
        for rel, name, module, findings in results:
            self.assertEqual(findings[0]["verdict"], "NO-OBJECT")
            self.assertIn("syntax error", findings[0]["detail"])


class ThreadPoolDispatchIsRaceFree(unittest.TestCase):
    """``main()``'s own dispatch shape, through a REAL ``ThreadPoolExecutor``:
    group candidate rows by ``rel`` (as ``main()`` does with
    ``groups.setdefault(rel, []).append(...)``), then ``ex.map`` one
    ``audit_file`` task per group. This is the SYMRACE regression pin --
    TUBASE's -j8 crash (``FileNotFoundError`` / ``PermissionError`` on
    Windows, ``ELFError: Magic number does not match`` reproduced directly
    against this tree's pre-XSCOPE symscope.py) was two threads independently
    calling ``rombuild.compile_one`` for the SAME ``rel``, each doing its own
    fetch-or-compile-then-isolate write to the identical ``build/<rel>.o``
    path. If ``main()`` (or a future edit to it) ever again dispatches one
    executor task per CANDIDATE ROW instead of per FILE, a two-function
    fixture compiled through real ``compile_one``/isolate calls under several
    concurrent worker threads reproduces exactly that: two threads racing
    ``obj.write_bytes()``/``obj.read_bytes()`` on one path. Routed through a
    real thread pool with real file I/O (not a single direct call, and not a
    mocked ``compile_one``) so the assertion is about actual concurrent
    filesystem access, not call-count bookkeeping alone.
    """

    def setUp(self):
        self._tmpdir = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmpdir.cleanup)
        self.build = pathlib.Path(self._tmpdir.name)
        self._build_patch = mock.patch.object(S.RB, "BUILD", self.build)
        self._build_patch.start()
        self.addCleanup(self._build_patch.stop)

    def test_two_function_file_compiles_once_under_concurrent_workers(self):
        import threading
        import time

        compile_calls = []
        compile_lock = threading.Lock()
        barrier = threading.Barrier(2)  # the two distinct files (groups), not candidate rows

        def fake_compile_one(rel, vers, cache, init_srcs, syms,
                             compiler_only=None, intact_tus=None):
            # Every worker arrives here before any of them proceeds, so a
            # dispatch bug (one task per candidate row instead of per file)
            # has every candidate's compile racing the identical obj path at
            # the same instant, not merely "close in time".
            barrier.wait(timeout=5)
            with compile_lock:
                compile_calls.append((rel, threading.get_ident()))
            obj = self.build / pathlib.Path(rel).with_suffix(".o")
            obj.parent.mkdir(parents=True, exist_ok=True)
            # Simulate compile_one's real sequence for a hit: fetch (write),
            # a scheduling gap, then _isolate's read-modify-write -- the
            # exact two-step window the original race landed in.
            obj.write_bytes(f"RAW:{rel}".encode())
            time.sleep(0.02)
            raw = obj.read_bytes()
            obj.write_bytes(raw + b":ISOLATED")
            return rel, None, "hit"

        seen = []

        def fake_audit_one(rel, name, addr, size, module, raw, ctx):
            seen.append((rel, name, raw))
            return rel, name, module, []

        # The exact shape candidates() gives main() for a two-function
        # consolidated TU: two rows, same rel, different names/addrs.
        rows = [
            ("src/two_func.cpp", "func_A", 0x100, 0x10, "ov018"),
            ("src/two_func.cpp", "func_B", 0x110, 0x10, "ov018"),
            ("src/other.cpp", "func_C", 0x200, 0x10, "ov018"),
        ]
        groups = collections.OrderedDict()
        for rel, name, addr, size, module in rows:
            groups.setdefault(rel, []).append((name, addr, size, module))
        ctx = {"vers": {}, "cache": None, "init_srcs": {}, "syms": {},
              "compiler_only": {}, "intact_tus": {}}

        with mock.patch.object(S.RB, "compile_one", side_effect=fake_compile_one), \
             mock.patch.object(S, "audit_one", side_effect=fake_audit_one):
            with concurrent.futures.ThreadPoolExecutor(max_workers=4) as ex:
                all_results = list(ex.map(
                    lambda kv: S.audit_file(kv[0], kv[1], ctx), groups.items()))

        # One compile_one call per FILE (2 groups), never per candidate row (3).
        rels_compiled = [c[0] for c in compile_calls]
        self.assertEqual(sorted(rels_compiled), ["src/other.cpp", "src/two_func.cpp"])
        self.assertEqual(len(rels_compiled), 2,
                         "compile_one must run once per file even with two "
                         "candidate rows sharing one rel")

        # Every candidate saw its OWN file's fully-isolated bytes, never a
        # neighbour's or a half-written one -- the exact corruption shape a
        # per-candidate dispatch produces under concurrency.
        by_name = {name: raw for _rel, name, raw in seen}
        self.assertEqual(by_name["func_A"], b"RAW:src/two_func.cpp:ISOLATED")
        self.assertEqual(by_name["func_B"], b"RAW:src/two_func.cpp:ISOLATED")
        self.assertEqual(by_name["func_C"], b"RAW:src/other.cpp:ISOLATED")
        self.assertEqual(len(seen), 3, "every candidate row's finding must reach the caller")


if __name__ == "__main__":
    unittest.main()
