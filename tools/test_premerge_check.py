#!/usr/bin/env python3
"""Tests for tools/premerge_check.py.

EVERY TEST IN HERE RUNS WITH NO GIT, NO `gh`, NO COMPILER AND NO `extracted/`.
That is a requirement of the suite, not an accident of what was easy to write. This
repository has shipped a gate that reported a clean tree after its match count had
silently gone to zero, and the general shape of that defect is a test file that
self-`return`s when its inputs are missing: the suite passes, CI is green, and the
thing under test has not been exercised at all. So the logic that decides the exit
code -- the verdict diff, the coverage delta, the merge-tree parse and the PR
resolution failures -- is written as pure functions over data, and tested as such.

The four tests that genuinely need a git binary call `self.skipTest` and say why.
A real skip is visible in the runner's output. A bare `return` is not, and would let
this file report a pass on a machine where it checked nothing.

    python -m unittest tools.test_premerge_check -v
"""
import json
import pathlib
import subprocess
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import premerge_check as P  # noqa: E402


class ClassifyTests(unittest.TestCase):
    """The four verdict pairs. This is the whole exit-code contract."""

    def test_green_green_is_ok(self):
        self.assertEqual(P.classify("pass", "pass"), "ok")

    def test_green_red_is_the_regression(self):
        self.assertEqual(P.classify("pass", "fail"), "REGRESSION")

    def test_red_red_is_not_this_prs_fault(self):
        # The load-bearing one. A gate already broken on the base must never be
        # charged to a PR that merely merges against it.
        self.assertEqual(P.classify("fail", "fail"), "pre-existing")

    def test_red_green_is_a_fix(self):
        self.assertEqual(P.classify("fail", "pass"), "fixed")

    def test_error_counts_as_red_on_the_merge_side(self):
        # A gate that could not RUN on the merge result is a fact about the merge
        # result. Treating exit 2 as a pass is how a gate stops meaning anything.
        self.assertEqual(P.classify("pass", "error"), "REGRESSION")

    def test_error_on_both_sides_is_pre_existing(self):
        self.assertEqual(P.classify("error", "error"), "pre-existing")


class DiffVerdictTests(unittest.TestCase):
    def test_rows_cover_the_union_and_only_green_red_regresses(self):
        base = {"a": {"status": "pass"}, "b": {"status": "fail"},
                "c": {"status": "fail"}, "d": {"status": "pass"}}
        merge = {"a": {"status": "pass"}, "b": {"status": "fail"},
                 "c": {"status": "pass"}, "d": {"status": "fail"}}
        rows = P.diff_verdicts(base, merge)
        got = {r["gate"]: r["verdict"] for r in rows}
        self.assertEqual(got, {"a": "ok", "b": "pre-existing",
                               "c": "fixed", "d": "REGRESSION"})
        self.assertEqual([r["gate"] for r in P.regressions(rows)], ["d"])

    def test_a_gate_that_vanishes_from_the_merge_result_is_a_regression(self):
        # `absent` is not `pass`. A gate dropping out of the merge result's run --
        # because the tool it names was deleted by one of the merged commits -- is a
        # thing to be told about, not a row that quietly disappears from the table.
        rows = P.diff_verdicts({"a": {"status": "pass"}}, {})
        self.assertEqual(rows, [{"gate": "a", "base": "pass", "merge": "absent",
                                 "verdict": "REGRESSION"}])

    def test_a_gate_only_the_merge_result_has_is_not_a_regression(self):
        rows = P.diff_verdicts({}, {"a": {"status": "pass"}})
        self.assertEqual(rows[0]["verdict"], "fixed")

    def test_no_gates_at_all_yields_no_regressions(self):
        self.assertEqual(P.diff_verdicts({}, {}), [])


