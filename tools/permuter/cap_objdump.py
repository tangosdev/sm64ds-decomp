#!/usr/bin/env python3
"""Capstone-based objdump replacement for decomp-permuter, so it needs no external
arm-none-eabi-objdump. Emits `arm-none-eabi-objdump -drz`-style text that the
permuter's simplify_objdump() parses into Line(mnemonic, row, has_symbol).

The permuter only ever compares OUR disassembly of the target against OUR disassembly
of each candidate, so we just need to be internally consistent (and parseable), not
byte-identical to GNU objdump's formatting.

Input: a path to an ELF .o (mwccarm output) OR a raw .bin of target bytes.
  - ELF:  disassemble the .text section; emit R_ARM_* reloc lines from .rel.text so
          the permuter wildcards relocated operands (bl targets, pool data refs).
  - raw:  disassemble the whole file as ARM code (used for a raw-bytes target; reloc
          offsets, if any, can be supplied via CAP_OBJDUMP_RELOCS=off1,off2,...).

Usage (permuter calls it via objdump_command):
    python cap_objdump.py <file>
"""
import os
import sys

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_LITTLE_ENDIAN

_md = Cs(CS_ARCH_ARM, CS_MODE_ARM | CS_MODE_LITTLE_ENDIAN)


def _elf_text_and_relocs(data):
    """Return (text_bytes, {offset: reloc_type}) for an ELF .o using pyelftools."""
    from io import BytesIO
    from elftools.elf.elffile import ELFFile

    elf = ELFFile(BytesIO(data))
    text = elf.get_section_by_name(".text")
    text_bytes = text.data()
    relocs = {}
    rel = elf.get_section_by_name(".rel.text") or elf.get_section_by_name(".rela.text")
    if rel is not None:
        try:
            for r in rel.iter_relocations():
                relocs[r["r_offset"]] = r["r_info_type"]
        except Exception:
            pass
    return text_bytes, relocs


# ARM ELF reloc type numbers -> objdump reloc strings the permuter understands.
_ARM_RELOC = {28: "R_ARM_CALL", 2: "R_ARM_ABS32", 10: "R_ARM_THM_CALL"}


def main():
    path = sys.argv[1]
    with open(path, "rb") as f:
        data = f.read()

    if data[:4] == b"\x7fELF":
        code, relocs = _elf_text_and_relocs(data)
    else:
        code = data
        relocs = {}
        # reloc offsets for a raw-bytes target: from a "<path>.relocs" sidecar
        # (one "offset[,type]" per line) and/or the CAP_OBJDUMP_RELOCS env var.
        sidecar = path + ".relocs"
        if os.path.exists(sidecar):
            for line in open(sidecar):
                line = line.split("#", 1)[0].strip()
                if not line:
                    continue
                parts = line.split(",")
                off = int(parts[0], 0)
                relocs[off] = int(parts[1], 0) if len(parts) > 1 else 28
        env = os.environ.get("CAP_OBJDUMP_RELOCS", "")
        for tok in filter(None, env.split(",")):
            relocs[int(tok, 0)] = 28  # treat as R_ARM_CALL by default

    # Header line (index 0 is skipped by simplify_objdump's skip_lines=1).
    out = ["cap_objdump: file format elf32-littlearm", "", "00000000 <func>:"]
    for ins in _md.disasm(code, 0):
        # Format: "<addr>:\t<rawhex>\t<mnemonic>\t<args>"  (addr+rawhex are discarded).
        if ins.address in relocs:
            # Reloc slot: emit a CANONICAL placeholder so the target's real value and
            # the candidate's reloc-zero compare equal (wildcarded, like our oracle).
            # Covers bl call targets and pc-relative pool-address words alike.
            out.append(f"{ins.address:8x}:\t00000000\treloc\t")
        else:
            out.append(f"{ins.address:8x}:\t{ins.bytes.hex()}\t{ins.mnemonic}\t{ins.op_str}")

    sys.stdout.write("\n".join(out) + "\n")


if __name__ == "__main__":
    main()
