"""rom_fingerprint.py: the ROM hash recorded in build/rtti_vtables.json, and who checks it.

Every test builds a small fake tree in a temp directory (arm9_dec.bin, two overlays,
and a dsd extraction beside them), so none of it needs the ROM or the toolchain.

    python -m unittest tools.test_rom_fingerprint -v
"""
import contextlib
import io
import json
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

import rom_fingerprint as RF  # noqa: E402
import rtti_vtables as RV     # noqa: E402
import tu_map as TM           # noqa: E402

REBUILD = "python tools/rtti_vtables.py"


def make_tree(root, stale=()):
    """arm9 + ov000 + ov001 as ROM images, with the dsd extraction copied from them.
    `stale` names overlays whose extraction is made different afterwards."""
    ex = pathlib.Path(root) / "extracted"
    (ex / "overlays").mkdir(parents=True)
    (ex / "dsd" / "arm9_overlays").mkdir(parents=True)
    (ex / "arm9_dec.bin").write_bytes(b"A" * 64)
    for i, body in enumerate((b"\x01" * 32, b"\x02" * 48)):
        (ex / "overlays" / ("overlay_%04d.bin" % i)).write_bytes(body)
        (ex / "dsd" / "arm9_overlays" / ("ov%03d.bin" % i)).write_bytes(body)
    for name in stale:
        (ex / "dsd" / "arm9_overlays" / (name + ".bin")).write_bytes(b"\x09" * 40)
    return pathlib.Path(root)


class FingerprintTests(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmp.cleanup)
        self.root = make_tree(self._tmp.name)

    def test_it_covers_arm9_and_every_overlay_and_is_stable(self):
        a = RF.fingerprint(self.root)
        self.assertEqual(a["modules"], 3)
        self.assertEqual(a["scheme"], RF.SCHEME)
        self.assertEqual(a, RF.fingerprint(self.root))

    def test_it_changes_when_a_rom_image_changes(self):
        before = RF.fingerprint(self.root)["sha256"]
        p = self.root / "extracted" / "overlays" / "overlay_0001.bin"
        p.write_bytes(b"\x02" * 47 + b"\x03")
        self.assertNotEqual(before, RF.fingerprint(self.root)["sha256"])

    def test_it_ignores_the_dsd_extraction(self):
        before = RF.fingerprint(self.root)["sha256"]
        (self.root / "extracted" / "dsd" / "arm9_overlays" / "ov000.bin").write_bytes(b"x")
        self.assertEqual(before, RF.fingerprint(self.root)["sha256"])

    def test_no_rom_images_is_an_error_not_a_hash_of_nothing(self):
        with tempfile.TemporaryDirectory() as empty:
            with self.assertRaises(SystemExit):
                RF.fingerprint(empty)


class ExtractionTests(unittest.TestCase):
    def tree(self, **kw):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        return make_tree(tmp.name, **kw)

    def test_a_matching_extraction_has_no_mismatch_and_passes_the_gate(self):
        root = self.tree()
        self.assertEqual(RF.extraction_mismatches(root), [])
        RF.require_matching_extraction("t", root)

    def test_a_different_overlay_is_named(self):
        root = self.tree(stale=("ov001",))
        self.assertEqual(RF.extraction_mismatches(root), [("ov001", 48, 40)])

    def test_same_size_but_different_bytes_still_counts(self):
        root = self.tree()
        p = root / "extracted" / "dsd" / "arm9_overlays" / "ov000.bin"
        p.write_bytes(b"\x07" * 32)
        self.assertEqual(RF.extraction_mismatches(root), [("ov000", 32, 32)])

    def test_a_missing_extraction_file_is_a_mismatch(self):
        root = self.tree()
        (root / "extracted" / "dsd" / "arm9_overlays" / "ov000.bin").unlink()
        self.assertEqual(RF.extraction_mismatches(root), [("ov000", 32, None)])

    def test_the_gate_refuses_and_says_which_overlay(self):
        root = self.tree(stale=("ov000", "ov001"))
        with self.assertRaises(SystemExit) as cm:
            RF.require_matching_extraction("rtti_vtables", root)
        msg = str(cm.exception)
        self.assertIn("rtti_vtables", msg)
        self.assertIn("2 of 2 overlays differ", msg)
        self.assertIn("ov000", msg)

    def test_rtti_vtables_main_refuses_before_reading_anything(self):
        root = self.tree(stale=("ov000",))
        with mock.patch.object(RF, "REPO", root), \
                mock.patch.object(sys, "argv", ["rtti_vtables.py"]):
            with self.assertRaises(SystemExit) as cm:
                RV.main()
        self.assertIn("does not match the ROM images", str(cm.exception))


