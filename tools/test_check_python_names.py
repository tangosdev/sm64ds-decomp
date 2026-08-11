"""What the python-names gate must catch, and what it must not fail on.

The first case is the #1367 defect verbatim -- a module bound as `AP` and read as
`asm_policy`, on a branch only reachable when a file fails. Static analysis is the
only thing that was ever going to find it, so it is pinned here.

The negative cases matter as much: this gate is deliberately narrow. If it starts
failing on unused imports it will be switched off, and then it catches nothing.
"""
import pathlib
import subprocess
import sys
import tempfile
import unittest

TOOL = pathlib.Path(__file__).resolve().parent / "check_python_names.py"

REPRO_1367 = '''import asm_policy as AP


def source_policy(worst, text):
    if worst == "NO-REPRO" and asm_policy.has_draft_banner(text):
        return "DRAFT"
    return worst
'''

FIXED = '''import asm_policy as AP


def source_policy(worst, text):
    if worst == "NO-REPRO" and AP.has_draft_banner(text):
        return "DRAFT"
    return worst
'''

STYLE_ONLY = '''import json
import os


def f():
    unused = 1
    return 2
'''


def run(*paths):
    return subprocess.run([sys.executable, str(TOOL), *map(str, paths)],
                          capture_output=True, text=True)


class Gate(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = pathlib.Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def _write(self, name, text):
        p = self.dir / name
        p.write_text(text, encoding="utf-8")
        return p

    def test_catches_the_1367_defect(self):
        r = run(self._write("repro.py", REPRO_1367))
        self.assertEqual(r.returncode, 1, r.stdout)
        self.assertIn("undefined name 'asm_policy'", r.stdout)

    def test_passes_once_the_name_is_bound(self):
        r = run(self._write("fixed.py", FIXED))
        self.assertEqual(r.returncode, 0, r.stdout)
        self.assertIn("PASS", r.stdout)

    def test_unparseable_file_fails(self):
        r = run(self._write("bad.py", "def broken(:\n"))
        self.assertEqual(r.returncode, 1, r.stdout)
        self.assertIn("could not be parsed", r.stdout)

    def test_style_findings_do_not_fail_the_gate(self):
        """The narrowness is the point -- a gate that lands red gets switched off."""
        r = run(self._write("style.py", STYLE_ONLY))
        self.assertEqual(r.returncode, 0, r.stdout)
        self.assertIn("advisory finding(s)", r.stdout)

    def test_advisories_are_counted_not_hidden(self):
        r = run("--advisories", self._write("style.py", STYLE_ONLY))
        self.assertEqual(r.returncode, 0, r.stdout)
        self.assertIn("imported but unused", r.stdout)

    def test_read_before_assignment_fails(self):
        r = run(self._write("local.py", "def f():\n    print(x)\n    x = 1\n"))
        self.assertEqual(r.returncode, 1, r.stdout)

    def test_this_repo_is_clean(self):
        """The baseline claim in the tool's docstring, asserted rather than trusted."""
        r = run()
        self.assertEqual(r.returncode, 0, r.stdout)
        self.assertIn("0 unresolvable names", r.stdout)


if __name__ == "__main__":
    unittest.main()
