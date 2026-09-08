"""PR merge-check tests; all API calls are fixtures, with no network or writes."""
import copy
import pathlib
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import check_pr_source_review as gate
from test_source_review import HEAD, BASE, WORKFLOW, evidence


def state():
    return {"schema": 3, "source_review_policy": {"workflow_commit": WORKFLOW}, "tasks": {
        "actor": {"task_id": "actor", "phase": "done", "input_session": "producer",
                  "resources": ["file:src/actor.cpp", "file:include/actor.h"],
                  "stages": [{"id": "verify", "mode": "verify"}],
                  "outputs": [{"stage_id": "verify", "session": "reviewer", "commit": HEAD,
                               "evidence": evidence()}]}}}


class PRSourceReviewTest(unittest.TestCase):
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
        pr = {"state": "open", "head": {"sha": HEAD}, "base": {"sha": BASE}, "changed_files": 1}
        posted = []

        def api(path, payload=None, paginate=False):
            if path.endswith("/check-runs"):
                posted.append(payload)
                return {}
            if "/files?" in path:
                return [[{"filename": "src/actor.cpp"}]]
            if "/git/ref/" in path:
                return {"object": {"sha": "e" * 40}}
            return copy.deepcopy(pr)

        with patch.object(gate, "api", side_effect=api), patch.object(
                gate, "queue_state", return_value=("d" * 40, state())):
            result = gate.check_pr("tangosdev/sm64ds-decomp", 2447, publish=True)
        self.assertEqual(result["result"], "fail")
        self.assertEqual(posted[0]["conclusion"], "failure")

    def test_renamed_source_requires_review_of_old_and_new_paths(self):
        pr = {"state": "open", "head": {"sha": HEAD}, "base": {"sha": BASE}, "changed_files": 1}
        with patch.object(gate, "api", side_effect=[pr, [[{
                "filename": "src/actor.cpp", "previous_filename": "src/old_actor.c"}]]]), patch.object(
                gate, "queue_state", return_value=("d" * 40, state())):
            self.assertEqual(gate.check_pr("tangosdev/sm64ds-decomp", 2447)["result"], "fail")

    def test_truncated_api_file_list_fails_closed(self):
        pr = {"state": "open", "head": {"sha": HEAD}, "base": {"sha": BASE}, "changed_files": 3001}
        with patch.object(gate, "api", side_effect=[pr, [[{"filename": "notes/inert.md"}]]]):
            self.assertEqual(gate.check_pr("tangosdev/sm64ds-decomp", 2447)["result"], "fail")


if __name__ == "__main__":
    unittest.main()
