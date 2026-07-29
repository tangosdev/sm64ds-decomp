"""Deterministic (zero-model) enrichment of a function's trace + nearmiss record.

Produces the structured facts that a cheap model pass then turns into a human
annotation (proposed signature / types / name / behavior). Everything here is
computed, not guessed:
  - per-arg profile (r0-r3): stable pointer? varies? zero? RAM ptr? I/O register?
    Fix12-looking? distinct observed values
  - callers seen at runtime (from lr resolution)
  - callees (BL targets disassembled from target_hex, resolved to symbols)
  - deref'd memory for pointer args, shown as word grids (reveals struct layout)
"""
import json
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import symindex  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parents[2]
RAM = (0x02000000, 0x02400000)
IO = (0x04000000, 0x05000000)


def _db_record(name):
    for line in (REPO / "nearmiss" / "db.jsonl").read_text(encoding="utf-8").splitlines():
        if not line.strip():
            continue
        r = json.loads(line)
        if r["name"] == name:
            return r
    return None


def _callees(target_hex, base):
    """Disassemble ARM BL (0xEB......) sites from the function bytes."""
    b = bytes.fromhex(target_hex)
    syms = symindex.get()
    out = []
    for off in range(0, len(b) - 3, 4):
        word = int.from_bytes(b[off:off + 4], "little")
        if (word >> 24) == 0xEB:  # unconditional BL
            imm = word & 0xFFFFFF
            if imm & 0x800000:
                imm -= 0x1000000
            tgt = (base + off + 8 + (imm << 2)) & 0xFFFFFFFF
            out.append((tgt, syms.resolve(tgt)))
    # de-dup, keep order
    seen, uniq = set(), []
    for t, n in out:
        if t not in seen:
            seen.add(t)
            uniq.append({"addr": f"{t:#010x}", "name": n})
    return uniq


def _classify(vals):
    nz = [v for v in vals]
    prof = {"distinct": sorted(set(f"{v:#010x}" for v in vals))[:6]}
    prof["stable"] = len(set(vals)) == 1
    prof["all_zero"] = all(v == 0 for v in vals)
    prof["ram_ptr"] = all(RAM[0] <= v < RAM[1] for v in vals) and not prof["all_zero"]
    prof["io_reg"] = all(IO[0] <= v < IO[1] for v in vals) and not prof["all_zero"]
    nzvals = [v for v in nz if v]
    prof["fix12_whole"] = bool(nzvals) and all(v % 4096 == 0 for v in nzvals)
    return prof


def _words(hexstr):
    b = bytes.fromhex(hexstr)
    return [int.from_bytes(b[i:i + 4], "little") for i in range(0, len(b) - 3, 4)]


def enrich(name, traces_dir=None):
    traces_dir = pathlib.Path(traces_dir or (REPO / "traces"))
    rec = _db_record(name)
    if rec is None:
        raise SystemExit(f"{name} not in nearmiss DB")
    module = rec.get("module", "arm9")
    tfile = traces_dir / module / f"{name}.jsonl"
    hits = [json.loads(l) for l in tfile.read_text(encoding="utf-8").splitlines() if l.strip()]
    if not hits:
        raise SystemExit(f"no trace hits for {name}")

    args = {}
    for i in range(4):
        vals = [h["regs"][f"r{i}"] for h in hits]
        args[f"r{i}"] = _classify(vals)

    # deref word-grids for pointer args (first hit that has each deref)
    derefs = {}
    for i in range(4):
        for h in hits:
            d = h.get("derefs", {}).get(f"r{i}")
            if d:
                derefs[f"r{i}"] = [f"{w:#010x}" for w in _words(d)][:16]
                break

    callers = sorted({h["caller"] for h in hits if h.get("caller")})
    base = int(str(rec["addr"]), 16)
    return {
        "name": name,
        "module": module,
        "addr": rec["addr"],
        "size": rec["size"],
        "divergences": rec["divergences"],
        "n_hits": len(hits),
        "arg_profiles": args,
        "arg_derefs": derefs,
        "callers": callers,
        "callees": _callees(rec["target_hex"], base),
        "draft_c_source": rec["c_source"],
    }


if __name__ == "__main__":
    print(json.dumps(enrich(sys.argv[1]), indent=2))