class CoverageLineTests(unittest.TestCase):
    """The verdict is source_coverage.py's byte-SET subtraction, not a byte COUNT.

    Every fixture here is the shape `source_coverage.py --check --json` actually
    emits, because that document -- not a pair of totals -- is what decides the row.
    """

    FLAT = {"baseBytes": 2067148, "currentBytes": 2067148, "netBytes": 0,
            "lostBytes": 0, "unwaivedLostBytes": 0, "gainedBytes": 0,
            "bySection": {".text": 1941436, ".init": 125712},
            "lost": [], "waived": [], "gained": []}

    # The measured case that motivated the rework. One .rodata range in
    # config/arm9/delinks.txt relocated by +0x400000: 71,984 B of claimed coverage
    # moved to an address the cartridge does not have. `currentBytes` and every
    # `bySection` total are IDENTICAL on both sides -- a count cannot see it, and the
    # count-based check this replaced called it `ok`.
    RELOCATED = {
        "baseBytes": 2067148, "currentBytes": 2067148, "netBytes": 0,
        "lostBytes": 71984, "unwaivedLostBytes": 71984, "gainedBytes": 71984,
        "bySection": {".text": 1941436, ".init": 125712},
        "lost": [{"module": "arm9", "section": ".rodata", "start": "0x2085000",
                  "end": "0x2096900", "bytes": 71984,
                  "owners": [{"path": "src/data/rodata.c",
                              "why": "range narrowed or re-partitioned; these bytes "
                                     "are no longer covered"}]}],
        "waived": [], "gained": []}

    def test_a_flat_comparison_produces_no_lines(self):
        self.assertEqual(P.coverage_lines(self.FLAT), [])

    def test_an_unmeasurable_side_produces_no_lines(self):
        # None findings means the check did not run. That is an ERROR, handled by
        # coverage_row(); it must never be dressed up here as "nothing lost".
        self.assertEqual(P.coverage_lines(None), [])

    def test_relocation_with_identical_totals_is_still_a_loss(self):
        # THE regression test for the count-vs-set defect. Every byte COUNT in
        # RELOCATED matches FLAT, and it is still 71,984 B handed back.
        self.assertEqual(self.RELOCATED["currentBytes"], self.FLAT["currentBytes"])
        self.assertEqual(self.RELOCATED["bySection"], self.FLAT["bySection"])
        lines = P.coverage_lines(self.RELOCATED)
        self.assertTrue(lines)
        self.assertIn("71,984 B", lines[0])
        self.assertTrue(any(".rodata" in ln for ln in lines))

    def test_the_owner_and_the_reason_are_carried_through(self):
        # A byte count is not actionable. source_coverage.py's own blame() says which
        # entry owned the range and which of the four things happened to it.
        lines = P.coverage_lines(self.RELOCATED)
        self.assertTrue(any("src/data/rodata.c" in ln for ln in lines))
        self.assertTrue(any("re-partitioned" in ln for ln in lines))

    def test_a_waived_loss_is_not_a_loss(self):
        # The waiver file CI honours is honoured here, because it is the same run of
        # the same tool: unwaivedLostBytes decides, lostBytes does not.
        f = json.loads(json.dumps(self.RELOCATED))
        f["unwaivedLostBytes"] = 0
        f["waived"] = [{"module": "arm9", "section": ".rodata", "bytes": 71984,
                        "reason": "moved to overlay 4 in #1234"}]
        f["lost"] = []
        self.assertEqual(P.coverage_lines(f), [])

    def test_waivers_are_mentioned_when_something_else_still_regressed(self):
        f = json.loads(json.dumps(self.RELOCATED))
        f["waived"] = [{"module": "arm9", "section": ".text", "bytes": 16,
                        "reason": "known"}]
        self.assertTrue(any("waived" in ln for ln in P.coverage_lines(f)))

    def test_long_range_lists_are_truncated_with_a_count(self):
        f = json.loads(json.dumps(self.RELOCATED))
        f["lost"] = f["lost"] * 30
        lines = P.coverage_lines(f, limit=3)
        self.assertTrue(any("and 27 more range(s)" in ln for ln in lines))

    def test_entries_and_modules_can_never_produce_a_line(self):
        # A TU promotion moves `entries` and nothing else. It must not reach the
        # verdict path -- and it cannot, because the verdict path never reads those
        # keys: they are not in a --check document at all.
        f = json.loads(json.dumps(self.FLAT))
        f["entries"] = 11012
        f["modules"] = 73
        self.assertEqual(P.coverage_lines(f), [])


class CoverageRowTests(unittest.TestCase):
    """Coverage goes through classify() like every other gate. It used to not."""

    def test_pass_on_both_sides_is_ok(self):
        row = P.coverage_row({"status": "pass"}, {"status": "pass"})
        self.assertEqual(row["verdict"], "ok")
        self.assertEqual(row["gate"], P.COVERAGE_GATE)

    def test_a_byte_loss_on_the_merge_result_is_the_regression(self):
        self.assertEqual(
            P.coverage_row({"status": "pass"}, {"status": "fail"})["verdict"],
            "REGRESSION")

    def test_unmeasurable_on_the_merge_result_is_a_REGRESSION_not_a_pass(self):
        # The defect this replaced: source_coverage.py failing on the merge result
        # produced `outcome: clean`, `exit: 0` and the word "error" in a column that
        # fed into nothing. A gate that could not run is not a gate that passed --
        # classify()'s own docstring says so, and coverage was the one gate routed
        # around it.
        row = P.coverage_row({"status": "pass"}, {"status": "error"})
        self.assertEqual(row["verdict"], "REGRESSION")
        self.assertEqual([r["gate"] for r in P.regressions([row])],
                         [P.COVERAGE_GATE])

    def test_unmeasurable_on_BOTH_sides_is_pre_existing_and_blames_nobody(self):
        # A source_coverage.py already broken on main -- or a malformed waiver file
        # there -- must not be charged to a PR that merely merges against it. That is
        # exactly what the base-vs-base self-check is run for.
        row = P.coverage_row({"status": "error"}, {"status": "error"})
        self.assertEqual(row["verdict"], "pre-existing")
        self.assertEqual(P.regressions([row]), [])

    def test_a_missing_side_is_absent_not_a_pass(self):
        self.assertEqual(P.coverage_row({"status": "pass"}, None)["verdict"],
                         "REGRESSION")


