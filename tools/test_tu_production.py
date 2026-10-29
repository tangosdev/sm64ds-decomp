import json
import pathlib
import sys
import tempfile
import unittest
import copy
import io
import struct
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rombuild as RB  # noqa: E402
import tu_production as TP  # noqa: E402
import initialized_boundaries as IB  # noqa: E402
import tubuild as TB  # noqa: E402
from elftools.elf.elffile import ELFFile


class InitializedBoundaryAdmission(unittest.TestCase):
    def entry(self):
        sections = [
            {"name": ".text", "start": "0x1000", "end": "0x1010"},
            {"name": ".data", "start": "0x2000", "end": "0x201c"},
        ]
        return {
            "id": "ov074/Test",
            "module": "ov074",
            "status": "promoted",
            "production_mode": "intact-object",
            "source": "src/Test.cpp",
            "sections": sections,
            "verification": {
                "linkcheck": {
                    "result": "scratch-data-verified",
                    "phases": {
                        key: True
                        for key in (
                            "delink",
                            "lcf",
                            "compile",
                            "link",
                            "checkModules",
                            "rom",
                        )
                    },
                    "symbolCheckNewVsBaseline": [],
                    "symbolCheckErrors": [],
                    "symbolCheckBaselineErrors": [],
                    "tuRanges": [
                        {
                            "section": row["name"],
                            "start": row["start"],
                            "end": row["end"],
                            "differingBytes": 0,
                        }
                        for row in sections
                    ],
                    "rom": {"sha256": "a" * 64, "matchesStockRom": True},
                }
            },
        }

    def test_normal_and_scratch_reject_malformed_opt_in_before_link_inputs(self):
        for mutate in (
            lambda e: e["sections"][1].update(terminal_linker_fill=None),
            lambda e: e["sections"][1].update(
                terminal_linker_fill={"alignment": 4, "value": 255}
            ),
            lambda e: e.update(
                initialized_prefix_aliases=[{"symbol": "alias", "vtable": "bad"}]
            ),
            lambda e: e.update(initialized_prefix_aliases=None),
        ):
            entry = self.entry()
            mutate(entry)
            with self.subTest(entry=entry):
                with self.assertRaises(RB.BuildError) as raised:
                    RB.intact_tu_policies(
                        {"src/Test.cpp"}, manifest={"entries": [entry]}
                    )
                self.assertIn(
                    "invalid initialized boundary policy", raised.exception.output
                )
                self.assertTrue(TB.manifest_section_claims(entry)[1])
                with self.assertRaises(ValueError):
                    IB.link_plan(
                        [entry], "absent-objects", "absent-lcf", "absent-config"
                    )

    def test_no_source_baseline_has_no_boundary_requests_or_input_access(self):
        self.assertEqual(
            IB.link_plan([], "absent-objects", "absent-lcf", "absent-config"),
            {"prefixes": [], "terminalFill": []},
        )

    def test_normal_policy_keeps_explicit_request_for_later_actual_link_proof(self):
        entry = self.entry()
        entry["sections"][1]["terminal_linker_fill"] = {"alignment": 4, "value": 0}
        self.assertEqual(
            RB.intact_tu_policies({"src/Test.cpp"}, manifest={"entries": [entry]}),
            {"src/Test.cpp": entry},
        )


