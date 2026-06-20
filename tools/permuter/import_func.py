#!/usr/bin/env python3
"""Set up a decomp-permuter working directory for one unmatched function.

Given a function (by module+addr or name), this resolves its ROM bytes, size, and the
reloc offsets inside it, then writes a permuter dir:

    <out>/target.o          raw ROM bytes of the function (cap_objdump reads raw)
    <out>/target.o.relocs   reloc offsets to wildcard (bl targets, pool-addr words)
    <out>/compile.sh        -> tools/permuter/mwccarm_compile.sh (our canonical build)
    <out>/base.c            the seed C (an LLM/m2c draft, or a near-miss to polish)
    <out>/settings.toml     compiler_type=mwcc, func_name, objdump_command=cap_objdump

Then run:
    python vendor/decomp-permuter/permuter.py <out> --stop-on-zero -j 4

Usage:
    python tools/permuter/import_func.py --module ov002 --addr 0x020570c0 --base seed.c
    python tools/permuter/import_func.py --name func_020570c0 --base seed.c

A score of 0 means the permuter found C that compiles byte-identical to the ROM; verify
and bank it with the normal oracle (tools/match.py / swarm.oracle_ok) before committing.
"""
import argparse
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(REPO / "tools"))
import io
import subprocess
import tempfile

import modules as MOD
import relocs as R
import sweep

PERM_DIR = REPO / "vendor" / "decomp-permuter"
WRAPPER = REPO / "tools" / "permuter" / "mwccarm_compile.sh"
CAP = REPO / "tools" / "permuter" / "cap_objdump.py"


def candidate_reloc_offsets(base_c_path):
    """Compile the seed and read its .rel.text offsets -- the authoritative set of
    reloc slots to wildcard (config relocs.txt omits data-pool relocs; the compiled
    object is the source of truth, same as our oracle uses)."""
    from elftools.elf.elffile import ELFFile
    with tempfile.NamedTemporaryFile(suffix=".o", delete=False) as f:
        obj = f.name
    try:
        subprocess.check_call(["bash", str(WRAPPER), str(base_c_path), "-o", obj],
                              stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        with open(obj, "rb") as f:
            elf = ELFFile(io.BytesIO(f.read()))
            rel = elf.get_section_by_name(".rel.text") or elf.get_section_by_name(".rela.text")
            return sorted(r["r_offset"] for r in rel.iter_relocations()) if rel else []
    except Exception:
        return None
    finally:
        try:
            __import__("os").remove(obj)
        except OSError:
            pass


def find_func(module, addr, name):
    """Return (mod, label, name, addr, size) for the requested function."""
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        if module and mod["name"] != module and label != module:
            continue
        for fname, faddr, fsize in sweep.funcs(mod):
            if (addr is not None and faddr == addr) or (name and fname == name):
                return mod, label, fname, faddr, fsize
    return None


def to_posix(p):
    """Absolute path in /c/... form, for the bash-run compile.sh."""
    s = str(pathlib.Path(p).resolve())
    if len(s) > 1 and s[1] == ":":
        s = "/" + s[0].lower() + s[2:].replace("\\", "/")
    return s


def to_win(p):
    """Absolute path in C:/... form, for objdump_command (run by native Windows
    Python via the permuter's subprocess, which can't open /c/... posix paths)."""
    return str(pathlib.Path(p).resolve()).replace("\\", "/")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default=None)
    ap.add_argument("--addr", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--name", default=None)
    ap.add_argument("--base", default=None, help="seed C file (the draft to permute)")
    ap.add_argument("--out", default=None, help="output dir (default vendor/.../work/<name>)")
    args = ap.parse_args()
    if args.addr is None and not args.name:
        ap.error("give --addr (with --module) or --name")

    found = find_func(args.module, args.addr, args.name)
    if not found:
        print("function not found", file=sys.stderr)
        sys.exit(1)
    mod, label, name, addr, size = found

    data = mod["bin"].read_bytes()
    tgt = data[addr - mod["base"]:addr - mod["base"] + size]

    out = pathlib.Path(args.out) if args.out else (PERM_DIR / "work" / name)
    out.mkdir(parents=True, exist_ok=True)

    (out / "compile.sh").write_text(
        f'#!/bin/bash\nexec "{to_posix(WRAPPER)}" "$@"\n')
    (out / "compile.sh").chmod(0o755)

    if args.base:
        (out / "base.c").write_text(pathlib.Path(args.base).read_text())
    elif not (out / "base.c").exists():
        (out / "base.c").write_text(
            f"// seed: replace with a logically-correct draft of {name}\n"
            f"void {name}(void) {{}}\n")

    (out / "target.o").write_bytes(tgt)
    # Reloc offsets to wildcard: prefer the seed's own .rel.text (authoritative, incl.
    # data-pool relocs), fall back to config relocs.txt if the seed won't compile.
    reloc_offs = candidate_reloc_offsets(out / "base.c")
    if reloc_offs is None:
        relocs = R.load_relocs_file(mod["relocs"])
        reloc_offs = sorted(o - addr for o in relocs if addr <= o < addr + size)
    (out / "target.o.relocs").write_text("".join(f"0x{o:x}\n" for o in reloc_offs))

    (out / "settings.toml").write_text(
        f'compiler_type = "mwcc"\n'
        f'func_name = "{name}"\n'
        f'objdump_command = "python {to_win(CAP)}"\n')

    print(f"imported {label} {name} @ 0x{addr:08x} (size 0x{size:x}, "
          f"{len(reloc_offs)} relocs) -> {out}")
    print(f"run: python {to_posix(PERM_DIR / 'permuter.py')} "
          f"{to_posix(out)} --stop-on-zero -j 4")


if __name__ == "__main__":
    main()
