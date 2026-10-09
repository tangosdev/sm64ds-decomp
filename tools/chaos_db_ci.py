"""CI-safe Chaos Viewer data generator: rebuilds chaos-db.json from COMMITTED
data only (no ROM, no local ledger), so GitHub Actions can refresh the
chaos-data branch on every push and the hosted viewer always shows current
modules and percentages.

Derived the same way as progress.py --write-readme:
  universe   config/**/symbols.txt  (name, addr, size per module)
  matched    srcpath resolves a source, asm_policy.counts_as_matched accepts it (no
             // NONMATCHING banner, or one overridden by a HAND-ASM PRIMITIVE banner;
             never an unbannered dcd transcription), and the record is not in the
             byte-gate-failure class (policy D -- see tools/bytegate.py)
  near-miss  nearmiss/db.jsonl (committed) -> div badge
  author     function-authors.json, keyed by module and address. That file is the
             only credit record. Git history is not consulted.
  project    tools/chaosviewer.config.json (committed branding/prompt config)

Not derivable without the ROM (left to local regens): disasm/callee detail
chunks, coddog sim/sibling. The details/ directory on the chaos-data branch is
preserved as-is by the workflow.

Usage: python tools/chaos_db_ci.py [--out chaos-db.json]
"""
import argparse
import collections
import json
import pathlib
import sys
import re
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"
SRC = REPO / "src"
sys.path.insert(0, str(REPO / "tools"))
import asm_policy  # noqa: E402
import srcpath as SP  # noqa: E402
import relocs as RL  # noqa: E402
import rombuild_check as RBC  # noqa: E402
import layout_check as LYC  # noqa: E402
import tiers as TIERS  # noqa: E402
import bytegate as BG  # noqa: E402


def enrolled_addresses():
    """{(module, addr)} for every range a delinks.txt marks ``complete``.

    This is the set the ROM build actually compiles and byte-compares against the
    cartridge.  ``matched`` below is a different and much weaker test -- a source
    resolved through enrollment/filename fallback, with no ``NONMATCHING`` banner or
    ``dcd`` blob and no byte-gate exclusion --
    and the two differ by several hundred functions, because a file can sit in the
    tree, be counted, and be compiled by nothing.  The published percentage was the
    weaker number alone, so both now ride along and the site can say which is which.

    Reads only committed config, so it stays CI-safe: no ROM, no compiler.
    """
    out = set()
    for sym, label in RL.module_universe():
        delinks = sym.parent / "delinks.txt"
        if not delinks.is_file():
            continue
        for rel, addr, _end in RBC.complete_entries(delinks):
            if not rel.startswith("mods/"):
                out.add((label, addr))
    return out


def alias_collision_addresses():
    """{(module, addr)} where a size-0 function record and a SIZED one collide.

    ADDRESSES, not records, so the caller must act on only the `size == 0` side. Both
    records live at the same key and dropping both would be the opposite of the fix: the
    sized primaries here are the bodies that SHOULD be counted once someone matches them,
    and func_01ff8708 is 1,776 bytes of it. bytegate.is_zero_size_alias is the one
    predicate that gets that right; call it rather than re-spelling the test.

    config/arm9/itcm/symbols.txt declares eight bodies twice, once as a sized function and
    once as a zero-size alias at the identical address (_dmul beside func_01ff8708,
    _ll_sdiv beside func_01ffaa34, _s32_div_f beside __aeabi_idiv, _u32_div_f beside
    __aeabi_uidiv, and _dadd, _deq, _ll_udiv, _ull_mod likewise); config/arm9/symbols.txt
    declared two more until the two MSL array helpers took the compiler's own spelling as
    their primary name and the zero-size halves went away (__cxa_vec_cleanup at 0x0207328c,
    __cxa_vec_ctor at 0x020733a8). srcpath resolves src/named/arm9/_dmul.c -- a real HAND-ASM PRIMITIVE match --
    onto the ZERO-SIZE record, so the same 1,776-byte body read as matched at 0 bytes
    under the alias and unmatched at full size under the primary. linkcheck reports those
    NO-SYM (len-mismatch), which is the byte gate declining to compare a real function
    against a zero-length range.

    Those alias records leave the universe here, numerator and denominator both. A second
    name for a function already in the list is not a second function, and the zero-size
    half can never be byte-compared, so leaving it in the denominator (which is what
    happened until 2026-09-06) parks records that no amount of decompilation can
    clear. Deriving the set rather than listing it means the drop self-heals: repoint the
    alias at a real size in config and the record returns to the count with no edit here.
    Every zero-size record in the universe today has a sized twin, and a zero-size symbol
    that stands alone is NOT caught by this -- it stays counted, so a genuinely unmatched
    stub cannot be hidden by calling it an alias.

    Committed config only, no ROM and no compiler, so it runs in the workflows that
    publish the count. The other half of the byte-gate-failure class cannot be; see
    tools/bytegate.py.
    """
    return BG.alias_collision_addresses(RL.module_universe)


