"""Fail-closed coverage for tubuild's vtable-partition licensing boundary.

WHY THIS MODULE EXISTS AND IS NOT PART OF ``tools/test_tubuild.py``.

Same reason ``tools/test_tubuild_owned_relocs.py`` exists, one boundary over.
``test_tubuild.py`` is pytest-style -- bare ``def test_*`` functions and zero
``unittest.TestCase`` classes -- so ``python -m unittest tools.test_tubuild``
prints "Ran 0 tests ... OK" and exits 0.  CI (``.github/workflows/tool-tests.yml``)
runs ``python -m unittest``, so nothing in that file has ever executed on a runner.
The four tests PR #2091 added to describe this boundary were written there, and
three of them need neither mwccarm nor ``extracted/``: they are mock- and
dict-driven, and they call only ``tubuild.*``.  They are moved here verbatim --
bodies unchanged, ``def f():`` to ``def f(self):`` and one indent level, nothing
else -- so the move is reviewable as a move and the assertions are the ones that
were reviewed on #2091.

WHAT THEY PIN, and why it is worth wiring rather than leaving decorative.

#2091 added a fail-closed licensing boundary for nested symbols carved out of a
vtable's public range: a partition named in the manifest is a COLLIDES-GAP refusal
unless ``partition_vtable_rebiases`` independently validated it against
content-bound baseline ELF metadata.  Measured on #2091's merge tree, that path
licensed nothing at all -- 0 of 97 manifest entries, 0 ``partition_symbols`` rows
tree-wide -- because no entry had ever carried such a row.  #2096
(``daPropeller_Heyho_c``) is what introduces the first two, ``FlyGuy_VT7`` and
``FlyGuy_VT14``.  So the boundary goes from unexercised to load-bearing in one
PR, and until this module it was described entirely by tests CI cannot see.

THE ONE INVARIANT THAT IS EASY TO RE-BREAK is the last test here, which is new
rather than moved.  ``validated_vtable_partition_symbols`` proves the part/parent
relationship with ``sectionIndex`` equality and deliberately does NOT compare
section NAMES.  It cannot: a linked ELF names a vtable's output section after its
overlay -- ``OV036``, ``OV047``, ``OV070`` measured on the baseline link -- while
a manifest ``section`` is one of ``.rodata .init .ctor .data .bss``.  The two
vocabularies never intersect, so an earlier form of this check that compared them
refused every real input, and the whole path was unreachable.  Commit 0b0e319
removed that comparison; the #2091 thread carries the A/B.  A future reader who
sees a "missing" section check and restores it would turn the boundary back into
an unconditional refusal, and no byte gate would notice, because a refusal here
is not a wrong ROM -- it is a TU that quietly stops being promotable.
"""

import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tubuild  # noqa: E402


