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
import class_rename  # noqa: E402

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
    def __init__(self, rows, newline="\n", module="ov002", symbols=SYMBOLS,
                 cfg_parts=None):
        self.dir = tempfile.mkdtemp()
        if cfg_parts is None:
            cfg_parts = ("config", "arm9", "overlays", module)
        cfg = os.path.join(self.dir, *cfg_parts)
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

    def test_tcm_autoload_symbols_live_under_arm9(self):
        # itcm's table is config/arm9/itcm/symbols.txt, not a top-level itcm directory under config.
        sym = "_ZN7dBgW_Kc11SetDepthDirEPK7Vector3 kind:function(arm,size=0x1c) addr:0x01ffb07c\n"
        fx = Fixture(["itcm\t0x01ffb07c\tfunc_01ffb07c\t"
                      "_ZN7dBgW_Kc11SetDepthDirEPK7Vector3\tmember setter"],
                     module="itcm", symbols=sym, cfg_parts=("config", "arm9", "itcm"))
        try:
            r = fx.run()
            self.assertEqual(r.returncode, 0, r.stdout)
            self.assertNotIn("no symbols.txt", r.stdout)
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


def ledger_lines(fx):
    with io.open(fx.ledger, "rb") as fh:
        raw = fh.read().decode("utf-8")
    lines, _newline = CRL.split_lines(raw)
    return lines


class TestChains(unittest.TestCase):
    """Temporary ledgers. The live file is not the fixture.

    A healthy chain is one hop. An append-only lineage is the coined name
    in the middle, still reachable. An in-place rewrite points the first
    row at the final name and leaves the lineage row sourcing the coined
    name, which nothing targets.
    """

    def test_healthy_chain_resolves(self):
        fx = Fixture(["ov002\t0x020b07f8\tfunc_ov002_020b07f8\t"
                      "_ZN10daChRoom_cD1Ev\tvtable slot 16"])
        try:
            self.assertEqual(CRL.chain_breaks(ledger_lines(fx)), [])
            r = fx.run()
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        finally:
            fx.close()

    def test_inplace_rewrite_severs_the_chain(self):
        fx = Fixture([
            "ov002\t0x020b07f8\tfunc_ov002_020b07f8\t_ZN10daChRoom_cD1Ev\t"
            "vtable slot 16 (was _ZN9BlueFlameD1Ev)",
            "ov002\t0x020b07f8\t_ZN9BlueFlameD1Ev\t_ZN10daChRoom_cD1Ev\t"
            "class renamed",
        ])
        try:
            breaks = CRL.chain_breaks(ledger_lines(fx))
            self.assertEqual([b[3] for b in breaks], ["_ZN9BlueFlameD1Ev"])
            r = fx.run()
            self.assertEqual(r.returncode, 1, r.stdout + r.stderr)
            self.assertIn("not reachable", r.stdout)
            # The gate reports the break. It does not rewrite the ledger to
            # paper over it.
            self.assertIn("(was _ZN9BlueFlameD1Ev)", fx.read())
        finally:
            fx.close()

    def test_append_only_lineage_resolves(self):
        # Column 4 of the first row is the coined mangled name. It is history.
        # The last row is the live name, and it matches symbols.txt.
        fx = Fixture([
            "ov002\t0x020b07f8\tfunc_ov002_020b07f8\t_ZN9BlueFlameD1Ev\t"
            "vtable slot 16",
            "ov002\t0x020b07f8\t_ZN9BlueFlameD1Ev\t_ZN10daChRoom_cD1Ev\t"
            "class renamed",
        ])
        try:
            self.assertEqual(CRL.chain_breaks(ledger_lines(fx)), [])
            r = fx.run()
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
            self.assertIn("superseded history", r.stdout)
        finally:
            fx.close()

    def test_appended_repair_keeps_the_live_name_and_resolves(self):
        # Same severed pair as above, then two appended rows: the missing hop
        # into the coined name, and a restatement of the live name so the last
        # row does not change what readers take as current.
        fx = Fixture([
            "ov002\t0x020b07f8\tfunc_ov002_020b07f8\t_ZN10daChRoom_cD1Ev\t"
            "vtable slot 16 (was _ZN9BlueFlameD1Ev)",
            "ov002\t0x020b07f8\t_ZN9BlueFlameD1Ev\t_ZN10daChRoom_cD1Ev\t"
            "lineage",
            "ov002\t0x020b07f8\tfunc_ov002_020b07f8\t_ZN9BlueFlameD1Ev\t"
            "chain repair",
            "ov002\t0x020b07f8\t_ZN9BlueFlameD1Ev\t_ZN10daChRoom_cD1Ev\t"
            "chain repair live",
        ])
        try:
            self.assertEqual(CRL.chain_breaks(ledger_lines(fx)), [])
            r = fx.run()
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        finally:
            fx.close()


class TestLedgerAppend(unittest.TestCase):
    def test_appends_a_hop_and_leaves_the_earlier_row(self):
        text = ("module\taddr\told\tnew\twhy\r\n"
                "ov085\t0x0212a6d4\tfunc_ov085_0212a6d4\t_ZN6RabbitD1Ev\t"
                "vtable slot 16\r\n")
        rs = class_rename.rules("Rabbit", "daMip_c", False)
        out, rows = class_rename.append_ledger_lineage(text, rs, "Rabbit", "daMip_c")
        self.assertTrue(out.startswith(text))
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0].split("\t")[:4],
                         ["ov085", "0x0212a6d4", "_ZN6RabbitD1Ev", "_ZN7daMip_cD1Ev"])
        self.assertNotIn("\n", out.replace("\r\n", ""))
        again, more = class_rename.append_ledger_lineage(out, rs, "Rabbit", "daMip_c")
        self.assertEqual(more, [])
        self.assertEqual(again, out)
        lines, _newline = CRL.split_lines(out)
        self.assertEqual(CRL.chain_breaks(lines), [])

    def test_a_severed_ledger_is_not_rewritten_in_place(self):
        text = ("module\taddr\told\tnew\twhy\n"
                "ov085\t0x0212a6d4\tfunc_ov085_0212a6d4\t_ZN7daMip_cD1Ev\t"
                "vtable slot 16 (was _ZN6RabbitD1Ev)\n"
                "ov085\t0x0212a6d4\t_ZN6RabbitD1Ev\t_ZN7daMip_cD1Ev\tlineage\n")
        rs = class_rename.rules("Rabbit", "daMip_c", False)
        out, rows = class_rename.append_ledger_lineage(text, rs, "Rabbit", "daMip_c")
        self.assertEqual(rows, [])
        self.assertEqual(out, text)

    def test_derived_symbol_appends_only_with_derived(self):
        text = ("module\taddr\told\tnew\twhy\n"
                "ov085\t0x0212cc2c\tfunc_ov085_0212cc2c\tRabbit_Spawn\tspawnfunc\n")
        derived = class_rename.rules("Rabbit", "daMip_c", True)
        _out, rows = class_rename.append_ledger_lineage(
            text, derived, "Rabbit", "daMip_c")
        self.assertEqual(rows[0].split("\t")[2:4], ["Rabbit_Spawn", "daMip_c_Spawn"])
        bare = class_rename.rules("Rabbit", "daMip_c", False)
        out, rows = class_rename.append_ledger_lineage(text, bare, "Rabbit", "daMip_c")
        self.assertEqual(rows, [])
        self.assertEqual(out, text)


if __name__ == "__main__":
    unittest.main()
