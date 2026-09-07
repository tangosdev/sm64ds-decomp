"""The queue file carries its own reading instructions as '#' rows.

csv wants the column header on line 1, so the note block cannot precede it; it
sits directly below. Without the filter in `rows()` the very first thing
`classqueue.py next` hands a writer is a comment line, which reads as a class
name that does not exist.
"""
import unittest
import pathlib
import sys
import tempfile
import types
import unittest.mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import classqueue


HEADER = "class_name\tshard_count\talready_promoted\tblockers\n"


class CommentRowsTest(unittest.TestCase):
    def _queue(self, body):
        tmp = pathlib.Path(tempfile.mkdtemp()) / "tu-promotion-queue.tsv"
        tmp.write_text(HEADER + body, encoding="utf-8")
        return tmp

    def test_note_rows_are_dropped(self):
        q = self._queue("# shard_count is a FLOOR, not a figure.\n"
                        "#\n"
                        "daBar_c\t10\tno\t-\n")
        with unittest.mock.patch.object(classqueue, "queue_path", lambda: q):
            _path, rows = classqueue.rows()
        self.assertEqual([r["class_name"] for r in rows], ["daBar_c"])

    def test_real_rows_survive_without_notes(self):
        q = self._queue("daBar_c\t10\tno\t-\ndBgActor_c\t11\tyes\t-\n")
        with unittest.mock.patch.object(classqueue, "queue_path", lambda: q):
            _path, rows = classqueue.rows()
        self.assertEqual([r["class_name"] for r in rows], ["daBar_c", "dBgActor_c"])

    def test_next_skips_notes_and_promoted(self):
        q = self._queue("# note\n"
                        "daBar_c\t10\tyes\t-\n"
                        "dBgActor_c\t11\tno\t-\n")
        with unittest.mock.patch.object(classqueue, "queue_path", lambda: q), \
             unittest.mock.patch.object(classqueue, "held", lambda: {}):
            _path, rows = classqueue.rows()
        workable = [r["class_name"] for r in rows if classqueue.workable(r)]
        self.assertEqual(workable, ["dBgActor_c"])

    def test_updated_legacy_acquisition_refuses_active_v2(self):
        result = types.SimpleNamespace(stdout="123\trefs/heads/agents/coordination\n")
        with unittest.mock.patch.object(classqueue, "git", return_value=result):
            with self.assertRaisesRegex(SystemExit, "v2 coordination is active"):
                classqueue.require_legacy_queue()

    def test_legacy_transport_failure_does_not_mean_queue_is_empty(self):
        with unittest.mock.patch.object(classqueue, "git", side_effect=SystemExit("transport failed")):
            with self.assertRaisesRegex(SystemExit, "transport failed"):
                classqueue.require_legacy_queue()


if __name__ == "__main__":
    unittest.main()
