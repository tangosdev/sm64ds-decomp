"""Build the machine-readable PR validation report for a committed test merge.

The private worker owns the ROM and compiler.  Its orchestration is intentionally
small: build/cache the committed base, create and commit the PR merge, run rombuild.py
for both, run pr_linkcheck for the affected files, then call this tool with the JSON
artifacts.  This tool supplies the stable policy and report schema used by CI.

Coverage and attribution are read from Git *commits*, never the worktree.  That is
load-bearing for path-only PRs: ``first_matchers`` follows committed rename lineage,
while an uncommitted merge makes every new path appear to have no author.

Example:
    python tools/validate_merge.py --base origin/main --head HEAD \
      --require-merge-commit --base-rom-report build/base-rom.json \
      --head-rom-report build/head-rom.json --link-report build/link.json \
      --out build/validate-report.json --markdown build/validate-report.md
"""
import argparse
import bisect
import collections
import hashlib
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import asm_policy as AP  # noqa: E402
import chaos_db_ci as CHAOS  # noqa: E402
import rombuild_check as RBC  # noqa: E402

FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)")
SOURCE_SUFFIXES = (".c", ".cpp")
# Credit moves named in the one-line reason, and rows in the check body's table.
CREDIT_NAMED = 3
CREDIT_ROWS = 25
# tangos-backend's /result cap. Kept here so the report clips itself rather than being
# rejected whole -- see the clamp in build_report.
SUMMARY_LIMIT = 500


def _git(*args, allow=(0,), repo=None):
    repo = REPO if repo is None else pathlib.Path(repo)
    proc = subprocess.run(["git", *args], cwd=repo, capture_output=True, text=True,
                          encoding="utf-8", errors="replace")
    if proc.returncode not in allow:
        raise RuntimeError(f"git {' '.join(args)} failed: {proc.stderr.strip()}")
    return proc.stdout


def resolve_commit(rev):
    return _git("rev-parse", "--verify", f"{rev}^{{commit}}").strip()


def tree_paths(rev, prefix=None):
    args = ["ls-tree", "-r", "--name-only", rev]
    if prefix:
        args += ["--", prefix]
    return [p for p in _git(*args).splitlines() if p]


def git_text(rev, path):
    return _git("show", f"{rev}:{path}")


def _module_from_symbols(path):
    p = pathlib.PurePosixPath(path)
    if p.as_posix() == "config/arm9/symbols.txt":
        return "arm9"
    m = re.fullmatch(r"config/arm9/overlays/(ov\d+)/symbols\.txt", p.as_posix())
    return m.group(1) if m else None


def _module_from_delinks(path):
    if path == "config/arm9/delinks.txt":
        return "arm9"
    m = re.fullmatch(r"config/arm9/overlays/(ov\d+)/delinks\.txt", path)
    return m.group(1) if m else None


_ENROLMENT_CACHE = {}


def _rev_enrolment(rev):
    """``(module -> sorted [addr], (module, addr) -> src path)`` at ``rev``.

    validate_merge's own copy of ``srcpath._enrolment``, and it has to be one: every
    snapshot here reads git revisions through ``git show``, never the working tree, so
    the filesystem-backed resolver cannot answer for a base commit. Same rule, same
    `mods/` exclusion -- an entry outside `src/` is a deliberate divergence from the
    cartridge and is not the source that reproduces a ROM function.

    Every entry is read, `complete` or not: this answers "which file owns this symbol",
    which is a different question from "which range does the ROM build compare".
    """
    if rev not in _ENROLMENT_CACHE:
        addrs = {}
        for path in tree_paths(rev, "config/arm9"):
            module = _module_from_symbols(path)
            if module is None:
                continue
            rows = []
            for line in git_text(rev, path).splitlines():
                m = FUNC_RE.match(line)
                if m:
                    rows.append(int(m.group(3), 16))
            addrs[module] = sorted(rows)
        owner = {}
        for path in tree_paths(rev, "config/arm9"):
            module = _module_from_delinks(path)
            if module is None:
                continue
            module_addrs = addrs.get(module, [])
            entry = None
            for line in git_text(rev, path).splitlines():
                if not line.strip():
                    continue
                if not line[0].isspace():
                    entry = line.strip().rstrip(":")
                    continue
                # An indented section line before the first entry is the MODULE's own
                # header and owns nothing; read as an entry's it would hand the first
                # file every function in its module.
                if entry is None or not entry.startswith("src/"):
                    continue
                m = RBC.ENTRY_SEC.match(line)
                if not m:
                    continue
                start, end = int(m.group(2), 16), int(m.group(3), 16)
                i = bisect.bisect_left(module_addrs, start)
                while i < len(module_addrs) and module_addrs[i] < end:
                    owner[(module, module_addrs[i])] = entry
                    i += 1
        _ENROLMENT_CACHE[rev] = (addrs, owner)
    return _ENROLMENT_CACHE[rev]


def function_snapshot(rev):
    paths = tree_paths(rev)
    sources = {}
    for p in paths:
        pp = pathlib.PurePosixPath(p)
        if p.startswith("src/") and pp.suffix in SOURCE_SUFFIXES:
            sources.setdefault(pp.stem, p)
    grep = _git("grep", "-l", "-F", "NONMATCHING", rev, "--", "src/", allow=(0, 1))
    # With a tree-ish, git prefixes each result with ``<rev>:``.  Keeping that
    # prefix would make every historical NONMATCHING file look matched.
    candidates = {line.split(":", 1)[1] if ":" in line else line
                  for line in grep.splitlines()}
    # Match progress.py and the rest of the repo's established hatch rule: the
    # marker is a source header, recognized anywhere in the file's leading comment
    # block (asm_policy.has_draft_banner -- the one rule every consumer shares).
    nonmatching = {path for path in candidates
                   if AP.has_draft_banner(git_text(rev, path))}
    # An unbannered dcd transcription byte-matches vacuously (it IS the ROM words
    # re-spelled), so it never counts as matched -- see tools/asm_policy.py. Built
    # the same revision-based way as ``nonmatching``: a cheap fixed-string grep
    # prefilter, then the shared classifier confirms on the committed blob.
    grep = _git("grep", "-l", "-F", "dcd 0x", rev, "--", "src/", allow=(0, 1))
    dcd_candidates = {line.split(":", 1)[1] if ":" in line else line
                      for line in grep.splitlines()}
    transcribed = {path for path in dcd_candidates
                   if AP.classify(git_text(rev, path)) == "transcribed"}

    # Which file owns a function has two answers and this needs the authoritative one.
    # `sources` above is the filename convention -- stem == symbol -- which is right for
    # legacy one-function sources and blind to every merged translation unit, whose name
    # is neither of the symbols in it. Blind here does not read as "unknown": it reads as
    # "no source exists", so the functions leave `matched`, leave the byte-verified
    # count, and hand their authors' credit to nobody, while the ROM build goes on
    # compiling and byte-comparing them. The enrolment table is asked first, and it is
    # the one dsd hands the linker.
    enrolled = _rev_enrolment(rev)[1]
    in_tree = set(paths)
    records = {}
    total_bytes = matched_bytes = 0
    for path in paths:
        module = _module_from_symbols(path)
        if module is None:
            continue
        for line in git_text(rev, path).splitlines():
            m = FUNC_RE.match(line)
            if not m:
                continue
            name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            key = f"{module}:0x{addr:08x}"
            owner = enrolled.get((module, addr))
            src = owner if owner in in_tree else sources.get(name)
            matched = bool(src and src not in nonmatching and src not in transcribed)
            records[key] = {"id": key, "module": module, "addr": addr, "name": name,
                            "size": size, "matched": matched, "srcPath": src}
            total_bytes += size
            if matched:
                matched_bytes += size
    matched = {k: r for k, r in records.items() if r["matched"]}
    return {
        "functions": records,
        "matched": matched,
        "stats": {
            "totalFunctions": len(records),
            "totalBytes": total_bytes,
            "matchedFunctions": len(matched),
            "matchedBytes": matched_bytes,
            "matchedFunctionPercent": 100.0 * len(matched) / len(records) if records else 0.0,
            "matchedBytePercent": 100.0 * matched_bytes / total_bytes if total_bytes else 0.0,
        },
    }