class FileShapeTests(unittest.TestCase):
    def test_stamp_puts_the_fingerprint_first_and_split_takes_it_off(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = make_tree(tmp)
            tables = {"dActor_c": {"slots": [1, 2]}, "fBase_c": {"slots": [1]}}
            stamped = RF.stamp(tables, root)
            self.assertEqual(next(iter(stamped)), RF.KEY)
            got, recorded = RF.split(json.loads(json.dumps(stamped)))
            self.assertEqual(got, tables)
            self.assertEqual(recorded, RF.fingerprint(root))

    def test_split_of_an_old_file_has_no_fingerprint(self):
        tables = {"dActor_c": {"slots": [1]}}
        self.assertEqual(RF.split(dict(tables)), (tables, None))


class GuardTests(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmp.cleanup)
        self.root = make_tree(self._tmp.name)
        RF._warned.clear()

    def run_guard(self, recorded, what="build/x.json"):
        err = io.StringIO()
        with contextlib.redirect_stderr(err):
            try:
                return RF.guard(recorded, what, REBUILD, self.root), err.getvalue()
            except SystemExit as e:
                return e, err.getvalue()

    def test_the_same_roms_match(self):
        verdict, err = self.run_guard(RF.fingerprint(self.root))
        self.assertEqual(verdict, "match")
        self.assertEqual(err, "")

    def test_other_roms_are_refused_with_the_rebuild_command(self):
        recorded = RF.fingerprint(self.root)
        (self.root / "extracted" / "overlays" / "overlay_0000.bin").write_bytes(b"\x05" * 32)
        verdict, _err = self.run_guard(recorded)
        self.assertIsInstance(verdict, SystemExit)
        self.assertIn("different ROM images", str(verdict))
        self.assertIn(REBUILD, str(verdict))

    def test_a_file_without_the_field_warns_loudly_once_and_is_not_refused(self):
        verdict, err = self.run_guard(None)
        self.assertEqual(verdict, "absent")
        self.assertIn("WARNING", err)
        self.assertIn("no ROM fingerprint", err)
        self.assertIn(REBUILD, err)
        _verdict, again = self.run_guard(None)
        self.assertEqual(again, "")

    def test_a_malformed_record_is_treated_as_absent(self):
        for bad in ({}, {"scheme": 1}, "abc", []):
            RF._warned.clear()
            verdict, _err = self.run_guard(bad)
            self.assertEqual(verdict, "absent", bad)

    def test_a_different_scheme_is_refused(self):
        recorded = dict(RF.fingerprint(self.root), scheme=0)
        verdict, _err = self.run_guard(recorded)
        self.assertIsInstance(verdict, SystemExit)


class TuMapReaderTests(unittest.TestCase):
    """tu_map reads build/rtti_vtables.json through the guard."""

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmp.cleanup)
        self.root = make_tree(self._tmp.name)
        self.vt = self.root / "rtti_vtables.json"
        RF._warned.clear()
        for patch in (mock.patch.object(RF, "REPO", self.root),
                      mock.patch.object(TM, "VTABLES", self.vt),
                      mock.patch.object(TM, "_require_vtables", lambda: None)):
            patch.start()
            self.addCleanup(patch.stop)

    def write(self, stamped):
        tables = {"dActor_c": {"module": "ov002", "slots": [0x02001000, 0, 0x02001004],
                               "own": []}}
        data = RF.stamp(tables, self.root) if stamped else tables
        self.vt.write_text(json.dumps(data), encoding="utf-8")

    def labels(self):
        err = io.StringIO()
        with contextlib.redirect_stderr(err):
            out = TM.vtable_labels()
        return out, err.getvalue()

    def test_a_matching_file_labels_functions_and_never_reads_the_key_as_a_class(self):
        self.write(stamped=True)
        out, err = self.labels()
        self.assertEqual(out["ov002"], {0x02001000: "dActor_c", 0x02001004: "dActor_c"})
        self.assertEqual(err, "")

    def test_a_file_from_other_rom_images_is_refused(self):
        self.write(stamped=True)
        (self.root / "extracted" / "arm9_dec.bin").write_bytes(b"B" * 64)
        with self.assertRaises(SystemExit) as cm:
            self.labels()
        self.assertIn("different ROM images", str(cm.exception))

    def test_an_old_file_still_loads_with_a_warning(self):
        self.write(stamped=False)
        out, err = self.labels()
        self.assertEqual(out["ov002"][0x02001000], "dActor_c")
        self.assertIn("no ROM fingerprint", err)

    def test_the_factory_reader_goes_through_the_same_gate(self):
        self.write(stamped=True)
        (self.root / "extracted" / "arm9_dec.bin").write_bytes(b"B" * 64)
        with self.assertRaises(SystemExit):
            TM.vtable_factory_classes()


if __name__ == "__main__":
    unittest.main()
