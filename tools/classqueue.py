#!/usr/bin/env python3
"""Claim queue for the class-reconstruction pipeline.

A claim is a ref under refs/claims/ pointing at a *parentless* commit unique to
the claiming agent. Two parentless commits are never ancestor-related, so a
second claimant's push is always a non-fast-forward and always rejected.

That subtlety is the whole point. An earlier version pushed HEAD, which fails
only when the pusher is *behind* the holder. Agents branching off one commit
push the same SHA, git answers "Everything up-to-date" and exits 0, and every
agent believes it won the race. Do not "simplify" this back to pushing HEAD.

Belt and braces: after pushing, the claim is read back from the remote and must
equal the commit we made. A push that reports success without the ref carrying
our object is not a claim.

    python tools/classqueue.py next    --role writer
    python tools/classqueue.py claim   dBgActor_c --role writer --worktree C:/tmp/foo
    python tools/classqueue.py release dBgActor_c --role writer
    python tools/classqueue.py list
"""
import argparse
import csv
import datetime
import getpass
import json
import os
import pathlib
import secrets
import socket
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent

# TU promotion is the live workstream: src/ holds ~9,400 one-function shards and
# the job is gathering them into per-class TUs. class-build-worklist.tsv tracks a
# different axis (header reconstruction, 387/429 already DONE) and would never
# offer a promotion target -- dBgActor_c reads DONE there while being row 12 here.
QUEUES = (
    REPO / "notes" / "data" / "tu-promotion-queue.tsv",
    REPO / "notes" / "data" / "class-build-worklist.tsv",
)
ROLES = ("scout", "writer", "humanizer", "builder", "reviewer")


def git(*args, check=True):
    r = subprocess.run(["git", "-C", str(REPO), *args],
                       capture_output=True, text=True)
    if check and r.returncode != 0:
        raise SystemExit(f"git {' '.join(args)} failed:\n{r.stderr.strip()}")
    return r


def ref_for(cls, role):
    return f"refs/claims/{role}/{cls}"


def held():
    """Return {ref: sha} for every live claim on the remote."""
    out = git("ls-remote", "origin", "refs/claims/*").stdout
    return dict(reversed(line.split("\t")) for line in out.splitlines() if "\t" in line)


def queue_path():
    for p in QUEUES:
        if p.exists():
            return p
    raise SystemExit("no queue found; looked for:\n  " +
                     "\n  ".join(str(p) for p in QUEUES))


def rows():
    path = queue_path()
    with path.open(newline="", encoding="utf-8") as fh:
        return path, list(csv.DictReader(fh, delimiter="\t"))


def workable(row):
    """Skip rows that are finished, already merged, or hard-blocked."""
    if row.get("state") == "DONE":
        return False
    if (row.get("already_promoted") or "").strip().lower() in ("true", "yes", "1"):
        return False
    if row.get("class_name", "").startswith("UNATTRIBUTED"):
        return False
    return True


def cmd_next(args):
    path, all_rows = rows()
    live = held()
    for row in all_rows:
        cls = row["class_name"]
        if not workable(row):
            continue
        if ref_for(cls, args.role) in live:
            continue
        row["_queue"] = str(path.relative_to(REPO))
        print(json.dumps(row, indent=2))
        return 0
    print(f"queue empty for role {args.role} ({path.name})", file=sys.stderr)
    return 1


def cmd_claim(args):
    ref = ref_for(args.cls, args.role)

    # A parentless commit over the empty tree, carrying a nonce so that two
    # agents can never produce the same object and collapse into a no-op push.
    empty_tree = git("mktree").stdout.strip()
    body = json.dumps({
        "class": args.cls,
        "role": args.role,
        "worktree": args.worktree,
        "holder": os.environ.get("AGENTLOCK_HOLDER") or getpass.getuser(),
        "host": socket.gethostname(),
        "at": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "nonce": secrets.token_hex(8),
    }, sort_keys=True)
    sha = git("commit-tree", empty_tree, "-m", body).stdout.strip()

    r = git("push", "origin", "--no-verify", f"{sha}:{ref}", check=False)
    if r.returncode != 0:
        print(f"DENIED {ref} -- already claimed", file=sys.stderr)
        return 1

    # A push can report success without our object landing. Read it back.
    actual = held().get(ref)
    if actual != sha:
        print(f"DENIED {ref} -- held by {actual}, not us", file=sys.stderr)
        return 1

    print("CLAIMED " + body)
    return 0


def cmd_release(args):
    git("push", "origin", "--no-verify", "--delete", ref_for(args.cls, args.role))
    print(f"RELEASED {args.cls} ({args.role})")
    return 0


def cmd_list(args):
    live = held()
    if not live:
        print("no live claims")
        return 0
    for ref, sha in sorted(live.items()):
        r = git("cat-file", "-p", sha, check=False)
        note = ""
        if r.returncode == 0:
            body = r.stdout.split("\n\n", 1)
            if len(body) == 2:
                try:
                    d = json.loads(body[1].strip())
                    note = f"  {d.get('holder', '?')} on {d.get('host', '?')} since {d.get('at', '?')}"
                except ValueError:
                    pass
        print(f"{sha[:9]}  {ref}{note}")
    return 0


def main():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd", required=True)

    n = sub.add_parser("next", help="print the first unclaimed workable row")
    n.add_argument("--role", required=True, choices=ROLES)
    n.set_defaults(fn=cmd_next)

    c = sub.add_parser("claim", help="atomically claim a class for a role")
    c.add_argument("cls")
    c.add_argument("--role", required=True, choices=ROLES)
    c.add_argument("--worktree", default="")
    c.set_defaults(fn=cmd_claim)

    r = sub.add_parser("release", help="drop a claim")
    r.add_argument("cls")
    r.add_argument("--role", required=True, choices=ROLES)
    r.set_defaults(fn=cmd_release)

    sub.add_parser("list", help="show live claims").set_defaults(fn=cmd_list)

    args = p.parse_args()
    return args.fn(args)


if __name__ == "__main__":
    sys.exit(main())
