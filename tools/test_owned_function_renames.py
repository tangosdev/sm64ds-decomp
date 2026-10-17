"""Synthetic policy/ELF regressions only: no compiler, ROM or saved native objects."""
import copy
import io
import json
import pathlib
import struct
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import objisolate as OI
import reloc_audit as RA
import tu_manifest as TUM
import tubuild as TB
from elftools.elf.elffile import ELFFile

OLD, NEW = "__arraydtor$332", "__arraydtor$328"


def fixture(helper_name=OLD, helper_type=2, helper_bind=0, helper_size=36,
            helper_section=5, alpha=True, extra=()):
    names = [".shstrtab", ".strtab", ".symtab", ".text", ".text.helper", ".data", ".rela.text", ".rela.text.helper"]
    symbols = [("@333", 0, 4, 0, 1, 6), ("@337", 4, 4, 0, 1, 6),
               (helper_name, 0, helper_size, helper_bind, helper_type, helper_section)]
    if alpha:
        symbols.append(("alpha", 0, 4, 1, 2, 4))
    symbols += list(extra)
    def strings(values):
        raw = b"\0"
        offsets = {"": 0}
        for value in values:
            if value not in offsets:
                offsets[value] = len(raw)
                raw += value.encode() + b"\0"
        return raw, offsets
    shstr, shoff = strings(names)
    strtab, st = strings([s[0] for s in symbols])
    symtab = bytes(16) + b"".join(struct.pack("<IIIBBH", st[n], v, z, (b << 4) | t, 0, sec)
                                   for n, v, z, b, t, sec in symbols)
    data_ordinal = next(i for i, s in enumerate(symbols, 1) if s[0] == "@333")
    blobs = [b"", shstr, strtab, symtab, bytes(4), bytes(36), b"12345678",
             struct.pack("<IIi", 0, (3 << 8) | 2, 0),
             struct.pack("<IIi", 32, (data_ordinal << 8) | 2, 0)]
    types = [0, 3, 3, 2, 1, 1, 1, 4, 4]
    flags = [0, 0, 0, 0, 6, 6, 3, 0, 0]
    links = [0, 0, 0, 2, 0, 0, 0, 3, 3]
    infos = [0, 0, 0, 4, 0, 0, 0, 4, 5]
    sizes = [0, 0, 0, 16, 0, 0, 0, 12, 12]
    cur, offsets = 52, []
    for b in blobs:
        offsets.append(cur if b else 0)
        cur += len(b)
    out = bytearray(struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + bytes(9),
                                1, 40, 1, 0, 0, cur, 0, 52, 0, 0, 40, len(blobs), 1))
    out.extend(b"".join(blobs))
    for i in range(len(blobs)):
        out.extend(struct.pack("<10I", shoff.get(names[i-1], 0) if i else 0, types[i], flags[i], 0,
                               offsets[i], len(blobs[i]), links[i], infos[i], 4, sizes[i]))
    return bytes(out)


def entry():
    return {"id": "ov004/Fixture", "module": "ov004",
            "sections": [{"name": ".text", "start": "0x1000", "end": "0x1028"}],
            "functions": [{"symbol": "alpha", "address": "0x1000", "size": "0x4", "ordinal": 0},
                          {"symbol": NEW, "address": "0x1004", "size": "0x24", "ordinal": 125}],
            "data": [{"symbol": "@329"}, {"symbol": "@333"}],
            "defined_symbol_renames": [
                {"from": "@333", "to": "@329", "evidence": "old nontext overlap"},
                {"from": "@337", "to": "@333", "evidence": "old nontext overlap"},
                {"from": OLD, "to": NEW, "evidence": "synthetic finite correspondence",
                 "owned_function": {"module": "ov004", "address": "0x1004", "size": "0x24",
                                    "function_ordinal": 125, "raw_type": "STT_FUNC", "raw_binding": "STB_LOCAL"}}]}


HOMES = {NEW: [{"symbol": NEW, "module": "ov004", "address": 0x1004, "size": 36, "type": "STT_FUNC"}]}


