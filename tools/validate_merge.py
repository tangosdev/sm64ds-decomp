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
    # marker is a source header and is recognized in the first 200 characters.
    nonmatching = {path for path in candidates
                   if "NONMATCHING" in git_text(rev, path)[:200]}
    # An unbannered dcd transcription byte-matches vacuously (it IS the ROM words
    # re-spelled), so it never counts as matched -- see tools/asm_policy.py. Built
    # the same revision-based way as ``nonmatching``: a cheap fixed-string grep
    # prefilter, then the shared classifier confirms on the committed blob.
    grep = _git("grep", "-l", "-F", "dcd 0x", rev, "--", "src/", allow=(0, 1))
    dcd_candidates = {line.split(":", 1)[1] if ":" in line else line
                      for line in grep.splitlines()}
    transcribed = {path for path in dcd_candidates
                   if AP.classify(git_text(rev, path)) == "transcribed"}

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
            src = sources.get(name)
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
    return {"entries": entries, "source": src, "mods": mods,
            "stats": {"sourceFunctions": len(src),
                      "sourceBytes": sum(e["size"] for e in src.values()),
                      "modFunctions": len(mods),
                      "modBytes": sum(e["size"] for e in mods.values())}}


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
        author = overrides.get(path) or finishers.get(path) or first.get(path)
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
            "failure": failure}


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
                 port_report=None, allow_attribution_change=False):
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
    ba, ha = attribution_snapshot(base_sha, bf), attribution_snapshot(head_sha, hf)
    diff = diff_snapshot(base_sha, head_sha, set(tree_paths(head_sha, "src/")))

    # The transcription gate for the PR itself, scoped STRICTLY to the sources this
    # merge adds or modifies (never renames-only or pre-existing files, so an old
    # transcription in the tree cannot fail an unrelated PR). Classified from the
    # committed head blob, same discipline as everything else in this report.
    changed_src = [row["path"] for row in diff["files"]
                   if row["status"][:1] in ("A", "M") and row.get("path", "")
                   .endswith(SOURCE_SUFFIXES)]
    changed_cls = {p: AP.classify(git_text(head_sha, p)) for p in sorted(changed_src)}
    new_transcribed = [p for p, c in changed_cls.items() if c == "transcribed"]
    new_unbannered = [p for p, c in changed_cls.items() if c == "unbannered-asm"]

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
    if base_keys - head_keys:
        reasons.append(f"lost {len(base_keys - head_keys)} matched function(s)")
    if (hf["stats"]["totalFunctions"] != bf["stats"]["totalFunctions"]
            or hf["stats"]["totalBytes"] != bf["stats"]["totalBytes"]):
        reasons.append("function/byte coverage denominator changed")
    if he["stats"]["sourceBytes"] < be["stats"]["sourceBytes"]:
        reasons.append("source-built byte coverage decreased")
    # The attribution-override label (carried on the job by tangos-backend) makes every
    # attribution finding advisory for this one pull request.  The finding is still
    # computed and still named -- an override says "I know, and I meant it", not "do not
    # look" -- it just stops being a reason the merge cannot land.
    credit_moved = bool(credit_changes or lost_credit)
    new_unattributed = (ha["stats"]["unattributedFunctions"]
                        > ba["stats"]["unattributedFunctions"])
    if credit_moved and not allow_attribution_change:
        reasons.append("contributor attribution changed or was lost "
                       f"({_credit_detail(credit_changes, lost_credit)}); label the pull "
                       "request attribution-override to accept it")
    if new_unattributed and not allow_attribution_change:
        reasons.append("the merge introduced an unattributed matched function")
    if diff["leftoverOldPaths"]:
        reasons.append("old source paths remain after rename")
    if new_transcribed:
        reasons.append(f"{len(new_transcribed)} asm-transcription file(s) with no "
                       f"HAND-ASM PRIMITIVE or NONMATCHING banner: "
                       + ", ".join(new_transcribed))
    if link["blocking"]:
        reasons.append(f"{len(link['blocking'])} blocking relocation verdict(s)")
    if port.get("available") and not port["passed"]:
        reasons.append(_port_reason(port))
    if rom_regression:
        reasons.append("full-ROM result regressed from the base commit")
    if head_rom_state.get("available") and not head_rom_state.get("passed") \
            and not same_baseline_failure:
        reasons.append("full-ROM validation failed")

    warnings = []
    if allow_attribution_change and credit_moved:
        warnings.append("attribution-override label: contributor attribution changed or "
                        f"was lost ({_credit_detail(credit_changes, lost_credit)}), "
                        "accepted without failing the pull request")
    if allow_attribution_change and new_unattributed:
        warnings.append("attribution-override label: the merge introduced an "
                        "unattributed matched function, accepted without failing "
                        "the pull request")
    if same_baseline_failure:
        warnings.append("base and merge share the same pre-existing ROM-build failure")
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

    coverage_delta = {
        "matchedFunctions": hf["stats"]["matchedFunctions"] - bf["stats"]["matchedFunctions"],
        "matchedBytes": hf["stats"]["matchedBytes"] - bf["stats"]["matchedBytes"],
        "sourceBuiltFunctions": he["stats"]["sourceFunctions"] - be["stats"]["sourceFunctions"],
        "sourceBuiltBytes": he["stats"]["sourceBytes"] - be["stats"]["sourceBytes"],
        "newMatchedFunctions": len(head_keys - base_keys),
        "removedMatchedFunctions": len(base_keys - head_keys),
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
    elif allow_attribution_change and (credit_moved or new_unattributed):
        # Passing *because* of the label is not the same verdict as nothing having
        # moved, and the one-liner is what most people read.
        summary = ("Committed merge passes; the attribution-override label accepted "
                   f"{len(credit_changes)} credit change(s), {len(lost_credit)} lost.")
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
        "coverage": {"base": bf["stats"], "head": hf["stats"], "delta": coverage_delta},
        "sourceBuild": {"base": base_source_stats, "head": head_source_stats},
        "attribution": {"base": ba["contributors"], "head": ha["contributors"],
                        "baseStats": ba["stats"], "headStats": ha["stats"],
                        "added": added_credit, "changed": credit_changes,
                        "lost": lost_credit, "overridden": allow_attribution_change},
        "diff": diff,
        "asmPolicy": {"transcribed": new_transcribed, "unbanneredAsm": new_unbannered},
        "linkcheck": link,
        "portRefcheck": port,
        "rom": {"base": base_rom_state, "head": head_rom_state,
                "regression": rom_regression, "sameBaselineFailure": same_baseline_failure},
    }
    report["reportMarkdown"] = render_markdown(report)
    return report


