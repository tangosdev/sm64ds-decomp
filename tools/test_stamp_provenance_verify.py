"""The verify gate in tools/stamp_provenance.py: what it compares, and when it refuses.

Regression test for a gate that never fired. Until this landed, `run_match` had two
independent defects and either one alone was enough to bank a non-reproducing file:

  1. It sent every non-arm9/arm7 symbol to match.py as `--module arm9` with no
     --bin/--base, so an overlay function was compared against extracted/arm9_dec.bin.
     That image is 0x9cfb8 bytes; an overlay address sits past its end, the target
     slice came back empty, and match.py printed
     "size differs: target 0x0 vs candidate 0x104" for a file that reproduces the ROM
     byte for byte.

  2. It gated on `if "MATCH" not in out`. match.py's last line is always
     "MATCHING VERSIONS: <list or none>", so "MATCH" is a substring of the output on
     every run, failures included, and it also exits 0 either way. The guard could not
     fire, so 1. was invisible and so was a genuinely wrong candidate.

Net effect: a file that did not reproduce could be stamped into
config/match_provenance.jsonl, logged as a matched attempt, and have its near-miss tip
pruned. Only CI validate would catch it, days later.

These cases drive the failure branch directly, which is the branch a green run never
exercises. FAIL_OV and PASS_OV are real captured match.py output.

    python -m unittest tools.test_stamp_provenance_verify -v
"""
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import stamp_provenance as SP  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent

# Captured from tools/match.py --brief on the pre-fix coercion: an overlay candidate
# that reproduces the ROM, compared against arm9_dec.bin at an out-of-range offset.
FAIL_OV = """TARGET func_ov006_020cb030 @ 0x020cb030 size 0x104  bytes:
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x0 vs candidate 0x104

========================================
MATCHING VERSIONS: none
"""

PASS_OV = """TARGET func_ov006_020cb030 @ 0x020cb030 size 0x104  bytes: 10402de9...
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
"""

CANONICAL = "2004/b56"


class FakeRun:
    """Stand-in for subprocess.run that records the argv it was handed."""

    def __init__(self, out, returncode=0):
        self.out, self.returncode, self.cmd = out, returncode, None

    def __call__(self, cmd, **kw):
        self.cmd = cmd
        return subprocess.CompletedProcess(cmd, self.returncode, self.out, "")

    def flag(self, name):
        """The value following `name` in the recorded argv."""
        return self.cmd[self.cmd.index(name) + 1]


class Verdict(unittest.TestCase):
    """Reading match.py's verdict line instead of a substring of it."""

    def test_failure_output_contains_the_substring_the_old_guard_used(self):
        """The defect verbatim: 'MATCH' is present in output that matched nothing."""
        self.assertIn("MATCH", FAIL_OV)
        self.assertEqual(SP.matching_versions(FAIL_OV), [])

    def test_none_is_no_versions(self):
        self.assertEqual(SP.matching_versions("MATCHING VERSIONS: none"), [])

    def test_one_version(self):
        self.assertEqual(SP.matching_versions(PASS_OV), [CANONICAL])

    def test_several_versions(self):
        self.assertEqual(
            SP.matching_versions("MATCHING VERSIONS: 1.2/base, 1.2/sp2, 2004/b56"),
            ["1.2/base", "1.2/sp2", CANONICAL],
        )

    def test_no_verdict_line_is_not_an_empty_list(self):
        """A crashed match.py must be distinguishable from a clean non-match."""
        self.assertIsNone(SP.matching_versions("Traceback (most recent call last):\n"))

    def test_last_verdict_wins(self):
        """Only the final line grades the run."""
        two = "MATCHING VERSIONS: 2004/b56\nsecond pass\nMATCHING VERSIONS: none\n"
        self.assertEqual(SP.matching_versions(two), [])


