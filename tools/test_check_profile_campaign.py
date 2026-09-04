"""Tests for tools/check_profile_campaign.py.

POSITIVE CONTROLS FIRST. The two failures that matter for a gate like this are
not "it reported something wrong" but "it reported nothing at all": a coined
ledger row asserting a name the tree does not carry MUST be reported, and a scan
that resolved almost nothing MUST exit non-zero instead of announcing a pass.
Those two are what the rest of this file is arranged around.

Every case is a fixture tree under a temporary directory -- no repo, no config/,
no compiler, no ROM -- so these run anywhere and cannot go quietly green because
the real tree changed underneath them. `unittest.TestCase` on purpose: pytest
collects these, and so does `python -m unittest`, which is how this repo's
workflows run tool tests. A file of bare `def test_*` functions contributes zero
tests to unittest and reports a green while asserting nothing, which is the
exact shape tool-tests.yml already refuses for test_tubuild.py.
"""
import io
import json
import os
import shutil
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import check_profile_campaign as C  # noqa: E402


REGISTRY_HEADER = [
    "profile_id", "overlay_resolution", "overlay", "profile_address",
    "current_profile_name", "proposed_profile_name",
    "factory_module", "factory_address",
    "current_factory_name", "proposed_factory_name", "factory_filename",
]


def registry_row(**kw):
    row = dict.fromkeys(REGISTRY_HEADER, "")
    row.update(kw)
    return row


class Fixture(unittest.TestCase):
    """A throwaway repo root, plus the floors lowered so fixtures can run.

    The floors are the vacuous-pass guard; a fixture necessarily sits below
    them, so they are lowered here and their real behaviour is asserted
    separately in ScanSizeGuard rather than disabled everywhere.
    """

    def setUp(self):
        self.repo = tempfile.mkdtemp(prefix="campaign-gate-")
        self.addCleanup(shutil.rmtree, self.repo, ignore_errors=True)
        self._floors = (C.MIN_SYMBOL_FILES, C.MIN_LEDGER_CLAIMS, C.MIN_REGISTRY_ROWS)
        C.MIN_SYMBOL_FILES, C.MIN_LEDGER_CLAIMS, C.MIN_REGISTRY_ROWS = 0, 0, 0
        self.addCleanup(self._restore)

    def _restore(self):
        C.MIN_SYMBOL_FILES, C.MIN_LEDGER_CLAIMS, C.MIN_REGISTRY_ROWS = self._floors

    # ---- fixture builders ----

    def write(self, rel, text, newline="\n"):
        p = os.path.join(self.repo, rel.replace("/", os.sep))
        os.makedirs(os.path.dirname(p), exist_ok=True)
        with io.open(p, "w", encoding="utf-8", newline="") as fh:
            fh.write(text.replace("\n", newline))
        return p

    def symbols(self, module, entries):
        """entries: [(name, addr, kind)] -> that module's symbols.txt."""
        body = "".join("%s kind:%s addr:%s\n" % (n, k, a) for n, a, k in entries)
        rel = ("config/arm9/symbols.txt" if module == "arm9"
               else "config/arm9/overlays/%s/symbols.txt" % module)
        self.write(rel, body)

    def delinks(self, module, entries):
        """entries: [(path, start, end)] -> that module's delinks.txt."""
        body = ".text       start:0x02000000 end:0x03000000 kind:code align:32\n\n"
        for path, start, end in entries:
            body += "%s:\n    complete\n    .text start:%s end:%s\n\n" % (path, start, end)
        rel = ("config/arm9/delinks.txt" if module == "arm9"
               else "config/arm9/overlays/%s/delinks.txt" % module)
        self.write(rel, body)

    def ledger(self, rows, newline="\n"):
        body = "module\taddr\told\tnew\twhy\n"
        for r in rows:
            body += "\t".join(r) + "\n"
        self.write(C.LEDGER, body, newline=newline)

    def registry(self, rows):
        body = "\t".join(REGISTRY_HEADER) + "\n"
        for r in rows:
            body += "\t".join(r[c] for c in REGISTRY_HEADER) + "\n"
        self.write(C.REGISTRY, body)

    def baseline(self, entries):
        self.write(C.BASELINE, json.dumps({"_comment": "", "known": entries}, indent=2))

    # ---- runner ----

    def run_tool(self, *args):
        buf = io.StringIO()
        old = sys.stdout
        sys.stdout = buf
        try:
            rc = C.main(["--repo", self.repo] + list(args))
        finally:
            sys.stdout = old
        return rc, buf.getvalue()


