"""Classify open GitHub pull requests without papering over their gates.

Usage:
    python tools/pr_monitor.py
    python tools/pr_monitor.py --repo tangosdev/sm64ds-decomp --skip 1450

The command is intentionally read-only.  It identifies the next repair or review
candidate; a reviewer must inspect the diff and CI evidence before approving.
"""

from __future__ import annotations

import argparse
import datetime as dt
import json
import subprocess
import sys
from typing import Any


FAILURES = {
    "ACTION_REQUIRED", "CANCELLED", "FAILURE", "STALE", "STARTUP_FAILURE",
    "TIMED_OUT",
}


def classify(pr: dict[str, Any], now: dt.datetime, minimum_age: int,
             skipped: set[int]) -> tuple[str, str]:
    """Return a conservative queue state and its reason for one PR payload."""
    number = pr["number"]
    if number in skipped:
        return "skipped", "explicitly skipped"
    if pr.get("isDraft"):
        return "draft", "author still has it in draft"

    checks = pr.get("statusCheckRollup") or []
    failed = []
    pending = []
    for check in checks:
        name = check.get("name", "unnamed check")
        conclusion = (check.get("conclusion") or "").upper()
        if check.get("status") != "COMPLETED":
            pending.append(name)
        elif conclusion in FAILURES:
            failed.append(name)
        elif conclusion not in {"SUCCESS", "NEUTRAL", "SKIPPED"}:
            pending.append(name)
    if pr.get("mergeStateStatus") == "DIRTY":
        return "conflict", "GitHub reports merge conflicts"
    if failed:
        return "failed", "failed check: " + ", ".join(failed)
    if pending or not checks:
        detail = ", ".join(pending) if pending else "checks have not appeared"
        return "waiting", detail

    created = dt.datetime.fromisoformat(pr["createdAt"].replace("Z", "+00:00"))
    age = now - created
    if age < dt.timedelta(minutes=minimum_age):
        return "cooldown", f"open for {int(age.total_seconds() // 60)}m"
    if pr.get("mergeStateStatus") != "CLEAN":
        return "merge-pending", "checks pass; GitHub mergeability is " + pr.get(
            "mergeStateStatus", "unknown")
    return "reviewable", "checks pass, conflict-free, and old enough"


def _run(*args: str) -> str:
    result = subprocess.run(args, text=True, capture_output=True)
    if result.returncode:
        raise RuntimeError(result.stderr.strip() or "command failed: " + " ".join(args))
    return result.stdout


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--repo", help="OWNER/REPO passed to gh; required outside a checkout")
    ap.add_argument("--skip", type=int, action="append", default=[1450],
                    help="PR number to omit (repeatable; default: 1450)")
    ap.add_argument("--minimum-age", type=int, default=15,
                    help="minutes a clean PR must be open before review (default: 15)")
    args = ap.parse_args(argv)
    fields = ("number,title,isDraft,mergeStateStatus,reviewDecision,statusCheckRollup,"
              "createdAt,url")
    command = ["gh", "pr", "list", "--state", "open", "--limit", "100",
               "--json", fields]
    if args.repo:
        command.extend(["--repo", args.repo])
    try:
        prs = json.loads(_run(*command))
    except (RuntimeError, json.JSONDecodeError) as exc:
        print(f"pr_monitor: {exc}", file=sys.stderr)
        return 2

    now = dt.datetime.now(dt.timezone.utc)
    states: dict[str, list[tuple[dict[str, Any], str]]] = {}
    for pr in prs:
        state, reason = classify(pr, now, args.minimum_age, set(args.skip))
        states.setdefault(state, []).append((pr, reason))

    order = ("conflict", "failed", "waiting", "merge-pending", "reviewable",
             "cooldown", "draft", "skipped")
    for state in order:
        for pr, reason in states.get(state, []):
            print(f"{state:13} #{pr['number']:4}  {pr['title']} ({reason})")
    for state in order:
        if states.get(state) and state in {"conflict", "failed", "reviewable"}:
            pr, _ = states[state][0]
            print(f"\nnext: {state} #{pr['number']} {pr['url']}")
            break
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
