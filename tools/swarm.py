"""Zero-token template tier of the matching swarm.

The idea (see notes): most trivial functions don't need an LLM at all. We
disassemble the target, recognise a small set of self-contained *shapes*
(constant returns, field load/store, two-arg arithmetic), emit the obvious C,
and let the existing match.py oracle (compile with mwccarm + relocation-aware
byte-diff) confirm it. Every function cleared here costs **zero tokens**.

Deliberately reloc-FREE only. A template that leaned on a relocation slot (e.g.
a tail-call wrapper `b somefunc`) would "match" via the reloc wildcard without
us knowing the real callee -- a false positive. So we only emit shapes whose
bytes are fully determined by the C, where a match is genuinely conclusive.

Usage:
    python tools/swarm.py                      # dry-run, report free coverage
    python tools/swarm.py --apply              # also append wins to matched.jsonl
    python tools/swarm.py --min 0x8 --max 0x40 --count 500
"""
import argparse
import io
import json
import pathlib
import re
import sys
import tempfile

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match as M  # reuse compile_c / extract_func / compare / target_bytes
import relocs as R  # resolve call targets / globals to symbol names

REPO = M.REPO
SYMS = REPO / "config" / "arm9" / "symbols.txt"
ARM9 = M.ARM9
BASE = M.ARM9_BASE
MATCHED = REPO / "progress" / "matched.jsonl"

md = Cs(CS_ARCH_ARM, CS_MODE_ARM)

LINE_RE = re.compile(
    r"^(\S+)\s+kind:function\((arm|thumb),size=0x([0-9a-fA-F]+)\)\s+addr:0x([0-9a-fA-F]+)"
)


# ----------------------------------------------------------------------------- targets
def load_done():
    done = set()
    if MATCHED.is_file():
        for line in MATCHED.read_text(errors="ignore").splitlines():
            line = line.strip()
            if not line:
                continue
            try:
                done.add(int(json.loads(line)["addr"], 0))
            except Exception:
                pass
    return done


def enumerate_targets(lo_size, hi_size, mode, count):
    done = load_done()
    limit = BASE + ARM9.stat().st_size
    out = []
    for line in SYMS.read_text(errors="ignore").splitlines():
        m = LINE_RE.match(line)
        if not m:
            continue
        name, fmode, size_hex, addr_hex = m.groups()
        size, addr = int(size_hex, 16), int(addr_hex, 16)
        if mode != "any" and fmode != mode:
            continue
        if not (lo_size <= size <= hi_size):
            continue
        if not (BASE <= addr < limit):
            continue
        if addr in done:
            continue
        out.append({"name": name, "addr": addr, "size": size, "mode": fmode})
        if count and len(out) >= count:
            break
    return out


# ----------------------------------------------------------------------------- helpers
def squash(op_str):
    return op_str.replace(" ", "")


def imm(op_str):
    m = re.search(r"#(-?0x[0-9a-fA-F]+|-?\d+)", op_str)
    return int(m.group(1), 0) if m else None


def cfunc(name, ret, params, body):
    return f"{ret} {name}({params}) {{ {body} }}\n"


def pretty(c_source):
    """Reformat a one-line cfunc() into the repo's brace-on-own-line style."""
    m = re.match(r"^(.*?) \{ (.*?) \}\s*$", c_source.strip())
    if not m:
        return c_source if c_source.endswith("\n") else c_source + "\n"
    sig, body = m.group(1), m.group(2).strip()
    inner = f"    {body}\n" if body else ""
    return f"{sig}\n{{\n{inner}}}\n"


# Load/store width tables: mnemonic -> (C pointer type, element size in bytes)
LOADS = {
    "ldr": ("int", 4), "ldrb": ("unsigned char", 1), "ldrsb": ("signed char", 1),
    "ldrh": ("unsigned short", 2), "ldrsh": ("short", 2),
}
STORES = {
    "str": ("int", 4), "strb": ("char", 1), "strh": ("short", 2),
}
ARITH = {"add": "+", "sub": "-", "orr": "|", "and": "&", "eor": "^", "mul": "*"}

MEM_RE = re.compile(r"^r0,\[r0(?:,#(-?0x[0-9a-fA-F]+|-?\d+))?\]$")   # ldr  dst r0, base r0
ST_RE = re.compile(r"^r1,\[r0(?:,#(-?0x[0-9a-fA-F]+|-?\d+))?\]$")    # str  src r1, base r0


# ----------------------------------------------------------------------------- rules
# Each rule: (name, insns, raw_bytes) -> (c_source, rule_label) or None.
def rule_empty(name, ins, b):
    if len(ins) == 1 and ins[0].mnemonic == "bx" and squash(ins[0].op_str) == "lr":
        return cfunc(name, "void", "void", ""), "empty/identity"
    return None


def rule_ret_const(name, ins, b):
    if len(ins) == 2 and ins[1].mnemonic == "bx":
        i = ins[0]
        s = squash(i.op_str)
        if i.mnemonic == "mov" and s.startswith("r0,#"):
            n = imm(s)
            if n is not None:
                return cfunc(name, "int", "void", f"return {n};"), "ret_const"
        if i.mnemonic == "mvn" and s.startswith("r0,#"):
            n = imm(s)
            if n is not None:
                return cfunc(name, "int", "void", f"return {~n};"), "ret_const_mvn"
    return None


def rule_ret_arg(name, ins, b):
    if len(ins) == 2 and ins[1].mnemonic == "bx" and ins[0].mnemonic == "mov":
        m = re.fullmatch(r"r0,r([0-3])", squash(ins[0].op_str))
        if m:
            n = int(m.group(1))
            params = ", ".join(f"int a{k}" for k in range(n + 1))
            return cfunc(name, "int", params, f"return a{n};"), "ret_arg"
    return None