class CoinedLedgerRows(Fixture):
    """Scan 2 -- the blindspot this tool exists to close."""

    def test_a_claim_symbols_txt_does_not_carry_is_reported(self):
        """THE control. This is PROPELLER_HEYHO, reduced.

        The ledger asserts daPropeller_Heyho_c_classInit at ov070 0x02120520
        while symbols.txt still spells it FlyGuy_Spawn -- a name the build has
        never emitted, sitting green because check_rename_ledger.py skips
        coined spellings. If this test ever passes silently the gate is
        pointless.
        """
        self.symbols("ov070", [("FlyGuy_Spawn", "0x02120520", "function(arm,size=0x50)")])
        self.ledger([
            ("ov070", "0x02120520", "func_ov070_02120520", "FlyGuy_Spawn", "spawnfunc"),
            ("ov070", "0x02120520", "FlyGuy_Spawn", "daPropeller_Heyho_c_classInit", "x"),
        ])
        rc, out = self.run_tool()
        self.assertEqual(rc, 1)
        self.assertIn("daPropeller_Heyho_c_classInit", out)
        self.assertIn("FlyGuy_Spawn", out)
        self.assertIn("coined ledger row(s) claim a name symbols.txt does not carry", out)

    def test_an_applied_claim_passes(self):
        self.symbols("ov070", [("daPropeller_Heyho_c_classInit", "0x02120520",
                                "function(arm,size=0x50)")])
        self.ledger([
            ("ov070", "0x02120520", "FlyGuy_Spawn", "daPropeller_Heyho_c_classInit", "x"),
        ])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("1 live coined ledger claim(s)", out)

    def test_only_the_last_row_on_an_address_is_the_live_claim(self):
        """The ledger is an append log, not a set of independent assertions.

        The first row's `FlyGuy_Spawn` is superseded history the moment a second
        row renames it. Checking every row in isolation would report the whole
        campaign as broken.
        """
        self.symbols("ov070", [("daPropeller_Heyho_c_classInit", "0x02120520",
                                "function(arm,size=0x50)")])
        self.ledger([
            ("ov070", "0x02120520", "func_ov070_02120520", "FlyGuy_Spawn", "spawnfunc"),
            ("ov070", "0x02120520", "FlyGuy_Spawn", "daPropeller_Heyho_c_classInit", "x"),
        ])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertNotIn("FlyGuy_Spawn", out)

    def test_mangled_rows_are_left_to_check_rename_ledger(self):
        """Not this tool's job, and double-reporting them would be noise."""
        self.symbols("ov070", [("something_else", "0x02120520", "function(arm,size=0x4)")])
        self.ledger([
            ("ov070", "0x02120520", "func_ov070_02120520", "_ZN7daKrb_cD1Ev", "slot 16"),
            ("ov070", "0x02120524", "data_ov070_02120524", "_ZTV7daKrb_c", "vtable"),
        ])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("0 live coined ledger claim(s)", out)

    def test_an_aliased_address_passes_on_membership_not_equality(self):
        """A descriptor can carry both a coined and a cartridge spelling."""
        self.symbols("ov029", [("g_profile_WC_OBJ01", "0x02113c08", "data(any)"),
                               ("data_ov029_02113c08", "0x02113c08", "data(any)")])
        self.ledger([("ov029", "0x02113c08", "old", "g_profile_WC_OBJ01", "x")])
        self.assertEqual(self.run_tool()[0], 0)

    def test_a_module_with_no_symbols_txt_is_a_divergence_not_a_pass(self):
        self.ledger([("ov999", "0x02113c08", "old", "g_profile_GONE", "x")])
        rc, out = self.run_tool()
        self.assertEqual(rc, 1)
        self.assertIn("g_profile_GONE", out)

    def test_crlf_ledger_is_read_without_merging_rows(self):
        """A CR left on the last column silently changes the claim being checked."""
        self.symbols("ov070", [("daPropeller_Heyho_c_classInit", "0x02120520",
                                "function(arm,size=0x50)")])
        self.ledger([("ov070", "0x02120520", "old", "daPropeller_Heyho_c_classInit", "x")],
                    newline="\r\n")
        self.assertEqual(self.run_tool()[0], 0)

    def test_arm9_symbols_live_at_config_arm9_symbols_txt(self):
        """Not under a `main/` subdirectory, which is the spelling people write."""
        self.symbols("arm9", [("g_profile_BOOT", "0x020914a8", "data(any)")])
        self.ledger([("arm9", "0x020914a8", "old", "g_profile_BOOT", "x")])
        self.assertEqual(self.run_tool()[0], 0)


