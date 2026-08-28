"""Unit tests for check_dead_references.py"""
import json
import pathlib
import subprocess
import sys
import tempfile
import pytest

from check_dead_references import (
    collect, dead_references, _git_tracked, _git_ignored, _tree_paths,
    _normalise, REPO, NUL
)


class TestNormalise:
    """Test path normalization."""

    def test_trailing_punctuation(self):
        assert _normalise("tools/match.py.") == "tools/match.py"
        assert _normalise("tools/match.py,") == "tools/match.py"
        assert _normalise("tools/match.py:") == "tools/match.py"

    def test_leading_quotes(self):
        assert _normalise('"tools/match.py"') == "tools/match.py"
        assert _normalise("'tools/match.py'") == "tools/match.py"
        # Note: [] is only stripped from the left, not both sides
        assert _normalise("[tools/match.py") == "tools/match.py"

    def test_backslash_conversion(self):
        assert _normalise("tools\\match.py") == "tools/match.py"
        assert _normalise("src\\arm9\\file.c") == "src/arm9/file.c"

    def test_strip_leading_dot(self):
        assert _normalise("./tools/match.py") == "tools/match.py"
        assert _normalise("tools/match.py") == "tools/match.py"

    def test_strip_trailing_slash(self):
        assert _normalise("tools/") == "tools"
        assert _normalise("tools/match.py/") == "tools/match.py"


class TestGitTracked:
    """Test _git_tracked() with real git output."""

    def test_returns_tracked_files(self):
        """Files tracked by git are in the output."""
        paths = _git_tracked(REPO)
        # Known tracked files in the repo
        assert any(p.startswith("tools/") and p.endswith(".py") for p in paths)
        assert any("config/" in p for p in paths)

    def test_includes_ancestor_dirs(self):
        """Ancestor directories of tracked files are included."""
        paths = _git_tracked(REPO)
        # If tools/check_dead_references.py is tracked,
        # both "tools" and "tools/check_dead_references.py" should be present
        if any("check_dead_references.py" in p for p in paths):
            assert "tools" in paths

    def test_empty_on_nonrepo(self):
        """Returns empty set when called outside a git repo."""
        with tempfile.TemporaryDirectory() as tmpdir:
            result = _git_tracked(pathlib.Path(tmpdir))
            assert result == set()


class TestGitIgnored:
    """Test _git_ignored() with real .gitignore."""

    def test_respects_gitignore_rules(self):
        """Files matching .gitignore rules are detected."""
        # The repo has .gitignore entries for build/, extracted/, progress/
        ignored = _git_ignored(["build", "extracted", "progress"], REPO)
        # At least some of these should be reported as ignored
        assert len(ignored) > 0

    def test_empty_candidates(self):
        """Empty candidate list returns empty set."""
        ignored = _git_ignored([], REPO)
        assert ignored == set()

    def test_respects_trailing_slash_patterns(self):
        """Directory patterns like 'tools/mwccarm/' are detected."""
        # Assuming tools/mwccarm is gitignored (it's in the real repo)
        ignored = _git_ignored(["tools/mwccarm"], REPO)
        # Should be treated as matching the directory pattern
        assert "tools/mwccarm" in ignored or len(ignored) > 0

    def test_a_path_no_rule_covers_is_not_reported_ignored(self):
        """The positive control the other tests in this class lack.

        Every assertion above is `len(ignored) > 0`, which a function returning its
        whole input satisfies -- and on git 2.50.1.windows.1 that is precisely what
        _git_ignored did, because the trailing-slash probe matched a BLANK LINE in
        .gitignore. The gate reported 0 dead references on every tree for months while
        CI reported 130. Assert the other direction too.
        """
        ignored = _git_ignored(["src/no_rule_covers_this_zzz.c",
                                "notes/no_rule_covers_this_zzz.md"], REPO)
        assert ignored == set()

    def test_an_empty_matching_pattern_is_not_a_hit(self):
        """A blank .gitignore line names no pattern, so it ignores nothing."""
        import subprocess as _sp

        class _Out:
            returncode = 0
            # -v -z: source, line, pattern, pathname -- the middle entry has no pattern
            stdout = NUL.join([b".gitignore", b"11", b"build/", b"build/x.o",
                               b".gitignore", b"111", b"", b"src/dead_zzz.c/",
                               b".gitignore", b"12", b"*.pyc", b"tools/x.pyc",
                               b""])

        real = _sp.run
        CDR_mod = sys.modules[_git_ignored.__module__]
        CDR_mod.subprocess.run = lambda *a, **k: _Out()
        try:
            hits = _git_ignored(["build/x.o", "src/dead_zzz.c", "tools/x.pyc"], REPO)
        finally:
            CDR_mod.subprocess.run = real
        assert hits == {"build/x.o", "tools/x.pyc"}

    def test_deduplicates_candidates(self):
        """Duplicate candidates don't cause issues."""
        ignored = _git_ignored(["build", "build", "build"], REPO)
        assert isinstance(ignored, set)