def enrollment_snapshot(rev):
    entries = {}
    for path in tree_paths(rev, "config/arm9"):
        module = _module_from_delinks(path)
        if module is None:
            continue
        for rel, addr, end in RBC.complete_entries_text(git_text(rev, path)):
            key = f"{module}:0x{addr:08x}-0x{end:08x}"
            entries[key] = {"id": key, "module": module, "addr": addr, "end": end,
                            "size": end - addr, "path": rel,
                            "kind": "mod" if rel.startswith("mods/") else "src"}
    src = {k: e for k, e in entries.items() if e["kind"] == "src"}
    mods = {k: e for k, e in entries.items() if e["kind"] == "mod"}
    # FUNCTIONS IN THE RANGES, NOT THE NUMBER OF RANGES. `len(src)` counted delinks
    # entries and called them functions, which was the same number only while every
    # entry held exactly one. Merging two entries into one covering the identical span
    # then reads as `source-built function coverage decreased`, an unconditional failure,
    # for arithmetic rather than for lost coverage: `sourceBytes` does not move,
    # because no byte left the set.
    addrs = _rev_enrolment(rev)[0]

    def _functions_in(group):
        n = 0
        for e in group.values():
            module_addrs = addrs.get(e["module"], [])
            i = bisect.bisect_left(module_addrs, e["addr"])
            while i < len(module_addrs) and module_addrs[i] < e["end"]:
                n += 1
                i += 1
        return n

    return {"entries": entries, "source": src, "mods": mods,
            "stats": {"sourceFunctions": _functions_in(src),
                      "sourceBytes": sum(e["size"] for e in src.values()),
                      "modFunctions": _functions_in(mods),
                      "modBytes": sum(e["size"] for e in mods.values())}}


def verification_split(functions, enrollment):
    """Divide ``matched`` into what the ROM build PROVES and what only asserts itself.

    ``function_snapshot`` calls a function matched when its delinks enrollment resolves
    to a source in ``src/`` (falling back to a file named after the symbol), that source
    carries no ``NONMATCHING`` banner, and is not a ``dcd`` transcription. Nothing in
    that classification compiles the file, links it, or compares a byte -- and it
    cannot, because the real per-function ledger (``progress/matched.jsonl``) is
    gitignored and never reaches a validator.

    What the cartridge actually settles is the OTHER set: a range carrying ``complete``
    in a ``delinks.txt`` is compiled, linked into its module, and byte-compared against
    retail.  Everything else is filled by a gap object holding the ROM's own bytes, so
    it is exact by construction and proves nothing about the source beside it.

    The two are not close.  Reporting only the larger one puts a 10-point overstatement
    in front of every reader, so both travel, separately labelled, and the measured one
    leads.  ``matched`` itself is deliberately left alone: ``attribution_snapshot`` keys
    contributor credit off it, so redefining it here would move credit for everyone.
    """
    # A complete delinks entry is an object contribution, not necessarily one
    # function.  A promoted TU can cover dozens of consecutive function records;
    # comparing only the entry's start address misclassifies every member after the
    # first as "claimed, not byte-verified" even though rombuild compiles and checks
    # the entire range.  Classify against the full enrolled interval, just as
    # enrollment_snapshot's sourceFunctions counter does.
    enrolled = collections.defaultdict(list)
    for entry in enrollment["source"].values():
        enrolled[entry["module"]].append((entry["addr"], entry["end"]))

    def is_verified(record):
        end = record["addr"] + record["size"]
        return any(start <= record["addr"] and end <= stop
                   for start, stop in enrolled[record["module"]])

    verified = {k: r for k, r in functions["matched"].items() if is_verified(r)}
    claimed = {k: r for k, r in functions["matched"].items() if not is_verified(r)}
    return {
        "verified": verified,
        "claimed": claimed,
        "stats": {
            "verifiedFunctions": len(verified),
            "verifiedBytes": sum(r["size"] for r in verified.values()),
            "claimedFunctions": len(claimed),
            "claimedBytes": sum(r["size"] for r in claimed.values()),
        },
    }


def _json_at(rev, path):
    try:
        return json.loads(git_text(rev, path))
    except (RuntimeError, json.JSONDecodeError):
        return {}


def attribution_snapshot(rev, functions):
    old_repo = CHAOS.REPO
    CHAOS.REPO = REPO
    try:
        first = CHAOS.first_matchers(rev)
        finishers = CHAOS.match_finishers(rev)
    finally:
        CHAOS.REPO = old_repo
    data = _json_at(rev, "attribution.json")
    overrides = data.get("overrides", {}) if isinstance(data, dict) else {}
    aliases = data.get("aliases", {}) if isinstance(data, dict) else {}

    def canon(name):
        return aliases.get(str(name).lower(), name)

    by_function, counts, sizes = {}, collections.Counter(), collections.Counter()
    for key, rec in functions["matched"].items():
        path = rec["srcPath"]
        # A reconstructed TU may combine functions first matched by different people.
        # Keep the path override as the default, but let path#symbol preserve credit
        # for each member without inventing duplicate source files.
        member_key = f"{path}#{rec['name']}"
        author = (overrides.get(member_key) or overrides.get(path)
                  or finishers.get(path) or first.get(path))
        if not author:
            continue
        author = canon(author)
        # The source path travels with the author because credit is decided per file:
        # a report that says only "2 changed" cannot tell a PR author which two.
        by_function[key] = {"author": author, "path": path}
        counts[author] += 1
        sizes[author] += rec["size"]
    return {"byFunction": by_function,
            "stats": {"attributedFunctions": len(by_function),
                      "unattributedFunctions": len(functions["matched"]) - len(by_function)},
            "contributors": {a: {"functions": counts[a], "bytes": sizes[a]}
                             for a in sorted(counts)}}


