#!/usr/bin/env python3
"""Does the tree agree with what the profile reconstruction campaign claims?

Two independent scans, both against `config/**/symbols.txt` -- the file the ROM
build actually consumes -- and neither of them covered by any gate that existed
before this one.

SCAN 1, THE REGISTRY. `symbols/profile_reconstruction_registry.tsv` is the
campaign's dataset: one row per ROM actor/process profile, naming the descriptor
global, the construction factory, the RTTI class, and the NSMBW-convention
filename the factory's source is meant to end up on. Per in-scope row this
asserts three things:

  1. the descriptor symbol carries its proposed `g_profile_<ID>` name
  2. the factory symbol carries its proposed `<Class>_classInit[_<ID>]` name
  3. the factory's standalone source sits on its `factory_filename`

Rows whose `overlay_resolution` starts with `superseded_by_` are out of scope:
the campaign resolved them to a different overlay's row and this one is a
duplicate reading of the same profile.

WHY 3 HAS AN EXEMPTION, AND WHY IT IS MEASURED. A factory that has been folded
into a promoted translation unit has no standalone one-function source to
rename, so there is no filename for it to be wrong about. That is not a list of
known TUs -- lists go stale. The owning source is read out of the module's
`delinks.txt`, and the entry is called folded when MORE THAN ONE function symbol
falls inside the range that entry claims. One function means a standalone
source, which must be on `factory_filename`; several means a TU, which is
exempt. `daPropeller_Heyho_c.cpp` owns 0x0211f000..0x02120570 in ov070 and the
factory at 0x02120520 is one of many functions inside it -- detected, not
hardcoded.

PENDING IS NOT A FAILURE, YET. Waves of this campaign are still landing, so a
row whose symbol still carries its pre-campaign coined spelling reports PENDING
and does not fail the build. `--strict` turns every pending row into a failure;
that is the switch to flip when the campaign closes, and it is what makes
"pending" a decision someone made rather than a silence.

SCAN 2, THE COINED LEDGER ROWS -- and this is the blindspot the tool exists for.
`tools/check_rename_ledger.py` checks `_ZN...` and `_ZTV...` rows only, and says
so; the coined spellings this campaign produces (`g_profile_BAR`,
`Foo_c_classInit`) have no counterpart it knows how to resolve, so about a
thousand ledger rows are unchecked by it. That is not a theoretical gap. The row

    ov070  0x02120520  FlyGuy_Spawn  daPropeller_Heyho_c_classInit

sat on main asserting a name that `config/arm9/overlays/ov070/symbols.txt`, the
function definition and the manifest all still spelled `FlyGuy_Spawn`. A ledger
row claiming a symbol the build never emitted, green, with nothing watching it.

The ledger is an append log, so a row is not checked in isolation: for each
(module, address) the LAST row's fourth column is the live claim and every
earlier row on that address is superseded history. Only the live claim is
resolved against symbols.txt, and only when it is a coined spelling -- the
mangled ones already belong to `check_rename_ledger.py` and are left to it.

RATCHET, NOT A SWEEP. `config/profile-campaign-baseline.json` banks the
divergences that exist today, each with the pair actually observed, so a banked
entry stops matching the moment either side changes. The job fails on a
divergence that is not banked, which is the shape a fresh drift makes. Banked
entries are counted on every run so a green cannot be read as a clean tree.

IT CANNOT PASS BY DOING NOTHING. The scan counts what it actually resolved and
exits non-zero with SCAN TOO SMALL below a floor rather than announcing a pass.
This repo has already shipped a gate that reported a pass after a reformat took
its match count to zero; that is the failure this guard refuses to repeat.

DEGRADES HONESTLY. The registry dataset is not on main yet (PR #2235). When it
is absent, or present without the columns these checks need, scan 1 reports
exactly what it could not read and is skipped -- it never passes vacuously --
while scan 2, which needs only files that are on main, still runs. `--strict`
refuses a missing dataset outright.
"""
import argparse
import collections
import io
import json
import os
import re
import sys

REGISTRY = "symbols/profile_reconstruction_registry.tsv"
LEDGER = "symbols/actor_renames.tsv"
BASELINE = "config/profile-campaign-baseline.json"