class Ratchet(Fixture):
    def _diverged_tree(self):
        self.symbols("ov070", [("FlyGuy_Spawn", "0x02120520", "function(arm,size=0x50)")])
        self.ledger([("ov070", "0x02120520", "FlyGuy_Spawn",
                      "daPropeller_Heyho_c_classInit", "x")])

    def test_a_banked_divergence_does_not_fail_the_build(self):
        self._diverged_tree()
        self.baseline([{"module": "ov070", "address": "0x02120520",
                        "claimed": "daPropeller_Heyho_c_classInit",
                        "actual": ["FlyGuy_Spawn"], "note": "known"}])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("1 diverge, 1 of them banked", out)

    def test_the_banked_count_is_printed_so_green_is_not_read_as_clean(self):
        self._diverged_tree()
        self.baseline([{"module": "ov070", "address": "0x02120520",
                        "claimed": "daPropeller_Heyho_c_classInit",
                        "actual": ["FlyGuy_Spawn"], "note": "known"}])
        self.assertIn("banked in", self.run_tool()[1])

    def test_banking_is_not_a_permanent_exemption_for_the_address(self):
        """Change either side and the entry stops matching.

        Otherwise a bank would license every future drift on that address,
        which is how a ratchet quietly becomes an allowlist.
        """
        self._diverged_tree()
        self.baseline([{"module": "ov070", "address": "0x02120520",
                        "claimed": "daPropeller_Heyho_c_classInit",
                        "actual": ["SomethingElse_Spawn"], "note": "stale"}])
        self.assertEqual(self.run_tool()[0], 1)

    def test_update_writes_todays_divergences(self):
        self._diverged_tree()
        self.assertEqual(self.run_tool("--update")[0], 0)
        with io.open(os.path.join(self.repo, C.BASELINE), encoding="utf-8") as fh:
            data = json.load(fh)
        self.assertEqual(len(data["known"]), 1)
        self.assertEqual(data["known"][0]["claimed"], "daPropeller_Heyho_c_classInit")
        self.assertEqual(self.run_tool()[0], 0)


class ScanSizeGuard(Fixture):
    """A gate that checks nothing must not report a pass."""

    def setUp(self):
        super().setUp()
        C.MIN_SYMBOL_FILES, C.MIN_LEDGER_CLAIMS = 15, 300

    def test_an_empty_scan_exits_non_zero(self):
        self.symbols("ov070", [("FlyGuy_Spawn", "0x02120520", "function(arm,size=0x50)")])
        self.ledger([("ov070", "0x02120520", "old", "FlyGuy_Spawn", "x")])
        rc, out = self.run_tool()
        self.assertEqual(rc, 2)
        self.assertIn("SCAN TOO SMALL", out)
        self.assertIn("Refusing to report a pass", out)

    def test_the_guard_runs_before_any_verdict(self):
        """A broken reader must not be able to produce OK, nor a FAIL list."""
        self.ledger([])
        rc, out = self.run_tool()
        self.assertEqual(rc, 2)
        self.assertNotIn("OK:", out)