class CoverageContextTests(unittest.TestCase):
    """`entries`/`modules` are reported, and can never fail a run."""

    BASE = {"currentBytes": 2066772, "entries": 11025, "modules": 74,
            "bySection": {".text": 1941060, ".init": 125712}}

    def test_flat_numbers_produce_no_noise(self):
        self.assertEqual(P.coverage_context(self.BASE, dict(self.BASE)), [])

    def test_an_entry_drop_with_flat_bytes_is_labelled_a_consolidation(self):
        m = json.loads(json.dumps(self.BASE))
        m["entries"] = 11017
        lines = P.coverage_context(self.BASE, m, byte_regressions=[])
        self.assertEqual(len(lines), 1)
        self.assertIn("11025 -> 11017", lines[0])
        self.assertIn("-8", lines[0])
        self.assertIn("consolidation", lines[0])

    def test_the_reassurance_is_withdrawn_when_bytes_fell_too(self):
        # If the byte SET really shrank, the entry drop is corroborating a loss and
        # must not be dressed up as a consolidation. The byte lines now come from
        # source_coverage.py's own --check document, not from a count subtraction.
        m = json.loads(json.dumps(self.BASE))
        m["entries"] = 11017
        byte_lines = P.coverage_lines(CoverageLineTests.RELOCATED)
        self.assertTrue(byte_lines)
        lines = P.coverage_context(self.BASE, m, byte_regressions=byte_lines)
        self.assertEqual(len(lines), 1)
        self.assertNotIn("consolidation", lines[0])

    def test_growth_is_reported_without_a_consolidation_note(self):
        m = json.loads(json.dumps(self.BASE))
        m["entries"] += 12
        lines = P.coverage_context(self.BASE, m)
        self.assertEqual(len(lines), 1)
        self.assertIn("+12", lines[0])
        self.assertNotIn("consolidation", lines[0])

    def test_unmeasured_side_reports_nothing(self):
        self.assertEqual(P.coverage_context(None, self.BASE), [])
        self.assertEqual(P.coverage_context(self.BASE, None), [])

    def test_the_context_keys_are_exactly_the_two_that_cannot_fail_a_run(self):
        # If a byte key appeared here it could fail a run through the verdict path
        # while being explained away as context in the same table. There is no byte
        # key list any more: the verdict is source_coverage.py's, and this is info.
        self.assertEqual(set(P.COVERAGE_CONTEXT_KEYS), {"entries", "modules"})
        self.assertFalse(hasattr(P, "COVERAGE_BYTE_KEYS"),
                         "a count-based byte comparison came back")


class MergeTreeParseTests(unittest.TestCase):
    OID = "c40335b6578cecbd313b241d50fc5fab8ca6fa12"

    def test_clean_merge_yields_a_tree_and_no_conflicts(self):
        tree, conflicts = P.parse_merge_tree(0, self.OID + "\n")
        self.assertEqual(tree, self.OID)
        self.assertEqual(conflicts, [])

    def test_conflict_is_its_own_outcome_not_a_gate_failure(self):
        out = (self.OID + "\n"
               "100644 aaa 1\tsrc/Foo.cpp\n"
               "100644 bbb 2\tsrc/Foo.cpp\n"
               "\nAuto-merging src/Foo.cpp\nCONFLICT (content): src/Foo.cpp\n")
        tree, conflicts = P.parse_merge_tree(1, out)
        self.assertEqual(tree, self.OID)
        self.assertTrue(conflicts)
        self.assertTrue(any("CONFLICT" in c for c in conflicts))

    def test_conflict_with_no_detail_still_reports_a_conflict(self):
        tree, conflicts = P.parse_merge_tree(1, self.OID + "\n")
        self.assertEqual(conflicts, ["conflict (no detail reported)"])

    def test_git_refusing_to_answer_is_neither_clean_nor_a_conflict(self):
        # conflicts is None -- the caller must report a TOOL ERROR. Reporting an
        # unresolvable ref as "this PR conflicts" would be a lie about the PR.
        tree, conflicts = P.parse_merge_tree(128, "")
        self.assertIsNone(tree)
        self.assertIsNone(conflicts)

    def test_exit_zero_with_no_oid_is_not_silently_a_clean_merge(self):
        tree, conflicts = P.parse_merge_tree(0, "")
        self.assertIsNone(tree)
        self.assertTrue(conflicts)


class PrNumberTests(unittest.TestCase):
    def test_what_counts_as_a_pr_number(self):
        for spec in ("2001", "#2001", " 2001 "):
            self.assertTrue(P.is_pr_number(spec), spec)
        for spec in ("origin/main", "487ba140e", "HEAD", "v1.2", "20a1"):
            self.assertFalse(P.is_pr_number(spec), spec)


class _Runner:
    """A stand-in for subprocess.run, so the gh failure paths need no gh."""

    def __init__(self, *, raises=None, rc=0, stdout="", stderr=""):
        self.raises, self.rc = raises, rc
        self.stdout, self.stderr = stdout, stderr
        self.calls = []

    def __call__(self, argv, **kw):
        self.calls.append(argv)
        self.kwargs = kw
        if self.raises:
            raise self.raises
        return subprocess.CompletedProcess(argv, self.rc, self.stdout, self.stderr)


