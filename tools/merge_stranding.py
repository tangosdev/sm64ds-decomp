#!/usr/bin/env python3
"""Find work committed to a branch AFTER the pull request that closed it merged.

`notes/handoff-marker-typing.md` section 7 names this trap; it has now cost the tree
four times (#1156 -> #1169, #1196 -> #1200, and twice before). The shape is always the
same:

    10:04  commit A pushed to branch B
    10:12  PR squash-merges branch B          <- GitHub freezes the PR head here
    10:53  commit C pushed to branch B        <- never in any pull request, ever
    13:46  commit D pushed to branch B        <- likewise

A squash merge lands ONE commit whose parent is main. It does not merge branch B, so no
commit on B ever becomes an ancestor of main. That is fine for A, whose content the
squash carries. C and D are simply lost: no PR contains them, no gate sees them, and the
branch is then deleted as "merged" because GitHub says the PR merged.

Nothing in the normal workflow surfaces this. The PR is green. The branch reads "merged".
`git log main` looks complete. Every previous instance was caught days later by noticing
that a *claim* disagreed with the tree -- a census count that did not match a note's
table, a PR title advertising a class reference whose generator was not in the tree.
That is not a gate, that is luck.

The detector is one comparison. GitHub stops updating a PR's head SHA once the PR
closes, so for any merged PR whose branch still exists:

    branch tip != PR head at merge   =>   commits exist that no pull request contains

That is a fact, not a heuristic, and it is reported as such. The second question --
does it still matter? -- is answered with git's own three-way merge rather than by
comparing files, because a branch can be stale for boring reasons and file comparison
cannot tell the difference:

    git merge-tree --write-tree main <tip>

    result tree == main's tree  ->  LANDED    the tip adds nothing; safe to delete
    result tree != main's tree  ->  ADDS      exactly these paths are not on main
    merge-tree exits nonzero    ->  CONFLICT  content differs and a human must choose

That distinction is worth the extra call. On #1156, comparing files flagged 84 paths as
suspicious -- every header the branch had touched and main had since moved past. The
merge-tree check narrows the same branch to one file, the note that #1169 re-landed with
a correction, which is the only thing that actually still differs.

--check fails on ADDS and on CONFLICT. Both mean the branch holds content main does not,
which is precisely the condition worth a human's attention. A branch that is genuinely
finished with drains out of the report the moment it is deleted.

Fork PRs are covered: their branch lives in the contributor's fork, so the tip is read
from the fork rather than from origin.

This needs the network and so cannot be a PR gate -- the damage happens *after* a merge,
when there is no PR left to fail. Run it on a schedule. It is otherwise cheap: stdlib
only, no compiler, no ROM, and it fetches only the commits it has to compare.

Auth: $GITHUB_TOKEN if set, else `gh auth token`. Public-repo read is enough.

Usage:
    python tools/merge_stranding.py                     # last 7 days, human summary
    python tools/merge_stranding.py --since 30          # wider window
    python tools/merge_stranding.py --pr 1156           # one PR, ignores the window
    python tools/merge_stranding.py --check             # exit 1 if anything is stranded
    python tools/merge_stranding.py --json out.json     # machine-readable
"""
import argparse
import json
import os
import subprocess
import sys
import urllib.error
import urllib.request
from datetime import datetime, timedelta, timezone

API = "https://api.github.com"
SLUG = "tangosdev/sm64ds-decomp"

LANDED, ADDS, CONFLICT, UNREACHABLE = "LANDED", "ADDS", "CONFLICT", "UNREACHABLE"


def git(*args, check=True):
    r = subprocess.run(("git",) + args, capture_output=True, text=True)
    if check and r.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)}: {r.stderr.strip()}")
    return r.stdout.strip()


def have(sha):
    return subprocess.run(["git", "cat-file", "-e", f"{sha}^{{commit}}"],
                          capture_output=True).returncode == 0


def token():
    t = os.environ.get("GITHUB_TOKEN")
    if t:
        return t
    r = subprocess.run(["gh", "auth", "token"], capture_output=True, text=True)
    return r.stdout.strip() if r.returncode == 0 and r.stdout.strip() else None


def api(path, tok):
    """GET a JSON path. None on 404, so callers can treat 'gone' as data."""
    req = urllib.request.Request(f"{API}{path}")
    req.add_header("Accept", "application/vnd.github+json")
    if tok:
        req.add_header("Authorization", f"Bearer {tok}")
    try:
        with urllib.request.urlopen(req, timeout=30) as fh:
            return json.load(fh)
    except urllib.error.HTTPError as e:
        if e.code == 404:
            return None
        raise