class RegistryScan(Fixture):
    def _tree(self):
        self.symbols("ov002", [
            ("daObjSwitch_c_classInit_HANSWITCH", "0x020baadc", "function(arm,size=0x30)"),
            ("g_profile_HANSWITCH", "0x02109900", "data(any)"),
        ])
        self.delinks("ov002", [("src/d_a_obj_switch_hanswitch.c", "0x020baadc", "0x020bab0c")])
        self.ledger([])

    def _row(self, **kw):
        base = dict(profile_id="HANSWITCH", overlay_resolution="unique_registry_context",
                    overlay="ov002", profile_address="0x02109900",
                    current_profile_name="ExclamationSwitch_SpawnInfo",
                    proposed_profile_name="g_profile_HANSWITCH",
                    factory_module="ov002", factory_address="0x020baadc",
                    current_factory_name="ExclamationSwitch_Spawn",
                    proposed_factory_name="daObjSwitch_c_classInit_HANSWITCH",
                    factory_filename="d_a_obj_switch_hanswitch.c")
        base.update(kw)
        return registry_row(**base)

    def test_a_fully_migrated_row_passes(self):
        self._tree()
        self.registry([self._row()])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("1 in scope", out)

    def test_a_row_still_on_its_pre_campaign_name_is_pending_not_a_failure(self):
        self.symbols("ov002", [
            ("ExclamationSwitch_Spawn", "0x020baadc", "function(arm,size=0x30)"),
            ("ExclamationSwitch_SpawnInfo", "0x02109900", "data(any)"),
        ])
        self.delinks("ov002", [("src/ExclamationSwitch_Spawn.c", "0x020baadc", "0x020bab0c")])
        self.ledger([])
        self.registry([self._row()])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("PENDING", out)
        self.assertIn("g_profile_HANSWITCH", out)

    def test_strict_turns_pending_into_a_failure(self):
        """The switch to flip when the campaign closes."""
        self.symbols("ov002", [
            ("ExclamationSwitch_Spawn", "0x020baadc", "function(arm,size=0x30)"),
            ("ExclamationSwitch_SpawnInfo", "0x02109900", "data(any)"),
        ])
        self.delinks("ov002", [("src/ExclamationSwitch_Spawn.c", "0x020baadc", "0x020bab0c")])
        self.ledger([])
        self.registry([self._row()])
        self.assertEqual(self.run_tool("--strict")[0], 1)

    def test_a_name_that_is_neither_current_nor_proposed_is_a_failure(self):
        """Not pending: no wave leaves a symbol in that state."""
        self.symbols("ov002", [
            ("who_is_this", "0x020baadc", "function(arm,size=0x30)"),
            ("g_profile_HANSWITCH", "0x02109900", "data(any)"),
        ])
        self.delinks("ov002", [("src/x.c", "0x020baadc", "0x020bab0c")])
        self.ledger([])
        self.registry([self._row()])
        rc, out = self.run_tool()
        self.assertEqual(rc, 1)
        self.assertIn("factory symbol diverged", out)

    def test_superseded_rows_are_out_of_scope(self):
        self._tree()
        self.registry([self._row(overlay_resolution="superseded_by_ov002",
                                 proposed_factory_name="nonsense_that_would_fail")])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("0 in scope (1 superseded)", out)

    def test_a_factory_folded_into_a_promoted_tu_is_exempt_and_it_is_measured(self):
        """Two functions inside the owning delinks range means a TU.

        Detected from the tree, never a list of known TUs: the range in ov070
        that owns daPropeller_Heyho_c's factory holds many functions, and the
        exemption has to follow from that, not from the file's name.
        """
        self.symbols("ov070", [
            ("daPropeller_Heyho_c_classInit", "0x02120520", "function(arm,size=0x50)"),
            ("_ZN18daPropeller_Heyho_cD1Ev", "0x0211f100", "function(arm,size=0x40)"),
            ("g_profile_PROPELLER_HEYHO", "0x02121000", "data(any)"),
        ])
        self.delinks("ov070", [("src/actors/daPropeller_Heyho_c.cpp", "0x0211f000",
                                "0x02120570")])
        self.ledger([])
        self.registry([self._row(
            profile_id="PROPELLER_HEYHO", overlay="ov070", profile_address="0x02121000",
            proposed_profile_name="g_profile_PROPELLER_HEYHO",
            current_profile_name="FlyGuy_SpawnInfo",
            factory_module="ov070", factory_address="0x02120520",
            current_factory_name="FlyGuy_Spawn",
            proposed_factory_name="daPropeller_Heyho_c_classInit",
            factory_filename="d_a_propeller_heyho.c")])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("1 factory source(s) exempt", out)

    def test_a_standalone_source_on_the_wrong_filename_is_pending(self):
        self.symbols("ov002", [
            ("daObjSwitch_c_classInit_HANSWITCH", "0x020baadc", "function(arm,size=0x30)"),
            ("g_profile_HANSWITCH", "0x02109900", "data(any)"),
        ])
        self.delinks("ov002", [("src/ExclamationSwitch_Spawn.c", "0x020baadc", "0x020bab0c")])
        self.ledger([])
        self.registry([self._row()])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("d_a_obj_switch_hanswitch.c", out)
        self.assertIn("filename", out)

    def test_a_factory_no_delinks_entry_owns_is_a_failure(self):
        self.symbols("ov002", [
            ("daObjSwitch_c_classInit_HANSWITCH", "0x020baadc", "function(arm,size=0x30)"),
            ("g_profile_HANSWITCH", "0x02109900", "data(any)"),
        ])
        self.delinks("ov002", [("src/elsewhere.c", "0x02000000", "0x02000010")])
        self.ledger([])
        self.registry([self._row()])
        rc, out = self.run_tool()
        self.assertEqual(rc, 1)
        self.assertIn("factory source unlocatable", out)