SYM_RE = re.compile(r"\s*(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-f]+)")
RANGE_RE = re.compile(r"start:(0x[0-9a-f]+)\s+end:(0x[0-9a-f]+)")

# Floors. Far below today's numbers (752 live coined ledger claims, 25 symbol
# files, 497 registry rows); they trip when a reader breaks, not when the
# campaign progresses.
MIN_SYMBOL_FILES = 15
MIN_LEDGER_CLAIMS = 300
MIN_REGISTRY_ROWS = 150

REGISTRY_COLUMNS = (
    "overlay_resolution", "overlay", "profile_address",
    "current_profile_name", "proposed_profile_name",
    "factory_module", "factory_address",
    "current_factory_name", "proposed_factory_name",
    "factory_filename",
)


# --------------------------------------------------------------------------
# readers


def symbols_path(repo, module):
    """Module name -> its symbols.txt, or None.

    arm9's file is `config/arm9/symbols.txt`; every overlay's is
    `config/arm9/overlays/<mod>/symbols.txt`. There is no `main/` subdirectory
    under `config/arm9`, which is the spelling that keeps getting written down.
    """
    for rel in ("config/arm9/overlays/%s/symbols.txt" % module,
                "config/%s/symbols.txt" % module):
        p = os.path.join(repo, rel)
        if os.path.exists(p):
            return p
    return None


def read_symbols(repo, module, cache):
    """-> {address: [names]} for one module, or None when it has no file.

    Aliased addresses are real here (a descriptor can carry both a coined and a
    cartridge spelling), so the value is a list and membership, not equality, is
    what the checks ask.
    """
    if module in cache:
        return cache[module]
    path = symbols_path(repo, module)
    table = None
    if path:
        table = collections.defaultdict(list)
        with io.open(path, encoding="utf-8", errors="replace") as fh:
            for line in fh:
                m = SYM_RE.match(line)
                if m:
                    table[int(m.group(3), 16)].append(m.group(1))
    cache[module] = table
    return table


def read_delinks(repo, module, cache):
    """-> [(source_path, start, end)] over a module's .text entries.

    A source path is the unindented line ending in ':'; the ranges that follow
    are its. The bare section header (`.text start:... kind:code align:32`) is
    unindented but carries no path, so entries before the first path line are
    dropped rather than attributed to it.
    """
    if module in cache:
        return cache[module]
    entries = []
    for rel in ("config/arm9/overlays/%s/delinks.txt" % module,
                "config/%s/delinks.txt" % module):
        p = os.path.join(repo, rel)
        if not os.path.exists(p):
            continue
        current = None
        with io.open(p, encoding="utf-8", errors="replace") as fh:
            for line in fh:
                stripped = line.rstrip("\n").rstrip("\r")
                if stripped and not stripped[0].isspace() and stripped.endswith(":"):
                    current = stripped[:-1]
                    continue
                if current is None:
                    continue
                m = RANGE_RE.search(stripped)
                if m and ".text" in stripped:
                    entries.append((current, int(m.group(1), 16), int(m.group(2), 16)))
        break
    cache[module] = entries
    return entries


def split_ledger_lines(raw):
    """Tolerate mixed endings: split on LF, drop a trailing CR.

    Splitting on a single detected terminator merges an LF row into the previous
    CRLF row's last column, which hides the merged row from every check below.
    """
    return [ln[:-1] if ln.endswith("\r") else ln for ln in raw.split("\n")]


def is_mangled(name):
    return name.startswith("_ZN") or name.startswith("_ZTV") or name.startswith("_ZTI") \
        or name.startswith("_ZTS")


def load_baseline(repo):
    p = os.path.join(repo, BASELINE)
    if not os.path.exists(p):
        return {"_comment": "", "known": []}
    with io.open(p, encoding="utf-8") as fh:
        return json.load(fh)


def baseline_keys(data):
    """Banked entries as a set of comparable tuples.

    The observed pair is part of the key on purpose: rebanking is not a way to
    silence an address forever, only the exact divergence someone looked at.
    """
    out = set()
    for e in data.get("known", []):
        out.add((e.get("module"), e.get("address"), e.get("claimed"),
                 tuple(e.get("actual") or [])))
    return out


# --------------------------------------------------------------------------
# scan 1: the registry


