import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import check_data_definitions as CDD  # noqa: E402


class ObjectDiscoveryTests(unittest.TestCase):
    """The gate used to glob only build/src/*.o, which eligible.py never writes,
    so an eligible.py run left it reporting "no objects" and skipping."""

    def roots(self, tmp):
        root = pathlib.Path(tmp)
        old = CDD.BUILD, CDD.SCRATCH
        CDD.BUILD = root / "build" / "src"
        CDD.SCRATCH = root / "build" / "eligible-scratch"
        return root, old

    def test_scratch_objects_are_found_without_a_rombuild(self):
        with tempfile.TemporaryDirectory() as tmp:
            root, old = self.roots(tmp)
            try:
                nested = CDD.SCRATCH / "src" / "actors"
                nested.mkdir(parents=True)
                (nested / "Piano.o").write_bytes(b"")
                self.assertFalse(CDD.BUILD.exists())
                self.assertEqual([o.name for o in CDD.objects()], ["Piano.o"])
            finally:
                CDD.BUILD, CDD.SCRATCH = old

    def test_both_roots_are_scanned_and_labelled_by_source(self):
        with tempfile.TemporaryDirectory() as tmp:
            root, old = self.roots(tmp)
            try:
                CDD.BUILD.mkdir(parents=True)
                (CDD.BUILD / "Legacy.o").write_bytes(b"")
                nested = CDD.SCRATCH / "src" / "actors"
                nested.mkdir(parents=True)
                (nested / "Piano.o").write_bytes(b"")
                found = CDD.objects()
                self.assertEqual(len(found), 2)
                self.assertEqual(
                    sorted(CDD.label(o) for o in found),
                    ["src/Legacy", "src/actors/Piano"],
                )
            finally:
                CDD.BUILD, CDD.SCRATCH = old

    def test_missing_roots_report_nothing_rather_than_raising(self):
        with tempfile.TemporaryDirectory() as tmp:
            root, old = self.roots(tmp)
            try:
                self.assertEqual(CDD.objects(), [])
            finally:
                CDD.BUILD, CDD.SCRATCH = old


if __name__ == "__main__":
    unittest.main()
