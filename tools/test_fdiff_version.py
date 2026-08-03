"""fdiff must say which compiler it measured with, and let you pick.

It used to compile with match.CANONICAL unconditionally and never report it. Different
mwccarm builds emit different code for the same source, so a function whose cluster
reproduces on 1.2/sp2p3 scored under a canonical of 2004/b56 is measured against the
wrong compiler -- silently, and the number looks perfectly ordinary. Observed on
MeshCollider::DetectClsn(RaycastLine&): the same candidate is 462 words under 2004/b56
and 464 under 1.2/sp2p3.

The version-rejection path runs before any compile or ROM read, so most of this needs
neither a compiler nor an extracted ROM.
"""
import pathlib
import subprocess
import sys

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

import match as M  # noqa: E402


def _run(*args):
    return subprocess.run([sys.executable, str(TOOLS / "fdiff.py"), *args],
                          capture_output=True, text=True)


def test_default_version_is_the_canonical_build():
    """Behaviour is unchanged when --version is omitted."""
    out = _run("--help").stdout
    assert "--version" in out
    assert M.CANONICAL in out, "the help should name the default build"


def test_unknown_version_is_refused_with_the_installed_list():
    """Not silently funnelled into compile_c's generic failure, where it would be
    indistinguishable from a candidate that does not compile."""
    r = _run("--c", __file__, "--name", "x", "--target-hex", "00", "--version", "9.9/nope")
    assert r.returncode != 0
    combined = r.stdout + r.stderr
    assert "no compiler" in combined
    assert "9.9/nope" in combined
    installed = M.installed_versions() if hasattr(M, "installed_versions") else []
    if installed:
        assert installed[0] in combined, "the error should list what IS installed"


def test_path_style_module_spelling_is_still_rejected_upstream():
    """Guard the neighbouring trap: the module ID is `itcm`, not the config path."""
    r = _run("--c", __file__, "--name", "x", "--module", "arm9/itcm",
             "--addr", "0x01ffb0fc", "--size", "0x10")
    assert r.returncode != 0 or "not found" in (r.stdout + r.stderr)


def test_include_dir_is_accepted_and_repeatable():
    out = _run("--help").stdout
    assert "--include-dir" in out


def test_every_result_line_names_the_build():
    """A pasted RESULT must be self-describing -- that is the whole point."""
    src = TOOLS / "fdiff.py"
    text = src.read_text(encoding="utf-8")
    result_lines = [ln for ln in text.splitlines() if "RESULT match=" in ln and "print" in ln]
    assert result_lines, "expected RESULT prints in fdiff.py"
    for ln in result_lines:
        assert "version=" in ln, f"RESULT line without the build: {ln.strip()}"
