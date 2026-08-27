#!/usr/bin/env python3
"""Ask SM64DS's pinned mwccarm compiler which symbols a C++ source emits.

This is a compiler oracle, not a hand-written Itanium mangler.  It compiles the
input with the repository's canonical 2004/b56 C++ flags and reads defined
GLOBAL/WEAK names from the ELF symbol table.  That makes substitutions, thunks,
constructor/destructor variants, and static data the compiler's answer.

Examples:
    python tools/mangle.py scratch.cpp
    python tools/mangle.py scratch.cpp --expect _ZN5Actor8BehaviorEv
    python tools/mangle.py scratch.cpp --mangled-only --json
"""
from __future__ import annotations

import argparse
import contextlib
import io
import json
import pathlib
import sys
from dataclasses import asdict, dataclass

from elftools.elf.elffile import ELFFile

import match as M
from rombuild import CFLAGS as _BUILD_CFLAGS


# The BUILD's flags, not match.DEFAULT_FLAGS, for the reason build_pin.py
# gives at length: an oracle answered with different flags than the link uses
# can bless -- or condemn -- something the build then disagrees with.
#
# The concrete miss: rombuild carries `-Cpp_exceptions off` and DEFAULT_FLAGS
# does not.  With exceptions on, `void operator delete(void *)` is rejected as
# an "exception specification list mismatch" against the implicit throw()
# declaration.  Asked through this tool, CodeWarrior therefore appeared to
# refuse a plain operator delete, and the tree recorded that refusal as a
# compiler restriction (src/game/objects/_Znwj.cpp, include/fBase_c.h) when it is purely
# an artefact of the wrong flag set.  Under the build's own flags it compiles
# and emits _ZdlPv.
CPP_FLAGS = _BUILD_CFLAGS.replace("-lang c99", "-lang c++")
SECTION_ORDER = {".text": 0, ".init": 1, ".data": 2, ".bss": 3}


@dataclass(frozen=True)
class EmittedSymbol:
    section: str
    name: str
    kind: str
    binding: str
    size: int


def _section_rank(name: str) -> tuple[int, str]:
    return SECTION_ORDER.get(name, len(SECTION_ORDER)), name


# mwccarm marks its coalesced RTTI records (_ZTS.../_ZTI...) with a
# processor-specific binding, not STB_GLOBAL/STB_WEAK.  pyelftools renders the
# first of those as 'STB_LOPROC' and leaves the rest as raw integers.  Filtering
# to GLOBAL/WEAK therefore hid every typeinfo symbol the compiler emits, which
# made "does the compiler produce _ZTI4Heap?" unanswerable through this reader.
# They are defined and externally visible, so they belong in the answer.
_EXTERNAL_BINDINGS = {"STB_GLOBAL", "STB_WEAK"}
_PROC_BINDING_RANGE = range(10, 16)      # STB_LOOS .. STB_HIPROC


def _is_external(binding) -> bool:
    if isinstance(binding, int):
        return binding in _PROC_BINDING_RANGE
    if binding in _EXTERNAL_BINDINGS:
        return True
    return binding.startswith(("STB_LOPROC", "STB_HIPROC", "STB_LOOS", "STB_HIOS"))


def defined_symbols(obj: bytes) -> list[EmittedSymbol]:
    """Return externally visible symbols defined by an mwccarm ELF object."""
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return []

    found: set[EmittedSymbol] = set()
    for symbol in symtab.iter_symbols():
        name = symbol.name
        section_index = symbol.entry["st_shndx"]
        binding = symbol.entry["st_info"]["bind"]
        kind = symbol.entry["st_info"]["type"]
        if not name or not isinstance(section_index, int):
            continue
        if not _is_external(binding):
            continue
        if kind in ("STT_FILE", "STT_SECTION"):
            continue
        section = elf.get_section(section_index)
        if section is None:
            continue
        found.add(EmittedSymbol(
            section=section.name,
            name=name,
            kind=str(kind).removeprefix("STT_"),
            binding=str(binding).removeprefix("STB_"),
            size=int(symbol.entry["st_size"]),
        ))

    return sorted(found, key=lambda row: (_section_rank(row.section), row.name))


def visible_symbols(rows: list[EmittedSymbol], mangled_only: bool) -> list[EmittedSymbol]:
    if not mangled_only:
        return rows
    return [row for row in rows if row.name.startswith("_Z")]


def print_text(rows: list[EmittedSymbol], details: bool = False) -> None:
    current_section = None
    for row in rows:
        if row.section != current_section:
            if current_section is not None:
                print()
            current_section = row.section
            print(f"{current_section}:")
        if details:
            print(f"{row.name}\t{row.kind}\t{row.binding}\t0x{row.size:x}")
        else:
            print(row.name)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("file", type=pathlib.Path,
                        help="C/C++ source containing definitions to compile as C++")
    parser.add_argument("--version", default=M.CANONICAL,
                        help=f"mwccarm version (default: {M.CANONICAL})")
    parser.add_argument("--include-dir", action="append", default=[],
                        help="extra include directory before repo include/ (repeatable)")
    parser.add_argument("--expect", action="append", default=[], metavar="SYMBOL",
                        help="require an exact emitted symbol (repeatable)")
    parser.add_argument("--mangled-only", action="store_true",
                        help="display only _Z-prefixed symbols")
    parser.add_argument("--details", action="store_true",
                        help="also show ELF type, binding, and size")
    parser.add_argument("--json", action="store_true",
                        help="write structured output instead of section groups")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if not args.file.is_file():
        print(f"error: source file not found: {args.file}", file=sys.stderr)
        return 1

    compile_log = io.StringIO()
    if args.json:
        with contextlib.redirect_stdout(compile_log):
            obj = M.compile_c(args.file, args.version, CPP_FLAGS, args.include_dir)
    else:
        obj = M.compile_c(args.file, args.version, CPP_FLAGS, args.include_dir)
    if obj is None:
        if args.json:
            print(json.dumps({
                "source": str(args.file),
                "version": args.version,
                "symbols": [],
                "expected": args.expect,
                "missing": args.expect,
                "compiler_output": compile_log.getvalue().splitlines(),
                "error": "compile failed",
            }, indent=2))
        return 1

    emitted = defined_symbols(obj)
    shown = visible_symbols(emitted, args.mangled_only)
    names = {row.name for row in emitted}
    missing = [name for name in args.expect if name not in names]

    if args.json:
        print(json.dumps({
            "source": str(args.file),
            "version": args.version,
            "symbols": [asdict(row) for row in shown],
            "expected": args.expect,
            "missing": missing,
            "compiler_output": compile_log.getvalue().splitlines(),
        }, indent=2))
    elif shown:
        print_text(shown, args.details)
    else:
        qualifier = " mangled" if args.mangled_only else ""
        print(f"no defined GLOBAL/WEAK{qualifier} symbols emitted")

    if missing:
        print("missing expected symbol(s): " + ", ".join(missing), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
