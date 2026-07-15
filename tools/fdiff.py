"""Iteration oracle for cracking ONE hard function: compile a candidate and show
the per-instruction byte diff against the ROM, with relocation slots wildcarded.

Unlike swarm.oracle_ok (pass/fail), this localizes exactly which instructions
differ, so a model can converge on a big function instead of guessing blind.

Usage:
    python tools/fdiff.py --c cand.c --name FUNC --target-hex <hex>
    python tools/fdiff.py --c cand.c --name FUNC --module ov002 --addr 0x.. --size 0x..
Prints each word: offset | target insn | candidate insn | OK/MISMATCH/reloc, then
a summary line "RESULT match=<bool> mismatches=<n>/<words>".
"""
import argparse
import difflib
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match as M
import swarm as S
import modules as MOD


PCREL = re.compile(r"\[pc,\s*#-?(?:0x[0-9a-fA-F]+|[0-9]+)\]")
STACK = re.compile(r"\[sp(?:,\s*#-?(?:0x[0-9a-fA-F]+|[0-9]+))?\]")
REG = re.compile(r"\b(?:r(?:1[0-2]|[0-9])|fp|ip|sb|sl|lr)\b")


def norm(ins):
    if ins.mnemonic.startswith("b"):
        return ins.mnemonic
    return ins.mnemonic + " " + PCREL.sub("[pc]", S.squash(ins.op_str))


def shape(ins):
    if ins.mnemonic.startswith("b"):
        return ins.mnemonic
    operands = S.squash(ins.op_str)
    operands = PCREL.sub("[pc]", operands)
    operands = STACK.sub("[sp]", operands)
    operands = REG.sub("r", operands)
    return ins.mnemonic + " " + operands


def target_from_args(a):
    if a.target_hex:
        return bytes.fromhex(a.target_hex)
    # Default to the main module when none is given: an arm9 target only carries addr/size, so
    # requiring --module for it just produced the cryptic "module None not found". Overlays always
    # pass ovNNN explicitly, so this only affects the main binary.
    want = a.module or "arm9"
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        if label == want:
            data = mod["bin"].read_bytes()
            return data[a.addr - mod["base"]:a.addr - mod["base"] + a.size]
    raise SystemExit(f"module {want} not found")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--c", required=True)
    ap.add_argument("--name", required=True)
    ap.add_argument("--target-hex", default=None)
    ap.add_argument("--module", default=None)
    ap.add_argument("--addr", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--size", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--quiet", action="store_true", help="summary line only")
    ap.add_argument("--common-prefix", action="store_true",
                    help="diff only the shared byte prefix when sizes differ")
    ap.add_argument("--limit", type=lambda x: int(x, 0), default=None,
                    help="limit the verbose comparison to this many bytes")
    ap.add_argument("--align", action="store_true",
                    help="show size-tolerant normalized instruction alignment")
    ap.add_argument("--align-mnemonic", action="store_true",
                    help="align only by mnemonic, ignoring operands")
    ap.add_argument("--align-shape", action="store_true",
                    help="align while ignoring register names and stack offsets")
    ap.add_argument("--align-changes", type=int, default=40,
                    help="maximum non-equal alignment ranges to print")
    ap.add_argument("--find-candidate", default=None,
                    help="print candidate instructions matching this regex")
    ap.add_argument("--candidate-start", type=lambda x: int(x, 0), default=None,
                    help="dump candidate disassembly starting at this byte offset")
    ap.add_argument("--candidate-length", type=lambda x: int(x, 0), default=0x100,
                    help="candidate disassembly byte count")
    ap.add_argument("--track", default=None,
                    help="checkpoint prefix: keep <prefix>.best.c at the lowest "
                         "mismatch count ever seen, so a near-miss is never lost")
    a = ap.parse_args()

    target = target_from_args(a)
    src = pathlib.Path(a.c).read_text(encoding="utf-8")
    cpp = src.startswith("//cpp")
    import tempfile
    with tempfile.TemporaryDirectory() as td:
        cf = pathlib.Path(td) / ("cand.cpp" if cpp else "cand.c")
        cf.write_text(src, encoding="utf-8")
        obj = M.compile_c(cf, M.CANONICAL, S.CPP_FLAGS if cpp else M.DEFAULT_FLAGS)
    if obj is None:
        print("RESULT match=False mismatches=compile-error")
        return
    code, relocs = M.extract_func(obj, a.name)
    if code is None:
        print(f"RESULT match=False mismatches=no-symbol:{a.name}")
        return
    if a.find_candidate:
        pattern = re.compile(a.find_candidate)
        for index, ins in enumerate(S.md.disasm(code, 0)):
            rendered = f"{ins.mnemonic} {ins.op_str}"
            if pattern.search(rendered):
                print(f"CAND {index:4} +0x{index * 4:04x}: {rendered}")
    if a.candidate_start is not None:
        begin = a.candidate_start
        end = min(len(code), begin + a.candidate_length)
        for ins in S.md.disasm(code[begin:end], begin):
            print(f"CAND +0x{ins.address:04x}: {ins.mnemonic:<8} {ins.op_str}")
    if a.align:
        tins = list(S.md.disasm(target, 0))
        cins = list(S.md.disasm(code, 0))
        if a.align_mnemonic:
            tnorm = [ins.mnemonic for ins in tins]
            cnorm = [ins.mnemonic for ins in cins]
        elif a.align_shape:
            tnorm = [shape(ins) for ins in tins]
            cnorm = [shape(ins) for ins in cins]
        else:
            tnorm = [norm(ins) for ins in tins]
            cnorm = [norm(ins) for ins in cins]
        sm = difflib.SequenceMatcher(a=tnorm, b=cnorm, autojunk=False)
        counts = {"equal": 0, "replace": 0, "insert": 0, "delete": 0}
        shown = 0
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            counts[tag] += max(i2 - i1, j2 - j1)
            if tag == "equal" or shown >= a.align_changes:
                continue
            print(f"{tag.upper()} target[{i1}:{i2}] cand[{j1}:{j2}]")
            for value in tnorm[i1:min(i2, i1 + 4)]:
                print(f"  T {value}")
            for value in cnorm[j1:min(j2, j1 + 4)]:
                print(f"  C {value}")
            shown += 1
        print(f"ALIGN target={len(tnorm)} cand={len(cnorm)} "
              f"ratio={sm.ratio():.4f} counts={counts}")
    compare_target = target
    compare_code = code
    if a.common_prefix and len(target) != len(code):
        common = min(len(target), len(code))
        compare_target = target[:common]
        compare_code = code[:common]
    if a.limit is not None:
        compare_target = compare_target[:a.limit]
        compare_code = compare_code[:a.limit]
    ok, ndiff = M.compare(compare_target, compare_code, relocs,
                          verbose=not a.quiet)
    words = max(len(target), len(code)) // 4
    print(f"RESULT match={ok} mismatches={ndiff}/{words}")

    if a.track:
        score_p = pathlib.Path(a.track + ".best.score")
        best_p = pathlib.Path(a.track + ".best.c")
        prev = int(score_p.read_text()) if score_p.exists() else 10 ** 9
        if ndiff < prev:
            best_p.write_text(src, encoding="utf-8")
            score_p.write_text(str(ndiff))
            print(f"NEW BEST {ndiff} (was {prev if prev < 10**9 else 'none'}) -> {best_p}")
        else:
            print(f"not improved (best so far {prev})")


if __name__ == "__main__":
    main()
