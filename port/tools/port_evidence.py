#!/usr/bin/env python3
"""How much of what the PC port compiles is PROVEN to be the game's code?

A gate smoke proves the host BEHAVES: 455 models render, an actor lives its
lifecycle. It does not prove the code producing that behaviour is the original
logic. A src/ file existing is not evidence; neither is a smoke passing. The
strongest evidence available is enrollment in a byte-exact ROM build -- if the
ROM comes out bit-identical with that object compiled from our source, that
source IS the game's logic.

This joins the port's slice manifests against the ROM build and reports, per
gate, how much is proven and what the rest is.

WHAT "PROVEN" DOES AND DOES NOT MEAN
    It means: this SOURCE, compiled by mwccarm for ARM, reproduces the retail
    bytes. That is a statement about the source's logic.
    It does NOT mean: the host build of it behaves identically. The port
    compiles the same text with MSVC for x86-32 -- different codegen,
    different ABI, host-supplied seams. Proven source is a necessary
    condition for a faithful port, not a sufficient one.

Buckets:
    proven      enrolled in the byte-exact ROM build
    explained   excluded for a structural reason the port expects to own
                (unresolvable extern -> HAL supplies it, extra .data/.bss
                sections -> HAL storage, lives in .init)
    UNPROVEN    cleared eligibility and still did not make the ROM -- no
                reason recorded, no proof it is the game's code
    BANNER      carries a NONMATCHING marker in the source itself
    replaced    port/unmatched/ -- a deliberate logically-correct-but-
                unmatched implementation, not a claim of fidelity
    unknown     not in the eligibility list at all

Usage:
  python port/tools/port_evidence.py               # per-gate summary
  python port/tools/port_evidence.py --gate 9      # one gate, with files
  python port/tools/port_evidence.py --list        # every non-proven file
  python port/tools/port_evidence.py --strict      # exit 1 if UNPROVEN/BANNER
"""
import argparse
import json
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
PORT = REPO / "port"
SRC = REPO / "src"
BUILD = REPO / "build"
BASELINE = PORT / "evidence-baseline.json"

PROVEN, EXPLAINED, UNPROVEN, BANNER, REPLACED, UNKNOWN = (
    "proven", "explained", "UNPROVEN", "BANNER", "replaced", "unknown")
ORDER = [PROVEN, EXPLAINED, UNPROVEN, BANNER, REPLACED, UNKNOWN]

# The gate ledger from port/README.md, so `--gate 4b` works the way the docs
# and commit messages talk about the port.
GATE_ALIAS = {
    "1": "smoke",          "2": "smoke_heap",     "3a": "smoke_roots",
    "3b": "smoke_fs",      "4a": "smoke_gx",      "4b": "smoke_model",
    "4c": "smoke_anim",    "4d": "smoke_soak",    "5": "smoke_frames",
    "5b": "smoke_soak_anim", "6": "smoke_oam",    "7": "smoke_modelanim",
    "8": "smoke_clsn",     "9": "smoke_actor",
}

# Exclusion reasons the port legitimately owns: the HAL supplies the symbol or
# the storage on host, so absence from the ROM link says nothing bad about the
# source. Anything NOT matching these is not silently forgiven.
EXPLAINED_RE = re.compile(
    r"^(unresolvable:|extra sections:|lives in \.init)", re.I)


def load_rom_evidence():
    """(enrolled stems, {relpath: reason}, report) -- or exit with why not."""
    objs = BUILD / "objects.txt"
    elig = BUILD / "rombuild-eligibility.json"
    rep = BUILD / "rombuild-report.json"
    for p in (objs, elig, rep):
        if not p.exists():
            sys.exit(f"missing {p.relative_to(REPO)} -- run the ROM build first "
                     "(the port's evidence comes from it)")

    report = json.loads(rep.read_text(encoding="utf-8", errors="replace"))
    # Enrollment only means something if the build actually came out exact.
    an = report.get("analysis", {})
    fid = an.get("moduleFidelity", {})
    if not (report.get("status") == "passed" and an.get("passed")
            and fid.get("differingBytes") == 0):
        sys.exit("the ROM build on record is NOT byte-exact "
                 f"(status={report.get('status')}, "
                 f"differingBytes={fid.get('differingBytes')}) -- "
                 "enrollment proves nothing until it is")

    enrolled = set()
    for line in objs.read_text(encoding="utf-8", errors="replace").splitlines():
        m = re.search(r"/build/src/(.+)\.o$", line.strip().replace("\\", "/"))
        if m:
            enrolled.add(m.group(1))

    reasons = {}
    for row in json.loads(elig.read_text(encoding="utf-8", errors="replace")):
        reasons[row["file"].replace("\\", "/")] = row.get("reason")
    return enrolled, reasons, report


