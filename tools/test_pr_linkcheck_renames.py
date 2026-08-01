"""The rename cases `_changed_paths` has to get right.

Regression test for the hole where `--diff-filter=AM` dropped every `R` entry, so a
`git mv` plus an edit to the `#include` line was reported green having compiled nothing.
Builds throwaway git repos rather than asserting against this repo's history, so the
cases stay readable and the test does not rot as main moves.
"""
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import pr_linkcheck as PL  # noqa: E402


def _git(repo, *args):
    subprocess.run(["git", "-C", str(repo), *args], check=True,
                   capture_output=True, text=True)


def _commit(repo, msg):
    _git(repo, "add", "-A")
    _git(repo, "-c", "user.email=t@t", "-c", "user.name=t", "commit", "-qm", msg)


class RenameScope(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        _git(self.repo, "init", "-q", ".")
        (self.repo / "src").mkdir()
        # big enough that git pairs a small edit as a rename rather than add+delete
        body = "".join(f"int f{i}(void) {{ return {i}; }}\n" for i in range(60))
        (self.repo / "src" / "moved_edited.c").write_text('#include "old.h"\n' + body)
        (self.repo / "src" / "moved_clean.c").write_text(body)
        (self.repo / "src" / "renamed.c").write_text(body)
        (self.repo / "src" / "plain.c").write_text("int p(void) { return 0; }\n")
        _commit(self.repo, "base")
        self._orig_repo = PL.REPO
        PL.REPO = self.repo

    def tearDown(self):
        PL.REPO = self._orig_repo
        self.tmp.cleanup()

    def _changed(self):
        return set(PL._changed_paths("HEAD~1"))

    def test_move_with_edit_is_checked(self):
        """The case that was silently skipped: `git mv` + an edited include line."""
        (self.repo / "src" / "sub").mkdir()
        _git(self.repo, "mv", "src/moved_edited.c", "src/sub/moved_edited.c")
        p = self.repo / "src" / "sub" / "moved_edited.c"
        p.write_text(p.read_text().replace("old.h", "new.h"))
        _commit(self.repo, "move+edit")
        self.assertIn("src/sub/moved_edited.c", self._changed())

    def test_pure_move_same_name_is_skipped(self):
        """Same basename, byte-identical content: the symbol and the bytes cannot move."""
        (self.repo / "src" / "sub").mkdir()
        _git(self.repo, "mv", "src/moved_clean.c", "src/sub/moved_clean.c")
        _commit(self.repo, "pure move")
        self.assertEqual(self._changed(), set())

    def test_rename_to_a_new_symbol_is_checked(self):
        """R100 but a different basename -- the file now claims a different symbol."""
        _git(self.repo, "mv", "src/renamed.c", "src/RealName.c")
        _commit(self.repo, "rename symbol")
        self.assertIn("src/RealName.c", self._changed())

    def test_plain_add_and_modify_still_reported(self):
        (self.repo / "src" / "plain.c").write_text("int p(void) { return 1; }\n")
        (self.repo / "src" / "added.c").write_text("int a(void) { return 0; }\n")
        _commit(self.repo, "add+modify")
        self.assertEqual(self._changed(), {"src/plain.c", "src/added.c"})


if __name__ == "__main__":
    unittest.main()