class ManifestPartitionLicensing(unittest.TestCase):
    def test_object_audit_licenses_only_validated_manifest_vtable_partitions(self):
        from unittest import mock

        def partition(name, address, size):
            return {"symbol": name, "address": hex(address), "size": hex(size),
                    "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                    "visibility": "STV_DEFAULT", "reuse_policy_symbol": "donor"}

        def proven(name, address, size):
            return {"symbol": name, "address": address, "size": size,
                    "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                    "visibility": "STV_DEFAULT", "baseline": {
                        "symbol": {"address": address, "size": size,
                                   "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                                   "visibility": "STV_DEFAULT", "section": "OV999",
                                   "sectionIndex": 4},
                        "vtable": {"sectionIndex": 4},
                    }}

        entry = {"module": "ov999", "functions": [], "sections": [
            {"name": ".text", "start": "0x1000", "end": "0x1004"},
            {"name": ".data", "start": "0x2000", "end": "0x2030"}],
            "data": [{"symbol": "_ZTV1P", "address": "0x2008", "size": "0x30",
                      "partition_symbols": [partition("VT7", 0x2010, 8),
                                              partition("RawOnly", 0x2018, 0x18)]}],
            "bss": []}
        policies = {"_ZTV1P": {"section": ".data", "partitionSymbols": [
            proven("VT7", 0x2010, 8),
            proven("ArbitraryCollision", 0x2040, 8),
        ]}}
        inventory = {
            "sections": [{"index": 4, "name": ".data", "size": 0,
                          "type": "SHT_PROGBITS"}],
            "symbols": [
                {"name": name, "bind": "STB_GLOBAL", "type": "STT_OBJECT",
                 "size": size, "shndx": 4}
                for name, size in (("VT7", 8), ("RawOnly", 0x18),
                                   ("ArbitraryCollision", 8))],
        }
        homes = {"VT7": [("ov999", 0x2010)],
                 "RawOnly": [("ov999", 0x2018)],
                 "ArbitraryCollision": [("ov999", 0x2040)]}
        with mock.patch.object(tubuild, "elf_inventory", return_value=inventory), \
                mock.patch.object(tubuild, "all_symbol_homes", return_value=homes):
            rows, extra, _emitted, order_ok = tubuild.audit_tu_object(
                b"ignored", entry, 0x1000, 0x1004, ranges={"ov999": []},
                validated_vtable_policies=policies)

        verdicts = {row["name"]: row["verdict"] for row in rows}
        assert verdicts == {"VT7": "LICENSED", "RawOnly": "COLLIDES-GAP",
                            "ArbitraryCollision": "COLLIDES-GAP"}
        reasons = tubuild.object_audit_refusals(rows, extra, order_ok)
        assert not any("VT7" in reason for reason in reasons)
        assert any("RawOnly" in reason for reason in reasons)
        assert any("ArbitraryCollision" in reason for reason in reasons)

    def test_partition_licensing_pins_section_index_not_section_name(self):
        """A part is licensed by landing in the vtable's section INDEX, not its name.

        This is the invariant 0b0e319 left standing after it removed a section-NAME
        comparison that could never pass.  Both halves are asserted: the name is not
        consulted (a real overlay name, and no name at all, both license), and the
        index is (a part that landed elsewhere stays unlicensed).
        """
        import copy

        entry = {
            "module": "ov070", "functions": [],
            "data": [{"symbol": "_ZTV1P", "address": "0x2008", "size": "0x30",
                      "partition_symbols": [
                          {"symbol": "VT7", "address": "0x2010", "size": "0x8",
                           "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                           "visibility": "STV_DEFAULT",
                           "reuse_policy_symbol": "donor"}]}],
            "bss": [],
        }

        def policies(part_section_index, part_section_name):
            return {"_ZTV1P": {"section": ".data", "partitionSymbols": [
                {"symbol": "VT7", "address": 0x2010, "size": 8,
                 "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                 "visibility": "STV_DEFAULT",
                 "baseline": {
                     "symbol": {"address": 0x2010, "size": 8,
                                "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                                "visibility": "STV_DEFAULT",
                                "section": part_section_name,
                                "sectionIndex": part_section_index},
                     "vtable": {"section": "OV070", "sectionIndex": 147},
                 }}]}}

        # The shape the pipeline actually produces: the linked ELF calls the output
        # section OV070 on both the vtable and its parts, and the manifest calls the
        # input section .data.  This licenses.
        self.assertEqual(
            tubuild.validated_vtable_partition_symbols(entry, policies(147, "OV070")),
            {"VT7"})

        # The section NAME is not consulted at all -- dropping it changes nothing.
        # If this ever starts refusing, someone restored the impossible comparison.
        nameless = copy.deepcopy(policies(147, "OV070"))
        del nameless["_ZTV1P"]["partitionSymbols"][0]["baseline"]["symbol"]["section"]
        self.assertEqual(
            tubuild.validated_vtable_partition_symbols(entry, nameless), {"VT7"})

        # The section INDEX is consulted.  A symbol that landed in a different output
        # section is not a part of this vtable, whatever the manifest claims.
        self.assertEqual(
            tubuild.validated_vtable_partition_symbols(entry, policies(148, "OV070")),
            set())


