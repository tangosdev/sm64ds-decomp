"""Tests for tools/check_dead_references.py.

The point of this gate is to catch a rename that did not carry into the prose, so the
tests that matter are the POSITIVE CONTROLS: given a docstring that names a file which
is not there, does the tool actually say so? A reference-integrity gate that quietly
finds nothing is worse than no gate at all -- this tree has shipped one of those before
-- so `test_a_dead_reference_is_detected` and `test_a_scan_that_found_nothing_fails`
exist to make a vacuous pass impossible.

Self-running: `python tools/test_check_dead_references.py`, or via unittest/pytest.
Needs no ROM and no compiler.
"""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import check_dead_references as CDR  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent


class Tree:
    """A throwaway repo-shaped tree to point the collector at."""

    def __init__(self, tmp):
        self.root = pathlib.Path(tmp)
        (self.root / "tools").mkdir(parents=True, exist_ok=True)
        (self.root / "notes").mkdir(parents=True, exist_ok=True)
        (self.root / "include").mkdir(parents=True, exist_ok=True)

    def write(self, rel, text):
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")
        return p


class DetectionTests(unittest.TestCase):
    """Positive controls: the tool must actually find things."""

    def _dead(self, build):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            build(t)
            old = CDR.REPO
            CDR.REPO = t.root
            try:
                _files, _refs, dead = CDR.dead_references(t.root)
            finally:
                CDR.REPO = old
            return {(f, r) for f, r in dead}

    def test_a_dead_reference_is_detected(self):
        """A docstring naming a header that is not there must be reported."""
        dead = self._dead(lambda t: t.write(
            "tools/thing.py", '"""See include/Enemy.h for the layout."""\n'))
        self.assertIn(("tools/thing.py", "include/Enemy.h"), dead)

    def test_a_live_reference_is_not_reported(self):
        def build(t):
            t.write("include/dEnemyBase_c.h", "struct dEnemyBase_c {};\n")
            t.write("tools/thing.py", '"""See include/dEnemyBase_c.h for the layout."""\n')
        self.assertEqual(self._dead(build), set())

    def test_a_dead_reference_in_a_comment_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "tools/thing.py", "x = 1  # cf. notes/gone.md\n"))
        self.assertIn(("tools/thing.py", "notes/gone.md"), dead)

    def test_a_dead_reference_in_argparse_help_is_detected(self):
        dead = self._dead(lambda t: t.write("tools/thing.py", (
            "import argparse\n"
            "ap = argparse.ArgumentParser()\n"
            "ap.add_argument('--x', help='writes tools/vanished.py')\n")))
        self.assertIn(("tools/thing.py", "tools/vanished.py"), dead)

    def test_a_dead_reference_in_a_note_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "notes/live.md", "The rule is stated in `include/ClsnResult.h`.\n"))
        self.assertIn(("notes/live.md", "include/ClsnResult.h"), dead)

    def test_a_string_literal_is_not_prose(self):
        """Only docstrings, comments and argparse help count -- not ordinary code."""
        dead = self._dead(lambda t: t.write(
            "tools/thing.py", "OPEN = 'include/Enemy.h'\n"))
        self.assertEqual(dead, set())


class NoiseTests(unittest.TestCase):
    """Negative controls: the shapes that must never be called a path."""

    def _dead(self, body):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            t.write("tools/thing.py", f'"""{body}"""\n')
            old = CDR.REPO
            CDR.REPO = t.root
            try:
                _f, _r, dead = CDR.dead_references(t.root)
            finally:
                CDR.REPO = old
            return {r for _f, r in dead}

    def test_slash_separated_word_pairs_are_not_paths(self):
        for body in ("ctor/dtor", "if/else", "106/106", "bytes/relocs", "D0/D1/D2"):
            with self.subTest(body=body):
                self.assertEqual(self._dead(body), set())

    def test_generated_roots_are_skipped(self):
        for body in ("build/tu_map.json", "extracted/dsd/config.yaml",
                     "progress/matched.jsonl"):
            with self.subTest(body=body):
                self.assertEqual(self._dead(body), set())

    def test_globs_and_placeholders_are_skipped(self):
        for body in ("tools/**/*.py", "include/<Class>.h", "config/{a,b}/x.txt"):
            with self.subTest(body=body):
                self.assertEqual(self._dead(body), set())

    def test_a_non_repo_first_segment_is_skipped(self):
        self.assertEqual(self._dead("vendor/m2c/main.py"), set())


