"""PR merge-check tests; all API calls are fixtures, with no network or writes."""
import copy
import json
import pathlib
import subprocess
import sys
import unittest
from unittest.mock import patch
from urllib.parse import unquote

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import check_pr_source_review as gate
from test_source_review import HEAD, BASE, WORKFLOW, evidence


def pull_request(branch="main"):
    return {"state": "open", "head": {"sha": HEAD},
            "base": {"sha": BASE, "ref": branch,
                     "repo": {"full_name": "tangosdev/sm64ds-decomp"}},
            "changed_files": 1}


def state():
    return {"schema": 3, "source_review_policy": {"workflow_commit": WORKFLOW}, "tasks": {
        "actor": {"task_id": "actor", "phase": "done", "input_session": "producer",
                  "resources": ["file:src/actor.cpp", "file:include/actor.h"],
                  "stages": [{"id": "verify", "mode": "verify"}],
                  "outputs": [{"stage_id": "verify", "session": "reviewer", "commit": HEAD,
                               "evidence": evidence()}]}}}


class PRSourceReviewTest(unittest.TestCase):
    def test_large_queue_uses_raw_content_at_the_pinned_commit(self):
        sha = "d" * 40
        snapshot = state()
        snapshot["history_fixture"] = "x" * 1_100_000
        replies = [json.dumps({"object": {"sha": sha}}), json.dumps(snapshot)]
        with patch.object(gate.subprocess, "run", side_effect=[
                subprocess.CompletedProcess([], 0, stdout=reply, stderr="")
                for reply in replies]) as run:
            self.assertEqual(gate.queue_state("tangosdev/sm64ds-decomp"), (sha, snapshot))
        self.assertEqual(run.call_count, 2)
        self.assertEqual(run.call_args_list[1].args[0], [
            "gh", "api", f"repos/tangosdev/sm64ds-decomp/contents/state.json?ref={sha}",
            "-H", "Accept: application/vnd.github.raw+json"])

    def test_old_queue_and_unreviewed_source_block_but_tooling_can_land(self):
        self.assertEqual(gate.evaluate({}, HEAD, BASE, ["src/actor.cpp"])["result"], "fail")
        self.assertEqual(gate.evaluate({}, HEAD, BASE, ["tools/source_review.py"])["result"], "pass")
        self.assertEqual(gate.evaluate({}, HEAD, BASE, ["config/arm9/delinks.txt"])["result"], "fail")

    def test_exact_published_review_passes_and_stale_or_partial_review_fails(self):
        self.assertEqual(gate.evaluate(state(), HEAD, BASE, ["src/actor.cpp"])["result"], "pass")
        for head, base, files in ((WORKFLOW, BASE, ["src/actor.cpp"]),
                                  (HEAD, WORKFLOW, ["src/actor.cpp"]),
                                  (HEAD, BASE, ["src/actor.cpp", "src/unreviewed.cpp"])):
            self.assertEqual(gate.evaluate(state(), head, base, files)["result"], "fail")

    def test_self_review_cancelled_task_and_unstructured_pass_fail(self):
        for change in ("author", "cancelled", "bare"):
            snapshot = state()
            task = snapshot["tasks"]["actor"]
            if change == "author":
                task["input_session"] = "reviewer"
            elif change == "cancelled":
                task["phase"] = "cancelled"
            else:
                task["outputs"][0]["evidence"] = {"verdict": "pass", "tested_commit": HEAD}
            self.assertEqual(gate.evaluate(snapshot, HEAD, BASE, ["src/actor.cpp"])["result"], "fail")

    def test_integrator_cannot_invent_an_independent_composition_reviewer(self):
        snapshot = state()
        output = snapshot["tasks"]["actor"]["outputs"][0]
        output["commit"] = BASE
        output["evidence"].update(composition_commit=HEAD, composition_base=BASE,
                                 composition_independent_verification=evidence())
        self.assertEqual(gate.evaluate(snapshot, HEAD, BASE, ["src/actor.cpp"])["result"], "fail")

    def test_queue_change_before_publication_emits_failure(self):
        pr = pull_request()
        posted = []

        def api(path, payload=None, paginate=False):
            if path.endswith("/check-runs"):
                posted.append(payload)
                return {}
            if "/files?" in path:
                return [[{"filename": "src/actor.cpp"}]]
            if path.endswith("/git/ref/heads/main"):
                return {"ref": "refs/heads/main", "object": {"type": "commit", "sha": BASE}}
            if "/git/ref/" in path:
                return {"object": {"sha": "e" * 40}}
            return copy.deepcopy(pr)

        with patch.object(gate, "api", side_effect=api), patch.object(
                gate, "queue_state", return_value=("d" * 40, state())):
            result = gate.check_pr("tangosdev/sm64ds-decomp", 2447, publish=True)
        self.assertEqual(result["result"], "fail")
        self.assertEqual(posted[0]["conclusion"], "failure")

    def test_renamed_source_requires_review_of_old_and_new_paths(self):
        pr = pull_request()
        with patch.object(gate, "target_branch", return_value=("main", BASE)), patch.object(
                gate, "api", side_effect=[pr, [[{
                "filename": "src/actor.cpp", "previous_filename": "src/old_actor.c"}]]]), patch.object(
                gate, "queue_state", return_value=("d" * 40, state())):
            self.assertEqual(gate.check_pr("tangosdev/sm64ds-decomp", 2447)["result"], "fail")

    def test_truncated_api_file_list_fails_closed(self):
        pr = pull_request()
        pr["changed_files"] = 3001
        with patch.object(gate, "target_branch", return_value=("main", BASE)), patch.object(
                gate, "api", side_effect=[pr, [[{"filename": "notes/inert.md"}]]]):
            self.assertEqual(gate.check_pr("tangosdev/sm64ds-decomp", 2447)["result"], "fail")


