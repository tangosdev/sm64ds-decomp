"""Show what a candidate destructor emits against the ROM, word by word.

build_pin.verify reports `999 word(s) differ` for ANY size mismatch -- that is
match.compare's sentinel, not a count -- so a candidate that is one instruction
short and one that is unrelated look identical from there. This prints the two
sizes and the disassembly side by side so the difference is readable.
"""
import argparse
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import build_pin as BP  # noqa: E402
import match as M       # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func")
    ap.add_argument("addr")
    ap.add_argument("size")
    ap.add_argument("label")
    args = ap.parse_args()

    addr, size = int(args.addr, 16), int(args.size, 16)
    src = pathlib.Path(args.src)
    version, why = BP.compiler_for(src, args.func)
    if version is None:
        print("no pin:", why)
        return
    tgt = BP.target_bytes(args.label, addr, size)
    obj = M.compile_c(src, version, BP.flags_for(src))
    if obj is None:
        print(f"does not compile under {version}")
        return
    code, relocs = M.extract_func(obj, args.func)
    if code is None:
        print(f"{args.func} is not in the object {version} produced")
        return
    print(f"pin {version}   ROM 0x{len(tgt):x}   candidate 0x{len(code):x}"
          f"   delta {len(code) - len(tgt):+d} bytes\n")
    n = max(len(tgt), len(code)) // 4
    for i in range(n):
        o = i * 4
        tw = tgt[o:o + 4] if o < len(tgt) else b""
        cw = code[o:o + 4] if o < len(code) else b""
        ti = next(M.md.disasm(tw, 0), None) if tw else None
        ci = next(M.md.disasm(cw, 0), None) if cw else None
        ts = f"{ti.mnemonic} {ti.op_str}" if ti else (tw.hex() or "--")
        cs = f"{ci.mnemonic} {ci.op_str}" if ci else (cw.hex() or "--")
        if o in relocs:
            tag = "reloc (wildcard)"
        elif tw == cw:
            tag = ""
        else:
            tag = "<<< MISMATCH"
        print(f" +0x{o:02x} | {ts:34} | {cs:34} | {tag}")


if __name__ == "__main__":
    main()
