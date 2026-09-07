#!/usr/bin/env python3
"""Opt-in durable task queue; see notes/agents/queue-v2.md before cutover.

One Git ref serializes resource reservations and handoffs. Every update uses an
explicit expected SHA, including creation. No timestamps grant ownership.
This is a cooperative protocol, not access control against repository writers.
"""
import argparse
import copy
import datetime
import hashlib
import json
import pathlib
import re
import secrets
import subprocess
import sys
import uuid

REF = "refs/heads/agents/coordination"
SCHEMA = 2


class QueueError(Exception):
    pass


def require(condition, message):
    if not condition:
        raise QueueError(message)


def encoded(value):
    return json.dumps(value, sort_keys=True, separators=(",", ":"))


def now():
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def identifier(value):
    require(isinstance(value, str) and
            re.fullmatch(r"[A-Za-z0-9_][A-Za-z0-9_.-]{0,127}", value),
            "IDs must be 1-128 letters, numbers, underscores, dots or hyphens")
    return value


def artifact_path(value):
    require(isinstance(value, str) and value and ":" not in value and
            "\\" not in value and not value.startswith("/") and
            all(p not in ("", ".", "..") for p in value.split("/")),
            "artifact paths must be literal repository-relative paths")
    return value


def resources(values):
    """Expand grouped classes; ranges use module-qualified half-open intervals."""
    require(isinstance(values, list) and values, "at least one resource is required")
    result = set()
    for value in values:
        require(isinstance(value, str) and ":" in value, "invalid resource")
        kind, name = value.split(":", 1)
        if kind == "class":
            for cls in name.split("+"):
                cls = cls.rsplit("/", 1)[-1]
                result.add("class:" + identifier(cls))
        elif kind in ("tu", "file"):
            # A TU and a file reservation of the same path must conflict.
            result.add("file:" + artifact_path(name).casefold())
        elif kind == "range":
            parts = name.split(":")
            require(len(parts) == 3 and
                    re.fullmatch(r"(?:arm9|arm7|itcm|dtcm|ov[0-9]{3})", parts[0]),
                    "range must be range:MODULE:START:END (e.g. ov006)")
            try:
                start, end = int(parts[1], 0), int(parts[2], 0)
            except ValueError as exc:
                raise QueueError("range bounds must be integers") from exc
            require(0 <= start < end <= 0x100000000, "invalid half-open range")
            result.add(f"range:{parts[0]}:{start}:{end}")
        else:
            raise QueueError("unknown resource kind: " + kind)
    return sorted(result)


def overlaps(left, right):
    for a in left:
        for b in right:
            if a == b:
                return True
            if a.startswith("range:") and b.startswith("range:"):
                _, am, ab, ae = a.split(":")
                _, bm, bb, be = b.split(":")
                if am == bm and max(int(ab), int(bb)) < min(int(ae), int(be)):
                    return True
    return False


def new_receipt(task_id, session):
    return {"schema": SCHEMA, "task_id": identifier(task_id),
            "session": identifier(session), "lease_id": str(uuid.uuid4()),
            "token": secrets.token_hex(32)}


def receipt_owner(receipt):
    require(isinstance(receipt, dict) and receipt.get("schema") == SCHEMA,
            "invalid receipt schema")
    for key in ("task_id", "session", "lease_id"):
        identifier(receipt.get(key))
    token = receipt.get("token", "")
    require(isinstance(token, str) and re.fullmatch(r"[0-9a-f]{64}", token),
            "invalid private receipt token")
    return {"session": receipt["session"], "lease_id": receipt["lease_id"],
            "token_hash": hashlib.sha256(token.encode()).hexdigest()}