def diff_snapshot(base, head, head_paths):
    rows, renames = [], []
    for line in _git("diff", "--name-status", "-M100%", f"{base}..{head}", "--", "src/").splitlines():
        parts = line.split("\t")
        if not parts:
            continue
        row = {"status": parts[0]}
        if parts[0].startswith("R") and len(parts) >= 3:
            row.update(oldPath=parts[1], newPath=parts[2])
            renames.append(row)
        elif len(parts) >= 2:
            row["path"] = parts[1]
        rows.append(row)
    leftovers = [r["oldPath"] for r in renames if r["oldPath"] in head_paths]
    return {"files": rows, "perfectRenames": renames, "leftoverOldPaths": leftovers}


def _load_json(path, optional=False):
    if not path:
        return None
    p = pathlib.Path(path)
    if optional and not p.exists():
        # An optional phase that did not run leaves no artifact.  That is "not
        # checked", not "failed" -- see _port_state.
        return None
    return json.loads(p.read_text(encoding="utf-8"))


def _rom_state(report):
    if report is None:
        return {"available": False}
    analysis = report.get("analysis", report)
    passed = report.get("status") == "passed" if "status" in report else bool(analysis.get("passed"))
    failure = report.get("failure")
    signature = None
    if failure:
        payload = {"phase": failure.get("phase"),
                   "returncode": failure.get("returncode"),
                   "output": failure.get("output", "")}
        signature = hashlib.sha256(
            json.dumps(payload, sort_keys=True).encode("utf-8")).hexdigest()
    elif not passed and "moduleFidelity" in analysis:
        payload = {"failures": analysis.get("failures", []),
                   "missingModuleBinaries": analysis.get("missingModuleBinaries", []),
                   "moduleResults": analysis.get("moduleFidelity", {}).get("results", [])}
        signature = hashlib.sha256(
            json.dumps(payload, sort_keys=True).encode("utf-8")).hexdigest()
    return {"available": True, "passed": passed, "signature": signature,
            "analysis": analysis if "moduleFidelity" in analysis else None,
            # tools/romdata_check.py's counts, present only from a rombuild that ran the
            # measurement. A base report cached before it existed simply has none, and
            # the ratchet below then does not run -- which is the right degradation:
            # nothing to compare against is not a regression.
            "romData": report.get("romData"),
            "failure": failure}


def _data_symbol_set(data, field):
    """Return stable ``(module, symbol)`` identities, or None for an old report."""
    rows = data.get(field)
    if not isinstance(rows, list):
        return None
    out = set()
    for row in rows:
        if not isinstance(row, dict) or not isinstance(row.get("symbol"), str):
            return None
        out.add((row.get("module"), row["symbol"]))
    return out


def _data_name(identity):
    module, symbol = identity
    return f"{module or '?'}:{symbol}"


def rom_data_regressions(base_data, head_data):
    """Name data-verification losses that a count-only ratchet can hide."""
    if base_data.get("verified") is None:
        return []
    if head_data.get("verified") is None:
        return ["head full-ROM report omitted the ROM-data measurement"]

    out = []
    base_verified = _data_symbol_set(base_data, "verifiedSymbols")
    head_verified = _data_symbol_set(head_data, "verifiedSymbols")
    if base_verified is not None and head_verified is not None:
        lost = sorted(base_verified - head_verified, key=lambda x: (x[0] or "", x[1]))
        if lost:
            names = ", ".join(_data_name(x) for x in lost[:3])
            more = f", +{len(lost) - 3} more" if len(lost) > 3 else ""
            out.append(f"ROM data verification lost {len(lost)} exact symbol(s): "
                       f"{names}{more}")
    elif head_data["verified"] < base_data["verified"]:
        out.append(
            f"ROM data verified from source fell from {base_data['verified']} to "
            f"{head_data['verified']} symbol(s)")

    if (base_data.get("verifiedBytes") is not None
            and head_data.get("verifiedBytes") is not None
            and head_data["verifiedBytes"] < base_data["verifiedBytes"]):
        out.append(
            f"ROM data verified bytes fell from {base_data['verifiedBytes']} to "
            f"{head_data['verifiedBytes']}")

    base_differing = _data_symbol_set(base_data, "differingSymbols")
    head_differing = _data_symbol_set(head_data, "differingSymbols")
    if base_differing is not None and head_differing is not None:
        new = sorted(head_differing - base_differing, key=lambda x: (x[0] or "", x[1]))
        if new:
            names = ", ".join(_data_name(x) for x in new[:3])
            more = f", +{len(new) - 3} more" if len(new) > 3 else ""
            out.append(f"ROM data gained {len(new)} differing symbol(s): {names}{more}")
    elif (base_data.get("differs") is not None
          and head_data.get("differs") is not None
          and head_data["differs"] > base_data["differs"]):
        out.append(
            f"ROM data differing symbols rose from {base_data['differs']} to "
            f"{head_data['differs']}")
    return out


