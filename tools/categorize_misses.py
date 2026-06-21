"""Categorize every near-miss in the DB by WHY it misses, so we can target the most
common (and most fixable) reasons. Heuristic classifier over the reloc-aware asm diff."""
import json
import pathlib
import re
import sys
import difflib
import tempfile
import os

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import match as M
import swarm as S

REG = re.compile(r"\b(r\d+|sb|sl|fp|ip|sp|lr|pc)\b")
COND = ("eq", "ne", "lt", "gt", "ge", "le", "mi", "pl", "cs", "cc", "hi", "ls", "vs", "vc")


def disasm(code, relocs):
    out = []
    for ins in S.code_insns(list(S.md.disasm(code, 0))):
        if ins.address in relocs:
            out.append(("reloc", "reloc"))
        else:
            op = re.sub(r"\[pc, #-?0x[0-9a-fA-F]+\]", "[pc,#pool]", ins.op_str)
            out.append((ins.mnemonic, f"{ins.mnemonic} {op}".rstrip()))
    return out


def skel(row):
    return REG.sub("#r", row)


def is_pred(mn):
    return any(mn.endswith(c) and mn not in ("b" + c,) for c in COND) and mn != "b"


def compile_dis(src, name):
    cpp = src.startswith("//cpp")
    fd, tmp = tempfile.mkstemp(suffix=".cpp" if cpp else ".c")
    os.close(fd)
    pathlib.Path(tmp).write_text(src)
    try:
        obj = M.compile_c(pathlib.Path(tmp), M.CANONICAL, S.CPP_FLAGS if cpp else M.DEFAULT_FLAGS)
    finally:
        pathlib.Path(tmp).unlink(missing_ok=True)
    if obj is None:
        return None, None
    return M.extract_func(obj, name)


def classify_entry(src, name, target):
    cand, crel = compile_dis(src, name)
    if cand is None:
        return "won't-compile"
    crel = crel or set()
    c = disasm(cand, crel)
    t = disasm(target, crel)
    if len(c) != len(t):
        size_gap = abs(len(c) - len(t))
    else:
        size_gap = 0
    def pair_cat(a, b):
        """classify one aligned candidate/ROM instruction pair (same position)."""
        am, arow = a
        bm, brow = b
        if arow == brow:
            return None
        if is_pred(am) != is_pred(bm) or am.startswith("b") != bm.startswith("b"):
            return "predicate vs branch"
        if am != bm:
            return "different op / idiom"
        # same mnemonic -> look at operands
        # addressing: one uses "[reg, #imm]" the other "[reg]" or "[reg, reg]"
        aa = bool(re.search(r"\[\w+, #", arow)); ba = bool(re.search(r"\[\w+, #", brow))
        areg = bool(re.search(r"\[\w+\]|\[\w+, \w+\]", arow)); breg = bool(re.search(r"\[\w+\]|\[\w+, \w+\]", brow))
        if (aa and breg) or (ba and areg):
            return "base materialization / addressing"
        if skel(arow) == skel(brow):
            return "register allocation"
        # same mnemonic+skeleton-shape but a constant immediate differs
        if re.sub(r"#-?0x[0-9a-fA-F]+|#-?\d+", "#K", arow) == re.sub(r"#-?0x[0-9a-fA-F]+|#-?\d+", "#K", brow):
            return "constant / value"
        return "different op / idiom"

    cats = {}
    sm = difflib.SequenceMatcher(a=[x[1] for x in c], b=[x[1] for x in t], autojunk=False)
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op == "equal":
            continue
        cs = [c[k] for k in range(i1, i2)]
        ts = [t[k] for k in range(j1, j2)]
        cm = [x[0] for x in cs]
        tm = [x[0] for x in ts]
        n = max(len(cs), len(ts))
        rom_addbase = any(re.match(r"add \w+, \w+, #", x[1]) for x in ts)
        cand_addbase = any(re.match(r"add \w+, \w+, #", x[1]) for x in cs)
        if any(m in ("push", "pop", "stmdb", "ldm", "ldmia") for m in cm + tm) and cm != tm:
            cat = "push-set / frame"
        elif rom_addbase and not cand_addbase and any("[" in x[1] for x in cs):
            # ROM computes a base address into a register; candidate uses direct [base,#imm]
            cat = "base materialization / addressing"
        elif op == "insert":
            cat = "missing logic (ROM does more)"
        elif op == "delete":
            cat = "extra logic (you do more)"
        elif sorted(x[1] for x in cs) == sorted(x[1] for x in ts):
            cat = "instruction reorder"
        elif len(cs) == len(ts):
            # aligned 1:1 -> classify each pair, take the most common
            pcs = [pair_cat(a, b) for a, b in zip(cs, ts)]
            pcs = [p for p in pcs if p]
            cat = max(set(pcs), key=pcs.count) if pcs else "register allocation"
        else:
            cat = "different op / idiom"
        cats[cat] = cats.get(cat, 0) + n
    if not cats:
        return "matches-now"
    primary = max(cats.items(), key=lambda kv: kv[1])[0]
    if size_gap and primary in ("base materialization", "register allocation", "instruction reorder"):
        # a size gap usually means a real structural difference dominates
        pass
    return primary


def main():
    db = [json.loads(l) for l in (REPO / "nearmiss" / "db.jsonl").read_text().splitlines() if l.strip()]
    from collections import Counter, defaultdict
    cat = Counter()
    examples = defaultdict(list)
    by_div = defaultdict(list)
    for r in db:
        try:
            c = classify_entry(r["c_source"], r["name"], bytes.fromhex(r["target_hex"]))
        except Exception as e:
            c = "error"
        cat[c] += 1
        if len(examples[c]) < 3:
            examples[c].append((r["name"], r.get("divergences")))
        by_div[c].append(r.get("divergences") or 0)
    FIX = {
        "predicate vs branch": "YES -- && chains / if(p){body} / guard-clause structure",
        "instruction reorder": "OFTEN -- reorder statements to disasm order; permuter helps",
        "register allocation": "OFTEN -- struct types + permuter (perm_struct_ref)",
        "missing logic (ROM does more)": "YES -- add the missing operation",
        "extra logic (you do more)": "YES -- remove the redundant statement",
        "different op / idiom": "YES -- match the exact mul/shift/op the disasm shows",
        "constant / value": "YES -- a wrong immediate/offset; fix the number",
        "push-set / frame": "PARTLY -- restructure values-live-across-calls",
        "base materialization / addressing": "HARD -- compiler addressing quirk; permuter can't, needs exact struct types or future model",
        "won't-compile": "YES -- fix the C so it compiles",
        "matches-now": "BANK IT (re-check with oracle)",
        "error": "-- investigate",
    }
    print(f"\n{'REASON':32} {'COUNT':>5} {'med-div':>7}  FIXABLE")
    print("-" * 92)
    for c, n in cat.most_common():
        ds = sorted(by_div[c]); med = ds[len(ds) // 2] if ds else 0
        print(f"{c:32} {n:>5} {med:>7}  {FIX.get(c,'?')}")
    print(f"\n{'total':32} {sum(cat.values()):>5}")
    print("\nexamples per reason:")
    for c, _ in cat.most_common():
        ex = ", ".join(f"{n[:30]}(d{d})" for n, d in examples[c])
        print(f"  {c}: {ex}")


if __name__ == "__main__":
    main()
