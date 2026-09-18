#!/usr/bin/env python3
"""Tests for queue_audit's --check-promoted arm -- the one that runs in CI.

The point of these is the FAILURE branches. A gate that only proves it can go
green cannot be told apart from `exit 0`, and this one exists precisely because
an unwatched column went stale for four days and cost an agent.

Each test builds a throwaway tree and repoints the module's REPO/QUEUE globals at
it, so nothing here reads or writes the real repository.
"""
import contextlib
import io as _io
import json
import pathlib
import tempfile
import unittest

from tools import queue_audit

COLS = ["class_name", "shard_count", "total_lines", "overlay",
        "already_promoted", "blockers"]
HEADER = "\t".join(COLS) + "\n"


@contextlib.contextmanager
def tree(rows, manifests):
    """A minimal repo: config/tu_manifest.d/<ov>/<Class>.json plus a queue TSV."""
    with tempfile.TemporaryDirectory() as d:
        root = pathlib.Path(d)
        for ov, cls, status in manifests:
            p = root / "config" / "tu_manifest.d" / ov
            p.mkdir(parents=True, exist_ok=True)
            (p / (cls + ".json")).write_text(
                json.dumps({"id": "%s/%s" % (ov, cls), "status": status}),
                encoding="utf-8")
        q = root / "notes" / "data" / "tu-promotion-queue.tsv"
        q.parent.mkdir(parents=True, exist_ok=True)
        q.write_text(HEADER + "".join(rows), encoding="utf-8")
        old_repo, old_queue = queue_audit.REPO, queue_audit.QUEUE
        queue_audit.REPO, queue_audit.QUEUE = root, q
        try:
            yield root
        finally:
            queue_audit.REPO, queue_audit.QUEUE = old_repo, old_queue


def run():
    """Return (exit_code, stdout) for one --check-promoted invocation."""
    buf = _io.StringIO()
    with contextlib.redirect_stdout(buf), contextlib.redirect_stderr(_io.StringIO()):
        rc = queue_audit.check_promoted_only()
    return rc, buf.getvalue()


def row(cls, ov, promoted):
    return "\t".join([cls, "1", "10", ov, promoted, "-"]) + "\n"


class CheckPromoted(unittest.TestCase):

    def test_agreeing_queue_passes(self):
        with tree([row("daFoo_c", "ov001", "yes"), row("daBar_c", "ov001", "no")],
                  [("ov001", "daFoo_c", "promoted"), ("ov001", "daBar_c", "draft")]):
            rc, out = run()
        self.assertEqual(rc, 0, out)
        self.assertIn("agrees with", out)

    def test_promoted_class_the_queue_calls_unpromoted_FAILS(self):
        """The exact shape that cost an agent: manifest promoted, queue says no."""
        with tree([row("daObjTh_Fall_Block_c", "ov063", "no")],
                  [("ov063", "daObjTh_Fall_Block_c", "promoted")]):
            rc, out = run()
        self.assertEqual(rc, 1, out)
        self.assertIn("daObjTh_Fall_Block_c", out)
        self.assertIn("is promoted but the queue says no", out)

    def test_unpromoted_class_the_queue_calls_promoted_FAILS(self):
        """The opposite direction must fail too, or the gate is one-sided."""
        with tree([row("daFoo_c", "ov001", "yes")],
                  [("ov001", "daFoo_c", "draft")]):
            rc, out = run()
        self.assertEqual(rc, 1, out)
        self.assertIn("is NOT promoted but the queue says yes", out)

    def test_multi_class_row_needs_EVERY_class_promoted(self):
        """`A+B` rows are one TU: a half-promoted pair is not promoted."""
        with tree([row("daFoo_c+daBar_c", "ov001", "yes")],
                  [("ov001", "daFoo_c", "promoted"), ("ov001", "daBar_c", "draft")]):
            rc, out = run()
        self.assertEqual(rc, 1, out)

    def test_rows_making_no_claim_are_skipped_not_failed(self):
        """UNATTRIBUTED carries '-': an aggregate count, not a class.

        Auditing it would red the gate permanently on a correct row, which is how
        a gate gets switched off.
        """
        with tree([row("UNATTRIBUTED", "(all overlays)", "-"),
                   row("daFoo_c", "ov001", "yes")],
                  [("ov001", "daFoo_c", "promoted")]):
            rc, out = run()
        self.assertEqual(rc, 0, out)
        self.assertIn("1 row(s) make no promotion claim", out)

    def test_comment_rows_are_not_audited(self):
        with tree([row("# a note row", "", ""), row("daFoo_c", "ov001", "yes")],
                  [("ov001", "daFoo_c", "promoted")]):
            rc, out = run()
        self.assertEqual(rc, 0, out)

    def test_no_manifest_entries_is_NOT_a_pass(self):
        with tree([row("daFoo_c", "ov001", "no")], []):
            rc, out = run()
        self.assertEqual(rc, 2, out)

    def test_meta_json_is_not_counted_as_an_entry(self):
        with tree([row("daFoo_c", "ov001", "no")], []) as root:
            p = root / "config" / "tu_manifest.d" / "ov001"
            p.mkdir(parents=True, exist_ok=True)
            (p / "_meta.json").write_text("{}", encoding="utf-8")
            rc, out = run()
        self.assertEqual(rc, 2, out)

    def test_queue_with_no_data_rows_is_NOT_a_pass(self):
        with tree([], [("ov001", "daFoo_c", "promoted")]):
            rc, out = run()
        self.assertEqual(rc, 2, out)

    def test_queue_where_no_row_makes_a_claim_is_NOT_a_pass(self):
        with tree([row("UNATTRIBUTED", "(all overlays)", "-")],
                  [("ov001", "daFoo_c", "promoted")]):
            rc, out = run()
        self.assertEqual(rc, 2, out)

    def test_output_names_what_it_does_not_check(self):
        """A green run must not read as the full audit."""
        with tree([row("daFoo_c", "ov001", "yes")],
                  [("ov001", "daFoo_c", "promoted")]):
            rc, out = run()
        self.assertEqual(rc, 0, out)
        self.assertIn("already_promoted ONLY", out)
        self.assertIn("NOT checked here", out)

    def test_needs_no_ROM_derived_input(self):
        """The whole reason this arm exists: build/ is absent on a CI runner."""
        with tree([row("daFoo_c", "ov001", "yes")],
                  [("ov001", "daFoo_c", "promoted")]) as root:
            self.assertFalse((root / "build").exists())
            rc, out = run()
        self.assertEqual(rc, 0, out)


if __name__ == "__main__":
    unittest.main()