def _signed(n):
    return f"{n:+,}"


def render_markdown(r):
    c, s, l = r["coverage"], r["sourceBuild"], r["linkcheck"]
    h, d = c["head"], c["delta"]
    relocation_summary = ", ".join(
        f"{v} {k}" for k, v in sorted(l["tally"].items())) or "no affected slots"
    lines = ["### Full merge validation", "",
             "| Check | Result |", "|---|---|",
             f"| Committed test merge | {'yes' if r['committedMerge'] else 'no'} |",
             f"| Matched functions | {h['matchedFunctions']:,} / {h['totalFunctions']:,} "
             f"({h['matchedFunctionPercent']:.1f}%, {_signed(d['matchedFunctions'])}) |",
             f"| Matched code bytes | {h['matchedBytes']:,} / {h['totalBytes']:,} "
             f"({h['matchedBytePercent']:.1f}%, {_signed(d['matchedBytes'])}) |",
             f"| Tracked source enrollment | {s['head']['sourceFunctions']:,} functions, "
             f"{s['head']['sourceBytes']:,} bytes "
             f"({s['head']['sourceBytesPercent']:.2f}%, {_signed(d['sourceBuiltBytes'])}) |",
             f"| Perfect source moves | {len(r['diff']['perfectRenames'])} R100 |",
             f"| Contributor credit | {len(r['attribution']['added'])} added, "
             f"{len(r['attribution']['changed'])} changed, "
             f"{len(r['attribution']['lost'])} lost"
             f"{' (override label)' if r['attribution'].get('overridden') else ''} |",
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
    elif head_rom.get("failure"):
        lines.append(f"| Full ROM build | {head_rom['failure'].get('phase')} failed |")
    else:
        lines.append("| Full ROM build | not supplied |")
    lines += _credit_table(r["attribution"])
    if r["warnings"]:
        lines += ["", "Warnings: " + "; ".join(r["warnings"]) + "."]
    return "\n".join(lines)


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
    ap.add_argument("--allow-attribution-change", action="store_true",
                    help="report attribution changes as warnings instead of failing "
                         "the merge; the worker passes this for a pull request "
                         "carrying the attribution-override label")
    ap.add_argument("--out", required=True)
    ap.add_argument("--markdown")
    args = ap.parse_args()
    report = build_report(args.base, args.head, _load_json(args.base_rom_report),
                          _load_json(args.head_rom_report), _load_json(args.link_report),
                          args.require_merge_commit, args.expected_pr_head,
                          _load_json(args.port_refcheck_report, optional=True),
                          args.allow_attribution_change)
    pathlib.Path(args.out).write_text(json.dumps(report, indent=2) + "\n",
                                      encoding="utf-8", newline="\n")
    if args.markdown:
        pathlib.Path(args.markdown).write_text(report["reportMarkdown"] + "\n",
                                               encoding="utf-8", newline="\n")
    print(report["reportMarkdown"])
    return 0 if report["status"] == "Passed" else 1


if __name__ == "__main__":
    sys.exit(main())
