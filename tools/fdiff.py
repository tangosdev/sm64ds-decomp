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
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match as M
import swarm as S
import modules as MOD


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
    ap.add_argument("--quiet", action="store_true", help="summary line only")
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
    ok, ndiff = M.compare(target, code, relocs, verbose=not a.quiet)
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