def _link_state(rows):
    flattened = []
    for row in rows or []:
        # pr_linkcheck's JSON is grouped per translation unit.  Accept already-flat
        # rows too so the report schema is easy for another worker to produce.
        if isinstance(row, dict) and "results" in row:
            if row.get("results"):
                # pr_linkcheck downgrades a TU whose head declares NONMATCHING
                # from NO-REPRO to worst=DRAFT (#968): a self-declared draft
                # makes no claim to reproduce, so its non-reproduction is
                # expected. That downgrade lives on the GROUP row only; the
                # per-slot results still say NO-REPRO, so carry it down or the
                # flattened tally re-blocks what pr_linkcheck already excused.
                draft = row.get("worst") == "DRAFT"
                # RAW-ASM is the opposite carry-down: pr_linkcheck stamps an
                # unbannered dcd transcription on the GROUP row, while the
                # per-slot results can read VERIFIED -- vacuously, since the
                # dcd words ARE the ROM bytes. Without the override those
                # VERIFIED slots would launder the gate at the flattened level.
                raw_asm = row.get("worst") == "RAW-ASM"
                for result in row["results"]:
                    flat = {"file": row.get("file"), **result}
                    if draft and str(flat.get("verdict")) == "NO-REPRO":
                        flat["verdict"] = "DRAFT"
                    if raw_asm:
                        flat["verdict"] = "RAW-ASM"
                    flattened.append(flat)
            else:
                flattened.append({"file": row.get("file"),
                                  "verdict": row.get("worst") or
                                  ("UNRESOLVED" if row.get("note") else "NONE")})
        else:
            flattened.append(row)
    tally = collections.Counter()
    blocking = []
    for row in flattened:
        verdict = str(row.get("verdict", "ERROR"))
        bucket = "BLIND" if verdict.startswith("BLIND") else verdict
        tally[bucket] += 1
        if bucket in ("WRONG", "NO-REPRO", "ERROR", "RAW-ASM"):
            blocking.append(row)
    return {"checked": len(flattened), "tally": dict(tally), "blocking": blocking,
            "rows": flattened}


def _port_state(report):
    """tools/port_refcheck.py's JSON: port/'s literal src/ path and symbol
    references, checked against the merged tree.

    The phase is optional -- an older worker, a PR that touches no source, or a
    base that predates the tool all leave no report -- so an absent report is
    "not run" and never a failure.  `failures` is port_refcheck's flat
    {check,file,line,message} list; the per-check grouping is accepted as a
    fallback so a report written by an older copy of the tool still reads.
    """
    if report is None:
        return {"available": False}
    failures = report.get("failures")
    if failures is None:
        failures = [dict(f, check=name)
                    for name, check in (report.get("checks") or {}).items()
                    for f in check.get("failures") or []]
    passed = bool(report.get("ok", not failures)) and not failures
    return {"available": True, "passed": passed,
            "checked": report.get("checked", 0), "failures": failures}


def _port_reason(port):
    """Name what broke, not just that something did: a PR author reading the
    check has to be able to find the stale reference without the worker log."""
    failures = port["failures"]
    shown = "; ".join(f"{f.get('file')}:{f.get('line')}: {f.get('message')}"
                      for f in failures[:3])
    if len(failures) > 3:
        shown += f"; +{len(failures) - 3} more"
    return f"port refcheck: {len(failures)} stale reference(s) ({shown})"


def _credit_detail(changed, lost):
    """Name whose credit moved, not just how many did: "2 changed" sends a PR author
    to the worker log to find out which two files and which two contributors.

    Only CREDIT_NAMED of them go inline, because this text reaches the check run's
    summary and tangos-backend refuses a summary over 500 characters outright -- a
    long list would cost the whole verdict.  The readable list is the table
    ``render_markdown`` writes into the check body, and the full one is the JSON
    report's ``attribution`` section.
    """
    moves = ([f"{c['path']}: {c['before']} -> {c['after']}" for c in changed]
             + [f"{c['path']}: {c['author']} -> nobody" for c in lost])
    shown = "; ".join(moves[:CREDIT_NAMED])
    if len(moves) > CREDIT_NAMED:
        shown += f"; +{len(moves) - CREDIT_NAMED} more"
    return f"{len(changed)} changed, {len(lost)} lost -- {shown}"


