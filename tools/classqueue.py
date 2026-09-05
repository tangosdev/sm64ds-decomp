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
import re
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


def normalize_cls(cls):
    """Reduce a class id to the single spelling the claim ref is named with.

    Callers reach for two spellings interchangeably -- bare `daOts_c` and
    overlay-qualified `ov064/daOts_c` -- and the role files use both. Left
    alone, those build *different* refs, so two agents claiming one class in
    two spellings would each create a ref and each believe it won. That is the
    same class of hole as pushing HEAD; it was live on the remote (claims
    existed as both `writer/daObjCtMecha04_c` and
    `writer/ov006/dScMgD3DBase_c`) before this was added.

    The bare class name is the canonical form: a class lives in exactly one
    overlay, so the prefix carries no information the name does not.
    """
    cls = cls.strip().strip("/")
    if not cls:
        raise SystemExit("empty class id")
    cls = cls.rsplit("/", 1)[-1]
    if not re.fullmatch(r"[A-Za-z0-9_+.-]+", cls):
        raise SystemExit(f"class id {cls!r} is not a valid ref component")
    return cls


def ref_for(cls, role):
    return f"refs/claims/{role}/{normalize_cls(cls)}"


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


def prior_work(cls):
    """Branches and worktrees that already mention this class.

    The claim lock is deliberately ephemeral: `release` deletes the ref, so a
    finished class leaves no trace. But an *abandoned* one leaves no trace
    either -- and its branch may be local-only, so no remote check can see it.
    A writer spent a full run reconstructing dScMgD3DBase_c before noticing
    `wip/dScMgD3DBase_c-humanizer-blocked-0905` sitting in a sibling worktree
    with the same class already done.

    So this warns, and does not deny: a branch is usually your own earlier work
    or a landed promotion, not a live conflict. Only a person can tell which.
    """
    cls = normalize_cls(cls)
    found = []
    r = git("branch", "--all", "--list", f"*{cls}*", "--format=%(refname:short)",
            check=False)
    if r.returncode == 0:
        found += [b.strip() for b in r.stdout.splitlines() if b.strip()]
    r = git("worktree", "list", "--porcelain", check=False)
    if r.returncode == 0:
        path = None
        for line in r.stdout.splitlines():
            if line.startswith("worktree "):
                path = line.split(" ", 1)[1]
            elif line.startswith("branch ") and cls in line:
                found.append(f"{line.split(' ', 1)[1]}  (checked out at {path})")
    return sorted(set(found))


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

    existing = prior_work(args.cls)
    if existing:
        print("", file=sys.stderr)
        print("WARNING: work already exists for this class. The claim lock cannot",
              file=sys.stderr)
        print("see it -- release deletes the ref, so an abandoned branch is invisible.",
              file=sys.stderr)
        print("Read these before writing anything:", file=sys.stderr)
        for b in existing:
            print(f"  {b}", file=sys.stderr)
    return 0


def cmd_status(args):
    """Read-only: is this class claimed, and by whom?

    Added because builder.md's "release your claim when done" gave no way to ask
    whether there *was* one. A class the writer already released, or that was
    never claimed, has no ref -- and a release of a claim you do not hold is
    indistinguishable from a bug without this.
    """
    cls = normalize_cls(args.cls)
    live = held()
    mine = {ref: sha for ref, sha in live.items()
            if ref.rsplit("/", 1)[-1] == cls}
    if not mine:
        print(f"UNCLAIMED {cls}")
    for ref, sha in sorted(mine.items()):
        body = git("cat-file", "-p", sha, check=False).stdout
        detail = body.strip().splitlines()[-1] if body.strip() else ""
        print(f"CLAIMED {ref}")
        print(f"  {detail}")

    prior = prior_work(cls)
    if prior:
        print("")
        print("branches and worktrees mentioning this class:")
        for b in prior:
            print(f"  {b}")
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

    st = sub.add_parser("status", help="report whether a class is claimed (read-only)")
    st.add_argument("cls")
    st.set_defaults(fn=cmd_status)

    r = sub.add_parser("release", help="drop a claim")
    r.add_argument("cls")
    r.add_argument("--role", required=True, choices=ROLES)
    r.set_defaults(fn=cmd_release)

    sub.add_parser("list", help="show live claims").set_defaults(fn=cmd_list)

    args = p.parse_args()
    return args.fn(args)


if __name__ == "__main__":
    sys.exit(main())