def merged_prs(tok, since, slug, only=None):
    if only is not None:
        pr = api(f"/repos/{slug}/pulls/{only}", tok)
        return [pr] if pr and pr.get("merged_at") else []
    out, page = [], 1
    while page <= 10:
        batch = api(f"/repos/{slug}/pulls?state=closed&sort=updated&direction=desc"
                    f"&per_page=100&page={page}", tok) or []
        if not batch:
            break
        stale = True
        for pr in batch:
            if not pr.get("merged_at"):
                continue
            if datetime.fromisoformat(pr["merged_at"].replace("Z", "+00:00")) >= since:
                out.append(pr)
                stale = False
        if stale:
            break
        page += 1
    return out


def tips(url, refs=None):
    """branch -> sha for one remote, in a single call."""
    cmd = ["git", "ls-remote", "--heads", url] + list(refs or [])
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        return {}
    out = {}
    for line in r.stdout.splitlines():
        parts = line.split()
        if len(parts) == 2 and parts[1].startswith("refs/heads/"):
            out[parts[1][len("refs/heads/"):]] = parts[0]
    return out


def fetch(sha, url, ref):
    if have(sha):
        return True
    for args in (["git", "fetch", "--quiet", url, sha],
                 ["git", "fetch", "--quiet", url, ref]):
        subprocess.run(args, capture_output=True)
        if have(sha):
            return True
    return False


def assess(tip, base):
    """What would merging `tip` into `base` do? The 'does it still matter' question."""
    r = subprocess.run(["git", "merge-tree", "--write-tree", base, tip],
                       capture_output=True, text=True)
    lines = r.stdout.splitlines()
    if not lines:
        return CONFLICT, [], (r.stderr or "merge-tree produced nothing").strip()
    tree = lines[0].strip()
    if r.returncode != 0:
        paths = sorted({ln.split("\t")[-1] for ln in lines[1:]
                        if "\t" in ln and not ln.startswith(("Auto-merging", "CONFLICT"))})
        return CONFLICT, paths, ""
    if tree == git("rev-parse", f"{base}^{{tree}}"):
        return LANDED, [], ""
    paths = [p for p in git("diff", "--name-only", f"{base}^{{tree}}", tree,
                            check=False).splitlines() if p]
    return ADDS, paths, ""


def inspect(pr, tip, base):
    head = pr["head"]["sha"]
    ref = pr["head"]["ref"]
    repo = (pr["head"].get("repo") or {}).get("full_name")
    url = f"https://github.com/{repo}.git" if repo else None
    rec = {"pr": pr["number"], "title": pr["title"], "branch": ref,
           "merged_at": pr["merged_at"], "head": head, "tip": tip}

    if url and not fetch(tip, url, ref):
        rec.update(verdict=UNREACHABLE, commits=[], paths=[], note="tip not fetchable")
        return rec

    extra = []
    for ln in git("log", "--format=%H\t%P\t%s", f"{head}..{tip}", check=False).splitlines():
        if ln.count("\t") >= 2:
            sha, parents, subj = ln.split("\t", 2)
            extra.append({"sha": sha, "subject": subj,
                          "merge": len(parents.split()) > 1})
    if not extra:
        return None            # tip differs but adds no commits: a rebase, not a loss

    verdict, paths, note = assess(tip, base)
    rec.update(verdict=verdict, commits=extra, paths=paths, note=note)
    return rec