def enrollment_of(label, addr, src_path, enrolled, blocks):
    """``enrolled`` / ``unenrolled`` / ``no_block`` for one function.

    ``verified`` above is a boolean, and a boolean cannot tell the two halves of its
    false case apart. They are different facts with different remedies:

      enrolled    a delinks range carries ``complete``: mwccarm compiles this source
                  and the link byte-compares it against the cartridge. Identical to
                  ``verified`` for a matched function, by construction: both read the
                  same enrolled_addresses() set, so the two fields cannot drift.
      unenrolled  a delinks entry names a source for this range but has no ``complete``.
                  The source is in the tree and the build reads ROM bytes instead. This
                  is where a real match that nobody promoted looks exactly like a file
                  that was never going to build.
      no_block    no delinks entry names this range at all. Overwhelmingly deliberate:
                  enroll.py skips thumb functions, addresses that are not 4-aligned,
                  zero-size alias symbols and everything in config/rombuild-exclude.txt,
                  and it writes an intentional divergence under mods/ rather than src/.

    Two existing readers, no third delinks parser: enrolled_addresses() for ``complete``
    (address-keyed, the same set ``verified`` uses) and layout_check.delinks_paths() for
    "is there an entry at all" (path-keyed, the L1/L5 reader).

    The two keyings can disagree for an entry under mods/: a delinks range naming a
    mods/ path can be marked ``complete``, so an address-coverage test calls it
    enrolled, but enrolled_addresses() drops mods/ on purpose, because a deliberate
    divergence must never be counted as a reproduction of the cartridge. Such an entry
    is reported no_block here, which is the honest answer for the src/ path it displaced:
    that file is compiled by nothing. `mods/` is currently empty, so this case does not
    occur in the tree today, but the reader still has to handle it correctly.
    """
    if (label, addr) in enrolled:
        return "enrolled"
    return "unenrolled" if src_path and src_path in blocks else "no_block"


def tier_stats():
    """The CONVERTED and LINKED tiers, flattened for the stats block.

    Flat rather than nested because the two consumers downstream are a shell
    script's python one-liner and a C# record, and neither gains anything from
    nesting. MATCHED is not repeated here; it is already the block above.

    Never raises. This runs inside the generator whose output pays the coin
    ledger, and a readability scan is not worth killing a refresh over -- a run
    that dies here would leave contributions.json unmoved and every match landed
    since the last refresh uncredited. On failure the key is simply absent, and
    the site falls back to showing MATCHED alone.
    """
    try:
        c = TIERS.converted()
        k = TIERS.linked()
    except Exception as e:  # noqa: BLE001 - see docstring
        print(f"  tiers: SKIPPED ({e})")
        return None
    out = {
        "converted": c["converted"],
        "convertedOf": c["functions"],
        "convertedSourceFiles": c["source_files"],
        "convertedPct": c["pct"],
    }
    if k:
        out.update({
            "linked": k["linked"],
            "linkedOf": k["matchedTus"],
            "linkedPct": k["pct"],
            "linkedMeasuredAt": k["measuredAt"],
            "linkedBranch": k["branch"],
            "linkedCommit": k["commit"],
        })
    return out

FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)")

# Match-coin weighting, per module. A match is worth 1 MC by default; a module
# listed here is worth its multiplier instead. This exists to steer effort at
# modules nobody is picking up, not to re-rank past work -- `matched` below stays
# the honest function count and only `coins` carries the weight.
#
# itcm at 10x: it was invisible in every viewer and every count until 2026-08-05,
# so nothing in it was ever chosen by anyone. 25 of its 43 functions are still
# unmatched, including _ZN12MeshCollider10DetectClsnER10SphereClsn at 7112 bytes,
# the largest unmatched function in the game.
COIN_WEIGHTS = {"itcm": 10}

