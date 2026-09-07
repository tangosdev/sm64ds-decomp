"""The verify step in tools/cpp_rename.py: what it sends to match.py, and what a
failure does to the tree.

Regression test for a verifier that could not pass. verify_match built a match.py
command with no --size, match.py declares --size required, so every invocation
exited 2 on argparse with an empty stdout, and the substring gate returned False
unconditionally. It also passed no --module/--bin/--base, so even with a size the
compare would have run against arm9_dec.bin for overlay functions (the same
coercion test_stamp_provenance_verify.py pins), and it passed the new mangled
name as --func even though renaming a file does not rename the function inside
it, so the symbol would not have been found in the object anyway. Three
independent defects, each alone enough to fail every verification.

apply_rename ran that gate as step 4, after git mv and after rewriting
config/match_provenance.jsonl, so a batch --apply renamed every file, printed
VERIFICATION FAILED for each, and left all of it in place.

These cases pin the fixed invocation (size from the module's own config
symbols.txt keyed by address, image from the module registry), the old-name
fallback, and the rollback contract: a failed verification restores provenance,
content, and path, and stops the batch.

    python -m unittest tools.test_cpp_rename_verify -v
"""
import json
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import cpp_rename as CR  # noqa: E402

OLD = "func_ov002_020ada40"
NEW = "_ZN3Foo3BarEv"
ADDR = 0x020ADA40
CANONICAL = "2004/b56"

PASS_OUT = f"""TARGET {OLD} @ 0x020ada40 size 0x100  bytes: 10402de9...
  {CANONICAL}: MATCH

========================================
MATCHING VERSIONS: {CANONICAL}
"""

NONE_OUT = f"""TARGET {OLD} @ 0x020ada40 size 0x100  bytes: 10402de9...
  {CANONICAL}: 4 word(s) differ

========================================
MATCHING VERSIONS: none
"""


def notfound(func):
    """match.py's output when --func names a symbol the object does not define."""
    return (f"TARGET {func} @ 0x020ada40 size 0x100  bytes: 10402de9...\n"
            f"  {CANONICAL}: symbol '{func}' not found in object\n\n"
            "========================================\n"
            "MATCHING VERSIONS: none\n")


class FakeRun:
    """Stand-in for subprocess.run that records every argv it is handed.

    Takes (stdout, returncode) pairs, consumed in call order; the last one
    repeats. Everything lands on stdout because verify_match reads the combined
    stream, the same as stamp_provenance.run_match.
    """

    def __init__(self, *outs):
        self.outs = list(outs)
        self.cmds = []

    def __call__(self, cmd, **kw):
        self.cmds.append(cmd)
        out, code = self.outs.pop(0) if len(self.outs) > 1 else self.outs[0]
        return subprocess.CompletedProcess(cmd, code, out, "")

    def flag(self, name, call=0):
        """The value following `name` in the recorded argv of call `call`."""
        cmd = self.cmds[call]
        return cmd[cmd.index(name) + 1]


