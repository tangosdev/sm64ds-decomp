"""fakematch_inventory counts code, not comments, and reads any git ref."""
import io
import pathlib
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stdout

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import fakematch_inventory as FI


class CountTextTests(unittest.TestCase):
    def test_comment_mentions_do_not_count(self):
        src = (
            "/* Kept volatile (#821 r63): the ROM reloads it */\n"
            "// volatile here too, and *(int *)&x in a comment\n"
            "int f(void) { return 0; }\n"
        )
        self.assertEqual(sum(FI.count_text(src).values()), 0)

    def test_string_literals_survive_comment_stripping(self):
        # A "//" inside a string must not swallow the code after it.
        src = 'const char *s = "http://x"; volatile int v;\n'
        self.assertEqual(FI.count_text(src)["volatile"], 1)

    def test_each_category(self):
        src = (
            "volatile int pad[4];\n"
            "#pragma push\n#pragma opt_common_subs off\n#pragma pop\n"
            "s16 a = *(s16 *)((char *)this + 0x90);\n"
            "Vector3 *p = (Vector3 *)&mFooX; const Vector3 *q = (const Vector3 *)&v;\n"
            "*(Vec3Words *)&a = *(Vec3Words *)&b;\n"
            "daFoo_c *self = this;\n"
            "extern char pmf_daFoo_c_Run[];\n"
        )
        c = FI.count_text(src)
        self.assertEqual(c["volatile"], 1)
        self.assertEqual(c["pragma"], 1)
        self.assertEqual(c["charthis"], 1)
        self.assertEqual(c["v3cast"], 2)
        self.assertEqual(c["launder"], 2)
        self.assertEqual(c["selfthis"], 1)
        self.assertEqual(c["pmfext"], 1)

    def test_lib_sources_are_out_of_scope(self):
        self.assertTrue(FI.is_counted("src/game/actors/d_a_foo.cpp"))
        self.assertTrue(FI.is_counted("src/engine/os/OS_Printf.c"))
        self.assertFalse(FI.is_counted("src/lib/msl/ansi_fp.c"))
        self.assertFalse(FI.is_counted("src/game/actors/d_a_foo.h"))
        self.assertFalse(FI.is_counted("include/foo.cpp"))


class GitRefTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.repo = self.tmp.name

        def git(*a):
            subprocess.run(["git", "-C", self.repo, *a], check=True, capture_output=True)
        self.git = git
        git("init", "-q")
        git("config", "user.email", "t@example.invalid")
        git("config", "user.name", "t")
        git("config", "core.autocrlf", "false")
        self.write("src/a.cpp", "volatile int x; volatile int y;\n")
        self.write("src/lib/msl/b.c", "volatile int z;\n")
        git("add", "-A")
        git("commit", "-q", "-m", "one")
        self.write("src/a.cpp", "int x; volatile int y; // volatile\n")
        git("commit", "-q", "-am", "two")

    def write(self, rel, text):
        p = pathlib.Path(self.repo, rel)
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")

    def test_counts_per_ref_from_git_not_the_worktree(self):
        self.write("src/a.cpp", "volatile volatile volatile\n")  # uncommitted: ignored
        old, files = FI.inventory(self.repo, "HEAD~1")
        new, _ = FI.inventory(self.repo, "HEAD")
        self.assertEqual(old["volatile"], 2)
        self.assertEqual(files["volatile"], {"src/a.cpp": 2})
        self.assertEqual(new["volatile"], 1)

    def test_cli_labels_and_top(self):
        out = io.StringIO()
        with redirect_stdout(out):
            FI.main(["--repo", self.repo, "--top", "3", "old=HEAD~1", "new=HEAD"])
        text = out.getvalue()
        self.assertIn("old", text)
        self.assertIn("new", text)
        self.assertIn("src/a.cpp", text)
        self.assertNotIn("src/lib", text)


if __name__ == "__main__":
    unittest.main()
