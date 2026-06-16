"""Size-tolerant block diff for cracking a big function incrementally.

fdiff.py needs the candidate to be the exact size before it can show a
per-instruction diff (the byte compare is positional). While building a large
function up, the size is usually wrong, so this aligns the two instruction
streams by sequence-matching their mnemonic/operand text (branch targets and
pc-relative offsets normalized, so relocations do not count as differences) and
prints where they agree, where an instruction was changed, and where the
candidate has an extra or missing instruction.

That lets you reconstruct block by block: see exactly which basic block first
diverges, fix it, and watch the aligned region grow, instead of guessing why the
whole function is the wrong length.

Usage:
    python tools/falign.py --c cand.c --name FUNC --module ovNN --addr 0x.. --size 0x..
    python tools/falign.py --c cand.c --name FUNC --target-hex <hex>
"""
import argparse
import difflib
import pathlib
import re
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match as M
import swarm as S
import modules as MOD

PCREL = re.compile(r"\[pc,\s*#-?(?:0x[0-9a-fA-F]+|[0-9]+)\]")


def norm(ins):
    """Instruction text for alignment: branch targets dropped and pc-relative
    offsets blanked (those are relocations / pool positions), everything else
    kept so real differences (offsets, constants, registers) still show."""
    if ins.mnemonic in ("bl", "blx", "b", "beq", "bne", "bgt", "blt", "bge",
                         "ble", "bhi", "bls", "bcs", "bcc", "bmi", "bpl"):
        # keep the conditional mnemonic, drop the target address
        return ins.mnemonic
    return ins.mnemonic + " " + PCREL.sub("[pc]", S.squash(ins.op_str))


def disasm_norm(code):
    return [norm(i) for i in S.md.disasm(code, 0)], list(S.md.disasm(code, 0))


def target_from_args(a):
    if a.target_hex:
        return bytes.fromhex(a.target_hex)
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        if label == a.module:
            data = mod["bin"].read_bytes()
            return data[a.addr - mod["base"]:a.addr - mod["base"] + a.size]
    raise SystemExit(f"module {a.module} not found")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--c", required=True)
    ap.add_argument("--name", required=True)
    ap.add_argument("--target-hex", default=None)
    ap.add_argument("--module", default=None)
    ap.add_argument("--addr", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--size", type=lambda x: int(x, 0), default=None)
    a = ap.parse_args()

    target = target_from_args(a)
    src = pathlib.Path(a.c).read_text(encoding="utf-8")
    cpp = src.startswith("//cpp")
    with tempfile.TemporaryDirectory() as td:
        cf = pathlib.Path(td) / ("cand.cpp" if cpp else "cand.c")
        cf.write_text(src, encoding="utf-8")
        obj = M.compile_c(cf, M.CANONICAL, S.CPP_FLAGS if cpp else M.DEFAULT_FLAGS)
    if obj is None:
        print("compile-error"); return
    code, _ = M.extract_func(obj, a.name)
    if code is None:
        print(f"no-symbol:{a.name}"); return

    tnorm, tins = disasm_norm(target)
    cnorm, cins = disasm_norm(code)
    sm = difflib.SequenceMatcher(a=tnorm, b=cnorm, autojunk=False)
    print(f"target {len(tnorm)} insns | candidate {len(cnorm)} insns | "
          f"ratio {sm.ratio():.3f}")
    eq = rep = ins_ = dele = 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            eq += i2 - i1
            print(f"  == [{i1:3}-{i2:3}] {i2-i1} insns match")
            continue
        if tag == "replace":
            rep += max(i2 - i1, j2 - j1)
        elif tag == "delete":
            dele += i2 - i1
        elif tag == "insert":
            ins_ += j2 - j1
        print(f"  {tag.upper()} target[{i1}:{i2}] vs cand[{j1}:{j2}]")
        for k in range(i1, i2):
            print(f"      target {k:3}: {tnorm[k]}")
        for k in range(j1, j2):
            print(f"      cand   {k:3}: {cnorm[k]}")
    print(f"SUMMARY equal={eq} replace={rep} insert(extra in cand)={ins_} "
          f"delete(missing in cand)={dele}")


if __name__ == "__main__":
    main()