class Invocation(unittest.TestCase):
    """What verify_match sends to match.py, and how it reads the answer."""

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        root = pathlib.Path(self._tmp.name)
        ov = root / "config" / "arm9" / "overlays" / "ov002"
        ov.mkdir(parents=True)
        (ov / "symbols.txt").write_text(
            f"{OLD} kind:function(arm,size=0x100) addr:0x{ADDR:08x}\n"
            "data_ov002_0212b890 kind:data(any) addr:0x0212b890\n",
            encoding="utf-8")
        (root / "config" / "arm9" / "symbols.txt").write_text(
            "func_02004100 kind:function(arm,size=0x20) addr:0x02004100\n",
            encoding="utf-8")
        self._bins = {"ov002": (root / "overlay_0002.bin", 0x020A8FC0),
                      "arm9": (root / "arm9_dec.bin", 0x02004000)}
        for p, _ in self._bins.values():
            p.write_bytes(b"")
        self._saved = (CR.REPO, CR.module_target, subprocess.run)
        CR.REPO = root
        CR.module_target = self._bins.get
        CR._size_cache.clear()

    def tearDown(self):
        CR.REPO, CR.module_target, subprocess.run = self._saved
        CR._size_cache.clear()
        self._tmp.cleanup()

    def _verify(self, fake, names=(OLD, NEW), module="ov002", addr=ADDR):
        subprocess.run = fake
        return CR.verify_match("src/x.cpp", list(names), module, addr)

    def test_command_carries_size_and_the_modules_own_image(self):
        """The defect: no --size (argparse exit 2), no --module/--bin/--base."""
        fake = FakeRun((PASS_OUT, 0))
        ok, reason = self._verify(fake)
        self.assertTrue(ok)
        self.assertEqual(reason, "")
        self.assertEqual(fake.flag("--size"), "0x100")
        self.assertEqual(fake.flag("--module"), "ov002")
        self.assertTrue(fake.flag("--bin").endswith("overlay_0002.bin"))
        self.assertEqual(fake.flag("--base"), "0x20a8fc0")
        self.assertEqual(fake.flag("--version"), CANONICAL)

    def test_arm9_uses_arm9_dec_at_its_base(self):
        fake = FakeRun((PASS_OUT, 0))
        ok, _ = self._verify(fake, names=("func_02004100",), module="arm9",
                             addr=0x02004100)
        self.assertTrue(ok)
        self.assertEqual(fake.flag("--size"), "0x20")
        self.assertTrue(fake.flag("--bin").endswith("arm9_dec.bin"))
        self.assertEqual(fake.flag("--base"), "0x2004000")

    def test_an_argparse_exit_is_a_reported_failure(self):
        """The old failure shape, empty stdout and exit 2, now carries a reason."""
        fake = FakeRun(("", 2))
        ok, reason = self._verify(fake)
        self.assertFalse(ok)
        self.assertIn("exited 2", reason)

    def test_a_none_verdict_fails_with_the_function_named(self):
        ok, reason = self._verify(FakeRun((NONE_OUT, 0)))
        self.assertFalse(ok)
        self.assertIn(OLD, reason)

    def test_no_verdict_line_is_not_a_pass(self):
        """A crashed match.py must not read as verified."""
        ok, reason = self._verify(FakeRun(("Traceback (most recent call last):\n", 0)))
        self.assertFalse(ok)
        self.assertIn("no MATCHING VERSIONS verdict", reason)

    def test_missing_size_refuses_before_spending_a_compile(self):
        """Data symbols carry no size; nothing to verify against, so no subprocess."""
        fake = FakeRun((PASS_OUT, 0))
        ok, reason = self._verify(fake, addr=0x0212B890)
        self.assertFalse(ok)
        self.assertIn("config symbols.txt", reason)
        self.assertEqual(fake.cmds, [])

    def test_unknown_module_refuses(self):
        ok, reason = self._verify(FakeRun((PASS_OUT, 0)), module="ov999")
        self.assertFalse(ok)
        self.assertIn("config symbols.txt", reason)

    def test_missing_binary_refuses(self):
        self._bins["ov002"][0].unlink()
        ok, reason = self._verify(FakeRun((PASS_OUT, 0)))
        self.assertFalse(ok)
        self.assertIn("binary missing", reason)

    def test_old_name_is_tried_first_then_the_mangled_one(self):
        """Renaming the file does not rename the function inside it."""
        fake = FakeRun((notfound(OLD), 0), (PASS_OUT, 0))
        ok, _ = self._verify(fake)
        self.assertTrue(ok)
        self.assertEqual(len(fake.cmds), 2)
        self.assertEqual(fake.flag("--func", call=0), OLD)
        self.assertEqual(fake.flag("--func", call=1), NEW)

    def test_neither_name_in_the_object_fails_with_both_named(self):
        fake = FakeRun((notfound(OLD), 0), (notfound(NEW), 0))
        ok, reason = self._verify(fake)
        self.assertFalse(ok)
        self.assertIn("defines none of", reason)
        self.assertIn(NEW, reason)


