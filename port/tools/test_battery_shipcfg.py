#!/usr/bin/env python3
"""Unit tests for the shipcfg fast-path decision (port/tools/battery.py).

These test the DECISION FUNCTION directly -- shipcfg_configure_decision() and
the table it and shipcfg_cache_defines() share -- against tiny fixture
directories that carry only a build.ninja and/or a CMakeCache.txt. No cmake is
invoked and nothing is built or launched, so these run anywhere Python does.
Run with: pytest port/tools/test_battery_shipcfg.py

THE REGRESSION THIS FILE PINS (Andrew's #2474 round-3 P2). Before run link100
lane SHIPCACHE, the shipcfg arm's fast path skipped the configure step
whenever build/port-kit/build.ninja was merely PRESENT, with nothing checking
that the cache it would then build from still carried PORT_ROM_CLEAN=ON,
CMAKE_BUILD_TYPE=Release and the static CRT. A build/port-kit left over from
any other invocation -- Andrew's own repro was PORT_ROM_CLEAN=OFF,
CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL, CMAKE_BUILD_TYPE=Debug sitting
behind a real build.ninja -- read "already configured" and shipped a binary
built from the wrong settings while the battery printed green.
test_wrong_cache_is_refused below is that exact fixture.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import battery

# Runnable with pytest (the repo convention for tools/test_*.py) OR, where
# pytest is not installed, as a plain script -- `python
# port/tools/test_battery_shipcfg.py` -- via the tiny fixture/assert shim at
# the bottom of this file, the same shape test_slot_lock.py and
# test_build_lock.py use. Both paths run the identical test bodies.
try:
    import pytest
    _HAVE_PYTEST = True
except ImportError:
    _HAVE_PYTEST = False

    class _Shim:
        """A pytest.fixture stand-in for the no-pytest runner."""
        @staticmethod
        def fixture(fn):
            fn._is_fixture = True
            return fn

    pytest = _Shim()


@pytest.fixture
def build(tmp_path):
    """A private build/port-kit stand-in, never the real machine-wide one."""
    return str(tmp_path / "port-kit")


def _write_ninja(build):
    os.makedirs(build, exist_ok=True)
    with open(os.path.join(build, "build.ninja"), "w", encoding="utf-8") as f:
        f.write("# fixture build.ninja, never read by the decision function\n")


def _write_cache(build, overrides=None, generator=battery.SHIPCFG_GENERATOR,
                 omit=()):
    """A CMakeCache.txt fixture. Starts from SHIPCFG_CACHE_TABLE (the real
    table, not a hand-copied guess of it, so these tests track the table
    battery.py actually enforces rather than a second opinion of it),
    applies `overrides` on top and drops any key named in `omit`."""
    os.makedirs(build, exist_ok=True)
    entries = dict(battery.SHIPCFG_CACHE_TABLE)
    if overrides:
        entries.update(overrides)
    for k in omit:
        entries.pop(k, None)
    lines = ["# This is a CMakeCache.txt file, fixture only.\n"]
    if generator is not None:
        lines.append("CMAKE_GENERATOR:INTERNAL=%s\n" % generator)
    for k, v in entries.items():
        # CMake's own type spelling is not uniform (battery.py's
        # _cmake_cache_entries docstring notes CMAKE_MSVC_RUNTIME_LIBRARY
        # reads UNINITIALIZED, not STRING, on a real cache) so the fixture
        # deliberately does not use one consistent type tag either.
        lines.append("%s:STRING=%s\n" % (k, v))
    # A comment line and a blank line, so the parser's skip rules are
    # exercised on every fixture rather than only on a dedicated test.
    lines.append("\n// a cmake-style comment line\n")
    with open(os.path.join(build, "CMakeCache.txt"), "w",
             encoding="utf-8") as f:
        f.writelines(lines)


# --- shipcfg_configure_decision(): the three fixtures the brief names -----

def test_correct_cache_is_fast_path(build):
    _write_ninja(build)
    _write_cache(build)
    status, detail = battery.shipcfg_configure_decision(build)
    assert status == "fast path"
    assert "CMakeCache.txt" in detail


def test_wrong_cache_is_refused(build):
    # Andrew's exact repro: PORT_ROM_CLEAN=OFF, MultiThreadedDLL, Debug,
    # behind a real build.ninja.
    _write_ninja(build)
    _write_cache(build, overrides={
        "PORT_ROM_CLEAN": "OFF",
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_MSVC_RUNTIME_LIBRARY": "MultiThreadedDLL",
    })
    status, detail = battery.shipcfg_configure_decision(build)
    assert status == "refused"
    assert "PORT_ROM_CLEAN" in detail
    assert "OFF" in detail


def test_missing_cache_is_refused(build):
    # build.ninja is there (a configure happened once); CMakeCache.txt is not
    # (deleted, or never copied alongside it).
    _write_ninja(build)
    status, detail = battery.shipcfg_configure_decision(build)
    assert status == "refused"
    assert "CMakeCache.txt" in detail


# --- every other way the cache can fail to prove the enforced settings ----

def test_missing_ninja_is_refused_even_with_a_good_cache(build):
    # A cache alone, no build.ninja: nothing is configured yet, so this is
    # the ordinary from-scratch case and must configure, not skip.
    _write_cache(build)
    status, detail = battery.shipcfg_configure_decision(build)
    assert status == "refused"
    assert "build.ninja" in detail


def test_wrong_generator_is_refused(build):
    _write_ninja(build)
    _write_cache(build, generator="NMake Makefiles")
    status, detail = battery.shipcfg_configure_decision(build)
    assert status == "refused"
    assert "CMAKE_GENERATOR" in detail
    assert "NMake Makefiles" in detail


def test_missing_generator_key_is_refused(build):
    _write_ninja(build)
    _write_cache(build, generator=None)
    status, detail = battery.shipcfg_configure_decision(build)
    assert status == "refused"
    assert "CMAKE_GENERATOR" in detail


def test_one_missing_key_is_refused_and_named(build):
    # The cache exists and is otherwise correct but never got a
    # PORT_ROM_CLEAN entry at all -- a missing key, not a wrong value.
    _write_ninja(build)
    _write_cache(build, omit=("PORT_ROM_CLEAN",))
    status, detail = battery.shipcfg_configure_decision(build)
    assert status == "refused"
    assert "PORT_ROM_CLEAN" in detail
    assert "None" in detail  # cache.get() found nothing for the key


def test_each_table_key_wrong_alone_is_refused_and_named(build):
    # Every enforced key, one at a time, so a future entry added to the table
    # is proven to gate the fast path by this loop rather than by one row
    # somebody remembered to add.
    for key, want in battery.SHIPCFG_CACHE_TABLE:
        _write_ninja(build)
        _write_cache(build, overrides={key: "WRONG_" + want})
        status, detail = battery.shipcfg_configure_decision(build)
        assert status == "refused", key
        assert key in detail, key


# --- the ONE table (Andrew: "derive both from ONE table in the code") -----

def test_cache_defines_and_decision_share_the_same_table():
    # shipcfg_cache_defines() is what the configure COMMAND gets;
    # shipcfg_configure_decision() is what the CHECK reads. Both must come
    # from SHIPCFG_CACHE_TABLE and neither may have a value the other lacks,
    # or the command and the check could drift apart again the way the P2
    # found them implicitly able to.
    defines = battery.shipcfg_cache_defines()
    for key, value in battery.SHIPCFG_CACHE_TABLE:
        assert "-D%s=%s" % (key, value) in defines


def test_cache_entries_parses_type_tag_and_skips_comments(build):
    _write_ninja(build)
    _write_cache(build)
    entries = battery._cmake_cache_entries(
        os.path.join(build, "CMakeCache.txt"))
    for key, value in battery.SHIPCFG_CACHE_TABLE:
        assert entries[key] == value
    assert entries["CMAKE_GENERATOR"] == battery.SHIPCFG_GENERATOR


# --- no-pytest standalone runner ------------------------------------------
# shipcfg_script() itself is not exercised here: it needs a real vcvars32.bat
# to get past its own toolchain-missing check before it ever calls
# shipcfg_configure_decision, and finding one is exactly the "no CMake in the
# unit test" real-configure territory the brief keeps this file out of. The
# decision function above is what shipcfg_script bakes into the generated
# .cmd unconditional-vs-guarded, and it is what these tests pin; the
# generated script itself is what the SHIPCACHE report's two pasted battery
# runs prove end to end, once on a good cache and once on the corrupted one.

def _standalone():
    import inspect
    import pathlib
    import tempfile
    import traceback

    tests = [(n, o) for n, o in sorted(globals().items())
             if n.startswith("test_") and callable(o)]
    passed = failed = 0
    for name, fn in tests:
        with tempfile.TemporaryDirectory() as td:
            params = inspect.signature(fn).parameters
            kwargs = {}
            if "build" in params:
                kwargs["build"] = str(pathlib.Path(td) / "port-kit")
            try:
                fn(**kwargs)
                print(f"  PASS {name}")
                passed += 1
            except Exception:
                print(f"  FAIL {name}")
                traceback.print_exc()
                failed += 1
    print(f"\n{passed} passed, {failed} failed")
    return 1 if failed else 0


if __name__ == "__main__":
    if _HAVE_PYTEST:
        sys.exit(pytest.main([__file__, "-q"]))
    print("pytest not installed -- running the standalone decision-logic "
          "harness:\n")
    sys.exit(_standalone())