def _prefix_fixture():
    """Synthetic ARM ELF; exact bytes/relocations, no compiler or ROM dependency."""
    vt, ti = "_ZTV5Thing", "_ZTI5Thing"
    strings = b"\0" + vt.encode() + b"\0" + ti.encode() + b"\0donor\0"
    ti_name = 2 + len(vt)
    names = [".data.vt", ".data.ti", ".rela.data.vt", ".symtab", ".strtab", ".shstrtab"]
    section_strings = b"\0"
    name_offsets = {}
    for name in names:
        name_offsets[name] = len(section_strings)
        section_strings += name.encode() + b"\0"
    symbols = bytes(16)
    symbols += struct.pack("<IIIBBH", 1, 8, 24, 0x11, 0, 1)
    symbols += struct.pack("<IIIBBH", ti_name, 0, 12, 0xD1, 0, 2)
    symbols += struct.pack("<IIIBBH", ti_name + len(ti) + 1, 0, 0, 0x10, 0, 0)
    payloads = [
        bytes(32),
        bytes(12),
        struct.pack("<IIi", 4, (2 << 8) | 2, 0),
        symbols,
        strings,
        section_strings,
    ]
    rows = [
        (1, 3, 0, 0, 4, 0),
        (1, 3, 0, 0, 4, 0),
        (4, 0, 4, 1, 4, 12),
        (2, 0, 5, 1, 4, 16),
        (3, 0, 0, 0, 1, 0),
        (3, 0, 0, 0, 1, 0),
    ]
    body, headers = bytearray(), bytes(40)
    for name, payload, (kind, flags, link, info, align, entsize) in zip(
        names, payloads, rows
    ):
        offset = 52 + len(body)
        body.extend(payload)
        headers += struct.pack(
            "<IIIIIIIIII",
            name_offsets[name],
            kind,
            flags,
            0,
            offset,
            len(payload),
            link,
            info,
            align,
            entsize,
        )
    header = b"\x7fELF\x01\x01\x01" + bytes(9)
    header += struct.pack(
        "<HHIIIIIHHHHHH", 1, 40, 1, 0, 0, 52 + len(body), 0, 52, 0, 0, 40, 7, 6
    )
    raw = header + body + headers
    entry = dict(
        module="ov999",
        source="src/actors/Thing.cpp",
        production_mode="intact-object",
        functions=[],
        sections=[
            dict(name=".text", start=0x1000, end=0x1004),
            dict(name=".data.vt", input_section=".data", start=0x2000, end=0x2020),
            dict(name=".data.ti", input_section=".data", start=0x2100, end=0x210C),
        ],
        data=[
            dict(
                symbol=vt,
                address=0x2008,
                emitted_storage_address=0x2000,
                address_point_bias=8,
                size=32,
                binding="STB_GLOBAL",
            ),
            dict(symbol=ti, address=0x2100, size=12, binding="STB_LOPROC"),
        ],
        relocations=[
            dict(
                section=".data.vt",
                source=0x2004,
                type="R_ARM_ABS32",
                kind="load",
                symbol=ti,
                addend=0,
                target_module="ov999",
                target_address=0x2100,
            )
        ],
    )
    claims, errors = TP.TB.manifest_section_claims(entry)
    assert not errors, errors
    homes = {
        vt: [("ov999", 0x2008)],
        ti: [("ov999", 0x2100)],
        "prefix": [("ov999", 0x2000)],
    }

    def target_reader(module, start, size):
        if start == 0x2000:
            return bytes(4) + struct.pack("<I", 0x2100) + bytes(24)
        return bytes(size)

    owned = TP.TB.verify_owned_sections(
        raw,
        entry,
        claims,
        name_index={},
        config_relocs={"ov999": {0x2004: ("load", 0x2100, "ov999")}},
        sym_index={},
        target_reader=target_reader,
        symbol_homes=homes,
        bss_boundaries=set(),
        public_address_points=True,
        normalized_undefined_vtables=True,
    )
    assert owned["ok"], owned
    return raw, entry, claims, homes, owned


