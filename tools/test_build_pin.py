"""A per-file verifier must ask the question the ROM build will ask.

`tools/match.py` sweeps 25 installed mwccarm builds and calls a function matched when
any one of them reproduces it. `tools/rombuild.py` compiles each file with exactly one
-- `rombuild.VERSION`, or the `config/rombuild-versions.txt` override for that file. The
tools that REWRITE source and keep the rewrite on a byte check used the first rule while
the build uses the second, so an edit could be verified under a compiler the link never
runs, and the build then rejected what the gate had passed.

Most of this needs neither a compiler nor an extracted ROM: the pin rules, the
fail-closed paths and the source-level guard are all static. The two end-to-end cases
skip themselves when the toolchain is not installed.
"""
import pathlib
import sys

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import build_pin as BP     # noqa: E402
import match as M          # noqa: E402
import rombuild as RB      # noqa: E402

# The tools that rewrite a source file and keep the rewrite when a check passes. These
# are the ones where "any version in the sweep" is a false green rather than a report.
VERIFY_AND_KEEP = ["resolve_placeholders.py", "extern_c_wrap.py", "resolve_blind.py",
                   "reconcile_names.py", "demember_calls.py"]


def _toolchain():
    return (M.MW / BP.DEFAULT_VERSION / "mwccarm.exe").is_file() and M.ARM9.is_file()


# --------------------------------------------------------------------------- pin rules

def test_default_is_the_build_default_not_the_sweep():
    assert BP.DEFAULT_VERSION == RB.VERSION
    assert len(M.SWEEP) > 1, "a one-version sweep would make this whole file vacuous"


def test_version_for_reproduces_rombuild_lookup_for_every_enrolled_file():
    """The property that matters, checked against the real tree rather than a fixture.

    `rombuild.compile_one` picks `vers.get(Path(rel).stem, VERSION)`. Anything else the
    verifier does is a divergence, and a divergence is exactly the bug."""
    vers = RB.versions()
    for rel in RB.enrolled():
        expected = vers.get(pathlib.Path(rel).stem, RB.VERSION)
        assert BP.version_for(rel) == expected, rel


def test_the_overrides_are_actually_exercised():
    """Guards the test above from passing because every file takes the default.

    An EMPTY override file is the goal state, not a failure: the retail ROM was built
    with one toolchain, so every pin is a source defect we have not found yet. What
    must never happen is a pin that is present and does not reach the build -- so the
    resolution check below runs on whatever is there, and only the emptiness is
    tolerated. `pins()` still returns {} here and None for an unreadable file, which
    test_missing_overrides_file_fails_closed pins down."""
    overrides = RB.versions()
    assert BP.pins() is not None, "the override file must be readable even when empty"
    if not overrides:
        return
    assert any(v != RB.VERSION for v in overrides.values())
    for stem, v in overrides.items():
        assert BP.version_for(f"src/{stem}.cpp") == v


def test_missing_overrides_file_fails_closed(monkeypatch=None):
    """An unreadable pin is UNKNOWN, not "the default". Falling back would verify the
    eight override files against a compiler the build will not use for them."""
    saved_file, saved_cache = BP.VERSIONS_FILE, BP._pins
    try:
        BP.VERSIONS_FILE = REPO / "config" / "no-such-versions-file.txt"
        BP._pins = None
        assert BP.pins() is None
        assert BP.version_for("src/func_02062428.c") is None
        v, why = BP.compiler_for("src/func_02062428.c")
        assert v is None and "missing" in why
    finally:
        BP.VERSIONS_FILE, BP._pins = saved_file, saved_cache


def test_override_recorded_under_a_symbol_the_build_cannot_see_fails_closed():
    """The build looks the FILE STEM up. An override keyed on a symbol whose file is
    named something else is an override the build silently ignores, so the pin is
    inconsistent and there is no safe answer -- report it instead of picking a side."""
    saved = BP._pins
    try:
        BP._pins = {"some_other_stem": "1.2/base", "SomeClass__method": "1.2/sp2"}
        assert BP.version_for("src/renamed.cpp", "SomeClass__method") is None
        v, why = BP.compiler_for("src/renamed.cpp", "SomeClass__method")
        assert v is None and "file stem" in why
        # ... and the ordinary case still answers.
        assert BP.version_for("src/some_other_stem.c") == "1.2/base"
    finally:
        BP._pins = saved


def test_uninstalled_pinned_compiler_fails_closed():
    saved = BP._pins
    try:
        BP._pins = {"ghost": "9.9/not-a-build"}
        v, why = BP.compiler_for("src/ghost.c")
        assert v is None and "not installed" in why
    finally:
        BP._pins = saved


# ------------------------------------------------------------------------------ flags

def test_flags_are_the_builds_flags():
    """`match.DEFAULT_FLAGS` is not the build's flag set -- it lacks `-Cpp_exceptions
    off`. Checking with different flags than the link compiles with can bless a version
    the build then breaks on, the same defect by another route."""
    assert BP.flags_for("x.c", "int f(void);") == RB.CFLAGS
    assert BP.flags_for("x.cpp", "//cpp\n") == RB.CFLAGS.replace("-lang c99", "-lang c++")
    # The marker decides, not the extension -- because that is what rombuild does.
    assert BP.flags_for("x.c", "//cpp\n") == RB.CFLAGS.replace("-lang c99", "-lang c++")
    assert "-Cpp_exceptions off" in BP.flags_for("x.c", "")


# ---------------------------------------------------------------- reloc-destination gate

