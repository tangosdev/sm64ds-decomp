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
  python port/tools/port_evidence.py --ratchet     # exit 1 only if debt GREW
  python port/tools/port_evidence.py --require     # missing evidence = failure

Exit codes: 0 passed, 1 FAILED, 2 could not check (no ROM build artifacts, the
build on record is not byte-exact, or no baseline). 2 is NOT a pass -- see the
CannotCheck docstring below.
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

# Same three-state exit contract as port_linkcheck.py:
#   0  checked, passed        1  checked, FAILED        2  could not check
OK, FAILED, CANNOT_CHECK = 0, 1, 2


class CannotCheck(Exception):
    """The ROM-build evidence this tool reads is absent or not byte-exact.

    Deliberately NOT the same outcome as a regression. --ratchet exiting 1
    because build/rombuild-report.json is missing is indistinguishable, to a
    hook or a CI job, from exiting 1 because someone compiled an unproven file
    into the port -- and the two demand opposite responses. One means "re-run
    the ROM build"; the other means "stop the merge". Conflating them trains
    people to ignore the gate, which costs more than the gate ever saved.
    """

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
    """(enrolled keys, {relpath: reason}, report).

    Raises CannotCheck when the evidence is unavailable -- never sys.exit, so
    the caller can distinguish "could not check" from "found a regression".
    """
    objs = BUILD / "objects.txt"
    elig = BUILD / "rombuild-eligibility.json"
    rep = BUILD / "rombuild-report.json"
    for p in (objs, elig, rep):
        if not p.exists():
            raise CannotCheck(f"missing {p.relative_to(REPO)} -- run the ROM "
                              "build first (the port's evidence comes from it)")

    report = json.loads(rep.read_text(encoding="utf-8", errors="replace"))
    # Enrollment only means something if the build actually came out exact.
    an = report.get("analysis", {})
    fid = an.get("moduleFidelity", {})
    if not (report.get("status") == "passed" and an.get("passed")
            and fid.get("differingBytes") == 0):
        raise CannotCheck("the ROM build on record is NOT byte-exact "
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


def enrolled_key(rel):
    """The key load_rom_evidence() stores, for a repo-relative source path.

    Enrollment keys come from build/src/<...>.o and KEEP their subdirectory --
    'engine/fader/_ZN15FaderBrightness7IsAtEndEv'. This used to be compared
    against Path(rel).stem, which can never contain a slash, so no file under a
    src/ SUBDIRECTORY could ever be classified proven. 138 of the 9,149
    enrolled objects live in subdirectories, and it put 7 byte-exact
    FaderBrightness files into the unproven debt ledger.

    Returns None for anything not under src/ (port/unmatched/, generated
    sources), which is then simply not eligible for PROVEN. Deliberately NOT
    falling back to the bare stem: a bare name can collide with an unrelated
    top-level object and report a file as proven on another file's evidence.
    No such collision exists today -- the point is that it cannot appear later.
    """
    rel = rel.replace("\\", "/")
    if not rel.startswith("src/"):
        return None
    return rel[len("src/"):].rsplit(".", 1)[0]


def classify(rel, enrolled, reasons):
    if rel.startswith("port/unmatched/"):
        return REPLACED
    p = REPO / rel
    if not p.exists():
        return UNKNOWN
    if "NONMATCHING" in p.read_text(encoding="utf-8", errors="replace"):
        return BANNER
    key = enrolled_key(rel)
    if key is not None and key in enrolled:
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
                    help="exit 1 if any UNPROVEN or BANNER file is compiled. "
                         "Composes with --ratchet: both must pass")
    ap.add_argument("--ratchet", action="store_true",
                    help="exit 1 only if the unproven set GREW vs the baseline")
    ap.add_argument("--update-baseline", action="store_true",
                    help="rewrite the baseline to the current set")
    ap.add_argument("--require", action="store_true",
                    help="treat missing/non-exact ROM evidence as a FAILURE "
                         "(exit 1) instead of 'could not check' (exit 2)")
    args = ap.parse_args()

    # --gate narrows the file set BEFORE the unproven set is computed, so
    # `--update-baseline --gate 9` would rewrite the ledger to one gate's files
    # and silently drop the rest. It fails loudly but much later -- as a
    # REGRESSION on the next full --ratchet, blamed on whoever runs it.
    if args.gate and (args.ratchet or args.update_baseline):
        sys.exit("--gate cannot be combined with --ratchet or "
                 "--update-baseline: both operate on the whole ledger, and a "
                 "narrowed run would compare against (or write) a partial one.")

    try:
        enrolled, reasons, report = load_rom_evidence()
    except CannotCheck as e:
        print(f"port-evidence: NOT CHECKED -- {e}")
        print("               This did NOT pass; it did not run.")
        if args.require:
            print("               --require was given, so a skip is a failure.")
            return FAILED
        return CANNOT_CHECK
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
        return OK

    if args.ratchet:
        # A boolean gate is the wrong shape here: there are already unproven
        # files, so --strict would be red from day one and get ignored or
        # bypassed. The repo's own merge gate is a set of "must not regress"
        # ratchets (notes/pr-validation.md); this matches that.
        # No baseline is "could not check" too: there is nothing to compare
        # against, which is not the same claim as "the debt grew".
        if not BASELINE.exists():
            print(f"\nport-evidence: NOT CHECKED -- no baseline at "
                  f"{BASELINE.relative_to(REPO)}; run --update-baseline to "
                  "record the current debt")
            return FAILED if args.require else CANNOT_CHECK
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
            return FAILED
        print(f"\nratchet OK: no new unproven files "
              f"({len(current)} known, baseline {len(known)})")
        # --strict used to be dead whenever --ratchet was passed: every path in
        # this block returned, and the strict check sits below it. The flag was
        # not rejected and not warned about, it just did nothing -- the worst of
        # the three options, since the caller reads the exit code as though both
        # gates ran. They are not in conflict: --strict is the absolute floor,
        # --ratchet the derivative, and "did not regress AND never above zero"
        # is coherent -- it becomes the natural gate once the ledger empties.
        # So fall through and let the stricter one also have its say.
        if not args.strict:
            return OK

    # Note: --update-baseline returns above without consulting --strict. That
    # one is deliberate -- it writes the ledger rather than gating on it.
    if args.strict and worst:
        # Say so explicitly. With --ratchet --strict the last thing printed is
        # "ratchet OK", and exiting 1 straight after that reads as a bug in the
        # tool rather than a verdict from the other gate.
        print(f"\nport-evidence: FAILED --strict: {worst} file(s) compiled into "
              f"the port are UNPROVEN or NONMATCHING-bannered.")
        return FAILED
    return OK


if __name__ == "__main__":
    sys.exit(main())
