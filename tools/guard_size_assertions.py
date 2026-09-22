"""Ask only the ROM's compiler to meet the ROM's layout assertions.

414 headers under include/ carry a size assertion of the shape

    typedef char Player_size_must_be_0x900[sizeof(Player) == 0x900 ? 1 : -1];

It is a statement about what mwccarm produces. mwccarm implements the Itanium
C++ ABI, so it reuses a base class's TAIL PADDING for a derived class's first
member and skips a vfptr for a base that declares no virtual of its own. MSVC
does neither, so the same declaration is a different size there and the typedef
takes a negative subscript. include/dPa_c.h names the mechanism in its own field
comment: `s16 scale; /* 0x006 - reuses base tail padding */`.

Measured by lane SYNC3 out of a compiled probe, not reasoned about:

    dPa_c::level_c::scaleCallback_c   header 0xc     MSVC 0x10
    Particle::SysTracker              header 0x81c   MSVC 0x82c

That is 748 `error C2118: negative subscript` rows in the port's first full
build after the merge, and the FIRST error in 485 of its 585 failing sources.
No MSVC switch expresses the difference. `#pragma pack(2)` makes the base six
bytes, so the base's own assertion fails instead.

THIS DOES NOT WEAKEN THE CHECK. Under mwccarm every assertion still compiles and
still catches what it is for. The only change is that a compiler which cannot
reproduce the ROM's layout is no longer asked to prove that it does. Where the
host object really is a different shape from the ROM's, that is a divergence the
port tracks on its own side; an assertion that stops the host build cannot record
it, it can only hide the rest of the build behind it.

WHAT IT DOES. Every assertion statement that is not already inside a conditional
that settles SM64DS_PLATFORM_PC gets wrapped:

    #ifndef SM64DS_PLATFORM_PC
    /* ROM layout under mwccarm; host ABI divergence is tracked separately. */
    typedef char Player_size_must_be_0x900[sizeof(Player) == 0x900 ? 1 : -1];
    #endif

Nothing else in the file changes: not the statement's own text, not its
indentation, not a blank line, not the file's line endings. A run of assertions
on adjacent lines shares one guard rather than stacking identical ones.

SM64DS_PLATFORM_PC is the guard because it is the marker this tree already uses
(include/ArrowSignRight.h:30, src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp:13),
every shipping port target defines it, and the matching build never does.

IT IS IDEMPOTENT, and that falls out of the rule rather than being bolted on: an
assertion the tool has already wrapped is inside a conditional that settles
SM64DS_PLATFORM_PC, which is exactly the case it skips. Running it twice on the
same tree changes nothing, and running it on the base commit reproduces the
committed result byte for byte.

    python tools/guard_size_assertions.py            # report, write nothing
    python tools/guard_size_assertions.py --write    # rewrite the headers
    python tools/guard_size_assertions.py --check    # exit 1 if any is unguarded
    python tools/guard_size_assertions.py --root DIR # a tree other than this one
"""
import argparse
import pathlib
import re
import sys

GUARD = "SM64DS_PLATFORM_PC"
GUARD_OPEN = "#ifndef " + GUARD
GUARD_NOTE = "/* ROM layout under mwccarm; host ABI divergence is tracked separately. */"
GUARD_CLOSE = "#endif"

HEADER_SUFFIXES = (".h", ".hpp")

# `typedef char Player_size_must_be_0x900[` opens the statement; it runs to the
# first `];`, which is on the same line for most and on the next one or two for
# the rest. Both spellings are in the tree and neither is normalised here.
ASSERT_OPEN = re.compile(r"^\s*typedef\s+char\s+\w*size_must_be\w*\s*\[")
ASSERT_CLOSE = "];"

COND = re.compile(r"^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)$")

REQUIRES, EXCLUDES, NEUTRAL = "requires", "excludes", "neutral"


def condition_polarity(kind, cond):
    """What a live `#if`-family condition settles about SM64DS_PLATFORM_PC.

    REQUIRES: the branch compiling implies the macro is defined.
    EXCLUDES: the branch compiling implies it is not.
    NEUTRAL:  the branch says nothing either way, which includes every condition
              that only mentions the macro under an `||`. `#if defined(A) ||
              defined(GUARD)` being true does NOT mean GUARD is defined, so
              claiming otherwise would let the tool delete a live check.
    """
    text = cond.strip()
    if kind == "ifdef":
        return REQUIRES if text == GUARD else NEUTRAL
    if kind == "ifndef":
        return EXCLUDES if text == GUARD else NEUTRAL
    if kind not in ("if", "elif"):
        return NEUTRAL
    if GUARD not in text:
        return NEUTRAL
    if "||" in text:
        return NEUTRAL
    if re.search(r"!\s*defined\s*\(\s*" + GUARD + r"\s*\)", text):
        return EXCLUDES
    if re.search(r"!\s*" + GUARD + r"\b", text):
        return EXCLUDES
    if re.search(r"\bdefined\s*\(\s*" + GUARD + r"\s*\)", text):
        return REQUIRES
    if re.search(r"\bdefined\s+" + GUARD + r"\b", text):
        return REQUIRES
    return NEUTRAL


def flip(polarity):
    if polarity == REQUIRES:
        return EXCLUDES
    if polarity == EXCLUDES:
        return REQUIRES
    return NEUTRAL