def build_report(base, head, base_rom=None, head_rom=None, link_rows=None,
                 require_merge_commit=False, expected_pr_head=None,
                 port_report=None):
    base_sha, head_sha = resolve_commit(base), resolve_commit(head)
    parents = _git("rev-list", "--parents", "-n", "1", head_sha).split()
    is_merge = len(parents) >= 3
    if require_merge_commit and not is_merge:
        raise RuntimeError(f"{head_sha[:12]} is not a committed merge")
    if require_merge_commit and parents[1] != base_sha:
        raise RuntimeError(
            f"committed merge first parent {parents[1][:12]} is not base {base_sha[:12]}")
    if expected_pr_head:
        expected_head_sha = resolve_commit(expected_pr_head)
        if not is_merge or parents[2] != expected_head_sha:
            actual = parents[2][:12] if is_merge else "missing"
            raise RuntimeError(
                f"committed merge second parent {actual} is not PR head "
                f"{expected_head_sha[:12]}")

    bf, hf = function_snapshot(base_sha), function_snapshot(head_sha)
    be, he = enrollment_snapshot(base_sha), enrollment_snapshot(head_sha)
    bv, hv = verification_split(bf, be), verification_split(hf, he)
    ba, ha = attribution_snapshot(base_sha, bf), attribution_snapshot(head_sha, hf)
    diff = diff_snapshot(base_sha, head_sha, set(tree_paths(head_sha, "src/")))

    # The transcription gate for the PR itself, scoped STRICTLY to the sources this
    # merge adds or modifies (never renames-only or pre-existing files, so an old
    # transcription in the tree cannot fail an unrelated PR). Classified from the
    # committed head blob, same discipline as everything else in this report.
    changed_src = [row["path"] for row in diff["files"]
                   if row["status"][:1] in ("A", "M") and row.get("path", "")
                   .endswith(SOURCE_SUFFIXES)]
    changed_text = {p: git_text(head_sha, p) for p in sorted(changed_src)}
    changed_cls = {p: AP.classify(t) for p, t in changed_text.items()}
    new_transcribed = [p for p, c in changed_cls.items() if c == "transcribed"]
    new_unbannered = [p for p, c in changed_cls.items() if c == "unbannered-asm"]
    # A marker the counters cannot see inverts the file's own claim: every consumer
    # recognizes NONMATCHING only inside the leading comment block
    # (asm_policy.has_draft_banner), so a marker that sits below the first line of
    # code leaves the file counted as MATCHED while its author believes it is
    # bannered. Scoped like the transcription gate: only sources this merge adds or
    # modifies, so a historical stray cannot fail an unrelated PR.
    stranded_markers = [p for p, t in changed_text.items()
                        if AP.DRAFT_BANNER in t and not AP.has_draft_banner(t)]

    base_keys, head_keys = set(bf["matched"]), set(hf["matched"])
    bc, hc = ba["byFunction"], ha["byFunction"]
    common_credit = set(bc) & set(hc)
    credit_changes = [{"id": k, "path": hc[k]["path"], "basePath": bc[k]["path"],
                       "before": bc[k]["author"], "after": hc[k]["author"]}
                      for k in sorted(common_credit)
                      if bc[k]["author"] != hc[k]["author"]]
    lost_credit = [{"id": k, "path": bc[k]["path"], "author": bc[k]["author"]}
                   for k in sorted(set(bc) - set(hc))]
    added_credit = [{"id": k, "path": hc[k]["path"], "author": hc[k]["author"]}
                    for k in sorted(set(hc) - set(bc))]

    base_rom_state, head_rom_state = _rom_state(base_rom), _rom_state(head_rom)
    rom_regression = (base_rom_state.get("passed") is True
                      and head_rom_state.get("passed") is not True)
    same_baseline_failure = (base_rom_state.get("passed") is False
                             and head_rom_state.get("passed") is False
                             and base_rom_state.get("signature") is not None
                             and base_rom_state.get("signature")
                             == head_rom_state.get("signature"))
    link = _link_state(link_rows)
    port = _port_state(port_report)
    reasons = []
    # A function can leave `matched` two ways, and they are opposite acts.
    #
    #   REMOVED    the file was deleted, renamed out from under its symbol, or turned
    #              into a dcd transcription. Something the tree had is gone. Blocks.
    #   WITHDRAWN  the same file is still there, still named after the same symbol, and
    #              now carries a NONMATCHING banner. Nothing was lost -- a claim was
    #              retracted, which is the sanctioned way to tell the truth here.
    #
    # Treating both as loss made the gate reward the lie: a file that compiles to 916
    # bytes where the ROM has 912 was never a match, and bannering it -- the only
    # mechanism this repo HAS for saying so -- was the one edit that could not land.
    # Same shape as the CONVERTED defect, where un-converting a class was the only way
    # to score. See tools/bytegate.py: policy D already stopped counting the
    # unevidenced half of `matched`; this is the same judgement at the PR gate.
    #
    # WITHDRAWN is restricted to functions that were NOT byte-verified in the base.
    # That restriction is the whole safety argument, and it is explicit rather than
    # emergent: a byte-verified function is one the ROM build compiles and compares, so
    # retracting its claim is a real coverage loss and stays a hard reason (it would
    # also trip `source-built function coverage decreased`, but relying on that
    # interaction would leave the rule true only by accident).
    base_enrolled = {key.split("-", 1)[0] for key in be["source"]}
    transcribed_now = set(new_transcribed)
    withdrawn, removed = [], []
    for k in sorted(base_keys - head_keys):
        hr, br = hf["functions"].get(k) or {}, bf["functions"].get(k) or {}
        if (k not in base_enrolled and hr.get("srcPath")
                and hr["srcPath"] == br.get("srcPath")
                and hr["srcPath"] not in transcribed_now):
            withdrawn.append({"id": k, "path": hr["srcPath"], "name": hr.get("name")})
        else:
            removed.append(k)
    if removed:
        reasons.append(f"lost {len(removed)} matched function(s)")
    if (hf["stats"]["totalFunctions"] != bf["stats"]["totalFunctions"]
            or hf["stats"]["totalBytes"] != bf["stats"]["totalBytes"]):
        reasons.append("function/byte coverage denominator changed")
    if he["stats"]["sourceBytes"] < be["stats"]["sourceBytes"]:
        reasons.append("source-built byte coverage decreased")
    # Bytes alone cannot see a SWAP. Dropping `complete` from one delinks entry and
    # adding it to another of equal or greater size holds sourceBytes flat while the
    # first range quietly leaves the set the ROM build actually byte-compares -- and
    # its src/ file still exists, so `matched` does not move either and nothing in the
    # report changes. Two more checks, because the pair is what closes it: the count
    # catches a same-size swap, and the range diff below names anything that left even
    # when both totals hold.
    if he["stats"]["sourceFunctions"] < be["stats"]["sourceFunctions"]:
        reasons.append("source-built function coverage decreased")
    dropped_enrollment = sorted(set(be["source"]) - set(he["source"]))
    # Reassignment alone (a name moving from one contributor to another, credit_changes)
    # is never a blocker: a rebase, a rename, or resolving someone else's merge conflict
    # all relabel a function's "last touched by" without losing anything, and this
    # project spends no tokens defending credit -- not even for an outright LOSS
    # (lost_credit: a function that had a contributor and now has none). Both are
    # reported below so a PR author can see what moved, but neither fails the merge.
    credit_moved = bool(credit_changes or lost_credit)
    new_unattributed = (ha["stats"]["unattributedFunctions"]
                        > ba["stats"]["unattributedFunctions"])
    if diff["leftoverOldPaths"]:
        reasons.append("old source paths remain after rename")
    if new_transcribed:
        reasons.append(f"{len(new_transcribed)} asm-transcription file(s) with no "
                       f"HAND-ASM PRIMITIVE or NONMATCHING banner: "
                       + ", ".join(new_transcribed))
    if stranded_markers:
        reasons.append(f"{len(stranded_markers)} file(s) say NONMATCHING outside the "
                       "leading comment block, where no counter or gate recognizes it "
                       "-- move the marker into the file header, or reword prose that "
                       "quotes the bare word: " + ", ".join(stranded_markers))
    if link["blocking"]:
        reasons.append(f"{len(link['blocking'])} blocking relocation verdict(s)")
    if port.get("available") and not port["passed"]:
        reasons.append(_port_reason(port))
    # A RATCHET, not a gate. The emitted vtable and RTTI that `objisolate` discards are
    # compared against the cartridge, and most of the tree fails that today for a
    # understood reason -- a generated flat header declares no virtuals, so mwcc emits a
    # two-slot stub where the ROM has thirty-one. Failing a merge on all existing debt
    # would fail nearly every C++ file. Instead, known-good symbol identities and bytes
    # may not be lost, new differing symbols are refused, and a missing head measurement
    # is not allowed to disable the ratchet.
    base_data = base_rom_state.get("romData") or {}
    head_data = head_rom_state.get("romData") or {}
    reasons.extend(rom_data_regressions(base_data, head_data))
    if rom_regression:
        reasons.append("full-ROM result regressed from the base commit")
    if head_rom_state.get("available") and not head_rom_state.get("passed") \
            and not same_baseline_failure:
        reasons.append("full-ROM validation failed")

    warnings = []
    if withdrawn:
        warnings.append(
            f"{len(withdrawn)} claimed match(es) withdrawn by a NONMATCHING banner "
            "rather than lost: each file is still present under the same name, and "
            "none was byte-verified -- "
            + ", ".join(w["path"] for w in withdrawn[:3])
            + (f", +{len(withdrawn) - 3} more" if len(withdrawn) > 3 else ""))
    if credit_changes:
        # Never a blocker (see above) -- reported so a reader can still see whose
        # credit moved.
        warnings.append("contributor attribution changed, not a blocker "
                        f"({_credit_detail(credit_changes, [])})")
    if lost_credit:
        warnings.append("contributor attribution was lost, not a blocker "
                        f"({_credit_detail([], lost_credit)})")
    if new_unattributed:
        warnings.append("the merge introduced an unattributed matched function, "
                        "not a blocker")
    if same_baseline_failure:
        warnings.append("base and merge share the same pre-existing ROM-build failure")
    if (dropped_enrollment
            and he["stats"]["sourceBytes"] >= be["stats"]["sourceBytes"]
            and he["stats"]["sourceFunctions"] >= be["stats"]["sourceFunctions"]):
        # Both totals held, so neither reason above fired -- but these ranges are no
        # longer byte-compared against the cartridge. Re-splitting an entry (its `end`
        # moving) lands here too, which is why it warns rather than failing.
        warnings.append(
            f"{len(dropped_enrollment)} address range(s) left the byte-verified set "
            f"while enrolled totals held steady: " + ", ".join(dropped_enrollment[:5])
            + (f", +{len(dropped_enrollment) - 5} more" if len(dropped_enrollment) > 5 else ""))
    if new_unbannered:
        warnings.append(f"{len(new_unbannered)} new/changed file(s) carry an asm body "
                        f"with no HAND-ASM PRIMITIVE or NONMATCHING banner: "
                        + ", ".join(new_unbannered))
    if link["tally"].get("BLIND"):
        warnings.append(f"{link['tally']['BLIND']} linkcheck result(s) have unresolved relocations")
    unresolved = sum(link["tally"].get(k, 0) for k in ("UNRESOLVED", "NO-SYM", "NONE"))
    if unresolved:
        warnings.append(f"{unresolved} affected source file(s) could not be fully link-checked")
    if not head_rom_state.get("available"):
        warnings.append("no head full-ROM report was supplied")
    new_claimed = hv["stats"]["claimedFunctions"] - bv["stats"]["claimedFunctions"]
    if new_claimed > 0:
        # Landing a match before enrolling it is ordinary, so this is not a failure --
        # but the count only ever grew silently before, and it is the number that makes
        # the headline percentage bigger than the evidence.
        warnings.append(
            f"{new_claimed} more function(s) now claim a match "
            f"that nothing compiles; enroll them in a delinks.txt to have the ROM "
            f"build check them")

    coverage_delta = {
        "matchedFunctions": hf["stats"]["matchedFunctions"] - bf["stats"]["matchedFunctions"],
        "matchedBytes": hf["stats"]["matchedBytes"] - bf["stats"]["matchedBytes"],
        "verifiedFunctions": (hv["stats"]["verifiedFunctions"]
                              - bv["stats"]["verifiedFunctions"]),
        "verifiedBytes": hv["stats"]["verifiedBytes"] - bv["stats"]["verifiedBytes"],
        "claimedFunctions": (hv["stats"]["claimedFunctions"]
                             - bv["stats"]["claimedFunctions"]),
        "claimedBytes": hv["stats"]["claimedBytes"] - bv["stats"]["claimedBytes"],
        "sourceBuiltFunctions": he["stats"]["sourceFunctions"] - be["stats"]["sourceFunctions"],
        "sourceBuiltBytes": he["stats"]["sourceBytes"] - be["stats"]["sourceBytes"],
        "newMatchedFunctions": len(head_keys - base_keys),
        # Kept as the total, so an existing reader of this field does not silently see
        # a smaller number than the tree actually shed. The split is beside it.
        "removedMatchedFunctions": len(base_keys - head_keys),
        "withdrawnMatchedFunctions": len(withdrawn),
        "lostMatchedFunctions": len(removed),
    }
    base_source_stats = dict(be["stats"])
    head_source_stats = dict(he["stats"])
    base_source_stats["sourceBytesPercent"] = (
        100.0 * base_source_stats["sourceBytes"] / bf["stats"]["totalBytes"]
        if bf["stats"]["totalBytes"] else 0.0)
    head_source_stats["sourceBytesPercent"] = (
        100.0 * head_source_stats["sourceBytes"] / hf["stats"]["totalBytes"]
        if hf["stats"]["totalBytes"] else 0.0)
    if reasons:
        summary = "Validation failed: " + "; ".join(reasons)
    elif lost_credit or new_unattributed:
        summary = ("Committed merge passes; "
                   f"{len(lost_credit)} lost credit(s) noted"
                   + (", including an unattributed match" if new_unattributed else "")
                   + ", not a blocker.")
    elif credit_changes:
        summary = (f"Committed merge passes; {len(credit_changes)} contributor credit "
                   "reassignment(s) noted, not a blocker.")
    else:
        summary = "Committed merge introduces no reconstruction or attribution regression."
    # tangos-backend refuses a result whose summary runs past 500 characters, and a refused
    # result is a job that never reports at all -- the check sits pending until the sweeper
    # writes it off. Naming what broke is worth the length; losing the verdict is not, and
    # `reasons` below still carries every reason in full.
    if len(summary) > SUMMARY_LIMIT:
        summary = summary[:SUMMARY_LIMIT - 1] + "…"
    report = {
        "schemaVersion": 1,
        "status": "Failed" if reasons else "Passed",
        "baseSha": base_sha,
        "headSha": head_sha,
        "committedMerge": is_merge,
        "summary": summary,
        "reasons": reasons,
        "warnings": warnings,
        "coverage": {"base": {**bf["stats"], **bv["stats"]},
                     "head": {**hf["stats"], **hv["stats"]},
                     "delta": coverage_delta},
        "sourceBuild": {"base": base_source_stats, "head": head_source_stats},
        "attribution": {"base": ba["contributors"], "head": ha["contributors"],
                        "baseStats": ba["stats"], "headStats": ha["stats"],
                        "added": added_credit, "changed": credit_changes,
                        "lost": lost_credit},
        "diff": diff,
        "asmPolicy": {"transcribed": new_transcribed, "unbanneredAsm": new_unbannered,
                      "strandedMarkers": stranded_markers},
        "matchedWithdrawn": withdrawn,
        "linkcheck": link,
        "portRefcheck": port,
        "rom": {"base": base_rom_state, "head": head_rom_state,
                "regression": rom_regression, "sameBaselineFailure": same_baseline_failure},
    }
    report["reportMarkdown"] = render_markdown(report)
    return report


