"""Publish Source review from trusted queue data; never check out or run PR code.

Read-only by default. --publish creates a GitHub Actions check when run by the
trusted workflow. Repository rules must separately require that app-owned check.
"""
import argparse
import json
import re
import subprocess
import sys
from urllib.parse import quote

import source_review as sr

# A scheduled progress push advances main without changing anything a source
# review reads. Skipping only those commits keeps the base currency requirement
# satisfiable; every other commit on main still retires review acceptance.
PROGRESS_BOT = "github-actions[bot]"
COMMIT_FILE_CAP = 300  # The commit endpoint caps its file list; a cap hides scope.
PROGRESS_WALK_LIMIT = 50
_refresh_parents = {}


def evaluate(state, head, bases, files):
    """Judge the head against every target commit a source review could have read.

    `bases` is ordered newest first: the branch tip, then each scheduled progress
    commit skipped beneath it. Those commits differ only in paths no review
    inspects, so a review recorded against any of them reviewed the same source.
    """
    reviewable = frozenset(bases)
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
                sr.require(evidence.get("tested_base") in reviewable,
                           "PR base changed since source review")
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


def progress_refresh(data, sha):
    """True for a single-parent bot commit that changes nothing a review inspects."""
    parents, files = data.get("parents"), data.get("files")
    if (data.get("sha") != sha or not isinstance(parents, list) or len(parents) != 1
            or not isinstance(files, list) or len(files) >= COMMIT_FILE_CAP
            or any(not isinstance(who, dict) or who.get("login") != PROGRESS_BOT
                   for who in (data.get("author"), data.get("committer")))):
        return False
    for item in files:
        if not isinstance(item, dict) or "filename" not in item:
            return False
        for path in (item["filename"], item.get("previous_filename", "")):
            if not isinstance(path, str) or sr.source_path(path):
                return False
    return True


def refresh_parent(repo, sha):
    """Parent of a scheduled progress commit, or None for every other commit.

    Main advances on a timer: the progress-refresh workflow pushes README, docs
    and contributions.json under `github-actions[bot]` with no source content.
    Requiring every open composition to contain that push makes Source review
    unsatisfiable, because no branch can outrun a scheduler. Only commits that
    change nothing a source review reads are skipped, so a real change to main
    still moves the review base and retires acceptance.
    """
    if (repo, sha) in _refresh_parents:
        return _refresh_parents[repo, sha]
    try:
        data = api(f"repos/{repo}/commits/{sha}")
        parent = (sr.commit(data["parents"][0]["sha"], "progress refresh parent")
                  if progress_refresh(data, sha) else None)
    except (RuntimeError, sr.ReviewError, ValueError, KeyError, TypeError):
        return None  # Unknown scope keeps the branch tip; it never widens acceptance.
    _refresh_parents[repo, sha] = parent  # Commits are immutable; one read per sweep.
    return parent


def target_branch(repo, pr):
    """Resolve the live target ref; PR base.sha can retain an older commit.

    Returns the branch and its reviewable target commits, newest first: the exact
    branch tip, then each scheduled progress commit skipped beneath it. Ancestry
    is judged against the oldest, acceptance against any of them.
    """
    base = pr["base"]
    base_repo = base["repo"]["full_name"]
    sr.require(isinstance(base_repo, str) and base_repo.casefold() == repo.casefold(),
               "PR targets a different repository")
    branch = base["ref"]
    sr.require(isinstance(branch, str) and bool(branch), "PR target branch is missing")
    ref = api(f"repos/{repo}/git/ref/heads/{quote(branch, safe='')}")
    sr.require(ref["ref"] == "refs/heads/" + branch, "API returned a different target ref")
    sr.require(ref["object"]["type"] == "commit", "Target branch does not identify a commit")
    bases = [sr.commit(ref["object"]["sha"], "target branch commit")]
    while len(bases) <= PROGRESS_WALK_LIMIT:
        parent = refresh_parent(repo, bases[-1])
        if parent is None:
            break
        bases.append(parent)
    return branch, tuple(bases)


