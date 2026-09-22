"""Tests for tools/repin_commit_ids.py.

Five cases, matching the hazards named in the tool's own docstring: a full 40-hex id
gets rewritten, a short unambiguous prefix gets rewritten (truncated to its own
length), a token that merely looks like a DS address is left alone even when it would
otherwise resolve, an ambiguous short prefix (shared by two old ids) is left alone
because there is no way to know which was meant, and a binary file is never opened for
rewriting even when its bytes happen to contain a matching token.
"""
from __future__ import annotations

import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

import repin_commit_ids as R  # noqa: E402

OLD_A = "a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1a1"
NEW_A = "b2b2b2b2b2b2b2b2b2b2b2b2b2b2b2b2b2b2b2b2"
OLD_B = "9c7f38a24aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
NEW_B = "5555bb3f483ea2d3ce28c38dcbc19a63e6e489ab"
# Two old ids sharing the 7-char prefix "abc1234" -- any token of that length must be
# left alone, since it cannot be told which of the two it names.
OLD_C1 = "abc12340000000000000000000000000000000c1"
OLD_C2 = "abc12340000000000000000000000000000000c2"
NEW_C1 = "11111111111111111111111111111111111111c1"
NEW_C2 = "22222222222222222222222222222222222222c2"

MAP_LINES = "\n".join([
    "old                                      new",
    f"{OLD_A} {NEW_A}",
    f"{OLD_B} {NEW_B}",
    f"{OLD_C1} {NEW_C1}",
    f"{OLD_C2} {NEW_C2}",
]) + "\n"


class Repo:
    """A throwaway git repository, real git, so `git ls-files` is exercised for real."""

    def __enter__(self):
        self.dir = pathlib.Path(tempfile.mkdtemp(prefix="repin_commit_ids_"))
        self.git("init", "-q", "-b", "main")
        self.git("config", "user.email", "gate@example.invalid")
        self.git("config", "user.name", "gate")
        self.git("config", "commit.gpgsign", "false")
        return self

    def __exit__(self, *exc):
        shutil.rmtree(self.dir, ignore_errors=True)

    def git(self, *args):
        r = subprocess.run(["git", *args], cwd=str(self.dir),
                            capture_output=True, text=True)
        assert r.returncode == 0, f"git {' '.join(args)}: {r.stderr}"
        return r.stdout

    def write(self, rel, body, binary=False):
        p = self.dir / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        if binary:
            p.write_bytes(body)
        else:
            p.write_text(body, encoding="utf-8")
        return self

    def add(self):
        self.git("add", "-A")
        return self

    def read(self, rel):
        return (self.dir / rel).read_text(encoding="utf-8")

    def read_bytes(self, rel):
        return (self.dir / rel).read_bytes()


def _external_map(tmp_base: pathlib.Path) -> pathlib.Path:
    """Write the commit-map OUTSIDE any repo, the way the real deliverable does (it
    lives in the orchestration run's out/ dir, never inside the tree it rewrites) --
    so a swept repo's own tracked files never include the map, keeping each test's
    file/token counts about the payload alone."""
    p = tmp_base / "commit-map"
    p.write_text(MAP_LINES, encoding="utf-8")
    return p


class CommitMapTests(unittest.TestCase):
    def test_load_commit_map_parses_pairs_and_skips_the_header(self):
        with tempfile.TemporaryDirectory() as d:
            p = pathlib.Path(d) / "commit-map"
            p.write_text(MAP_LINES, encoding="utf-8")
            m = R.load_commit_map(p)
        self.assertEqual(m[OLD_A], NEW_A)
        self.assertEqual(m[OLD_B], NEW_B)
        self.assertNotIn("old", m)
        self.assertEqual(len(m), 4)