class RunMatch(unittest.TestCase):
    """What run_match sends to match.py, and what it does with the answer."""

    def setUp(self):
        # run_match checks the module binary exists before it spends a compile, so the
        # stand-in registry points at real (empty) files rather than patching pathlib.
        self._tmp = tempfile.TemporaryDirectory()
        d = pathlib.Path(self._tmp.name)
        images = {"ov006": (d / "overlay_0006.bin", 0x020C7000),
                  "arm9": (d / "arm9_dec.bin", 0x02004000)}
        for p, _ in images.values():
            p.write_bytes(b"")
        self._run = subprocess.run
        self._target = SP.module_target
        SP.module_target = images.get

    def tearDown(self):
        subprocess.run = self._run
        SP.module_target = self._target
        self._tmp.cleanup()

    def _call(self, fake, module="ov006", version=CANONICAL):
        subprocess.run = fake
        SP.run_match(pathlib.Path("scratch/f.cpp"), "func_ov006_020cb030",
                     0x020CB030, 0x104, module, version)

    def test_overlay_failure_refuses_to_bank(self):
        with self.assertRaises(SystemExit) as e:
            self._call(FakeRun(FAIL_OV))
        self.assertIn("not banking", str(e.exception))

    def test_overlay_pass_banks(self):
        self._call(FakeRun(PASS_OV))  # no SystemExit

    def test_nonzero_exit_refuses_even_with_a_passing_verdict(self):
        with self.assertRaises(SystemExit) as e:
            self._call(FakeRun(PASS_OV, returncode=1))
        self.assertIn("exited 1", str(e.exception))

    def test_no_verdict_refuses(self):
        with self.assertRaises(SystemExit) as e:
            self._call(FakeRun("Traceback: match.py died\n"))
        self.assertIn("no MATCHING VERSIONS verdict", str(e.exception))

    def test_a_match_under_another_version_is_not_the_requested_one(self):
        with self.assertRaises(SystemExit) as e:
            self._call(FakeRun("MATCHING VERSIONS: 1.2/base\n"))
        self.assertIn("not the requested", str(e.exception))

    def test_overlay_is_compared_against_its_own_image(self):
        """The coercion: --module arm9 for an overlay symbol, and no --bin/--base."""
        fake = FakeRun(PASS_OV)
        self._call(fake)
        self.assertEqual(fake.flag("--module"), "ov006")
        self.assertTrue(fake.flag("--bin").endswith("overlay_0006.bin"))
        self.assertEqual(fake.flag("--base"), "0x20c7000")

    def test_arm9_still_uses_arm9_dec_at_0x02004000(self):
        fake = FakeRun(PASS_OV)
        self._call(fake, module="arm9")
        self.assertEqual(fake.flag("--module"), "arm9")
        self.assertTrue(fake.flag("--bin").endswith("arm9_dec.bin"))
        self.assertEqual(fake.flag("--base"), "0x2004000")

    def test_unresolvable_module_refuses_instead_of_falling_back_to_arm9(self):
        """arm7 has no module in the registry. Silence here is how 1. survived."""
        subprocess.run = FakeRun(PASS_OV)
        with self.assertRaises(SystemExit) as e:
            SP.run_match(pathlib.Path("scratch/f.c"), "f", 0x037F8000, 0x10,
                         "arm7", CANONICAL)
        self.assertIn("no target binary for module", str(e.exception))

    def test_zero_size_refuses(self):
        with self.assertRaises(SystemExit):
            SP.run_match(pathlib.Path("scratch/f.c"), "f", 0x02004000, 0, "arm9",
                         CANONICAL)


@unittest.skipUnless((REPO / "extracted" / "arm9_dec.bin").is_file(),
                     "needs extracted/ (ROM dump)")
class RegistryTargets(unittest.TestCase):
    """module_target against the real registry, when a checkout has the ROM.

    Guarded rather than mocked because the point is the values tools/modules.py
    actually produces. A checkout without extracted/ skips; it also cannot verify.
    """

    def test_arm9(self):
        binp, base = SP.module_target("arm9")
        self.assertEqual(binp, REPO / "extracted" / "arm9_dec.bin")
        self.assertEqual(base, 0x02004000)

    def test_overlay_base_is_its_lowest_config_symbol(self):
        """The delinked base, not the ROM overlay table's ramAddress."""
        import modules as MOD

        ov = next((m for m in MOD.modules() if m["name"].startswith("ov")), None)
        if ov is None:
            self.skipTest("no overlay modules in this checkout")
        binp, base = SP.module_target(ov["name"])
        self.assertEqual(binp, pathlib.Path(ov["bin"]))
        self.assertEqual(base, MOD._overlay_base(ov["syms"]))

    def test_an_overlay_address_is_outside_arm9(self):
        """Why the coercion could never work, measured rather than asserted."""
        arm9 = (REPO / "extracted" / "arm9_dec.bin").stat().st_size
        self.assertGreater(0x020CB030 - 0x02004000, arm9)

    def test_arm7_has_no_module(self):
        self.assertIsNone(SP.module_target("arm7"))


if __name__ == "__main__":
    unittest.main()