def owning_source(repo, module, address, delinks_cache, symbol_table):
    """-> (source_path, function_symbols_in_that_range) or (None, 0).

    The count is what separates a standalone one-function source from a promoted
    TU; see the module docstring.
    """
    for path, start, end in read_delinks(repo, module, delinks_cache):
        if start <= address < end:
            n = 0
            if symbol_table:
                for addr in symbol_table:
                    if start <= addr < end:
                        n += 1
            return path, n
    return None, 0


def symbol_state(table, address, current, proposed):
    """COMPLETE / PENDING / DIVERGED for one symbol.

    DIVERGED covers both "the address is not in symbols.txt at all" and "it
    carries something that is neither the pre-campaign name nor the proposed
    one", because both mean the dataset and the tree disagree about a name and
    neither is a state a wave leaves behind.
    """
    if table is None:
        return "DIVERGED", "module has no symbols.txt"
    names = table.get(address)
    if not names:
        return "DIVERGED", "address not in symbols.txt"
    if proposed and proposed in names:
        return "COMPLETE", None
    if current and current in names:
        return "PENDING", None
    return "DIVERGED", "carries %s" % ", ".join(sorted(names))


def scan_registry(repo, sym_cache, delinks_cache):
    """-> (report dict, failures, pendings).

    report['status'] is one of ok / absent / incomplete, and the caller decides
    what that means; this function never turns a dataset it could not read into
    a pass.
    """
    report = {"status": "ok", "rows": 0, "scope": 0, "superseded": 0,
              "complete": 0, "pending": 0, "exempt": 0, "missing_columns": []}
    failures = []
    pendings = []

    path = os.path.join(repo, REGISTRY)
    if not os.path.exists(path):
        report["status"] = "absent"
        return report, failures, pendings

    import csv
    with io.open(path, encoding="utf-8", errors="replace", newline="") as fh:
        rows = list(csv.DictReader(fh, delimiter="\t"))
    report["rows"] = len(rows)
    if not rows:
        report["status"] = "incomplete"
        report["missing_columns"] = list(REGISTRY_COLUMNS)
        return report, failures, pendings

    have = set(rows[0].keys())
    missing = [c for c in REGISTRY_COLUMNS if c not in have]
    if missing:
        report["status"] = "incomplete"
        report["missing_columns"] = missing
        return report, failures, pendings

    for r in rows:
        if (r.get("overlay_resolution") or "").startswith("superseded_by_"):
            report["superseded"] += 1
            continue
        pid = r.get("profile_id") or r.get("actor_id") or "?"
        report["scope"] += 1

        # 1. descriptor
        mod = r.get("overlay") or "arm9"
        if r.get("proposed_profile_name") and r.get("profile_address"):
            table = read_symbols(repo, mod, sym_cache)
            try:
                addr = int(r["profile_address"], 16)
            except ValueError:
                failures.append(("descriptor address unreadable", pid, mod,
                                 r["profile_address"]))
                addr = None
            if addr is not None:
                state, why = symbol_state(table, addr, r.get("current_profile_name"),
                                          r["proposed_profile_name"])
                if state == "COMPLETE":
                    report["complete"] += 1
                elif state == "PENDING":
                    report["pending"] += 1
                    pendings.append(("descriptor", pid, mod, r["profile_address"],
                                     r["proposed_profile_name"]))
                else:
                    failures.append(("descriptor symbol diverged", pid, mod,
                                     "%s: want %s, %s" % (r["profile_address"],
                                                          r["proposed_profile_name"], why)))

        # 2. factory symbol, and 3. its filename
        fmod = r.get("factory_module") or mod
        if r.get("proposed_factory_name") and r.get("factory_address"):
            table = read_symbols(repo, fmod, sym_cache)
            try:
                faddr = int(r["factory_address"], 16)
            except ValueError:
                failures.append(("factory address unreadable", pid, fmod,
                                 r["factory_address"]))
                faddr = None
            if faddr is not None:
                state, why = symbol_state(table, faddr, r.get("current_factory_name"),
                                          r["proposed_factory_name"])
                if state == "COMPLETE":
                    report["complete"] += 1
                elif state == "PENDING":
                    report["pending"] += 1
                    pendings.append(("factory", pid, fmod, r["factory_address"],
                                     r["proposed_factory_name"]))
                else:
                    failures.append(("factory symbol diverged", pid, fmod,
                                     "%s: want %s, %s" % (r["factory_address"],
                                                          r["proposed_factory_name"], why)))

                want = r.get("factory_filename")
                if want:
                    src, nfuncs = owning_source(repo, fmod, faddr, delinks_cache, table)
                    if src is None:
                        failures.append(("factory source unlocatable", pid, fmod,
                                         "%s owned by no delinks entry" % r["factory_address"]))
                    elif nfuncs > 1:
                        report["exempt"] += 1
                    elif os.path.basename(src) == want:
                        report["complete"] += 1
                    else:
                        report["pending"] += 1
                        pendings.append(("filename", pid, fmod, src, want))

    return report, failures, pendings


