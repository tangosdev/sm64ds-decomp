"""Reproducible path-byte measurements without weakening answer checks."""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import notes_bench as B


class NotesBenchTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = pathlib.Path(self.tmp.name)

    def write(self, path, text):
        p = self.root / path
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")

    def test_full_measurement_is_equal_for_lf_and_crlf(self):
        front = "Caf\u00e9 [answer](notes/answer.md)\n"
        answer = "The \u732b fact is here.\n"
        self.write("README.md", front)
        self.write("notes/answer.md", answer)
        rows = [{"id": "Q", "answer_file": "notes/answer.md",
                 "answer_regex": "fact is here", "question": "Where is the fact?"}]
        first = B.run(self.root, rows)
        self.assertTrue(first[0]["resolves"])
        self.assertEqual(first[0]["hops"], 1)
        self.assertEqual(first[0]["path_bytes"], len((front + answer).encode("utf-8")))
        for p in self.root.rglob("*.md"):
            p.write_bytes(p.read_bytes().replace(b"\n", b"\r\n"))
        self.assertEqual(B.run(self.root, rows), first)

    def test_equal_hop_paths_use_lexical_neighbors(self):
        self.write("README.md", "Front\n")
        graph = {"README.md": ["notes/z.md", "notes/a.md"],
                 "notes/z.md": ["notes/end.md"],
                 "notes/a.md": ["notes/end.md"]}
        dist, parent = B.bfs(graph, self.root)
        self.assertEqual(parent["notes/end.md"], "notes/a.md")
        self.assertEqual(dist["notes/end.md"], 2)
        sizes = {"README.md": 10, "notes/a.md": 100, "notes/z.md": 1,
                 "notes/end.md": 20}
        self.assertEqual(B.path_bytes_for("notes/end.md", sizes, parent, dist), 130)
        graph["README.md"].reverse()
        self.assertEqual(B.bfs(graph, self.root), (dist, parent))

    def test_front_door_priority_is_preserved(self):
        self.write("AGENTS.md", "Front A\n")
        self.write("README.md", "Front B\n")
        _, parent = B.bfs({"README.md": {"notes/end.md"},
                           "AGENTS.md": {"notes/end.md"}}, self.root)
        self.assertEqual(parent["notes/end.md"], "AGENTS.md")

    def test_shorter_route_beats_lexically_earlier_detour(self):
        self.write("README.md", "Front\n")
        graph = {"README.md": {"notes/a.md", "notes/end.md"},
                 "notes/a.md": {"notes/end.md"}}
        dist, parent = B.bfs(graph, self.root)
        self.assertEqual(dist["notes/end.md"], 1)
        self.assertEqual(parent["notes/end.md"], "README.md")

    def test_missing_answer_and_changed_fact_remain_failures(self):
        self.assertFalse(B.check_resolves(self.root, "notes/missing.md", "fact")[0])
        self.write("notes/answer.md", "Different text\n")
        self.assertFalse(B.check_resolves(self.root, "notes/answer.md", "old fact")[0])
        self.assertFalse(B.check_resolves(self.root, "notes/answer.md", "[")[0])

    def test_unreachable_path_has_no_byte_count(self):
        self.assertIsNone(B.path_bytes_for("missing", {}, {}, {}))


if __name__ == "__main__":
    unittest.main(verbosity=2)