def rule_load(name, ins, b):
    if len(ins) == 2 and ins[1].mnemonic == "bx" and ins[0].mnemonic in LOADS:
        m = MEM_RE.match(squash(ins[0].op_str))
        if m:
            ptype, esize = LOADS[ins[0].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            if off % esize == 0:
                idx = off // esize
                return cfunc(name, "int", f"{ptype} *p", f"return p[{idx}];"), "load_field"
    return None


def rule_store(name, ins, b):
    if len(ins) == 2 and ins[1].mnemonic == "bx" and ins[0].mnemonic in STORES:
        m = ST_RE.match(squash(ins[0].op_str))
        if m:
            ptype, esize = STORES[ins[0].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            if off % esize == 0:
                idx = off // esize
                return cfunc(name, "void", f"{ptype} *p, int v", f"p[{idx}] = v;"), "store_field"
    return None


def rule_arith(name, ins, b):
    if len(ins) == 2 and ins[1].mnemonic == "bx" and ins[0].mnemonic in ARITH:
        if re.fullmatch(r"r0,r0,r1", squash(ins[0].op_str)):
            op = ARITH[ins[0].mnemonic]
            return cfunc(name, "int", "int a, int b", f"return a {op} b;"), "arith2"
    return None


GG_RE = re.compile(r"^r0,\[r0(?:,#(0x[0-9a-fA-F]+|\d+))?\]$")   # <load> r0, [r0, #off]


def rule_global_getter(name, ins, b):
    # ldr r0,[pc,#x] ; <load> r0,[r0,#off] ; bx lr ; .word G  -> return a global field.
    # The .word is the global's absolute address: a relocation the oracle wildcards.
    if (len(ins) >= 3 and ins[0].mnemonic == "ldr"
            and squash(ins[0].op_str).startswith("r0,[pc")
            and ins[1].mnemonic in LOADS
            and ins[2].mnemonic == "bx" and squash(ins[2].op_str) == "lr"):
        m = GG_RE.match(squash(ins[1].op_str))
        if m:
            ptype, esize = LOADS[ins[1].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            if off % esize == 0:
                src = (f"extern {ptype} G[];\n"
                       f"int {name}(void) {{ return G[{off // esize}]; }}\n")
                return src, "global_getter"
    return None


IMM = r"#(0x[0-9a-fA-F]+|\d+)"


def rule_load_mask(name, ins, b):
    # <load> r0,[r0(,#off)] ; and r0,r0,#imm ; bx lr   -> return field & imm
    if (len(ins) == 3 and ins[0].mnemonic in LOADS and ins[1].mnemonic == "and"
            and ins[2].mnemonic == "bx" and squash(ins[2].op_str) == "lr"):
        m = MEM_RE.match(squash(ins[0].op_str))
        a = re.fullmatch(r"r0,r0," + IMM, squash(ins[1].op_str))
        if m and a:
            ptype, esize = LOADS[ins[0].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            if off % esize == 0:
                return cfunc(name, "int", f"{ptype} *p",
                             f"return p[{off // esize}] & {int(a.group(1), 0)};"), "load_mask"
    return None


def rule_store_const(name, ins, b):
    # mov rX,#imm ; <store> rX,[rBase(,#off)] ; bx lr  -> base r0..r3 = the (n+1)th pointer arg
    if (len(ins) == 3 and ins[0].mnemonic == "mov" and ins[1].mnemonic in STORES
            and ins[2].mnemonic == "bx" and squash(ins[2].op_str) == "lr"):
        mv = re.fullmatch(r"(r\d+)," + IMM, squash(ins[0].op_str))
        if mv:
            st = re.fullmatch(mv.group(1) + r",\[(r[0-3])(?:," + IMM + r")?\]", squash(ins[1].op_str))
            if st:
                ptype, esize = STORES[ins[1].mnemonic]
                off = int(st.group(2), 0) if st.group(2) else 0
                argn = int(st.group(1)[1])
                if off % esize == 0:
                    params = [f"int a{i}" for i in range(argn)] + [f"{ptype} *p"]
                    return cfunc(name, "void", ", ".join(params),
                                 f"p[{off // esize}] = {int(mv.group(2), 0)};"), "store_const"
    return None


def rule_global_setter(name, ins, b):
    # ldr rB,[pc..] ; <store> r0,[rB(,#off)] ; bx lr ; .word G   -> GLOBAL.field = arg
    if (len(ins) >= 3 and ins[0].mnemonic == "ldr" and ins[1].mnemonic in STORES
            and ins[2].mnemonic == "bx" and squash(ins[2].op_str) == "lr"):
        ld = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        if ld:
            st = re.fullmatch(r"r0,\[" + ld.group(1) + r"(?:," + IMM + r")?\]", squash(ins[1].op_str))
            if st:
                ptype, esize = STORES[ins[1].mnemonic]
                off = int(st.group(1), 0) if st.group(1) else 0
                if off % esize == 0:
                    return (f"extern {ptype} G[];\n"
                            f"void {name}(int v) {{ G[{off // esize}] = v; }}\n"), "global_setter"
    return None


def rule_store_global_addr(name, ins, b):
    # ldr rX,[pc..] ; str rX,[r0(,#off)] ; bx lr ; .word G   -> p[idx] = &global (vtable install)
    if (len(ins) >= 3 and ins[0].mnemonic == "ldr" and ins[1].mnemonic == "str"
            and ins[2].mnemonic == "bx" and squash(ins[2].op_str) == "lr"):
        ld = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        if ld:
            st = re.fullmatch(ld.group(1) + r",\[r0(?:," + IMM + r")?\]", squash(ins[1].op_str))
            if st:
                off = int(st.group(1), 0) if st.group(1) else 0
                if off % 4 == 0:
                    return (f"extern int G[];\n"
                            f"void {name}(int *p) {{ p[{off // 4}] = (int)G; }}\n"), "store_global_addr"
    return None


def rule_field_bitop(name, ins, b):
    # add rB,r0,#off ; ldr rV,[rB] ; orr|bic rV,rV,#imm ; str rV,[rB] ; bx lr
    if (len(ins) == 5 and ins[0].mnemonic == "add" and ins[1].mnemonic == "ldr"
            and ins[2].mnemonic in ("orr", "bic") and ins[3].mnemonic == "str"
            and ins[4].mnemonic == "bx" and squash(ins[4].op_str) == "lr"):
        a = re.fullmatch(r"(r\d+),r0," + IMM, squash(ins[0].op_str))
        if a:
            rB = a.group(1)
            ld = re.fullmatch(r"(r\d+),\[" + rB + r"\]", squash(ins[1].op_str))
            op = re.fullmatch(r"(r\d+),(r\d+)," + IMM, squash(ins[2].op_str))
            stm = re.fullmatch(r"(r\d+),\[" + rB + r"\]", squash(ins[3].op_str))
            off = int(a.group(2), 0)
            if ld and op and stm and off % 4 == 0:
                imm = int(op.group(3), 0)
                idx = off // 4
                body = f"p[{idx}] |= {imm};" if ins[2].mnemonic == "orr" else f"p[{idx}] &= ~{imm};"
                return cfunc(name, "void", "int *p", body), "field_bitop"
    return None


def rule_cmp_eq(name, ins, b):
    # <load> r0,[r0(,#off)] ; cmp r0,#k ; moveq r0,#a ; movne r0,#bv ; bx lr  -> return p[i] ==/!= k
    if len(ins) == 5 and ins[0].mnemonic in LOADS and ins[1].mnemonic == "cmp" \
            and ins[2].mnemonic == "moveq" and ins[3].mnemonic == "movne" \
            and ins[4].mnemonic == "bx" and squash(ins[4].op_str) == "lr":
        m = MEM_RE.match(squash(ins[0].op_str))
        c = re.fullmatch(r"r0," + IMM, squash(ins[1].op_str))
        eq = re.fullmatch(r"r0," + IMM, squash(ins[2].op_str))
        ne = re.fullmatch(r"r0," + IMM, squash(ins[3].op_str))
        if m and c and eq and ne:
            ptype, esize = LOADS[ins[0].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            eqv, nev = int(eq.group(1), 0), int(ne.group(1), 0)
            if off % esize == 0 and {eqv, nev} == {0, 1}:
                op = "==" if eqv == 1 else "!="
                return cfunc(name, "int", f"{ptype} *p",
                             f"return p[{off // esize}] {op} {int(c.group(1), 0)};"), "cmp_eq"
    return None


def rule_struct_copy(name, ins, b):
    # K pairs of ldr rT,[BASE,#o] ; str rT,[OTHER,#o] (bases r0/r1, consistent) ; bx lr
    if len(ins) < 3 or len(ins) % 2 == 0:
        return None
    if not (ins[-1].mnemonic == "bx" and squash(ins[-1].op_str) == "lr"):
        return None
    lbase = sbase = None
    pairs = []
    for k in range(0, len(ins) - 1, 2):
        ld, st = ins[k], ins[k + 1]
        if ld.mnemonic != "ldr" or st.mnemonic != "str":
            return None
        lm = re.fullmatch(r"(r\d+),\[(r[01])(?:," + IMM + r")?\]", squash(ld.op_str))
        sm = re.fullmatch(r"(r\d+),\[(r[01])(?:," + IMM + r")?\]", squash(st.op_str))
        if not (lm and sm and lm.group(1) == sm.group(1)):
            return None
        if lbase is None:
            lbase, sbase = lm.group(2), sm.group(2)
        if (lm.group(2), sm.group(2)) != (lbase, sbase) or lbase == sbase:
            return None
        soff = int(lm.group(3), 0) if lm.group(3) else 0
        doff = int(sm.group(3), 0) if sm.group(3) else 0
        if soff % 4 or doff % 4:
            return None
        pairs.append((doff // 4, soff // 4))
    nm = {"r0": "a", "r1": "b"}                       # r0 = first param, r1 = second
    src, dst = nm[lbase], nm[sbase]
    body = " ".join(f"{dst}[{d}] = {src}[{s}];" for d, s in pairs)
    return cfunc(name, "void", "int *a, int *b", body), "struct_copy"


def rule_add_imm(name, ins, b):
    # add/sub r0,r0,#imm ; bx lr   -> return arg +/- imm  (e.g. &this->field)
    if len(ins) == 2 and ins[0].mnemonic in ("add", "sub") \
            and ins[1].mnemonic == "bx" and squash(ins[1].op_str) == "lr":
        m = re.fullmatch(r"r0,r0," + IMM, squash(ins[0].op_str))
        if m:
            sign = "+" if ins[0].mnemonic == "add" else "-"
            return cfunc(name, "int", "int p", f"return p {sign} {int(m.group(1), 0)};"), "add_imm"
    return None


# unsigned load types -- needed so `>>` emits a logical shift (lsr), not arithmetic (asr)
UWORD = {"ldr": ("unsigned int", 4), "ldrb": ("unsigned char", 1), "ldrh": ("unsigned short", 2)}


def rule_bitfield(name, ins, b):
    # <load> r0,[r0(,#off)] ; lsr r0,r0,#sh ; and r0,r0,#mask ; bx lr
    if len(ins) == 4 and ins[0].mnemonic in UWORD and ins[1].mnemonic == "lsr" \
            and ins[2].mnemonic == "and" and ins[3].mnemonic == "bx" \
            and squash(ins[3].op_str) == "lr":
        m = MEM_RE.match(squash(ins[0].op_str))
        sh = re.fullmatch(r"r0,r0," + IMM, squash(ins[1].op_str))
        mk = re.fullmatch(r"r0,r0," + IMM, squash(ins[2].op_str))
        if m and sh and mk:
            ptype, esize = UWORD[ins[0].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            if off % esize == 0:
                return cfunc(name, "int", f"{ptype} *p",
                             f"return (p[{off // esize}] >> {int(sh.group(1), 0)}) & "
                             f"{int(mk.group(1), 0)};"), "bitfield"
    return None


def rule_global_store_const(name, ins, b):
    # ldr rB,[pc] ; mov rV,#imm ; <store> rV,[rB(,#off)] ; bx lr ; .word G
    if len(ins) >= 4 and ins[0].mnemonic == "ldr" and ins[1].mnemonic == "mov" \
            and ins[2].mnemonic in STORES and ins[3].mnemonic == "bx" \
            and squash(ins[3].op_str) == "lr":
        ld = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        mv = re.fullmatch(r"(r\d+)," + IMM, squash(ins[1].op_str))
        if ld and mv:
            st = re.fullmatch(mv.group(1) + r",\[" + ld.group(1) + r"(?:," + IMM + r")?\]",
                              squash(ins[2].op_str))
            if st:
                ptype, esize = STORES[ins[2].mnemonic]
                off = int(st.group(1), 0) if st.group(1) else 0
                if off % esize == 0:
                    return (f"extern {ptype} G[];\n"
                            f"void {name}(void) {{ G[{off // esize}] = "
                            f"{int(mv.group(2), 0)}; }}\n"), "global_store_const"
    return None


def rule_global_index(name, ins, b):
    # ldr rB,[pc] ; <load> r0,[rB,r0(,lsl #k)] ; bx lr ; .word G   -> return G[i]
    if len(ins) >= 3 and ins[0].mnemonic == "ldr" and ins[1].mnemonic in LOADS \
            and ins[2].mnemonic == "bx" and squash(ins[2].op_str) == "lr":
        ld = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        if ld:
            idx = re.fullmatch(r"r0,\[" + ld.group(1) + r",r0(?:,lsl" + IMM + r")?\]",
                               squash(ins[1].op_str))
            if idx:
                ptype, _ = LOADS[ins[1].mnemonic]
                return (f"extern {ptype} G[];\n"
                        f"int {name}(int i) {{ return G[i]; }}\n"), "global_index"
    return None


def rule_two_global_setters(name, ins, b):
    # ldr rB,[pc] ; ldr rC,[pc] ; <store> r0,[rB] ; <store> r1,[rC] ; bx lr ; .word A ; .word B
    if len(ins) >= 5 and ins[0].mnemonic == "ldr" and ins[1].mnemonic == "ldr" \
            and ins[2].mnemonic in STORES and ins[3].mnemonic in STORES \
            and ins[4].mnemonic == "bx" and squash(ins[4].op_str) == "lr":
        b0 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        b1 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[1].op_str))
        if b0 and b1:
            s0 = re.fullmatch(r"r0,\[" + b0.group(1) + r"\]", squash(ins[2].op_str))
            s1 = re.fullmatch(r"r1,\[" + b1.group(1) + r"\]", squash(ins[3].op_str))
            if s0 and s1:
                pt0, _ = STORES[ins[2].mnemonic]
                pt1, _ = STORES[ins[3].mnemonic]
                return (f"extern {pt0} A[]; extern {pt1} B[];\n"
                        f"void {name}(int a, int b) {{ A[0] = a; B[0] = b; }}\n"), "two_global_setters"
    return None


def rule_global_swap(name, ins, b):
    # ldr rB,[pc] ; ldr rO,[rB] ; str r0,[rB] ; mov r0,rO ; bx lr ; .word &G
    # old = G; G = arg; return old
    if len(ins) >= 5 and ins[0].mnemonic == "ldr" and ins[1].mnemonic == "ldr" \
            and ins[2].mnemonic == "str" and ins[3].mnemonic == "mov" \
            and ins[4].mnemonic == "bx" and squash(ins[4].op_str) == "lr":
        b0 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        if b0:
            lo = re.fullmatch(r"(r\d+),\[" + b0.group(1) + r"\]", squash(ins[1].op_str))
            st = re.fullmatch(r"r0,\[" + b0.group(1) + r"\]", squash(ins[2].op_str))
            if lo and st and re.fullmatch(r"r0," + lo.group(1), squash(ins[3].op_str)):
                return ("extern int G;\n"
                        f"int {name}(int v) {{ int old = G; G = v; return old; }}\n"), "global_swap"
    return None


def rule_two_global_store_const(name, ins, b):
    # ldr rB,[pc] ; mov rV,#k ; ldr rC,[pc] ; <store> rV,[rB] ; <store> rV,[rC] ; bx lr ; .word A,B
    if len(ins) >= 6 and ins[0].mnemonic == "ldr" and ins[1].mnemonic == "mov" \
            and ins[2].mnemonic == "ldr" and ins[3].mnemonic in STORES \
            and ins[4].mnemonic == ins[3].mnemonic and ins[5].mnemonic == "bx" \
            and squash(ins[5].op_str) == "lr":
        b0 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        mv = re.fullmatch(r"(r\d+)," + IMM, squash(ins[1].op_str))
        b1 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[2].op_str))
        if b0 and mv and b1:
            s0 = re.fullmatch(mv.group(1) + r",\[" + b0.group(1) + r"\]", squash(ins[3].op_str))
            s1 = re.fullmatch(mv.group(1) + r",\[" + b1.group(1) + r"\]", squash(ins[4].op_str))
            if s0 and s1:
                ptype, _ = STORES[ins[3].mnemonic]
                k = int(mv.group(2), 0)
                return (f"extern {ptype} A, B;\n"
                        f"void {name}(void) {{ A = {k}; B = {k}; }}\n"), "two_global_store_const"
    return None


def rule_global_field_bitop(name, ins, b):
    # ldr rB,[pc] ; <load> rV,[rB,#off] ; orr|bic rV,rV,#imm ; <store> rV,[rB,#off] ; bx lr
    if len(ins) >= 5 and ins[0].mnemonic == "ldr" and ins[1].mnemonic in LOADS \
            and ins[2].mnemonic in ("orr", "bic") and ins[3].mnemonic in STORES \
            and ins[4].mnemonic == "bx" and squash(ins[4].op_str) == "lr":
        ld0 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        if ld0:
            rB = ld0.group(1)
            lm = re.fullmatch(r"(r\d+),\[" + rB + r"(?:," + IMM + r")?\]", squash(ins[1].op_str))
            op = re.fullmatch(r"(r\d+),(r\d+)," + IMM, squash(ins[2].op_str))
            sm = re.fullmatch(r"(r\d+),\[" + rB + r"(?:," + IMM + r")?\]", squash(ins[3].op_str))
            if lm and op and sm:
                ltype, lesz = LOADS[ins[1].mnemonic]
                _, sesz = STORES[ins[3].mnemonic]
                off = int(lm.group(2), 0) if lm.group(2) else 0
                soff = int(sm.group(2), 0) if sm.group(2) else 0
                if lesz == sesz and off == soff and off % lesz == 0:
                    imm = int(op.group(3), 0)
                    idx = off // lesz
                    # Coloring: casting the array name to a pointer and dereferencing
                    # (*(T*)G) reproduces the ROM's r0/r1 ordering, where indexing
                    # (G[0]) compiles to the swapped allocation. For idx>0 keep the
                    # equivalent pointer form so the access still lands at the offset.
                    lval = f"*({ltype}*)G" if idx == 0 else f"*({ltype}*)(G+{idx})"
                    body = f"{lval} |= {imm};" if ins[2].mnemonic == "orr" else f"{lval} &= ~{imm};"
                    return (f"extern {ltype} G[];\nvoid {name}(void) {{ {body} }}\n"), "global_field_bitop"
    return None


def rule_global_bit_test(name, ins, b):
    # ldr rB,[pc] ; <load> r0,[rB] ; ands r0,r0,#mask ; movne r0,#a ; moveq r0,#bv ; bx ; .word &G
    if len(ins) >= 6 and ins[0].mnemonic == "ldr" and ins[1].mnemonic in LOADS \
            and ins[2].mnemonic == "ands" and ins[3].mnemonic == "movne" \
            and ins[4].mnemonic == "moveq" and ins[5].mnemonic == "bx" \
            and squash(ins[5].op_str) == "lr":
        b0 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        if b0:
            ld = re.fullmatch(r"r0,\[" + b0.group(1) + r"\]", squash(ins[1].op_str))
            a = re.fullmatch(r"r0,r0," + IMM, squash(ins[2].op_str))
            ne = re.fullmatch(r"r0," + IMM, squash(ins[3].op_str))
            eq = re.fullmatch(r"r0," + IMM, squash(ins[4].op_str))
            if ld and a and ne and eq:
                ptype = UWORD.get(ins[1].mnemonic, (LOADS[ins[1].mnemonic][0],))[0]
                nev, eqv = int(ne.group(1), 0), int(eq.group(1), 0)
                if {nev, eqv} == {0, 1}:
                    op = "!=" if nev == 1 else "=="
                    return (f"extern {ptype} G;\n"
                            f"int {name}(void) {{ return (G & {int(a.group(1), 0)}) {op} 0; }}\n"), \
                        "global_bit_test"
    return None


def rule_bit_test(name, ins, b):
    # <load> r0,[r0(,#off)] ; ands r0,r0,#mask ; moveq r0,#a ; movne r0,#bv ; bx lr
    if len(ins) == 5 and ins[0].mnemonic in LOADS and ins[1].mnemonic == "ands" \
            and ins[2].mnemonic == "moveq" and ins[3].mnemonic == "movne" \
            and ins[4].mnemonic == "bx" and squash(ins[4].op_str) == "lr":
        m = MEM_RE.match(squash(ins[0].op_str))
        a = re.fullmatch(r"r0,r0," + IMM, squash(ins[1].op_str))
        eq = re.fullmatch(r"r0," + IMM, squash(ins[2].op_str))
        ne = re.fullmatch(r"r0," + IMM, squash(ins[3].op_str))
        if m and a and eq and ne:
            ptype, esize = LOADS[ins[0].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            eqv, nev = int(eq.group(1), 0), int(ne.group(1), 0)
            if off % esize == 0 and {eqv, nev} == {0, 1}:
                op = "==" if eqv == 1 else "!="
                return cfunc(name, "int", f"{ptype} *p",
                             f"return (p[{off // esize}] & {int(a.group(1), 0)}) {op} 0;"), "bit_test"
    return None


def rule_two_indexed_store(name, ins, b):
    # ldr rB,[pc] ; ldr rC,[pc] ; str r1,[rB,r0,lsl #2] ; strb r2,[rC,r0] ; bx lr ; .word A ; .word B
    if len(ins) >= 5 and ins[0].mnemonic == "ldr" and ins[1].mnemonic == "ldr" \
            and ins[2].mnemonic == "str" and ins[3].mnemonic == "strb" \
            and ins[4].mnemonic == "bx" and squash(ins[4].op_str) == "lr":
        b0 = re.fullmatch(r"(r\d+|ip|sb|sl|fp),\[pc.*\]", squash(ins[0].op_str))
        b1 = re.fullmatch(r"(r\d+|ip|sb|sl|fp),\[pc.*\]", squash(ins[1].op_str))
        if b0 and b1 \
                and re.fullmatch(r"r1,\[" + b0.group(1) + r",r0,lsl#2\]", squash(ins[2].op_str)) \
                and re.fullmatch(r"r2,\[" + b1.group(1) + r",r0\]", squash(ins[3].op_str)):
            return ("extern int A[]; extern unsigned char B[];\n"
                    f"void {name}(int i, int v1, int v2) {{ A[i] = v1; B[i] = v2; }}\n"), "two_indexed_store"
    return None


def rule_setbit_byidx(name, ins, b):
    # ldr rB,[pc] ; mov rV,#1 ; ldr rW,[rB,#off] ; orr r0,rW,rV,lsl r0 ; str r0,[rB,#off] ; bx lr
    if len(ins) >= 6 and ins[0].mnemonic == "ldr" and ins[1].mnemonic == "mov" \
            and ins[2].mnemonic == "ldr" and ins[3].mnemonic == "orr" \
            and ins[4].mnemonic == "str" and ins[5].mnemonic == "bx" \
            and squash(ins[5].op_str) == "lr":
        ld = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        mv = re.fullmatch(r"(r\d+),#1", squash(ins[1].op_str))
        if ld and mv:
            lw = re.fullmatch(r"(r\d+),\[" + ld.group(1) + r"(?:," + IMM + r")?\]", squash(ins[2].op_str))
            orr = re.fullmatch(r"r0,(r\d+)," + mv.group(1) + r",lslr0", squash(ins[3].op_str))
            st = re.fullmatch(r"r0,\[" + ld.group(1) + r"(?:," + IMM + r")?\]", squash(ins[4].op_str))
            if lw and orr and st:
                off = int(lw.group(2), 0) if lw.group(2) else 0
                if off % 4 == 0:
                    return ("extern int G[];\n"
                            f"void {name}(int n) {{ G[{off // 4}] |= 1 << n; }}\n"), "setbit_byidx"
    return None


def rule_testbit_byidx(name, ins, b):
    # ldr rB,[pc] ; mov rV,#1 ; ldr rW,[rB,#off] ; and r0,rW,rV,lsl r0 ; bx lr
    if len(ins) >= 5 and ins[0].mnemonic == "ldr" and ins[1].mnemonic == "mov" \
            and ins[2].mnemonic == "ldr" and ins[3].mnemonic == "and" \
            and ins[4].mnemonic == "bx" and squash(ins[4].op_str) == "lr":
        ld = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
        mv = re.fullmatch(r"(r\d+),#1", squash(ins[1].op_str))
        if ld and mv:
            lw = re.fullmatch(r"(r\d+),\[" + ld.group(1) + r"(?:," + IMM + r")?\]", squash(ins[2].op_str))
            a = re.fullmatch(r"r0,(r\d+)," + mv.group(1) + r",lslr0", squash(ins[3].op_str))
            if lw and a:
                off = int(lw.group(2), 0) if lw.group(2) else 0
                if off % 4 == 0:
                    return ("extern int G[];\n"
                            f"int {name}(int n) {{ return G[{off // 4}] & (1 << n); }}\n"), "testbit_byidx"
    return None


def rule_pool_const(name, ins, b):
    # ldr r0, [pc, #..] ; bx lr ; .word N   (size 12)
    # Must load into r0 (the return reg) and RETURN (bx lr) -- not a thunk (bx ip).
    if len(b) >= 12 and ins and ins[0].mnemonic == "ldr":
        s0 = squash(ins[0].op_str)
        if s0.startswith("r0,[pc") and any(
                i.mnemonic == "bx" and squash(i.op_str) == "lr" for i in ins[:3]):
            word = int.from_bytes(b[8:12], "little")
            signed = word - (1 << 32) if word >= (1 << 31) else word
            return cfunc(name, "int", "void", f"return {signed};"), "pool_const"
    return None


def is_thunk(ins):
    """Tail-call / linker veneer: a computed jump `bx <reg>` to anything but lr.
    These are link-time artifacts -- not reproducible from a single C function."""
    return any(i.mnemonic == "bx" and squash(i.op_str) != "lr" for i in ins)


def code_insns(ins):
    """Trim the trailing literal pool. A symbol's size includes its pool words, which
    decode as junk (andeq/andseq...). Real code ends at the LAST return instruction
    (bx lr / pop ..pc / ldm ..pc / mov pc,lr); everything after is pool. Use this
    before taking a mnemonic shape or scanning for register use, or the pool pollutes
    the result."""
    last = -1
    for i, x in enumerate(ins):
        op = squash(x.op_str)
        if x.mnemonic == "bx" and op in ("lr", "ip", "r12", "r14"):
            last = i
        elif x.mnemonic in ("pop", "ldm", "ldmia", "ldmfd") and "pc" in op:
            last = i
        elif x.mnemonic == "mov" and op == "pc,lr":
            last = i
    return ins[:last + 1] if last >= 0 else ins


# ----------------------------------------------------------------------------- reloc-aware rules
# These need the function's address + the reloc/symbol tables, so they take a
# wider signature than the leaf rules. They resolve `bl` targets to real names.
def callee_name(addr, ins, relocs, syms):
    """Resolved name of the first bl/blx callee in the function, or None."""
    for i in ins:
        if i.mnemonic in ("bl", "blx"):
            entry = relocs.get(addr + i.address)
            if entry:
                return R.name_for_reloc(entry, syms)
    return None


def rule_wrapper(name, ins, b, addr, relocs, syms):
    # push {r4,lr} ; mov r4,r0 ; bl X ; mov r0,r4 ; pop {r4,lr} ; bx lr
    #   -> call X for its side effect, return the original arg
    m = [i.mnemonic for i in ins]
    if m[:6] == ["push", "mov", "bl", "mov", "pop", "bx"] \
            and squash(ins[5].op_str) == "lr" \
            and squash(ins[1].op_str) == "r4,r0" and squash(ins[3].op_str) == "r0,r4":
        callee = callee_name(addr, ins, relocs, syms)
        if callee and callee != name:
            return (f"extern int {callee}();\n"
                    f"int {name}(int x) {{ {callee}(x); return x; }}\n"), "wrapper"
    return None


def rule_ctor_vtable(name, ins, b, addr, relocs, syms):
    # push {r4,lr} ; ldr r1,[pc] ; mov r4,r0 ; str r1,[r4] ; bl X ; mov r0,r4 ; pop ; bx lr ; .word VT
    #   -> install vtable into *this, call X(this), return this
    m = [i.mnemonic for i in ins]
    if m[:8] == ["push", "ldr", "mov", "str", "bl", "mov", "pop", "bx"] \
            and squash(ins[7].op_str) == "lr" \
            and re.fullmatch(r"r1,\[pc.*\]", squash(ins[1].op_str)) \
            and squash(ins[2].op_str) == "r4,r0" \
            and squash(ins[3].op_str) == "r1,[r4]" \
            and squash(ins[5].op_str) == "r0,r4":
        callee = callee_name(addr, ins, relocs, syms)
        if callee and callee != name:
            return (f"extern int VT[]; extern int {callee}();\n"
                    f"int {name}(int *x) {{ x[0] = (int)VT; {callee}(x); return (int)x; }}\n"), "ctor_vtable"
    return None


def rule_chain_dtor(name, ins, b, addr, relocs, syms):
    # push {r4,lr} ; mov r4,r0 ; [ (add r0,r4,#off)? ; bl X ]+ ; mov r0,r4 ; pop {r4,lr} ; bx lr
    #   -> destructor-style chain: call each helper on this(+off), return this
    if not (ins[0].mnemonic == "push" and ins[1].mnemonic == "mov"
            and squash(ins[1].op_str) == "r4,r0"):
        return None
    if not (len(ins) >= 6 and ins[-1].mnemonic == "bx" and squash(ins[-1].op_str) == "lr"
            and ins[-2].mnemonic == "pop" and ins[-3].mnemonic == "mov"
            and squash(ins[-3].op_str) == "r0,r4"):
        return None
    body = ins[2:-3]
    calls, i = [], 0
    while i < len(body):
        if body[i].mnemonic == "bl":                       # r0 already = this
            off, bi, i = 0, body[i], i + 1
        elif body[i].mnemonic == "add" and i + 1 < len(body) and body[i + 1].mnemonic == "bl":
            mm = re.fullmatch(r"r0,r4," + IMM, squash(body[i].op_str))
            if not mm:
                return None
            off, bi, i = int(mm.group(1), 0), body[i + 1], i + 2
        elif body[i].mnemonic == "mov" and squash(body[i].op_str) == "r0,r4" \
                and i + 1 < len(body) and body[i + 1].mnemonic == "bl":
            off, bi, i = 0, body[i + 1], i + 2
        else:
            return None
        e = relocs.get(addr + bi.address)
        if not e:
            return None
        calls.append((off, R.name_for_reloc(e, syms)))
    if not calls:
        return None
    decls = "".join(f"extern int {c}();\n" for c in dict.fromkeys(c for _, c in calls))
    lines = "".join(f"    {c}(t{f' + {off:#x}' if off else ''});\n" for off, c in calls)
    if name in (c for _, c in calls):
        return None
    return f"{decls}\nint {name}(char *t)\n{{\n{lines}    return (int)t;\n}}\n", "chain_dtor"


def rule_guard_call(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; mov r4,r0; bl C1; cmp r0,#0; popeq; bxeq; mov r0,r4; bl C2; pop; bx lr
    #   -> r = C1(this); if (r == 0) return r; return C2(this);
    if [i.mnemonic for i in ins] != ["push", "mov", "bl", "cmp", "popeq", "bxeq",
                                     "mov", "bl", "pop", "bx"]:
        return None
    if squash(ins[1].op_str) != "r4,r0" or squash(ins[3].op_str) != "r0,#0" \
            or squash(ins[6].op_str) != "r0,r4" or squash(ins[9].op_str) != "lr":
        return None
    e1, e2 = relocs.get(addr + ins[2].address), relocs.get(addr + ins[7].address)
    if not (e1 and e2):
        return None
    c1, c2 = R.name_for_reloc(e1, syms), R.name_for_reloc(e2, syms)
    if name in (c1, c2):
        return None
    decls = f"extern int {c1}();\n" + (f"extern int {c2}();\n" if c2 != c1 else "")
    return (f"{decls}\nint {name}(char *t)\n{{\n    int r = {c1}(t);\n"
            f"    if (r == 0) return r;\n    return {c2}(t);\n}}\n"), "guard_call"


def rule_global_deref(name, ins, b):
    # ldr rB,[pc] ; ldr rP,[rB] ; [add rP,rP,#big] ; <load> r0,[rP(,#off)] ; bx lr ; .word &G
    # G is a global pointer; returns G[idx]. Trailing pool words are ignored.
    if len(ins) < 4 or ins[0].mnemonic != "ldr" or ins[1].mnemonic != "ldr":
        return None
    b0 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
    if not b0:
        return None
    ldp = re.fullmatch(r"(r\d+),\[" + b0.group(1) + r"\]", squash(ins[1].op_str))
    if not ldp:
        return None
    rP = ldp.group(1)
    base_off, i = 0, 2
    if ins[i].mnemonic == "add":
        a = re.fullmatch(rP + r"," + rP + r"," + IMM, squash(ins[i].op_str))
        if not a:
            return None
        base_off = int(a.group(1), 0)
        i += 1
    if i + 1 >= len(ins) or ins[i].mnemonic not in LOADS:
        return None
    if ins[i + 1].mnemonic != "bx" or squash(ins[i + 1].op_str) != "lr":
        return None
    m = re.fullmatch(r"r0,\[" + rP + r"(?:," + IMM + r")?\]", squash(ins[i].op_str))
    if not m:
        return None
    ptype, esize = LOADS[ins[i].mnemonic]
    off = base_off + (int(m.group(1), 0) if m.group(1) else 0)
    if off % esize:
        return None
    return (f"extern {ptype} *G;\n"
            f"int {name}(void) {{ return G[{off // esize}]; }}\n"), "global_deref"


def _parse_obj_body(body, addr, relocs, syms):
    # Parse a run of object-init instructions over `this` held in r4.
    #   add r0,r4,#off / mov r0,r4          -> next call's arg offset (this+off)
    #   bl C                                -> ('ctor', off, C)   one-arg call C(this+off)
    #   ldr rX,[pc] ; str rX,[r4(,#voff)]   -> ('vt', voff, idx)  vtable/pool install
    #   ldr rX,[pc] ; ldr rX,[rX] ; bl D    -> ('dealloc', off, D, gidx)  D(this+off, GLOBAL)
    # Returns (ops, vt_count, g_count) or None. ops are in retire (instruction) order.
    ops, pending, vt, g, pool = [], None, 0, 0, {}
    for ii in body:
        s = squash(ii.op_str)
        if ii.mnemonic == "add":
            mm = re.fullmatch(r"r0,r4," + IMM, s)
            if not mm:
                return None
            pending = int(mm.group(1), 0)
        elif ii.mnemonic == "mov" and s == "r0,r4":
            pending = 0
        elif ii.mnemonic == "bl":
            e = relocs.get(addr + ii.address)
            if not e:
                return None
            who = R.name_for_reloc(e, syms)
            gid = next((v[1] for r, v in pool.items() if isinstance(v, tuple)), None)
            if gid is not None:
                pool = {r: v for r, v in pool.items() if not isinstance(v, tuple)}
                ops.append(("dealloc", pending or 0, who, gid))
            else:
                ops.append(("ctor", pending or 0, who))
            pending = None
        elif ii.mnemonic == "ldr" and re.fullmatch(r"(r\d+),\[pc.*\]", s):
            pool[re.fullmatch(r"(r\d+),\[pc.*\]", s).group(1)] = "raw"
        elif ii.mnemonic == "ldr":
            dm = re.fullmatch(r"(r\d+),\[(r\d+)\]", s)
            if not dm or dm.group(1) != dm.group(2) or pool.get(dm.group(2)) != "raw":
                return None
            pool[dm.group(1)] = ("g", g)
            g += 1
        elif ii.mnemonic == "str":
            sm = re.fullmatch(r"(r\d+),\[r4(?:," + IMM + r")?\]", s)
            if not sm or pool.get(sm.group(1)) != "raw":
                return None
            ops.append(("vt", int(sm.group(2), 0) if sm.group(2) else 0, vt))
            vt += 1
            del pool[sm.group(1)]
        else:
            return None
    return ops, vt, g


def _obj_decls(ops, vt, g):
    seen, decls = set(), ""
    for op in ops:
        if op[0] == "ctor" and op[2] not in seen:
            seen.add(op[2])
            decls += f"extern void {op[2]}(void *);\n"
        elif op[0] == "dealloc" and op[2] not in seen:
            seen.add(op[2])
            decls += f"extern void {op[2]}(void *, void *);\n"
    decls += "".join(f"extern int VT{k}[];\n" for k in range(vt))
    decls += "".join(f"extern void *G{k};\n" for k in range(g))
    return decls


def _emit_obj_ops(ops, var, indent):
    lines = ""
    for op in ops:
        arg = var if op[1] == 0 else f"(char *){var} + {op[1]:#x}"
        if op[0] == "ctor":
            lines += f"{indent}{op[2]}({arg});\n"
        elif op[0] == "dealloc":
            lines += f"{indent}{op[2]}({arg}, G{op[3]});\n"
        else:
            lhs = f"{var}[0]" if op[1] == 0 else f"*(int *)((char *){var} + {op[1]:#x})"
            lines += f"{indent}{lhs} = (int)VT{op[2]};\n"
    return lines


def rule_virtual_call(name, ins, b):
    # stmdb sp!,{lr}; sub sp,sp,#4;
    #   <this+OFF, vtable into rV>:  ldr rV,[r0,#OFF]!   OR   add r0,r0,#OFF ; ldr rV,[r0]
    #   mov r1,#ARG; ldr rV,[rV,#VOFF]; blx rV; mov r0,#RET; add sp,sp,#4; ldm sp!,{lr}; bx lr
    #   -> C++ virtual dispatch: Base* b = &this->base; b->m(ARG); return RET.
    #   The pre-indexed-writeback `ldr rV,[r0,#OFF]!` is a C++-ABI artifact C cannot emit.
    if len(ins) < 9 or ins[0].mnemonic != "stmdb" or squash(ins[0].op_str) != "sp!,{lr}" \
            or ins[1].mnemonic != "sub":
        return None
    w = re.fullmatch(r"(r\d+),\[r0," + IMM + r"\]!", squash(ins[2].op_str))
    if w:                                              # writeback form
        vreg, off, k = w.group(1), int(w.group(2), 0), 3
    elif ins[2].mnemonic == "add" and re.fullmatch(r"r0,r0," + IMM, squash(ins[2].op_str)) \
            and ins[3].mnemonic == "ldr":              # split add + load form
        a = re.fullmatch(r"r0,r0," + IMM, squash(ins[2].op_str))
        lv = re.fullmatch(r"(r\d+),\[r0\]", squash(ins[3].op_str))
        if not lv:
            return None
        vreg, off, k = lv.group(1), int(a.group(1), 0), 4
    else:
        return None
    if k + 7 > len(ins):
        return None
    arg = re.fullmatch(r"r1," + IMM, squash(ins[k].op_str))
    vo = re.fullmatch(vreg + r",\[" + vreg + r"," + IMM + r"\]", squash(ins[k + 1].op_str))
    rt = re.fullmatch(r"r0," + IMM, squash(ins[k + 3].op_str))
    if not (arg and vo and rt and ins[k + 2].mnemonic == "blx"
            and squash(ins[k + 2].op_str) == vreg and ins[k + 6].mnemonic == "bx"
            and squash(ins[k + 6].op_str) == "lr"):
        return None
    voff = int(vo.group(1), 0)
    if voff % 4 or off % 4:
        return None
    virts = "".join(f"virtual void v{i}(); " for i in range(voff // 4))
    return ("//cpp\n"
            f"struct Base {{ {virts}virtual void m(int); }};\n"
            f"struct Derived {{ char pad[{off:#x}]; Base base; }};\n"
            f'extern "C" int {name}(Derived *d) {{ Base *b = &d->base; b->m({int(arg.group(1), 0)});'
            f" return {int(rt.group(1), 0)}; }}\n"), "virtual_call"


def rule_pmf_call(name, ins, b):
    # C++ pointer-to-member-function call: a PMF-array pointer at this+OFF, the (N/8)th PMF
    #   called on this. The ARM PMF ABI:
    #   ldr rA,[r0,#OFF]; [add r3,rA,#N]; ldr r1,[r3,#4]; add r0,r0,r1,asr#1; ands r1,r1,#1;
    #   ldrne r2,[r0]; ldrne r1,[r3]; ldrne r1,[r2,r1]; ldreq r1,[r3]; blx r1; ...
    if len(ins) < 13 or ins[0].mnemonic != "stmdb" or squash(ins[0].op_str) != "sp!,{lr}" \
            or ins[1].mnemonic != "sub":
        return None
    lo = re.fullmatch(r"(r\d+),\[r0," + IMM + r"\]", squash(ins[2].op_str))
    if ins[2].mnemonic != "ldr" or not lo:
        return None
    off, rA, k, n = int(lo.group(2), 0), lo.group(1), 3, 0
    if ins[3].mnemonic == "add":
        am = re.fullmatch(r"r3," + rA + r"," + IMM, squash(ins[3].op_str))
        if not am:
            return None
        n, k = int(am.group(1), 0), 4
    elif rA != "r3":
        return None
    if k + 10 >= len(ins):
        return None
    canon = [(k, "ldr", "r1,[r3,#4]"), (k + 1, "add", "r0,r0,r1,asr#1"),
             (k + 2, "ands", "r1,r1,#1"), (k + 3, "ldrne", "r2,[r0]"),
             (k + 4, "ldrne", "r1,[r3]"), (k + 5, "ldrne", "r1,[r2,r1]"),
             (k + 6, "ldreq", "r1,[r3]"), (k + 7, "blx", "r1"),
             (k + 8, "add", None), (k + 9, "ldm", "sp!,{lr}"), (k + 10, "bx", "lr")]
    for idx, mn, exp in canon:
        if ins[idx].mnemonic != mn or (exp is not None and squash(ins[idx].op_str) != exp):
            return None
    if n % 8:
        return None
    pexpr = "c->pp" if n == 0 else f"c->pp + {n // 8}"
    return ("//cpp\n"
            "struct C; typedef void (C::*PMF)();\n"
            f"struct C {{ char pad[{off:#x}]; PMF *pp; }};\n"
            f'extern "C" void {name}(C *c) {{ PMF *p = {pexpr}; (c->**p)(); }}\n'), "pmf_call"


def rule_pmf_call_guarded(name, ins, b):
    # PMF passed as arg, stored at this+OFF, null-checked, then called; returns its result (or 0).
    #   str r1,[r0,#OFF]; ldr r3,[r0,#OFF]; ldr r2,[r3]; cmp r2,#0; <return 0 if null>;
    #   ldr r1,[r3,#4]; add r0,r0,r1,asr#1; ands r1,r1,#1; ldrne r1,[r0]; ldrne r1,[r1,r2];
    #   ldreq r1,[r3]; blx r1; ...
    want = ["stmdb", "sub", "str", "ldr", "ldr", "cmp", "addeq", "moveq", "ldmeq", "bxeq",
            "ldr", "add", "ands", "ldrne", "ldrne", "ldreq", "blx", "add", "ldm", "bx"]
    if [i.mnemonic for i in ins[:len(want)]] != want or squash(ins[0].op_str) != "sp!,{lr}":
        return None
    so = re.fullmatch(r"r1,\[r0," + IMM + r"\]", squash(ins[2].op_str))
    lo = re.fullmatch(r"r3,\[r0," + IMM + r"\]", squash(ins[3].op_str))
    if not (so and lo and so.group(1) == lo.group(1)):
        return None
    canon = {4: "r2,[r3]", 5: "r2,#0", 9: "lr", 10: "r1,[r3,#4]",
             11: "r0,r0,r1,asr#1", 12: "r1,r1,#1", 13: "r1,[r0]", 14: "r1,[r1,r2]",
             15: "r1,[r3]", 16: "r1", 19: "lr"}
    nret = re.fullmatch(r"r0," + IMM, squash(ins[7].op_str))      # null-path return value
    if not nret or any(squash(ins[k].op_str) != v for k, v in canon.items()):
        return None
    return ("//cpp\n"
            "struct C; typedef int (C::*PMF)();\n"
            f"struct C {{ char pad[{int(so.group(1), 0):#x}]; PMF *pp; }};\n"
            f'extern "C" int {name}(C *c, PMF *p) {{ c->pp = p; PMF *q = c->pp;'
            f" if (*q == 0) return {int(nret.group(1), 0)}; return (c->**q)(); }}\n"),\
        "pmf_call_guarded"


def rule_pmf_table_call(name, ins, b):
    # PMF dispatch through a global PMF table indexed by a struct field this+OFF.
    #   [str r1,[r0,#OFF];] ldr r1,[r0,#OFF]; ldr r2,[pc](=&TABLE);
    #   add rD,r2,r1,lsl#SH; [add r3,rD,#SUB;]  <PMF call sequence on r3>
    #   -> (c->*TABLE[c->idx].pmf[SUB/8])();  with optional `c->idx = i;` store
    if len(ins) < 13 or ins[0].mnemonic != "stmdb" or squash(ins[0].op_str) != "sp!,{lr}" \
            or ins[1].mnemonic != "sub":
        return None
    i, store = 2, False
    sm = re.fullmatch(r"r1,\[r0," + IMM + r"\]", squash(ins[i].op_str))
    if ins[i].mnemonic == "str" and sm:
        off, store, i = int(sm.group(1), 0), True, i + 1
        lm = re.fullmatch(r"r1,\[r0," + IMM + r"\]", squash(ins[i].op_str))
        if ins[i].mnemonic != "ldr" or not lm or int(lm.group(1), 0) != off:
            return None
        i += 1
    else:
        lm = re.fullmatch(r"r1,\[r0," + IMM + r"\]", squash(ins[i].op_str))
        if ins[i].mnemonic != "ldr" or not lm:
            return None
        off, i = int(lm.group(1), 0), i + 1
    if ins[i].mnemonic != "ldr" or not re.fullmatch(r"r2,\[pc.*\]", squash(ins[i].op_str)):
        return None
    i += 1
    am = re.fullmatch(r"(r\d+),r2,r1,lsl" + IMM, squash(ins[i].op_str))
    if ins[i].mnemonic != "add" or not am:
        return None
    sh, dst, i = int(am.group(2), 0), am.group(1), i + 1
    sub = 0
    if dst != "r3":
        a2 = re.fullmatch(r"r3," + dst + r"," + IMM, squash(ins[i].op_str))
        if ins[i].mnemonic != "add" or not a2:
            return None
        sub, i = int(a2.group(1), 0), i + 1
    if i + 10 >= len(ins):
        return None
    seq = [(i, "ldr", "r1,[r3,#4]"), (i + 1, "add", "r0,r0,r1,asr#1"), (i + 2, "ands", "r1,r1,#1"),
           (i + 3, "ldrne", "r2,[r0]"), (i + 4, "ldrne", "r1,[r3]"), (i + 5, "ldrne", "r1,[r2,r1]"),
           (i + 6, "ldreq", "r1,[r3]"), (i + 7, "blx", "r1"), (i + 10, "bx", "lr")]
    for idx, mn, exp in seq:
        if ins[idx].mnemonic != mn or squash(ins[idx].op_str) != exp:
            return None
    esz = 1 << sh
    if esz % 8 or sub % 8 or sub // 8 >= esz // 8:
        return None
    decl = ("//cpp\n"
            "struct C; typedef void (C::*PMF)();\n"
            f"struct Entry {{ PMF pmf[{esz // 8}]; }};\n"
            "extern Entry TABLE[];\n"
            f"struct C {{ char pad[{off:#x}]; int idx; }};\n")
    call = f"(c->*TABLE[j].pmf[{sub // 8}])();"
    if store:
        body = f'extern "C" void {name}(C *c, int i) {{ c->idx = i; int j = c->idx; {call} }}\n'
    else:
        body = f'extern "C" void {name}(C *c) {{ int j = c->idx; {call} }}\n'
    return decl + body, "pmf_table_call"


def rule_set_fields(name, ins, b):
    # [mov rVi,#ki | add rT,rB,#big]* with <store> rVi,[rB(,#off)] ; [mov r0,#ret] ; bx lr
    #   -> a run of `field = const` stores at mixed widths/values; return ret if present, else void
    if len(ins) < 3:
        return None
    bxj = next((j for j in range(1, len(ins))
                if ins[j].mnemonic == "bx" and squash(ins[j].op_str) == "lr"), None)
    if bxj is None:
        return None
    end, ret = bxj, None
    if ins[bxj - 1].mnemonic == "mov":
        rm = re.fullmatch(r"r0," + IMM, squash(ins[bxj - 1].op_str))
        if rm:
            end, ret = bxj - 1, int(rm.group(1), 0)
    base, regval, fields = {"r0": 0}, {}, []
    for ii in ins[:end]:
        s = squash(ii.op_str)
        if ii.mnemonic == "mov":
            mm = re.fullmatch(r"(r\d+)," + IMM, s)
            if not mm or mm.group(1) == "r0":     # mov r0,#k mid-body would clobber the param
                return None
            regval[mm.group(1)] = int(mm.group(2), 0)
        elif ii.mnemonic == "add":
            am = re.fullmatch(r"(r\d+),(r\d+)," + IMM, s)
            if not am or am.group(2) not in base:
                return None
            base[am.group(1)] = base[am.group(2)] + int(am.group(3), 0)
        elif ii.mnemonic in STORES:
            sm = re.fullmatch(r"(r\d+),\[(r\d+)(?:," + IMM + r")?\]", s)
            if not sm or sm.group(1) not in regval or sm.group(2) not in base:
                return None
            _, esize = STORES[ii.mnemonic]
            off = base[sm.group(2)] + (int(sm.group(3), 0) if sm.group(3) else 0)
            if off % esize:
                return None
            fields.append((off, esize, regval[sm.group(1)]))
        else:
            return None
    if not fields:
        return None
    ty = {4: "int", 2: "short", 1: "char"}
    lines = "".join(f"    *({ty[es]} *)(p + {off:#x}) = {val};\n" for off, es, val in fields)
    rettype = "void" if ret is None else "int"
    body = lines + ("" if ret is None else f"    return {ret};\n")
    return (f"{rettype} {name}(char *p)\n{{\n{body}}}\n"), "set_fields"


def rule_store_const_ret(name, ins, b):
    # mov rV,#k ; <store> rV,[r0(,#off)] ; mov r0,#ret ; bx lr  -> p[idx] = k; return ret;
    if len(ins) == 4 and ins[0].mnemonic == "mov" and ins[1].mnemonic in STORES \
            and ins[2].mnemonic == "mov" and ins[3].mnemonic == "bx" \
            and squash(ins[3].op_str) == "lr":
        mv = re.fullmatch(r"(r\d+)," + IMM, squash(ins[0].op_str))
        rt = re.fullmatch(r"r0," + IMM, squash(ins[2].op_str))
        if mv and rt:
            st = re.fullmatch(mv.group(1) + r",\[r0(?:," + IMM + r")?\]", squash(ins[1].op_str))
            if st:
                ptype, esize = STORES[ins[1].mnemonic]
                off = int(st.group(1), 0) if st.group(1) else 0
                if off % esize == 0:
                    return cfunc(name, "int", f"{ptype} *p",
                                 f"p[{off // esize}] = {int(mv.group(2), 0)}; "
                                 f"return {int(rt.group(1), 0)};"), "store_const_ret"
    return None


def rule_zero_then_global_copy(name, ins, b):
    # mov rZ,#0; ldr rB,[pc]; <store> rZ,[r0,#A]; ldr rX,[rB]; ldr rY,[rB,#4];
    #   str rX,[r0,#D]; str rY,[r0,#D+4]; bx lr   -> *(p+A)=0; *(struct{int,int}*)(p+D)=G;
    if len(ins) < 8 or ins[0].mnemonic != "mov" or ins[1].mnemonic != "ldr":
        return None
    mz = re.fullmatch(r"(r\d+),#0", squash(ins[0].op_str))
    bp = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[1].op_str))
    if not mz or not bp or mz.group(1) == "r0":
        return None
    z, rB = mz.group(1), bp.group(1)
    sz = re.fullmatch(z + r",\[r0," + IMM + r"\]", squash(ins[2].op_str))
    if ins[2].mnemonic not in STORES or not sz:
        return None
    _, zes = STORES[ins[2].mnemonic]
    zoff = int(sz.group(1), 0)
    lx = re.fullmatch(r"(r\d+),\[" + rB + r"\]", squash(ins[3].op_str))
    ly = re.fullmatch(r"(r\d+),\[" + rB + r",#4\]", squash(ins[4].op_str))
    if ins[3].mnemonic != "ldr" or ins[4].mnemonic != "ldr" or not lx or not ly:
        return None
    sx = re.fullmatch(lx.group(1) + r",\[r0," + IMM + r"\]", squash(ins[5].op_str))
    sy = re.fullmatch(ly.group(1) + r",\[r0," + IMM + r"\]", squash(ins[6].op_str))
    if ins[5].mnemonic != "str" or ins[6].mnemonic != "str" or not sx or not sy \
            or ins[7].mnemonic != "bx" or squash(ins[7].op_str) != "lr":
        return None
    d0, d1 = int(sx.group(1), 0), int(sy.group(1), 0)
    ty = {4: "int", 2: "short", 1: "char"}
    if zoff % zes or d0 % 4 or d1 != d0 + 4:
        return None
    return ("struct S { int w[2]; };\nextern struct S G;\n"
            f"void {name}(char *p) {{ *({ty[zes]} *)(p + {zoff:#x}) = 0; "
            f"*(struct S *)(p + {d0:#x}) = G; }}\n"), "zero_then_global_copy"


def rule_global_struct_copy(name, ins, b):
    # ldr rB,[pc]; (ldr rXi,[rB,#i*4]) xN ; (str rXi,[r0,#base+i*4]) xN ; bx lr ; .word &G
    #   -> *(struct{int w[N];}*)(p + base) = G   (copy an N-word global struct into a field)
    if len(ins) < 4 or ins[0].mnemonic != "ldr":
        return None
    b0 = re.fullmatch(r"(r\d+),\[pc.*\]", squash(ins[0].op_str))
    if not b0:
        return None
    rB = b0.group(1)
    end = next((j for j in range(1, len(ins))
                if ins[j].mnemonic == "bx" and squash(ins[j].op_str) == "lr"), None)
    if end is None:
        return None
    mid = ins[1:end]
    if len(mid) < 2 or len(mid) % 2:
        return None
    n = len(mid) // 2
    srcreg = {}
    for k, ld in enumerate(mid[:n]):
        lm = re.fullmatch(r"(r\d+),\[" + rB + r"(?:," + IMM + r")?\]", squash(ld.op_str))
        if ld.mnemonic != "ldr" or not lm:
            return None
        soff = int(lm.group(2), 0) if lm.group(2) else 0
        if soff % 4 or soff // 4 != k:
            return None
        srcreg[lm.group(1)] = k
    base = None
    for k, stt in enumerate(mid[n:]):
        sm = re.fullmatch(r"(r\d+),\[r0(?:," + IMM + r")?\]", squash(stt.op_str))
        if stt.mnemonic != "str" or not sm or srcreg.get(sm.group(1)) != k:
            return None
        doff = int(sm.group(2), 0) if sm.group(2) else 0
        if base is None:
            base = doff
        if base % 4 or doff != base + 4 * k:
            return None
    return (f"struct S {{ int w[{n}]; }};\nextern struct S G;\n"
            f"void {name}(char *p) {{ *(struct S *)(p + {base:#x}) = G; }}\n"), "global_struct_copy"


def rule_new_ctor(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; mov r0,#sz; bl NEW; movs r4,r0; beq END; <obj body>; mov r0,r4; pop; bx; .word VT
    #   -> p = new(sz); if (p) { ...ctors + vtable stores... } return p;
    m = [i.mnemonic for i in ins]
    if m[:1] != ["push"] or m[2:5] != ["bl", "movs", "beq"] or squash(ins[3].op_str) != "r4,r0":
        return None
    # size arg: either `mov r0,#sz` (small) or `ldr r0,[pc,#..]` (pool literal, non-reloc)
    sz = re.fullmatch(r"r0," + IMM, squash(ins[1].op_str))
    if ins[1].mnemonic == "mov" and sz:
        size_val = int(sz.group(1), 0)
    elif ins[1].mnemonic == "ldr":
        lm = re.fullmatch(r"r0,\[pc,#(0x[0-9a-fA-F]+|\d+)\]", squash(ins[1].op_str))
        if not lm:
            return None
        coff = ins[1].address + 8 + int(lm.group(1), 0)
        if relocs.get(addr + coff) is not None or coff + 4 > len(b):
            return None
        size_val = int.from_bytes(b[coff:coff + 4], "little")
    else:
        return None
    e_new = relocs.get(addr + ins[2].address)
    if not e_new:
        return None
    tail = None
    for j in range(5, len(ins) - 2):
        if ins[j].mnemonic == "mov" and squash(ins[j].op_str) == "r0,r4" \
                and ins[j + 1].mnemonic == "pop" and ins[j + 2].mnemonic == "bx" \
                and squash(ins[j + 2].op_str) == "lr":
            tail = j
            break
    if tail is None:
        return None
    parsed = _parse_obj_body(ins[5:tail], addr, relocs, syms)
    if not parsed:
        return None
    ops, vt, g = parsed
    new = R.name_for_reloc(e_new, syms)
    ctors = [o[2] for o in ops if o[0] in ("ctor", "dealloc")]
    if not ctors or name in ([new] + ctors):
        return None
    decls = f"extern void *{new}(unsigned);\n" + _obj_decls(ops, vt, g)
    lines = _emit_obj_ops(ops, "p", "        ")
    return (decls +
            f"int *{name}(void)\n{{\n    int *p = (int *){new}({size_val});\n"
            f"    if (p) {{\n{lines}    }}\n    return p;\n}}\n"), "new_ctor"


def rule_dtor_vtable(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; ...; mov r4,r0; <obj body with >=1 vtable install>; mov r0,r4; pop; bx; .word VT
    #   -> install vtable(s) into this, call dtors on this(+off), return this.
    #   The `mov r4,r0` may sit after hoisted `ldr rX,[pc]` pool loads, so find it by scan.
    if len(ins) < 6 or ins[0].mnemonic != "push":
        return None
    tail = None
    for j in range(2, len(ins) - 2):
        if ins[j].mnemonic == "mov" and squash(ins[j].op_str) == "r0,r4" \
                and ins[j + 1].mnemonic == "pop" and ins[j + 2].mnemonic == "bx" \
                and squash(ins[j + 2].op_str) == "lr":
            tail = j
            break
    if tail is None:
        return None
    mv = next((j for j in range(1, tail) if ins[j].mnemonic == "mov"
               and squash(ins[j].op_str) == "r4,r0"), None)
    if mv is None:
        return None
    parsed = _parse_obj_body([ins[j] for j in range(1, tail) if j != mv], addr, relocs, syms)
    if not parsed:
        return None
    ops, vt, g = parsed
    dtors = [o[2] for o in ops if o[0] in ("ctor", "dealloc")]
    if vt == 0 or name in dtors:        # vt==0 is plain chain_dtor's job
        return None
    lines = _emit_obj_ops(ops, "t", "    ")
    return (_obj_decls(ops, vt, g) +
            f"int *{name}(int *t)\n{{\n{lines}    return t;\n}}\n"), "dtor_vtable"


def rule_frame_call_const(name, ins, b, addr, relocs, syms):
    # stmdb sp!,{lr}; sub sp,sp,#k; [ (ldr r0,[pc])? bl Ci ]+ ; mov r0,#ret;
    #   add sp,sp,#k; ldm sp!,{lr}; bx lr; .word G...
    #   -> call each Ci (on a pool global, or on the incoming this), return constant
    if len(ins) < 6 or ins[0].mnemonic != "stmdb" or squash(ins[0].op_str) != "sp!,{lr}" \
            or ins[1].mnemonic != "sub":
        return None
    subm = re.fullmatch(r"sp,sp," + IMM, squash(ins[1].op_str))
    if not subm:
        return None
    tail = ret = None
    for j in range(2, len(ins) - 3):
        if ins[j].mnemonic == "mov" and ins[j + 1].mnemonic == "add" \
                and ins[j + 2].mnemonic == "ldm" and ins[j + 3].mnemonic == "bx" \
                and squash(ins[j + 3].op_str) == "lr" and squash(ins[j + 2].op_str) == "sp!,{lr}":
            rm = re.fullmatch(r"r0," + IMM, squash(ins[j].op_str))
            am = re.fullmatch(r"sp,sp," + IMM, squash(ins[j + 1].op_str))
            if rm and am and am.group(1) == subm.group(1):
                tail, ret = j, int(rm.group(1), 0)
                break
    if tail is None:
        return None
    calls, gi, pglob, poff = [], 0, False, None
    for ii in ins[2:tail]:
        s = squash(ii.op_str)
        if ii.mnemonic == "ldr" and re.fullmatch(r"r0,\[pc.*\]", s):
            pglob = True
        elif ii.mnemonic == "add" and re.fullmatch(r"r0,r0," + IMM, s):
            poff = int(re.fullmatch(r"r0,r0," + IMM, s).group(1), 0)
        elif ii.mnemonic == "bl":
            e = relocs.get(addr + ii.address)
            if not e:
                return None
            nm = R.name_for_reloc(e, syms)
            if pglob:
                calls.append((nm, "g", gi))
                gi += 1
            elif poff is not None:
                calls.append((nm, "o", poff))
            else:
                calls.append((nm, "t", None))
            pglob, poff = False, None
        else:
            return None
    if not calls or name in (c for c, _, _ in calls):
        return None
    use_this = any(k in ("t", "o") for _, k, _ in calls)

    def _arg(k, v):
        return "t" if k == "t" else (f"(char *)t + {v:#x}" if k == "o" else f"G{v}")
    decls = "".join(f"extern void {c}(void *);\n" for c in dict.fromkeys(c for c, _, _ in calls))
    decls += "".join(f"extern int G{k}[];\n" for k in range(gi))
    lines = "".join(f"    {c}({_arg(k, v)});\n" for c, k, v in calls)
    params = "void *t" if use_this else "void"
    return (decls + f"int {name}({params})\n{{\n{lines}    return {ret};\n}}\n"), "frame_call_const"


def rule_frame_call_bool(name, ins, b, addr, relocs, syms):
    # stmdb sp!,{lr}; sub sp,sp,#k; [ldr r0,[pc] | add r0,r0,#off]? bl X; cmp r0,#0;
    #   movne r0,#1; moveq r0,#0; add sp,sp,#k; ldm sp!,{lr}; bx lr  -> return X(arg) != 0;
    if len(ins) < 9 or ins[0].mnemonic != "stmdb" or squash(ins[0].op_str) != "sp!,{lr}" \
            or ins[1].mnemonic != "sub":
        return None
    subm = re.fullmatch(r"sp,sp," + IMM, squash(ins[1].op_str))
    if not subm:
        return None
    k, arg = 2, ("t", None)
    if ins[k].mnemonic == "ldr" and re.fullmatch(r"r0,\[pc.*\]", squash(ins[k].op_str)):
        arg, k = ("g", 0), k + 1
    elif ins[k].mnemonic == "add" and re.fullmatch(r"r0,r0," + IMM, squash(ins[k].op_str)):
        arg = ("o", int(re.fullmatch(r"r0,r0," + IMM, squash(ins[k].op_str)).group(1), 0))
        k += 1
    seq = [ins[k + i].mnemonic for i in range(7)] if k + 7 <= len(ins) else []
    if seq != ["bl", "cmp", "movne", "moveq", "add", "ldm", "bx"]:
        return None
    if squash(ins[k + 1].op_str) != "r0,#0" or squash(ins[k + 2].op_str) != "r0,#1" \
            or squash(ins[k + 3].op_str) != "r0,#0" or squash(ins[k + 6].op_str) != "lr" \
            or squash(ins[k + 5].op_str) != "sp!,{lr}" \
            or re.fullmatch(r"sp,sp," + IMM, squash(ins[k + 4].op_str)).group(1) != subm.group(1):
        return None
    e = relocs.get(addr + ins[k].address)
    if not e:
        return None
    X = R.name_for_reloc(e, syms)
    if name == X:
        return None
    kind, v = arg
    a = "t" if kind == "t" else (f"(char *)t + {v:#x}" if kind == "o" else "G")
    decls = f"extern int {X}(void *);\n" + ("extern int G[];\n" if kind == "g" else "")
    params = "void" if kind == "g" else "void *t"
    return (decls + f"int {name}({params}) {{ return {X}({a}) != 0; }}\n"), "frame_call_bool"


def rule_deleting_dtor(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; ldr r1,[pc]; mov r4,r0; str r1,[r4]; bl D1; ldr r1,[pc]; mov r0,r4;
    #   ldr r1,[r1]; bl D2; mov r0,r4; pop; bx; .word VT, &HEAP
    #   -> this[0] = &VT; D1(this); D2(this, HEAP); return this;
    m = [i.mnemonic for i in ins]
    if m[:12] != ["push", "ldr", "mov", "str", "bl", "ldr", "mov", "ldr", "bl", "mov", "pop", "bx"]:
        return None
    s = lambda k: squash(ins[k].op_str)
    if not (re.fullmatch(r"r1,\[pc.*\]", s(1)) and s(2) == "r4,r0" and s(3) == "r1,[r4]"
            and re.fullmatch(r"r1,\[pc.*\]", s(5)) and s(6) == "r0,r4"
            and s(7) == "r1,[r1]" and s(9) == "r0,r4" and s(11) == "lr"):
        return None
    e1, e2 = relocs.get(addr + ins[4].address), relocs.get(addr + ins[8].address)
    if not (e1 and e2):
        return None
    d1, d2 = R.name_for_reloc(e1, syms), R.name_for_reloc(e2, syms)
    if d1 == d2 or name in (d1, d2):
        return None
    decls = (f"extern int VT[];\nextern void {d1}(void *);\n"
             f"extern void {d2}(void *, void *);\nextern void *HEAP;\n")
    return (decls + f"int *{name}(int *t)\n{{\n    t[0] = (int)VT;\n    {d1}(t);\n"
            f"    {d2}(t, HEAP);\n    return t;\n}}\n"), "deleting_dtor"


def rule_call_scale_fields(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; mov r4,r0; ldrsh r1,[r4,#A]; add r0,r4,#B (or mov r0,r4); bl F;
    #   (ldr r0,[r4,#Si]; asr r0,r0,#sh; str r0,[r4,#Di]) xN ; pop{r4,lr}; bx lr
    #   -> F(t+B, *(short*)(t+A)); t->Di = t->Si >> sh; ...
    if len(ins) < 6 or ins[0].mnemonic != "push" or ins[1].mnemonic != "mov" \
            or squash(ins[1].op_str) != "r4,r0" or ins[2].mnemonic != "ldrsh":
        return None
    # collect consecutive ldrsh into r1, r2, r3, ... (the call's short args)
    shorts, k = [], 2
    while k < len(ins) and ins[k].mnemonic == "ldrsh":
        lm = re.fullmatch(r"r" + str(len(shorts) + 1) + r",\[r4(?:," + IMM + r")?\]",
                          squash(ins[k].op_str))
        if not lm:
            break
        shorts.append(int(lm.group(1), 0) if lm.group(1) else 0)
        k += 1
    if not shorts:
        return None
    if ins[k].mnemonic == "add":
        am = re.fullmatch(r"r0,r4," + IMM, squash(ins[k].op_str))
        if not am:
            return None
        B = int(am.group(1), 0)
    elif ins[k].mnemonic == "mov" and squash(ins[k].op_str) == "r0,r4":
        B = 0
    else:
        return None
    k += 1
    if k >= len(ins) or ins[k].mnemonic != "bl":
        return None
    e = relocs.get(addr + ins[k].address)
    if not e:
        return None
    F = R.name_for_reloc(e, syms)
    start = k + 1
    end = next((j for j in range(start, len(ins) - 1) if ins[j].mnemonic == "pop"
                and ins[j + 1].mnemonic == "bx" and squash(ins[j + 1].op_str) == "lr"), None)
    if end is None or name == F:
        return None
    body = ins[start:end]
    if not body or len(body) % 3:
        return None
    triples = []
    for k in range(0, len(body), 3):
        ld, sh, st = body[k], body[k + 1], body[k + 2]
        lm = re.fullmatch(r"r0,\[r4(?:," + IMM + r")?\]", squash(ld.op_str))
        sm = re.fullmatch(r"r0,r0," + IMM, squash(sh.op_str))
        tm = re.fullmatch(r"r0,\[r4(?:," + IMM + r")?\]", squash(st.op_str))
        if ld.mnemonic != "ldr" or sh.mnemonic != "asr" or st.mnemonic != "str" \
                or not (lm and sm and tm):
            return None
        triples.append((int(tm.group(1), 0) if tm.group(1) else 0,
                        int(lm.group(1), 0) if lm.group(1) else 0, int(sm.group(1), 0)))
    arg0 = "t" if B == 0 else f"t + {B:#x}"
    shortargs = "".join(f", *(short *)(t + {A:#x})" for A in shorts)
    lines = f"    {F}({arg0}{shortargs});\n"
    for Di, Si, sh in triples:
        lines += f"    *(int *)(t + {Di:#x}) = *(int *)(t + {Si:#x}) >> {sh};\n"
    fparams = "void *" + "".join(", int" for _ in shorts)
    return (f"extern void {F}({fparams});\n"
            f"void {name}(char *t)\n{{\n{lines}}}\n"), "call_scale_fields"


def rule_two_call_wrapper(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; mov r4,r0; bl X; mov r0,r4; [mov r1,#k;] bl Y; pop{r4,lr}; bx lr
    #   -> X(this); Y(this[, k]);   (the second call's int arg is optional)
    if ins[:2] == [] or ins[0].mnemonic != "push" or squash(ins[1].op_str) != "r4,r0" \
            or ins[2].mnemonic != "bl" or squash(ins[3].op_str) != "r0,r4":
        return None
    if ins[4].mnemonic == "mov":                       # X(this); Y(this, k)
        kk = re.fullmatch(r"r1," + IMM, squash(ins[4].op_str))
        yi, arg = 5, kk
    else:                                              # X(this); Y(this)
        kk, yi, arg = None, 4, True
    if not arg or len(ins) < yi + 3 or ins[yi].mnemonic != "bl" \
            or ins[yi + 1].mnemonic != "pop" or squash(ins[yi + 2].op_str) != "lr":
        return None
    e1, e2 = relocs.get(addr + ins[2].address), relocs.get(addr + ins[yi].address)
    if not (e1 and e2):
        return None
    X, Y = R.name_for_reloc(e1, syms), R.name_for_reloc(e2, syms)
    if X == Y or name in (X, Y):
        return None
    if kk:
        return (f"extern void {X}(void *);\nextern void {Y}(void *, int);\n"
                f"void {name}(void *t)\n{{\n    {X}(t);\n    {Y}(t, {int(kk.group(1), 0)});\n}}\n"),\
            "two_call_wrapper"
    return (f"extern void {X}(void *);\nextern void {Y}(void *);\n"
            f"void {name}(void *t)\n{{\n    {X}(t);\n    {Y}(t);\n}}\n"), "two_call_wrapper"


def rule_indexed_inc(name, ins, b, addr, relocs, syms):
    # ldr rB,[pc]; add rB,r0,rB; ldr r0,[rB,r1,lsl#2]; add r0,r0,#1; str r0,[rB,r1,lsl#2]; bx; .word G
    #   -> int *a = (int *)(p + G); a[i] = a[i] + 1;   (G is a non-reloc field offset)
    if [i.mnemonic for i in ins[:6]] != ["ldr", "add", "ldr", "add", "str", "bx"] \
            or squash(ins[5].op_str) != "lr":
        return None
    b0 = re.fullmatch(r"(r\d+),\[pc,#(0x[0-9a-fA-F]+|\d+)\]", squash(ins[0].op_str))
    if not b0:
        return None
    rB = b0.group(1)
    if squash(ins[1].op_str) != f"{rB},r0,{rB}" or squash(ins[2].op_str) != f"r0,[{rB},r1,lsl#2]" \
            or squash(ins[3].op_str) != "r0,r0,#1" or squash(ins[4].op_str) != f"r0,[{rB},r1,lsl#2]":
        return None
    coff = ins[0].address + 8 + int(b0.group(2), 0)
    if relocs.get(addr + coff) is not None or coff + 4 > len(b):
        return None
    G = int.from_bytes(b[coff:coff + 4], "little")
    return (f"void {name}(char *p, int i)\n{{\n    int *a = (int *)(p + {G});\n"
            f"    a[i] = a[i] + 1;\n}}\n"), "indexed_inc"


def rule_frame_call_globals(name, ins, b, addr, relocs, syms):
    # stmdb sp!,{lr}; sub sp,sp,#k; [ (ldr rArg,[pc])+ bl F ]+ ; add sp,sp,#k; ldm sp!,{lr}; bx lr
    #   each F(args) with args loaded from pool: reloc slot -> extern global, else a literal const
    if len(ins) < 5 or ins[0].mnemonic != "stmdb" or squash(ins[0].op_str) != "sp!,{lr}" \
            or ins[1].mnemonic != "sub":
        return None
    subm = re.fullmatch(r"sp,sp," + IMM, squash(ins[1].op_str))
    if not subm:
        return None
    end = None
    for j in range(2, len(ins) - 2):
        if ins[j].mnemonic == "add" and ins[j + 1].mnemonic == "ldm" \
                and ins[j + 2].mnemonic == "bx" and squash(ins[j + 2].op_str) == "lr" \
                and squash(ins[j + 1].op_str) == "sp!,{lr}":
            am = re.fullmatch(r"sp,sp," + IMM, squash(ins[j].op_str))
            if am and am.group(1) == subm.group(1):
                end = j
                break
    if end is None:
        return None
    calls, pending, expect, gmap = [], [], 0, {}
    for ii in ins[2:end]:
        s = squash(ii.op_str)
        lm = re.fullmatch(r"r(\d+),\[pc,#(0x[0-9a-fA-F]+|\d+)\]", s)
        if ii.mnemonic == "ldr" and lm:
            if int(lm.group(1)) != expect:
                return None
            coff = ii.address + 8 + int(lm.group(2), 0)
            if coff + 4 > len(b):
                return None
            r = relocs.get(addr + coff)
            if r is not None:
                key = tuple(r[1:])                 # dedup globals by reloc target identity
                if key not in gmap:
                    gmap[key] = len(gmap)
                pending.append(("g", gmap[key]))
            else:
                w = int.from_bytes(b[coff:coff + 4], "little")
                pending.append(("c", w - (1 << 32) if w >= (1 << 31) else w))
            expect += 1
        elif ii.mnemonic == "bl":
            e = relocs.get(addr + ii.address)
            if not e or not pending:
                return None
            calls.append((R.name_for_reloc(e, syms), pending))
            pending, expect = [], 0
        else:
            return None
    if not calls or pending:
        return None
    fnames = dict.fromkeys(c[0] for c in calls)
    if name in fnames:
        return None
    decls = "".join(f"extern void {f}();\n" for f in fnames)
    decls += "".join(f"extern int G{k}[];\n" for k in range(len(gmap)))
    lines = "".join(
        f"    {f}({', '.join(f'G{a[1]}' if a[0] == 'g' else str(a[1]) for a in args)});\n"
        for f, args in calls)
    return (decls + f"void {name}(void)\n{{\n{lines}}}\n"), "frame_call_globals"


def rule_guarded_call_block(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; mov r4,r0; [arg] bl G; cmp r0,#0; beq END;
    #   ( [arg] bl Ci )* ; END: mov r0,#ret; pop{r4,lr}; bx lr
    #   -> if (G(arg)) { Ci(arg); ... } return ret;  (arg = this / this+off / pool global)
    if len(ins) < 7 or ins[0].mnemonic != "push" or ins[1].mnemonic != "mov" \
            or squash(ins[1].op_str) != "r4,r0":
        return None
    end = ret = None
    for j in range(2, len(ins) - 2):
        if ins[j].mnemonic == "mov" and ins[j + 1].mnemonic == "pop" \
                and ins[j + 2].mnemonic == "bx" and squash(ins[j + 2].op_str) == "lr":
            rm = re.fullmatch(r"r0," + IMM, squash(ins[j].op_str))
            if rm:
                end, ret = j, int(rm.group(1), 0)
                break
    if end is None:
        return None
    body, glob = ins[2:end], [0]

    def arg_at(i):
        if i >= len(body):
            return None
        ii = body[i]
        s = squash(ii.op_str)
        if ii.mnemonic == "add":
            mm = re.fullmatch(r"r0,r4," + IMM, s)
            return (f"(char *)t + {int(mm.group(1), 0):#x}", i + 1) if mm else None
        if ii.mnemonic == "mov" and s == "r0,r4":
            return "t", i + 1
        if ii.mnemonic == "ldr" and re.fullmatch(r"r0,\[pc.*\]", s):
            g = glob[0]
            glob[0] += 1
            return f"G{g}", i + 1
        return "t", i

    ga = arg_at(0)
    if not ga:
        return None
    gexpr, i = ga
    if i >= len(body) or body[i].mnemonic != "bl":
        return None
    ge = relocs.get(addr + body[i].address)
    if not ge:
        return None
    Gname = R.name_for_reloc(ge, syms)
    i += 1
    if i + 1 >= len(body) or body[i].mnemonic != "cmp" or squash(body[i].op_str) != "r0,#0" \
            or body[i + 1].mnemonic != "beq":
        return None
    bt = int(squash(body[i + 1].op_str).lstrip("#"), 0)   # beq target = end of the if-block
    i += 2
    guarded, after = [], []
    while i < len(body):
        aa = arg_at(i)
        if not aa:
            return None
        aexpr, ni = aa
        if ni >= len(body) or body[ni].mnemonic != "bl":
            return None
        e = relocs.get(addr + body[ni].address)
        if not e:
            return None
        (guarded if body[ni].address < bt else after).append((R.name_for_reloc(e, syms), aexpr))
        i = ni + 1
    if not guarded or name in ([Gname] + [c for c, _ in guarded + after]):
        return None
    decls = f"extern int {Gname}(void *);\n"
    for c in dict.fromkeys(c for c, _ in guarded + after):
        if c != Gname:
            decls += f"extern void {c}(void *);\n"
    decls += "".join(f"extern int G{k}[];\n" for k in range(glob[0]))
    gblock = "".join(f"        {c}({a});\n" for c, a in guarded)
    ablock = "".join(f"    {c}({a});\n" for c, a in after)
    return (decls + f"int {name}(void *t)\n{{\n    if ({Gname}({gexpr})) {{\n{gblock}    }}\n"
            f"{ablock}    return {ret};\n}}\n"), "guarded_call_block"


def _parse_method_body(body, addr, relocs, syms):
    # `this` in r4. Ops in instruction order:
    #   mov rV,#k ; <store> rV,[r4,#off]      -> ('set', off, esize, k, 'c')   field = const
    #   ldr rV,[pc] ; <store> rV,[r4,#off]    -> ('set', off, esize, gidx,'g') field = &global
    #   add r0,r4,#off | mov r0,r4 ; bl X     -> ('call', off, X)              X(this+off)
    regval, pending, g, ops = {}, None, 0, []
    for ii in body:
        s, mn = squash(ii.op_str), ii.mnemonic
        if mn == "add":
            mm = re.fullmatch(r"r0,r4," + IMM, s)
            if not mm:
                return None
            pending = int(mm.group(1), 0)
        elif mn == "mov" and s == "r0,r4":
            pending = 0
        elif mn == "mov":
            mm = re.fullmatch(r"(r\d+)," + IMM, s)
            if not mm:
                return None
            regval[mm.group(1)] = ("c", int(mm.group(2), 0))
        elif mn == "ldr" and re.fullmatch(r"(r\d+),\[pc.*\]", s):
            regval[re.fullmatch(r"(r\d+),\[pc.*\]", s).group(1)] = ("g", g)
            g += 1
        elif mn in STORES:
            sm = re.fullmatch(r"(r\d+),\[r4(?:," + IMM + r")?\]", s)
            if not sm or sm.group(1) not in regval:
                return None
            _, esize = STORES[mn]
            off = int(sm.group(2), 0) if sm.group(2) else 0
            if off % esize:
                return None
            kind, v = regval[sm.group(1)]
            ops.append(("set", off, esize, v, kind))
        elif mn == "bl":
            e = relocs.get(addr + ii.address)
            if not e:
                return None
            ops.append(("call", pending or 0, R.name_for_reloc(e, syms)))
            pending = None
        else:
            return None
    return ops, g


def rule_method_body(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; mov r4,r0; <field sets + calls on this(+off)>; (mov r0,#RET | mov r0,r4); pop; bx
    if len(ins) < 5 or ins[0].mnemonic != "push" or ins[1].mnemonic != "mov" \
            or squash(ins[1].op_str) != "r4,r0":
        return None
    tail = ret = None
    retthis = False
    for j in range(2, len(ins) - 2):
        if ins[j].mnemonic == "mov" and ins[j + 1].mnemonic == "pop" \
                and ins[j + 2].mnemonic == "bx" and squash(ins[j + 2].op_str) == "lr":
            rm = re.fullmatch(r"r0," + IMM, squash(ins[j].op_str))
            if rm:
                tail, ret = j, int(rm.group(1), 0)
                break
            if squash(ins[j].op_str) == "r0,r4":
                tail, retthis = j, True
                break
    if tail is None:
        return None
    parsed = _parse_method_body(ins[2:tail], addr, relocs, syms)
    if not parsed:
        return None
    ops, g = parsed
    calls = [o[2] for o in ops if o[0] == "call"]
    if not calls or not any(o[0] == "set" for o in ops) or name in calls:
        return None
    ty = {4: "int", 2: "short", 1: "char"}
    decls = "".join(f"extern void {c}(void *);\n" for c in dict.fromkeys(calls))
    decls += "".join(f"extern int G{k}[];\n" for k in range(g))
    lines = ""
    for op in ops:
        if op[0] == "call":
            arg = "c" if op[1] == 0 else f"(char *)c + {op[1]:#x}"
            lines += f"    {op[2]}({arg});\n"
        else:
            _, off, esize, v, kind = op
            rhs = f"(int)G{v}" if kind == "g" else str(v)
            lines += f"    *({ty[esize]} *)(c + {off:#x}) = {rhs};\n"
    if retthis:
        return decls + f"int *{name}(char *c)\n{{\n{lines}    return (int *)c;\n}}\n", "method_body"
    return decls + f"int {name}(char *c)\n{{\n{lines}    return {ret};\n}}\n", "method_body"


def rule_guard_load_call(name, ins, b, addr, relocs, syms):
    # stmdb sp!,{lr}; sub sp,sp,#k; ldr r0,[r0]; cmp r0,#0; <return if null>;
    #   ldrb r0,[r0,#OFF]; bl F; add sp; ldm; bx   -> p = *c; if (p) F(p[OFF]);
    want = ["stmdb", "sub", "ldr", "cmp", "addeq", "ldmeq", "bxeq", "ldrb", "bl", "add", "ldm", "bx"]
    if [i.mnemonic for i in ins[:12]] != want or squash(ins[0].op_str) != "sp!,{lr}":
        return None
    lb = re.fullmatch(r"r0,\[r0," + IMM + r"\]", squash(ins[7].op_str))
    if squash(ins[2].op_str) != "r0,[r0]" or squash(ins[3].op_str) != "r0,#0" \
            or squash(ins[6].op_str) != "lr" or not lb or squash(ins[11].op_str) != "lr":
        return None
    e = relocs.get(addr + ins[8].address)
    if not e:
        return None
    f = R.name_for_reloc(e, syms)
    if name == f:
        return None
    return (f"extern void {f}(int);\n"
            f"void {name}(char **c) {{ unsigned char *p = (unsigned char *)*c;"
            f" if (p) {f}(p[{int(lb.group(1), 0):#x}]); }}\n"), "guard_load_call"


def rule_fixed5_wrapper(name, ins, b, addr, relocs, syms):
    # stmdb sp!,{lr}; sub sp,sp,#k; mov r2,#0; mov r1,r0; mov r3,r2; mov r0,#K; str r2,[sp];
    #   bl F; add sp; ldm; bx   -> F(K, this, 0, 0, 0);
    want = ["stmdb", "sub", "mov", "mov", "mov", "mov", "str", "bl", "add", "ldm", "bx"]
    if [i.mnemonic for i in ins[:11]] != want or squash(ins[0].op_str) != "sp!,{lr}":
        return None
    k = re.fullmatch(r"r0," + IMM, squash(ins[5].op_str))
    if squash(ins[2].op_str) != "r2,#0" or squash(ins[3].op_str) != "r1,r0" \
            or squash(ins[4].op_str) != "r3,r2" or not k or squash(ins[6].op_str) != "r2,[sp]" \
            or squash(ins[10].op_str) != "lr":
        return None
    e = relocs.get(addr + ins[7].address)
    if not e:
        return None
    f = R.name_for_reloc(e, syms)
    if name == f:
        return None
    return (f"extern void {f}(int, void *, int, int, int);\n"
            f"void {name}(void *c) {{ {f}({int(k.group(1), 0)}, c, 0, 0, 0); }}\n"), "fixed5_wrapper"


def rule_call_then_virtual(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; mov r4,r0; mov r0,r1; bl X; mov r0,r4; ldr r1,[r0]; ldr r1,[r1,#VOFF]; blx r1; pop; bx
    #   -> X(arg1); c->m();   (X is a reloc bl callee; m is a C++ virtual at vtable+VOFF)
    if [i.mnemonic for i in ins[:10]] != ["push", "mov", "mov", "bl", "mov", "ldr", "ldr",
                                          "blx", "pop", "bx"]:
        return None
    vo = re.fullmatch(r"r1,\[r1," + IMM + r"\]", squash(ins[6].op_str))
    if squash(ins[1].op_str) != "r4,r0" or squash(ins[2].op_str) != "r0,r1" \
            or squash(ins[4].op_str) != "r0,r4" or squash(ins[5].op_str) != "r1,[r0]" \
            or not vo or squash(ins[7].op_str) != "r1" or squash(ins[9].op_str) != "lr":
        return None
    e = relocs.get(addr + ins[3].address)
    if not e:
        return None
    x, voff = R.name_for_reloc(e, syms), int(vo.group(1), 0)
    if name == x or voff % 4:
        return None
    virts = "".join(f"virtual void v{i}(); " for i in range(voff // 4))
    return ("//cpp\n"
            f"struct Base {{ {virts}virtual void m(); }};\n"
            f'extern "C" void {x}(int);\n'
            f'extern "C" void {name}(Base *c, int a) {{ {x}(a); c->m(); }}\n'), "call_then_virtual"


def rule_call2_then_virtual(name, ins, b, addr, relocs, syms):
    # push{r4,lr}; mov r4,r0; add r0,r4,#A; add r1,r4,#B; bl X; add r0,r4,#OFF; ldr r2,[r0];
    #   mov r1,#ARG; ldr r2,[r2,#VOFF]; blx r2; mov r0,#RET; pop; bx
    #   -> X(this+A, this+B); Sub* b = &this->sub; b->m(ARG); return RET;
    want = ["push", "mov", "add", "add", "bl", "add", "ldr", "mov", "ldr", "blx", "mov", "pop", "bx"]
    if [i.mnemonic for i in ins[:13]] != want or squash(ins[1].op_str) != "r4,r0":
        return None
    a0 = re.fullmatch(r"r0,r4," + IMM, squash(ins[2].op_str))
    a1 = re.fullmatch(r"r1,r4," + IMM, squash(ins[3].op_str))
    a2 = re.fullmatch(r"r0,r4," + IMM, squash(ins[5].op_str))
    arg = re.fullmatch(r"r1," + IMM, squash(ins[7].op_str))
    vo = re.fullmatch(r"r2,\[r2," + IMM + r"\]", squash(ins[8].op_str))
    rt = re.fullmatch(r"r0," + IMM, squash(ins[10].op_str))
    if not (a0 and a1 and a2 and arg and vo and rt) or squash(ins[6].op_str) != "r2,[r0]" \
            or squash(ins[9].op_str) != "r2" or squash(ins[12].op_str) != "lr":
        return None
    e = relocs.get(addr + ins[4].address)
    if not e:
        return None
    x, voff = R.name_for_reloc(e, syms), int(vo.group(1), 0)
    if name == x or voff % 4:
        return None
    virts = "".join(f"virtual void v{i}(); " for i in range(voff // 4))
    return ("//cpp\n"
            f"struct Sub {{ {virts}virtual void m(int); }};\n"
            f"struct Base {{ char pad[{int(a2.group(1), 0):#x}]; Sub sub; }};\n"
            f'extern "C" void {x}(void *, void *);\n'
            f'extern "C" int {name}(Base *c) {{ {x}((char *)c + {int(a0.group(1), 0):#x}, '
            f"(char *)c + {int(a1.group(1), 0):#x}); Sub *b = &c->sub; b->m({int(arg.group(1), 0)});"
            f" return {int(rt.group(1), 0)}; }}\n"), "call2_then_virtual"


RELOC_RULES = [rule_wrapper, rule_ctor_vtable, rule_chain_dtor, rule_guard_call,
               rule_new_ctor, rule_dtor_vtable, rule_frame_call_const, rule_deleting_dtor,
               rule_call_scale_fields, rule_frame_call_bool, rule_two_call_wrapper,
               rule_indexed_inc, rule_frame_call_globals, rule_guarded_call_block,
               rule_call_then_virtual, rule_call2_then_virtual, rule_method_body,
               rule_guard_load_call, rule_fixed5_wrapper]


RULES = [rule_empty, rule_ret_const, rule_ret_arg, rule_load, rule_load_mask,
         rule_bitfield, rule_store, rule_store_const, rule_arith, rule_add_imm,
         rule_global_getter, rule_global_setter, rule_global_index,
         rule_global_store_const, rule_store_global_addr, rule_field_bitop,
         rule_global_field_bitop, rule_two_global_setters, rule_two_indexed_store,
         rule_setbit_byidx, rule_testbit_byidx, rule_bit_test, rule_cmp_eq,
         rule_struct_copy, rule_pool_const, rule_global_deref,
         rule_global_swap, rule_two_global_store_const, rule_global_bit_test,
         rule_global_struct_copy, rule_store_const_ret, rule_set_fields,
         rule_virtual_call, rule_pmf_call, rule_pmf_call_guarded, rule_pmf_table_call,
         rule_zero_then_global_copy]


# ----------------------------------------------------------------------------- oracle
# C++ sources (for C++-ABI idioms like virtual dispatch) are marked with a leading
# `//cpp` line; they compile with -lang c++ and a .cpp temp file. extern "C" keeps
# the function symbol unmangled so extract_func still finds it by name.
CPP_FLAGS = M.DEFAULT_FLAGS.replace("-lang c99", "-lang c++")


def oracle_ok(c_source, name, target):
    """Compile candidate C/C++ and relocation-aware byte-diff against the ROM."""
    cpp = c_source.startswith("//cpp")
    with tempfile.TemporaryDirectory() as td:
        cfile = pathlib.Path(td) / ("cand.cpp" if cpp else "cand.c")
        cfile.write_text(c_source)
        obj = M.compile_c(cfile, M.CANONICAL, CPP_FLAGS if cpp else M.DEFAULT_FLAGS)
    if obj is None:
        return False
    code, relocs = M.extract_func(obj, name)
    if code is None:
        return False
    ok, _ = M.compare(target, code, relocs, verbose=False)
    return ok


# ----------------------------------------------------------------------------- main
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min", type=lambda x: int(x, 0), default=0x4)
    ap.add_argument("--max", type=lambda x: int(x, 0), default=0x40)
    ap.add_argument("--count", type=int, default=0, help="0 = all unmatched in range")
    ap.add_argument("--mode", default="arm", choices=["arm", "thumb", "any"])
    ap.add_argument("--apply", action="store_true", help="append wins to matched.jsonl")
    ap.add_argument("--verbose", action="store_true", help="print each win's C")
    args = ap.parse_args()

    targets = enumerate_targets(args.min, args.max, args.mode, args.count)
    relocs, syms = R.load_relocs(), R.load_all_syms()
    print(f"scanning {len(targets)} unmatched {args.mode} functions "
          f"(size 0x{args.min:x}-0x{args.max:x})\n")

    stats = {}
    wins = []
    proposed = 0
    for t in targets:
        tgt = M.target_bytes(t["addr"], t["size"])
        ins = list(md.disasm(tgt, 0))
        if is_thunk(ins):
            stats["(thunk skipped)"] = stats.get("(thunk skipped)", 0) + 1
            continue
        cand = None
        for rule in RULES:
            cand = rule(t["name"], ins, tgt)
            if cand:
                break
        if not cand:
            for rule in RELOC_RULES:
                cand = rule(t["name"], ins, tgt, t["addr"], relocs, syms)
                if cand:
                    break
        if not cand:
            stats["(no template)"] = stats.get("(no template)", 0) + 1
            continue
        proposed += 1
        c_source, label = cand
        if oracle_ok(c_source, t["name"], tgt):
            stats[label] = stats.get(label, 0) + 1
            wins.append((t, label, c_source))
            if args.verbose:
                print(f"  MATCH {t['name']} @ 0x{t['addr']:08x}  [{label}]  {c_source.strip()}")
        else:
            stats["(template miss)"] = stats.get("(template miss)", 0) + 1

    written_src = 0
    if args.apply and wins:
        src_dir = REPO / "src"
        with MATCHED.open("a") as f:
            for t, label, c_source in wins:
                f.write(json.dumps({
                    "addr": f"0x{t['addr']:08x}", "name": t["name"],
                    "size": t["size"], "module": "arm9",
                    "versions": [f"template:{label}"],
                }) + "\n")
                cpath = src_dir / f"{t['name']}.c"
                if not cpath.exists():
                    cpath.write_text(pretty(c_source))
                    written_src += 1

    print("=" * 52)
    matched = len(wins)
    print(f"templated candidates: {proposed}   |   MATCHED (zero tokens): {matched}")
    print("-" * 52)
    for label in sorted(stats, key=lambda k: -stats[k]):
        print(f"  {stats[label]:5}  {label}")
    if args.apply:
        print(f"\napplied: appended {matched} to matched.jsonl, wrote {written_src} src/*.c")
    elif matched:
        print("\n(dry-run: re-run with --apply to record these in matched.jsonl)")


if __name__ == "__main__":
    main()