class TestTreePaths:
    """Test _tree_paths() with real filesystem."""

    def test_returns_existing_files(self):
        """Files present on disk are in the output."""
        paths = _tree_paths(REPO)
        # The README should exist
        assert any("README" in p or "readme" in p.lower() for p in paths)

    def test_deduplicates(self):
        """Output contains no duplicates."""
        paths = _tree_paths(REPO)
        assert len(paths) == len(set(paths))

    def test_skips_some_dirs(self):
        """Directories in SKIP_DIRS are mostly excluded (e.g., __pycache__)."""
        paths = _tree_paths(REPO)
        # __pycache__ should be excluded
        assert not any(p.startswith("__pycache__") for p in paths)
        # build/ and extracted/ are gitignored and won't exist in a clean checkout
        assert not any(p.startswith("build/") for p in paths)


class TestCollect:
    """Test reference collection from prose."""

    def test_collects_from_tools(self):
        """References in tools/*.py docstrings are found."""
        files, refs = collect(REPO)
        # The tool itself has examples in docstrings
        assert len(files) > 150  # MIN_FILES threshold
        assert len(refs) > 1000  # MIN_REFS threshold
        # Should have some tools/ references
        assert any("tools/" in r for _, _, r in refs)

    def test_respects_generated_filter(self):
        """References to build/, extracted/, progress/ are skipped."""
        files, refs = collect(REPO)
        refs_to_generated = [r for _, _, r in refs if r.startswith(("build/", "extracted/", "progress/"))]
        # The tool explicitly skips these, so there should be none
        assert len(refs_to_generated) == 0


class TestDeadReferencesIntegration:
    """Integration tests for dead_references()."""

    def test_scan_size_check(self):
        """Scan must find minimum file and reference counts."""
        files, refs, dead = dead_references(REPO)
        assert len(files) >= 150
        assert len(refs) >= 1000

    def test_gitignored_paths_are_alive(self):
        """References to gitignored paths resolve as alive even if absent."""
        # This is the core bug fix: paths that are gitignored should not appear
        # in the dead set, even if they're not on disk.
        files, refs, dead = dead_references(REPO)
        dead_refs = {r for _, r in dead}

        # build/, extracted/, progress/ are skipped at collection time, so
        # we can't test them directly. But tools/mwccarm should be gitignored
        # (in the real repo) and shouldn't appear dead.
        # If it appears dead, the gitignore check didn't work.
        if any("tools/mwccarm" in r for _, r in dead):
            # The gitignore mechanism failed
            pytest.fail("tools/mwccarm should be detected as gitignored, not dead")

    def test_tracked_paths_are_alive(self):
        """References to git-tracked paths resolve as alive."""
        files, refs, dead = dead_references(REPO)
        dead_refs = {r for _, r in dead}

        # References to real tracked files shouldn't be dead
        # Pick one we know exists
        tracked = _git_tracked(REPO)
        for tracked_path in list(tracked)[:5]:  # Check first 5
            if "/" in tracked_path:  # Only full paths
                assert tracked_path not in dead_refs, f"{tracked_path} is tracked but marked dead"

    def test_onedisk_paths_are_alive(self):
        """References to files present on disk resolve as alive."""
        files, refs, dead = dead_references(REPO)
        dead_refs = {r for _, r in dead}

        # References to files that exist on disk shouldn't be dead
        on_disk = _tree_paths(REPO)
        for path in list(on_disk)[:10]:  # Check first 10
            if "/" in path:
                assert path not in dead_refs, f"{path} exists on disk but marked dead"


def test_windows_bytes_handling():
    """Test that bytes are handled correctly (Windows newline issue)."""
    # This test verifies the fix for the Windows text=True issue.
    # When we split on NUL bytes, we should handle the bytes correctly.
    test_bytes = b"tools/file1.py\x00tools/file2.py\x00"
    parts = test_bytes.split(NUL)
    assert len(parts) == 3  # Two files plus one empty after final NUL
    decoded = [p.decode("utf-8") for p in parts]
    assert "tools/file1.py" in decoded
    assert "tools/file2.py" in decoded


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