class ApplyRollback(unittest.TestCase):
    """A failed verification must leave the tree exactly as it was.

    Real throwaway git repositories, not mocks: the rollback is a git mv plus two
    file rewrites, and whether the index ends up clean is git's call to make.
    """

    SRC = "int func_ov002_020ada40(void) { return 1; }\n"

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self._tmp.name)
        self.git("init", "-q", "-b", "main")
        self.git("config", "user.email", "setup@example.com")
        self.git("config", "user.name", "Setup")
        (self.repo / "src").mkdir()
        self.old = self.repo / "src" / f"{OLD}.c"
        self.new = self.repo / "src" / f"{NEW}.cpp"
        self.old.write_text(self.SRC, encoding="utf-8")
        self.prov = self.repo / "config" / "match_provenance.jsonl"
        self.prov.parent.mkdir()
        rows = [{"id": f"ov002:0x{ADDR:08x}", "srcPath": f"src/{OLD}.c"},
                {"id": "arm9:0x02004100", "srcPath": "src/other.c"}]
        self.prov.write_text("".join(json.dumps(r) + "\n" for r in rows),
                             encoding="utf-8")
        self.git("add", "-A")
        self.git("commit", "-qm", "init")
        self._saved = (CR.REPO, CR.verify_match)
        CR.REPO = self.repo
        self.item = {"old_path": self.old, "new_path": self.new, "symbol": NEW,
                     "addr": f"0x{ADDR:08x}", "is_cpp": True}

    def tearDown(self):
        CR.REPO, CR.verify_match = self._saved
        self._tmp.cleanup()

    def git(self, *args):
        return subprocess.run(["git", *args], cwd=self.repo, check=True,
                              capture_output=True, text=True).stdout

    def src_paths(self):
        return [json.loads(l)["srcPath"] for l in
                self.prov.read_text(encoding="utf-8").splitlines()]

    def test_failure_restores_path_content_provenance_and_index(self):
        CR.verify_match = lambda *a, **k: (False, "forced")
        ok, reason = CR.apply_rename(self.item)
        self.assertFalse(ok)
        self.assertEqual(reason, "forced")
        self.assertTrue(self.old.is_file())
        self.assertFalse(self.new.exists())
        self.assertEqual(self.old.read_text(encoding="utf-8"), self.SRC)
        self.assertEqual(self.src_paths(), [f"src/{OLD}.c", "src/other.c"])
        self.assertEqual(self.git("status", "--porcelain"), "")

    def test_success_keeps_the_rename_and_the_provenance_update(self):
        CR.verify_match = lambda *a, **k: (True, "")
        ok, _ = CR.apply_rename(self.item)
        self.assertTrue(ok)
        self.assertFalse(self.old.exists())
        self.assertTrue(self.new.read_text(encoding="utf-8").startswith("//cpp\n"))
        self.assertEqual(self.src_paths(), [f"src/{NEW}.cpp", "src/other.c"])

    def test_success_hands_verify_both_names_and_the_right_target(self):
        seen = {}

        def spy(rel, names, module, addr):
            seen.update(rel=rel, names=names, module=module, addr=addr)
            return True, ""

        CR.verify_match = spy
        CR.apply_rename(self.item)
        self.assertEqual(seen["rel"], f"src/{NEW}.cpp")
        self.assertEqual(seen["names"], [OLD, NEW])
        self.assertEqual(seen["module"], "ov002")
        self.assertEqual(seen["addr"], ADDR)

    def test_an_underivable_module_fails_before_touching_the_tree(self):
        odd = self.repo / "src" / "func_zz12_02000000.c"
        odd.write_text(self.SRC, encoding="utf-8")
        self.git("add", "-A")
        self.git("commit", "-qm", "odd")
        CR.verify_match = lambda *a, **k: (True, "")
        ok, reason = CR.apply_rename({"old_path": odd, "new_path": self.new,
                                      "symbol": NEW, "addr": "0x02000000",
                                      "is_cpp": True})
        self.assertFalse(ok)
        self.assertIn("module", reason)
        self.assertTrue(odd.is_file())
        self.assertFalse(self.new.exists())
        self.assertEqual(self.git("status", "--porcelain"), "")


class BatchPolicy(unittest.TestCase):
    """A verification failure stops the batch; a per-file git error does not."""

    def setUp(self):
        self._saved = CR.apply_rename
        self.items = [{"old_path": CR.REPO / "src" / f"f{i}.c",
                       "new_path": CR.REPO / "src" / f"g{i}.cpp"}
                      for i in range(3)]

    def tearDown(self):
        CR.apply_rename = self._saved

    def test_stops_after_the_first_verification_failure(self):
        calls = []

        def fake(item):
            calls.append(item)
            return (len(calls) != 2), "no match" if len(calls) == 2 else ""

        CR.apply_rename = fake
        self.assertEqual(CR.apply_all(self.items), (1, 1, 1))
        self.assertEqual(len(calls), 2)

    def test_a_git_error_skips_the_file_and_continues(self):
        calls = []

        def fake(item):
            calls.append(item)
            if len(calls) == 2:
                raise subprocess.CalledProcessError(128, ["git", "mv"])
            return True, ""

        CR.apply_rename = fake
        self.assertEqual(CR.apply_all(self.items), (2, 1, 0))
        self.assertEqual(len(calls), 3)


if __name__ == "__main__":
    unittest.main()