class PartitionRebiasValidation(unittest.TestCase):
    def test_partitioned_vtable_interior_symbols_require_exact_policy_and_baseline(self):
        entry = {
            "module": "ov999", "functions": [],
            "externalized_output": [
                {"symbol": "_ZTS1B", "disposition": "canonical-import"},
                {"symbol": "_ZTS1A", "disposition": "canonical-import"},
            ],
            "data": [{
                "symbol": "_ZTV1P", "address": "0x2008",
                "emitted_storage_address": "0x2000", "address_point_bias": "0x8",
                "size": "0x30", "partition_symbols": [
                    {"symbol": "VT7", "address": "0x2010", "size": "0x8",
                     "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                     "visibility": "STV_DEFAULT", "reuse_policy_symbol": "_ZTS1B"},
                    {"symbol": "VT14", "address": "0x2018", "size": "0x18",
                     "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                     "visibility": "STV_DEFAULT", "reuse_policy_symbol": "_ZTS1A"},
                ],
            }], "bss": [],
        }
        claims = [{"name": ".data", "start": 0x2000, "end": 0x2030}]
        baseline = {
            "_ZTV1P": [{"address": 0x2008, "size": 8, "binding": "STB_GLOBAL",
                         "type": "STT_OBJECT", "visibility": "STV_DEFAULT",
                         "sectionIndex": 4, "section": "OV999"}],
            "VT7": [{"address": 0x2010, "size": 8, "binding": "STB_GLOBAL",
                     "type": "STT_OBJECT", "visibility": "STV_DEFAULT",
                     "sectionIndex": 4, "section": "OV999"}],
            "VT14": [{"address": 0x2018, "size": 0x18, "binding": "STB_GLOBAL",
                      "type": "STT_OBJECT", "visibility": "STV_DEFAULT",
                      "sectionIndex": 4, "section": "OV999"}],
        }
        homes = {"_ZTV1P": [("ov999", 0x2008)], "VT7": [("ov999", 0x2010)],
                 "VT14": [("ov999", 0x2018)]}
        original_homes = tubuild.all_symbol_homes
        original_linked = tubuild.linked_symbol_rows
        try:
            tubuild.all_symbol_homes = lambda: homes
            policies, reasons = tubuild.partition_vtable_rebiases(
                entry, claims, baseline_symbols=baseline, baseline_sha256="c" * 64)
            assert reasons == []
            policy = policies["_ZTV1P"]
            assert policy["publicSize"] == 8
            assert [(row["symbol"], row["value"], row["size"], row["donor"])
                    for row in policy["partitionSymbols"]] == [
                        ("VT7", 0x10, 8, "_ZTS1B"),
                        ("VT14", 0x18, 0x18, "_ZTS1A")]
            assert policy["baseline"]["elfSha256"] == "c" * 64
            assert [row[1]["symbol"] for row in
                    tubuild.manifest_vtable_partition_rows(entry)] == ["VT7", "VT14"]
            assert tubuild.validated_vtable_partition_symbols(entry, policies) == {
                "VT7", "VT14"}

            # This is the ordinary intact call shape: the manifest owns input ``.data``,
            # while the validated stock final-link metadata names its output section after
            # the module.  Same linked section index proves the parent/part relationship.
            inventory = {
                "sections": [{"index": 4, "name": ".data", "size": 0,
                              "type": "SHT_PROGBITS"}],
                "symbols": [{"name": name, "bind": "STB_GLOBAL",
                             "type": "STT_OBJECT", "size": size, "shndx": 4}
                            for name, size in (("VT7", 8), ("VT14", 0x18))],
            }
            from unittest import mock
            with mock.patch.object(tubuild, "elf_inventory", return_value=inventory):
                rows, extra, _emitted, order_ok = tubuild.audit_tu_object(
                    b"ignored", entry, 0x1000, 0x1004, ranges={"ov999": []},
                    validated_vtable_policies=policies)
            assert {row["name"]: row["verdict"] for row in rows} == {
                "VT7": "LICENSED", "VT14": "LICENSED"}
            assert tubuild.object_audit_refusals(rows, extra, order_ok) == []

            tubuild.linked_symbol_rows = lambda _path, _names: (baseline, None)
            proof = tubuild.verify_linked_storage_aliases("ignored.o", policies)
            assert proof["ok"] and proof["rows"][0]["exact"]
            wrong_baseline = {key: [dict(row) for row in value]
                              for key, value in baseline.items()}
            wrong_baseline["VT14"][0]["sectionIndex"] = 5
            _policies, reasons = tubuild.partition_vtable_rebiases(
                entry, claims, baseline_symbols=wrong_baseline,
                baseline_sha256="d" * 64)
            assert any("baseline metadata differs" in reason for reason in reasons)

            import copy
            overlap = copy.deepcopy(entry)
            overlap["data"][0]["partition_symbols"][1].update(
                {"address": "0x2014", "size": "0x1c"})
            _policies, reasons = tubuild.partition_vtable_rebiases(
                overlap, claims, baseline_symbols=baseline)
            assert any("overlaps" in reason for reason in reasons)

            gap = copy.deepcopy(entry)
            gap["data"][0]["partition_symbols"][1].update(
                {"address": "0x201c", "size": "0x14"})
            tubuild.all_symbol_homes = lambda: {
                **homes, "VT14": [("ov999", 0x201c)]}
            _policies, reasons = tubuild.partition_vtable_rebiases(
                gap, claims, baseline_symbols=baseline)
            assert any("leaves a gap" in reason for reason in reasons)

            bad_donor = copy.deepcopy(entry)
            bad_donor["data"][0]["partition_symbols"][0]["reuse_policy_symbol"] = "missing"
            tubuild.all_symbol_homes = lambda: homes
            _policies, reasons = tubuild.partition_vtable_rebiases(
                bad_donor, claims, baseline_symbols=baseline)
            assert any("not an explicit compiler-only" in reason for reason in reasons)

            reused = copy.deepcopy(entry)
            reused["data"][0]["partition_symbols"][1]["reuse_policy_symbol"] = "_ZTS1B"
            _policies, reasons = tubuild.partition_vtable_rebiases(
                reused, claims, baseline_symbols=baseline)
            assert any("donor _ZTS1B is reused" in reason for reason in reasons)
        finally:
            tubuild.all_symbol_homes = original_homes
            tubuild.linked_symbol_rows = original_linked


class IntactLinkGate(unittest.TestCase):
    def test_intact_link_gate_requires_final_vtable_split_symbol_fidelity(self):
        good = dict(module_ok=True, symbols_ok=True, rom_ok=True,
                    split_symbols_ok=True)
        assert tubuild.linkcheck_pipeline_ready(**good)
        bad = dict(good, split_symbols_ok=False)
        assert not tubuild.linkcheck_pipeline_ready(**bad)


if __name__ == "__main__":
    unittest.main()
