"""Differential compiler probe: find C constructs whose codegen differs across
mwccarm versions, to pin the exact one the ROM was built with.

Compiles one C file with each candidate version and compares every function's
bytes ACROSS versions (objects are unlinked, so reloc slots are zero in all and
a raw compare is valid). Reports which functions discriminate the versions.

Usage:
    python tools/probe_versions.py --c tools/probes/discriminate.c
    python tools/probe_versions.py --c <file> --versions 1.2/base,1.2/sp2,1.2/sp2p3
"""
import argparse
import io
import pathlib

from elftools.elf.elffile import ELFFile

from match import compile_c, DEFAULT_FLAGS, PINNED


def funcs_in(obj: bytes) -> dict:
    """name -> code bytes for every defined function symbol in the object."""
    elf = ELFFile(io.BytesIO(obj))
    st = elf.get_section_by_name(".symtab")
    out = {}
    for s in st.iter_symbols():
        if (s["st_info"]["type"] == "STT_FUNC" and s["st_size"] > 0
                and s["st_shndx"] not in ("SHN_UNDEF", "SHN_ABS")):
            sec = elf.get_section(s["st_shndx"])
            out[s.name] = sec.data()[s["st_value"]:s["st_value"] + s["st_size"]]
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--c", required=True)
    ap.add_argument("--versions", default=",".join(PINNED))
    ap.add_argument("--flags", default=DEFAULT_FLAGS)
    args = ap.parse_args()

    versions = [v.strip() for v in args.versions.split(",") if v.strip()]
    cfile = pathlib.Path(args.c)

    per_version = {}
    for v in versions:
        obj = compile_c(cfile, v, args.flags)
        if obj is None:
            print(f"  ! compile failed for {v}")
            continue
        per_version[v] = funcs_in(obj)

    if len(per_version) < 2:
        print("need >=2 versions compiling to compare")
        return

    names = sorted(set().union(*[set(d) for d in per_version.values()]))
    discriminating = []
    for name in names:
        variants = {}
        for v, d in per_version.items():
            variants.setdefault(d.get(name, b"").hex(), []).append(v)
        if len(variants) > 1:
            discriminating.append((name, variants))

    vs = list(per_version.keys())
    print(f"versions compared: {', '.join(vs)}")
    print(f"functions: {len(names)}   DISCRIMINATING: {len(discriminating)}\n")
    if not discriminating:
        print("=> all versions produced byte-identical code for every probe function.")
        print("   These service packs are codegen-equivalent here; any is safe as canonical.")
        return
    for name, variants in discriminating:
        print(f"### {name} — splits the versions:")
        for hexb, vlist in variants.items():
            print(f"   [{', '.join(vlist)}]  len {len(hexb)//2}  {hexb}")
        print()


if __name__ == "__main__":
    main()
