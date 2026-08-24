"""Tests for the TU manifest's on-disk shape.

The point of the directory layout is that two TU PRs never touch the same file,
so the invariants that matter are: a load/save round-trip loses nothing, the
in-memory shape is identical to what the old single file produced, and an entry
that goes away does not linger as a stale file the next load would read back in.
"""
from __future__ import annotations

import json
import pathlib
import sys
import tempfile
import unittest

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import tu_manifest as TUM  # noqa: E402


def _entry(tu_id, **kw):
    e = {"id": tu_id, "module": tu_id.split("/")[0], "status": "text-verified"}
    e.update(kw)
    return e


class SlugTests(unittest.TestCase):
    def test_module_becomes_a_directory(self):
        self.assertEqual(TUM.slug("ov002/CameraTag"), "ov002/CameraTag.json")

    def test_plus_survives_because_a_real_entry_uses_it(self):
        self.assertEqual(TUM.slug("ov002/EnemySpawner+EnemySwitchTag"),
                         "ov002/EnemySpawner+EnemySwitchTag.json")

    def test_id_with_no_module_lands_at_the_top(self):
        self.assertEqual(TUM.slug("Loose"), "Loose.json")

    def test_deeper_ids_are_flattened_into_the_module_directory(self):
        self.assertEqual(TUM.slug("ov1/a/b/c"), "ov1/a-b-c.json")

    def test_traversal_cannot_escape_the_module_directory(self):
        self.assertEqual(TUM.slug("../../etc/passwd"), "etc/passwd.json")
        self.assertNotIn("..", TUM.slug("../../etc/passwd"))
        self.assertNotIn("\\", TUM.slug("ov1/x\\evil"))

    def test_an_id_with_no_usable_part_is_rejected(self):
        with self.assertRaises(ValueError):
            TUM.slug("../..")


class RoundTripTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self.tmp.name) / "tu_manifest.d"
        self.addCleanup(self.tmp.cleanup)

    def test_save_then_load_preserves_entries_and_meta(self):
        data = {"schema_version": 1, "about": "hello",
                "entries": [_entry("ov002/B"), _entry("ov001/A")]}
        TUM.save(data, self.root)
        back = TUM.load(self.root)
        self.assertEqual(back["schema_version"], 1)
        self.assertEqual(back["about"], "hello")
        self.assertEqual([e["id"] for e in back["entries"]], ["ov001/A", "ov002/B"])

    def test_entries_come_back_sorted_regardless_of_write_order(self):
        TUM.save({"entries": [_entry("ov9/Z"), _entry("ov0/A"), _entry("ov5/M")]}, self.root)
        self.assertEqual([e["id"] for e in TUM.load(self.root)["entries"]],
                         ["ov0/A", "ov5/M", "ov9/Z"])

    def test_one_file_per_entry_under_its_module_plus_meta(self):
        TUM.save({"entries": [_entry("ov1/A"), _entry("ov2/B")]}, self.root)
        names = sorted(p.relative_to(self.root).as_posix()
                       for p in self.root.rglob("*.json"))
        self.assertEqual(names, ["_meta.json", "ov1/A.json", "ov2/B.json"])

    def test_a_module_directory_holds_all_of_its_entries(self):
        TUM.save({"entries": [_entry("ov1/A"), _entry("ov1/B"), _entry("ov2/C")]},
                 self.root)
        self.assertEqual(sorted(p.name for p in (self.root / "ov1").glob("*.json")),
                         ["A.json", "B.json"])

    def test_dropped_entry_does_not_linger(self):
        TUM.save({"entries": [_entry("ov1/A"), _entry("ov2/B")]}, self.root)
        TUM.save({"entries": [_entry("ov1/A")]}, self.root)
        self.assertFalse((self.root / "ov2" / "B.json").exists())
        self.assertEqual([e["id"] for e in TUM.load(self.root)["entries"]], ["ov1/A"])

    def test_emptied_module_directory_is_removed(self):
        TUM.save({"entries": [_entry("ov1/A"), _entry("ov2/B")]}, self.root)
        TUM.save({"entries": [_entry("ov1/A")]}, self.root)
        self.assertFalse((self.root / "ov2").exists())

    def test_missing_root_loads_as_empty_not_an_error(self):
        self.assertEqual(TUM.load(self.root)["entries"], [])
        self.assertFalse(TUM.exists(self.root))

    def test_duplicate_ids_on_disk_are_rejected(self):
        TUM.save({"entries": [_entry("ov1/A")]}, self.root)
        (self.root / "ov1" / "sneaky.json").write_text(
            json.dumps(_entry("ov1/A")), encoding="utf-8")
        with self.assertRaises(ValueError):
            TUM.load(self.root)

    def test_entry_without_an_id_is_rejected(self):
        self.root.mkdir(parents=True)
        (self.root / "bad.json").write_text(json.dumps({"module": "ov1"}), encoding="utf-8")
        with self.assertRaises(ValueError):
            TUM.load(self.root)

    def test_colliding_slugs_are_rejected_on_save(self):
        # Deeper ids flatten into the module directory, so two of them can land
        # on one filename rather than silently overwriting each other.
        self.assertEqual(TUM.slug("ov1/a/b"), TUM.slug("ov1/a-b"))
        with self.assertRaises(ValueError):
            TUM.save({"entries": [_entry("ov1/a/b"), _entry("ov1/a-b")]}, self.root)


