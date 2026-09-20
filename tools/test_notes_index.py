"""Cross-platform accounting and real failure controls for the generated notes index."""
import contextlib
import io
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import notes_index as N


class NotesIndexTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = pathlib.Path(self.tmp.name)
        self.write("notes/README.md", "Index seed\n")
        self.write("notes/guide.md", "Caf\u00e9 \u732b\nSecond line\n")
        rows = [
            ("notes/README.md", "doctrine", "process", "live", "", "Generated index."),
            ("notes/guide.md", "doctrine", "matching", "live", "", "A guide."),
            ("notes/data/notes-registry.tsv", "data", "process", "live", "", "Registry."),
        ]
        self.write("notes/data/notes-registry.tsv", "\t".join(N.FIELDS) + "\n" +
                   "".join("\t".join(row) + "\n" for row in rows))
        stack = contextlib.ExitStack()
        self.addCleanup(stack.close)
        stack.enter_context(mock.patch.multiple(N, REPO=self.root,
            README=self.root / "notes/README.md",
            REGISTRY=self.root / "notes/data/notes-registry.tsv",
            BUCKET_ORDER=("process", "matching")))
        stack.enter_context(mock.patch.object(N, "tracked_or_trackable_notes_files",
            side_effect=lambda repo: sorted(p.relative_to(repo).as_posix()
                for p in (repo / "notes").rglob("*") if p.is_file())))

    def write(self, path, text):
        p = self.root / path
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")

    def run_main(self, *args):
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            rc = N.main(list(args))
        return rc, output.getvalue()

    def make_crlf(self):
        for p in (self.root / "notes").rglob("*"):
            if p.is_file():
                p.write_bytes(p.read_bytes().replace(b"\r\n", b"\n").replace(b"\n", b"\r\n"))

    def test_budget_counts_utf8_bytes_independent_of_checkout_endings(self):
        rows = N.load_registry(N.REGISTRY)
        expected = len("Index seed\nCaf\u00e9 \u732b\nSecond line\n".encode("utf-8"))
        self.assertEqual(N.doctrine_bytes(rows, self.root)[0], expected)
        self.make_crlf()
        self.assertEqual(N.doctrine_bytes(rows, self.root)[0], expected)

    def test_write_reaches_fixed_point_and_is_idempotent(self):
        self.assertEqual(self.run_main("--write")[0], 0)
        first = N.README.read_bytes()
        self.assertEqual(self.run_main("--check")[0], 0)
        self.assertEqual(self.run_main("--write")[0], 0)
        self.assertEqual(N.README.read_bytes(), first)
        self.assertEqual(N.render_readme(N.load_registry(N.REGISTRY), self.root),
                         first.decode("utf-8"))

    def test_all_crlf_checkout_checks_without_regeneration(self):
        self.assertEqual(self.run_main("--write")[0], 0)
        first = N.README.read_bytes()
        self.make_crlf()
        self.assertEqual(self.run_main("--check")[0], 0)
        self.assertEqual(self.run_main("--write")[0], 0)
        self.assertEqual(N.README.read_bytes(), first)

    def test_stale_readme_still_fails(self):
        self.assertEqual(self.run_main("--write")[0], 0)
        self.write("notes/README.md", N.README.read_text(encoding="utf-8") + "Stale\n")
        rc, message = self.run_main("--check")
        self.assertEqual(rc, 1)
        self.assertIn("is stale", message)

    def test_unregistered_note_still_fails(self):
        self.assertEqual(self.run_main("--write")[0], 0)
        self.write("notes/new.md", "Not registered\n")
        rc, message = self.run_main("--check")
        self.assertEqual(rc, 1)
        self.assertIn("has no registry row", message)

    def test_missing_registered_note_still_fails(self):
        self.assertEqual(self.run_main("--write")[0], 0)
        (self.root / "notes/guide.md").unlink()
        rc, message = self.run_main("--check")
        self.assertEqual(rc, 1)
        self.assertIn("does not exist", message)


if __name__ == "__main__":
    unittest.main(verbosity=2)
