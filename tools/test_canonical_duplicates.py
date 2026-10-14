"""Canonical imports: real compiler objects, destination preservation, and refusals.

Run python -m unittest tools.test_canonical_duplicates -v. Optional private Moray
fixture: set CANONICAL_DUPLICATE_FIXTURE to its clean_new directory (object and
proof.json). The fixture never changes tracked source or configuration.
"""
import io
import json
import os
import pathlib
import struct
import subprocess
import sys
import tempfile
import unittest
from contextlib import ExitStack
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tubuild as TB
import rombuild as RB
import tu_production as TP
from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

ROOT = pathlib.Path(__file__).resolve().parent.parent
COMPILER = RB.MW / RB.VERSION / "mwccarm.exe"
NAME = "_ZN1VC1Ev"
TARGET = "callback"
ADDRESS = 0x02001000


@unittest.skipUnless(COMPILER.is_file(), "pinned compiler is unavailable")
class CanonicalDuplicates(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory()
        cls.directory = pathlib.Path(cls.temp.name)
        source = cls.directory / "fixture.cpp"
        source.write_text("#pragma dont_inline on\nstruct V { int v[3]; V(){} };\n"
                          "struct C { V a[7]; C(); }; C::C(){}\n"
                          'extern "C" int kept(int x) { return x + 1; }\n')
        obj = cls.directory / "fixture.o"
        result = subprocess.run([str(COMPILER),
            *RB.CFLAGS.replace("-lang c99", "-lang c++").split(),
            "-c", str(source), "-o", str(obj)], capture_output=True, text=True,
            env=dict(os.environ, LM_LICENSE_FILE=str(RB.LICENSE)))
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
        cls.raw = obj.read_bytes()
        elf = ELFFile(io.BytesIO(cls.raw))
        symbols = list(elf.get_section_by_name(".symtab").iter_symbols())
        cls.index, symbol = next((i, s) for i, s in enumerate(symbols) if s.name == NAME)
        cls.section_index = symbol["st_shndx"]
        cls.body = elf.get_section(cls.section_index).data()
        cls.functions = [s.name for s in symbols if isinstance(s["st_shndx"], int)
                         and s["st_info"]["type"] == "STT_FUNC" and s.name != NAME
                         and not s.name.startswith("$")]

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def setUp(self):
        self.entry = {"id": "ov001/Test", "module": "ov001", "source": "src/Test.cpp",
            "functions": [{"symbol": name} for name in self.functions],
            "compiler_only_output": [{"symbol": NAME, "canonical_symbol": TARGET,
                "disposition": "deadstrip-duplicate", "canonical_module": "arm9",
                "canonical_address": hex(ADDRESS), "reason": "Original array callback."}]}
        self.homes = {TARGET: [("arm9", ADDRESS)]}
        self.config = self.directory / "symbols.txt"
        self.config.write_text(f"{TARGET} kind:function(arm,size=0x{len(self.body):x}) "
                               f"addr:0x{ADDRESS:x}\n")
        self.stack = ExitStack()
        self.addCleanup(self.stack.close)
        for owner, name, value in ((TB.RL, "module_universe", [(self.config, "arm9")]),
                (TB.BP, "target_bytes", self.body), (TB.RA, "build_config_relocs", {}),
                (TB, "all_symbol_homes", self.homes)):
            self.stack.enter_context(mock.patch.object(owner, name, return_value=value))

    def apply(self, raw=None):
        return TB.apply_compiler_only_policy(self.raw if raw is None else raw,
                                             self.entry, homes=self.homes)

    def refused(self, text, raw=None):
        out, _, errors = self.apply(raw)
        self.assertTrue(out is None, "policy unexpectedly accepted the object")
        self.assertIn(text, "\n".join(errors))

    def mutate_symbol(self, offset, value, fmt="I", index=None):
        raw = bytearray(self.raw)
        elf = ELFFile(io.BytesIO(raw))
        start = elf.get_section_by_name(".symtab")["sh_offset"]
        struct.pack_into("<" + fmt, raw, start + (self.index if index is None else index) * 16 + offset, value)
        return bytes(raw)

    def references(self, raw):
        elf = ELFFile(io.BytesIO(raw))
        return [(i, r["r_offset"], r["r_info_type"], r.entry.get("r_addend"))
                for i, sec in enumerate(elf.iter_sections()) if isinstance(sec, RelocationSection)
                for r in sec.iter_relocations() if r["r_info_sym"] == self.index]

    def test_keeps_caller_indices_and_addends(self):
        out, _, errors = self.apply()
        self.assertEqual(errors, [])
        sym = ELFFile(io.BytesIO(out)).get_section_by_name(".symtab").get_symbol(self.index)
        self.assertEqual((sym.name, sym["st_shndx"]), (TARGET, "SHN_UNDEF"))
        self.assertTrue(self.references(self.raw))
        self.assertEqual(self.references(self.raw), self.references(out))

    def test_ordinary_rombuild_prepares_real_object(self):
        policies = RB.compiler_only_policies(manifest={"entries": [self.entry]}, homes=self.homes)
        obj = self.directory / "ordinary.o"
        obj.write_bytes(self.raw)
        self.assertIsNone(RB._isolate(obj, "src/Test.cpp", {"src/Test.cpp": self.functions},
                                     compiler_only=policies))
        sym = ELFFile(io.BytesIO(obj.read_bytes())).get_section_by_name(".symtab").get_symbol(self.index)
        self.assertEqual((sym.name, sym["st_shndx"]), (TARGET, "SHN_UNDEF"))

    def test_single_function_source_still_checks_canonical_policy(self):
        functions = ["_ZN1CC2Ev"]
        raw, plan = TB.OI.derive_deadstrip(self.raw,
            [name for name in self.functions if name not in functions])
        self.assertIsNone(plan["error"])
        self.assertEqual(len(functions), 1)
        self.entry["functions"] = [{"symbol": functions[0]}]
        policies = RB.compiler_only_policies(manifest={"entries": [self.entry]}, homes=self.homes)
        obj = self.directory / "single.o"
        obj.write_bytes(raw)
        self.assertIsNone(RB._isolate(obj, "src/Test.cpp", {"src/Test.cpp": functions},
                                     compiler_only=policies))
        names = {s.name for s in ELFFile(io.BytesIO(obj.read_bytes())).get_section_by_name(".symtab").iter_symbols()}
        self.assertIn(TARGET, names)
        self.assertNotIn(NAME, names)

    def test_existing_destination_and_unavailable_rom_are_refused(self):
        with mock.patch.object(TB.BP, "target_bytes", return_value=None):
            self.refused("ROM bytes are unavailable")
        self.entry["functions"] = [r for r in self.entry["functions"] if r["symbol"] != "kept"]
        self.entry["compiler_only_output"][0]["canonical_symbol"] = "kept"
        self.homes = {"kept": [("arm9", ADDRESS)]}
        self.config.write_text(f"kept kind:function(arm,size=0x4) addr:0x{ADDRESS:x}\n")
        self.refused("destination already exists")

    def test_invalid_field_and_disposition_fail_cleanly(self):
        row = self.entry["compiler_only_output"][0]
        row["canonical_symbol"] = {}
        self.refused("non-empty strings")
        with self.assertRaises(RB.BuildError):
            RB.compiler_only_policies(manifest={"entries": [self.entry]}, homes=self.homes)
        row["canonical_symbol"] = TARGET
        row["disposition"] = "deadstrip"
        self.refused("requires deadstrip-duplicate")

    def test_optional_home_and_wrong_partial_home(self):
        row = self.entry["compiler_only_output"][0]
        del row["canonical_module"]
        self.refused("declared canonical home")
        del row["canonical_address"]
        self.assertEqual(self.apply()[2], [])

    def test_missing_ambiguous_and_mismatched_homes(self):
        for homes in ({}, {TARGET: [("arm9", ADDRESS), ("ov001", ADDRESS)]},
                      {TARGET: [("arm9", ADDRESS + 4)]}):
            with self.subTest(homes=homes):
                self.homes = homes
                self.refused("canonical")

    def test_existing_source_home_and_owned_names(self):
        self.homes[NAME] = [("arm9", ADDRESS)]
        self.refused("already has configured")
        del self.homes[NAME]
        for name in (NAME, TARGET):
            self.entry["data"] = [{"symbol": name}]
            self.refused("owned by this manifest")

    def test_owned_range_overlap(self):
        self.entry.update(module="arm9", sections=[{"name": ".text",
                          "start": hex(ADDRESS + 1), "end": hex(ADDRESS + 8)}])
        self.refused("overlaps")

    def test_owned_function_containment_without_sections(self):
        self.entry["module"] = "arm9"
        self.entry["functions"] = [{"symbol": "owner", "address": hex(ADDRESS - 4), "size": "0xc"}]
        self.refused("overlaps a manifest-owned function")

    def own_nontext(self, field, row):
        self.entry["module"] = "arm9"
        self.entry["functions"] = [{"symbol": name, "address": hex(ADDRESS + 0x100 + i * 4),
                                    "size": "0x4"} for i, name in enumerate(self.functions)]
        self.entry[field] = [row]

    def test_each_nontext_field_contains_canonical_without_sections(self):
        for field in ("rodata", "init", "ctor", "data", "bss", "exception", "exceptix"):
            with self.subTest(field=field):
                self.own_nontext(field, {"symbol": "owned_data", "address": hex(ADDRESS - 4), "size": 12})
                self.homes["owned_data"] = [("arm9", ADDRESS - 4)]
                self.refused("overlaps owned")
                with self.assertRaises(RB.BuildError):
                    RB.compiler_only_policies(manifest={"entries": [self.entry]}, homes=self.homes)
                del self.entry[field]

    def test_nontext_half_open_boundaries(self):
        for start, size, overlaps in ((ADDRESS - 4, 4, False), (ADDRESS + 4, 4, False),
                                     (ADDRESS - 4, 5, True), (ADDRESS + 3, 4, True),
                                     (ADDRESS, 4, True), (ADDRESS - 4, 12, True)):
            with self.subTest(start=start, size=size):
                self.own_nontext("data", {"symbol": "owned_data", "address": hex(start), "size": size})
                if overlaps:
                    self.refused("overlaps owned")
                else:
                    self.assertEqual(self.apply()[2], [])

    def test_vtable_ownership_includes_emitted_preamble(self):
        self.own_nontext("data", {"symbol": "_ZTV5Owned", "address": hex(ADDRESS + 4),
            "emitted_storage_address": hex(ADDRESS - 4), "address_point_bias": 8, "size": 12})
        self.homes["_ZTV5Owned"] = [("arm9", ADDRESS + 4)]
        self.refused("overlaps owned .data object _ZTV5Owned storage")
        self.entry["data"][0].update(address=hex(ADDRESS + 12),
                                     emitted_storage_address=hex(ADDRESS + 4))
        self.homes["_ZTV5Owned"] = [("arm9", ADDRESS + 12)]
        self.assertEqual(self.apply()[2], [])

    def test_rom_symbol_address_ownership_fallback(self):
        self.own_nontext("data", {"symbol": "owned_data", "rom_symbol_address": hex(ADDRESS - 4), "size": 12})
        self.refused("overlaps owned")

    def test_malformed_nontext_extents_fail_closed(self):
        cases = ({"size": 0}, {"size": -1}, {"size": "bad"}, {"size": None},
                 {"address": "bad"}, {"address": None}, {"address": True},
                 {"address": -1}, {"address": "0xffffffff", "size": 2},
                 {"emitted_storage_address": hex(ADDRESS + 8)},
                 {"address_point_bias": 8},
                 {"emitted_storage_address": hex(ADDRESS + 8), "address_point_bias": 4},
                 {"emitted_storage_address": hex(ADDRESS + 24), "address_point_bias": -8},
                 {"emitted_storage_address": hex(ADDRESS), "address_point_bias": 16})
        for changes in cases:
            with self.subTest(changes=changes):
                row = {"symbol": "owned_data", "address": hex(ADDRESS + 16), "size": 12}
                row.update(changes)
                self.own_nontext("data", row)
                self.refused("owned .data object")
        for missing in ("address", "size"):
            row = {"symbol": "owned_data", "address": hex(ADDRESS + 16), "size": 12}
            del row[missing]
            self.own_nontext("data", row)
            self.refused("owned .data object")

    def test_malformed_nontext_containers_fail_closed(self):
        self.own_nontext("data", {"symbol": "owned_data", "address": hex(ADDRESS + 16), "size": 12})
        for rows in (None, {}, [None], [{}], [{"symbol": 1}], [{"symbol": ""}]):
            with self.subTest(rows=rows):
                self.entry["data"] = rows
                self.refused("owned data must contain named object rows")

    def test_alias_overlap_cycle_and_destination_collision(self):
        self.entry["undefined_symbol_aliases"] = [{"symbol": TARGET, "canonical_symbol": NAME}]
        self.refused("overlapping")
        del self.entry["undefined_symbol_aliases"]
        self.entry["compiler_only_output"].append({"symbol": TARGET, "canonical_symbol": NAME,
            "disposition": "deadstrip-duplicate", "reason": "cycle"})
        self.refused("overlapping")
        self.entry["compiler_only_output"].pop()
        self.entry["compiler_only_output"].append(dict(self.entry["compiler_only_output"][0], symbol="other"))
        self.refused("duplicate source or canonical destination")

    def test_rejects_data_target_even_at_function_address(self):
        self.config.write_text(f"{TARGET} kind:data size:0x4 addr:0x{ADDRESS:x}\n"
                               f"other kind:function(arm,size=0x4) addr:0x{ADDRESS:x}\n")
        self.refused("uniquely configured sized function")

    def test_wrong_binding_extent_and_bytes(self):
        self.refused("STB_LOPROC", self.mutate_symbol(12, 0x12, "B"))
        self.refused("exact canonical extent", self.mutate_symbol(8, len(self.body) - 1))
        self.refused("exact canonical extent", self.mutate_symbol(4, 1))
        raw = bytearray(self.raw)
        section = ELFFile(io.BytesIO(raw)).get_section(self.section_index)
        raw[section["sh_offset"]] ^= 1
        self.refused("complete body differs", bytes(raw))

    def test_nonexclusive_section(self):
        elf = ELFFile(io.BytesIO(self.raw))
        index = next(i for i, s in enumerate(elf.get_section_by_name(".symtab").iter_symbols())
                     if s.name == "kept")
        self.refused("another occupant", self.mutate_symbol(14, self.section_index, "H", index))

    def test_any_emitted_or_configured_relocation_is_refused(self):
        with mock.patch.object(TB.RA, "build_config_relocs", return_value={"arm9": {ADDRESS: ("load", 0, "arm9")}}):
            self.refused("configured relocations")
        raw = bytearray(self.raw)
        elf = ELFFile(io.BytesIO(raw))
        index = next(i for i, sec in enumerate(elf.iter_sections())
                     if isinstance(sec, RelocationSection) and sec.num_relocations())
        struct.pack_into("<I", raw, elf["e_shoff"] + index * elf["e_shentsize"] + 28, self.section_index)
        self.refused("emitted relocations", bytes(raw))

    def test_reference_through_section_symbol_is_refused(self):
        raw = bytearray(self.raw)
        elf = ELFFile(io.BytesIO(raw))
        symbols = list(elf.get_section_by_name(".symtab").iter_symbols())
        section_symbol = next(i for i, symbol in enumerate(symbols) if symbol.name == "kept")
        entry = elf.get_section_by_name(".symtab")["sh_offset"] + section_symbol * 16
        struct.pack_into("<IIBBH", raw, entry + 4, 0, 0, 3, 0, self.section_index)
        found = False
        for section in elf.iter_sections():
            if not isinstance(section, RelocationSection):
                continue
            for ordinal, relocation in enumerate(section.iter_relocations()):
                if relocation["r_info_sym"] == self.index:
                    struct.pack_into("<I", raw,
                        section["sh_offset"] + ordinal * section["sh_entsize"] + 4,
                        (section_symbol << 8) | relocation["r_info_type"])
                    found = True
        self.assertTrue(found)
        self.refused("through another symbol", bytes(raw))

    def test_ordinary_path_refuses_before_writing_bad_object(self):
        policies = RB.compiler_only_policies(manifest={"entries": [self.entry]}, homes=self.homes)
        raw = self.mutate_symbol(8, 1)
        obj = self.directory / "bad.o"
        obj.write_bytes(raw)
        self.assertIn("canonical duplicate", RB._isolate(obj, "src/Test.cpp",
            {"src/Test.cpp": self.functions}, compiler_only=policies))
        self.assertEqual(obj.read_bytes(), raw)


@unittest.skipUnless(os.environ.get("CANONICAL_DUPLICATE_FIXTURE"), "private Moray fixture not requested")
class MorayIntact(unittest.TestCase):
    def test_real_intact_factory_all_functions(self):
        import linkcheck as LC
        directory = pathlib.Path(os.environ["CANONICAL_DUPLICATE_FIXTURE"])
        entry = json.loads((ROOT / "config/tu_manifest.d/ov016/daMoray_c.json").read_text())
        proof = json.loads((directory / "proof.json").read_text())
        entry["defined_symbol_renames"] = proof["proposed_defined_symbol_renames"]
        entry["compiler_only_output"].append({"symbol": "_ZN7Vector3C1Ev",
            "canonical_symbol": "func_0203d384", "disposition": "deadstrip-duplicate",
            "canonical_module": "arm9", "canonical_address": "0x0203d384",
            "reason": "Original Vector3[7], stride-12 array construction callback."})
        raw = (directory / "daMoray_c.o").read_bytes()
        prepared, _ = TP.prepare_intact_object(raw, entry)
        elf = ELFFile(io.BytesIO(raw))
        constructor = next(s for s in elf.get_section_by_name(".symtab").iter_symbols()
                           if s.name == "_ZN7Vector3C1Ev")
        corrupt = bytearray(raw)
        corrupt[elf.get_section(constructor["st_shndx"])["sh_offset"]] ^= 1
        with self.assertRaisesRegex(TP.ProductionTuError, "complete body differs"):
            TP.prepare_intact_object(corrupt, entry)
        names = TB.RA.build_name_index()
        for function in entry["functions"]:
            check = LC.linkcheck(function["symbol"], int(function["address"], 0),
                int(function["size"], 0), "ov016", names, obj=prepared, sym=function["symbol"])
            self.assertEqual((check["verdict"], check["diffs"], check["blind"]),
                             ("VERIFIED", [], 0), function["symbol"])


if __name__ == "__main__":
    unittest.main()