def commit_tree(repo, commit):
    """Resolve an immutable commit to its tree without downloading source blobs."""
    data = api(f"repos/{repo}/git/commits/{commit}")
    sr.require(data["sha"] == commit, "API returned a different commit")
    return sr.commit(data["tree"]["sha"], "commit tree")


def read_tree(repo, sha, recursive=False):
    suffix = "?recursive=1" if recursive else ""
    data = api(f"repos/{repo}/git/trees/{sha}{suffix}")
    sr.require(data["sha"] == sha, "API returned a different tree")
    sr.require(type(data["truncated"]) is bool, "Tree completeness flag is missing or invalid")
    sr.require(isinstance(data["tree"], list), "Tree entries are missing or invalid")
    modes = {"100644": "blob", "100755": "blob", "120000": "blob",
             "040000": "tree", "160000": "commit"}
    entries = {}
    for item in data["tree"]:
        path, mode, kind = item["path"], item["mode"], item["type"]
        sr.require(isinstance(path, str) and bool(path) and
                   not any(c in path for c in "\\\0\r\n") and
                   all(part not in ("", ".", "..") for part in path.split("/")),
                   "Tree contains an invalid relative path")
        sr.require(recursive or "/" not in path, "Nonrecursive tree contains a nested path")
        sr.require(path not in entries, "Tree contains a duplicate path: " + path)
        sr.require(isinstance(mode, str) and modes.get(mode) == kind,
                   "Tree contains an invalid mode or object type")
        entries[path] = (mode, kind, sr.commit(item["sha"], "tree entry object"))
    if recursive and not data["truncated"]:
        for path in entries:
            parent = path.rpartition("/")[0]
            ancestors = {sha}
            while parent:
                entry = entries.get(parent)
                sr.require(entry is not None and entry[1] == "tree",
                           "Recursive tree is missing a parent directory")
                sr.require(entry[2] not in ancestors, "Tree contains a cycle")
                ancestors.add(entry[2])
                parent = parent.rpartition("/")[0]
            if entries[path][1] == "tree":
                sr.require(entries[path][2] not in ancestors, "Tree contains a cycle")
    return entries, data["truncated"]


def complete_tree(repo, sha, cache):
    """Enumerate every leaf, falling back to individual trees if recursion is capped."""
    entries, truncated = read_tree(repo, sha, recursive=True)
    if not truncated:
        return {path: entry for path, entry in entries.items() if entry[1] != "tree"}
    # GitHub documents recursive truncation at 100,000 entries or 7 MB. Discard
    # that partial result; only complete nonrecursive responses establish scope.
    leaves = {}
    pending = [("", sha, frozenset())]
    while pending:
        prefix, current, ancestors = pending.pop()
        sr.require(current not in ancestors, "Tree contains a cycle")
        if current not in cache:
            children, truncated = read_tree(repo, current)
            sr.require(not truncated, "Nonrecursive tree is truncated; review coverage unknown")
            cache[current] = children
        for name, entry in cache[current].items():
            path = prefix + name
            if entry[1] == "tree":
                pending.append((path + "/", entry[2], ancestors | {current}))
            else:
                sr.require(path not in leaves, "Tree contains a duplicate path: " + path)
                leaves[path] = entry
    return leaves