class InitializedPrefixProof(unittest.TestCase):
    def setUp(self):
        self.raw, self.entry, self.claims, self.homes, self.owned = _prefix_fixture()
        self.path = pathlib.Path("/synthetic/src/actors/Thing.o")
        self.receipt = IB.prepared_receipt(
            self.raw, self.entry, self.claims, self.owned
        )
        self.extents = [
            dict(symbol=name, module=module, address=address, size=0, initialized=True)
            for name, rows in self.homes.items()
            for module, address in rows
        ]

    def prove(self, raw=None, refresh=False, objects=None):
        raw = self.raw if raw is None else raw
        proof = (
            IB.prepared_receipt(raw, self.entry, self.claims, self.owned)
            if refresh
            else self.receipt
        )
        return IB.prefix_storage_license(
            objects or [(self.path, raw)],
            self.entry,
            self.claims,
            self.homes,
            vtable="_ZTV5Thing",
            alias="prefix",
            preparation_proofs={str(self.path): proof},
            configured_extents=self.extents,
        )

    def mutate_symbol(self, index, offset, value, fmt):
        elf = ELFFile(io.BytesIO(self.raw))
        raw = bytearray(self.raw)
        struct.pack_into(
            fmt,
            raw,
            elf.get_section_by_name(".symtab")["sh_offset"] + index * 16 + offset,
            value,
        )
        return bytes(raw)

    def test_complete_native_prefix_is_zero_size_identity(self):
        result = self.prove()
        self.assertEqual(
            (
                result["identitySize"],
                result["licensedStoragePrefixBytes"],
                result["fullStorageBytes"],
                result["publicBytes"],
            ),
            (0, 8, 32, 24),
        )

    def test_stale_object_and_claim_proofs_refuse(self):
        with self.assertRaisesRegex(ValueError, "stale"):
            self.prove(self.raw + b"\0")
        self.claims[0]["end"] += 4
        with self.assertRaisesRegex(ValueError, "stale"):
            self.prove()

    def test_failed_owned_proof_refuses(self):
        with self.assertRaises(ValueError):
            IB.prepared_receipt(self.raw, self.entry, self.claims, {"ok": False})

    def test_actual_layout_not_just_manifest_address(self):
        self.claims[1]["start"] -= 4
        with self.assertRaisesRegex(ValueError, "actual prepared section layout"):
            self.prove(refresh=True)

    def test_non_dedicated_offset_and_local_label_refuse(self):
        for raw in (
            self.mutate_symbol(1, 4, 12, "<I"),
            self.mutate_symbol(3, 14, 1, "<H"),
        ):
            with self.subTest(), self.assertRaises(ValueError):
                self.prove(raw, refresh=True)

    def test_duplicate_provider_and_wrong_source_refuse(self):
        for objects in (
            [(self.path, self.raw)] * 2,
            [(self.path.with_name("Other.o"), self.raw)],
        ):
            with self.subTest(), self.assertRaises(ValueError):
                self.prove(objects=objects)

    def test_wrong_module_and_ambiguous_homes_refuse(self):
        for homes in ([("ov998", 0x2000)], [("ov999", 0x2000), ("ov998", 0x2000)]):
            self.homes["prefix"] = homes
            with self.subTest(), self.assertRaises(ValueError):
                self.prove()

    def test_configured_competing_identity_refuses(self):
        for address, size in ((0x2004, 0), (0x1FFC, 8)):
            self.extents.append(
                dict(symbol="other", module="ov999", address=address, size=size)
            )
            with self.subTest(), self.assertRaises(ValueError):
                self.prove()
            self.extents.pop()

    def test_rtti_binding_visibility_and_unique_claim_refuse(self):
        for raw in (
            self.mutate_symbol(2, 12, 0x11, "<B"),
            self.mutate_symbol(2, 13, 2, "<B"),
        ):
            with self.subTest(), self.assertRaises(ValueError):
                self.prove(raw, refresh=True)
        self.claims.append(copy.deepcopy(self.claims[2]))
        with self.assertRaises(ValueError):
            self.prove(refresh=True)

    def test_malformed_section_and_relocation_refuse(self):
        with self.assertRaises(ValueError):
            self.prove(self.mutate_symbol(2, 14, 0xFF00, "<H"), refresh=True)
        elf = ELFFile(io.BytesIO(self.raw))
        for offset, value in ((4, 0xFFFF0002), (0, 6)):
            raw = bytearray(self.raw)
            struct.pack_into("<I", raw, elf.get_section(3)["sh_offset"] + offset, value)
            with self.subTest(), self.assertRaises(ValueError):
                self.prove(bytes(raw), refresh=True)