class RelativeLinkTests(unittest.TestCase):
    """A markdown link resolves against ITS OWN DIRECTORY, not the repo root.

    The positive control here is `test_the_2036_shape_is_detected`: the exact link that
    landed in the EAD cross-reference note with all four checks green, because the
    repo-rooted scan resolved the target from the ROOT and found it there. If that test
    ever starts passing vacuously, this gate is back where it was before #2037.
    """

    def _broken(self, build, dead=()):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            build(t)
            old = CDR.REPO
            CDR.REPO = t.root
            try:
                _links, broken = CDR.broken_links(t.root, dead)
            finally:
                CDR.REPO = old
            return set(broken)

    def _targets(self, build):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            build(t)
            old = CDR.REPO
            CDR.REPO = t.root
            try:
                links = CDR.collect_links(t.root)
            finally:
                CDR.REPO = old
            return [target for _f, _l, target, _r in links]

    def test_the_2036_shape_is_detected(self):
        """A link naming `config/...` from inside `notes/` points under `notes/`."""
        def build(t):
            t.write("config/rom-name-glossary.json", "{}\n")
            t.write("notes/crossref.md",
                    "Bumped in [the glossary](config/rom-name-glossary.json).\n")
        self.assertEqual(self._broken(build), {
            ("notes/crossref.md", 1, "config/rom-name-glossary.json",
             "notes/config/rom-name-glossary.json")})

    def test_a_correct_dotdot_link_resolves(self):
        def build(t):
            t.write("config/rom-name-glossary.json", "{}\n")
            t.write("notes/crossref.md",
                    "Bumped in [the glossary](../config/rom-name-glossary.json).\n")
        self.assertEqual(self._broken(build), set())

    def test_a_sibling_link_resolves(self):
        def build(t):
            t.write("notes/other.md", "x\n")
            t.write("notes/crossref.md", "See [the other note](other.md).\n")
        self.assertEqual(self._broken(build), set())

    def test_an_http_link_is_ignored(self):
        self.assertEqual(self._broken(lambda t: t.write("notes/n.md", (
            "Filed as [an issue](https://github.com/tangosdev/sm64ds-decomp/issues/2037)\n"
            "and raised with [a human](mailto:someone@example.com).\n"))), set())

    def test_an_anchor_only_link_is_ignored(self):
        self.assertEqual(self._broken(lambda t: t.write(
            "notes/n.md", "Jump to [the ratchet](#how-it-avoids-crying-wolf).\n")), set())

    def test_a_fragment_is_stripped_before_resolving(self):
        def build(t):
            t.write("notes/other.md", "x\n")
            t.write("notes/n.md", "See [the section](other.md#the-section).\n")
        self.assertEqual(self._broken(build), set())

    def test_a_link_to_a_directory_resolves(self):
        def build(t):
            t.write("include/dEnemyBase_c.h", "struct dEnemyBase_c {};\n")
            t.write("notes/n.md", "The headers live in [include](../include).\n")
        self.assertEqual(self._broken(build), set())

    def test_a_link_inside_a_fence_is_not_a_link(self):
        """Shown, not followed -- unlike a repo-rooted path in the same fence."""
        self.assertEqual(self._broken(lambda t: t.write("notes/n.md", (
            "Write it like this:\n\n"
            "```markdown\n"
            "[the glossary](config/rom-name-glossary.json)\n"
            "```\n"))), set())

    def test_a_link_inside_backticks_is_not_a_link(self):
        """Hand-rolled dispatch reads exactly like a link and is not one."""
        self.assertEqual(self._broken(lambda t: t.write(
            "notes/n.md",
            "Hand-rolling it as `(*(fn**)this)[4](this, ...)` reproduces the\n"
            "instructions but not the register.\n")), set())

    def test_angle_bracket_and_titled_targets_are_unwrapped(self):
        def build(t):
            t.write("notes/a b.md", "x\n")
            t.write("notes/other.md", "x\n")
            t.write("notes/n.md",
                    "See [spaced](<a b.md>) and [titled](other.md \"the note\").\n")
        self.assertEqual(self._broken(build), set())

    def test_a_reference_style_definition_is_resolved(self):
        def build(t):
            t.write("config/rom-name-glossary.json", "{}\n")
            t.write("notes/n.md",
                    "Cited as [glossary].\n\n[glossary]: config/rom-name-glossary.json\n")
        self.assertEqual(self._broken(build), {
            ("notes/n.md", 3, "config/rom-name-glossary.json",
             "notes/config/rom-name-glossary.json")})

    def test_a_repo_rooted_dead_link_is_not_reported_twice(self):
        """A link to a path that is dead BOTH ways belongs to the repo-rooted scan."""
        build = lambda t: t.write(
            "notes/n.md", "See [the header](../include/Enemy.h).\n")
        self.assertEqual(self._broken(build), {
            ("notes/n.md", 1, "../include/Enemy.h", "include/Enemy.h")})
        self.assertEqual(self._broken(build, dead=[("notes/n.md", "include/Enemy.h")]),
                         set())

    def test_link_syntax_in_python_is_not_a_link(self):
        """A regex character class followed by a group is not a markdown link."""
        self.assertEqual(self._targets(lambda t: t.write(
            "tools/thing.py", '"""Matches [a-z]+(?:pp)? in the name."""\n')), [])

    def test_the_tree_has_no_broken_relative_links(self):
        """Fail-closed: this check banks nothing, so the tree itself must be clean."""
        _files, _refs, dead = CDR.dead_references()
        links, broken = CDR.broken_links(REPO, dead)
        self.assertGreaterEqual(len(links), CDR.MIN_LINKS)
        self.assertEqual(broken, [], f"broken relative links: {broken}")


