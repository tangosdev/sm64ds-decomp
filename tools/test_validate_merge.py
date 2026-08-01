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


if __name__ == "__main__":
    unittest.main()