class Queue:
    def __init__(self, repo, remote="origin"):
        self.repo = pathlib.Path(repo)
        self.remote = remote

    def git(self, *args, input=None, check=True):
        try:
            result = subprocess.run(["git", "-C", str(self.repo), *args],
                                    input=input.encode("utf-8") if input is not None else None,
                                    capture_output=True)
        except OSError as exc:
            raise QueueError(f"Git unavailable: {exc}") from exc
        # Binary stdin avoids Windows newline translation in mktree path names.
        result.stdout = result.stdout.decode("utf-8", errors="replace")
        result.stderr = result.stderr.decode("utf-8", errors="replace")
        if check and result.returncode:
            raise QueueError(f"git {args[0]} failed: {result.stderr.strip()}")
        return result

    def commit(self, sha):
        require(isinstance(sha, str) and re.fullmatch(r"[0-9a-f]{40}|[0-9a-f]{64}", sha),
                "commit must be a full immutable object ID")
        require(self.git("cat-file", "-t", sha).stdout.strip() == "commit",
                "artifact object is not a commit")
        return sha

    def paths(self, sha, paths):
        self.commit(sha)
        for path in paths:
            path = artifact_path(path)
            require(self.git("cat-file", "-t", f"{sha}:{path}").stdout.strip() == "blob",
                    f"artifact is not a file: {sha}:{path}")

    def read(self, allow_missing=False):
        # Missing transport/metadata must never look like an empty queue.
        listing = self.git("ls-remote", self.remote, REF, "refs/claims/*").stdout
        refs = {}
        for line in listing.splitlines():
            fields = line.split("\t")
            require(len(fields) == 2, "malformed remote ref listing")
            refs[fields[1]] = fields[0]
        require(not any(ref.startswith("refs/claims/") for ref in refs),
                "legacy claims exist: v2 requires a drained, upgraded fleet; "
                "do not delete another session's claims")
        sha = refs.get(REF)
        if not sha:
            require(allow_missing, "v2 is not initialized on this remote")
            return None, None
        # Fetch explicitly: ls-remote does not materialize claim/state objects.
        self.git("fetch", "--no-tags", "--no-write-fetch-head", self.remote, REF)
        self.commit(sha)
        try:
            state = json.loads(self.git("show", f"{sha}:state.json").stdout)
        except (ValueError, TypeError) as exc:
            raise QueueError("invalid remote queue JSON") from exc
        require(isinstance(state, dict) and state.get("schema") == SCHEMA and
                state.get("cutover") == "legacy-clients-stopped-and-upgraded" and
                isinstance(state.get("tasks"), dict) and
                isinstance(state.get("operations"), dict), "invalid remote queue schema")
        return sha, state

    def save(self, expected, state, parents=()):
        blob = self.git("hash-object", "-w", "--stdin", input=encoded(state)).stdout.strip()
        tree = self.git("mktree", input=f"100644 blob {blob}\tstate.json\n").stdout.strip()
        ancestry = list(dict.fromkeys(([expected] if expected else []) + list(parents)))
        args = ["commit-tree", tree]
        for parent in ancestry:
            args.extend(("-p", self.commit(parent)))
        sha = self.git(*args, "-m", "classqueue v2 transaction " + str(uuid.uuid4())).stdout.strip()
        # A single ref makes grouped acquisition and all task transitions atomic.
        # Even two updates from the same old parent cannot both succeed.
        result = self.git("push", "--no-verify", self.remote,
                          f"--force-with-lease={REF}:{expected or ''}",
                          f"{sha}:{REF}", check=False)
        if result.returncode:
            # The push may have landed before a connection failed. Do not retry
            # blindly: callers retry with the SAME operation ID / private receipt.
            raise QueueError("queue update failed or result is uncertain; reread and "
                             "retry the same request ID/receipt: " + result.stderr.strip())
        return sha

    def init(self, session):
        sha, state = self.read(allow_missing=True)
        if sha:
            return {"initialized": True, "state_commit": sha}
        state = {"schema": SCHEMA, "cutover": "legacy-clients-stopped-and-upgraded",
                 "initialized_by": identifier(session), "created_at": now(),
                 "tasks": {}, "operations": {}}
        return {"initialized": True, "state_commit": self.save(None, state)}

    def transaction(self, request_id, payload, operation):
        identifier(request_id)
        sha, state = self.read()
        fingerprint = hashlib.sha256(encoded(payload).encode()).hexdigest()
        old = state["operations"].get(request_id)
        if old:
            require(old["fingerprint"] == fingerprint,
                    "request ID was already used for a different operation")
            return copy.deepcopy(old["result"])
        result, parents = operation(state)
        state["operations"][request_id] = {"fingerprint": fingerprint, "result": result}
        self.save(sha, state, parents)
        return result

    def enqueue(self, spec, request_id, coordinator_receipt):
        spec = copy.deepcopy(spec)
        task_id = identifier(spec.get("task_id"))
        identifier(spec.get("coordinator"))
        coordinator = receipt_owner(coordinator_receipt)
        require(coordinator_receipt["task_id"] == task_id and
                coordinator["session"] == spec["coordinator"],
                "coordinator receipt must match the task and coordinator session")
        self.commit(spec.get("workflow_commit"))
        require(isinstance(spec.get("next_action"), str) and spec["next_action"].strip(),
                "next_action is required")
        spec["resources"] = resources(spec.get("resources"))
        self.commit(spec.get("base_commit"))
        spec.setdefault("input_commit", spec["base_commit"])
        self.commit(spec["input_commit"])
        require(self.git("merge-base", "--is-ancestor", spec["base_commit"],
                         spec["input_commit"], check=False).returncode == 0,
                "initial input must descend from the pinned source base")
        spec.setdefault("evidence_inputs", [])
        require(isinstance(spec["evidence_inputs"], list), "evidence_inputs must be a list")
        for evidence in spec["evidence_inputs"]:
            require(isinstance(evidence, dict) and isinstance(evidence.get("paths"), list),
                    "evidence input needs a commit and literal artifact paths")
            self.paths(evidence.get("commit"), evidence["paths"])
        stages = spec.get("stages")
        require(isinstance(stages, list) and stages, "configure at least one stage")
        ids = set()
        for stage in stages:
            require(isinstance(stage, dict), "invalid stage")
            sid = identifier(stage.get("id"))
            require(sid not in ids, "stage IDs must be unique (roles may repeat)")
            ids.add(sid)
            identifier(stage.get("role"))
            require(stage.get("mode") in ("write", "verify"), "mode must be write or verify")
            for key in ("requires", "produces"):
                require(isinstance(stage.get(key), list), f"stage {key} must be a path list")
                for path in stage[key]:
                    artifact_path(path)
        if stages[0]["mode"] == "verify":
            identifier(spec.get("input_session"))

        def operation(state):
            require(task_id not in state["tasks"], "task ID already exists; use a new follow-up ID")
            for other in state["tasks"].values():
                if other["phase"] not in ("done", "cancelled") and overlaps(spec["resources"], other["resources"]):
                    raise QueueError("resources reserved by task " + other["task_id"])
            task = {**spec, "phase": "ready", "stage_index": 0, "owner": None,
                    "coordinator_owner": coordinator, "outputs": [], "attempts": [],
                    "stage_inputs": {}, "history": [], "created_at": now()}
            self.input_for(task, 0)
            state["tasks"][task_id] = task
            return {"task_id": task_id, "phase": "ready"}, [
                spec["base_commit"], spec["input_commit"], spec["workflow_commit"],
                *[evidence["commit"] for evidence in spec["evidence_inputs"]]]
        return self.transaction(request_id, {"enqueue": spec, "coordinator": coordinator}, operation)

    def input_for(self, task, index):
        self.commit(task["base_commit"])
        self.commit(task["workflow_commit"])
        stage = task["stages"][index]
        for evidence in task["evidence_inputs"]:
            self.paths(evidence["commit"], evidence["paths"])
        if index:
            require(len(task["outputs"]) >= index, "predecessor stage is incomplete")
            output = task["outputs"][index - 1]
            require(output["stage_id"] == task["stages"][index - 1]["id"],
                    "predecessor artifact belongs to the wrong stage")
            sha = output["commit"]
            self.paths(sha, task["stages"][index - 1]["produces"])
        else:
            sha = task["input_commit"]
        self.paths(sha, stage["requires"])
        revision = task["stage_inputs"].get(str(index))
        if revision:
            # Original inputs remain pinned above. A promotion may have removed
            # its input shards from the revision; require its produced TU there.
            self.paths(revision, stage["produces"])
            sha = revision
        return sha

    def ready(self, task):
        if task["phase"] not in ("ready", "offered"):
            return None
        index = task["stage_index"] + (task["phase"] == "offered")
        require(index < len(task["stages"]), "invalid next stage index")
        return index, self.input_for(task, index)

    def next(self, role):
        _, state = self.read()
        found = []
        for task in state["tasks"].values():
            ready = self.ready(task)
            if ready and task["stages"][ready[0]]["role"] == role:
                index, sha = ready
                found.append({"task_id": task["task_id"], "stage": task["stages"][index],
                              "input_commit": sha, "base_commit": task["base_commit"],
                              "workflow_commit": task["workflow_commit"],
                              "evidence_inputs": task["evidence_inputs"],
                              "resources": task["resources"], "next_action": task["next_action"]})
        return found

    def claim(self, receipt, stage_id, role, input_commit, worktree):
        owner = receipt_owner(receipt)
        task_id = receipt["task_id"]
        payload = {"claim": task_id, "owner": owner, "stage_id": stage_id,
                   "role": role, "input_commit": input_commit, "worktree": worktree}

        def operation(state):
            task = self.task(state, task_id)
            ready = self.ready(task)
            require(ready is not None, "task is not ready for a claim")
            index, actual_input = ready
            stage = task["stages"][index]
            require(stage["id"] == stage_id and stage["role"] == role,
                    "requested role/stage is not the next stage")
            require(actual_input == input_commit, "input changed; inspect next before claiming")
            if stage["mode"] == "verify":
                producer = self.producer_session(task, index)
                require(producer != owner["session"],
                        "verification requires a different session from the source producer")
            if task["phase"] == "offered":
                task["outputs"][-1]["accepted_by"] = owner["session"]
                task["outputs"][-1]["accepted_at"] = now()
            task.update(phase="running", stage_index=index,
                        owner={**owner, "role": role, "worktree": worktree,
                               "input_commit": input_commit, "at": now()})
            self.event(task, "claim", task["owner"])
            return {"task_id": task_id, "stage_id": stage_id, "lease_id": owner["lease_id"],
                    "input_commit": input_commit, "phase": "running"}, []
        result = self.transaction("claim-" + owner["lease_id"], payload, operation)
        # An old claim retry must not return a misleading CLAIMED after handoff.
        _, state = self.read()
        self.owned(self.task(state, task_id), receipt)
        return result

    @staticmethod
    def producer_session(task, index):
        for output in reversed(task["outputs"][:index]):
            stage = next(s for s in task["stages"] if s["id"] == output["stage_id"])
            if stage["mode"] == "write":
                return output["session"]
        return identifier(task.get("input_session"))

    @staticmethod
    def task(state, task_id):
        require(task_id in state["tasks"], "unknown task: " + task_id)
        return state["tasks"][task_id]

    @staticmethod
    def event(task, action, owner):
        task["history"].append({"action": action, "session": owner["session"],
                                "lease_id": owner["lease_id"], "at": now(),
                                **{key: owner[key] for key in ("worktree", "role", "input_commit")
                                   if key in owner}})

    @staticmethod
    def owned(task, receipt):
        supplied = receipt_owner(receipt)
        held = task.get("owner")
        require(task["phase"] == "running" and held and
                all(held.get(key) == value for key, value in supplied.items()),
                "not the current running owner; stale or foreign receipt")
        return held

    def release(self, receipt, request_id, reason):
        require(isinstance(reason, str) and reason.strip(), "release needs a reason / next action")
        owner = receipt_owner(receipt)

        def operation(state):
            task = self.task(state, receipt["task_id"])
            held = self.owned(task, receipt)
            self.event(task, "release", held)
            task.update(phase="ready", owner=None, next_action=reason)
            return {"task_id": task["task_id"], "released_lease": owner["lease_id"]}, []
        return self.transaction(request_id, {"release": receipt["task_id"],
                                            "owner": owner, "reason": reason}, operation)

    def rework(self, receipt, evidence, next_action, request_id):
        owner = receipt_owner(receipt)
        require(isinstance(evidence, dict) and evidence.get("verdict") in ("fail", "blocked"),
                "rework evidence.verdict must be fail or blocked")
        require(isinstance(next_action, str) and next_action.strip(), "next_action is required")

        def operation(state):
            task = self.task(state, receipt["task_id"])
            held = self.owned(task, receipt)
            index = task["stage_index"]
            require(task["stages"][index]["mode"] == "verify", "only a verifier can request rework")
            require(evidence.get("tested_commit") == held["input_commit"],
                    "rework evidence must identify the accepted tested_commit")
            writes = [i for i in range(index) if task["stages"][i]["mode"] == "write"]
            require(writes, "verify-only task has no producer stage; release, then have the "
                    "coordinator cancel and enqueue a continuation with a producer stage")
            target = writes[-1]
            task["attempts"].append({"rejected_by": owner["session"], "at": now(),
                                     "evidence": evidence, "outputs": task["outputs"][target:]})
            task["outputs"] = task["outputs"][:target]
            # Continue from the rejected source, preserving the actual work.
            task["stage_inputs"][str(target)] = held["input_commit"]
            self.input_for(task, target)
            self.event(task, "rework", held)
            task.update(phase="ready", stage_index=target, owner=None, next_action=next_action)
            return {"task_id": task["task_id"], "phase": "ready",
                    "stage_id": task["stages"][target]["id"]}, []
        return self.transaction(request_id, {"rework": receipt["task_id"], "owner": owner,
                                            "evidence": evidence, "next_action": next_action}, operation)

    def coordinate(self, receipt, request_id, action, reason, additions=None):
        coordinator = receipt_owner(receipt)
        require(action in ("amend", "cancel"), "unknown coordinator action")
        require(isinstance(reason, str) and reason.strip(), "coordinator action needs a reason")
        additions = resources(additions) if action == "amend" else []

        def operation(state):
            task = self.task(state, receipt["task_id"])
            require(task["coordinator_owner"] == coordinator, "not the task coordinator receipt")
            require(task["phase"] in ("ready", "offered"),
                    "coordinator changes require an idle task; holder must release first")
            if action == "amend":
                expanded = sorted(set(task["resources"] + additions))
                for other in state["tasks"].values():
                    if other is not task and other["phase"] not in ("done", "cancelled"):
                        require(not overlaps(expanded, other["resources"]),
                                "resources reserved by task " + other["task_id"])
                task["resources"] = expanded
            else:
                task.update(phase="cancelled", owner=None)
            self.event(task, action, coordinator)
            task["next_action"] = reason
            return {"task_id": task["task_id"], "phase": task["phase"],
                    "resources": task["resources"]}, []
        return self.transaction(request_id, {"coordinate": receipt["task_id"],
                                            "owner": coordinator, "action": action,
                                            "additions": additions, "reason": reason}, operation)

    def publish(self, receipt, output_commit, evidence, next_action, request_id):
        owner = receipt_owner(receipt)
        self.commit(output_commit)
        require(isinstance(evidence, dict), "evidence must be a JSON object")
        require(isinstance(next_action, str) and next_action.strip(), "next_action is required")

        def operation(state):
            task = self.task(state, receipt["task_id"])
            held = self.owned(task, receipt)
            index = task["stage_index"]
            stage = task["stages"][index]
            input_commit = self.input_for(task, index)
            if stage["mode"] == "verify":
                require(output_commit == input_commit, "verify stages cannot change the input commit")
                require(evidence.get("verdict") == "pass" and
                        evidence.get("tested_commit") == input_commit,
                        "verify stages must record evidence.verdict=pass and the exact tested_commit")
            else:
                result = self.git("merge-base", "--is-ancestor", input_commit, output_commit,
                                  check=False)
                require(result.returncode == 0, "output does not descend from the accepted input")
            self.paths(output_commit, stage["produces"])
            output = {"stage_id": stage["id"], "commit": output_commit,
                      "input_commit": input_commit, "artifacts": stage["produces"],
                      "evidence": evidence, "session": owner["session"],
                      "lease_id": owner["lease_id"], "at": now(), "accepted_by": None}
            task["outputs"].append(output)
            last = index + 1 == len(task["stages"])
            if not last:
                # Do not strand an unconsumable output: validate the offered input.
                self.input_for(task, index + 1)
            self.event(task, "publish", held)
            task.update(phase="done" if last else "offered", next_action=next_action)
            if last:
                task["owner"] = None
            return {"task_id": task["task_id"], "phase": task["phase"],
                    "stage_id": stage["id"], "output_commit": output_commit}, [output_commit]
        return self.transaction(request_id, {"publish": receipt["task_id"], "owner": owner,
                                            "output": output_commit, "evidence": evidence,
                                            "next_action": next_action}, operation)