def gates():
    """{gate label: [repo-relative source paths]} from the CMake targets.

    Reads the real build description rather than a hand-kept list, so a gate
    that grows its slice is reflected here without anyone remembering to.
    """
    cml = (PORT / "CMakeLists.txt").read_text(encoding="utf-8", errors="replace")

    slices = {}          # SLICE2_SOURCES -> [paths]
    for f in sorted(PORT.glob("slice_gate*.txt")):
        n = f.stem.replace("slice_gate", "").upper()
        got = []
        for line in f.read_text(encoding="utf-8", errors="replace").splitlines():
            line = line.strip().replace("\\", "/")
            if line and not line.startswith("#"):
                got.append(line)
        slices[f"SLICE{n}_SOURCES"] = got
        # Gate 1 predates the numbered convention and is plain SLICE_SOURCES.
        # Without this alias the gate-1 target silently reports zero files and
        # drops out of the table entirely -- the quietest way for an evidence
        # tool to lie.
        if n == "1":
            slices["SLICE_SOURCES"] = got

    hostgen = {}         # GATE4A_GEN -> [paths]
    for m in re.finditer(r"set\((GATE\w*)_SYMS\s+(.*?)\)\s*\n", cml, re.S):
        got = []
        for s in m.group(2).split():
            for ext in (".c", ".cpp"):
                if (SRC / f"{s}{ext}").exists():
                    got.append(f"src/{s}{ext}")
                    break
        hostgen[m.group(1) + "_GEN"] = got

    out = {}
    for m in re.finditer(r"add_executable\(\s*(\w+)(.*?)\)\s*\n", cml, re.S):
        target, body = m.group(1), m.group(2)
        files = []
        for var in re.findall(r"\$\{(\w+)\}", body):
            files += slices.get(var, hostgen.get(var, []))
        if files:
            out[target] = sorted(set(files))
    return out