class SingleFileModeTests(unittest.TestCase):
    """`tubuild.py --manifest scratch.json` must keep working -- the test suite
    points it at a private copy so a `verify` run cannot mutate the tracked one."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.path = pathlib.Path(self.tmp.name) / "scratch.json"
        self.addCleanup(self.tmp.cleanup)

    def test_json_suffix_round_trips_as_one_file(self):
        data = {"schema_version": 1, "about": "x", "entries": [_entry("ov1/A")]}
        TUM.save(data, self.path)
        self.assertTrue(self.path.is_file())
        self.assertEqual(TUM.load(self.path), data)

    def test_missing_single_file_loads_as_empty(self):
        self.assertFalse(TUM.exists(self.path))
        self.assertEqual(TUM.load(self.path)["entries"], [])


class TrackedManifestTests(unittest.TestCase):
    def test_the_real_manifest_loads_and_every_entry_has_a_unique_id(self):
        data = TUM.load()
        ids = [e["id"] for e in data["entries"]]
        self.assertTrue(ids, "the tracked manifest is empty")
        self.assertEqual(len(ids), len(set(ids)))

    def test_every_tracked_entry_file_is_named_for_the_id_it_holds(self):
        for path in TUM.DEFAULT_ROOT.rglob("*.json"):
            if path.name == TUM.META_NAME:
                continue
            entry = json.loads(path.read_text(encoding="utf-8"))
            self.assertEqual(path.relative_to(TUM.DEFAULT_ROOT).as_posix(),
                             TUM.slug(entry["id"]),
                             f"{path} holds id {entry['id']!r}")

    def test_every_tracked_entry_sits_under_its_own_module(self):
        for path in TUM.DEFAULT_ROOT.rglob("*.json"):
            if path.name == TUM.META_NAME:
                continue
            entry = json.loads(path.read_text(encoding="utf-8"))
            self.assertEqual(path.parent.name, entry["module"],
                             f"{path} belongs to module {entry['module']!r}")

    def test_rewriting_the_tracked_manifest_is_a_no_op(self):
        """Saving what was just loaded must not churn the tree -- otherwise every
        tubuild run would show spurious diffs on entries it never touched."""
        def snap(root):
            return {p.relative_to(root).as_posix(): p.read_bytes()
                    for p in root.rglob("*.json")}
        before = snap(TUM.DEFAULT_ROOT)
        with tempfile.TemporaryDirectory() as td:
            mirror = pathlib.Path(td) / "tu_manifest.d"
            TUM.save(TUM.load(), mirror)
            after = snap(mirror)
        self.assertEqual(sorted(before), sorted(after))
        for name in before:
            self.assertEqual(before[name], after[name], f"{name} would be rewritten")


class ConsumersAcceptADirectoryRootTests(unittest.TestCase):
    """Every consumer must go through tu_manifest, not read the root as a file.

    check_src_tu_compiles had two manifest accesses and only one was converted;
    the survivor did `manifest_path.read_text()`, which against a directory
    raises PermissionError on Windows and IsADirectoryError elsewhere. No
    workflow runs that tool -- src-tu-refs.yml runs the refs half and leaves the
    compile half to pre-push and the build box -- so CI stayed green on a tool
    that could not start. This is the cheap guard for that shape.
    """

    def test_check_src_tu_compiles_reads_the_directory_root(self):
        import check_src_tu_compiles as CSTC

        # only=() a name no entry has: the manifest still has to be read and
        # filtered, which is the line that used to blow up, but nothing compiles
        # so this needs no toolchain.
        report = CSTC.check(TUM.DEFAULT_ROOT, only=("no/such/entry",))
        self.assertEqual(report["checked"]["units"], 0)
        self.assertTrue(any("no manifest entry" in f.get("message", "")
                            for f in report["failures"]),
                        f"expected the unknown-id failure, got {report['failures']}")

    def test_cpp_tu_state_reads_the_directory_root(self):
        import cpp_tu_state as STATE

        self.assertTrue(TUM.exists(REPO / "config" / "tu_manifest.d"))
        self.assertTrue(callable(STATE.collect))


if __name__ == "__main__":
    unittest.main()