class OwnedFunctionRenames(unittest.TestCase):
    def setUp(self):
        self.patch = mock.patch.object(RA, "function_rename_homes", return_value=copy.deepcopy(HOMES))
        self.patch.start()
        self.addCleanup(self.patch.stop)
        self.saved = RA._RENAME_POLICIES
        self.addCleanup(setattr, RA, "_RENAME_POLICIES", self.saved)

    def prepare(self, raw=None, e=None):
        return TB.apply_defined_symbol_rename_policy(fixture() if raw is None else raw, entry() if e is None else e)

    def policy(self, e):
        funcs = frozenset(f["symbol"] for f in e["functions"])
        mapping = {r["from"]: r["to"] for r in e["defined_symbol_renames"]}
        RA._RENAME_POLICIES = [(funcs, mapping, e)]

    def assert_refused(self, raw=None, e=None):
        result, report, errors = self.prepare(raw, e)
        self.assertIsNone(result)
        self.assertTrue(errors)
        return errors

    def test_exact_function_and_overlapping_nontext_simultaneous(self):
        raw = fixture()
        out, report, errors = self.prepare(raw)
        self.assertEqual(errors, [])
        self.assertEqual([r["from"] for r in report["renamed"]], ["@333", "@337", OLD])
        oldelf, newelf = ELFFile(io.BytesIO(raw)), ELFFile(io.BytesIO(out))
        olds = list(oldelf.get_section_by_name(".symtab").iter_symbols())
        news = list(newelf.get_section_by_name(".symtab").iter_symbols())
        self.assertEqual([s.entry for s in olds], [s.entry for s in news])
        self.assertEqual([s.header for s in oldelf.iter_sections()], [s.header for s in newelf.iter_sections()])
        for oldsec, newsec in zip(oldelf.iter_sections(), newelf.iter_sections()):
            if oldsec.name != ".strtab":
                self.assertEqual(oldsec.data(), newsec.data())
        self.assertEqual([s.name for s in news], ["", "@329", "@333", NEW, "alpha"])
        self.assertEqual(oldelf.header, newelf.header)
        self.assertEqual(len(out), len(raw))
        # Incoming relocation keeps ordinal3/addend0; the symbol name alone changes.
        self.assertEqual(oldelf.get_section_by_name(".rela.text").data(), newelf.get_section_by_name(".rela.text").data())

    def test_TB_missing_source_refuses_RA_canonical_passes_through(self):
        e = entry(); self.policy(e)
        out, _, errors = self.prepare(e=e)
        self.assertFalse(errors)
        self.assert_refused(out, e)
        self.assertEqual(RA._with_manifest_renames(out), out)
        self.assertEqual(RA._with_manifest_renames(fixture()), out)

    def test_RA_rejects_missing_unrelated_function_and_wrong_descriptor(self):
        e = entry(); self.policy(e)
        raw = fixture(alpha=False)
        self.assertEqual(RA._with_manifest_renames(raw), raw)
        e["defined_symbol_renames"][-1]["owned_function"]["module"] = "ov005"
        raw = fixture(); self.assertEqual(RA._with_manifest_renames(raw), raw)

    def test_RA_registry_loader_keeps_entry_for_function_policy(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td)/"config/tu_manifest.d/ov004/Fixture.json"
            path.parent.mkdir(parents=True); path.write_text(json.dumps(entry()))
            with mock.patch.object(RA, "REPO", pathlib.Path(td)):
                RA._RENAME_POLICIES = None
                policies = RA._defined_symbol_rename_policies()
                self.assertEqual(len(policies), 1)
                self.assertEqual(len(policies[0]), 3)
                self.assertNotEqual(RA._with_manifest_renames(fixture()), fixture())

    def test_actual_destination_identity_uses_canonical_module(self):
        e = entry(); self.policy(e)
        raw = fixture(); out = RA._with_manifest_renames(raw)
        homes = {OLD: ("ov005", 0x1004), NEW: ("ov004", 0x1004)}
        self.assertEqual(RA.object_reloc_dests(raw, "alpha", homes)[0][0][2], "ov005")
        self.assertEqual(RA.object_reloc_dests(out, "alpha", homes)[0][0][2], "ov004")

    def test_no_descriptor_no_function_license(self):
        e = entry(); del e["defined_symbol_renames"][-1]["owned_function"]
        self.assertTrue(any("does not own" in x for x in self.assert_refused(e=e)))

    def test_descriptor_missing_unknown_or_wrong_each_fact(self):
        for key in list(entry()["defined_symbol_renames"][-1]["owned_function"]):
            with self.subTest(missing=key):
                e=entry(); del e["defined_symbol_renames"][-1]["owned_function"][key]; self.assert_refused(e=e)
        for key, value in [("module", "ov005"), ("address", "0x1008"), ("size", "0x20"),
                           ("function_ordinal",126), ("raw_type","STT_OBJECT"), ("raw_binding","STB_GLOBAL"),
                           ("size",True), ("extra",1)]:
            with self.subTest(key=key,value=value):
                e=entry();e["defined_symbol_renames"][-1]["owned_function"][key]=value;self.assert_refused(e=e)

    def test_registry_full_home_type_size_and_duplicate_checks(self):
        variants=[]
        for key,value in [("module","ov005"),("address",0x1008),("size",32),("type","NON_FUNCTION")]:
            h=copy.deepcopy(HOMES);h[NEW][0][key]=value;variants.append(h)
        variants += [{}, {NEW:HOMES[NEW]*2}]
        for homes in variants:
            with self.subTest(homes=homes), mock.patch.object(RA,"function_rename_homes",return_value=homes):self.assert_refused()

    def test_registry_parser_retains_duplicate_and_nonfunction_homes(self):
        text = f"{NEW} kind:function(arm,size=0x24) addr:0x1004\n{NEW} kind:data addr:0x1004\n"
        rows=TUM.function_rename_registry_rows(text,"ov004")
        self.assertEqual(len(rows),2);self.assertEqual(rows[0]["size"],36);self.assertEqual(rows[1]["type"],"NON_FUNCTION")

    def test_raw_symbol_failures(self):
        for options in [dict(helper_type=1),dict(helper_bind=1),dict(helper_size=32),dict(helper_section=0),
                        dict(helper_section=6),dict(extra=[(OLD,0,36,0,2,5)]),dict(extra=[(NEW,0,36,0,2,5)])]:
            with self.subTest(options=options):self.assert_refused(raw=fixture(**options))

    def test_complete_span_ordinal_and_conflicting_policies(self):
        variants=[]
        e=entry();e["sections"][0]["end"]="0x1024";variants.append(e)
        e=entry();e["functions"][0]["size"]="0x8";variants.append(e)
        e=entry();e["functions"][0]["ordinal"]=125;variants.append(e)
        e=entry();e["functions"].append(copy.deepcopy(e["functions"][-1]));variants.append(e)
        for field in ["compiler_only_output","externalized_output","undefined_symbol_aliases"]:
            e=entry();e[field]=[{"symbol":OLD}];variants.append(e)
        for e in variants:
            with self.subTest(e=e):self.assert_refused(e=e)

    def test_function_cycle_chain_duplicate_and_missing_evidence_refuse(self):
        variants=[]
        e=entry();e["defined_symbol_renames"].append({"from":NEW,"to":"@329","evidence":"chain"});variants.append(e)
        e=entry();e["defined_symbol_renames"][0]["to"]=OLD;variants.append(e)
        e=entry();e["defined_symbol_renames"].append(copy.deepcopy(e["defined_symbol_renames"][-1]));variants.append(e)
        e=entry();e["defined_symbol_renames"][-1]["evidence"]="";variants.append(e)
        for e in variants:
            with self.subTest(e=e):self.assert_refused(e=e)

    def test_historical_nondict_and_emptydict_rows_refuse(self):
        for rows in [{},"bad",[None]]:
            e=entry();e["defined_symbol_renames"]=rows;self.assert_refused(e=e)

    def test_rename_growth_suffix_alias_and_wrong_ELF_machine_refuse(self):
        e=entry();e["defined_symbol_renames"][-1]["from"]="x"
        self.assert_refused(raw=fixture(helper_name="x"),e=e)
        raw=bytearray(fixture(extra=[("alias",0,0,0,0,0)]))
        elf=ELFFile(io.BytesIO(raw));table=elf.get_section_by_name(".symtab");symbols=list(table.iter_symbols())
        struct.pack_into("<I",raw,table["sh_offset"]+(len(symbols)-1)*16,symbols[3]["st_name"]+2)
        self.assert_refused(raw=bytes(raw))
        raw=bytearray(fixture());struct.pack_into("<H",raw,18,3)
        self.assert_refused(raw=bytes(raw))

    def test_canonical_source_collision_and_evidence_type_refuse(self):
        e=entry();e["functions"].append({"symbol":OLD,"address":"0x2000","size":"0x24","ordinal":126})
        self.assert_refused(e=e)
        for evidence in [True, ["not text"], "   "]:
            e=entry();e["defined_symbol_renames"][-1]["evidence"]=evidence;self.assert_refused(e=e)

    def test_binding_separate_and_no_byte_correctness_claim_from_rename(self):
        raw=fixture();out,_,errors=self.prepare(raw);self.assertFalse(errors)
        e=entry();e["symbol_binding_rewrites"]=[{"symbol":NEW,"from":"STB_LOCAL","to":"STB_GLOBAL","evidence":"separate existing policy"}]
        bound,_,errors=TB.apply_symbol_binding_policy(out,e);self.assertFalse(errors)
        symbols=list(ELFFile(io.BytesIO(bound)).get_section_by_name(".symtab").iter_symbols())
        self.assertEqual(symbols[3]["st_info"]["bind"],"STB_GLOBAL")
        # The rename intentionally does not masquerade as a ROM byte gate.
        changed=bytearray(raw);section=ELFFile(io.BytesIO(raw)).get_section_by_name(".text.helper");changed[section["sh_offset"]]=1
        renamed,_,errors=self.prepare(bytes(changed));self.assertFalse(errors)
        self.assertNotEqual(renamed,out)


    def classify(self, raw=None, e=None):
        # No registry/ROM access: complete function homes come from setUp's
        # shared-validator fixture; the old classifier uses an empty home map.
        with mock.patch.object(OI, "derive_deadstrip", side_effect=AssertionError("no surgery")):
            return TB.apply_compiler_only_policy(
                fixture() if raw is None else raw, entry() if e is None else e, homes={})

    def test_early_classifier_then_later_rename_preserves_order_and_bytes(self):
        raw = fixture()
        e = entry()
        before = copy.deepcopy(e)
        with mock.patch.object(TB, "apply_defined_symbol_rename_policy",
                               side_effect=AssertionError("rename must remain later")):
            classified, report, errors = self.classify(raw, e)
        self.assertEqual(errors, [])
        self.assertEqual(classified, raw)
        self.assertEqual(e, before)
        self.assertEqual(report["deadstripped"], [])
        renamed, _, errors = self.prepare(classified, e)
        self.assertEqual(errors, [])
        symbols = ELFFile(io.BytesIO(renamed)).get_section_by_name(".symtab")
        self.assertEqual([s.name for s in symbols.iter_symbols()], ["", "@329", "@333", NEW, "alpha"])
        for name in [".rela.text", ".rela.text.helper", ".text", ".text.helper"]:
            self.assertEqual(ELFFile(io.BytesIO(raw)).get_section_by_name(name).data(),
                             ELFFile(io.BytesIO(renamed)).get_section_by_name(name).data())

    def test_early_classifier_invalid_descriptor_and_full_home_refuse(self):
        for key, value in [("module", "ov005"), ("address", "0x1008"),
                           ("size", "0x20"), ("function_ordinal", 126),
                           ("raw_type", "STT_OBJECT"), ("raw_binding", "STB_GLOBAL")]:
            with self.subTest(key=key):
                e = entry(); e["defined_symbol_renames"][-1]["owned_function"][key] = value
                result, _, errors = self.classify(e=e)
                self.assertIsNone(result); self.assertTrue(errors)
        variants = [{}, {NEW: HOMES[NEW] * 2}]
        for key, value in [("module", "ov005"), ("address", 0x1008), ("size", 32), ("type", "NON_FUNCTION")]:
            homes = copy.deepcopy(HOMES); homes[NEW][0][key] = value; variants.append(homes)
        for homes in variants:
            with self.subTest(homes=homes), mock.patch.object(RA, "function_rename_homes", return_value=homes):
                result, _, errors = self.classify()
                self.assertIsNone(result); self.assertTrue(errors)

    def test_early_classifier_actual_raw_attributes_refuse(self):
        variants = [fixture(helper_type=1), fixture(helper_bind=1), fixture(helper_size=32),
                    fixture(helper_section=0), fixture(helper_section=6),
                    fixture(extra=[(OLD, 0, 36, 0, 2, 5)]),
                    fixture(extra=[(NEW, 0, 36, 0, 2, 5)])]
        for raw in variants:
            with self.subTest(sha=TB.hashlib.sha256(raw).hexdigest()):
                result, _, errors = self.classify(raw)
                self.assertIsNone(result); self.assertTrue(errors)

    def test_early_classifier_unknown_extra_remains_unlicensed(self):
        raw = fixture(extra=[("unowned", 0, 4, 0, 2, 4)])
        result, _, errors = self.classify(raw)
        self.assertIsNone(result)
        self.assertTrue(any("unlicensed function unowned" in e for e in errors))

    def test_early_classifier_no_owned_row_preserves_nontext_behavior(self):
        e = entry(); e["defined_symbol_renames"] = e["defined_symbol_renames"][:2]
        e["functions"][-1]["symbol"] = OLD
        with mock.patch.object(RA, "validated_defined_symbol_rename_plan",
                               side_effect=AssertionError("nontext must use old path")):
            result, _, errors = self.classify(e=e)
        self.assertEqual(errors, []); self.assertEqual(result, fixture())
        # A descriptorless function row does not confer classifier ownership.
        e = entry(); del e["defined_symbol_renames"][-1]["owned_function"]
        result, _, errors = self.classify(e=e)
        self.assertIsNone(result)
        self.assertTrue(any("unlicensed function " + OLD in x for x in errors))

    def test_early_classifier_conflicting_disposal_and_alias_refuse(self):
        for field in ["compiler_only_output", "externalized_output", "undefined_symbol_aliases"]:
            for symbol in [OLD, NEW]:
                with self.subTest(field=field, symbol=symbol):
                    e = entry(); e[field] = [{"symbol": symbol, "from": symbol,
                                             "to": "other", "disposition": "deadstrip", "reason": "conflict"}]
                    result, _, errors = self.classify(e=e)
                    self.assertIsNone(result); self.assertTrue(errors)

    def test_early_classifier_missing_raw_and_unowned_target_refuse(self):
        result, _, errors = self.classify(fixture(helper_name=NEW))
        self.assertIsNone(result); self.assertTrue(errors)
        e = entry(); e["functions"] = e["functions"][:1]
        result, _, errors = self.classify(e=e)
        self.assertIsNone(result); self.assertTrue(errors)

    def test_early_classifier_malformed_and_duplicate_owned_rows_refuse(self):
        variants = []
        e = entry(); e["defined_symbol_renames"][-1]["owned_function"] = None; variants.append(e)
        e = entry(); e["defined_symbol_renames"].append(copy.deepcopy(e["defined_symbol_renames"][-1])); variants.append(e)
        e = entry(); e["defined_symbol_renames"][-1]["evidence"] = ""; variants.append(e)
        for e in variants:
            with self.subTest(e=e):
                result, _, errors = self.classify(e=e)
                self.assertIsNone(result); self.assertTrue(errors)

if __name__ == "__main__":
    unittest.main()
