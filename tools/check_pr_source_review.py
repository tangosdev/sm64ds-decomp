"""Publish Source review from trusted queue data; never check out or run PR code.

Read-only by default. --publish creates a GitHub Actions check when run by the
trusted workflow. Repository rules must separately require that app-owned check.
"""
import argparse
import json
import re
import subprocess
import sys

import source_review as sr


def evaluate(state, head, base, files):
    required = {p.casefold() for p in files if sr.source_path(p)}
    if not required:
        return {"result": "pass", "summary": "No reconstruction source, header or TU manifest changed."}
    if state.get("schema") != 3 or not state.get("source_review_policy"):
        return {"result": "fail", "summary": "Source review has not been activated for the fleet."}
    covered, accepted, errors = set(), [], []
    policy = state["source_review_policy"]["workflow_commit"]
    for task in state.get("tasks", {}).values():
        if task.get("phase") == "cancelled":
            continue
        stages = {s["id"]: s for s in task.get("stages", [])}
        for output in task.get("outputs", []):
            if stages.get(output.get("stage_id"), {}).get("mode") != "verify":
                continue
            evidence = output.get("evidence", {})
            reviewer = output.get("session")
            # A composed PR needs its own independently published verify output.
            # An integrator's nested assertion cannot impersonate that reviewer.
            if output.get("commit") != head:
                continue
            try:
                review = sr.validate_evidence(task, evidence, head, reviewer)
                sr.require(evidence.get("tested_base") == base, "PR base changed since source review")
                sr.require(evidence.get("workflow_commit") == policy, "review uses an older policy")
                covered.update(p.casefold() for p in review["files"])
                accepted.append(task["task_id"])
            except (sr.ReviewError, KeyError, TypeError) as exc:
                errors.append(task.get("task_id", "unknown task") + ": " + str(exc))
    missing = sorted(required - covered)
    if missing:
        return {"result": "fail", "summary": "Independent source review is missing or invalid.",
                "missing_files": missing, "errors": errors, "accepted_tasks": sorted(set(accepted))}
    return {"result": "pass", "summary": "Independent source review covers this PR head and base.",
            "accepted_tasks": sorted(set(accepted))}


def api(path, payload=None, paginate=False, raw=False):
    args = ["gh", "api", path]
    if raw:
        args += ["-H", "Accept: application/vnd.github.raw+json"]
    if paginate:
        args += ["--paginate", "--slurp"]
    if payload is not None:
        args += ["--method", "POST", "--input", "-"]
    result = subprocess.run(args, input=json.dumps(payload) if payload is not None else None,
                            capture_output=True, text=True, encoding="utf-8")
    if result.returncode:
        raise RuntimeError("GitHub API request failed: " + result.stderr.strip())
    return json.loads(result.stdout)


def queue_state(repo):
    ref = api(f"repos/{repo}/git/ref/heads/agents/coordination")
    sha = ref["object"]["sha"]
    # The default Contents response omits inline content above 1 MiB.
    contents = api(f"repos/{repo}/contents/state.json?ref={sha}", raw=True)
    return sha, contents


def check_pr(repo, number, publish=False):
    pr = api(f"repos/{repo}/pulls/{number}")
    if pr["state"] != "open":
        return {"pr": number, "result": "closed"}
    head, base = pr["head"]["sha"], pr["base"]["sha"]
    state_sha = None
    try:
        pages = api(f"repos/{repo}/pulls/{number}/files?per_page=100", paginate=True)
        files = [f for page in pages for f in page]
        sr.require(len(files) == pr["changed_files"], "incomplete PR file list; review coverage unknown")
        paths = [f["filename"] for f in files]
        paths += [f["previous_filename"] for f in files if f.get("previous_filename")]
        if any(sr.source_path(p) for p in paths):
            state_sha, state = queue_state(repo)
        else:
            state = {}
        report = evaluate(state, head, base, paths)
    except (RuntimeError, sr.ReviewError, ValueError, KeyError, TypeError) as exc:
        report = {"result": "fail", "summary": "Source review could not be established: " + str(exc)}
    report.update(pr=number, head=head, base=base, queue_commit=state_sha)
    if publish:
        current = api(f"repos/{repo}/pulls/{number}")
        if (current["head"]["sha"], current["base"]["sha"], current["state"]) != (head, base, "open"):
            raise RuntimeError("PR changed while checking; rerun before publishing a verdict")
        if state_sha is not None:
            current_queue = api(f"repos/{repo}/git/ref/heads/agents/coordination")["object"]["sha"]
            if current_queue != state_sha:
                report.update(result="fail", summary="Queue changed during review; refresh the check.")
        api(f"repos/{repo}/check-runs", {
            "name": "Source review", "head_sha": head, "status": "completed",
            "conclusion": "success" if report["result"] == "pass" else "failure",
            "external_id": f"source-review:{number}:{head}:{base}:{state_sha}",
            "output": {"title": report["summary"][:250],
                       "summary": "```json\n" + json.dumps(report, indent=2)[:60000] + "\n```"}})
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", default="tangosdev/sm64ds-decomp")
    parser.add_argument("--pr", type=int)
    parser.add_argument("--publish", action="store_true")
    args = parser.parse_args(argv)
    if not re.fullmatch(r"[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+", args.repo):
        parser.error("--repo must be owner/repository")
    try:
        numbers = [args.pr] if args.pr else [pr["number"] for page in api(
            f"repos/{args.repo}/pulls?state=open&per_page=100", paginate=True) for pr in page]
        reports = [check_pr(args.repo, n, args.publish) for n in numbers]
        print(json.dumps(reports, indent=2))
        return int(any(r["result"] == "fail" for r in reports))
    except (RuntimeError, ValueError, KeyError, TypeError) as exc:
        print("Source review unavailable: " + str(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
