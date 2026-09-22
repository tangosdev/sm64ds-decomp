"""Protocol tests against disposable LOCAL bare Git remotes, never origin/GitHub.

Races use separate repositories and subprocesses with a barrier immediately
before compare-and-swap. Both participants must read the same state first.
"""
import concurrent.futures
import copy
import json
import pathlib
import subprocess
import sys
import tempfile
import time
import unittest
from unittest.mock import patch

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import classqueue_v2 as cq
from test_source_review import evidence as review_evidence, finding


TOOLS = pathlib.Path(__file__).resolve().parent
RACER = r"""
import json, pathlib, sys, time
sys.path.insert(0, sys.argv[1])
import classqueue_v2 as cq
queue = cq.Queue(sys.argv[2])
command = json.loads(sys.argv[3])
barrier = pathlib.Path(sys.argv[4])
participant = sys.argv[5]
original = queue.save
def save(*args, **kwargs):
    (barrier / participant).touch()
    deadline = time.monotonic() + 20
    while len(list(barrier.iterdir())) != 2:
        if time.monotonic() > deadline:
            raise RuntimeError('race barrier timed out')
        time.sleep(0.01)
    return original(*args, **kwargs)
queue.save = save
try:
    action = command.pop('action')
    if 'operation_action' in command:
        command['action'] = command.pop('operation_action')
    print(json.dumps(getattr(queue, action)(**command)))
except cq.QueueError as exc:
    print(str(exc), file=sys.stderr)
    sys.exit(1)
"""


class GitProtocolTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="classqueue-v2-test-")
        self.addCleanup(self.tmp.cleanup)
        self.root = pathlib.Path(self.tmp.name)
        self.remote = self.root / "remote.git"
        self.coordinator_receipts = {}
        self.run_git(self.root, "init", "--bare", str(self.remote))
        self.a = self.make_repo("a")
        self.base = self.make_commit(self.a, {"README": "base"})
        self.a.git("push", "origin", f"{self.base}:refs/heads/main")
        self.b = self.make_repo("b")
        self.b.git("fetch", "origin", "refs/heads/main")
        self.a.init("coordinator")

    @staticmethod
    def run_git(repo, *args, input=None):
        result = subprocess.run(["git", "-C", str(repo), *args], input=input,
                                capture_output=True, text=True, encoding="utf-8")
        if result.returncode:
            raise AssertionError(result.stderr)
        return result.stdout.strip()

    def make_repo(self, name):
        path = self.root / name
        self.run_git(self.root, "init", str(path))
        self.run_git(path, "config", "user.name", "Queue Test")
        self.run_git(path, "config", "user.email", "queue-test@example.invalid")
        self.run_git(path, "remote", "add", "origin", str(self.remote))
        return cq.Queue(path)

    def make_commit(self, queue, files, parent=None):
        tree_files = {}
        for path, body in sorted(files.items()):
            blob = queue.git("hash-object", "-w", "--stdin", input=body).stdout.strip()
            node = tree_files
            parts = path.split("/")
            for part in parts[:-1]:
                node = node.setdefault(part, {})
            node[parts[-1]] = blob

        def make_tree(node):
            entries = []
            for name, value in sorted(node.items()):
                if isinstance(value, dict):
                    entries.append(f"040000 tree {make_tree(value)}\t{name}\n")
                else:
                    entries.append(f"100644 blob {value}\t{name}\n")
            return queue.git("mktree", input="".join(entries)).stdout.strip()
        tree = make_tree(tree_files)
        args = ["commit-tree", tree]
        if parent:
            args += ["-p", parent]
        return queue.git(*args, "-m", "test artifact").stdout.strip()

    def spec(self, task_id="task-a", resource="class:ov006/A+B", stages=None):
        return {"task_id": task_id, "coordinator": "coordinator", "resources": [resource],
                "base_commit": self.base, "workflow_commit": self.base,
                "next_action": "Inspect the ROM",
                "stages": stages or [
                    {"id": "facts", "role": "scout", "mode": "write",
                     "requires": [], "produces": ["facts.json"]},
                    {"id": "source", "role": "writer", "mode": "write",
                     "requires": ["facts.json"], "produces": ["source.cpp"]},
                    {"id": "check", "role": "builder", "mode": "verify",
                     "requires": ["source.cpp"], "produces": []}]}

    def enqueue(self, **kwargs):
        spec = self.spec(**kwargs)
        return self.a.enqueue(spec, "enqueue-" + kwargs.get("task_id", "task-a"), self.coordinator(spec))

    def coordinator(self, spec=None):
        spec = spec or self.spec()
        task_id = spec["task_id"]
        if task_id not in self.coordinator_receipts:
            self.coordinator_receipts[task_id] = cq.new_receipt(task_id, spec["coordinator"])
        return self.coordinator_receipts[task_id]

    def claim(self, queue=None, task_id="task-a", role="scout", stage="facts",
              session="scout-session", input_commit=None):
        receipt = cq.new_receipt(task_id, session)
        (queue or self.a).claim(receipt, stage, role, input_commit or self.base, "test-worktree")
        return receipt

    def scout_output(self):
        return self.make_commit(self.a, {"facts.json": '{"identity":"A"}'}, self.base)

    def publish_scout(self, receipt):
        output = self.scout_output()
        self.a.publish(receipt, output, {"identity": "ROM"}, "Write source", "publish-facts")
        return output

    def race(self, left, right):
        barrier = self.root / "barrier"
        barrier.mkdir()
        processes = [subprocess.Popen(
            [sys.executable, "-c", RACER, str(TOOLS), str(queue.repo),
             json.dumps(command), str(barrier), str(i)],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            for i, (queue, command) in enumerate(((self.a, left), (self.b, right)))]
        results = []
        try:
            with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
                futures = [pool.submit(process.communicate, timeout=30) for process in processes]
                for process, future in zip(processes, futures):
                    out, err = future.result()
                    results.append((process.returncode, out, err))
        finally:
            for process in processes:
                if process.poll() is None:
                    process.kill()
                    process.communicate()
        self.assertEqual(sorted(result[0] for result in results), [0, 1], results)
        self.assertNotIn("Traceback", "".join(r[2] for r in results), results)
        return results

    def test_roles_and_grouped_resources_are_exclusive(self):
        self.enqueue()
        self.assertEqual(self.a.next("writer"), [])
        with self.assertRaisesRegex(cq.QueueError, "role/stage"):
            self.claim(role="writer", stage="source")
        self.claim()
        with self.assertRaisesRegex(cq.QueueError, "not ready"):
            self.claim(queue=self.b, session="another-scout")
        for spelling in ("class:A", "class:ov007/B", "class:B+C"):
            with self.assertRaisesRegex(cq.QueueError, "reserved"):
                spec = self.spec("overlap", spelling)
                self.b.enqueue(spec, "overlap-" + str(len(spelling)), self.coordinator(spec))
        self.assertEqual(self.a.read()[1]["tasks"]["task-a"]["resources"], ["class:A", "class:B"])

    def test_real_group_acquisition_race_has_one_winner_no_partial_reservations(self):
        left = self.spec("left", "class:A+B")
        right = self.spec("right", "class:B+C")
        self.race({"action": "enqueue", "spec": left, "request_id": "left",
                   "coordinator_receipt": self.coordinator(left)},
                  {"action": "enqueue", "spec": right, "request_id": "right",
                   "coordinator_receipt": self.coordinator(right)})
        state = self.a.read()[1]
        self.assertEqual(len(state["tasks"]), 1)
        winner = next(iter(state["tasks"]))
        loser = right if winner == "left" else left
        with self.assertRaisesRegex(cq.QueueError, "reserved"):
            self.a.enqueue(loser, "retry", self.coordinator(loser))
        self.enqueue(task_id="unrelated", resource="class:Z")

    def test_real_claim_race_same_role_has_one_owner(self):
        self.enqueue()
        receipts = [cq.new_receipt("task-a", "session-" + str(i)) for i in range(2)]
        commands = [{"action": "claim", "receipt": receipt, "stage_id": "facts", "role": "scout",
                     "input_commit": self.base, "worktree": "test"} for receipt in receipts]
        results = self.race(*commands)
        winner = next(i for i, result in enumerate(results) if result[0] == 0)
        owner = self.a.read()[1]["tasks"]["task-a"]["owner"]
        self.assertEqual(owner["lease_id"], receipts[winner]["lease_id"])

    def test_foreign_stale_release_and_claim_replay_cannot_affect_replacement(self):
        self.enqueue()
        old = self.claim()
        foreign = copy.deepcopy(old)
        foreign["token"] = "0" * 64
        with self.assertRaisesRegex(cq.QueueError, "stale or foreign"):
            self.b.release(foreign, "foreign-release", "done")
        result = self.a.release(old, "release-old", "Resume scouting")
        replacement = self.claim(queue=self.b, session="replacement")
        self.assertEqual(self.a.release(old, "release-old", "Resume scouting"), result)
        with self.assertRaisesRegex(cq.QueueError, "stale or foreign"):
            self.a.release(old, "new-stale-release", "done")
        with self.assertRaisesRegex(cq.QueueError, "stale or foreign"):
            self.a.claim(old, "facts", "scout", self.base, "test-worktree")
        owner = self.a.read()[1]["tasks"]["task-a"]["owner"]
        self.assertEqual(owner["lease_id"], replacement["lease_id"])

    def test_release_publish_race_cannot_lose_an_output_or_delete_new_owner(self):
        self.enqueue()
        receipt = self.claim()
        output = self.scout_output()
        # Both clients need this object before the synchronized publish race.
        self.a.git("push", "origin", f"{output}:refs/heads/artifact")
        self.b.git("fetch", "origin", "refs/heads/artifact")
        results = self.race(
            {"action": "release", "receipt": receipt, "request_id": "release", "reason": "resume"},
            {"action": "publish", "receipt": receipt, "output_commit": output,
             "evidence": {}, "next_action": "write", "request_id": "publish"})
        task = self.a.read()[1]["tasks"]["task-a"]
        if results[0][0] == 0:
            self.assertEqual(task["phase"], "ready")
            self.assertEqual(task["outputs"], [])
        else:
            self.assertEqual(task["phase"], "offered")
            self.assertEqual(task["outputs"][0]["commit"], output)

    def test_handoff_persists_after_branch_deletion_and_acknowledges_consumer(self):
        self.enqueue()
        scout = self.claim()
        output = self.publish_scout(scout)
        self.a.git("push", "origin", f"{output}:refs/heads/scout")
        self.a.git("push", "origin", "--delete", "scout")
        # A completely fresh repo has no predecessor objects until queue.read fetches.
        fresh = self.make_repo("fresh")
        ready = fresh.next("writer")
        self.assertEqual(ready[0]["input_commit"], output)
        with self.assertRaisesRegex(cq.QueueError, "stale or foreign"):
            self.a.release(scout, "release-offered", "done")
        writer = self.claim(fresh, role="writer", stage="source", session="writer-session",
                            input_commit=output)
        task = self.a.read()[1]["tasks"]["task-a"]
        self.assertEqual(task["outputs"][0]["accepted_by"], writer["session"])
        with self.assertRaisesRegex(cq.QueueError, "stale or foreign"):
            self.a.publish(scout, output, {}, "write", "stale-publish")
        fresh.release(writer, "writer-paused", "Resume source")
        self.assertEqual(fresh.next("writer")[0]["input_commit"], output)
        with self.assertRaisesRegex(cq.QueueError, "reserved"):
            spec = self.spec("duplicate", "class:A")
            self.a.enqueue(spec, "duplicate", self.coordinator(spec))

    def test_missing_artifacts_wrong_input_and_wrong_stage_fail_closed(self):
        self.enqueue()
        scout = self.claim()
        with self.assertRaises(cq.QueueError):
            self.a.publish(scout, self.base, {}, "write", "bad-output")
        self.assertEqual(self.a.read()[1]["tasks"]["task-a"]["phase"], "running")
        output = self.publish_scout(scout)
        with self.assertRaisesRegex(cq.QueueError, "input changed"):
            self.claim(role="writer", stage="source", session="writer-session", input_commit=self.base)
        sha, state = self.a.read()
        state["tasks"]["task-a"]["outputs"][0]["stage_id"] = "other-stage"
        self.a.save(sha, state)
        with self.assertRaisesRegex(cq.QueueError, "wrong stage"):
            self.b.next("writer")

    def test_missing_objects_metadata_and_transport_are_errors(self):
        self.enqueue()
        sha, state = self.a.read()
        state["tasks"]["task-a"]["base_commit"] = "f" * 40
        self.a.save(sha, state)
        with self.assertRaises(cq.QueueError):
            self.b.next("scout")
        missing = cq.Queue(self.a.repo, str(self.root / "nonexistent.git"))
        with self.assertRaises(cq.QueueError):
            missing.next("scout")
        empty = self.root / "empty.git"
        self.run_git(self.root, "init", "--bare", str(empty))
        with self.assertRaisesRegex(cq.QueueError, "not initialized"):
            cq.Queue(self.a.repo, str(empty)).read()
        self.a.git("push", "origin", f"+{self.base}:{cq.REF}")
        with self.assertRaises(cq.QueueError):
            self.b.read()

    def test_legacy_claims_block_v2_but_old_clients_can_still_race_cutover(self):
        self.enqueue()
        # This deliberate old-style push proves v2 is NOT a lock on v1 clients.
        legacy = self.make_commit(self.b, {"legacy": "claim"})
        self.b.git("push", "origin", f"{legacy}:refs/claims/reviewer/ov006/A")
        with self.assertRaisesRegex(cq.QueueError, "legacy claims exist"):
            self.a.next("scout")
        with self.assertRaisesRegex(cq.QueueError, "legacy claims exist"):
            self.claim()
        with self.assertRaisesRegex(cq.QueueError, "legacy claims exist"):
            self.a.init("coordinator")

    def test_verify_is_readonly_independent_and_records_evidence(self):
        self.enqueue()
        scout = self.claim()
        facts = self.publish_scout(scout)
        writer = self.claim(role="writer", stage="source", session="writer-session", input_commit=facts)
        source = self.make_commit(self.a, {"source.cpp": "// source"}, facts)
        self.a.publish(writer, source, {}, "Verify", "publish-source")
        with self.assertRaisesRegex(cq.QueueError, "different session"):
            self.claim(role="builder", stage="check", session="writer-session", input_commit=source)
        verifier = self.claim(role="builder", stage="check", session="verifier", input_commit=source)
        changed = self.make_commit(self.a, {"source.cpp": "// changed"}, source)
        with self.assertRaisesRegex(cq.QueueError, "cannot change"):
            self.a.publish(verifier, changed, {"gate": "pass"}, "done", "bad-verify")
        with self.assertRaisesRegex(cq.QueueError, "record evidence"):
            self.a.publish(verifier, source, {}, "done", "empty-verify")
        result = self.a.publish(verifier, source, {"verdict": "pass", "tested_commit": source},
                                "Review integration separately", "verify")
        self.assertEqual(result["phase"], "done")
        self.assertEqual(self.a.read()[1]["tasks"]["task-a"]["outputs"][-1]["commit"], source)
        # Explicit follow-up tasks are independent of the promotion TSV's DONE flag.
        self.enqueue(task_id="already-promoted-followup", resource="class:A")
        self.assertEqual(self.a.next("scout")[0]["task_id"], "already-promoted-followup")

    def test_idempotence_request_collision_and_uncertain_push(self):
        spec = self.spec()
        result = self.a.enqueue(spec, "enqueue", self.coordinator(spec))
        old = self.a.read()[0]
        self.assertEqual(self.a.enqueue(spec, "enqueue", self.coordinator(spec)), result)
        self.assertEqual(self.a.read()[0], old)
        with self.assertRaisesRegex(cq.QueueError, "different operation"):
            other = self.spec("other", "class:Z")
            self.a.enqueue(other, "enqueue", self.coordinator(other))
        receipt = cq.new_receipt("task-a", "scout-session")
        save = self.a.save

        def landed_then_disconnect(*args, **kwargs):
            save(*args, **kwargs)
            raise cq.QueueError("connection lost after push landed")

        with patch.object(self.a, "save", landed_then_disconnect):
            with self.assertRaisesRegex(cq.QueueError, "connection lost"):
                self.a.claim(receipt, "facts", "scout", self.base, "test")
        self.a.claim(receipt, "facts", "scout", self.base, "test")
        task = self.a.read()[1]["tasks"]["task-a"]
        self.assertEqual(len(task["history"]), 1)

    def test_remote_state_contains_no_private_token(self):
        self.enqueue()
        receipt = self.claim()
        _, state = self.b.read()
        self.assertNotIn(receipt["token"], json.dumps(state))
        self.assertEqual(state["tasks"]["task-a"]["owner"]["token_hash"],
                         cq.receipt_owner(receipt)["token_hash"])

    def test_repeated_roles_have_distinct_stage_ids_and_no_policy_sequence(self):
        stages = [{"id": "sustained-producer", "role": "writer", "mode": "write",
                   "requires": [], "produces": []},
                  {"id": "layout-followup", "role": "writer", "mode": "write",
                   "requires": [], "produces": []}]
        self.enqueue(stages=stages)
        receipt = self.claim(role="writer", stage="sustained-producer", session="producer")
        self.a.publish(receipt, self.base, {}, "Continue layout", "pass-one")
        self.assertEqual(self.a.next("writer")[0]["stage"]["id"], "layout-followup")

    def test_adopt_existing_source_and_separate_scout_commit_without_rewriting(self):
        source = self.make_commit(self.a, {"source.cpp": "// existing source"}, self.base)
        facts = self.scout_output()
        spec = self.spec(stages=[{"id": "continue", "role": "producer", "mode": "write",
                                 "requires": ["source.cpp"], "produces": ["source.cpp"]}])
        spec.update(input_commit=source, previous_branch="cpp/existing-work",
                    evidence_inputs=[{"kind": "scout-facts", "commit": facts,
                                      "paths": ["facts.json"]}])
        self.a.enqueue(spec, "adopt-existing", self.coordinator(spec))
        fresh = self.make_repo("fresh-continuation")
        task = fresh.next("producer")[0]
        self.assertEqual(task["base_commit"], self.base)
        self.assertEqual(task["input_commit"], source)
        self.assertEqual(task["evidence_inputs"][0]["commit"], facts)
        fresh.paths(facts, ["facts.json"])
        self.claim(fresh, role="producer", stage="continue", session="new-session",
                   input_commit=source)

    def test_adoption_rejects_missing_evidence_and_unrelated_source(self):
        spec = self.spec()
        spec["evidence_inputs"] = [{"commit": "f" * 40, "paths": ["facts.json"]}]
        with self.assertRaises(cq.QueueError):
            self.a.enqueue(spec, "missing-evidence", self.coordinator(spec))
        spec["evidence_inputs"] = []
        spec["input_commit"] = self.make_commit(self.a, {"source.cpp": "unrelated root"})
        with self.assertRaisesRegex(cq.QueueError, "descend"):
            self.a.enqueue(spec, "unrelated-input", self.coordinator(spec))

    def ready_verification(self, extra_stages=()):
        stages = [{"id": "producer", "role": "producer", "mode": "write",
                   "requires": [], "produces": ["source.cpp"]},
                  {"id": "verify", "role": "verifier", "mode": "verify",
                   "requires": ["source.cpp"], "produces": []}, *extra_stages]
        self.enqueue(stages=stages)
        producer = self.claim(role="producer", stage="producer", session="producer")
        source = self.make_commit(self.a, {"source.cpp": "// candidate"}, self.base)
        self.a.publish(producer, source, {}, "Verify candidate", "publish-candidate")
        verifier = self.claim(role="verifier", stage="verify", session="verifier", input_commit=source)
        return producer, source, verifier

    def test_blocked_verification_does_not_complete_or_free_resources(self):
        _, source, verifier = self.ready_verification()
        for evidence in ({"verdict": "blocked", "tested_commit": source},
                         {"verdict": "pass", "tested_commit": self.base}):
            with self.assertRaisesRegex(cq.QueueError, "record evidence"):
                self.a.publish(verifier, source, evidence, "done", "bad-verdict")
        self.assertEqual(self.a.read()[1]["tasks"]["task-a"]["phase"], "running")
        with self.assertRaisesRegex(cq.QueueError, "reserved"):
            self.enqueue(task_id="overlapping", resource="class:A")

    def test_rework_preserves_rejected_source_and_requires_fresh_independent_verification(self):
        producer, source, verifier = self.ready_verification()
        evidence = {"verdict": "fail", "tested_commit": source, "reason": "WRONG-DEST"}
        with self.assertRaisesRegex(cq.QueueError, "stale or foreign"):
            self.a.rework(producer, evidence, "Fix callee", "foreign-rework")
        self.a.rework(verifier, evidence, "Fix callee", "rework")
        ready = self.b.next("producer")[0]
        self.assertEqual(ready["input_commit"], source)
        self.assertEqual(ready["stage"]["id"], "producer")
        producer2 = self.claim(self.b, role="producer", stage="producer", session="producer",
                               input_commit=source)
        revised = self.make_commit(self.b, {"source.cpp": "// correct callee"}, source)
        self.b.publish(producer2, revised, {}, "Reverify", "publish-revised")
        with self.assertRaisesRegex(cq.QueueError, "stale or foreign"):
            self.a.publish(verifier, source, {"verdict": "pass", "tested_commit": source},
                           "done", "stale-verification")
        new_verifier = self.claim(role="verifier", stage="verify", session="verifier-2",
                                  input_commit=revised)
        self.a.publish(new_verifier, revised, {"verdict": "pass", "tested_commit": revised},
                       "Ready for integration", "reverified")
        task = self.a.read()[1]["tasks"]["task-a"]
        self.assertEqual(task["phase"], "done")
        self.assertEqual(task["attempts"][0]["outputs"][0]["commit"], source)
        self.assertEqual(task["attempts"][0]["evidence"], evidence)

    def test_verify_only_adoption_requires_producer_identity(self):
        spec = self.spec(stages=[{"id": "verify", "role": "verifier", "mode": "verify",
                                 "requires": [], "produces": []}])
        with self.assertRaises(cq.QueueError):
            self.a.enqueue(spec, "no-identity", self.coordinator(spec))
        spec["input_session"] = "producer"
        self.a.enqueue(spec, "adopt-verify", self.coordinator(spec))
        with self.assertRaisesRegex(cq.QueueError, "different session"):
            self.claim(role="verifier", stage="verify", session="producer")
        verifier = self.claim(role="verifier", stage="verify", session="independent")
        with self.assertRaisesRegex(cq.QueueError, "no producer stage"):
            self.a.rework(verifier, {"verdict": "fail", "tested_commit": self.base},
                          "needs source work", "rework-verify-only")
        self.a.release(verifier, "release-verify-only", "Failure needs producer continuation")
        self.a.coordinate(self.coordinator(spec), "cancel-verify-only", "cancel", "Superseded")
        self.enqueue(task_id="continuation", resource="class:A")

    def test_verify_chain_remembers_original_source_producer(self):
        third = {"id": "review", "role": "reviewer", "mode": "verify", "requires": [], "produces": []}
        _, source, verifier = self.ready_verification([third])
        self.a.publish(verifier, source, {"verdict": "pass", "tested_commit": source},
                       "Independent review", "verified")
        with self.assertRaisesRegex(cq.QueueError, "different session"):
            self.claim(role="reviewer", stage="review", session="producer", input_commit=source)

    def test_coordinator_recovery_is_fenced_and_preserves_history(self):
        self.enqueue()
        coordinator = self.coordinator()
        foreign = cq.new_receipt("task-a", "coordinator")
        with self.assertRaisesRegex(cq.QueueError, "not the task coordinator"):
            self.a.coordinate(foreign, "foreign-cancel", "cancel", "supersede")
        owner = self.claim()
        for action, additions in (("cancel", None), ("amend", ["file:include/shared.h"])):
            with self.assertRaisesRegex(cq.QueueError, "idle task"):
                self.a.coordinate(coordinator, action + "-running", action, "dependency", additions)
        self.a.release(owner, "pause", "Reserve dependency before continuing")
        self.a.coordinate(coordinator, "expand", "amend", "Need shared header", ["file:include/shared.h"])
        self.assertIn("file:include/shared.h", self.a.read()[1]["tasks"]["task-a"]["resources"])
        with self.assertRaisesRegex(cq.QueueError, "reserved"):
            self.enqueue(task_id="conflict", resource="file:include/shared.h")
        self.a.coordinate(coordinator, "cancel", "cancel", "Superseded by issue-2")
        task = self.a.read()[1]["tasks"]["task-a"]
        self.assertEqual(task["phase"], "cancelled")
        self.assertEqual([e["action"] for e in task["history"]], ["claim", "release", "amend", "cancel"])
        self.assertEqual(task["history"][0]["worktree"], "test-worktree")
        self.assertEqual(task["history"][1]["input_commit"], self.base)
        self.enqueue(task_id="replacement", resource="file:include/shared.h")

    def test_real_resource_expansion_race_has_one_winner(self):
        self.enqueue()
        other = self.spec("other", "class:Z")
        self.a.enqueue(other, "enqueue-other", self.coordinator(other))
        self.race(*[{"action": "coordinate", "receipt": self.coordinator(spec),
                     "request_id": "expand-" + spec["task_id"],
                     "reason": "Shared dependency", "additions": ["file:include/shared.h"],
                     "operation_action": "amend"} for spec in (self.spec(), other)])
        tasks = self.a.read()[1]["tasks"].values()
        self.assertEqual(sum("file:include/shared.h" in task["resources"] for task in tasks), 1)

    def test_real_cancel_claim_race_cannot_cancel_running_work(self):
        self.enqueue()
        receipt = cq.new_receipt("task-a", "worker")
        self.race({"action": "coordinate", "operation_action": "cancel", "receipt": self.coordinator(),
                   "request_id": "cancel", "reason": "Superseded"},
                  {"action": "claim", "receipt": receipt, "stage_id": "facts", "role": "scout",
                   "input_commit": self.base, "worktree": "test"})
        task = self.a.read()[1]["tasks"]["task-a"]
        self.assertIn(task["phase"], ("running", "cancelled"))
        if task["phase"] == "running":
            self.assertEqual(task["owner"]["lease_id"], receipt["lease_id"])

    def test_rework_after_promotion_keeps_deleted_shard_as_original_input(self):
        original = self.make_commit(self.a, {"old-shard.c": "// shard"}, self.base)
        stages = [{"id": "promote", "role": "producer", "mode": "write",
                   "requires": ["old-shard.c"], "produces": ["TU.cpp"]},
                  {"id": "verify", "role": "verifier", "mode": "verify",
                   "requires": ["TU.cpp"], "produces": []}]
        spec = self.spec(stages=stages)
        spec["input_commit"] = original
        self.a.enqueue(spec, "enqueue-promotion", self.coordinator(spec))
        producer = self.claim(role="producer", stage="promote", session="producer", input_commit=original)
        promoted = self.make_commit(self.a, {"TU.cpp": "// promoted"}, original)
        self.a.publish(producer, promoted, {}, "Verify", "promote")
        verifier = self.claim(role="verifier", stage="verify", session="verifier", input_commit=promoted)
        self.a.rework(verifier, {"verdict": "fail", "tested_commit": promoted}, "Fix TU", "rework")
        self.assertEqual(self.a.next("producer")[0]["input_commit"], promoted)

    def review_workflow(self):
        return self.make_commit(self.a, {"tools/source_review.py": "# reviewed tool",
                                        "tools/check_pr_source_review.py": "# reviewed check"}, self.base)

    def enable_reviews(self):
        self.policy = self.review_workflow()
        before = self.a.read()[0]
        self.a.enable_source_review("coordinator", self.policy, before, "activate-review")

    def source_candidate(self):
        stages = [{"id": "write", "role": "producer", "mode": "write", "requires": [],
                   "produces": ["source.cpp"]},
                  {"id": "verify", "role": "verifier", "mode": "verify", "requires": ["source.cpp"],
                   "produces": []},
                  {"id": "integrate", "role": "integrator", "mode": "verify", "requires": ["source.cpp"],
                   "produces": []}]
        self.enqueue(stages=stages)
        writer = self.claim(role="producer", stage="write", session="producer")
        source = self.make_commit(self.a, {"source.cpp": "// candidate"}, self.base)
        self.a.publish(writer, source, {}, "Review source", "write-source")
        return source

    def evidence(self, head, reviewer="verifier"):
        result = review_evidence(head, self.base, reviewer, self.policy)
        result["source_review"]["files"] = ["source.cpp"]
        return result

    def test_review_activation_preserves_receipts_tasks_pins_and_history(self):
        self.enqueue()
        receipt = self.coordinator()
        before_sha, before = self.a.read()
        policy = self.review_workflow()
        result = self.a.enable_source_review("coordinator", policy, before_sha, "activate")
        after = self.a.read()[1]
        self.assertEqual(after["tasks"], before["tasks"])
        self.assertEqual(after["schema"], 3)
        self.assertEqual(receipt["schema"], 2)
        self.assertEqual(cq.receipt_owner(receipt), before["tasks"]["task-a"]["coordinator_owner"])
        self.assertEqual(self.a.enable_source_review("coordinator", policy, before_sha, "activate"), result)
        self.a.coordinate(receipt, "cancel-after-upgrade", "cancel", "Checkpoint retained")
        self.assertEqual(self.a.read()[1]["tasks"]["task-a"]["phase"], "cancelled")

    def test_review_activation_refuses_running_workers_foreign_coordinator_and_stale_inventory(self):
        self.enqueue()
        policy = self.review_workflow()
        sha = self.a.read()[0]
        with self.assertRaisesRegex(cq.QueueError, "coordinator"):
            self.a.enable_source_review("other", policy, sha, "foreign-upgrade")
        receipt = self.claim()
        with self.assertRaisesRegex(cq.QueueError, "queue changed"):
            self.a.enable_source_review("coordinator", policy, sha, "stale-upgrade")
        with self.assertRaisesRegex(cq.QueueError, "running leases"):
            self.a.enable_source_review("coordinator", policy, self.a.read()[0], "running-upgrade")
        self.assertEqual(self.a.read()[1]["schema"], 2)
        self.a.owned(self.a.read()[1]["tasks"]["task-a"], receipt)

    def test_required_review_rejects_invalid_evidence_without_losing_lease(self):
        self.enable_reviews()
        source = self.source_candidate()
        verifier = self.claim(role="verifier", stage="verify", session="verifier", input_commit=source)
        good = self.evidence(source)
        invalid = [{"verdict": "pass", "tested_commit": source}]
        for key, value in (("reviewed_commit", self.base), ("reviewer_session", "invented"),
                           ("findings", [finding("open")])):
            item = copy.deepcopy(good)
            item["source_review"][key] = value
            invalid.append(item)
        before = self.a.read()[0]
        for i, item in enumerate(invalid):
            with self.subTest(i=i), self.assertRaises(cq.QueueError):
                self.a.publish(verifier, source, item, "Integrate", "bad-review-" + str(i))
            self.assertEqual(self.a.read()[0], before)
        self.a.publish(verifier, source, good, "Integrate reviewed source", "good-review")
        integrator = self.claim(role="integrator", stage="integrate", session="integrator", input_commit=source)
        self.a.publish(integrator, source, self.evidence(source, "integrator"), "Done", "integrate")

    def test_existing_unreviewed_offer_cannot_be_integrated_after_activation(self):
        source = self.source_candidate()
        verifier = self.claim(role="verifier", stage="verify", session="verifier", input_commit=source)
        self.a.publish(verifier, source, {"verdict": "pass", "tested_commit": source}, "Integrate", "old-pass")
        self.enable_reviews()
        with self.assertRaisesRegex(cq.QueueError, "reviewer"):
            self.claim(role="integrator", stage="integrate", session="integrator", input_commit=source)
        self.assertEqual(self.a.read()[1]["tasks"]["task-a"]["phase"], "offered")

    def test_rework_requires_new_review_and_preserves_findings(self):
        self.enable_reviews()
        source = self.source_candidate()
        verifier = self.claim(role="verifier", stage="verify", session="verifier", input_commit=source)
        failure = self.evidence(source)
        failure["verdict"] = "fail"
        failure["source_review"].update(result="changes_requested", findings=[finding("open", "correctness")])
        self.a.rework(verifier, failure, "Fix finding", "reject-source")
        producer = self.claim(role="producer", stage="write", session="producer", input_commit=source)
        fixed = self.make_commit(self.a, {"source.cpp": "// corrected"}, source)
        self.a.publish(producer, fixed, {}, "Review correction", "publish-correction")
        verifier = self.claim(role="verifier", stage="verify", session="verifier", input_commit=fixed)
        with self.assertRaises(cq.QueueError):
            self.a.publish(verifier, fixed, self.evidence(source), "Integrate", "stale-review")
        with self.assertRaisesRegex(cq.QueueError, "disappeared"):
            self.a.publish(verifier, fixed, self.evidence(fixed), "Integrate", "dropped-finding")
        accepted = self.evidence(fixed)
        accepted["source_review"]["findings"] = [finding("fixed", "correctness")]
        self.a.publish(verifier, fixed, accepted, "Integrate", "fixed-review")

    def test_composition_requires_a_separately_published_review(self):
        self.enable_reviews()
        source = self.source_candidate()
        verifier = self.claim(role="verifier", stage="verify", session="verifier", input_commit=source)
        self.a.publish(verifier, source, self.evidence(source), "Integrate", "source-review")
        integrator = self.claim(role="integrator", stage="integrate", session="integrator", input_commit=source)
        composed = self.make_commit(self.a, {"source.cpp": "// composition"}, source)
        final = self.evidence(source, "integrator")
        final.update(composition_commit=composed, composition_base=self.base,
                     composition_independent_verification=self.evidence(composed, "invented-reviewer"))
        with self.assertRaises(cq.QueueError):
            self.a.publish(integrator, source, final, "Done", "invented-composition-review")
        spec = self.spec("composition-review", "file:notes/review.json", stages=[{
            "id": "verify", "role": "humanizer", "mode": "verify", "requires": ["source.cpp"], "produces": []}])
        spec.update(input_commit=composed, input_session="integrator", predecessor_tasks=["task-a"])
        self.a.enqueue(spec, "enqueue-composition-review", self.coordinator(spec))
        reviewer = self.claim(task_id="composition-review", role="humanizer", stage="verify",
                              session="independent-composition-reviewer", input_commit=composed)
        self.a.publish(reviewer, composed, self.evidence(composed, "independent-composition-reviewer"),
                       "Finish integration", "publish-composition-review")
        final["composition_review_task"] = "composition-review"
        self.a.publish(integrator, source, final, "Done", "accept-composition")
        self.assertEqual(self.a.read()[1]["tasks"]["task-a"]["phase"], "done")

    def test_source_graph_and_earlier_author_cannot_bypass_review(self):
        self.enable_reviews()
        with self.assertRaisesRegex(cq.QueueError, "independent verification"):
            self.enqueue(stages=[{"id": "write", "role": "other", "mode": "write", "requires": [], "produces": []}])
        source = self.source_candidate()
        spec = self.spec("adopt-review", "file:notes/adopt-review.json", stages=[{
            "id": "verify", "role": "verifier", "mode": "verify", "requires": ["source.cpp"], "produces": []}])
        spec.update(input_commit=source, input_session="later-writer", predecessor_tasks=["task-a"])
        self.a.enqueue(spec, "adopt-review", self.coordinator(spec))
        with self.assertRaisesRegex(cq.QueueError, "different session"):
            self.claim(task_id="adopt-review", role="verifier", stage="verify", session="producer", input_commit=source)


class ResourceTest(unittest.TestCase):
    def test_files_tus_and_module_qualified_intervals(self):
        self.assertTrue(cq.overlaps(cq.resources(["tu:src/Actor.cpp"]),
                                    cq.resources(["file:src/actor.cpp"])))
        a = cq.resources(["range:ov006:0x100:0x200"])
        self.assertTrue(cq.overlaps(a, cq.resources(["range:ov006:0x180:0x280"])))
        self.assertFalse(cq.overlaps(a, cq.resources(["range:ov007:0x180:0x280"])))
        self.assertFalse(cq.overlaps(a, cq.resources(["range:ov006:0x200:0x280"])))
        for value in ("class:A+", "file:../include/A.h", "range:6:1:2", "range:ov006:2:1"):
            with self.assertRaises(cq.QueueError):
                cq.resources([value])

    def test_receipt_saved_before_network_and_reused_without_printing_secret(self):
        with tempfile.TemporaryDirectory(prefix="queue-receipt-test-") as tmp:
            path = pathlib.Path(tmp) / "private.json"
            first = cq.receipt_file(path, "task", "session")
            self.assertEqual(cq.receipt_file(path, "task", "session"), first)
            with self.assertRaisesRegex(cq.QueueError, "another task/session"):
                cq.receipt_file(path, "task", "another-session")


if __name__ == "__main__":
    unittest.main()