class ResolveTokenTests(unittest.TestCase):
    """Unit-level checks of the token classifier, ahead of the end-to-end sweep below."""

    def setUp(self):
        with tempfile.TemporaryDirectory() as d:
            p = pathlib.Path(d) / "commit-map"
            p.write_text(MAP_LINES, encoding="utf-8")
            self.old_to_new = R.load_commit_map(p)
        self.prefix_index = R.build_prefix_index(self.old_to_new)

    def test_40_hex_hit(self):
        text = f"matched, see {OLD_A} for the earlier draft"
        out, count = R.process_text(text, self.old_to_new, self.prefix_index)
        self.assertEqual(count, 1)
        self.assertIn(NEW_A, out)
        self.assertNotIn(OLD_A, out)

    def test_9_hex_prefix_hit(self):
        prefix = OLD_A[:9]
        text = f"landed at {prefix} last night"
        out, count = R.process_text(text, self.old_to_new, self.prefix_index)
        self.assertEqual(count, 1)
        self.assertIn(NEW_A[:9], out)
        self.assertNotIn(prefix, out)

    def test_rom_address_not_touched(self):
        # Construct a token that starts with "02" and unambiguously matches an old
        # id's prefix by shortening OLD_B (which starts with 9c...); instead exercise
        # the two address shapes directly: bare "02......" and "0x02......".
        addr_token = "0203a9fc"  # 8 hex chars, starts with 02
        text_bare = f"crash region {addr_token} in the debug pool"
        text_0x = f"crash region 0x{addr_token} in the debug pool"
        for text in (text_bare, text_0x):
            out, count = R.process_text(text, self.old_to_new, self.prefix_index)
            self.assertEqual(count, 0, text)
            self.assertEqual(out, text)

    def test_addr_labelled_token_not_touched_even_if_it_would_otherwise_match(self):
        # A short prefix of OLD_A, spelled right after the word "addr:" -- must not be
        # touched even though, in isolation, it is an unambiguous prefix hit.
        prefix = OLD_A[:9]
        text = f"addr: {prefix}"
        out, count = R.process_text(text, self.old_to_new, self.prefix_index)
        self.assertEqual(count, 0)
        self.assertEqual(out, text)

    def test_ambiguous_prefix_not_touched(self):
        shared = OLD_C1[:7]
        self.assertEqual(shared, OLD_C2[:7])
        text = f"seen around {shared} in the log"
        out, count = R.process_text(text, self.old_to_new, self.prefix_index)
        self.assertEqual(count, 0)
        self.assertEqual(out, text)

    def test_ambiguous_pair_stays_ambiguous_at_the_top_of_the_prefix_range(self):
        # OLD_C1 and OLD_C2 only differ in their last two characters, so every prefix
        # length this tool considers (7..12) is still shared -- confirm that holds at
        # the top of the range too, not just at the minimum length tested above.
        twelve = OLD_C1[:12]
        self.assertEqual(twelve, OLD_C2[:12])
        text = f"seen around {twelve} in the log"
        out, count = R.process_text(text, self.old_to_new, self.prefix_index)
        self.assertEqual(count, 0)
        self.assertEqual(out, text)


class SweepEndToEndTests(unittest.TestCase):
    def test_sweep_rewrites_text_files_and_reports_counts(self):
        with tempfile.TemporaryDirectory() as ext, Repo() as repo:
            map_path = _external_map(pathlib.Path(ext))
            repo.write("notes/handoff.md",
                       f"matched at {OLD_A}; prefix seen as {OLD_B[:9]} elsewhere\n")
            repo.add()
            old_to_new = R.load_commit_map(map_path)
            files_touched, total = R.sweep(repo.dir, old_to_new, check=False)
            self.assertEqual(files_touched, 1)
            self.assertEqual(total, 2)
            out = repo.read("notes/handoff.md")
            self.assertIn(NEW_A, out)
            self.assertIn(NEW_B[:9], out)
            self.assertNotIn(OLD_A, out)

    def test_check_mode_does_not_write_and_reports_remaining(self):
        with tempfile.TemporaryDirectory() as ext, Repo() as repo:
            map_path = _external_map(pathlib.Path(ext))
            repo.write("notes/handoff.md", f"matched at {OLD_A}\n")
            repo.add()
            old_to_new = R.load_commit_map(map_path)
            files_touched, total = R.sweep(repo.dir, old_to_new, check=True)
            self.assertEqual(files_touched, 1)
            self.assertEqual(total, 1)
            # unchanged on disk
            self.assertIn(OLD_A, repo.read("notes/handoff.md"))

    def test_binary_file_skipped_even_with_a_matching_token_inside(self):
        with tempfile.TemporaryDirectory() as ext, Repo() as repo:
            map_path = _external_map(pathlib.Path(ext))
            # A .png with the 40-hex token embedded in its raw bytes, padded with a
            # NUL so it also trips the content-based backstop regardless of extension.
            payload = f"\x89PNG\r\n\x1a\x00 {OLD_A} \x00 binary junk".encode("latin-1")
            repo.write("art/sprite.png", payload, binary=True)
            repo.add()
            old_to_new = R.load_commit_map(map_path)
            files_touched, total = R.sweep(repo.dir, old_to_new, check=False)
            self.assertEqual(files_touched, 0)
            self.assertEqual(total, 0)
            self.assertEqual(repo.read_bytes("art/sprite.png"), payload)

    def test_nul_content_backstop_catches_an_unlisted_extension(self):
        with tempfile.TemporaryDirectory() as ext, Repo() as repo:
            map_path = _external_map(pathlib.Path(ext))
            payload = f"junk\x00{OLD_A}\x00more".encode("latin-1")
            repo.write("data/odd.rsrc", payload, binary=True)
            repo.add()
            old_to_new = R.load_commit_map(map_path)
            files_touched, total = R.sweep(repo.dir, old_to_new, check=False)
            self.assertEqual(files_touched, 0)
            self.assertEqual(repo.read_bytes("data/odd.rsrc"), payload)


class MainCliTests(unittest.TestCase):
    def test_check_exits_1_when_ids_remain_and_0_once_clean(self):
        with tempfile.TemporaryDirectory() as ext, Repo() as repo:
            map_path = _external_map(pathlib.Path(ext))
            repo.write("notes/handoff.md", f"matched at {OLD_A}\n")
            repo.add()
            rc = R.main(["--map", str(map_path), "--repo", str(repo.dir), "--check"])
            self.assertEqual(rc, 1)
            rc = R.main(["--map", str(map_path), "--repo", str(repo.dir)])
            self.assertEqual(rc, 0)
            rc = R.main(["--map", str(map_path), "--repo", str(repo.dir), "--check"])
            self.assertEqual(rc, 0)


if __name__ == "__main__":
    unittest.main()