class ResolvePrTests(unittest.TestCase):
    """Every one of these is a sentence a person reads, not a traceback."""

    OPEN = {"headRefOid": "a" * 40, "title": "Convert daKrb_c",
            "state": "OPEN", "baseRefName": "main"}

    def test_success_returns_the_head_sha_a_label_and_the_prs_state(self):
        r = _Runner(stdout=json.dumps(self.OPEN))
        sha, label, info = P.resolve_pr("#2001", runner=r)
        self.assertEqual(sha, "a" * 40)
        self.assertIn("#2001", label)
        self.assertIn("daKrb_c", label)
        self.assertEqual(r.calls[0][:4], ["gh", "pr", "view", "2001"])
        self.assertEqual(info, {"state": "OPEN", "baseRefName": "main"})

    def test_gh_is_run_IN_the_repository_being_gated(self):
        # `gh pr view` with no cwd resolves the PR against whatever repository the
        # shell is sitting in, which made the documented --repo flag a lie for the
        # primary usage mode and turned a wrong-directory failure into an auth
        # complaint.
        r = _Runner(stdout=json.dumps(self.OPEN))
        P.resolve_pr("2001", runner=r, repo="/some/clone")
        self.assertEqual(r.kwargs.get("cwd"), "/some/clone")

    def test_the_failure_message_names_the_directory_it_asked_from(self):
        r = _Runner(rc=1, stderr="could not determine base repo\n")
        with self.assertRaises(ValueError) as ctx:
            P.resolve_pr("2001", runner=r, repo="/some/clone")
        self.assertIn("/some/clone", str(ctx.exception))
        self.assertIn("git remotes", str(ctx.exception))

    def test_gh_not_installed_says_so_and_offers_the_way_round_it(self):
        r = _Runner(raises=FileNotFoundError())
        with self.assertRaises(ValueError) as ctx:
            P.resolve_pr("2001", runner=r)
        msg = str(ctx.exception)
        self.assertIn("not on PATH", msg)
        self.assertIn("head sha", msg)   # the workaround, not just the complaint

    def test_gh_unauthenticated_surfaces_ghs_own_last_line(self):
        r = _Runner(rc=1, stderr="gh: To use GitHub CLI, run: gh auth login\n")
        with self.assertRaises(ValueError) as ctx:
            P.resolve_pr("2001", runner=r)
        self.assertIn("gh auth login", str(ctx.exception))

    def test_gh_failing_with_no_stderr_still_says_something_useful(self):
        r = _Runner(rc=4, stderr="")
        with self.assertRaises(ValueError) as ctx:
            P.resolve_pr("2001", runner=r)
        self.assertIn("gh exited 4", str(ctx.exception))

    def test_unparseable_output_is_an_error_not_a_crash(self):
        r = _Runner(stdout="not json at all")
        with self.assertRaises(ValueError) as ctx:
            P.resolve_pr("2001", runner=r)
        self.assertIn("no usable JSON", str(ctx.exception))

    def test_json_without_a_head_sha_is_an_error(self):
        r = _Runner(stdout=json.dumps({"title": "x"}))
        with self.assertRaises(ValueError) as ctx:
            P.resolve_pr("2001", runner=r)
        self.assertIn("no headRefOid", str(ctx.exception))

    def test_os_error_running_gh_is_an_error_not_a_crash(self):
        r = _Runner(raises=OSError("permission denied"))
        with self.assertRaises(ValueError) as ctx:
            P.resolve_pr("2001", runner=r)
        self.assertIn("permission denied", str(ctx.exception))


class PrWarningTests(unittest.TestCase):
    """`state` and `baseRefName` were requested from gh and then read by nothing."""

    def test_an_open_pr_targeting_the_base_is_silent(self):
        self.assertEqual(
            P.pr_warnings("2024", {"state": "OPEN", "baseRefName": "main"},
                          "origin/main"), [])

    def test_a_merged_pr_is_called_out(self):
        w = P.pr_warnings("2001", {"state": "MERGED", "baseRefName": "main"},
                          "origin/main")
        self.assertEqual(len(w), 1)
        self.assertIn("MERGED", w[0])

    def test_a_pr_targeting_another_branch_is_called_out(self):
        # A clean verdict against origin/main for a PR aimed at `port` is a confident
        # answer to a question nobody asked.
        w = P.pr_warnings("2011", {"state": "OPEN", "baseRefName": "port"},
                          "origin/main")
        self.assertEqual(len(w), 1)
        self.assertIn("port", w[0])
        self.assertIn("origin/main", w[0])

    def test_missing_metadata_produces_no_noise(self):
        self.assertEqual(P.pr_warnings("1", {}, "origin/main"), [])
        self.assertEqual(P.pr_warnings("1", None, "origin/main"), [])


class WorstRcTests(unittest.TestCase):
    """max() over return codes is wrong, and it is wrong in the PASS direction."""

    def test_all_clean_is_clean(self):
        self.assertEqual(P.worst_rc([0, 0, 0]), 0)
        self.assertEqual(P.status_for_rc(P.worst_rc([0, 0, 0])), "pass")

    def test_a_plain_failure_among_passes_fails(self):
        self.assertEqual(P.status_for_rc(P.worst_rc([0, 1, 0])), "fail")

    def test_a_signal_killed_batch_is_not_a_pass(self):
        # THE regression test. CI is Linux; header-offsets runs ~5 batches of ~120
        # headers; an OOM kill gives -9 and `max(0, -9)` is 0. That reported green.
        self.assertNotEqual(P.worst_rc([0, -9, 0]), 0)
        self.assertEqual(P.status_for_rc(P.worst_rc([0, -9, 0])), "error")

    def test_a_segfault_among_passes_is_an_error(self):
        self.assertEqual(P.status_for_rc(P.worst_rc([0, 0, -11])), "error")

    def test_an_abnormal_code_outranks_a_plain_failure(self):
        # exit 2 means "could not answer" in this tree's tools; it must not be
        # flattened into "the gate said no".
        self.assertEqual(P.status_for_rc(P.worst_rc([1, 2])), "error")
        self.assertEqual(P.status_for_rc(P.worst_rc([2, 1])), "error")

    def test_a_signal_kill_reaches_the_gate_result(self):
        # Not just the helper: run_gate must record it too.
        class _Killed:
            returncode, stdout, stderr = -9, "", ""

        real = P.subprocess.run
        P.subprocess.run = lambda *a, **k: _Killed()
        try:
            res = P.run_gate({"key": "k", "commands": lambda t: [["x"], ["y"]]}, ".")
        finally:
            P.subprocess.run = real
        self.assertEqual(res["status"], "error")
        self.assertIn("signal 9", res["summary"])