# --------------------------------------------------------------------------
# scan 2: coined ledger rows


def scan_ledger(repo, sym_cache):
    """-> (claims_checked, divergences).

    divergence: (module, '0x...', claimed, [actual names], ledger line number)
    """
    path = os.path.join(repo, LEDGER)
    if not os.path.exists(path):
        return 0, []
    with io.open(path, "rb") as fh:
        raw = fh.read().decode("utf-8", errors="replace")

    live = collections.OrderedDict()
    for i, line in enumerate(split_ledger_lines(raw), 1):
        parts = line.split("\t")
        if len(parts) < 4:
            continue
        if i == 1 and parts[0] == "module":
            continue
        module, addr_text, claimed = parts[0], parts[1], parts[3]
        try:
            addr = int(addr_text, 16)
        except ValueError:
            continue
        # append log: the last row on an address is the live claim, every
        # earlier one is superseded history
        live[(module, addr)] = (i, addr_text, claimed)

    checked = 0
    diverged = []
    for (module, addr), (lineno, addr_text, claimed) in live.items():
        if is_mangled(claimed):
            continue  # check_rename_ledger.py owns these
        table = read_symbols(repo, module, sym_cache)
        if table is None:
            diverged.append((module, addr_text, claimed, [], lineno))
            continue
        checked += 1
        names = table.get(addr) or []
        if claimed not in names:
            diverged.append((module, addr_text, claimed, sorted(names), lineno))
    return checked, diverged