class FailLoudlyTests(unittest.TestCase):
    """A scan that inspected nothing must not print a pass."""

    def test_a_scan_that_found_nothing_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            old_repo, old_base = CDR.REPO, CDR.BASELINE
            CDR.REPO = pathlib.Path(tmp)
            CDR.BASELINE = pathlib.Path(tmp) / "config" / "baseline.json"
            try:
                rc = CDR.main([])
            finally:
                CDR.REPO, CDR.BASELINE = old_repo, old_base
        self.assertEqual(rc, 2, "an empty scan must exit non-zero, not report clean")

    def test_the_floors_are_below_the_real_tree(self):
        files, refs, _dead = CDR.dead_references()
        self.assertGreaterEqual(len(files), CDR.MIN_FILES)
        self.assertGreaterEqual(len(refs), CDR.MIN_REFS)


class BaselineTests(unittest.TestCase):

    def test_the_committed_baseline_matches_the_tree(self):
        """The gate passes on this checkout -- and passes because it ran, not because
        it found nothing."""
        files, refs, dead = CDR.dead_references()
        self.assertGreater(len(files), 0)
        self.assertGreater(len(refs), 0)
        known = CDR.load_baseline()
        new = sorted(set(dead) - known)
        self.assertEqual(new, [], f"unbaselined dead references: {new}")

    def test_the_baseline_is_wellformed(self):
        data = json.loads(CDR.BASELINE.read_text(encoding="utf-8"))
        self.assertIn("known", data)
        for e in data["known"]:
            self.assertEqual(set(e), {"file", "ref"})
            self.assertTrue((REPO / e["file"]).exists(),
                            f"baseline names a citing file that is gone: {e['file']}")

    def test_the_baseline_is_sorted_and_unique(self):
        data = json.loads(CDR.BASELINE.read_text(encoding="utf-8"))
        pairs = [(e["file"], e["ref"]) for e in data["known"]]
        self.assertEqual(pairs, sorted(pairs))
        self.assertEqual(len(pairs), len(set(pairs)))