class PrintTableTests(unittest.TestCase):
    """What the reader is actually shown. Nothing here needs a tree."""

    ROWS = [{"gate": "layout-check", "base": "pass", "merge": "pass",
             "verdict": "ok"}]

    def _render(self, **kw):
        out = []
        P.print_table(say=out.append, **kw)
        return "\n".join(out)

    def test_a_clean_table_shows_every_gate_and_the_coverage_row(self):
        text = self._render(rows=self.ROWS,
                            cov_row=P.coverage_row({"status": "pass"},
                                                   {"status": "pass"}))
        self.assertIn("layout-check", text)
        self.assertIn(P.COVERAGE_GATE, text)
        self.assertIn("ok", text)

    def test_an_unmeasured_coverage_row_prints_the_reason_not_see_above(self):
        # The old message said "not measured on both trees -- see above" and the
        # function that saw the child's output had thrown it away, so there was no
        # above. The reason belongs on the row.
        text = self._render(
            rows=self.ROWS,
            cov_row=P.coverage_row({"status": "pass"}, {"status": "error"}),
            cov_detail=["source_coverage.py --check exited 1",
                        "  File \"tools/source_coverage.py\", line 4",
                        "SyntaxError: invalid syntax"])
        self.assertNotIn("see above", text)
        self.assertIn("SyntaxError", text)
        self.assertIn("REGRESSION", text)

    def test_a_coverage_loss_prints_its_ranges(self):
        text = self._render(
            rows=self.ROWS,
            cov_row=P.coverage_row({"status": "pass"}, {"status": "fail"}),
            cov_lines=P.coverage_lines(CoverageLineTests.RELOCATED))
        self.assertIn("71,984 B", text)
        self.assertIn(".rodata", text)

    def test_context_lines_are_labelled_info(self):
        text = self._render(rows=self.ROWS,
                            cov_row=P.coverage_row({"status": "pass"},
                                                   {"status": "pass"}),
                            cov_context=["entries: 11026 -> 11012 (-14)"])
        self.assertIn("info: entries: 11026 -> 11012", text)


class BaseAgeTests(unittest.TestCase):
    """The base is read out of the object store and nothing refreshes it."""

    def _age(self, seconds_old, ok=True):
        real = P.git
        P.git = lambda repo, *a, **k: subprocess.CompletedProcess(
            a, 0 if ok else 128,
            str(int(1_000_000) - seconds_old) if ok else "", "")
        try:
            return P.base_age("repo", "sha", now=1_000_000)
        finally:
            P.git = real

    def test_a_fresh_base_reads_as_minutes(self):
        hours, note = self._age(600)
        self.assertLess(hours, 1)
        self.assertIn("m ago", note)

    def test_a_day_old_base_reads_as_hours(self):
        hours, note = self._age(30 * 3600)
        self.assertAlmostEqual(hours, 30.0, places=3)
        self.assertIn("30h ago", note)

    def test_a_week_old_base_shouts_in_days(self):
        hours, note = self._age(7 * 24 * 3600)
        self.assertIn("DAYS", note)
        self.assertGreater(hours, P.BASE_STALE_HOURS)

    def test_an_unanswerable_age_is_not_reported_as_fresh(self):
        hours, note = self._age(0, ok=False)
        self.assertIsNone(hours)
        self.assertIn("unknown", note)


class LangmodeGateTests(unittest.TestCase):
    """The ratchet is a gate now, and a missing baseline is an ERROR."""

    def test_it_is_in_the_gate_table(self):
        self.assertIn("langmode-ratchet", [g["key"] for g in P.GATES])

    def test_a_tree_with_the_baseline_gets_the_check_command(self):
        import tempfile
        with tempfile.TemporaryDirectory(prefix="premerge-test-") as tmp:
            (pathlib.Path(tmp) / P.LANGMODE_BASELINE).write_text("{}")
            cmds = P._langmode_commands(tmp)
            self.assertEqual(len(cmds), 1)
            self.assertIn("tools/langmode_audit.py", cmds[0])
            self.assertIn("--check", cmds[0])

    def test_a_tree_without_the_baseline_is_an_error_not_a_pass(self):
        # CI falls back to _chaos_data/langmode-baseline.json, which is UNTRACKED and
        # therefore absent from every export. A ratchet measured against nothing must
        # never read as green.
        import tempfile
        with tempfile.TemporaryDirectory(prefix="premerge-test-") as tmp:
            res = P.run_gate({"key": "langmode-ratchet",
                              "commands": P._langmode_commands}, tmp)
        self.assertEqual(res["status"], "error")
        self.assertIn("chaos-data", res["summary"])

    def test_a_missing_baseline_on_both_sides_blames_nobody(self):
        # ... and when main has dropped the root override too, the pair is
        # pre-existing, which is exit 0. Absent is not a PR's fault either.
        self.assertEqual(P.classify("error", "error"), "pre-existing")


