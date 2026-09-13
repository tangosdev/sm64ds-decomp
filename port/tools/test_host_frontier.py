#!/usr/bin/env python3
"""Unit tests for the batch-attribution parser in port/tools/host_frontier.py.

check_batch runs one `cl /Zs` invocation over many source files and has to
figure out, from the text cl printed, which source each error belongs to.
cl's own error lines carry the path of wherever the error was actually
RAISED -- the source itself for an ordinary syntax error, but a HEADER's
path for an error raised while expanding a header that source pulled in.
Matching an error line's path straight against the batch's file list (the
old approach) makes a source with a header-raised error read as clean: the
error's path names the header, which is not in the batch, so no file ever
gets charged for it. cl also echoes each source's bare file name on its own
line, in argument order, right before it starts compiling that source --
that echo is the per-invocation cursor host_frontier.parse_cl_transcript
now tracks as `current`, and every error line is charged to whichever
source was current when it appeared, with the error's own path kept only
as a `raised_in` detail (None when the error was raised in the source
itself, the header's path otherwise).

Runs with: python -m pytest port/tools/test_host_frontier.py -q
"""
import os
import sys
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import host_frontier
from host_frontier import parse_cl_transcript


def _files(*names):
    """Path objects for a batch, rooted under a fake tree (never touches disk;
    parse_cl_transcript only ever reads .name and dict-keys these objects)."""
    return [Path(f"C:/tmp/l7b/src/{n}") for n in names]


def test_error_in_the_source_itself():
    files = _files("foo.cpp")
    transcript = (
        "foo.cpp\n"
        "c:\\tmp\\l7b\\src\\foo.cpp(10): error C2065: 'bar': undeclared identifier\n"
    )
    results = parse_cl_transcript(transcript, files)
    assert results[files[0]] == ("C2065", None)


def test_header_error_lands_on_second_of_three_sources():
    files = _files("a.cpp", "b.cpp", "c.cpp")
    a, b, c = files
    # b.cpp includes foo.h, which is where the error actually raises; a.cpp
    # and c.cpp compile clean. The old attribution (match the error's own
    # path against the batch) finds no member named foo.h and leaves ALL
    # THREE marked clean -- that is the exact bug this lane fixes.
    transcript = (
        "a.cpp\n"
        "b.cpp\n"
        "c:\\tmp\\l7b\\include\\foo.h(42): error C2065: 'baz': undeclared identifier\n"
        "c.cpp\n"
    )
    results = parse_cl_transcript(transcript, files)
    assert results[a] is None, "a.cpp compiled clean and must stay clean"
    assert results[b] == ("C2065", "c:\\tmp\\l7b\\include\\foo.h"), \
        "the header error must land on b.cpp, the source cl was compiling"
    assert results[c] is None, "c.cpp compiled clean and must stay clean"


def test_same_basename_different_directories():
    # Two sources named util.cpp in different directories. cl's echo line
    # is just the bare name for both, so the ONLY thing that can tell them
    # apart is argument order -- the old code matched by filename suffix
    # and always grabbed the FIRST same-named file, mis-attributing an
    # error in the second one back onto the first.
    first = Path("C:/tmp/l7b/src/foo/util.cpp")
    second = Path("C:/tmp/l7b/src/bar/util.cpp")
    files = [first, second]
    transcript = (
        "util.cpp\n"
        "util.cpp\n"
        "c:\\tmp\\l7b\\src\\bar\\util.cpp(5): error C2059: syntax error\n"
    )
    results = parse_cl_transcript(transcript, files)
    assert results[first] is None, "the FIRST util.cpp must stay clean"
    assert results[second] == ("C2059", None), \
        "the error is in the SECOND util.cpp's own text, not a header"


def test_clean_batch():
    files = _files("clean_a.cpp", "clean_b.cpp", "clean_c.cpp")
    transcript = "clean_a.cpp\nclean_b.cpp\nclean_c.cpp\n"
    results = parse_cl_transcript(transcript, files)
    assert all(v is None for v in results.values())


def test_first_error_per_source_rule_still_holds():
    # A second error against a source that already failed must not
    # overwrite the first -- including when the second one is header-raised
    # and the first was not, or vice versa.
    files = _files("d.cpp")
    transcript = (
        "d.cpp\n"
        "c:\\tmp\\l7b\\src\\d.cpp(3): error C2065: 'x': undeclared identifier\n"
        "c:\\tmp\\l7b\\include\\bar.h(9): error C2143: syntax error\n"
    )
    results = parse_cl_transcript(transcript, files)
    assert results[files[0]] == ("C2065", None)


if __name__ == "__main__":
    import pytest
    sys.exit(pytest.main([__file__, "-q"]))