class NewSurfaceTests(unittest.TestCase):
    """Positive and negative controls for the surfaces the 2026-09 extension added:
    `src/`/`src_tu/`/`include/` comments, `.js` string/template literals, config and
    root JSON, and `.jsonl`. Each gets a live-detection test and, where the surface
    exists specifically to suppress a false-positive population (TU-merge history in
    C/C++ comments, ledger files), a noise test proving the suppression still works.
    """

    def _dead(self, build):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            build(t)
            old_repo, old_base = CDR.REPO, CDR.BASELINE
            CDR.REPO = t.root
            # Needed the moment a test writes under config/: `_prose_targets` computes
            # `BASELINE.relative_to(REPO)`, which raises unless BASELINE is repointed
            # under this same tree (mirrors `FailLoudlyTests`' pattern).
            CDR.BASELINE = t.root / "config" / "dead-reference-baseline.json"
            try:
                _files, _refs, dead = CDR.dead_references(t.root)
            finally:
                CDR.REPO, CDR.BASELINE = old_repo, old_base
            return {(f, r) for f, r in dead}

    # -- src/, src_tu/, include/ comments --

    def test_a_dead_notes_reference_in_a_c_comment_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "src/thing.c", "/* see notes/gone.md for the story */\nint x;\n"))
        self.assertIn(("src/thing.c", "notes/gone.md"), dead)

    def test_a_reference_in_a_c_string_literal_is_not_detected(self):
        """Only comments count here -- a ROM string constant's stray `/` is noise."""
        dead = self._dead(lambda t: t.write(
            "src/thing.c", 'const char *s = "notes/gone.md";\n'))
        self.assertEqual(dead, set())

    def test_a_dead_src_reference_in_a_c_comment_is_not_reported(self):
        """`src/`/`src_tu/`/`include/` comments narrate their own TU-merge history in
        `src/` paths that are SUPPOSED to be gone -- only a `notes/` head counts here."""
        dead = self._dead(lambda t: t.write(
            "include/thing.h",
            "// absorbed from src/func_gone.c into this header\n"))
        self.assertEqual(dead, set())

    def test_legacy_listing_lines_are_stripped_from_c_comments(self):
        """Unit-level: `_c_comments` itself, since `NOTES_ONLY_HEADS` would mask the
        same effect if tested only through `dead_references`."""
        comments = CDR._c_comments(
            "/* absorbed functions:\n"
            " * [1] 0x02001234  src/func_gone_a.c\n"
            " * [2] 0x02001238  src/func_gone_b.c\n"
            " * see notes/gone.md for the full story\n"
            " */\n")
        joined = "\n".join(snippet for _ln, snippet in comments)
        self.assertNotIn("func_gone_a.c", joined)
        self.assertNotIn("func_gone_b.c", joined)
        self.assertIn("notes/gone.md", joined)

    # -- tools/**/*.js --

    def test_a_dead_reference_in_a_js_comment_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "tools/thing.js", "// see notes/gone.md\nconsole.log(1);\n"))
        self.assertIn(("tools/thing.js", "notes/gone.md"), dead)

    def test_a_dead_reference_in_a_js_template_literal_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "tools/thing.js",
            "const prompt = `Read notes/gone.md before you start.`\n"))
        self.assertIn(("tools/thing.js", "notes/gone.md"), dead)

    # -- config/**/*.json, incl. the tu_manifest.d field restriction --

    def test_a_dead_reference_in_config_json_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "config/thing.json", '{"note": "see notes/gone.md"}\n'))
        self.assertIn(("config/thing.json", "notes/gone.md"), dead)

    def test_tu_manifest_boundary_evidence_is_scanned(self):
        dead = self._dead(lambda t: t.write(
            "config/tu_manifest.d/ov001/Thing.json",
            json.dumps({"boundary_evidence": "see notes/gone.md",
                        "legacy_source": ["src/func_gone.c"]})))
        self.assertIn(
            ("config/tu_manifest.d/ov001/Thing.json", "notes/gone.md"), dead)

    def test_tu_manifest_legacy_source_is_not_scanned(self):
        """`legacy_source` names the pre-merge file a promotion absorbed -- gone on
        purpose, not a citation this gate should judge."""
        dead = self._dead(lambda t: t.write(
            "config/tu_manifest.d/ov001/Thing.json",
            json.dumps({"legacy_source": ["src/func_gone.c"]})))
        self.assertEqual(dead, set())

    def test_tu_manifest_prose_only_yields_boundary_evidence_and_notes(self):
        """Unit-level: `_tu_manifest_prose` itself."""
        text = json.dumps({
            "boundary_evidence": "see notes/gone.md",
            "notes": ["also notes/other.md"],
            "legacy_source": ["src/func_gone.c"],
        })
        refs = [snippet for _ln, snippet in CDR._tu_manifest_prose(text)]
        self.assertEqual(set(refs), {"see notes/gone.md", "also notes/other.md"})

    # -- root-level *.json, incl. ROOT_JSON_LEDGERS --

    def test_a_dead_reference_in_root_json_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "tangos.json", '{"readFirst": "notes/gone.md"}\n'))
        self.assertIn(("tangos.json", "notes/gone.md"), dead)

    def test_root_json_ledgers_are_skipped(self):
        dead = self._dead(lambda t: t.write(
            "attribution.json", '{"src/func_gone.c": {"who": "someone"}}\n'))
        self.assertEqual(dead, set())

    # -- **/*.jsonl, incl. JSONL_LEDGERS --

    def test_a_dead_reference_in_jsonl_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "notes/levers.jsonl", '{"source": "sm64ds:notes/gone.md"}\n'))
        self.assertIn(("notes/levers.jsonl", "notes/gone.md"), dead)

    def test_jsonl_ledgers_are_skipped(self):
        dead = self._dead(lambda t: t.write(
            "config/match_attempts.jsonl", '{"srcPath": "src/func_gone.c"}\n'))
        self.assertEqual(dead, set())


if __name__ == "__main__":
    unittest.main(verbosity=2)