def _signed(n):
    return f"{n:+,}"


def _pct(n, d):
    return f"{100.0 * n / d:.2f}%" if d else "n/a"


def render_markdown(r):
    c, s, l = r["coverage"], r["sourceBuild"], r["linkcheck"]
    h, d = c["head"], c["delta"]
    relocation_summary = ", ".join(
        f"{v} {k}" for k, v in sorted(l["tally"].items())) or "no affected slots"
    lines = ["### Full merge validation", "",
             "| Check | Result |", "|---|---|",
             f"| Committed test merge | {'yes' if r['committedMerge'] else 'no'} |",
             # The measured number leads and the asserted one is named as an assertion.
             # Printing only `matchedFunctions` overstated coverage by ten points,
             # because a filename with no `complete` delinks entry counts there and is
             # compiled by nothing -- see verification_split.
             f"| Byte-verified functions | {h['verifiedFunctions']:,} / "
             f"{h['totalFunctions']:,} ({_pct(h['verifiedFunctions'], h['totalFunctions'])}, "
             f"{_signed(d['verifiedFunctions'])}) |",
             f"| Byte-verified code bytes | {h['verifiedBytes']:,} / {h['totalBytes']:,} "
             f"({_pct(h['verifiedBytes'], h['totalBytes'])}, {_signed(d['verifiedBytes'])}) |",
             f"| Claimed, not byte-verified | {h['claimedFunctions']:,} functions, "
             f"{h['claimedBytes']:,} bytes ({_signed(d['claimedFunctions'])}) |",
             f"| Perfect source moves | {len(r['diff']['perfectRenames'])} R100 |"]
    # Only when it happened. A retracted claim is news; a zero here would be noise on
    # every other pull request.
    if r.get("matchedWithdrawn"):
        lines.append(
            f"| Claims withdrawn (banner added) | {len(r['matchedWithdrawn'])} "
            f"function(s), none byte-verified |")
    # The delinks view of the same quantity. Identical to byte-verified above whenever
    # every enrolled range is a matched symbols.txt function, which is the healthy
    # state -- so it earns a row only when the two disagree, where the disagreement is
    # the news: an enrolled range with no matched function behind it, or a matched
    # function whose enrolled entry the symbol table does not know about.
    if s["head"]["sourceFunctions"] != h["verifiedFunctions"]:
        lines.append(
            f"| Enrolled ranges (delinks `complete`) | {s['head']['sourceFunctions']:,} "
            f"functions, {s['head']['sourceBytes']:,} bytes "
            f"({s['head']['sourceBytesPercent']:.2f}%, {_signed(d['sourceBuiltBytes'])}) "
            f"-- differs from byte-verified by "
            f"{s['head']['sourceFunctions'] - h['verifiedFunctions']:+,} |")
    lines += [
             f"| Contributor credit | {len(r['attribution']['added'])} added, "
             f"{len(r['attribution']['changed'])} changed, "
             f"{len(r['attribution']['lost'])} lost |",
             f"| Relocation check | {l['checked']} checked; {relocation_summary} |"]
    # Optional phase: no row at all when it did not run, so an older worker's
    # report reads exactly as it did before.
    port = r.get("portRefcheck") or {}
    if port.get("available"):
        lines.append(f"| Port reference check | {port['checked']} checked; "
                     f"{len(port['failures'])} stale |")
    head_rom = r["rom"]["head"]
    if head_rom.get("analysis"):
        mf = head_rom["analysis"]["moduleFidelity"]
        sf = head_rom["analysis"]["sourceBuild"]
        lines.append(f"| Module fidelity | {mf['modulesExact']}/{mf['modulesChecked']} exact; "
                     f"{mf['percent']:.6f}% compared bytes |")
        lines.append(f"| Code linked from verified source | {sf['sourceFunctions']:,} functions, "
                     f"{sf['sourceBytes']:,} bytes ({sf['sourceBytesPercent']:.2f}%) |")
        # What the 100.000000% above is a percentage OF. Every byte not built from
        # source is one dsd handed back from the cartridge and compared against itself.
        mc = head_rom["analysis"].get("moduleComposition")
        if mc:
            unowned_data = mc.get("unownedDataBytes", mc["dataBytes"])
            unowned_percent = (100.0 * unowned_data / mc["moduleBytes"]
                               if mc["moduleBytes"] else 0.0)
            lines.append(
                f"| Module bytes from source | {mc['sourceBytes']:,} / "
                f"{mc['moduleBytes']:,} ({mc['sourceBytesOfModulePercent']:.1f}%); "
                f"{unowned_data:,} ({unowned_percent:.1f}%) are data "
                f"no delink entry reaches |")
            if mc.get("retailGapBytes") is not None:
                lines.append(
                    f"| Retail-gap contribution | {mc['retailGapBytes']:,} module "
                    f"bytes ({mc.get('retailGapCodeBytes', 0):,} function-code; "
                    f"{mc.get('unownedDataBytes', mc['dataBytes']):,} "
                    "data/non-function) |")
    rom_data = head_rom.get("romData")
    if rom_data:
        lines.append(f"| ROM data reproduced from source | {rom_data['verified']:,} "
                     f"symbol(s) exact, {rom_data['partial']:,} partial, "
                     f"{rom_data['differs']:,} differ |")
    elif head_rom.get("failure"):
        lines.append(f"| Full ROM build | {head_rom['failure'].get('phase')} failed |")
    else:
        lines.append("| Full ROM build | not supplied |")
    lines += _credit_table(r["attribution"])
    if h.get("claimedFunctions"):
        lines += ["", f"*Byte-verified* means the range carries `complete` in a "
                      f"`delinks.txt`, so the ROM build compiled it and compared it to "
                      f"the cartridge. The {h['claimedFunctions']:,} *claimed* functions "
                      f"have a `src/` file named after the symbol with no `NONMATCHING` "
                      f"banner, and nothing compiles them -- dsd fills their addresses "
                      f"with the ROM's own bytes. Both together are the "
                      f"{h['matchedFunctions']:,} this project calls matched."]
    lines += _module_fidelity_detail(head_rom)
    lines += _rom_failure_detail(head_rom)
    if r["warnings"]:
        lines += ["", "Warnings: " + "; ".join(r["warnings"]) + "."]
    return "\n".join(lines)