# --------------------------------------------------------------------------


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--repo", default=".", help="repository root (default: cwd)")
    ap.add_argument("--strict", action="store_true",
                    help="fail on any PENDING row, and on a missing dataset -- "
                         "the mode to flip on when the campaign closes")
    ap.add_argument("--list", action="store_true", help="print every finding, not a sample")
    ap.add_argument("--update", action="store_true",
                    help="rewrite %s from what is diverged today" % BASELINE)
    args = ap.parse_args(argv)
    repo = args.repo

    sym_cache = {}
    delinks_cache = {}

    reg, reg_failures, pendings = scan_registry(repo, sym_cache, delinks_cache)
    claims, diverged = scan_ledger(repo, sym_cache)

    # the count of files actually READ, not the count that exist -- a floor on
    # what the scan resolved is only honest if it measures the scan
    n_symbol_files = sum(1 for v in sym_cache.values() if v)

    base = load_baseline(repo)
    banked = baseline_keys(base)

    if args.update:
        entries = []
        for module, addr_text, claimed, actual, lineno in diverged:
            entries.append({"module": module, "address": addr_text, "claimed": claimed,
                            "actual": actual, "note": ""})
        base["known"] = entries
        base.setdefault("_comment",
                        "Coined actor_renames.tsv claims that config/**/symbols.txt does "
                        "not carry at that address, banked so tools/check_profile_campaign.py "
                        "fails only on NEW drift. An entry stops matching as soon as either "
                        "side changes; it is not a permanent exemption for the address.")
        with io.open(os.path.join(repo, BASELINE), "w", encoding="utf-8", newline="\n") as fh:
            json.dump(base, fh, indent=2, sort_keys=False)
            fh.write("\n")
        print("check_profile_campaign: banked %d coined ledger divergence(s) in %s"
              % (len(entries), BASELINE))
        return 0

    new_diverged = [d for d in diverged
                    if (d[0], d[1], d[2], tuple(d[3])) not in banked]
    n_banked_hit = len(diverged) - len(new_diverged)

    # ---- the guard, before any verdict ----
    too_small = []
    if n_symbol_files < MIN_SYMBOL_FILES:
        too_small.append("%d symbols.txt file(s), expected at least %d"
                         % (n_symbol_files, MIN_SYMBOL_FILES))
    if claims < MIN_LEDGER_CLAIMS:
        too_small.append("%d live coined ledger claim(s), expected at least %d"
                         % (claims, MIN_LEDGER_CLAIMS))
    if reg["status"] == "ok" and reg["scope"] < MIN_REGISTRY_ROWS:
        too_small.append("%d in-scope registry row(s), expected at least %d"
                         % (reg["scope"], MIN_REGISTRY_ROWS))
    if too_small:
        print("check_profile_campaign: SCAN TOO SMALL -- " + "; ".join(too_small) + ".")
        print("  A reader is broken, or the tool is being run outside the repo.")
        print("  Refusing to report a pass on a scan this size.")
        return 2

    # ---- report ----
    if reg["status"] == "absent":
        print("check_profile_campaign: registry scan SKIPPED -- %s is not in the tree "
              "(it lands with PR #2235)." % REGISTRY)
    elif reg["status"] == "incomplete":
        print("check_profile_campaign: registry scan SKIPPED -- %s lacks the column(s) %s."
              % (REGISTRY, ", ".join(reg["missing_columns"]) or "(file is empty)"))
    else:
        print("check_profile_campaign: %d registry row(s), %d in scope (%d superseded); "
              "%d assertion(s) complete, %d pending, %d factory source(s) exempt as folded "
              "into a promoted TU"
              % (reg["rows"], reg["scope"], reg["superseded"], reg["complete"],
                 reg["pending"], reg["exempt"]))

    print("check_profile_campaign: %d live coined ledger claim(s) resolved against "
          "%d symbols.txt file(s); %d diverge, %d of them banked in %s"
          % (claims, n_symbol_files, len(diverged), n_banked_hit, BASELINE))

    rc = 0

    if reg_failures:
        rc = 1
        print("\nFAIL: %d registry row(s) disagree with the tree:" % len(reg_failures))
        shown = reg_failures if args.list else reg_failures[:40]
        for kind, pid, mod, detail in shown:
            print("  %s [%s %s]" % (kind, pid, mod))
            print("      %s" % detail)
        if len(shown) < len(reg_failures):
            print("  ... and %d more (--list for all)" % (len(reg_failures) - len(shown)))

    if new_diverged:
        rc = 1
        print("\nFAIL: %d coined ledger row(s) claim a name symbols.txt does not carry:"
              % len(new_diverged))
        shown = new_diverged if args.list else new_diverged[:40]
        for module, addr_text, claimed, actual, lineno in shown:
            print("  %s:%d  %s %s" % (LEDGER, lineno, module, addr_text))
            print("      ledger says %s; symbols.txt carries %s"
                  % (claimed, ", ".join(actual) if actual else "(nothing at that address)"))
        if len(shown) < len(new_diverged):
            print("  ... and %d more (--list for all)" % (len(new_diverged) - len(shown)))
        print("\n  Either the rename never reached symbols.txt -- in which case the build")
        print("  never emitted the name this row asserts -- or symbols.txt moved on and no")
        print("  ledger row followed. Fix the side that is wrong; bank it with --update")
        print("  only once someone has decided the divergence is correct.")

    if pendings:
        label = "FAIL" if args.strict else "PENDING"
        if args.strict:
            rc = 1
        print("\n%s: %d campaign assertion(s) not yet applied:" % (label, len(pendings)))
        shown = pendings if args.list else pendings[:40]
        for kind, pid, mod, where, want in shown:
            print("  %-10s %-22s %s  %s -> %s" % (kind, pid, mod, where, want))
        if len(shown) < len(pendings):
            print("  ... and %d more (--list for all)" % (len(pendings) - len(shown)))
        if not args.strict:
            print("\n  Pending rows do not fail the build while waves are still landing.")
            print("  Run with --strict once the campaign closes.")

    if args.strict and reg["status"] != "ok":
        rc = 1
        print("\nFAIL: --strict requires the registry dataset, and the registry scan was "
              "skipped.")

    if rc == 0:
        print("\nOK: every campaign claim this tool can resolve is carried by the tree.")
    return rc


if __name__ == "__main__":
    sys.exit(main())
