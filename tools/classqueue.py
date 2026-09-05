#!/usr/bin/env python3
"""Claim queue for the class-reconstruction pipeline.

A claim is an atomic remote ref create under refs/claims/. The remote accepts a
given ref name exactly once, so two agents racing for the same class cannot both
win, and no shared file has to be edited (which is what produces merge cascades).

    python tools/classqueue.py next --role writer
    python tools/classqueue.py claim <class> --role writer --worktree C:/tmp/sm64ds-foo
    python tools/classqueue.py release <class> --role writer
    python tools/classqueue.py list
"""
import argparse
import csv
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
QUEUE = REPO / "notes" / "data" / "class-build-worklist.tsv"
ROLES = ("writer", "humanizer", "builder", "reviewer")


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


def rows():
    if not QUEUE.exists():
        raise SystemExit(f"no queue at {QUEUE} - run the scout first")
    with QUEUE.open(newline="", encoding="utf-8") as fh:
        return list(csv.DictReader(fh, delimiter="\t"))


def cmd_next(args):
    live = held()
    for row in rows():
        cls = row["class_name"]
        if row.get("state") == "DONE":
            continue
        if ref_for(cls, args.role) in live:
            continue
        print(json.dumps(row, indent=2))
        return 0
    print("queue empty for role " + args.role, file=sys.stderr)
    return 1


def cmd_claim(args):
    ref = ref_for(args.cls, args.role)
    r = git("push", "origin", "--no-verify", f"HEAD:{ref}", check=False)
    if r.returncode != 0:
        print(f"DENIED {ref} - already claimed", file=sys.stderr)
        return 1
    note = {"class": args.cls, "role": args.role, "worktree": args.worktree}
    print("CLAIMED " + json.dumps(note))
    return 0


def cmd_release(args):
    git("push", "origin", "--no-verify", "--delete", ref_for(args.cls, args.role))
    print(f"RELEASED {args.cls} ({args.role})")
    return 0


def cmd_list(args):
    for ref, sha in sorted(held().items()):
        print(f"{sha[:9]}  {ref}")
    return 0


def main():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd", required=True)

    n = sub.add_parser("next", help="print the first unclaimed row for a role")
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