# Bump whenever COIN_WEIGHTS changes, or whenever the DEFINITION of a matched
# function changes. The backend credits the DELTA between a published career total
# and what it last paid, so a formula change would otherwise read as thousands of
# new matches and pay a retroactive windfall (and fire a wall of Discord
# milestones). The backend rebases silently when this number moves: it banks the new
# totals as already-credited without touching balances, so a change only ever affects
# matches landed AFTER it.
#
# 2 -> 3: policy D. `matched` now excludes the byte-gate-failure class, which takes 58
# coins back off five contributors (ruspecial -43, tangosdev -8, lunavyqo -4,
# andrewboudreau -2, mitch030504 -1). Without the bump the backend would read those as
# negative deltas and claw the balances back; with it, the smaller totals are banked as
# the new baseline and nobody's balance moves.
COIN_FORMULA = 3

# A NONMATCHING file that reproduces the ROM and has no match left to chase - the banner
# tags which kind. These are NOT pending work, so the viewers paint them apart from real
# near-miss drafts instead of lumping everything unmatched together.
NOMATCH_RE = re.compile(r"NONMATCHING \((ASM-PRIMITIVE|NOT-C-EXPRESSIBLE)\)")
NOMATCH_REASONS = {
    "ASM-PRIMITIVE": (
        "asm-primitive",
        "Nintendo shipped this as an assembly primitive. There is no original C to "
        "recover, so writing C for it would invent a source that never existed.",
    ),
    "NOT-C-EXPRESSIBLE": (
        "not-c-expressible",
        "A bare epilogue or mid-frame exit stub the symbol table split out, not a real "
        "function. No standalone C construct produces it.",
    ),
}


def no_match_needed(head: str) -> dict[str, str] | None:
    """Bucket + hover explanation for a file that needs no match, else None."""
    m = NOMATCH_RE.search(head)
    if not m:
        return None
    bucket, reason = NOMATCH_REASONS[m.group(1)]
    return {"bucket": bucket, "reason": reason}


# An asm-bodied file is countable only under an explicit banner: HAND-ASM PRIMITIVE
# (policy-matched -- the original really was assembly) or NONMATCHING (draft / hatch).
# A dcd blob with neither is a transcription: it byte-matches vacuously because it IS
# the ROM words re-spelled, so counting it as matched puts a lie in the progress bar
# (PR #1072 landed 8 of these as +8 matches / +8,208 bytes / credit). "transcribed"
# is demoted from matched; mnemonic asm without a banner is a policy gray zone
# (embedded hatches inside real C, e.g. CP15 intrinsics) and is only WARNED about.
# The detector is asm_policy.classify, shared with validate_merge and pr_linkcheck.
#
# When a file carries BOTH banners the HAND-ASM one wins (Tango's ruling, 2026-09-09).
# Twenty files here say "byte-exact hand-written asm ... no original C to recover and
# no match to chase" and then also print the word NONMATCHING, and the count read only
# the second half. asm_policy.counts_as_matched holds that whole rule so this generator,
# progress.py and validate_merge cannot drift apart on it again.


