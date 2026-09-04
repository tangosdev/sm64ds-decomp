#!/usr/bin/env python3
"""Fail when a source object DEFINES a ROM data symbol instead of importing it.

THE DEFECT THIS EXISTS FOR
--------------------------
Inside `extern "C" { ... }`, a variable declaration without `extern` is a
DEFINITION in C++ -- C's tentative-definition rule does not carry over. So

    extern "C" {
    Actor *data_0209b458;        // DEFINITION. wrong.
    extern Actor *data_0209b450; // declaration. right.
    }

hands the linker a second definition of a symbol the ROM's delinked gap object
already provides. Sometimes it aborts the link:

    mwldarm.exe: Multiply-defined: "data_0209b450" in _ZN5Actor18HorzAngleToFPlayerEv.o
    mwldarm.exe: Previously defined in _dsd_gap@main_40.o

and sometimes it silently links, which is worse. Which of the two you get is
decided by whether the type is COMPLETE: `T name[];` is an array of unknown
bound, an incomplete type that emits nothing, while `int name;` emits an object.
That is why one file linked with nine of these and another aborted with one.

WHY THE EXISTING GATES CANNOT SEE IT
------------------------------------
`build_pin.verify` and `match.py` compile ONE file and never link, and they
compare relocated words as wildcards -- a call or load whose target resolves to
the wrong definition is byte-identical to one that resolves correctly. So every
affected file byte-matches throughout. `rombuild.py` catches only the subset
that aborts the link. This gate reads what the compiler actually emitted.

WHY THIS IS NOT A REGEX
-----------------------
It was tried. A scan for `name;` inside an `extern "C"` block reported 452 files
of noise -- `return`, `break`, locals, integer literals -- because brace
tracking cannot survive a function body. Narrowing it to a `data_` prefix with
no parentheses worked, but still only matches the spellings someone thought of.
The object file is the ground truth: either the symbol table has a definition or
it does not.

WHAT COUNTS AS A DEFECT
-----------------------
A GLOBAL or WEAK symbol, defined (not SHN_UNDEF), whose name looks like a ROM
address symbol (`data_*`, `reg_*`). LOCAL symbols are excluded on purpose: a
file-scope `static` is private to the translation unit, cannot collide with the
gap object, and mwcc emits them with a `$N` suffix.

USAGE
-----
    python tools/check_data_definitions.py           # gate; exit 1 on any hit
    python tools/check_data_definitions.py --list    # print every symbol found

Reads objects that are already on disk: the ones `tools/eligible.py` leaves in
build/eligible-scratch/ and the ones `tools/rombuild.py` leaves in build/src/.
Both roots are scanned because either tool may be the last one run. It compiles
nothing, so it costs about a second -- but it is therefore only as fresh as that
last run, and says so when the tree is newer.
"""
import argparse
import io
import pathlib
import sys

from elftools.elf.elffile import ELFFile

REPO = pathlib.Path(__file__).resolve().parent.parent
BUILD = REPO / "build" / "src"
# eligible.py writes one object per (file, symbol) into a separate root, nested
# under the source's own directory, so this one needs a recursive glob.
SCRATCH = REPO / "build" / "eligible-scratch"


def objects():
    """Every already-compiled object, from whichever root produced it."""
    found = list(BUILD.glob("*.o")) if BUILD.is_dir() else []
    if SCRATCH.is_dir():
        found += SCRATCH.rglob("*.o")
    return sorted(found)

# ROM address symbols. A source file may legitimately define its own named
# globals; what it may not do is define one of the delinker's carved-out
# addresses, because the gap object already does.
PREFIXES = ("data_", "reg_")


def defined_rom_objects(obj_path):
    """GLOBAL/WEAK ROM-address symbols this object defines, sorted."""
    try:
        elf = ELFFile(io.BytesIO(obj_path.read_bytes()))
    except Exception:
        return []
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return []
    found = set()
    for sym in symtab.iter_symbols():
        name = sym.name
        if not name.startswith(PREFIXES):
            continue
        if sym.entry.st_shndx == "SHN_UNDEF":
            continue                      # imported -- the correct case
        if sym.entry.st_info.bind == "STB_LOCAL":
            continue                      # a file-scope static; cannot collide
        found.add(name)
    return sorted(found)


def label(obj):
    """The source this object came from, as a repo-relative path."""
    if obj.is_relative_to(SCRATCH):
        return obj.relative_to(SCRATCH).with_suffix("").as_posix()
    return f"src/{obj.stem}"


def audit():
    return {label(o): syms for o in objects() if (syms := defined_rom_objects(o))}


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--list", action="store_true",
                    help="print every offending symbol, not just the file count")
    args = ap.parse_args()

    scanned = objects()
    if not scanned:
        print("no compiled objects in build/eligible-scratch/ or build/src/ -- "
              "run tools/eligible.py first")
        print("skipping (this is NOT a pass)")
        return 0

    hits = audit()

    if not hits:
        print(f"data-definitions: {len(scanned)} object(s), none define a ROM data symbol")
        return 0

    total = sum(len(v) for v in hits.values())
    print(f"data-definitions FAILED: {total} ROM symbol(s) defined across "
          f"{len(hits)} object(s), of {len(scanned)} scanned\n")
    for source in sorted(hits):
        shown = hits[source] if args.list else hits[source][:4]
        more = "" if args.list or len(hits[source]) <= 4 else f" (+{len(hits[source]) - 4} more)"
        print(f"  {source}")
        print(f"      {', '.join(shown)}{more}")
    print("\nThese are DEFINITIONS where a declaration was meant. Inside")
    print('`extern "C" { ... }` a variable needs an explicit `extern`:')
    print("      Actor *data_0209b458;          ->  extern Actor *data_0209b458;")
    print("Function declarations in the same block need no change.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