def locate_composition(repo, base, head):
    """Find where the composition left the target, and whether it still contains it.

    Two different questions share one comparison. Scope -- what this PR changes --
    is measured from the merge base, which is the content a reviewer reads and the
    only content merging the PR can introduce. Currency -- whether an existing
    review still applies -- is the separate judgement the caller imposes on a PR
    that actually changes reviewable source (queue-v2.md and
    SOURCE-REVIEW-CUTOVER.md): a stale branch needs a new composition and review.
    Compare metadata establishes ancestry only: its files list is capped at 300,
    even with pagination.
    """
    base = sr.commit(base, "comparison base")
    head = sr.commit(head, "comparison head")
    comparison = api(f"repos/{repo}/compare/{base}...{head}?per_page=1")
    sr.require(comparison["base_commit"]["sha"] == base,
               "Comparison returned a different base")
    merge_base = sr.commit(comparison["merge_base_commit"]["sha"], "comparison merge base")
    status = comparison["status"]
    sr.require(status in ("ahead", "behind", "identical", "diverged"),
               "Comparison returned an unusable status; the composition cannot be scoped")
    return merge_base, merge_base == base and status in ("ahead", "identical")


def changed_paths(repo, base, head):
    """Compare the exact composition, not PR /files metadata for a cached base.

    Git tree objects supply the complete changed-path set. Leaf comparison retains
    both rename sides and mode/type changes, including symlinks, submodules, and
    file/directory replacements.
    """
    base = sr.commit(base, "comparison base")
    head = sr.commit(head, "comparison head")
    before_tree, after_tree = commit_tree(repo, base), commit_tree(repo, head)
    if before_tree == after_tree:
        return []
    cache = {}
    before = complete_tree(repo, before_tree, cache)
    after = complete_tree(repo, after_tree, cache)
    return sorted(path for path in before.keys() | after.keys()
                  if before.get(path) != after.get(path))


def check_pr(repo, number, publish=False):
    pr = api(f"repos/{repo}/pulls/{number}")
    if pr["state"] != "open":
        return {"pr": number, "result": "closed"}
    head = pr["head"]["sha"]
    base, tip, branch = None, None, None
    state_sha = None
    try:
        branch, bases = target_branch(repo, pr)
        tip, base = bases[0], bases[-1]
        merge_base, retains_base = locate_composition(repo, base, head)
        paths = changed_paths(repo, merge_base, head)
        if any(sr.source_path(p) for p in paths):
            # Currency is required of the PRs the policy is about. A PR that
            # changes no reviewable source has no review to keep current.
            sr.require(retains_base, "PR head does not contain the live target base;"
                       " restack and review the composition")
            state_sha, state = queue_state(repo)
        else:
            state = {}
        report = evaluate(state, head, bases, paths)
    except (RuntimeError, sr.ReviewError, ValueError, KeyError, TypeError) as exc:
        report = {"result": "fail", "summary": "Source review could not be established: " + str(exc)}
    report.update(pr=number, head=head, base=base, base_ref=branch, target_tip=tip,
                  pr_base=pr["base"].get("sha"), queue_commit=state_sha)
    if not publish and report["result"] != "pass":
        return report
    current_head_confirmed = False
    try:
        current = api(f"repos/{repo}/pulls/{number}")
        sr.require((current["head"]["sha"], current["state"]) == (head, "open"),
                   "PR changed while checking; rerun before using the verdict")
        current_head_confirmed = True
        if state_sha is not None:
            current_queue = api(f"repos/{repo}/git/ref/heads/agents/coordination")["object"]["sha"]
            if current_queue != state_sha:
                report.update(result="fail", summary="Queue changed during review; refresh the check.")
        # Read the actual branch tip last, before returning or creating a check.
        # Re-reading the PR's cached base.sha cannot detect main advancing.
        # Compare the reviewable target, not the tip: a progress push landing
        # mid-check changes no reviewed content and must not void the verdict.
        current_branch, current_bases = target_branch(repo, current)
        if (current_branch, current_bases[-1]) != (branch, base):
            report.update(result="fail", summary="Target branch changed during review; refresh the check.",
                          observed_base=current_bases[-1], observed_base_ref=current_branch,
                          observed_tip=current_bases[0])
    except (RuntimeError, sr.ReviewError, ValueError, KeyError, TypeError) as exc:
        report.update(result="fail", summary="Final review inputs could not be confirmed: " + str(exc))
    if publish and current_head_confirmed:
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
