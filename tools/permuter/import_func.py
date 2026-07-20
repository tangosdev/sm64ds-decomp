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
import json
import pathlib
import re
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
# Direct compile (no bash): git-bash startup is ~400ms/candidate on Windows; calling
# mwccarm.exe directly is ~140ms (~4x). The permuter's compiler.py reads cc.txt.
MWCC = REPO / "tools" / "mwccarm" / "1.2" / "sp2p3" / "mwccarm.exe"
LICENSE = REPO / "tools" / "mwccarm" / "license.dat"
CFLAGS = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc".split()


def cpp_to_c(src):
    """Strip a `//cpp` draft down to plain C so the C-only permuter parser (pycparser)
    can mutate it: drop the `//cpp` marker, unwrap every `extern "C" { ... }` linkage
    block (keeping its inner content), remove single-declaration `extern "C"` forms, and
    auto-typedef bare struct tags (C++ accepts `Name x;`, C needs `struct Name` or a
    typedef). The mangled names are valid C identifiers.

    Returns None only when `src` is not a `//cpp` draft. Over-broad stripping is SAFE:
    permutable_base() keeps the C form only when it compiles byte-IDENTICALLY to the C++
    original, so a draft that still contains real C++ after stripping (member-fn defs
    `T Class::method(...)`, references, templates, virtuals) either fails to compile as C
    or byte-mismatches, and is rejected -- never a wrong seed. Widening this from the old
    single-wrapper regex unlocked the large class of drafts that are plain C wrapped in
    `extern "C"` purely for name mangling / linkage."""
    if not src.startswith("//cpp"):
        return None
    s = re.sub(r'^//cpp[^\n]*\n', '', src, count=1)
    # Unwrap braced `extern "C" { ... }` blocks (possibly several, with content between),
    # brace-matching each so nested braces in the body are handled.
    out, i = [], 0
    while i < len(s):
        m = re.compile(r'extern\s*"C"\s*\{').match(s, i)
        if not m:
            out.append(s[i]); i += 1; continue
        depth, j = 1, m.end()
        while j < len(s) and depth:
            depth += {"{": 1, "}": -1}.get(s[j], 0)
            j += 1
        if depth == 0:
            out.append(s[m.end():j - 1]); i = j    # keep inner, drop the wrapper braces
        else:
            out.append(s[i]); i += 1               # unbalanced: leave verbatim
    s = "".join(out)
    # Single-declaration `extern "C" int foo();` -> drop just the linkage specifier.
    s = re.sub(r'extern\s*"C"\s+(?=[A-Za-z_])', '', s)
    # Auto-typedef bare struct tags so C accepts `Name x;`.
    seen, pre = set(), []
    for tag in re.findall(r'\bstruct\s+([A-Za-z_]\w*)\s*\{', s):
        if tag not in seen:
            seen.add(tag); pre.append(f"typedef struct {tag} {tag};")
    return ("\n".join(pre) + "\n" + s) if pre else s


def permutable_base(src, name):
    """Return a permuter-parseable base source. For a `//cpp` near-miss, return its
    C-converted form ONLY when it compiles byte-IDENTICALLY (so mutating it is faithful);
    otherwise return the original (the permuter will skip a real-C++ source as before)."""
    if not src.startswith("//cpp"):
        return src
    c = cpp_to_c(src)
    if not c:
        return src
    import match as M, swarm as S

    def _b(s, cpp):
        flags = S.CPP_FLAGS if cpp else M.DEFAULT_FLAGS
        with tempfile.TemporaryDirectory() as td:
            cf = pathlib.Path(td) / ("c.cpp" if cpp else "c.c")
            cf.write_text(s)
            o = M.compile_c(cf, "1.2/sp2p3", flags)
        if o is None:
            return None
        code, _ = M.extract_func(o, name)
        return code

    a, b = _b(src, True), _b(c, False)
    return c if (a is not None and a == b) else src


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


def setup_dir(found, base_src, out=None):
    """Write a permuter working dir for `found` (mod,label,name,addr,size) seeded with
    the C text `base_src`. Returns (out_path, name, addr, size, n_relocs)."""
    mod, label, name, addr, size = found
    data = mod["bin"].read_bytes()
    tgt = data[addr - mod["base"]:addr - mod["base"] + size]

    out = pathlib.Path(out) if out else (PERM_DIR / "work" / name)
    out.mkdir(parents=True, exist_ok=True)

    (out / "compile.sh").write_text(
        f'#!/bin/bash\nexec "{to_posix(WRAPPER)}" "$@"\n')
    (out / "compile.sh").chmod(0o755)
    # Direct-compile sidecar: the permuter's compiler.py uses this to call mwccarm.exe
    # without spawning bash (~4x faster per candidate). Mirrors the wrapper's flags.
    (out / "cc.txt").write_text(json.dumps(
        {"cmd": [to_win(MWCC), *CFLAGS, "-c"], "license": to_win(LICENSE)}))
    (out / "base.c").write_text(base_src)
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
    return out, name, addr, size, len(reloc_offs)


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
    label = found[1]
    base_src = (pathlib.Path(args.base).read_text() if args.base
                else f"// seed: replace with a draft of {found[2]}\nvoid {found[2]}(void) {{}}\n")
    out, name, addr, size, nrel = setup_dir(found, base_src, args.out)
    print(f"imported {label} {name} @ 0x{addr:08x} (size 0x{size:x}, {nrel} relocs) -> {out}")
    print(f"run: python {to_posix(PERM_DIR / 'permuter.py')} "
          f"{to_posix(out)} --stop-on-zero -j 4")


if __name__ == "__main__":
    main()
