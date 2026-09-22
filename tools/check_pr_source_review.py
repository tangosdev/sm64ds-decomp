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


def evaluate(state, head, base, files, bases=None):
    # Any commit in the base window carries the same reviewable tree, so a
    # review naming the tip it was taken against is as good as one naming the
    # anchor. See stable_target().
    accepted_bases = {base} if bases is None else set(bases)
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
                sr.require(evidence.get("tested_base") in accepted_bases,
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


# A progress refresh touches a handful of generated files; a real change does
# not. Cap the walk so a long run of them cannot become an unbounded API crawl,
# and keep the cap above the observed bot cadence (11 of 40 commits on main,
# measured 2026-09-10).
TARGET_WALK_LIMIT = 25

# The scoped walk steps over real source merges that miss this PR's files, so
# it has to reach far past the bot cadence: main took 26 source merges in the
# 24h before 2026-09-21. Each commit is read once per run and shared by every
# PR (see commit_source_paths), so the cost of raising this is bounded by the
# length of main's history, not by that length times the number of open PRs.
SCOPED_WALK_LIMIT = 200

# The commit endpoint caps its file list at 300 entries and reports no
# truncation flag, so a list at the cap cannot prove a commit is inert.
COMMIT_FILES_CAP = 300


def commit_source_paths(repo, sha, cache=None):
    """Return (the source paths one commit changes, its parent).

    (None, None) means the commit could not be established as a single-parent
    commit with a complete file list, so nothing may be concluded about it --
    an unreadable commit, a merge, a file list at the API cap, or a malformed
    entry all read that way, and every caller treats it as reviewable.

    Results are keyed by commit id and immutable, so they are cached across the
    whole run. Every open PR walks the same main history; without the cache a
    single refresh would re-read it once per PR.
    """
    if cache is not None and sha in cache:
        return cache[sha]
    result = (None, None)
    try:
        data = api(f"repos/{repo}/commits/{sha}")
    except (RuntimeError, ValueError):
        data = None
    if isinstance(data, dict) and data.get("sha") == sha:
        parents, files = data.get("parents"), data.get("files")
        if (isinstance(parents, list) and len(parents) == 1 and
                isinstance(files, list) and len(files) < COMMIT_FILES_CAP):
            paths, usable = set(), True
            for entry in files:
                if not isinstance(entry, dict) or not isinstance(entry.get("filename"), str):
                    usable = False
                    break
                previous = entry.get("previous_filename")
                if previous is not None and not isinstance(previous, str):
                    usable = False
                    break
                # A rename retains both sides, so source leaving src/ still
                # needs review, and still counts as touching where it came from.
                for path in (entry["filename"], previous or ""):
                    if sr.source_path(path):
                        paths.add(path.casefold())
            parent = parents[0].get("sha") if isinstance(parents[0], dict) else None
            if usable and isinstance(parent, str) and re.fullmatch(
                    r"[0-9a-f]{40}|[0-9a-f]{64}", parent):
                result = (frozenset(paths), parent)
    if cache is not None:
        cache[sha] = result
    return result


def reviewable_commit(repo, sha, scope=None, cache=None):
    """Say whether one commit changes anything this review could be about.

    Without a scope: anything matching source_path(), the same predicate the
    check uses to decide what needs review. With a scope -- the set of source
    paths the PR itself changes -- only a commit touching one of those paths.

    Fails closed in every uncertain case, which leaves the target on the live
    tip exactly as before.
    """
    paths, parent = commit_source_paths(repo, sha, cache)
    if paths is None:
        return True, None
    return bool(paths if scope is None else paths & scope), parent


def stable_target(repo, tip, scope=None, cache=None, stop=None):
    """Anchor the target to the newest commit that changed review-relevant source.

    Reading the live tip is deliberate: a composition has to be current. But
    main also carries periodic bot progress refreshes -- contributions.json,
    docs/index.html, docs/progress-treemap.svg, marked [skip ci] -- that move
    the tip without changing anything a reviewer can act on. Anchoring to those
    expires every open PR's published review on a timer nobody is watching.
    Measured 2026-09-10 and again 2026-09-12: all open PRs red, and main moved
    twice during a single triage session.

    Walking back past only the commits that change no source_path() leaves the
    ancestry requirement in changed_paths() exactly as strong, because any
    commit that does touch reviewable source stops the walk. The commits walked
    over are returned as the accepted base window: a review tested against any
    of them was tested against the same reviewable tree, so they are
    interchangeable as evidence and a reviewer need not guess which one to name.

    That covered the bot, not the fleet. A *source* merge stops the unscoped
    walk for every open PR at once, so each merge expired every other PR's
    review: measured 2026-09-21, merging #2884 turned 9 of 13 open PRs red
    without any of them being touched, and none of the 9 shared a single file
    with it. With a scope, the walk steps over commits that change reviewable
    source the PR's review says nothing about, and the window then means the
    narrower thing it has to mean -- the same tree *in the paths this review
    covers*. Any commit touching one of those paths still stops the walk.

    What this deliberately stops catching is the indirect case: main edits a
    header the PR's sources include, the paths do not intersect, and the walk
    steps over it. That drift is a compile/link fact, and it is the ROM build
    gate that measures it against the actual merge -- not this check, which
    judges whether reconstructed source is admissible. Scoping by include graph
    was the alternative and it is not available here: this workflow is
    privileged and must never check out or execute PR code.
    """
    target, window = tip, [tip]
    for _ in range(TARGET_WALK_LIMIT if scope is None else SCOPED_WALK_LIMIT):
        # The commit the PR branched from ends the walk: everything the review
        # has to be current with has been accounted for by the time we reach it,
        # and walking past it would anchor the review behind its own base.
        if target == stop:
            break
        reviewable, parent = reviewable_commit(repo, target, scope, cache)
        if reviewable or parent is None:
            break
        target = parent
        window.append(target)
    return target, window


def live_target(repo, pr):
    """Resolve the live target ref; PR base.sha can retain an older commit."""
    base = pr["base"]
    base_repo = base["repo"]["full_name"]
    sr.require(isinstance(base_repo, str) and base_repo.casefold() == repo.casefold(),
               "PR targets a different repository")
    branch = base["ref"]
    sr.require(isinstance(branch, str) and bool(branch), "PR target branch is missing")
    ref = api(f"repos/{repo}/git/ref/heads/{quote(branch, safe='')}")
    sr.require(ref["ref"] == "refs/heads/" + branch, "API returned a different target ref")
    sr.require(ref["object"]["type"] == "commit", "Target branch does not identify a commit")
    return branch, sr.commit(ref["object"]["sha"], "target branch commit")


def target_branch(repo, pr, scope=None, cache=None):
    """Resolve the live target ref and anchor it, ignoring how the PR relates."""
    branch, tip = live_target(repo, pr)
    target, window = stable_target(repo, tip, scope, cache)
    return branch, target, window


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


STALE_COMPOSITION = ("PR head does not contain the live target base; "
                     "restack and review the composition")


def contains(repo, base, head):
    """Return (does head already contain base, the commit they actually share).

    Ancestry only. Separated from changed_paths() so review_anchor() can ask
    where a PR branched from without that question being fatal on its own.
    """
    base = sr.commit(base, "comparison base")
    head = sr.commit(head, "comparison head")
    comparison = api(f"repos/{repo}/compare/{base}...{head}?per_page=1")
    sr.require(comparison["base_commit"]["sha"] == base,
               "Comparison returned a different base")
    merge_base = sr.commit(comparison["merge_base_commit"]["sha"], "merge base commit")
    return (merge_base == base and
            comparison["status"] in ("ahead", "identical")), merge_base


def review_anchor(repo, tip, head, scope=None, cache=None):
    """Resolve the base a review is measured against, and the accepted window.

    Anchors on the live tip first, so a PR that is current keeps exactly the
    behaviour and the API cost it had. Only once main is found to have moved
    ahead of the PR does this pay for the PR's own diff, and re-anchor the walk
    to the paths that diff covers. See stable_target() for why that is sound
    and what it gives up.
    """
    if scope is None:
        target, window = stable_target(repo, tip, None, cache)
        contained, merge_base = contains(repo, target, head)
        if contained:
            return target, window, None
        scope = frozenset(p.casefold() for p in changed_paths(repo, merge_base, head)
                          if sr.source_path(p))
    else:
        _, merge_base = contains(repo, tip, head)
    if not scope:
        # Nothing here needs review at all, so no commit on main can expire it.
        return merge_base, [tip, merge_base], scope
    target, window = stable_target(repo, tip, scope, cache, stop=merge_base)
    sr.require(target == merge_base, STALE_COMPOSITION)
    return target, window, scope


def changed_paths(repo, base, head):
    """Compare the exact composition, not PR /files metadata for a cached base.

    The composition must retain its base ancestry (queue-v2.md and
    SOURCE-REVIEW-CUTOVER.md). A stale branch needs a new composition and review.
    Compare metadata establishes ancestry only: its files list is capped at 300,
    even with pagination. Git tree objects supply the complete changed-path set.
    Leaf comparison retains both rename sides and mode/type changes, including
    symlinks, submodules, and file/directory replacements.
    """
    contained, _ = contains(repo, base, head)
    sr.require(contained, STALE_COMPOSITION)
    before_tree, after_tree = commit_tree(repo, base), commit_tree(repo, head)
    if before_tree == after_tree:
        return []
    cache = {}
    before = complete_tree(repo, before_tree, cache)
    after = complete_tree(repo, after_tree, cache)
    return sorted(path for path in before.keys() | after.keys()
                  if before.get(path) != after.get(path))


def check_pr(repo, number, publish=False, cache=None):
    pr = api(f"repos/{repo}/pulls/{number}")
    if pr["state"] != "open":
        return {"pr": number, "result": "closed"}
    head = pr["head"]["sha"]
    base, branch, window = None, None, []
    state_sha, scope = None, None
    if cache is None:
        cache = {}
    try:
        branch, tip = live_target(repo, pr)
        base, window, scope = review_anchor(repo, tip, head, None, cache)
        paths = changed_paths(repo, base, head)
        if any(sr.source_path(p) for p in paths):
            state_sha, state = queue_state(repo)
        else:
            state = {}
        report = evaluate(state, head, base, paths, window)
    except (RuntimeError, sr.ReviewError, ValueError, KeyError, TypeError) as exc:
        report = {"result": "fail", "summary": "Source review could not be established: " + str(exc)}
    report.update(pr=number, head=head, base=base, base_ref=branch,
                  live_tip=window[0] if window else None, base_window=window,
                  pr_base=pr["base"].get("sha"), queue_commit=state_sha, published=False,
                  # Present only when main moved ahead of this PR: the paths the
                  # walk was narrowed to, so a reader can see what was compared.
                  review_scope=None if scope is None else sorted(scope))
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
        current_branch, current_tip = live_target(repo, current)
        current_base, _, _ = review_anchor(repo, current_tip, head, scope, cache)
        if (current_branch, current_base) != (branch, base):
            report.update(result="fail", summary="Target branch changed during review; refresh the check.",
                          observed_base=current_base, observed_base_ref=current_branch)
    except (RuntimeError, sr.ReviewError, ValueError, KeyError, TypeError) as exc:
        report.update(result="fail", summary="Final review inputs could not be confirmed: " + str(exc))
    # Only a verdict its check run carries is actually reported; one that could
    # not be published speaks for nothing. main() keys its exit code on this.
    report["published"] = bool(publish and current_head_confirmed)
    if report["published"]:
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
        # One commit cache for the whole run: every PR walks the same main.
        cache = {}
        reports = [check_pr(args.repo, n, args.publish, cache) for n in numbers]
        print(json.dumps(reports, indent=2))
        # A published failure is already reported by its own "Source review"
        # check run. Failing the job too states the same verdict a second
        # time, doubling the red marks on every PR without adding signal:
        # measured 2026-09-17, 17 open PRs carried 34 red checks for 17
        # distinct verdicts. A read-only run has no check run to speak for
        # it, and a publish that could not create one is an unreported
        # failure, so both of those still exit nonzero.
        return int(any(r["result"] == "fail" and not r.get("published")
                       for r in reports))
    except (RuntimeError, ValueError, KeyError, TypeError) as exc:
        print("Source review unavailable: " + str(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