def read_json(path):
    try:
        return json.loads(pathlib.Path(path).read_text(encoding="utf-8"))
    except (OSError, ValueError) as exc:
        raise QueueError(f"cannot read JSON {path}: {exc}") from exc


def receipt_file(path, task_id, session):
    path = pathlib.Path(path)
    if path.exists():
        receipt = read_json(path)
        require(receipt.get("task_id") == task_id and receipt.get("session") == session,
                "receipt belongs to another task/session; choose a new file")
        receipt_owner(receipt)
        return receipt
    receipt = new_receipt(task_id, session)
    # Save the secret BEFORE sending the claim. Exclusive create avoids clobbering
    # another process's receipt. Parent directories must already exist.
    try:
        with path.open("x", encoding="utf-8") as fh:
            fh.write(encoded(receipt) + "\n")
        path.chmod(0o600)
    except OSError as exc:
        raise QueueError(f"cannot save private receipt {path}: {exc}") from exc
    return receipt


def main(argv=None, repo=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--remote", default="origin", help="configured Git remote (default origin)")
    sub = parser.add_subparsers(dest="command", required=True)
    init = sub.add_parser("init", help="initialize AFTER the documented fleet cutover")
    init.add_argument("--session", required=True)
    init.add_argument("--legacy-clients-stopped-and-upgraded", action="store_true", required=True)
    enqueue = sub.add_parser("enqueue", help="reserve resources for an explicit task")
    enqueue.add_argument("spec", help="JSON task specification")
    enqueue.add_argument("--request-id", required=True)
    enqueue.add_argument("--receipt", required=True, help="private coordinator receipt file")
    nxt = sub.add_parser("next", help="list ready stages with verified inputs")
    nxt.add_argument("--role", required=True)
    sub.add_parser("list", help="read the durable state")
    claim = sub.add_parser("claim", help="accept the next stage; save a private receipt")
    claim.add_argument("task_id")
    for arg in ("stage", "role", "session", "input-commit", "worktree", "receipt"):
        claim.add_argument("--" + arg, required=True)
    release = sub.add_parser("release", help="relinquish a running lease, preserving task and outputs")
    release.add_argument("--receipt", required=True)
    release.add_argument("--request-id", required=True)
    release.add_argument("--reason", required=True)
    publish = sub.add_parser("publish", help="publish an immutable output and offer the handoff")
    for arg in ("receipt", "request-id", "output-commit", "next-action"):
        publish.add_argument("--" + arg, required=True)
    publish.add_argument("--evidence", help="JSON evidence object; required for verify stages")
    rework = sub.add_parser("rework", help="return a failed verification to its producer stage")
    for arg in ("receipt", "request-id", "evidence", "next-action"):
        rework.add_argument("--" + arg, required=True)
    for command in ("amend", "cancel"):
        operation = sub.add_parser(command, help="coordinator recovery of an idle task")
        for arg in ("receipt", "request-id", "reason"):
            operation.add_argument("--" + arg, required=True)
        if command == "amend":
            operation.add_argument("--resource", action="append", required=True)
    args = parser.parse_args(argv)
    queue = Queue(repo or pathlib.Path(__file__).resolve().parent.parent, args.remote)
    try:
        if args.command == "init":
            result = queue.init(args.session)
        elif args.command == "enqueue":
            spec = read_json(args.spec)
            receipt = receipt_file(args.receipt, spec["task_id"], spec["coordinator"])
            result = queue.enqueue(spec, args.request_id, receipt)
        elif args.command == "next":
            result = queue.next(args.role)
        elif args.command == "list":
            sha, state = queue.read()
            result = {"state_commit": sha, **state}
        elif args.command == "claim":
            receipt = receipt_file(args.receipt, args.task_id, args.session)
            result = queue.claim(receipt, args.stage, args.role, args.input_commit, args.worktree)
        elif args.command == "release":
            result = queue.release(read_json(args.receipt), args.request_id, args.reason)
        elif args.command == "rework":
            result = queue.rework(read_json(args.receipt), read_json(args.evidence),
                                  args.next_action, args.request_id)
        elif args.command in ("amend", "cancel"):
            result = queue.coordinate(read_json(args.receipt), args.request_id,
                                      args.command, args.reason, getattr(args, "resource", None))
        else:
            result = queue.publish(read_json(args.receipt), args.output_commit,
                                   read_json(args.evidence) if args.evidence else {},
                                   args.next_action, args.request_id)
        print(json.dumps(result, indent=2))
        return 0
    except QueueError as exc:
        print(f"DENIED: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