SHOWN = 12


def _rom_failure_detail(head_rom):
    """Print the compiler's own words when the ROM build dies, not just the phase name.

    rombuild.py already captures up to 4000 chars of the failing phase's output into
    failure["output"] (rombuild.py:799), and the summary table rendered only
    failure["phase"] -- so a build that died on one bad #include reached the author as
    the four words "mwccarm failed" and nothing else. That is unactionable from the PR
    page and actively misleading when the box and the author's machine disagree: the
    author sees a clean local build and has no way to learn what the box saw. Same
    defect and same remedy as _module_fidelity_detail above.

    Returns markdown lines, or [] when the build did not fail or said nothing.
    """
    failure = (head_rom or {}).get("failure")
    if not failure:
        return []
    out = (failure.get("output") or "").strip()
    if not out:
        return []
    LIMIT = 3000
    if len(out) > LIMIT:
        out = "[... earlier output trimmed, showing the last " + str(LIMIT) + " chars ...]" + chr(10) + out[-LIMIT:]
    phase = failure.get("phase")
    code = failure.get("returncode")
    return ["",
            "<details><summary>Full ROM build output (%s failed, exit %s)</summary>" % (phase, code),
            "", "```", out, "```", "", "</details>"]


def _module_fidelity_detail(head_rom):
    """Name the modules and functions behind a `105/106`, instead of only counting them.

    The worker already ships all of this -- `moduleFidelity.results` carries a row per
    module and `failures` a row per function range -- and the summary table used to
    drop both. A bare "105/106 exact; 99.999738% compared bytes" says a few bytes are
    wrong somewhere in three million and gives no way to find them, while every
    per-file check on the PR stays green, because a per-file check compiles with the
    right pin and the module compare is the only thing that does not. That combination
    cost a full day on #1607. rombuild_check.py has printed this to the worker's
    console all along; this puts it in the comment.

    Returns markdown lines, or [] when the build is exact or said nothing.
    """
    analysis = (head_rom or {}).get("analysis")
    if not analysis:
        return []
    mf = analysis.get("moduleFidelity") or {}
    bad_modules = [m for m in (mf.get("results") or []) if not m.get("exact")]
    failures = analysis.get("failures") or []
    if not bad_modules and not failures:
        return []

    out = ["", f"**Module fidelity detail --- {len(bad_modules)} module(s) differ "
               f"from the cartridge.**", ""]
    if bad_modules:
        out += ["| Module | Bytes differing | Module bytes |", "|---|---:|---:|"]
        for m in sorted(bad_modules, key=lambda m: -m.get("differingBytes", 0))[:SHOWN]:
            out.append(f"| `{m.get('module')}` | {m.get('differingBytes', 0):,} "
                       f"| {m.get('comparedBytes', 0):,} |")
        out.append("")

    if failures:
        out += ["Function ranges that account for it:", "",
                "| Module | Function | Address | Size | Bytes differing |",
                "|---|---|---|---:|---:|"]
        for f in sorted(failures,
                        key=lambda f: -(f.get("differingBytes") or 0))[:SHOWN]:
            detail = (f"{f['differingBytes']:,}" if f.get("differingBytes") is not None
                      else f.get("reason", "?"))
            out.append(f"| `{f.get('module')}` | `{f.get('name')}` "
                       f"| 0x{f.get('addr', 0):08x} | 0x{f.get('size', 0):x} "
                       f"| {detail} |")
        if len(failures) > SHOWN:
            out.append(f"| ... | {len(failures) - SHOWN} more | | | |")
        out.append("")
    else:
        # The distinction worth calling out: a module can differ with every enrolled
        # function exact, when the differing bytes fall outside every delink range
        # (padding, alignment, or data no entry reaches). Nothing per-function can
        # ever surface those, so say so rather than leave an empty section.
        out += ["No enrolled function range accounts for these bytes --- they fall "
                "outside every `delinks.txt` range, so no per-function or per-file "
                "check can see them.", ""]

    # A function that is pinned to a non-default mwccarm and is ALSO mismatching is
    # the signature of a pin that did not apply: the source is fine, the compiler was
    # wrong. Worth naming, because the fix is a config re-key, not a decomp change.
    pinned = ((analysis.get("alternateToolchain") or {}).get("applied")
              or (head_rom or {}).get("alternateToolchain", {}).get("applied") or {})
    hits = [f for f in failures if f.get("name") in pinned]
    if hits:
        out += ["Pinned to a non-default mwccarm in `config/rombuild-versions.txt`: "
                + ", ".join(f"`{f['name']}` ({pinned[f['name']]})" for f in hits)
                + ". A pinned function that mismatches usually means the ROM build "
                  "did not apply its pin --- check `alternateToolchainFiles` in the "
                  "build report before treating this as a source defect.", ""]
    return out