class Frame(object):
    """One `#if`...`#endif` nesting level, and which of its branches is live."""

    def __init__(self, kind, cond):
        self.opening = condition_polarity(kind, cond)
        self.live = self.opening

    def take_elif(self, cond):
        # Reaching an `#elif` means every earlier branch was false. Only the
        # opening branch is read for polarity; an earlier `#elif` that settled
        # the macro would make this one unreachable anyway, and reading it would
        # need real expression evaluation rather than this.
        self.live = condition_polarity("elif", cond)
        if self.live == NEUTRAL:
            self.live = flip(self.opening)

    def take_else(self):
        # The `#else` of a conjunction is read as the opposite of the opening.
        # That is not strict logic: `#else` of `#if defined(GUARD) &&
        # defined(_MSC_VER)` also compiles on a host that defines GUARD without
        # _MSC_VER. It is the reading the tree wants, because such a branch is
        # the ROM-shaped declaration and every Itanium-ABI host satisfies it, and
        # the counts main() prints make any future case of it visible rather than
        # silent: a run reported as host-only is one this tool did not guard.
        self.live = flip(self.opening)


def guard_state(stack):
    """What the whole live nesting settles about the macro at this point."""
    seen = set(frame.live for frame in stack)
    if EXCLUDES in seen:
        return EXCLUDES          # contradictory nesting is dead code either way
    if REQUIRES in seen:
        return REQUIRES
    return NEUTRAL


def find_runs(lines):
    """Every assertion statement in one header, grouped into adjacent runs.

    Returns a list of (start, end, state) with `lines[start:end]` covering whole
    statements only, and `state` the guard_state where they sit. Two statements
    join a run only when they are on adjacent lines AND in the same conditional
    branch, so a run is always safe to bracket with a single pair of directives.
    """
    runs = []
    stack = []
    i = 0
    while i < len(lines):
        line = lines[i].rstrip("\r")
        directive = COND.match(line)
        if directive:
            kind, rest = directive.group(1), directive.group(2)
            if kind in ("if", "ifdef", "ifndef"):
                stack.append(Frame(kind, rest))
            elif kind == "elif":
                if stack:
                    stack[-1].take_elif(rest)
            elif kind == "else":
                if stack:
                    stack[-1].take_else()
            elif kind == "endif":
                if stack:
                    stack.pop()
            i += 1
            continue
        if not ASSERT_OPEN.match(line):
            i += 1
            continue
        end = i
        while end < len(lines) and ASSERT_CLOSE not in lines[end]:
            end += 1
        if end >= len(lines):
            # An unterminated statement is a file this tool does not understand.
            raise ValueError("unterminated size assertion at line %d" % (i + 1))
        end += 1
        state = guard_state(stack)
        if runs and runs[-1][1] == i and runs[-1][2] == state:
            runs[-1] = (runs[-1][0], end, state)
        else:
            runs.append((i, end, state))
        i = end
    return runs


def rewrite(text):
    """The header's new text, and how many statements were wrapped."""
    lines = text.split("\n")
    eol = "\r" if "\r\n" in text else ""
    runs = [r for r in find_runs(lines) if r[2] == NEUTRAL]
    if not runs:
        return text, 0
    out = []
    wrapped = 0
    cursor = 0
    for start, end, _state in runs:
        out.extend(lines[cursor:start])
        out.append(GUARD_OPEN + eol)
        out.append(GUARD_NOTE + eol)
        out.extend(lines[start:end])
        out.append(GUARD_CLOSE + eol)
        wrapped += sum(1 for ln in lines[start:end] if ASSERT_OPEN.match(ln.rstrip("\r")))
        cursor = end
    out.extend(lines[cursor:])
    return "\n".join(out), wrapped


def read(path):
    """Bytes in, text out, with the line endings left alone.

    Text mode would translate this tree's CRLF to LF on the way in and hand the
    tool a file that differs from the one on disk in every line, so the rewrite
    would land a whole-file ending change under a one-line diff.
    """
    return path.read_bytes().decode("utf-8", "surrogateescape")


def write(path, text):
    path.write_bytes(text.encode("utf-8", "surrogateescape"))


def headers(root):
    base = pathlib.Path(root) / "include"
    return sorted(p for p in base.rglob("*")
                  if p.is_file() and p.suffix in HEADER_SUFFIXES)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--root", default=str(pathlib.Path(__file__).resolve().parent.parent),
                    help="repository root (default: this tool's own)")
    ap.add_argument("--write", action="store_true", help="rewrite the headers in place")
    ap.add_argument("--check", action="store_true",
                    help="exit 1 if any assertion is still unguarded")
    args = ap.parse_args(argv)

    files = headers(args.root)
    if not files:
        print("no headers under %s/include" % args.root)
        return 1

    touched = []
    total = 0
    already = 0
    host_only = 0
    for path in files:
        text = read(path)
        if "size_must_be" not in text:
            continue
        # Name the file. Over 500 headers a bare "unterminated size assertion at
        # line 61" is a traceback nobody can act on, and this is the one branch a
        # green run never exercises.
        try:
            states = [r[2] for r in find_runs(text.split("\n"))]
            new, wrapped = rewrite(text)
        except ValueError as exc:
            print("%s: %s" % (path.as_posix(), exc))
            return 1
        already += states.count(EXCLUDES)
        host_only += states.count(REQUIRES)
        if wrapped:
            total += wrapped
            touched.append((path, wrapped))
            if args.write:
                write(path, new)

    verb = "wrapped" if args.write else "would wrap"
    print("%s %d assertion(s) in %d header(s)" % (verb, total, len(touched)))
    print("%d run(s) already inside a conditional that excludes %s" % (already, GUARD))
    print("%d run(s) inside a host-only conditional, left asserting" % host_only)
    if not args.write:
        for path, n in touched[:10]:
            print("   %s  %d" % (path.as_posix(), n))
        if len(touched) > 10:
            print("   ... %d more" % (len(touched) - 10))
    if args.check and total:
        print("FAIL: run `python tools/guard_size_assertions.py --write`")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
