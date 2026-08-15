"""Generate mwccarm C for __sinit_* from classified ARM disasm (pool-skip safe)."""
import argparse
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import m2c_draft as M2
import modules as MOD
import nearmiss_db as NM
import relocs as R
import swarm as S
import m2c_sinit_tail as TAIL
import sinit_tail_disasm as TDIS

REPO = pathlib.Path(__file__).resolve().parent.parent
TAIL_START = 0x37AC
PCREL_RE = re.compile(r"^(\w+),\[pc,#(-?0x[0-9a-fA-F]+|-?\d+)\]$")
MOVIMM_RE = re.compile(r"^r(\d+),(#0x[0-9a-fA-F]+|\d+)$")

FN_NARGS = {
    "func_020731dc": 3,
    "func_0203b9b4": 2,
    "func_ov002_020e6ad4": 1,
}


def squash(s):
    return re.sub(r"\s+", "", s)


def pool_val(addr, insn_addr, off_str, window):
    off = int(off_str, 0)
    slot = insn_addr + 8 + off
    rel = relocs.get(slot)
    if rel:
        return ("sym", R.name_for_reloc(rel, syms))
    w = int.from_bytes(window[slot - addr : slot - addr + 4], "little")
    if w >= (1 << 31):
        w -= 1 << 32
    return ("const", w)


def walk_code(code, ins, tgt, addr):
    ordered = []
    seen = set()
    work = [0]
    while work:
        off = work.pop(0)
        while 0 <= off < len(tgt) and off not in seen and off in code:
            seen.add(off)
            ordered.append(off)
            i = ins[off]
            if i.mnemonic == "b":
                t = M2._branch_target(i)
                if t is not None:
                    work.append(t - addr)
                break
            if M2.BL_RE.match(i.mnemonic):
                off += 4
                continue
            if M2._ends_flow(i):
                break
            off += 4
    return ordered


def emit_call(fn, args):
    parts = []
    for n, (kind, val) in enumerate(args):
        if kind == "const":
            parts.append(f"0x{val:x}" if val >= 0 else str(val))
            continue
        is_fn = val.startswith("func_") or val.startswith("_Z")
        if fn == "func_020731dc":
            if n == 0:
                parts.append(f"&{val}")
            elif is_fn:
                parts.append(f"(void *)&{val}")
            else:
                parts.append(f"(void **)&{val}")
        elif fn == "func_0203b9b4":
            parts.append(f"(int *)&{val}" if n == 0 else (f"0x{val:x}" if kind == "const" else str(val)))
        elif fn == "func_02017acc":
            parts.append(val if n == 0 else (f"0x{val:x}" if kind == "const" else str(val)))
        elif fn == "func_ov002_020e6ad4":
            parts.append(f"&{val}")
        elif is_fn:
            parts.append(f"(void *)&{val}")
        elif n == 0:
            parts.append(f"&{val}")
        else:
            parts.append(f"0x{val:x}" if kind == "const" else str(val))
    return f"    {fn}({', '.join(parts)});\n"


def try_struct_copy_16(ins, off, addr, window):
    """ld rD,[pc]; ld rA,[pc]; ld rB,[pc]; 8-byte copy x2."""
    need = ["ldr", "ldr", "ldr", "ldr", "ldr", "str", "str", "ldr", "ldr", "str", "str"]
    seq = []
    for k in range(len(need)):
        if off + k * 4 not in ins:
            return None, 0
        seq.append(ins[off + k * 4].mnemonic)
    if seq != need:
        return None, 0
    slots = []
    for k in range(3):
        m = PCREL_RE.match(squash(ins[off + k * 4].op_str))
        if not m:
            return None, 0
        slots.append(pool_val(addr, addr + off + k * 4, m.group(2), window))
    if any(s[0] != "sym" for s in slots):
        return None, 0
    dest, src_a, src_b = (s[1] for s in slots)
    line = (f"    {dest}.lo = {src_a};\n"
            f"    {dest}.hi = {src_b};\n")
    return line, 44


def try_struct_copy_8(ins, off, addr, window):
    """ld rD,[pc]; ld rS,[pc]; ldr rX,[rS]; ldr rY,[rS,#4]; str rX,[rD]; str rY,[rD,#4]."""
    need = ["ldr", "ldr", "ldr", "ldr", "str", "str"]
    seq = []
    for k in range(6):
        if off + k * 4 not in ins:
            return None, 0
        seq.append(ins[off + k * 4].mnemonic)
    if seq != need:
        return None, 0
    m0 = PCREL_RE.match(squash(ins[off].op_str))
    m1 = PCREL_RE.match(squash(ins[off + 4].op_str))
    if not (m0 and m1):
        return None, 0
    d = pool_val(addr, addr + off, m0.group(2), window)
    s = pool_val(addr, addr + off + 4, m1.group(2), window)
    if d[0] != "sym" or s[0] != "sym":
        return None, 0
    line = f"    {d[1]} = {s[1]};\n"
    return line, 24


def nargs_for(fn):
    if fn in FN_NARGS:
        return FN_NARGS[fn]
    if fn.startswith("_Z") or fn in ("func_02017acc", "func_02017934", "func_02017b4c"):
        return 2
    return 2