def _credit_table(a):
    """Spell out every credit the merge moved or dropped.

    The summary line has room for CREDIT_NAMED of them; this is the check body, where
    a PR author can actually read the list and see whether the moves are the ones the
    pull request meant to make.  Added credit is not listed -- new work earning new
    credit is what a merge is for, and it is never what fails the check.
    """
    rows = ([(c["id"], c["path"], c.get("basePath", c["path"]), c["before"], c["after"])
             for c in a["changed"]]
            + [(c["id"], c["path"], c["path"], c["author"], "nobody") for c in a["lost"]])
    if not rows:
        return []
    lines = ["", f"#### Contributor credit moved ({len(rows)})", "",
             "| Function | Source | Before | After |", "|---|---|---|---|"]
    for fid, path, base_path, before, after in rows[:CREDIT_ROWS]:
        # A rename that also moves credit is the case worth seeing whole, so the old
        # path stays visible rather than being silently replaced by the new one.
        where = path if base_path == path else f"{base_path} -> {path}"
        lines.append(f"| `{fid}` | `{where}` | {before} | {after} |")
    if len(rows) > CREDIT_ROWS:
        lines += ["", f"+{len(rows) - CREDIT_ROWS} more; the full list is in the "
                      "JSON report's `attribution` section."]
    return lines


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--base", required=True)
    ap.add_argument("--head", default="HEAD")
    ap.add_argument("--require-merge-commit", action="store_true")
    ap.add_argument("--expected-pr-head",
                    help="exact commit required as the merge's second parent")
    ap.add_argument("--base-rom-report")
    ap.add_argument("--head-rom-report")
    ap.add_argument("--link-report")
    ap.add_argument("--port-refcheck-report",
                    help="tools/port_refcheck.py --json output; the check is "
                         "reported only when this is supplied and exists")
    ap.add_argument("--out", required=True)
    ap.add_argument("--markdown")
    args = ap.parse_args()
    report = build_report(args.base, args.head, _load_json(args.base_rom_report),
                          _load_json(args.head_rom_report), _load_json(args.link_report),
                          args.require_merge_commit, args.expected_pr_head,
                          _load_json(args.port_refcheck_report, optional=True))
    pathlib.Path(args.out).write_text(json.dumps(report, indent=2) + "\n",
                                      encoding="utf-8", newline="\n")
    if args.markdown:
        pathlib.Path(args.markdown).write_text(report["reportMarkdown"] + "\n",
                                               encoding="utf-8", newline="\n")
    print(report["reportMarkdown"])
    return 0 if report["status"] == "Passed" else 1


if __name__ == "__main__":
    sys.exit(main())
