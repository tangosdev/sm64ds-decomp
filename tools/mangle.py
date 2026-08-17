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


CPP_FLAGS = M.DEFAULT_FLAGS.replace("-lang c99", "-lang c++")
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
        if binding not in ("STB_GLOBAL", "STB_WEAK"):
            continue
        if kind in ("STT_FILE", "STT_SECTION"):
            continue
        section = elf.get_section(section_index)
        if section is None:
            continue
        found.add(EmittedSymbol(
            section=section.name,
            name=name,
            kind=kind.removeprefix("STT_"),
            binding=binding.removeprefix("STB_"),
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