def generate(name):
    global relocs, syms
    addr_hex, size, module, thex = NM.resolve_name(name)
    if not addr_hex:
        raise SystemExit(f"unknown function {name}")
    addr = int(addr_hex, 16)
    mod = next(m for m in MOD.modules() if m["name"] == module)
    tgt = MOD.read_bytes(mod, addr, size)
    window = mod["bin"].read_bytes()[addr - mod["base"] : addr - mod["base"] + size + 0x4000]
    relocs = R.load_relocs_file(mod["relocs"])
    syms = R.load_all_syms()
    ins = M2.decode_words(tgt, addr)
    code = M2.classify(tgt, addr, ins, relocs)
    ordered = walk_code(code, ins, tgt, addr)

    body = []
    syms_used = set()
    p2_syms = set()
    p4_syms = set()
    fn_decls = set()
    regs = {}
    i = 0
    while i < len(ordered):
        off = ordered[i]
        if off >= TAIL_START:
            break
        insn = ins[off]

        hit, adv = try_struct_copy_16(ins, off, addr, window)
        if hit:
            body.append(hit)
            dest_m = re.match(r"\s+(\S+)\.lo", hit)
            if dest_m:
                p4_syms.add(dest_m.group(1))
                syms_used.add(dest_m.group(1))
            for m in re.findall(r"= (\S+);", hit):
                syms_used.add(m)
                p2_syms.add(m)
            i += 1
            while i < len(ordered) and ordered[i] < off + adv:
                i += 1
            continue

        hit, adv = try_struct_copy_8(ins, off, addr, window)
        if hit:
            body.append(hit)
            for m in re.findall(r"(data_[^\s;=]+)", hit):
                syms_used.add(m)
                p2_syms.add(m)
            i += 1
            while i < len(ordered) and ordered[i] < off + adv:
                i += 1
            continue

        s = squash(insn.op_str)
        if insn.mnemonic == "ldr":
            m = PCREL_RE.match(s)
            if m:
                regs[m.group(1)] = pool_val(addr, addr + off, m.group(2), window)
        elif insn.mnemonic == "mov":
            m = MOVIMM_RE.match(s)
            if m:
                regs["r" + m.group(1)] = ("const", int(m.group(2).lstrip("#"), 0))
        elif M2.BL_RE.match(insn.mnemonic):
            e = relocs.get(addr + off)
            fn = R.name_for_reloc(e, syms) if e else f"func_{M2._branch_target(insn):08x}"
            fn_decls.add(fn)
            nargs = nargs_for(fn)
            args = []
            for k in range(nargs):
                arg = regs.get(f"r{k}")
                if arg is None:
                    raise SystemExit(f"missing r{k} for {fn} at {addr + off:#x}")
                args.append(arg)
                if arg[0] == "sym":
                    syms_used.add(arg[1])
            body.append(emit_call(fn, args))
            for k in range(4):
                regs.pop(f"r{k}", None)

        i += 1

    tail_text, tail_used, tail_p2, tail_p6 = TDIS.emit_tail_disasm(
        ordered, ins, addr, window, relocs, syms, TAIL_START
    )
    body.append(tail_text)
    syms_used |= tail_used
    p2_syms |= tail_p2
    p6_syms = tail_p6

    body_text = "".join(body)
    sym_name = r"(data_[A-Za-z0-9_]+)"
    p4_used = set(re.findall(sym_name + r"\.lo\b", body_text))
    p4_used |= set(re.findall(sym_name + r"\.hi\b", body_text))
    p4_used |= set(re.findall(sym_name + r"\.tail\b", body_text))
    p4_used |= p6_syms

    fixed_decls = [
        "struct P2 { int a, b; };\n",
        "struct P4 { struct P2 lo, hi; };\n",
        "struct P6 { struct P2 lo, hi, tail; };\n",
    ]
    for fn in sorted(fn_decls):
        if fn.startswith("_Z"):
            fixed_decls.append(f"extern int {fn}(void *, int);\n")
        elif fn == "func_0203b9b4":
            fixed_decls.append(f"extern void {fn}(int *, int);\n")
        elif fn == "func_020731dc":
            fixed_decls.append(f"extern void {fn}(void *, void *, void **);\n")
        elif fn in ("func_02017acc", "func_02017934", "func_02017b4c"):
            fixed_decls.append(f"extern int {fn}(void *, int);\n")
        elif fn == "func_ov002_020e6ad4":
            fixed_decls.append(f"extern void {fn}(void *);\n")
        else:
            fixed_decls.append(f"extern void {fn}();\n")
    for sym in sorted(syms_used):
        if sym.startswith("func_") or sym.startswith("_Z"):
            if sym not in fn_decls:
                fixed_decls.append(f"extern void {sym}(void);\n")
        elif sym in tail_used or sym in p6_syms:
            if sym == "data_02086b58":
                fixed_decls.append(f"extern struct P2 {sym};\n")
            elif sym in p6_syms or sym in p4_used:
                fixed_decls.append(f"extern struct P6 {sym};\n")
            else:
                fixed_decls.append(f"extern struct P2 {sym};\n")
        else:
            fixed_decls.append(f"extern char {sym}[];\n")

    src = (
        "".join(fixed_decls)
        + f"void {name}(void)\n{{\n"
        + "    struct P2 tmp;\n"
        + "    int ta, tb;\n\n"
        + body_text
        + "}\n"
    )
    return src, thex


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--name", required=True)
    ap.add_argument("--out", default=None)
    ap.add_argument("--check", action="store_true")
    args = ap.parse_args()
    src, thex = generate(args.name)
    out = pathlib.Path(args.out) if args.out else REPO / f"_{args.name}.c"
    out.write_text(src)
    print(f"wrote {out} ({src.count(chr(10))} lines, {len(src)} bytes)")
    if args.check:
        ok, _ = S.oracle_check(src, args.name, bytes.fromhex(thex))
        print(f"match={ok}")
        return 0 if ok else 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())