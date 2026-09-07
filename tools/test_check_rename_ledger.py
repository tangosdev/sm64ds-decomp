"""Tests for tools/check_rename_ledger.py.

A gate's FAILURE branch is the path a green run never exercises, and this one
rewrites a tracked file, so every case below is a defect that was actually
found in review of the first draft rather than an invented one:

  * `da[A-Z_]` classified `_ZTV7da1up_c` as a legacy spelling, so pick() saw two
    legacy candidates at an aliased address and refused a row it could resolve.
  * splitting on a single detected terminator merged an LF row into the previous
    CRLF row's last column: the merged row went unreported, and --fix would then
    have rewritten the following row's alloc= figure.
  * --fix exited 0 even with rows it had refused, so wiring it into CI would
    have gone green over known-bad rows.
  * a module with no symbols.txt was skipped in silence, shrinking coverage
    invisibly.
  * the correction overwrote column 4, and five retired names had no other row
    in the file -- AmbientSoundEffects has no other authoritative mention in the
    tree at all -- so the provenance had to be kept.
"""
import io
import os
import shutil
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check_rename_ledger as CRL  # noqa: E402

TOOL = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                    "check_rename_ledger.py")

HEADER = "module\taddr\told\tnew\twhy"

SYMBOLS = """\
    _ZN10daChRoom_cD1Ev kind:function addr:0x020b07f8 size:0x40
    _ZTV4Exit kind:data addr:0x021086b4 size:0xd4
    _ZTV11PowerFlower kind:data addr:0x02109800 size:0x88
    _ZTV18daObjPowerUpItem_c kind:data addr:0x02109800 size:0x88
"""


class Fixture(object):
    def __init__(self, rows, newline="\n", module="ov002", symbols=SYMBOLS):
        self.dir = tempfile.mkdtemp()
        cfg = os.path.join(self.dir, "config", "arm9", "overlays", module)
        os.makedirs(cfg)
        if symbols is not None:
            with io.open(os.path.join(cfg, "symbols.txt"), "w",
                         encoding="utf-8", newline="") as fh:
                fh.write(symbols)
        os.makedirs(os.path.join(self.dir, "symbols"))
        self.ledger = os.path.join(self.dir, CRL.LEDGER.replace("/", os.sep))
        body = newline.join([HEADER] + rows) + newline
        with io.open(self.ledger, "w", encoding="utf-8", newline="") as fh:
            fh.write(body)

    def read(self):
        with io.open(self.ledger, encoding="utf-8", newline="") as fh:
            return fh.read()

    def run(self, *args):
        return subprocess.run([sys.executable, TOOL, "--repo", self.dir] + list(args),
                              capture_output=True, text=True)

    def close(self):
        shutil.rmtree(self.dir, ignore_errors=True)


class TestSpelling(unittest.TestCase):
    def test_cartridge_name_with_a_digit(self):
        # da1up_c is a cartridge name; the old da[A-Z_] said legacy.
        self.assertTrue(CRL.is_cartridge_spelling("_ZTV7da1up_c"))

    def test_cartridge_and_legacy(self):
        self.assertTrue(CRL.is_cartridge_spelling("_ZTV18daObjPowerUpItem_c"))
        self.assertFalse(CRL.is_cartridge_spelling("_ZTV11PowerFlower"))
        self.assertFalse(CRL.is_cartridge_spelling("_ZTV6damage"))

    def test_pick_prefers_the_legacy_spelling(self):
        self.assertEqual(
            CRL.pick(["_ZTV11PowerFlower", "_ZTV18daObjPowerUpItem_c"]),
            "_ZTV11PowerFlower")

    def test_pick_is_fail_closed(self):
        self.assertIsNone(CRL.pick([]))
        self.assertIsNone(CRL.pick(["_ZTV1A", "_ZTV1B"]))
        self.assertIsNone(CRL.pick(["_ZTV4daA_c", "_ZTV4daB_c"]))