class ProductionTuAdmission(unittest.TestCase):
    def test_duplicate_request_refuses_before_touching_baseline_or_config(self):
        with self.assertRaisesRegex(TP.ProductionTuError, "duplicate"):
            TP.prepare(["ov002/Thing", "ov002/Thing"], "unused", "unused")

    def test_manifest_entry_requires_partitioned_link_verified(self):
        data = {"entries": [{"id": "ov002/Thing",
                              "partitioned_link": {"state": "attempted"}}]}
        with self.assertRaisesRegex(TP.ProductionTuError,
                                    "partitioned-link-verified"):
            TP._entry(data, "ov002/Thing")

    def test_missing_content_bound_baseline_refuses(self):
        with tempfile.TemporaryDirectory() as td, \
                mock.patch.object(TP.TB, "BASELINE_LINK", pathlib.Path(td)):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                        "missing strict stock control"):
                TP._strict_baseline()

    def test_clean_worker_bootstraps_rom_gap_control(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing", "module": "ov047"}}
        baseline = {"symbolErrors": [], "romSha256": "ab" * 32}
        with mock.patch.object(
                TP, "_strict_baseline",
                side_effect=[TP.ProductionTuError("missing"), baseline]) as strict, \
                mock.patch.object(TP.subprocess, "run",
                                  return_value=mock.Mock(returncode=0)) as run:
            self.assertIs(
                TP._current_or_bootstrapped_intact_baseline(entries, 7), baseline)

        self.assertEqual(strict.call_count, 2)
        self.assertEqual(strict.call_args_list,
                         [mock.call(entries), mock.call(entries)])
        command = run.call_args.args[0]
        self.assertIn("--baseline", command)
        self.assertIn("ov047", command)
        self.assertIn("7", command)
        self.assertIs(run.call_args.kwargs["check"], False)

    def test_clean_worker_refuses_failed_control_bootstrap(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing", "module": "ov047"}}
        with mock.patch.object(
                TP, "_strict_baseline",
                side_effect=TP.ProductionTuError("missing")), \
                mock.patch.object(
                    TP.subprocess, "run",
                    return_value=mock.Mock(
                        returncode=3,
                        stdout="[4/8] mwccarm\n\n      REFUSED -- pin disagreement\n")):
            with self.assertRaisesRegex(
                    TP.ProductionTuError, "baseline command exited 3"):
                TP._current_or_bootstrapped_intact_baseline(entries, 7)

    def test_failed_control_bootstrap_reports_what_the_baseline_said(self):
        """An exit code alone cannot be acted on.

        A remote gate surfaces this exception and nothing else, so the refusal has
        to carry the baseline's own last words -- blank lines dropped, so a short
        tail is real output rather than padding.
        """
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing", "module": "ov047"}}
        with mock.patch.object(
                TP, "_strict_baseline",
                side_effect=TP.ProductionTuError("missing")),                 mock.patch.object(
                    TP.subprocess, "run",
                    return_value=mock.Mock(
                        returncode=1,
                        stdout="[4/8] mwccarm\n\n      REFUSED -- pin disagreement\n")):
            with self.assertRaisesRegex(
                    TP.ProductionTuError,
                    r"last output: .*mwccarm \| .*REFUSED -- pin disagreement"):
                TP._current_or_bootstrapped_intact_baseline(entries, 7)

    def test_failed_control_bootstrap_says_so_when_there_was_no_output(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing", "module": "ov047"}}
        with mock.patch.object(
                TP, "_strict_baseline",
                side_effect=TP.ProductionTuError("missing")),                 mock.patch.object(TP.subprocess, "run",
                                  return_value=mock.Mock(returncode=1, stdout="")):
            with self.assertRaisesRegex(
                    TP.ProductionTuError, "the command produced no output"):
                TP._current_or_bootstrapped_intact_baseline(entries, 7)

    def test_rom_gap_control_requires_exact_intact_inventory(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing", "module": "ov047"}}
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            (root / "final_link.o").write_bytes(b"independent linked control")
            report = {
                "baseline": True,
                "analysis": {
                    "passed": True,
                    "moduleFidelity": {"moduleSetSha256": "ef" * 32},
                },
                "phases": {
                    "checkModules": {"ok": True},
                    "checkSymbols": {"errors": ["[ERROR] old"]},
                },
                "intactTusDemoted": [{
                    "id": "ov047/Thing", "source": "src/actors/Thing.cpp"}],
                "rom": {"sha256": "ab" * 32},
            }
            (root / "linkcheck.json").write_text(
                json.dumps(report), encoding="utf-8")
            with mock.patch.object(TP.TB, "BASELINE_LINK", root), \
                    mock.patch.object(
                        TP.TB, "validate_partition_baseline_evidence",
                        return_value=("cd" * 32, None)):
                baseline = TP._strict_baseline(entries)
                self.assertEqual(baseline["romSha256"], "ab" * 32)
                self.assertEqual(baseline["moduleSetSha256"], "ef" * 32)
                self.assertIsNone(baseline["matchesStockRom"])
                with self.assertRaisesRegex(
                        TP.ProductionTuError, "current intact TU inventory"):
                    TP._strict_baseline({
                        "src/actors/Other.cpp": {
                            "id": "ov047/Other", "module": "ov047"}})


class ProductionTuObjects(unittest.TestCase):
    def test_automatic_intact_link_plan_uses_current_control_and_vtable_biases(self):
        claims = [{"name": ".text", "start": 0x1000, "end": 0x1010},
                  {"name": ".data", "start": 0x2000, "end": 0x2010}]
        baseline = {
            "symbolErrors": ["[ERROR] old"],
            "romSha256": "ab" * 32,
            "matchesStockRom": True,
            "moduleSetSha256": "ef" * 32,
        }
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": ["[ERROR] old"],
                "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline), \
                mock.patch.object(TP.TB, "manifest_section_claims",
                                  return_value=(claims, [])), \
                mock.patch.object(TP.TB, "partition_vtable_rebiases",
                                  return_value=({"_ZTV1T": {"bias": 8}}, [])):
            prepared = TP.prepare_intact_link_verification(entries)
        self.assertIs(prepared["baseline"], baseline)
        self.assertEqual(prepared["admittedRomSha256"], ["ab" * 32])
        self.assertEqual(prepared["admittedModuleSetSha256"], "ef" * 32)
        self.assertEqual(prepared["entries"], [{
            "id": "ov047/Thing", "source": "src/actors/Thing.cpp",
            "biases": {"_ZTV1T": {"bias": 8}},
        }])

    def test_automatic_intact_link_plan_rejects_laundered_control_error(self):
        baseline = {"symbolErrors": ["[ERROR] old", "[ERROR] new"],
                    "romSha256": "ab" * 32, "matchesStockRom": True,
                    "moduleSetSha256": "ef" * 32}
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": ["[ERROR] old"],
                "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                         "pre-promotion inventory"):
                TP.prepare_intact_link_verification(entries)

    def test_automatic_intact_link_plan_rejects_a_control_that_lost_errors(self):
        # The admitted inventory must be the control's own, not a superset: a
        # stale proof that still admits a resolved row is re-recorded, not kept.
        baseline = {"symbolErrors": ["[ERROR] kept"],
                    "romSha256": "ab" * 32, "matchesStockRom": True,
                    "moduleSetSha256": "ef" * 32}
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": ["[ERROR] kept", "[ERROR] resolved"],
                "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                         "pre-promotion inventory"):
                TP.prepare_intact_link_verification(entries)

    def test_automatic_intact_link_plan_rejects_disagreeing_admitted_inventories(self):
        baseline = {"symbolErrors": [],
                    "romSha256": "ab" * 32, "matchesStockRom": True,
                    "moduleSetSha256": "ef" * 32}

        def entry(tu_id, errors):
            return {"id": tu_id, "verification": {"linkcheck": {
                "symbolCheckErrors": errors, "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}}}

        entries = {"src/actors/Thing.cpp": entry("ov047/Thing", []),
                   "src/actors/Other.cpp": entry("ov047/Other", ["[ERROR] old"])}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                         "pre-promotion inventory"):
                TP.prepare_intact_link_verification(entries)

    def test_same_worker_stock_control_allows_environment_specific_outer_rom(self):
        baseline = {
            "symbolErrors": [], "romSha256": "cd" * 32,
            "matchesStockRom": True, "moduleSetSha256": "ef" * 32,
        }
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": [], "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline), \
                mock.patch.object(TP.TB, "manifest_section_claims",
                                  return_value=([], [])), \
                mock.patch.object(TP.TB, "partition_vtable_rebiases",
                                  return_value=({}, [])):
            prepared = TP.prepare_intact_link_verification(entries)
        self.assertEqual(prepared["baseline"]["romSha256"], "cd" * 32)
        self.assertEqual(prepared["admittedRomSha256"], ["ab" * 32])

    def test_environment_specific_control_without_stock_comparison_refuses(self):
        baseline = {
            "symbolErrors": [], "romSha256": "cd" * 32,
            "matchesStockRom": None, "moduleSetSha256": "ef" * 32,
        }
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": [], "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                        "no same-worker stock-ROM comparison"):
                TP.prepare_intact_link_verification(entries)

    def test_same_worker_stock_control_still_requires_admitted_module_set(self):
        baseline = {
            "symbolErrors": [], "romSha256": "cd" * 32,
            "matchesStockRom": True, "moduleSetSha256": "12" * 32,
        }
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": [], "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                        "executable-module fingerprint"):
                TP.prepare_intact_link_verification(entries)

    def test_admitted_intact_proof_requires_module_set_fingerprint(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": [],
                "rom": {"sha256": "ab" * 32}}},
        }}
        with self.assertRaisesRegex(TP.ProductionTuError,
                                    "executable-module fingerprint"):
            TP.prepare_intact_link_verification(entries)

    def test_intact_object_runs_all_fail_closed_policy_gates(self):
        entry = {"id": "ov047/Thing"}
        claims = [{"name": ".text", "start": 0x1000, "end": 0x1010},
                  {"name": ".data", "start": 0x2000, "end": 0x2010}]
        owned = {"ok": True, "rows": [], "errors": []}
        section_order = {"groups": [{"section": ".data", "original": [5, 7],
                                      "desired": [7, 5]}], "objisolate": {}}
        with mock.patch.object(TP.TB, "manifest_section_claims",
                               return_value=(claims, [])), \
                mock.patch.object(TP.TB, "apply_compiler_only_policy",
                                  return_value=(b"compiler", {"deadstripped": []}, [])), \
                mock.patch.object(TP.TB, "apply_externalized_output_policy",
                                  return_value=(b"external", {"externalized": []}, [])), \
                mock.patch.object(TP.TB, "prepare_owned_nontext_section_order",
                                  return_value=(b"ordered", section_order, [])) as order, \
                mock.patch.object(TP.TB, "verify_owned_sections",
                                  side_effect=[owned, owned]) as verify, \
                mock.patch.object(TP.TB, "partition_vtable_rebiases",
                                  return_value=({"_ZTV1T": {"bias": 8}}, [])), \
                mock.patch.object(TP.TB.OI, "rebias_object_symbols",
                                  return_value=(b"linked", {"error": None})) as rebias, \
                mock.patch.object(TP.TB, "complete_ranges", return_value={}), \
                mock.patch.object(TP.TB, "audit_tu_object",
                                  return_value=([], [], [], True)) as audit, \
                mock.patch.object(TP.TB, "object_audit_refusals", return_value=[]):
            output, evidence = TP.prepare_intact_object(b"raw", entry)
        self.assertEqual(output, b"linked")
        order.assert_called_once_with(b"external", entry, claims)
        rebias.assert_called_once_with(
            b"ordered", {"_ZTV1T": {"bias": 8}}, normalize_undefined=True,
            skip_undefined=set())
        audit.assert_called_once_with(
            b"linked", entry, 0x1000, 0x1010, {},
            validated_vtable_policies={"_ZTV1T": {"bias": 8}})
        self.assertEqual(verify.call_args_list, [
            mock.call(b"ordered", entry, claims),
            mock.call(b"linked", entry, claims, public_address_points=True,
                      normalized_undefined_vtables=True),
        ])
        self.assertIs(evidence["sectionOrder"], section_order)
        self.assertEqual(evidence["sha256"],
                         __import__("hashlib").sha256(b"linked").hexdigest())

    def test_intact_object_keeps_an_already_ordered_external_object(self):
        """FlyGuy-shaped no-op order still flows through every production gate."""
        entry = {"id": "ov070/FlyGuy"}
        claims = [{"name": ".text", "start": 0x1000, "end": 0x1010},
                  {"name": ".data", "start": 0x2000, "end": 0x2010}]
        owned = {"ok": True, "rows": [], "errors": []}
        section_order = {"groups": [{"section": ".data", "original": [5, 7],
                                      "desired": [5, 7]}], "objisolate": None}
        with mock.patch.object(TP.TB, "manifest_section_claims",
                               return_value=(claims, [])), \
                mock.patch.object(TP.TB, "apply_compiler_only_policy",
                                  return_value=(b"compiler", {}, [])), \
                mock.patch.object(TP.TB, "apply_externalized_output_policy",
                                  return_value=(b"external", {}, [])), \
                mock.patch.object(TP.TB, "prepare_owned_nontext_section_order",
                                  return_value=(b"external", section_order, [])) as order, \
                mock.patch.object(TP.TB, "verify_owned_sections",
                                  side_effect=[owned, owned]) as verify, \
                mock.patch.object(TP.TB, "partition_vtable_rebiases",
                                  return_value=({}, [])), \
                mock.patch.object(TP.TB.OI, "rebias_object_symbols",
                                  return_value=(b"external", {"error": None})) as rebias, \
                mock.patch.object(TP.TB, "complete_ranges", return_value={}), \
                mock.patch.object(TP.TB, "audit_tu_object",
                                  return_value=([], [], [], True)), \
                mock.patch.object(TP.TB, "object_audit_refusals", return_value=[]):
            output, evidence = TP.prepare_intact_object(b"raw", entry)
        self.assertEqual(output, b"external")
        order.assert_called_once_with(b"external", entry, claims)
        rebias.assert_called_once_with(b"external", {}, normalize_undefined=True,
                                       skip_undefined=set())
        self.assertEqual(verify.call_args_list, [
            mock.call(b"external", entry, claims),
            mock.call(b"external", entry, claims, public_address_points=True,
                      normalized_undefined_vtables=True),
        ])
        self.assertIs(evidence["sectionOrder"], section_order)

    def test_intact_object_refuses_section_order_before_owned_verification(self):
        entry = {"id": "ov047/Thing"}
        claims = [{"name": ".text", "start": 0x1000, "end": 0x1010},
                  {"name": ".data", "start": 0x2000, "end": 0x2010}]
        with mock.patch.object(TP.TB, "manifest_section_claims",
                               return_value=(claims, [])), \
                mock.patch.object(TP.TB, "apply_compiler_only_policy",
                                  return_value=(b"compiler", {}, [])), \
                mock.patch.object(TP.TB, "apply_externalized_output_policy",
                                  return_value=(b"external", {}, [])), \
                mock.patch.object(TP.TB, "prepare_owned_nontext_section_order",
                                  return_value=(None, {}, ["ambiguous order"])), \
                mock.patch.object(TP.TB, "verify_owned_sections") as verify:
            with self.assertRaisesRegex(TP.ProductionTuError,
                                        "manifest non-text section order"):
                TP.prepare_intact_object(b"raw", entry)
        verify.assert_not_called()

    def test_partitioned_object_normalizes_undefined_vtable_imports(self):
        entry = {
            "id": "ov999/Thing", "module": "ov999",
            "source": "src/actors/Thing.cpp",
            "functions": [{"symbol": "Thing_f", "ordinal": 0,
                           "legacy_source": "src/Thing_f.cpp"}],
        }
        claims = [{"name": ".text", "start": 0x1000, "end": 0x1010},
                  {"name": ".data", "start": 0x2000, "end": 0x2010}]
        owned = {"ok": True, "rows": [], "errors": []}
        biases = {"_ZTV1T": {"bias": 8}}
        section_order = {"groups": [{"section": ".data", "original": [5, 7],
                                      "desired": [7, 5]}], "objisolate": {}}
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            config = root / "config"
            module = config / "overlays" / "ov999"
            module.mkdir(parents=True)
            (module / "delinks.txt").write_text("placeholder\n", encoding="utf-8")
            with mock.patch.object(TP.TB, "manifest_section_claims",
                                   return_value=(claims, [])), \
                    mock.patch.object(TP.TB, "add_partitioned_tu_entry",
                                      return_value=(["src/Thing_f.cpp"], [])), \
                    mock.patch.object(TP.TB, "_compile_tu",
                                      return_value=(b"raw", "2004/b56", ["-O4"],
                                                    root, root / "raw.o")), \
                    mock.patch.object(TP.TB, "derive_function_objects",
                                      return_value={"Thing_f": (b"text", {})}), \
                    mock.patch.object(TP.TB, "production_objects",
                                      return_value=({}, [])), \
                    mock.patch.object(TP.TB, "partial_report", return_value=[
                        (0, "Thing_f", {"identical": True})]), \
                    mock.patch.object(TP.TB, "apply_compiler_only_policy",
                                      return_value=(b"compiler", {}, [])), \
                    mock.patch.object(TP.TB, "apply_externalized_output_policy",
                                      return_value=(b"external", {}, [])), \
                    mock.patch.object(TP.TB, "apply_undefined_symbol_alias_policy",
                                      return_value=(b"aliased", {}, [])) as aliases, \
                    mock.patch.object(TP.TB, "apply_symbol_binding_policy",
                                      return_value=(b"bound", {}, [])) as bindings, \
                    mock.patch.object(TP.TB, "prepare_owned_nontext_section_order",
                                      return_value=(b"ordered", section_order, [])) as order, \
                    mock.patch.object(TP.TB, "verify_owned_sections",
                                      side_effect=[owned, owned]) as verify, \
                    mock.patch.object(TP.TB, "derive_owned_nontext_object",
                                      return_value=(b"storage", {}, [])) as derive, \
                    mock.patch.object(TP.TB, "partition_vtable_rebiases",
                                      return_value=(biases, [])), \
                    mock.patch.object(TP.TB.OI, "rebias_object_symbols",
                                      return_value=(b"linked", {"error": None})) as rebias:
                result = TP._prepare_one(entry, config, root / "work", 1)

        rebias.assert_called_once_with(
            b"storage", biases, normalize_undefined=True, skip_undefined=set())
        aliases.assert_called_once_with(b"external", entry)
        bindings.assert_called_once_with(b"aliased", entry)
        order.assert_called_once_with(b"bound", entry, claims)
        derive.assert_called_once_with(b"ordered", entry, claims)
        self.assertEqual(verify.call_args_list, [
            mock.call(b"ordered", entry, claims),
            mock.call(b"linked", entry, claims, public_address_points=True,
                      normalized_undefined_vtables=True),
        ])
        self.assertIs(result["sectionOrder"], section_order)
        self.assertEqual(result["storageSha256"],
                         __import__("hashlib").sha256(b"linked").hexdigest())

    def test_compile_one_installs_prepared_object_without_compiler(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            prepared = root / "prepared.o"
            prepared.write_bytes(b"derived object")
            rel, error, outcome = RB.compile_one(
                "src/Legacy.cpp", build_root=root / "build", prebuilt=prepared)
            self.assertEqual((rel, error, outcome),
                             ("src/Legacy.cpp", None, "tu-derived"))
            self.assertEqual((root / "build/src/Legacy.o").read_bytes(),
                             b"derived object")

    def test_final_gate_allows_only_baseline_symbol_errors(self):
        prepared = {
            "baseline": {"symbolErrors": ["[ERROR] old"]},
            "entries": [{"biases": {"_ZTV1A": {"bias": 8}}}],
        }
        calls = [
            (True, "modules ok", 0.1),
            (True, "[ERROR] old\nError: Some symbol(s) did not match.", 0.1),
        ]
        with mock.patch.object(TP.TB, "_run_dsd", side_effect=calls), \
                mock.patch.object(TP.TB, "verify_linked_storage_aliases",
                                  return_value={"ok": True, "rows": [{"exact": True}],
                                                "errors": []}):
            result = TP.verify_link("config.yaml", "final_link.o", prepared)
        self.assertTrue(result["ok"])
        self.assertEqual(result["newSymbolErrors"], [])

    def test_final_gate_rejects_new_symbol_error(self):
        prepared = {"baseline": {"symbolErrors": ["[ERROR] old"]}, "entries": []}
        calls = [
            (True, "modules ok", 0.1),
            (False, "[ERROR] old\n[ERROR] new", 0.1),
        ]
        with mock.patch.object(TP.TB, "_run_dsd", side_effect=calls):
            result = TP.verify_link("config.yaml", "final_link.o", prepared)
        self.assertFalse(result["ok"])
        self.assertEqual(result["newSymbolErrors"], ["[ERROR] new"])

    def test_final_gate_rejects_symbol_check_operational_failure(self):
        prepared = {"baseline": {"symbolErrors": []}, "entries": []}
        calls = [(True, "modules ok", 0.1),
                 (False, "tool crashed before producing an inventory", 0.1)]
        with mock.patch.object(TP.TB, "_run_dsd", side_effect=calls):
            result = TP.verify_link("config.yaml", "final_link.o", prepared)
        self.assertFalse(result["ok"])
        self.assertFalse(result["symbolsCommandOk"])


if __name__ == "__main__":
    unittest.main()
