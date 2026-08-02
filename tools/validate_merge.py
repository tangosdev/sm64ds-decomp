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
import chaos_db_ci as CHAOS  # noqa: E402
import rombuild_check as RBC  # noqa: E402

FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)")
SOURCE_SUFFIXES = (".c", ".cpp")


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
            matched = bool(src and src not in nonmatching)
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
        by_function[key] = author
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


def _load_json(path):
    if not path:
        return None
    return json.loads(pathlib.Path(path).read_text(encoding="utf-8"))


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
                for result in row["results"]:
                    flat = {"file": row.get("file"), **result}
                    if draft and str(flat.get("verdict")) == "NO-REPRO":
                        flat["verdict"] = "DRAFT"
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
        if bucket in ("WRONG", "NO-REPRO", "ERROR"):
            blocking.append(row)
    return {"checked": len(flattened), "tally": dict(tally), "blocking": blocking,
            "rows": flattened}


def build_report(base, head, base_rom=None, head_rom=None, link_rows=None,
                 require_merge_commit=False, expected_pr_head=None):
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

    base_keys, head_keys = set(bf["matched"]), set(hf["matched"])
    common_credit = set(ba["byFunction"]) & set(ha["byFunction"])
    credit_changes = [{"id": k, "before": ba["byFunction"][k], "after": ha["byFunction"][k]}
                      for k in sorted(common_credit)
                      if ba["byFunction"][k] != ha["byFunction"][k]]
    lost_credit = [{"id": k, "author": ba["byFunction"][k]}
                   for k in sorted(set(ba["byFunction"]) - set(ha["byFunction"]))]
    added_credit = [{"id": k, "author": ha["byFunction"][k]}
                    for k in sorted(set(ha["byFunction"]) - set(ba["byFunction"]))]

    base_rom_state, head_rom_state = _rom_state(base_rom), _rom_state(head_rom)
    rom_regression = (base_rom_state.get("passed") is True
                      and head_rom_state.get("passed") is not True)
    same_baseline_failure = (base_rom_state.get("passed") is False
                             and head_rom_state.get("passed") is False
                             and base_rom_state.get("signature") is not None
                             and base_rom_state.get("signature")
                             == head_rom_state.get("signature"))
    link = _link_state(link_rows)
    reasons = []
    if base_keys - head_keys:
        reasons.append(f"lost {len(base_keys - head_keys)} matched function(s)")
    if (hf["stats"]["totalFunctions"] != bf["stats"]["totalFunctions"]
            or hf["stats"]["totalBytes"] != bf["stats"]["totalBytes"]):
        reasons.append("function/byte coverage denominator changed")
    if he["stats"]["sourceBytes"] < be["stats"]["sourceBytes"]:
        reasons.append("source-built byte coverage decreased")
    if credit_changes or lost_credit:
        reasons.append("contributor attribution changed or was lost")
    if ha["stats"]["unattributedFunctions"] > ba["stats"]["unattributedFunctions"]:
        reasons.append("the merge introduced an unattributed matched function")
    if diff["leftoverOldPaths"]:
        reasons.append("old source paths remain after rename")
    if link["blocking"]:
        reasons.append(f"{len(link['blocking'])} blocking relocation verdict(s)")
    if rom_regression:
        reasons.append("full-ROM result regressed from the base commit")
    if head_rom_state.get("available") and not head_rom_state.get("passed") \
            and not same_baseline_failure:
        reasons.append("full-ROM validation failed")

    warnings = []
    if same_baseline_failure:
        warnings.append("base and merge share the same pre-existing ROM-build failure")
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
    report = {
        "schemaVersion": 1,
        "status": "Failed" if reasons else "Passed",
        "baseSha": base_sha,
        "headSha": head_sha,
        "committedMerge": is_merge,
        "summary": "Validation failed: " + "; ".join(reasons) if reasons
                   else "Committed merge introduces no reconstruction or attribution regression.",
        "reasons": reasons,
        "warnings": warnings,
        "coverage": {"base": bf["stats"], "head": hf["stats"], "delta": coverage_delta},
        "sourceBuild": {"base": base_source_stats, "head": head_source_stats},
        "attribution": {"base": ba["contributors"], "head": ha["contributors"],
                        "baseStats": ba["stats"], "headStats": ha["stats"],
                        "added": added_credit, "changed": credit_changes,
                        "lost": lost_credit},
        "diff": diff,
        "linkcheck": link,
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
             f"{len(r['attribution']['lost'])} lost |",
             f"| Relocation check | {l['checked']} checked; {relocation_summary} |"]
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
    if r["warnings"]:
        lines += ["", "Warnings: " + "; ".join(r["warnings"]) + "."]
    return "\n".join(lines)


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
    ap.add_argument("--out", required=True)
    ap.add_argument("--markdown")
    args = ap.parse_args()
    report = build_report(args.base, args.head, _load_json(args.base_rom_report),
                          _load_json(args.head_rom_report), _load_json(args.link_report),
                          args.require_merge_commit, args.expected_pr_head)
    pathlib.Path(args.out).write_text(json.dumps(report, indent=2) + "\n",
                                      encoding="utf-8", newline="\n")
    if args.markdown:
        pathlib.Path(args.markdown).write_text(report["reportMarkdown"] + "\n",
                                               encoding="utf-8", newline="\n")
    print(report["reportMarkdown"])
    return 0 if report["status"] == "Passed" else 1


if __name__ == "__main__":
    sys.exit(main())