def classify(rel, enrolled, reasons):
    if rel.startswith("port/unmatched/"):
        return REPLACED
    p = REPO / rel
    if not p.exists():
        return UNKNOWN
    if "NONMATCHING" in p.read_text(encoding="utf-8", errors="replace"):
        return BANNER
    if p.stem in enrolled:
        return PROVEN
    if rel not in reasons:
        return UNKNOWN
    reason = reasons[rel]
    if reason is None:
        return UNPROVEN
    return EXPLAINED if EXPLAINED_RE.match(str(reason)) else UNPROVEN


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--gate", help="show one target (e.g. 9, smoke_actor)")
    ap.add_argument("--list", action="store_true", help="list every non-proven file")
    ap.add_argument("--strict", action="store_true",
                    help="exit 1 if any UNPROVEN or BANNER file is compiled")
    ap.add_argument("--ratchet", action="store_true",
                    help="exit 1 only if the unproven set GREW vs the baseline")
    ap.add_argument("--update-baseline", action="store_true",
                    help="rewrite the baseline to the current set")
    args = ap.parse_args()

    enrolled, reasons, report = load_rom_evidence()
    sha = report.get("romArtifact", {}).get("sha256", "?")
    print(f"evidence: byte-exact ROM build, {report['enrolledFiles']:,} files "
          f"enrolled, sha256 {sha[:16]}...")
    print(f"          {report['analysis']['moduleFidelity']['modulesExact']}"
          f"/{report['analysis']['moduleFidelity']['modulesChecked']} modules "
          f"exact, 0 differing bytes\n")

    g = gates()
    if args.gate:
        key = args.gate.lower()
        want = args.gate if args.gate in g else GATE_ALIAS.get(key)
        if want is None:
            cand = [k for k in g if key in k.lower()]
            if len(cand) != 1:
                sys.exit(f"--gate {args.gate!r} matched {cand or 'nothing'}\n"
                         f"  gates:   {', '.join(sorted(GATE_ALIAS))}\n"
                         f"  targets: {', '.join(sorted(g))}")
            want = cand[0]
        if want not in g:
            sys.exit(f"target {want!r} is not built by port/CMakeLists.txt")
        g = {want: g[want]}

    rows = []
    everything = {}
    cache = {}
    for target, files in sorted(g.items()):
        counts = dict.fromkeys(ORDER, 0)
        for rel in files:
            b = cache.get(rel) or cache.setdefault(
                rel, classify(rel, enrolled, reasons))
            counts[b] += 1
            everything.setdefault(b, set()).add(rel)
        n = len(files)
        pct = 100.0 * counts[PROVEN] / n if n else 0.0
        rows.append((target, n, pct, counts))
    # UNIQUE files, not the sum down the column: most sources appear in many
    # targets, so summing would report a number several times the real one.
    worst = len(everything.get(UNPROVEN, ())) + len(everything.get(BANNER, ()))

    w = max(len(r[0]) for r in rows)
    print(f"{'target':<{w}}  {'files':>5} {'proven':>7}  "
          f"{'expl':>4} {'UNPROV':>6} {'BANNER':>6} {'repl':>4} {'unk':>4}")
    for target, n, pct, c in rows:
        print(f"{target:<{w}}  {n:>5} {pct:>6.1f}%  {c[EXPLAINED]:>4} "
              f"{c[UNPROVEN]:>6} {c[BANNER]:>6} {c[REPLACED]:>4} {c[UNKNOWN]:>4}")

    if args.gate or args.list:
        for b in (UNPROVEN, BANNER, UNKNOWN, EXPLAINED, REPLACED):
            items = sorted(everything.get(b, ()))
            if not items:
                continue
            print(f"\n--- {b} ({len(items)}) ---")
            for rel in items:
                note = reasons.get(rel)
                print(f"  {rel}" + (f"\n      reason: {note}" if note else ""))

    if worst:
        print(f"\n{worst} file(s) compiled into the port are UNPROVEN or "
              f"NONMATCHING-bannered.")
        print("A passing smoke does not cover this: it proves host behaviour, "
              "not that the code is the game's.")

    current = sorted(everything.get(UNPROVEN, set()) | everything.get(BANNER, set()))
    if args.update_baseline:
        BASELINE.write_text(json.dumps({
            "comment": "Files compiled into the port with no proof they are the "
                       "game's code. This is a DEBT LEDGER, not a target -- "
                       "--ratchet fails when it grows. Shrink it by matching the "
                       "function, not by editing this file.",
            "unproven": current,
        }, indent=2) + "\n", encoding="utf-8")
        print(f"\nbaseline updated: {len(current)} file(s) -> "
              f"{BASELINE.relative_to(REPO)}")
        return 0

    if args.ratchet:
        # A boolean gate is the wrong shape here: there are already unproven
        # files, so --strict would be red from day one and get ignored or
        # bypassed. The repo's own merge gate is a set of "must not regress"
        # ratchets (notes/pr-validation.md); this matches that.
        if not BASELINE.exists():
            print(f"\nno baseline at {BASELINE.relative_to(REPO)} -- "
                  "run --update-baseline to record the current debt")
            return 1
        known = set(json.loads(BASELINE.read_text(encoding="utf-8"))["unproven"])
        added = [f for f in current if f not in known]
        removed = sorted(known - set(current))
        if removed:
            print(f"\n{len(removed)} file(s) left the unproven set -- "
                  f"run --update-baseline to bank it:")
            for f in removed:
                print(f"  - {f}")
        if added:
            print(f"\nREGRESSION: {len(added)} file(s) newly compiled into the "
                  f"port without proof they are the game's code:")
            for f in added:
                print(f"  + {f}")
            return 1
        print(f"\nratchet OK: no new unproven files "
              f"({len(current)} known, baseline {len(known)})")
        return 0

    if args.strict and worst:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