class _StubAudit:
    def __init__(self, rows, exc=None):
        self.rows, self.exc = rows, exc

    def check_destinations(self, *a, **k):
        if self.exc:
            raise self.exc
        return self.rows, 0


def _strict(rows=None, exc=None):
    return (_StubAudit(rows, exc), None, None, None)


def test_bytes_matching_is_not_a_pass_when_a_relocation_points_elsewhere():
    """The observed failure, in miniature. `match.compare` treats every relocated word
    as a wildcard, so a call to the wrong function compares clean; only the link sees
    it. src/func_ov006_020cb030.cpp passed a byte-only check, was enrolled, and the ROM
    build reported ov006 mismatching at a `bl` going to func_ov006_020c9024 where the
    ROM's relocation records 0x020cb134."""
    if not _toolchain():
        return
    rel = "src/func_ov006_020cb030.cpp"
    name, addr, size, label = "func_ov006_020cb030", 0x020cb030, 0x104, "ov006"
    ok, _ = BP.verify(REPO / rel, name, addr, size, label)
    assert ok, "precondition: this file's BYTES reproduce under the pinned compiler"
    bad = [{"verdict": "WRONG-DEST", "off": "+0x58", "cand": "func_ov006_020c9024",
            "cand_addr": "0x020c9024", "cfg": "0x020cb134:ov006"}]
    ok, why = BP.verify(REPO / rel, name, addr, size, label, strict=_strict(bad))
    assert not ok and "reloc destination" in why


def test_a_destination_check_that_could_not_run_is_not_a_pass():
    if not _toolchain():
        return
    rel = "src/func_ov006_020cb030.cpp"
    args = ("func_ov006_020cb030", 0x020cb030, 0x104, "ov006")
    for strict in (_strict(None), _strict(exc=RuntimeError("index unavailable"))):
        ok, why = BP.verify(REPO / rel, *args, strict=strict)
        assert not ok and "could not run" in why


# --------------------------------------------------------------- the pin is load-bearing

def test_the_pinned_version_is_not_interchangeable_with_the_sweep():
    """Every file in config/rombuild-versions.txt is there BECAUSE the build default
    gets it wrong. So for each of them the pin must pass and the default must fail --
    which is precisely the gap a sweep-based check papers over, using committed data
    rather than a bug that someone may later fix."""
    if not _toolchain():
        return
    import rombuild_versions as RV
    info = RV.symbol_info()
    checked = 0
    for stem, pinned in RB.versions().items():
        if pinned == RB.VERSION or stem not in info:
            continue
        label, addr, size = info[stem]
        src = next((REPO / f"src/{stem}{e}" for e in (".c", ".cpp")
                    if (REPO / f"src/{stem}{e}").is_file()), None)
        if src is None or BP.target_bytes(label, addr, size) is None:
            continue
        ok, why = BP.verify(src, stem, addr, size, label)
        assert ok, f"{stem} should reproduce under its pin {pinned}: {why}"
        ok, _ = BP.verify(src, stem, addr, size, label, version=RB.VERSION)
        assert not ok, (f"{stem} reproduces under {RB.VERSION} too, so it does not "
                        f"belong in config/rombuild-versions.txt")
        # And the old rule would have said yes on the strength of the pinned build
        # alone -- that is the false green, restated.
        assert BP.sweep_version(src, stem, addr, size, label) is not None
        checked += 1
    # Not `assert checked`: with no pins left there is nothing to exercise, and that
    # is the outcome this whole file is trying to reach. The guard against a silently
    # vacuous pass lives in test_the_overrides_are_actually_exercised, which fails if
    # a pin is present but unreachable.
    assert checked or not RB.versions(),         "overrides exist but none was exercisable; this test proved nothing"


# ------------------------------------------------------------------- keep it from coming back

def _reads_the_sweep(path):
    """Does this module actually EVALUATE match.SWEEP? Parsed, not grepped -- every one
    of these files now explains the defect in prose, and a substring search cannot tell
    an explanation from a relapse."""
    import ast
    tree = ast.parse(path.read_text(encoding="utf-8"))
    return any(isinstance(n, ast.Attribute) and n.attr == "SWEEP"
               and isinstance(n.value, ast.Name) and n.value.id in ("M", "match")
               for n in ast.walk(tree))


def test_no_verify_and_keep_tool_accepts_a_sweep_again():
    """A source-level guard, because the defect is a shape: a loop over match.SWEEP
    around a compare whose success keeps an edit on disk. Nothing in the runtime
    behaviour of these tools makes that impossible to reintroduce."""
    for tool in VERIFY_AND_KEEP:
        p = TOOLS / tool
        if not p.is_file():
            continue          # demember_calls.py has not landed everywhere yet
        assert not _reads_the_sweep(p), (
            f"{tool} verifies against the sweep again; the ROM build compiles each "
            f"file with one version (see tools/build_pin.py)")
        assert "build_pin" in p.read_text(encoding="utf-8"), \
            f"{tool} should verify through tools/build_pin.py"


def test_build_pin_itself_is_allowed_to_name_the_sweep():
    """Guards the guard: build_pin.sweep_version deliberately implements the old rule so
    the audit can measure the difference. If that disappeared, the test above would go
    green by accident rather than by construction."""
    assert _reads_the_sweep(TOOLS / "build_pin.py")


if __name__ == "__main__":
    fails = 0
    for nm, fn in sorted(globals().items()):
        if nm.startswith("test_") and callable(fn):
            try:
                fn()
                print(f"  PASS  {nm}")
            except AssertionError as e:
                fails += 1
                print(f"  FAIL  {nm}: {e}")
    print(f"\n{fails} failure(s)")
    sys.exit(1 if fails else 0)
