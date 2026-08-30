import json
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tu_promote as TP  # noqa: E402


def intact_proof(sections):
    return {"linkcheck": {
        "result": "scratch-data-verified",
        "phases": {name: True for name in
                   ("delink", "lcf", "compile", "link", "checkModules", "rom")},
        "symbolCheckNewVsBaseline": [],
        "symbolCheckErrors": ["[ERROR] old"],
        "symbolCheckBaselineErrors": ["[ERROR] old"],
        "tuRanges": [
            {"section": row["name"], "start": row["start"], "end": row["end"],
             "differingBytes": 0}
            for row in sections],
        "rom": {"sha256": "a" * 64, "matchesStockRom": True},
    }}


class IntactPromotion(unittest.TestCase):
    def test_batch_preflight_refuses_shared_legacy_before_mutation(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            (root / "src_tu").mkdir()
            (root / "src").mkdir()
            (root / "src_tu/One.cpp").write_text("//cpp\n", encoding="utf-8")
            (root / "src_tu/Two.cpp").write_text("//cpp\n", encoding="utf-8")
            (root / "src/Shared.cpp").write_text("//cpp\n", encoding="utf-8")
            plans = [
                {"id": "ov047/One", "source": "src_tu/One.cpp",
                 "dest": "src/One.cpp", "legacy": ["src/Shared.cpp"],
                 "delinks": root / "config/delinks.txt", "claims": []},
                {"id": "ov047/Two", "source": "src_tu/Two.cpp",
                 "dest": "src/Two.cpp", "legacy": ["src/Shared.cpp"],
                 "delinks": root / "config/delinks.txt", "claims": []},
            ]
            with mock.patch.object(TP, "REPO", root):
                with self.assertRaisesRegex(TP.PromoteError,
                                             "consumed by both"):
                    TP.batch_preflight(plans)

    def test_batch_preflight_refuses_cross_plan_claim_overlap(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            (root / "src_tu").mkdir()
            (root / "src").mkdir()
            for rel in ("src_tu/One.cpp", "src_tu/Two.cpp",
                        "src/First.cpp", "src/Second.cpp"):
                (root / rel).write_text("//cpp\n", encoding="utf-8")
            delinks = root / "config/delinks.txt"
            plans = [
                {"id": "ov047/One", "source": "src_tu/One.cpp",
                 "dest": "src/One.cpp", "legacy": ["src/First.cpp"],
                 "delinks": delinks,
                 "claims": [(".text", 0x1000, 0x1010),
                            (".data", 0x2000, 0x2020)]},
                {"id": "ov047/Two", "source": "src_tu/Two.cpp",
                 "dest": "src/Two.cpp", "legacy": ["src/Second.cpp"],
                 "delinks": delinks,
                 "claims": [(".text", 0x1010, 0x1020),
                            (".data", 0x2010, 0x2030)]},
            ]
            with mock.patch.object(TP, "REPO", root):
                with self.assertRaisesRegex(TP.PromoteError, "overlaps"):
                    TP.batch_preflight(plans)

    def test_converted_baseline_moves_only_banked_members_to_promoted_tu(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            config = root / "config"
            config.mkdir()
            baseline = config / "converted-baseline.json"
            baseline.write_text(
                '{"_note":"old","criteria":[],"count":2,"converted":['
                '"src/First.cpp","src/Unrelated.cpp"]}\n', encoding="utf-8")
            original = baseline.read_text(encoding="utf-8")
            plans = [{
                "dest": "src/actors/TU.cpp",
                "functions": [
                    {"symbol": "First", "legacy_source": "src/First.cpp"},
                    {"symbol": "Second", "legacy_source": "src/Second.cpp"},
                ],
            }]
            with mock.patch.object(TP, "CONFIG", config):
                prepared = TP.converted_baseline_update(plans)
                self.assertEqual(baseline.read_text(encoding="utf-8"), original)
                moved = TP.rewrite_converted_baseline(plans, prepared)

            data = json.loads(baseline.read_text(encoding="utf-8"))
            self.assertEqual(moved, 1)
            self.assertEqual(data["count"], 2)
            self.assertEqual(data["converted"], [
                "src/Unrelated.cpp", "src/actors/TU.cpp#First"])
            self.assertEqual(data["_note"], TP.TR.NOTE)

    def test_single_member_converted_identity_stays_path_based(self):
        with tempfile.TemporaryDirectory() as td:
            config = pathlib.Path(td) / "config"
            config.mkdir()
            baseline = config / "converted-baseline.json"
            baseline.write_text(
                '{"converted":["src/Only.cpp"],"count":1}\n', encoding="utf-8")
            plans = [{"dest": "src/actors/Only.cpp", "functions": [
                {"symbol": "Only", "legacy_source": "src/Only.cpp"}]}]
            with mock.patch.object(TP, "CONFIG", config):
                TP.rewrite_converted_baseline(plans)
            data = json.loads(baseline.read_text(encoding="utf-8"))
            self.assertEqual(data["converted"], ["src/actors/Only.cpp"])

    def test_plan_and_rewrite_keep_nontext_claims_for_intact_object(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            config = root / "config"
            delinks = config / "arm9/overlays/ov047/delinks.txt"
            delinks.parent.mkdir(parents=True)
            delinks.write_text(
                "    .text start:0x00001000 end:0x00001100 kind:code align:4\n"
                "    .data start:0x00002000 end:0x00002100 kind:data align:4\n\n"
                "src/First.cpp:\n    complete\n"
                "    .text start:0x00001000 end:0x00001010\n\n"
                "src/Second.cpp:\n    complete\n"
                "    .text start:0x00001010 end:0x00001020\n\n",
                encoding="utf-8")
            (root / "src_tu").mkdir()
            (root / "src_tu/TU.cpp").write_text("//cpp\n", encoding="utf-8")
            (root / "src").mkdir()
            (root / "src/First.cpp").write_text("//cpp\n", encoding="utf-8")
            (root / "src/Second.cpp").write_text("//cpp\n", encoding="utf-8")
            sections = [
                {"name": ".text", "start": "0x00001000", "end": "0x00001020"},
                {"name": ".data", "start": "0x00002040", "end": "0x00002050"},
            ]
            entry = {
                "id": "ov047/TU", "module": "ov047",
                "status": "scratch-data-verified",
                "production_mode": "intact-object",
                "source": "src_tu/TU.cpp", "promoted_source": "src/TU.cpp",
                "sections": sections,
                "data": [{"symbol": "owned", "address": "0x2040", "size": "0x10"}],
                "verification": intact_proof(sections),
                "functions": [
                    {"symbol": "First", "address": "0x1000", "size": "0x10",
                     "legacy_source": "src/First.cpp"},
                    {"symbol": "Second", "address": "0x1010", "size": "0x10",
                     "legacy_source": "src/Second.cpp"},
                ],
            }
            with mock.patch.object(TP, "REPO", root), \
                    mock.patch.object(TP, "CONFIG", config):
                planned = TP.plan(entry)
                TP.rewrite_delinks(planned)
            written = delinks.read_text(encoding="utf-8")
            self.assertNotIn("src/First.cpp:", written)
            self.assertNotIn("src/Second.cpp:", written)
            self.assertIn("src/TU.cpp:\n    complete\n"
                          "    .text start:0x00001000 end:0x00001020\n"
                          "    .data start:0x00002040 end:0x00002050\n", written)

    def test_intact_promotion_refuses_unimplemented_section_retargeting(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            config = root / "config"
            delinks = config / "arm9/overlays/ov047/delinks.txt"
            delinks.parent.mkdir(parents=True)
            delinks.write_text(
                "    .text start:0x00001000 end:0x00001100 kind:code align:4\n"
                "    .data start:0x00002000 end:0x00002100 kind:data align:4\n\n"
                "src/Only.cpp:\n    complete\n"
                "    .text start:0x00001000 end:0x00001010\n\n",
                encoding="utf-8")
            (root / "src_tu").mkdir()
            (root / "src_tu/TU.cpp").write_text("//cpp\n", encoding="utf-8")
            (root / "src").mkdir()
            (root / "src/Only.cpp").write_text("//cpp\n", encoding="utf-8")
            entry = {
                "id": "ov047/TU", "module": "ov047", "status": "text-verified",
                "production_mode": "intact-object", "source": "src_tu/TU.cpp",
                "promoted_source": "src/TU.cpp",
                "sections": [
                    {"name": ".text", "start": "0x1000", "end": "0x1010"},
                    {"name": ".rodata", "module_section": ".data",
                     "start": "0x2000", "end": "0x2010"}],
                "functions": [{"symbol": "Only", "address": "0x1000", "size": "0x10",
                               "legacy_source": "src/Only.cpp"}],
            }
            with mock.patch.object(TP, "REPO", root), \
                    mock.patch.object(TP, "CONFIG", config):
                with self.assertRaisesRegex(TP.PromoteError,
                                             "input-section retargeting"):
                    TP.plan(entry)

    def test_intact_promotion_preflights_production_admission(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            config = root / "config"
            delinks = config / "arm9/overlays/ov047/delinks.txt"
            delinks.parent.mkdir(parents=True)
            delinks.write_text(
                "    .text start:0x00001000 end:0x00001100 kind:code align:4\n"
                "    .data start:0x00002000 end:0x00002100 kind:data align:4\n\n"
                "src/Only.cpp:\n    complete\n"
                "    .text start:0x00001000 end:0x00001010\n\n",
                encoding="utf-8")
            (root / "src_tu").mkdir()
            (root / "src_tu/TU.cpp").write_text("//cpp\n", encoding="utf-8")
            (root / "src").mkdir()
            (root / "src/Only.cpp").write_text("//cpp\n", encoding="utf-8")
            entry = {
                "id": "ov047/TU", "module": "ov047", "status": "text-verified",
                "production_mode": "intact-object", "source": "src_tu/TU.cpp",
                "promoted_source": "src/TU.cpp",
                "sections": [
                    {"name": ".text", "start": "0x00001000", "end": "0x00001010"},
                    {"name": ".data", "start": "0x00002000", "end": "0x00002010"}],
                "functions": [{"symbol": "Only", "address": "0x1000", "size": "0x10",
                               "legacy_source": "src/Only.cpp"}],
                "verification": {"linkcheck": {}},
            }
            with mock.patch.object(TP, "REPO", root), \
                    mock.patch.object(TP, "CONFIG", config):
                with self.assertRaisesRegex(TP.PromoteError,
                                             "production admission preflight failed"):
                    TP.plan(entry)

    def test_nontext_promotion_without_intact_mode_is_refused(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            (root / "src_tu").mkdir()
            (root / "src_tu/TU.cpp").write_text("//cpp\n", encoding="utf-8")
            entry = {"id": "ov047/TU", "source": "src_tu/TU.cpp",
                     "promoted_source": "src/TU.cpp", "module": "ov047",
                     "sections": [
                         {"name": ".text", "start": "0x1000", "end": "0x1010"},
                         {"name": ".data", "start": "0x2000", "end": "0x2010"},
                     ]}
            with mock.patch.object(TP, "REPO", root):
                with self.assertRaisesRegex(TP.PromoteError, "intact-object"):
                    TP.plan(entry)


if __name__ == "__main__":
    unittest.main()