class TargetBranchReviewTest(unittest.TestCase):
    def run_fixture(self, refs, review_base=BASE, publish=False, source=True,
                    branch="main", final_branch=None, final_head=None):
        snapshot = state()
        output = snapshot["tasks"]["actor"]["outputs"][0]["evidence"]
        output["tested_base"] = review_base
        output["source_review"]["reviewed_base"] = review_base
        pr = pull_request(branch)
        replies = iter(refs)
        calls, posts = [], []
        reads = 0

        def api(path, payload=None, paginate=False):
            nonlocal reads
            calls.append(path)
            if path.endswith("/check-runs"):
                posts.append(payload)
                return {}
            if "/files?" in path:
                return [[{"filename": "src/actor.cpp" if source else "tools/inert.py"}]]
            if path.endswith("/git/ref/heads/agents/coordination"):
                return {"object": {"sha": "d" * 40}}
            if "/git/ref/" in path:
                value = next(replies)
                if isinstance(value, Exception):
                    raise value
                if isinstance(value, dict):
                    return value
                return {"ref": "refs/" + unquote(path.split("/git/ref/")[1]),
                        "object": {"type": "commit", "sha": value}}
            if "/pulls/" in path:
                reads += 1
                current = copy.deepcopy(pr)
                if reads > 1 and final_branch is not None:
                    current["base"]["ref"] = final_branch
                if reads > 1 and final_head is not None:
                    current["head"]["sha"] = final_head
                return current
            raise AssertionError("Unexpected API request: " + path)

        with patch.object(gate, "api", side_effect=api), patch.object(
                gate, "queue_state", return_value=("d" * 40, snapshot)) as queue:
            result = gate.check_pr("tangosdev/sm64ds-decomp", 2447, publish=publish)
        return result, calls, posts, queue.call_count

    def test_stale_pr_base_does_not_accept_old_review_or_reject_current_review(self):
        tip = "e" * 40
        old, calls, _, _ = self.run_fixture([tip])
        self.assertEqual(old["result"], "fail")
        self.assertEqual(old["base"], tip)
        self.assertEqual(old["pr_base"], BASE)
        self.assertIn("repos/tangosdev/sm64ds-decomp/git/ref/heads/main", calls)
        current, _, _, _ = self.run_fixture([tip, tip], review_base=tip)
        self.assertEqual(current["result"], "pass")

    def test_target_advance_during_read_only_evaluation_invalidates_success(self):
        result, calls, posts, _ = self.run_fixture([BASE, "e" * 40])
        self.assertEqual(result["result"], "fail")
        self.assertEqual(result["observed_base"], "e" * 40)
        self.assertEqual(posts, [])
        self.assertTrue(calls[-1].endswith("/git/ref/heads/main"))

    def test_final_read_failure_also_blocks_read_only_integrator_check(self):
        result, _, posts, _ = self.run_fixture([BASE, RuntimeError("ref unavailable")])
        self.assertEqual(result["result"], "fail")
        self.assertEqual(posts, [])

    def test_head_movement_blocks_read_only_and_publishing_checks(self):
        for publish in (False, True):
            with self.subTest(publish=publish):
                result, _, posts, _ = self.run_fixture([BASE], publish=publish, final_head="f" * 40)
                self.assertEqual(result["result"], "fail")
                self.assertIn("PR changed", result["summary"])
                self.assertEqual(posts, [])

    def test_target_advance_before_publication_invalidates_success(self):
        result, calls, posts, _ = self.run_fixture([BASE, "e" * 40], publish=True)
        self.assertEqual(result["result"], "fail")
        self.assertIn("Target branch changed", result["summary"])
        self.assertEqual(posts[0]["conclusion"], "failure")
        self.assertTrue(calls[-2].endswith("/git/ref/heads/main"))
        self.assertTrue(calls[-1].endswith("/check-runs"))

    def test_current_base_review_can_publish_despite_retained_pr_base_metadata(self):
        tip = "e" * 40
        result, _, posts, _ = self.run_fixture([tip, tip], review_base=tip, publish=True)
        self.assertEqual(result["result"], "pass")
        self.assertEqual(result["base"], tip)
        self.assertEqual(posts[0]["conclusion"], "success")
        self.assertIn(":" + tip + ":", posts[0]["external_id"])

    def test_initial_ref_failure_does_not_fall_back_to_pr_base(self):
        result, _, posts, queue_reads = self.run_fixture([RuntimeError("ref unavailable")])
        self.assertEqual(result["result"], "fail")
        self.assertIsNone(result["base"])
        self.assertEqual(posts, [])
        self.assertEqual(queue_reads, 0)

    def test_publication_ref_failure_emits_failure_instead_of_old_success(self):
        result, _, posts, _ = self.run_fixture([BASE, RuntimeError("ref unavailable")], publish=True)
        self.assertEqual(result["result"], "fail")
        self.assertIn("could not be confirmed", result["summary"])
        self.assertEqual(posts[0]["conclusion"], "failure")

    def test_wrong_or_malformed_ref_cannot_supply_a_review_base(self):
        cases = [
            {},
            {"ref": "refs/heads/main", "object": {"type": "commit", "sha": "bad"}},
            {"ref": "refs/heads/other", "object": {"type": "commit", "sha": BASE}},
            {"ref": "refs/heads/main", "object": {"type": "tag", "sha": BASE}},
        ]
        for reply in cases:
            with self.subTest(reply=reply):
                result, _, _, _ = self.run_fixture([reply])
                self.assertEqual(result["result"], "fail")
                self.assertIsNone(result["base"])

    def test_retargeting_to_same_oid_still_invalidates_publication(self):
        result, _, posts, _ = self.run_fixture([BASE, BASE], publish=True, final_branch="release")
        self.assertEqual(result["result"], "fail")
        self.assertEqual(posts[0]["conclusion"], "failure")

    def test_branch_names_with_slashes_use_exact_escaped_ref(self):
        result, calls, _, _ = self.run_fixture([BASE, BASE], branch="release/2026")
        self.assertEqual(result["result"], "pass")
        self.assertEqual(result["base_ref"], "release/2026")
        self.assertIn("repos/tangosdev/sm64ds-decomp/git/ref/heads/release%2F2026", calls)

    def test_tooling_exemption_does_not_invent_target_identity(self):
        result, _, _, reads = self.run_fixture([BASE, BASE], source=False)
        self.assertEqual(result["result"], "pass")
        self.assertEqual(reads, 0)
        unavailable, _, _, _ = self.run_fixture([RuntimeError("missing branch")], source=False)
        self.assertEqual(unavailable["result"], "fail")

    def test_different_base_repository_is_not_followed(self):
        pr = pull_request()
        pr["base"]["repo"]["full_name"] = "another/repository"
        with patch.object(gate, "api", return_value=pr) as api:
            result = gate.check_pr("tangosdev/sm64ds-decomp", 2447)
        self.assertEqual(result["result"], "fail")
        self.assertEqual(api.call_count, 1)


if __name__ == "__main__":
    unittest.main()