class TestAudit(unittest.TestCase):
    def test_finds_a_shifted_row_and_offers_the_true_owner(self):
        fx = Fixture(["ov002\t0x020b07f8\tfunc_ov002_020b07f8\t"
                      "_ZN10daCamTag_cD1Ev\tvtable slot 16"])
        try:
            _, _, findings, missing, stats = CRL.audit(fx.dir)
            self.assertEqual(len(findings), 1)
            self.assertEqual(findings[0][4], "_ZN10daChRoom_cD1Ev")
            self.assertEqual(missing, [])
            self.assertEqual(stats["checked"], 1)
        finally:
            fx.close()

    def test_coined_rows_are_counted_not_checked(self):
        fx = Fixture(["ov002\t0x020b07c8\tfunc_ov002_020b07c8\t"
                      "daCamTag_c_Spawn\tspawn func"])
        try:
            _, _, findings, _, stats = CRL.audit(fx.dir)
            self.assertEqual(findings, [])
            self.assertEqual(stats["checked"], 0)
            self.assertEqual(stats["unchecked"], 1)
        finally:
            fx.close()

    def test_mixed_line_endings_do_not_swallow_a_row(self):
        rows = ["ov002\t0x020b07f8\tfunc_ov002_020b07f8\t_ZN10daCamTag_cD1Ev\tvtable slot 16",
                "ov002\t0x021086b4\tdata_ov002_021086b4\t_ZTV11VirtualDoor\tvtable alloc=0xd4"]
        fx = Fixture([])
        try:
            with io.open(fx.ledger, "w", encoding="utf-8", newline="") as fh:
                fh.write(HEADER + "\r\n" + rows[0] + "\n" + rows[1] + "\r\n")
            _, _, findings, _, _ = CRL.audit(fx.dir)
            self.assertEqual(len(findings), 2)
        finally:
            fx.close()


class TestFix(unittest.TestCase):
    ROW = ("ov002\t0x021086b4\tdata_ov002_021086b4\t"
           "_ZTV11VirtualDoor\tvtable alloc=0xd4")

    def test_corrects_withdraws_the_size_and_keeps_the_retired_name(self):
        fx = Fixture([self.ROW])
        try:
            r = fx.run("--fix")
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
            line = fx.read().splitlines()[1]
            self.assertEqual(
                line,
                "ov002\t0x021086b4\tdata_ov002_021086b4\t_ZTV4Exit\t"
                "vtable alloc=? (was _ZTV11VirtualDoor)")
        finally:
            fx.close()

    def test_fix_is_idempotent_and_appends_the_provenance_once(self):
        fx = Fixture([self.ROW])
        try:
            fx.run("--fix")
            first = fx.read()
            second = fx.run("--fix")
            self.assertEqual(second.returncode, 0)
            self.assertEqual(fx.read(), first)
            self.assertEqual(first.count("(was"), 1)
        finally:
            fx.close()

    def test_round_trip_preserves_crlf_and_the_trailing_newline(self):
        fx = Fixture([self.ROW], newline="\r\n")
        try:
            fx.run("--fix")
            body = fx.read()
            self.assertTrue(body.endswith("\r\n"))
            self.assertNotIn("\n\n", body.replace("\r\n", "\n").rstrip("\n") + "\n")
        finally:
            fx.close()

    def test_clean_file_exits_zero_and_reports_the_unchecked_count(self):
        fx = Fixture(["ov002\t0x020b07f8\tfunc_ov002_020b07f8\t"
                      "_ZN10daChRoom_cD1Ev\tvtable slot 16"])
        try:
            r = fx.run()
            self.assertEqual(r.returncode, 0, r.stdout)
            self.assertIn("out of scope and unchecked", r.stdout)
        finally:
            fx.close()

    def test_dirty_file_exits_one_without_fix(self):
        fx = Fixture([self.ROW])
        try:
            r = fx.run()
            self.assertEqual(r.returncode, 1)
            self.assertEqual(fx.read().splitlines()[1], self.ROW)
        finally:
            fx.close()

    def test_fix_exits_one_when_it_refused_a_row(self):
        # Two legacy candidates at one address: pick() returns None, the row is
        # left alone, and the run must NOT report success.
        sym = SYMBOLS + "    _ZTV6Portal kind:data addr:0x021086b4 size:0xd4\n"
        fx = Fixture([self.ROW], symbols=sym)
        try:
            r = fx.run("--fix")
            self.assertEqual(r.returncode, 1, r.stdout)
            self.assertIn("1 left for a human", r.stdout)
            self.assertEqual(fx.read().splitlines()[1], self.ROW)
        finally:
            fx.close()

    def test_missing_symbols_txt_is_loud(self):
        fx = Fixture(["ov999\t0x02100000\tdata_ov999_02100000\t_ZTV1A\tvtable"],
                     symbols=None)
        try:
            r = fx.run()
            self.assertEqual(r.returncode, 1, r.stdout)
            self.assertIn("no symbols.txt for module(s): ov999", r.stdout)
        finally:
            fx.close()


if __name__ == "__main__":
    unittest.main()