def function_authors() -> dict[str, str]:
    """``{'module:0xaddress': login}`` from function-authors.json. The only credit record."""
    p = REPO / "function-authors.json"
    if not p.is_file():
        return {}
    try:
        data = json.loads(p.read_text(encoding="utf-8"))
        functions = data.get("functions", {}) if isinstance(data, dict) else {}
    except (OSError, json.JSONDecodeError) as exc:
        print(f"  (function-authors.json skipped: {exc})")
        return {}
    out = {}
    for key, row in functions.items():
        author = row.get("author") if isinstance(row, dict) else row
        if isinstance(key, str) and isinstance(author, str) and author:
            out[key] = author
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default="chaos-db.json")
    ap.add_argument("--contrib-out", default=None,
                    help="path for contributions.json (default: next to --out)")
    ap.add_argument("--fail-on-transcribed", action="store_true",
                    help="exit 1 if any unbannered dcd transcription is in the tree")
    args = ap.parse_args()

    nm = {}
    nm_path = REPO / "nearmiss" / "db.jsonl"
    if nm_path.is_file():
        for l in nm_path.read_text(encoding="utf-8", errors="ignore").splitlines():
            if l.strip():
                try:
                    r = json.loads(l)
                    a = r["addr"]
                    nm[(r["module"], int(a, 0) if isinstance(a, str) else a)] = r
                except Exception:
                    continue

    recorded_authors = function_authors()

    functions = []
    total_b = matched_b = matched_n = 0
    verified_b = verified_n = 0
    handasm_b = handasm_n = 0
    enrolled = enrolled_addresses()
    # Every src path any delinks.txt names, promoted or not. Read once: delinks_paths
    # walks all 106 delinks files, and calling it per function would walk them 11,396
    # times.
    blocks = set(LYC.delinks_paths())
    # The byte-gate-failure class, both halves, read once for the same reason. See the
    # `matched` conjunct below and tools/bytegate.py.
    alias_addrs = alias_collision_addresses()
    # Same eight addresses, keyed to the names the aliases carry, so a sized record
    # whose source is filed under its alias can find it.  Derived from the same
    # committed config, in one pass, for the same reason the set above is.
    alias_srcnames = BG.alias_names(RL.module_universe)
    wont_build = BG.excluded_paths()
    bytegate_n = collections.Counter()
    enrollment_n = collections.Counter()
    alias_dropped = 0
    transcribed_files, unbannered_files = set(), set()
    # Every module, itcm included. relocs.module_universe is the one definition of
    # what "every module" means, and it fails loudly rather than skipping a new one.
    for sym, label in RL.module_universe():
        for line in sym.read_text(errors="ignore").splitlines():
            m = FUNC_RE.match(line)
            if not m:
                continue
            name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            # A zero-size record sharing an address with a sized one is a second NAME
            # for the function already on that address, not a second function, so it
            # never becomes a record at all -- it is out of the numerator and out of
            # the denominator alike. It used to be published as an unmatched record,
            # which put ten rows in every total that nothing could ever clear, gave the
            # treemap ten zero-area rectangles, and handed two records the same `id`.
            # See alias_collision_addresses for the ten and for how the drop self-heals.
            if BG.is_zero_size_alias(label, addr, size, alias_addrs):
                alias_dropped += 1
                continue
            f = SP.path_for(name)
            if f is None:
                # An aliased address files its source under whichever name the author
                # knew the function by, which in this tree is the ALIAS: src/named/arm9/_dmul.c
                # decompiles the bytes the symbol table calls func_01ff8708. Asked only
                # about the sized record's own name, the lookup finds nothing and four
                # byte-exact ITCM primitives read as never attempted. The zero-size
                # record is still dropped above, so this attaches the source to the one
                # record that survives rather than counting the pair twice.
                for alt in alias_srcnames.get((label, addr), ()):
                    f = SP.path_for(alt)
                    if f is not None:
                        break
            src_path = f.relative_to(REPO).as_posix() if f else None
            text = f.read_text(errors="ignore") if f else ""
            # The settled tag lives in the banner, and banners drift downward as the
            # recovery prose above them grows -- so the leading comment block, never a
            # fixed byte window.
            head = asm_policy.header_region(text)
            cls = asm_policy.classify(text) if src_path else None
            # Policy D (Tango's ruling on audit/enrollment_report.md section 6): a file
            # that exists is not evidence if the byte gate cannot get a verdict out of
            # it. linkcheck reports 22 of the 251 matched-but-unverified functions as
            # NO-SYM -- 18 that no compiler in the sweep will build and 4 zero-size alias
            # records counting a body their sized twin reports unmatched -- and those 22
            # stop counting here. Everything that REPRODUCES the cartridge keeps its
            # matched status, including the 185 unenrolled rows that byte-match but are
            # not promoted, which is why the report's options B and C were rejected: they
            # would have deleted matches that are demonstrably correct. `verified` is
            # unchanged and still published beside this, and none of the 22 was verified,
            # so that number does not move. The alias half of the class is handled above,
            # by never becoming a record; what is left here is the manifest half.
            #
            # The gate is applied only to records the OLD test would have counted, so
            # that it is credited with what it actually removed rather than with every
            # record the class happens to describe.
            countable = bool(src_path) and asm_policy.counts_as_matched(text)
            # Assembly that counts, kept separately visible. It is a real match -- the
            # original was assembly, so the asm block IS the recovered source -- but it
            # is not C, and a bar that says "byte-exact C" has to be able to say how
            # much of itself is not.
            hand_asm = countable and asm_policy.has_hand_banner(text)
            bytegate_fail = countable and src_path is not None and src_path in wont_build
            matched = countable and not bytegate_fail
            total_b += size
            rec = {"id": f"{label}:0x{addr:08x}", "module": label, "name": name,
                   "addr": addr, "size": size, "matched": matched}
            if bytegate_fail:
                # Recorded on the record, not just subtracted from a total. A reader who
                # wonders why a src/ file exists for an unmatched function gets the
                # answer here instead of having to re-run the gate.
                rec["byteGate"] = "will-not-build"
                bytegate_n[rec["byteGate"]] += 1
            if src_path:
                rec["srcPath"] = src_path
                if cls == "transcribed":
                    rec["transcribed"] = True
                    transcribed_files.add(src_path)
                elif cls == "unbannered-asm":
                    unbannered_files.add(src_path)
                nomatch = no_match_needed(head)
                if nomatch:
                    rec["noMatch"] = nomatch
                if matched:
                    a = recorded_authors.get(f"{label}:0x{addr:08x}")
                    if a:
                        rec["author"] = a
            if matched:
                matched_b += size
                matched_n += 1
                if hand_asm:
                    # Published per record as well as summed, so a reader who wants to
                    # know WHICH matched functions are assembly rather than C can answer
                    # it from this file instead of re-grepping src/.
                    rec["handAsm"] = True
                    handasm_b += size
                    handasm_n += 1
                # Byte-verified is the subset the ROM build proves: enrolled, compiled,
                # linked into its module and compared to the cartridge. The rest are
                # matched on the strength of a source claim and are filled at link time by
                # a gap object holding the ROM's own bytes.
                if (label, addr) in enrolled:
                    rec["verified"] = True
                    verified_b += size
                    verified_n += 1
            else:
                r = nm.get((label, addr))
                if r and r.get("divergences") is not None:
                    rec["div"] = r["divergences"]
            # Appended last, deliberately. Every field above keeps the position it had
            # before this existed, so the whole diff against a previous chaos-db is one
            # inserted key per record and a reviewer can see at a glance that nothing
            # else moved. Nothing here reads it back, and `matched` is untouched: this
            # annotation reports the tree, it does not redefine the count.
            rec["enrollment"] = enrollment_of(label, addr, src_path, enrolled, blocks)
            enrollment_n[(rec["enrollment"], matched)] += 1
            functions.append(rec)

    if transcribed_files:
        print(f"TRANSCRIBED: {len(transcribed_files)} unbannered dcd transcription(s) "
              f"-- NOT counted as matched:")
        for p in sorted(transcribed_files):
            print(f"  {p}")
    if unbannered_files:
        print(f"unbannered-asm: {len(unbannered_files)} asm-bodied file(s) with no "
              f"HAND-ASM PRIMITIVE or NONMATCHING banner -- policy review needed "
              f"(banner or reclassify):")
        for p in sorted(unbannered_files):
            print(f"  {p}")

    project = None
    pc = REPO / "tools" / "chaosviewer.config.json"
    if pc.is_file():
        project = json.loads(pc.read_text(encoding="utf-8"))

    db = {
        "generatedAt": time.strftime("%Y-%m-%d %H:%M", time.gmtime()) + " UTC",
        "project": project,
        "stats": {
            "totalFunctions": len(functions),
            "matchedFunctions": matched_n,
            "totalBytes": total_b,
            "matchedBytes": matched_b,
            # The subset of `matched` the cartridge actually settles. `matched` is kept
            # unchanged -- it is what contributor credit is computed from, and every
            # existing consumer reads it -- but shipping it alone overstated coverage,
            # so the measured figure travels beside it. See enrolled_addresses.
            "verifiedFunctions": verified_n,
            "verifiedBytes": verified_b,
            # The assembly subset of `matched`: functions whose source is a bannered
            # HAND-ASM PRIMITIVE, byte-faithful asm because the original was assembly.
            # Reported so the README caption and tiers.py can name it rather than
            # letting "matched C" quietly include work that is not C.
            "handAsmFunctions": handasm_n,
            "handAsmBytes": handasm_b,
            "moduleCount": len({f["module"] for f in functions}),
            # The other two tiers ride along here so every consumer reads ONE file.
            # romstats-sync.sh on the VPS fetches this db and nothing else, and the
            # backend stores what it is told rather than walking any repository, so a
            # tier that is not in this block does not reach the site at all. Both are
            # computed from committed source with no ROM and no build, which is what
            # keeps this generator CI-safe.
            "tiers": tier_stats(),
        },
        "functions": functions,
    }
    out = pathlib.Path(args.out)
    out.write_text(json.dumps(db), encoding="utf-8")
    print(f"wrote {out} ({out.stat().st_size // 1024} KB): "
          f"{matched_n}/{len(functions)} funcs, {matched_b}/{total_b} bytes, "
          f"{db['stats']['moduleCount']} modules, "
          f"{sum(1 for f in functions if 'author' in f)} authored")
    # Both numbers in the log, always. The gap between them is the number of functions
    # counted on the strength of a source claim that no build compiles, and it is only
    # visible if the smaller figure is printed next to the larger one.
    print(f"  byte-verified: {verified_n}/{len(functions)} funcs, "
          f"{verified_b}/{total_b} bytes "
          f"({100.0 * verified_b / total_b:.2f}% vs {100.0 * matched_b / total_b:.2f}% "
          f"matched); {matched_n - verified_n} matched function(s) are compiled by "
          f"nothing")
    print(f"  of the matched: {handasm_n} function(s), {handasm_b} bytes are byte-exact "
          f"hand-written assembly (HAND-ASM PRIMITIVE), not C")
    # ...and WHICH KIND of nothing, because the two halves have different remedies. A
    # matched/unenrolled function has a delinks entry waiting for a `complete`; a
    # matched/no_block one is almost always deliberate (thumb, alias, exclude list) and
    # promoting it is not on the table. Printed so the split lands in the CI log next to
    # the number it explains. See audit/enrollment_report.md.
    print("  enrollment: " + ", ".join(
        f"{k[0]}{'/matched' if k[1] else ''}={n}"
        for k, n in sorted(enrollment_n.items(), key=lambda kv: (kv[0][0], not kv[0][1]))))
    # The policy-D subtraction, in the log next to the number it explains. A silent
    # exclusion is the same mistake the enrollment split was added to fix.
    print("  byte-gate failures (NOT counted matched): " + (", ".join(
        f"{k}={n}" for k, n in sorted(bytegate_n.items())) or "none"))
    # And the records that never became records. This one changes the DENOMINATOR, so it
    # is the last thing that may move quietly: if this number drifts, the published rate
    # drifted with it and the log is where anyone would look.
    print(f"  zero-size aliases dropped from the universe: {alias_dropped} "
          f"(second names for functions already counted at the same address)")
    # A stale manifest row means someone edited one of the will-not-build files without
    # re-running the gate, so its exclusion has lapsed and that function is being counted
    # again. Permissive by design -- the count falls back to the old behaviour rather than
    # guessing -- but it must never be silent, and tools/test_bytegate.py fails on it.
    for s in BG.stale_rows():
        print(f"  WARNING: bytegate row {s['problem']} ({s['src']}); its exclusion has "
              f"lapsed and the function is counted matched again. Re-run "
              f"`python tools/bytegate.py --recheck`.")
    # Per-module counts in the log, so a module that stops being emitted shows up in
    # the CI diff as a line that vanished. The silent version of this cost itcm its
    # entire visibility; a number that goes to zero is at least readable.
    per_mod = collections.Counter(f["module"] for f in functions)
    print("  modules: " + ", ".join(f"{m}={n}" for m, n in sorted(per_mod.items())))

    # The single source of truth for the contributor chart: matched-function count per canonical
    # login. Regenerated on every merge (the workflow re-runs this), so "someone's number" is a
    # committed fact, not re-derived from git each time. --contrib-out defaults next to --out.
    tally = collections.Counter(f["author"] for f in functions if f.get("author"))
    # Weighted match coins. Kept as a SEPARATE field from `matched`, which stays the
    # plain honest count the contributor chart and the progress numbers read.
    coins = collections.Counter()
    for f in functions:
        who = f.get("author")
        if who:
            coins[who] += COIN_WEIGHTS.get(f["module"], 1)
    contrib = {
        "generatedAt": db["generatedAt"],
        "note": "Matched functions per contributor. The author of each function is "
                "function-authors.json. This file is regenerated from that list. Do not "
                "hand-edit it. `matched` is the function count; `coins` weights the same "
                "set by COIN_WEIGHTS. They differ only for weighted modules.",
        "totalMatched": matched_n,
        "coinFormula": COIN_FORMULA,
        "coinWeights": COIN_WEIGHTS,
        "contributors": [{"login": who, "matched": n, "coins": coins[who]}
                         for who, n in tally.most_common()],
    }
    cpath = pathlib.Path(args.contrib_out) if args.contrib_out else out.with_name("contributions.json")
    cpath.write_text(json.dumps(contrib, indent=1), encoding="utf-8")
    print(f"wrote {cpath}: {len(tally)} contributors "
          f"(top: {', '.join(f'{w}={n}' for w, n in tally.most_common(4))})")

    if args.fail_on_transcribed and transcribed_files:
        print(f"FAILED: {len(transcribed_files)} unbannered dcd transcription(s) in the "
              f"tree (see the TRANSCRIBED list above)")
        sys.exit(1)


if __name__ == "__main__":
    main()