def render(reports, checked):
    bad = [r for r in reports if r["verdict"] != LANDED]
    ok = [r for r in reports if r["verdict"] == LANDED]
    out = [f"merge_stranding: {checked} merged PR(s) checked, "
           f"{len(bad)} with work in no pull request"]

    for r in bad:
        out += ["", f"  #{r['pr']}  {r['branch']}   *** {r['verdict']} ***",
                f"    merged at {r['head'][:8]}, branch tip is now {r['tip'][:8]}"]
        if r["verdict"] == UNREACHABLE:
            out.append(f"    {r['note']} -- check the fork by hand")
            continue
        out.append(f"    {len(r['commits'])} commit(s) no pull request contains:")
        out += ["      {}{}  {}".format(c["sha"][:8], " (merge)" if c.get("merge") else "",
                                        c["subject"]) for c in r["commits"]]
        if r["note"]:
            out.append(f"    {r['note']}")
        label = ("merging the tip would add" if r["verdict"] == ADDS
                 else "merging the tip conflicts in")
        out.append(f"    {label} {len(r['paths'])} path(s):")
        out += [f"      {p}" for p in r["paths"][:20]]
        if len(r["paths"]) > 20:
            out.append(f"      ... and {len(r['paths']) - 20} more")
        # commits are newest-first, so the range is oldest^..newest. A merge in the
        # range cannot be cherry-picked without -m, so name the non-merges instead of
        # printing a range that will not run.
        picks = [c for c in r["commits"] if not c.get("merge")]
        if len(picks) == len(r["commits"]) and len(picks) > 1:
            span = f"{picks[-1]['sha'][:8]}^..{picks[0]['sha'][:8]}"
        else:
            span = " ".join(c["sha"][:8] for c in reversed(picks)) or "(merges only)"
        out.append(f"    -> re-land: git cherry-pick {span}")
        if len(picks) != len(r["commits"]):
            out.append("       (merge commits in the range are skipped -- pick by hand)")

    for r in ok:
        out += ["", f"  #{r['pr']}  {r['branch']}   tip is newer but adds nothing "
                    f"-- safe to delete"]
    if not bad:
        out += ["", "  no work stranded by a squash merge."]
    return "\n".join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--since", type=int, default=7, help="days back (default 7)")
    ap.add_argument("--pr", type=int, help="check one PR, ignore the window")
    ap.add_argument("--repo", default=SLUG, help=f"owner/name (default {SLUG})")
    ap.add_argument("--base", default="origin/main", help="what 'landed' means")
    ap.add_argument("--check", action="store_true", help="exit 1 if anything stranded")
    ap.add_argument("--json", metavar="PATH", help="write the report as JSON")
    ap.add_argument("--baseline", metavar="PATH",
                    help="acknowledged findings; --check ignores these unless the "
                         "branch has moved again")
    ap.add_argument("--bank", metavar="PATH",
                    help="write the current findings as a baseline")
    args = ap.parse_args()

    tok = token()
    if not tok:
        print("no GitHub token: set $GITHUB_TOKEN or run `gh auth login`", file=sys.stderr)
        return 2

    subprocess.run(["git", "fetch", "--quiet", "origin", "main"], capture_output=True)
    since = datetime.now(timezone.utc) - timedelta(days=args.since)
    prs = merged_prs(tok, since, args.repo, only=args.pr)

    # One ls-remote for origin covers every same-repo PR; forks are asked individually.
    by_remote = {}
    for pr in prs:
        repo = (pr["head"].get("repo") or {}).get("full_name")
        if repo:
            by_remote.setdefault(repo, set()).add(pr["head"]["ref"])
    live = {repo: tips(f"https://github.com/{repo}.git", refs)
            for repo, refs in by_remote.items()}

    reports = []
    for pr in prs:
        repo = (pr["head"].get("repo") or {}).get("full_name")
        tip = live.get(repo, {}).get(pr["head"]["ref"])
        if tip is None or tip == pr["head"]["sha"]:
            continue                      # branch deleted, or untouched since the merge
        r = inspect(pr, tip, args.base)
        if r:
            reports.append(r)

    print(render(reports, len(prs)))

    if args.json:
        with open(args.json, "w", encoding="utf-8") as fh:
            json.dump({"checked": len(prs), "reports": reports}, fh, indent=2)
        print(f"\nwrote {args.json}")

    if args.bank:
        banked = {str(r["pr"]): {"branch": r["branch"], "tip": r["tip"],
                                 "verdict": r["verdict"]}
                  for r in reports if r["verdict"] != LANDED}
        with open(args.bank, "w", encoding="utf-8") as fh:
            json.dump({"_note": ("Acknowledged stranding, keyed by PR. --check stays "
                                 "green for these until the branch tip MOVES AGAIN. "
                                 "Delete an entry once the branch is re-landed or "
                                 "deleted; regenerate with --bank."),
                       "acknowledged": banked}, fh, indent=2, sort_keys=True)
        print(f"\nwrote {args.bank} ({len(banked)} acknowledged)")

    live = [r for r in reports if r["verdict"] != LANDED]
    if args.baseline and os.path.exists(args.baseline):
        with open(args.baseline, encoding="utf-8") as fh:
            ack = json.load(fh).get("acknowledged", {})
        # Keyed on the tip, not just the PR: an acknowledged branch that receives
        # ANOTHER commit is new stranding and must fire again.
        live = [r for r in live if ack.get(str(r["pr"]), {}).get("tip") != r["tip"]]
        if len(live) != len(reports):
            print(f"\n{len(reports) - len(live)} finding(s) acknowledged in "
                  f"{args.baseline}")

    if args.check and live:
        print("\nmerge_stranding FAILED -- commits exist that no pull request contains:")
        for r in live:
            print(f"  #{r['pr']}  {r['branch']}  ({r['verdict']})")
        print("\nSee notes/handoff-marker-typing.md section 7. Cherry-pick them onto")
        print("main in a re-land PR, or delete the branch if the work is obsolete.")
        print("If it is neither, acknowledge it: python tools/merge_stranding.py "
              "--bank stranding-baseline.json")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