class GateShapeTests(unittest.TestCase):
    """Properties of the gate table itself, checkable without running anything."""

    FORBIDDEN = ("rombuild", "eligible")

    def test_no_gate_can_ever_invoke_a_rom_build(self):
        # `build/` is shared between worktrees here, so a gate that needed a ROM
        # build could not run alongside the work it gates. This asserts the rule
        # rather than trusting the docstring to keep being true.
        for gate in P.GATES:
            try:
                cmds = gate["commands"]("/nonexistent")
            except Exception:
                continue          # needs a real tree; covered by the end-to-end test
            for argv in cmds or []:
                joined = " ".join(argv)
                for bad in self.FORBIDDEN:
                    self.assertNotIn(bad, joined, gate["key"])

    def test_every_gate_names_an_existing_tool(self):
        repo = pathlib.Path(__file__).resolve().parent.parent
        for gate in P.GATES:
            try:
                cmds = gate["commands"]("/nonexistent")
            except Exception:
                continue
            for argv in cmds or []:
                script = argv[1]
                self.assertTrue((repo / script).is_file(),
                                f"{gate['key']} names {script}, which is not there")

    def test_a_gate_with_nothing_to_do_is_an_error_not_a_pass(self):
        # The anti-hollow rule, at the level this tool controls: header-offsets over
        # zero headers must not read as green.
        res = P.run_gate({"key": "empty", "commands": lambda t: []}, "/nonexistent")
        self.assertEqual(res["status"], "error")
        self.assertIn("empty input set", res["summary"])

    def test_a_gate_whose_command_builder_raises_is_an_error_not_a_pass(self):
        def boom(_tree):
            raise RuntimeError("index unreadable")
        res = P.run_gate({"key": "boom", "commands": boom}, "/nonexistent")
        self.assertEqual(res["status"], "error")
        self.assertIn("index unreadable", res["summary"])

    def test_a_gate_that_cannot_be_launched_is_an_error_not_a_pass(self):
        res = P.run_gate(
            {"key": "nope",
             "commands": lambda t: [["definitely-not-a-real-binary-xyz"]]},
            pathlib.Path(__file__).resolve().parent)
        self.assertEqual(res["status"], "error")


class SummariseTests(unittest.TestCase):
    """Which line of a gate's output a reader is shown."""

    def test_a_passing_gate_is_summarised_by_its_last_line(self):
        self.assertEqual(
            P.summarise(["scanning...", "CONVERTED ratchet PASS   baseline 2553"],
                        "pass"),
            "CONVERTED ratchet PASS   baseline 2553")

    def test_a_red_batched_gate_reports_the_complaint_not_the_last_line(self):
        # header-offsets prints one line per header and ends on whichever sorted
        # last, which says nothing about why it failed.
        lines = ["include/A.h: 0 commented fields, 0 mismatched, 0 unparsed",
                 "include/B.h: 12 commented fields, 3 mismatched, 0 unparsed",
                 "include/Z.h: 0 commented fields, 0 mismatched, 0 unparsed"]
        self.assertIn("B.h", P.summarise(lines, "fail"))

    def test_zero_mismatched_is_not_read_as_a_complaint(self):
        lines = ["include/A.h: 0 commented fields, 0 mismatched, 0 unparsed",
                 "check: something else went wrong"]
        # No `[1-9]\\d* mismatched` anywhere, so the fallback (last line) is used.
        self.assertEqual(P.summarise(lines, "fail"),
                         "check: something else went wrong")

    def test_no_output_at_all_says_so(self):
        self.assertEqual(P.summarise([], "fail"), "(no output)")


class ExportTreeTests(unittest.TestCase):
    """The one place a real git binary is genuinely required."""

    def test_exported_tree_answers_git_ls_files(self):
        try:
            subprocess.run(["git", "--version"], capture_output=True, check=True)
        except (OSError, subprocess.CalledProcessError):
            self.skipTest("no git binary on PATH -- the export step cannot be "
                          "exercised here (the pure logic above still was)")
        repo = pathlib.Path(__file__).resolve().parent.parent
        head = subprocess.run(["git", "-C", str(repo), "rev-parse", "HEAD^{tree}"],
                              capture_output=True, text=True)
        if head.returncode != 0:
            self.skipTest("tools/ is not inside a git repository with a HEAD")
        import tempfile
        with tempfile.TemporaryDirectory(prefix="premerge-test-") as tmp:
            dest = pathlib.Path(tmp) / "tree"
            P.export_tree(repo, head.stdout.strip(), dest, tmp)
            # The whole point of the `git init` + index copy: a plain
            # `checkout-index --prefix=` export exits 128 on this command, and four
            # of the gates shell out to exactly it.
            out = subprocess.run(["git", "-C", str(dest), "ls-files", "tools/*.py"],
                                 capture_output=True, text=True)
            self.assertEqual(out.returncode, 0, out.stderr)
            # Assert on a file the export is guaranteed to carry. Not this file: the
            # export is of HEAD^{tree}, so an uncommitted premerge_check.py is
            # legitimately absent and asserting on it would fail for the wrong reason.
            self.assertIn("tools/tiers_ratchet.py", out.stdout)
            self.assertTrue((dest / "tools" / "tiers_ratchet.py").is_file())

    # The three below run against a THROWAWAY three-file repository rather than this
    # one: they are about export_tree's own bookkeeping, and exporting ~12,700 files
    # three more times would put a minute on a suite CI runs on every push for no
    # additional coverage. The real-repository export above stays, once.

    def _toy(self, tmp):
        """A tiny git repo -> (repo_path, tree_sha). No commit: a tree is enough."""
        self._need_git()
        repo = pathlib.Path(tmp) / "toy"
        (repo / "src").mkdir(parents=True)
        (repo / "src" / "keep.c").write_text("int keep;\n")
        (repo / "src" / "gone.c").write_text("int gone;\n")
        (repo / "README").write_text("toy\n")
        for argv in (["init", "-q", "."], ["add", "-A"]):
            out = subprocess.run(["git", "-C", str(repo), *argv],
                                 capture_output=True, text=True)
            self.assertEqual(out.returncode, 0, out.stderr)
        out = subprocess.run(["git", "-C", str(repo), "write-tree"],
                             capture_output=True, text=True)
        self.assertEqual(out.returncode, 0, out.stderr)
        return repo, out.stdout.strip()

    def test_a_short_export_is_refused_before_any_gate_runs(self):
        # An empty or truncated export makes every gate ERROR on BOTH sides,
        # classify() reads that as `pre-existing`, and the run prints "Safe to merge"
        # and exits 0. The floor is what stops that -- and it is the code path a bad
        # tree sha or a partial merge result takes.
        import tempfile
        with tempfile.TemporaryDirectory(prefix="premerge-test-") as tmp:
            repo, tree = self._toy(tmp)
            with self.assertRaises(RuntimeError) as ctx:
                P.export_tree(repo, tree, pathlib.Path(tmp) / "out", tmp)
            self.assertIn("TOO SMALL", str(ctx.exception))

    def test_a_truncated_checkout_is_caught(self):
        # checkout-index stopping part-way leaves an index promising files nothing
        # wrote. The gates then error for a reason that has nothing to do with the
        # merge -- on both sides, which reads as `pre-existing` and exits 0.
        import tempfile
        with tempfile.TemporaryDirectory(prefix="premerge-test-") as tmp:
            repo, tree = self._toy(tmp)
            dest = P.export_tree(repo, tree, pathlib.Path(tmp) / "out", tmp, floor=1)
            (dest / "src" / "gone.c").unlink()
            with self.assertRaises(RuntimeError) as ctx:
                P.verify_export(repo, tree, dest, floor=1)
            self.assertIn("not on disk", str(ctx.exception))
            self.assertIn("src/gone.c", str(ctx.exception))

    def test_reusing_a_directory_does_not_keep_the_previous_trees_files(self):
        # `mkdir(exist_ok=True)` + `checkout-index -a -f` overwrites every path the
        # new tree HAS and silently keeps every path it does not, so the directory
        # becomes the union of two trees and corresponds to no commit. A file DELETED
        # by the merge survives, and duplicate-sources and layout-check report on it.
        import tempfile
        with tempfile.TemporaryDirectory(prefix="premerge-test-") as tmp:
            repo, tree = self._toy(tmp)
            dest = pathlib.Path(tmp) / "out"
            P.export_tree(repo, tree, dest, tmp, floor=1)
            ghost = dest / "src" / "ghost_from_the_previous_run.c"
            ghost.write_text("int ghost;\n")
            P.export_tree(repo, tree, dest, tmp, floor=1)
            self.assertFalse(ghost.exists())
            self.assertTrue((dest / "src" / "keep.c").is_file())

    def _need_git(self):
        try:
            subprocess.run(["git", "--version"], capture_output=True, check=True)
        except (OSError, subprocess.CalledProcessError):
            self.skipTest("no git binary on PATH -- the export step cannot be "
                          "exercised here (the pure logic above still was)")

    def _tree(self, repo):
        out = subprocess.run(["git", "-C", str(repo), "rev-parse", "HEAD^{tree}"],
                             capture_output=True, text=True)
        if out.returncode != 0:
            self.skipTest("tools/ is not inside a git repository with a HEAD")
        return out.stdout.strip()


