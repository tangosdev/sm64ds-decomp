"""Unit contracts for source-granular delinks regeneration."""
import collections
import contextlib
import io
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import enroll as EN  # noqa: E402


HEADER = ["    .text start:0x00001000 end:0x00002000 kind:code align:4"]
SOURCE = "src/actors/Pair.cpp"


def candidate(name, addr, size, rel=SOURCE, section=".text"):
    return (pathlib.Path("config/arm9"), name, rel, addr, size, section)


class EnrollStructuredEntries(unittest.TestCase):
    def test_parser_keeps_complete_and_every_section_line(self):
        text = """    .text start:0x00001000 end:0x00002000 kind:code align:4

src/actors/Pair.cpp:
    complete
    .text start:0x00001000 end:0x00001008
    .init start:0x00001800 end:0x00001810
    .data start:0x00003000 end:0x00003020
"""
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "delinks.txt"
            path.write_text(text, encoding="utf-8")
            header, entries = EN.read_delinks_full(path)
            legacy_header, marks = EN.read_delinks(path)

        self.assertEqual(header, HEADER)
        self.assertEqual(legacy_header, HEADER)
        self.assertEqual(marks, {SOURCE: True})
        self.assertEqual(entries, [(SOURCE, [
            "    complete",
            "    .text start:0x00001000 end:0x00001008",
            "    .init start:0x00001800 end:0x00001810",
            "    .data start:0x00003000 end:0x00003020",
        ])])

    def test_nested_entry_points_round_trip_as_one_complete_source(self):
        body = [
            "    complete",
            "    .text start:0x00001000 end:0x00001100",
            "    .init start:0x00001800 end:0x00001810",
            "    .data start:0x00003000 end:0x00003020",
            "    .bss start:0x00004000 end:0x00004010",
        ]
        rows = [
            candidate("Outer", 0x1000, 0x100),
            candidate("InnerA", 0x1020, 0x20),
            candidate("InnerB", 0x1040, 0x40),
        ]
        text, count, complete = EN.render_entries(HEADER, [(SOURCE, body)], rows)

        self.assertEqual(count, 1)
        self.assertEqual(complete, 1)
        self.assertEqual(text.count(f"{SOURCE}:"), 1)
        for line in body:
            self.assertEqual(text.count(line), 1)

    def test_demote_all_removes_only_complete(self):
        body = [
            "    complete",
            "    .text start:0x00001000 end:0x00001008",
            "    .data start:0x00003000 end:0x00003020",
        ]
        rows = [candidate("First", 0x1000, 4), candidate("Second", 0x1004, 4)]
        text, count, complete = EN.render_entries(
            HEADER, [(SOURCE, body)], rows, demote_all=True)

        self.assertEqual((count, complete), (1, 0))
        self.assertNotIn("    complete", text)
        self.assertIn(body[1], text)
        self.assertIn(body[2], text)

    def test_partial_complete_list_for_shared_source_fails_closed(self):
        body = ["    .text start:0x00001000 end:0x00001008"]
        rows = [candidate("First", 0x1000, 4), candidate("Second", 0x1004, 4)]

        with self.assertRaisesRegex(EN.EnrollmentError, "partial --complete-list"):
            EN.render_entries(HEADER, [(SOURCE, body)], rows, {"First"})

        text, count, complete = EN.render_entries(
            HEADER, [(SOURCE, body)], rows, {"First", "Second"})
        self.assertEqual((count, complete), (1, 1))
        self.assertIn("    complete", text)

    def test_new_shared_source_without_authoritative_body_is_refused(self):
        rows = [candidate("First", 0x1000, 4), candidate("Second", 0x1004, 4)]
        with self.assertRaisesRegex(EN.EnrollmentError, "no existing delinks body"):
            EN.render_entries(HEADER, [], rows)

    def test_single_symbol_move_keeps_body_and_complete_state(self):
        name = "_ZN4Pair5FirstEv"
        old = f"src/{name}.cpp"
        new = f"mods/{name}.cpp"
        body = [
            "    complete",
            "    .text start:0x00001000 end:0x00001004",
            "    .data start:0x00003000 end:0x00003010",
        ]
        rows = [candidate(name, 0x1000, 4, rel=new)]
        text, count, complete = EN.render_entries(HEADER, [(old, body)], rows)

        self.assertEqual((count, complete), (1, 1))
        self.assertIn(f"{new}:", text)
        self.assertNotIn(f"{old}:", text)
        self.assertIn(body[1], text)
        self.assertIn(body[2], text)

    def test_new_single_symbol_keeps_legacy_generated_shape(self):
        rows = [candidate("First", 0x1000, 4)]
        text, count, complete = EN.render_entries(
            HEADER, [], rows, {"First"})
        self.assertEqual((count, complete), (1, 1))
        self.assertIn(f"{SOURCE}:\n    complete\n", text)
        self.assertIn("    .text start:0x00001000 end:0x00001004", text)

    def test_unmatched_rom_byte_placeholder_is_preserved(self):
        stale = "src/NonmatchingDraft.cpp"
        body = ["    .text start:0x00001100 end:0x00001110"]
        text, count, complete = EN.render_entries(HEADER, [(stale, body)], [])

        self.assertEqual((count, complete), (1, 0))
        self.assertIn(f"{stale}:\n{body[0]}\n", text)

    def test_unmatched_complete_source_fails_instead_of_implicit_demotion(self):
        stale = "src/MissingOrDraft.cpp"
        body = [
            "    complete",
            "    .text start:0x00001100 end:0x00001110",
        ]
        with self.assertRaisesRegex(EN.EnrollmentError, "complete existing source"):
            EN.render_entries(HEADER, [(stale, body)], [])

    def test_preserved_body_must_cover_every_candidate(self):
        body = ["    complete", "    .text start:0x00001000 end:0x00001004"]
        rows = [candidate("First", 0x1000, 4), candidate("Second", 0x1004, 4)]
        with self.assertRaisesRegex(EN.EnrollmentError, "does not cover"):
            EN.render_entries(HEADER, [(SOURCE, body)], rows)

    def test_main_refusal_writes_no_earlier_module(self):
        with tempfile.TemporaryDirectory() as td:
            repo = pathlib.Path(td)
            first = repo / "config/arm9"
            second = repo / "config/arm9/overlays/ov001"
            first.mkdir(parents=True)
            second.mkdir(parents=True)
            for module in (first, second):
                (module / "symbols.txt").write_text("", encoding="utf-8")
            first_text = (HEADER[0] + "\n\n"
                          "src/Stale.c:\n"
                          "    .text start:0x00001100 end:0x00001104\n")
            first_dl = first / "delinks.txt"
            first_dl.write_text(first_text, encoding="utf-8")
            (second / "delinks.txt").write_text(
                HEADER[0] + "\n\n" + SOURCE + ":\n"
                "    .text start:0x00001000 end:0x00001008\n", encoding="utf-8")
            complete = repo / "complete.txt"
            complete.write_text("First\n", encoding="utf-8")
            shared = [
                (first, "Solo", "src/Solo.c", 0x1000, 4, ".text"),
                (second, "First", SOURCE, 0x1000, 4, ".text"),
                (second, "Second", SOURCE, 0x1004, 4, ".text"),
            ]

            with mock.patch.object(EN, "REPO", repo), \
                    mock.patch.object(EN, "CONFIG", repo / "config"), \
                    mock.patch.object(EN, "candidates", return_value=(
                        shared, collections.Counter())), \
                    mock.patch.object(sys, "argv", [
                        "enroll.py", "--complete-list", str(complete)]), \
                    contextlib.redirect_stdout(io.StringIO()), \
                    contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(EN.main(), 2)

            self.assertEqual(first_dl.read_text(encoding="utf-8"), first_text)


if __name__ == "__main__":
    unittest.main()