class DegradesHonestly(Fixture):
    """The dataset is on PR #2235, not on main. That must not crash or pass."""

    def test_a_missing_registry_is_reported_and_skipped(self):
        self.symbols("ov070", [("daPropeller_Heyho_c_classInit", "0x02120520",
                                "function(arm,size=0x50)")])
        self.ledger([("ov070", "0x02120520", "old", "daPropeller_Heyho_c_classInit", "x")])
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("registry scan SKIPPED", out)
        self.assertIn("PR #2235", out)

    def test_the_ledger_scan_still_runs_without_the_registry(self):
        """Skipping scan 1 must not take scan 2 down with it."""
        self.symbols("ov070", [("FlyGuy_Spawn", "0x02120520", "function(arm,size=0x50)")])
        self.ledger([("ov070", "0x02120520", "old", "daPropeller_Heyho_c_classInit", "x")])
        rc, out = self.run_tool()
        self.assertEqual(rc, 1)
        self.assertIn("daPropeller_Heyho_c_classInit", out)

    def test_missing_columns_are_named_rather_than_guessed_at(self):
        self.symbols("ov002", [("g_profile_HANSWITCH", "0x02109900", "data(any)")])
        self.ledger([])
        self.write(C.REGISTRY, "profile_id\toverlay\n" + "HANSWITCH\tov002\n")
        rc, out = self.run_tool()
        self.assertEqual(rc, 0)
        self.assertIn("registry scan SKIPPED", out)
        self.assertIn("factory_filename", out)
        self.assertIn("overlay_resolution", out)

    def test_an_empty_registry_does_not_pass_as_a_complete_campaign(self):
        self.symbols("ov002", [("g_profile_HANSWITCH", "0x02109900", "data(any)")])
        self.ledger([])
        self.write(C.REGISTRY, "\t".join(REGISTRY_HEADER) + "\n")
        rc, out = self.run_tool()
        self.assertIn("registry scan SKIPPED", out)
        self.assertNotIn("in scope", out)

    def test_strict_refuses_a_missing_dataset(self):
        self.symbols("ov070", [("daPropeller_Heyho_c_classInit", "0x02120520",
                                "function(arm,size=0x50)")])
        self.ledger([("ov070", "0x02120520", "old", "daPropeller_Heyho_c_classInit", "x")])
        rc, out = self.run_tool("--strict")
        self.assertEqual(rc, 1)
        self.assertIn("--strict requires the registry dataset", out)


if __name__ == "__main__":
    unittest.main()
