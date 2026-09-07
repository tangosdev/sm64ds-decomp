"""Fail the build on a Python name that does not exist.

WHY THIS EXISTS. `tools/pr_linkcheck.py` shipped in #1367 with

    import asm_policy as AP
    ...
    if w == "NO-REPRO" and asm_policy.has_draft_banner(text):

The module is bound as `AP`, so `asm_policy` is not a name in that scope. `and`
short-circuits, so the line only evaluated once a file graded NO-REPRO -- and the
first PR to contain one got `NameError` in the middle of the classification loop.
The validator reported "worker error: pr_linkcheck exited 1 without a report" and
threw away the verdicts it had already computed. Fixed in #1392.

**A gate's failure branch is the one path a green CI run never exercises.** That is
where this class of defect survives, and it is why the tools that guard this repo
need a guard of their own. Static analysis finds it in under a second; the ROM build
never will, because a tool is not compiled into the ROM.

WHAT IS FATAL, AND WHY NOT EVERYTHING. Only names that cannot resolve at runtime, plus
files that will not parse. Those are bugs by construction and the tree has ZERO of
them today, so this gate starts green and every future red is a real regression.

Pyflakes also reports unused imports, unused locals and placeholder-free f-strings --
32 of them across the tree when this was written. They are style, not breakage, and
gating on them would have meant either a 32-file cleanup commit that buries the
signal, or a gate that lands red and gets switched off within a week. They are
counted and printed here so nobody has to guess the number, and ratcheting them
downward the way `langmode_audit.py` does is a reasonable follow-up -- but it needs a
banked baseline, and this does not.

    python tools/check_python_names.py          # every tracked *.py
    python tools/check_python_names.py a.py b.py
    python tools/check_python_names.py --advisories   # also list the style findings
"""
import argparse
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent

try:
    from pyflakes import api as pyflakes_api, messages as M
except ImportError:
    sys.exit("pyflakes is not installed -- `pip install pyflakes` (CI pins it in "
             ".github/workflows/python-names.yml)")

# A name that will not resolve, or a file that will not parse. Everything pyflakes
# reports outside this set is style.
FATAL = (
    M.UndefinedName,          # the #1367 defect: reading a name nothing binds
    M.UndefinedLocal,         # read before assignment in the same scope
    M.UndefinedExport,        # __all__ naming something that does not exist
    M.InvalidPrintSyntax,     # `print >>x` under py3 -- a runtime TypeError
    M.ForwardAnnotationSyntaxError,
    M.DoctestSyntaxError,
)


class Collector:
    """Reporter that keeps message objects instead of printing them.

    pyflakes' own reporter writes straight to a stream and loses the type, which is
    the only thing that separates a broken name from an unused import.
    """

    def __init__(self):
        self.messages = []
        self.errors = []

    def flake(self, message):
        self.messages.append(message)

    def unexpectedError(self, filename, msg):
        self.errors.append((filename, msg))

    def syntaxError(self, filename, msg, lineno, offset, text):
        self.errors.append((filename, f"line {lineno}: {msg}"))


def tracked_python_files():
    out = subprocess.run(["git", "-C", str(REPO), "ls-files", "*.py"],
                         capture_output=True, text=True, check=True).stdout
    return [REPO / line for line in out.splitlines() if line.strip()]


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*", help="paths to check (default: every tracked *.py)")
    ap.add_argument("--advisories", action="store_true",
                    help="list the style findings as well as counting them")
    args = ap.parse_args()

    explicit = bool(args.files)
    files = [pathlib.Path(f) for f in args.files] if explicit else tracked_python_files()
    collector = Collector()
    for path in files:
        try:
            source = path.read_text(encoding="utf-8", errors="replace")
        except OSError as exc:
            collector.errors.append((str(path), str(exc)))
            continue
        pyflakes_api.check(source, str(path), collector)

    fatal = [m for m in collector.messages if isinstance(m, FATAL)]
    advisory = [m for m in collector.messages if not isinstance(m, FATAL)]

    def rel(p):
        try:
            return pathlib.Path(p).resolve().relative_to(REPO).as_posix()
        except ValueError:
            return str(p)

    scope = "file(s)" if explicit else "tracked file(s)"
    print(f"python-names: {len(files)} {scope} checked")

    if args.advisories and advisory:
        print(f"\nadvisory ({len(advisory)}) -- style, not gated:")
        for m in sorted(advisory, key=lambda m: (m.filename, m.lineno)):
            print(f"  {rel(m.filename)}:{m.lineno}: {m.message % m.message_args}")

    if collector.errors:
        print(f"\nFAIL: {len(collector.errors)} file(s) could not be parsed:")
        for filename, msg in collector.errors:
            print(f"  {rel(filename)}: {msg}")

    if fatal:
        print(f"\nFAIL: {len(fatal)} unresolvable name(s):")
        for m in sorted(fatal, key=lambda m: (m.filename, m.lineno)):
            print(f"  {rel(m.filename)}:{m.lineno}: {m.message % m.message_args}")
        print("\nA name nothing binds is a NameError the moment that line runs, and the\n"
              "branch it sits on may be one no test reaches. Fix it or bind the name.")

    if fatal or collector.errors:
        return 1

    print(f"  0 unresolvable names, 0 unparseable files, {len(advisory)} advisory finding(s)")
    print("python-names: PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
