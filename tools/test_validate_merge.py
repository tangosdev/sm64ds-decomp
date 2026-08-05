"""PR reporting is revision-scoped so committed R100 moves preserve credit."""
import os
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import validate_merge as VM  # noqa: E402


def git(repo, *args, env=None):
    merged_env = dict(os.environ)
    if env:
        merged_env.update(env)
    return subprocess.run(["git", "-C", str(repo), *args], check=True,
                          capture_output=True, text=True, env=merged_env).stdout.strip()


def commit(repo, message, login):
    git(repo, "add", "-A")
    git(repo, "-c", f"user.name={login}",
        "-c", f"user.email={login}@users.noreply.github.com",
        "commit", "-qm", message)
    return git(repo, "rev-parse", "HEAD")


class ValidateMerge(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        git(self.repo, "init", "-q", ".")
        (self.repo / "src").mkdir()
        config = self.repo / "config" / "arm9"
        config.mkdir(parents=True)
        (config / "symbols.txt").write_text(
            "Example kind:function(arm,size=0x4) addr:0x02000000\n",
            encoding="utf-8")
        (config / "delinks.txt").write_text(
            "    .text start:0x02000000 end:0x02000004 kind:code\n\n"
            "src/Example.c:\n"
            "    complete\n"
            "    .text start:0x02000000 end:0x02000004\n",
            encoding="utf-8")
        (self.repo / "src" / "Example.c").write_text("int Example(void) { return 0; }\n")
        self.base = commit(self.repo, "base", "alice")
        self.base_branch = git(self.repo, "branch", "--show-current")
        self.old_repo = VM.REPO
        VM.REPO = self.repo

    def tearDown(self):
        VM.REPO = self.old_repo
        self.tmp.cleanup()

    def test_committed_perfect_move_preserves_counts_and_credit(self):
        (self.repo / "src" / "nested").mkdir()
        git(self.repo, "mv", "src/Example.c", "src/nested/Example.c")
        config = self.repo / "config" / "arm9" / "delinks.txt"
        config.write_text(config.read_text(encoding="utf-8").replace(
            "src/Example.c:", "src/nested/Example.c:"), encoding="utf-8")
        head = commit(self.repo, "move", "bob")
        report = VM.build_report(self.base, head)
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["coverage"]["delta"]["matchedFunctions"], 0)
        self.assertEqual(len(report["diff"]["perfectRenames"]), 1)
        self.assertEqual(report["attribution"]["base"], report["attribution"]["head"])
        self.assertEqual(report["attribution"]["added"], [])
        self.assertEqual(report["attribution"]["changed"], [])
        self.assertEqual(report["attribution"]["lost"], [])
        self.assertEqual(report["attribution"]["head"]["alice"]["functions"], 1)

    def test_nonmatching_is_read_from_requested_revision(self):
        (self.repo / "src" / "Example.c").write_text(
            "// NONMATCHING\nint Example(void) { return 0; }\n")
        head = commit(self.repo, "mark draft", "bob")
        self.assertEqual(VM.function_snapshot(self.base)["stats"]["matchedFunctions"], 1)
        self.assertEqual(VM.function_snapshot(head)["stats"]["matchedFunctions"], 0)

    def test_require_merge_commit_rejects_an_uncommitted_merge_shape(self):
        with self.assertRaisesRegex(RuntimeError, "not a committed merge"):
            VM.build_report(self.base, "HEAD", require_merge_commit=True)

    def test_pr_linkcheck_grouped_json_is_flattened(self):
        state = VM._link_state([{
            "file": "src/Example.c",
            "results": [{"sym": "Example", "verdict": "VERIFIED"},
                        {"sym": "Thunk", "verdict": "BLIND-RELOC"}],
        }])
        self.assertEqual(state["checked"], 2)
        self.assertEqual(state["tally"], {"VERIFIED": 1, "BLIND": 1})
        self.assertEqual(state["blocking"], [])

    def test_declared_draft_no_repro_does_not_block(self):
        # pr_linkcheck marks a self-declared NONMATCHING draft worst=DRAFT while
        # its per-slot results still read NO-REPRO (#968). Flattening must keep
        # the downgrade, or every PR that so much as renames a declared draft
        # fails on a non-reproduction the gate already excused.
        state = VM._link_state([{
            "file": "src/_ZN5Model27LoadCompressedTextureToVramEPcjS0_.cpp",
            "worst": "DRAFT",
            "results": [{"sym": "_ZN5Model27LoadCompressedTextureToVramEPcjS0_",
                         "verdict": "NO-REPRO"}],
        }])
        self.assertEqual(state["checked"], 1)
        self.assertEqual(state["tally"], {"DRAFT": 1})
        self.assertEqual(state["blocking"], [])

    def test_draft_downgrade_does_not_shield_wrong_dest(self):
        # The draft excuse covers non-reproduction only. A WRONG verdict on a
        # declared draft still blocks: its call graph must be honest even if
        # its bytes differ.
        state = VM._link_state([{
            "file": "src/Draft.c",
            "worst": "DRAFT",
            "results": [{"sym": "Draft", "verdict": "WRONG"}],
        }])
        self.assertEqual(state["tally"], {"WRONG": 1})
        self.assertEqual(len(state["blocking"]), 1)

    def test_port_refcheck_absent_is_neutral_and_stale_refs_are_named(self):
        # The phase is optional: a worker that never ran it, or a PR that
        # touches no source, must read exactly like a clean run -- no row, no
        # reason. When it does fail, the reason has to name the stale
        # reference, because the PR author never sees the worker's log.
        neutral = VM.build_report(self.base, "HEAD")
        self.assertEqual(neutral["portRefcheck"], {"available": False})
        self.assertNotIn("Port reference check", neutral["reportMarkdown"])

        stale = {"schemaVersion": 1, "ok": False, "checked": 395, "failed": 1,
                 "failures": [{"check": "manifests", "file": "port/slice_gate8.txt",
                               "line": 6, "message": "src/Example.c not found (renamed?)"}]}
        report = VM.build_report(self.base, "HEAD", port_report=stale)
        self.assertEqual(report["status"], "Failed")
        self.assertIn("port/slice_gate8.txt:6: src/Example.c not found (renamed?)",
                      "; ".join(report["reasons"]))
        self.assertIn("| Port reference check | 395 checked; 1 stale |",
                      report["reportMarkdown"])

    def test_port_refcheck_report_without_the_flat_failure_list_still_reads(self):
        # Tolerate a report written by an older port_refcheck that only groups
        # failures per check, so a base/worker version skew cannot make a real
        # failure look like a pass.
        grouped = {"ok": False, "checked": 2, "checks": {
            "cmake-symbols": {"checked": 2, "failures": [
                {"file": "port/CMakeLists.txt", "line": 9,
                 "message": "symbol 'Example' has no src/Example.c"}]}}}
        state = VM._port_state(grouped)
        self.assertFalse(state["passed"])
        self.assertEqual(state["failures"][0]["check"], "cmake-symbols")
        self.assertTrue(VM._port_state({"ok": True, "checked": 2, "failures": []})["passed"])

    def test_committed_test_merge_is_accepted_and_keeps_feature_author_credit(self):
        git(self.repo, "switch", "-q", "-c", "feature")
        (self.repo / "src" / "nested").mkdir()
        git(self.repo, "mv", "src/Example.c", "src/nested/Example.c")
        config = self.repo / "config" / "arm9" / "delinks.txt"
        config.write_text(config.read_text(encoding="utf-8").replace(
            "src/Example.c:", "src/nested/Example.c:"), encoding="utf-8")
        feature = commit(self.repo, "feature move", "bob")
        git(self.repo, "switch", "-q", self.base_branch)
        git(self.repo, "-c", "user.name=maintainer",
            "-c", "user.email=maintainer@users.noreply.github.com",
            "merge", "--no-ff", "-qm", "test merge", "feature")
        report = VM.build_report(
            self.base, "HEAD", require_merge_commit=True, expected_pr_head=feature)
        self.assertTrue(report["committedMerge"])
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["attribution"]["head"]["alice"]["functions"], 1)
        with self.assertRaisesRegex(RuntimeError, "second parent"):
            VM.build_report(
                self.base, "HEAD", require_merge_commit=True,
                expected_pr_head=self.base)

    def test_identical_base_failure_is_warning_but_changed_failure_blocks(self):
        base_failure = {"status": "error", "failure": {
            "phase": "mwldarm", "returncode": 1, "output": "undefined Existing"}}
        same = VM.build_report(self.base, "HEAD", base_failure, base_failure)
        self.assertEqual(same["status"], "Passed")
        self.assertTrue(same["rom"]["sameBaselineFailure"])
        changed_failure = {"status": "error", "failure": {
            "phase": "mwldarm", "returncode": 1, "output": "undefined New"}}
        changed = VM.build_report(self.base, "HEAD", base_failure, changed_failure)
        self.assertEqual(changed["status"], "Failed")
        self.assertIn("full-ROM validation failed", changed["reasons"])

    def test_function_universe_change_is_not_hidden_as_percentage_progress(self):
        symbols = self.repo / "config" / "arm9" / "symbols.txt"
        symbols.write_text(
            symbols.read_text(encoding="utf-8")
            + "Other kind:function(arm,size=0x4) addr:0x02000004\n",
            encoding="utf-8")
        head = commit(self.repo, "change denominator", "bob")
        report = VM.build_report(self.base, head)
        self.assertEqual(report["status"], "Failed")
        self.assertIn("function/byte coverage denominator changed", report["reasons"])

    def _declare_symbol(self, name):
        """Add ``name`` to the base symbol universe (no source yet) and commit,
        so a later src/ addition changes neither denominator nor matched set."""
        symbols = self.repo / "config" / "arm9" / "symbols.txt"
        symbols.write_text(
            symbols.read_text(encoding="utf-8")
            + f"{name} kind:function(arm,size=0x4) addr:0x02000004\n",
            encoding="utf-8")
        return commit(self.repo, f"declare {name}", "alice")

    def test_unbannered_dcd_transcription_is_demoted_and_fails_the_pr(self):
        # A dcd dump byte-matches vacuously (it IS the ROM words re-spelled), so
        # without a banner it must not count as matched at any revision, and a PR
        # that adds one must fail with a reason that names the file.
        base = self._declare_symbol("Raw")
        (self.repo / "src" / "Raw.c").write_text(
            "asm void Raw(void) {\n    dcd 0xe12fff1e\n}\n", encoding="utf-8")
        head = commit(self.repo, "transcribe Raw", "bob")
        snap = VM.function_snapshot(head)
        self.assertEqual(snap["stats"]["matchedFunctions"], 1)  # Example only
        self.assertFalse(snap["functions"]["arm9:0x02000004"]["matched"])
        report = VM.build_report(base, head)
        self.assertEqual(report["status"], "Failed")
        self.assertIn("src/Raw.c", "; ".join(report["reasons"]))
        self.assertTrue(any("asm-transcription" in r for r in report["reasons"]))
        self.assertEqual(report["asmPolicy"]["transcribed"], ["src/Raw.c"])

    def test_nonmatching_bannered_dcd_stays_on_the_draft_path(self):
        # The same dcd body under a NONMATCHING banner is an honest draft: not
        # matched (the established draft rule), and NOT a transcription failure.
        base = self._declare_symbol("Raw")
        (self.repo / "src" / "Raw.c").write_text(
            "// NONMATCHING\nasm void Raw(void) {\n    dcd 0xe12fff1e\n}\n",
            encoding="utf-8")
        head = commit(self.repo, "draft Raw", "bob")
        snap = VM.function_snapshot(head)
        self.assertEqual(snap["stats"]["matchedFunctions"], 1)
        report = VM.build_report(base, head)
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["asmPolicy"]["transcribed"], [])

    def test_hand_asm_primitive_banner_keeps_an_asm_body_matched(self):
        # The other legitimate tier: the original really was assembly, the banner
        # says so, and the function counts as matched with no gate reason.
        base = self._declare_symbol("Prim")
        (self.repo / "src" / "Prim.c").write_text(
            "// HAND-ASM PRIMITIVE: byte-faithful asm-block match (CPSR read).\n"
            "asm void Prim(void) {\n    mrs r0, cpsr\n    bx lr\n}\n",
            encoding="utf-8")
        head = commit(self.repo, "match Prim", "bob")
        snap = VM.function_snapshot(head)
        self.assertEqual(snap["stats"]["matchedFunctions"], 2)
        self.assertTrue(snap["functions"]["arm9:0x02000004"]["matched"])
        report = VM.build_report(base, head)
        self.assertEqual(report["status"], "Passed")
        self.assertEqual(report["asmPolicy"], {"transcribed": [], "unbanneredAsm": []})

    def test_raw_asm_group_verdict_blocks_and_overrides_vacuous_slots(self):
        # pr_linkcheck stamps RAW-ASM on the group row while the transcription's
        # per-slot results read VERIFIED (vacuously -- the dcd words ARE the ROM
        # bytes). Flattening must carry the override or those slots launder it.
        state = VM._link_state([{
            "file": "src/Raw.c",
            "worst": "RAW-ASM",
            "results": [{"sym": "Raw", "verdict": "VERIFIED"}],
        }])
        self.assertEqual(state["tally"], {"RAW-ASM": 1})
        self.assertEqual(len(state["blocking"]), 1)


if __name__ == "__main__":
    unittest.main()