def _cp(rc, stdout="", stderr=""):
    return subprocess.CompletedProcess([], rc, stdout, stderr)


class MainTests(unittest.TestCase):
    """main() end to end, with git, gh and the gates replaced by data.

    main() is what decides the exit code, and nothing exercised it: every defect in
    the wiring between a gate result and `"outcome": "clean"` lived here, unobserved.
    Everything expensive is stubbed -- no export, no merge-tree, no 70-second ratchet
    -- so these run in milliseconds and still cover the wiring, which is the part that
    was wrong.
    """

    BASE = "a" * 40
    HEAD = "b" * 40
    TREE = "c" * 40

    PASS = {"status": "pass", "rc": 0, "summary": "layout-check: ok", "output": ""}
    COV_OK = {"status": "pass", "rc": 0, "summary": "source coverage: no bytes lost",
              "output": "", "findings": {"unwaivedLostBytes": 0}}
    METRICS = {"data": {"entries": 11026, "modules": 106, "currentBytes": 2067148,
                        "bySection": {".text": 1941436}},
               "output": "", "summary": ""}

    def _fake_git(self, repo, *args, check=False):
        a = list(args)
        if a[0] == "rev-parse" and a[1].endswith("^{tree}"):
            return _cp(0, self.TREE + "\n")
        if a[0] == "rev-parse":
            return _cp(0, self.BASE + "\n")
        if a[0] == "log":
            return _cp(0, str(int(P.time.time()) - 3600) + "\n")
        return _cp(0, "")

    def _run(self, cov_merge, base_cov=None, argv=None):
        """main() over stubbed everything -> (exit_code, stdout_text)."""
        import contextlib
        import io
        import shutil
        import tempfile
        real = {name: getattr(P, name) for name in
                ("git", "git_supports_write_tree", "export_tree", "run_all_gates",
                 "coverage_metrics", "coverage_check")}
        real_run = P.subprocess.run
        holder = tempfile.mkdtemp(prefix="premerge-test-")
        checks = iter([base_cov or self.COV_OK, cov_merge])
        gates = getattr(self, "_gates", None)
        try:
            P.git = self._fake_git
            P.git_supports_write_tree = lambda repo: True
            P.export_tree = lambda repo, tree, dest, scratch: pathlib.Path(holder)
            P.run_all_gates = ((lambda tree: next(gates)) if gates
                               else (lambda tree: {"layout-check": dict(self.PASS)}))
            P.coverage_metrics = lambda tree: dict(self.METRICS)
            P.coverage_check = lambda tree, repo, ref, base_ref: next(checks)
            P.subprocess.run = lambda *a, **k: _cp(0, self.TREE + "\n")
            buf = io.StringIO()
            with contextlib.redirect_stdout(buf):
                rc = P.main(argv or [self.HEAD, "--json"])
            return rc, buf.getvalue()
        finally:
            for name, fn in real.items():
                setattr(P, name, fn)
            P.subprocess.run = real_run
            shutil.rmtree(holder, ignore_errors=True)

    def test_a_clean_run_exits_zero_and_says_clean(self):
        rc, out = self._run(self.COV_OK)
        doc = json.loads(out)
        self.assertEqual(rc, 0)
        self.assertEqual(doc["exit"], 0)
        self.assertEqual(doc["targets"][0]["outcome"], "clean")
        self.assertTrue(doc["targets"][0]["coverageMeasured"])

    def test_unmeasured_coverage_does_not_exit_zero_and_is_not_clean(self):
        # THE F1 regression test. coverage_metrics() swallowed the OSError, the
        # timeout, the non-zero exit and the unparseable JSON alike and returned None,
        # and the delta of two Nones was []. The run printed `"outcome": "clean"`,
        # `"exit": 0`, `"coverage": null` with nothing anywhere saying the gate had
        # not run -- the precise treatment classify()'s own docstring forbids.
        broken = {"status": "error", "rc": 1,
                  "summary": "source_coverage.py --check exited 1",
                  "output": "Traceback ...\nSyntaxError: invalid syntax",
                  "findings": None}
        rc, out = self._run(broken)
        doc = json.loads(out)
        self.assertNotEqual(rc, 0)
        self.assertEqual(doc["exit"], 1)
        self.assertEqual(doc["targets"][0]["outcome"], "regression")
        self.assertIs(doc["targets"][0]["coverageMeasured"], False)
        self.assertEqual(doc["targets"][0]["coverageStatus"], "error")

    def test_the_reason_coverage_could_not_be_measured_is_printed(self):
        # The child's stdout and stderr were captured and dropped on the floor. The
        # reader was told a number was missing and never told why.
        broken = {"status": "error", "rc": 1, "summary": "source_coverage.py failed",
                  "output": "SyntaxError: invalid syntax", "findings": None}
        rc, out = self._run(broken, argv=[self.HEAD])
        self.assertIn("SyntaxError: invalid syntax", out)
        self.assertNotIn("see above", out)

    def test_a_relocation_with_flat_totals_fails_the_run(self):
        # F2, end to end: every byte COUNT identical on both sides, 71,984 B handed
        # back, and the tool goes red -- because CI does.
        lost = {"status": "fail", "rc": 1,
                "summary": "source coverage: 71984 unwaived byte(s) lost",
                "output": "", "findings": CoverageLineTests.RELOCATED}
        rc, out = self._run(lost)
        doc = json.loads(out)
        self.assertEqual(rc, 1)
        self.assertEqual(doc["targets"][0]["outcome"], "regression")
        self.assertTrue(any("71,984 B" in ln
                            for ln in doc["targets"][0]["coverageRegressions"]))

    def test_coverage_broken_on_the_base_too_is_not_this_prs_fault(self):
        # The other direction, and the reason the base self-check exists: a
        # source_coverage.py already broken ON MAIN runs in the merge tree too --
        # it is the merge tree's own copy -- errors identically, and must not be
        # charged to a PR that merely merges against it.
        broken = {"status": "error", "rc": 2, "summary": "boom", "output": "boom",
                  "findings": None}
        rc, out = self._run(broken, base_cov=broken)
        doc = json.loads(out)
        self.assertEqual(rc, 0)
        self.assertEqual(doc["targets"][0]["outcome"], "clean")
        # ... and the reader is still told the number does not exist.
        self.assertIs(doc["targets"][0]["coverageMeasured"], False)
        self.assertIs(doc["base"]["coverageMeasured"], False)

    def test_both_sides_red_differently_prints_both_summaries(self):
        # F4. `fail`/`fail` was printed as one word with neither summary shown, so
        # `backslide: 1 file` on the base and `backslide: 500 files` on the merge
        # result read identically. The exit code is unchanged; what is printed is not.
        self._gates = iter([
            {"converted-ratchet": {"status": "fail", "rc": 1, "output": "",
                                   "summary": "CONVERTED backslide: 1 file"}},
            {"converted-ratchet": {"status": "fail", "rc": 1, "output": "",
                                   "summary": "CONVERTED backslide: 500 files"}}])
        try:
            rc, out = self._run(self.COV_OK, argv=[self.HEAD])
        finally:
            del self._gates
        self.assertEqual(rc, 0)                       # pre-existing: still exit 0
        self.assertIn("CONVERTED backslide: 1 file", out)
        self.assertIn("CONVERTED backslide: 500 files", out)
        self.assertIn("not in the same way", out)

    def test_a_stale_base_is_named(self):
        real = P.base_age
        P.base_age = lambda repo, sha, now=None: (172.0, "(committed 7.2 DAYS ago)")
        try:
            rc, out = self._run(self.COV_OK, argv=[self.HEAD])
        finally:
            P.base_age = real
        self.assertIn("DAYS ago", out)
        self.assertIn("--fetch", out)

    def test_a_pr_state_warning_reaches_the_report(self):
        real = P.resolve_pr
        P.resolve_pr = lambda spec, runner=None, repo=None: (
            self.HEAD, "#2001 something", {"state": "MERGED", "baseRefName": "main"})
        try:
            rc, out = self._run(self.COV_OK, argv=["#2001", "--json"])
        finally:
            P.resolve_pr = real
        doc = json.loads(out)
        self.assertTrue(any("MERGED" in w for w in doc["warnings"]))


if __name__ == "__main__":
    unittest.main(verbosity=2)
