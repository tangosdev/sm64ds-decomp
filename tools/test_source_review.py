"""Behavioral tests for source-review acceptance and retained findings."""
import copy
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import source_review as sr

HEAD, BASE, WORKFLOW = "a" * 40, "b" * 40, "c" * 40


def review(head=HEAD, base=BASE, reviewer="reviewer"):
    return {"schema": 1, "result": "pass", "reviewed_commit": head,
            "reviewed_base": base, "reviewer_session": reviewer,
            "summary": "Reviewed the actor interface, lifecycle, naming and remaining bridges.",
            "completion": "complete", "files": ["src/actor.cpp", "include/actor.h"], "findings": []}


def evidence(head=HEAD, base=BASE, reviewer="reviewer", workflow=WORKFLOW):
    return {"verdict": "pass", "tested_commit": head, "tested_base": base,
            "workflow_commit": workflow, "reviewer_session": reviewer,
            "source_review": review(head, base, reviewer)}


def finding(disposition="fixed", kind="reconstruction"):
    return {"id": "actor-virtual-interface", "location": "src/actor.cpp:20",
            "summary": "Fake slot table duplicates the existing actor interface.",
            "kind": kind, "disposition": disposition,
            "reason": "The candidate now calls the declared actor method."}


class SourceReviewTest(unittest.TestCase):
    def check(self, value, **kwargs):
        return sr.validate(value, HEAD, BASE, "reviewer", {"producer"},
                           ["src/actor.cpp", "include/actor.h"], **kwargs)

    def test_review_binds_candidate_base_author_and_coverage(self):
        self.check(review())
        for key, value in (("reviewed_commit", BASE), ("reviewed_base", HEAD),
                           ("reviewer_session", "producer"), ("files", ["src/actor.cpp"]),
                           ("result", "not_run"), ("summary", "REPLACE_WITH_REVIEW")):
            with self.subTest(key=key), self.assertRaises(sr.ReviewError):
                item = review()
                item[key] = value
                self.check(item)
        with self.assertRaisesRegex(sr.ReviewError, "independent"):
            sr.validate(review(reviewer="producer"), HEAD, BASE, "producer", {"producer"})

    def test_missing_or_malformed_review_does_not_pass(self):
        for item in (None, {}, "pass", {"schema": 1}):
            with self.subTest(item=item), self.assertRaises(sr.ReviewError):
                self.check(item)

    def test_open_findings_block_and_cannot_disappear_or_change_kind(self):
        prior = {"actor-virtual-interface": finding("open", "correctness")}
        with self.assertRaisesRegex(sr.ReviewError, "disappeared"):
            self.check(review(), prior=prior)
        item = review()
        item["findings"] = [finding("open")]
        with self.assertRaisesRegex(sr.ReviewError, "unresolved"):
            self.check(item)
        item["findings"] = [finding()]
        with self.assertRaisesRegex(sr.ReviewError, "reclassified"):
            self.check(item, prior=prior)
        item["findings"] = [finding("fixed", "correctness")]
        self.check(item, prior=prior)

    def test_deferred_reconstruction_requires_partial_scope_owner_and_issue(self):
        item = review()
        item["findings"] = [finding("deferred")]
        with self.assertRaises(sr.ReviewError):
            self.check(item)
        item["completion"] = "partial"
        item["findings"][0].update(owner="actor-producer", issue="https://github.com/tangosdev/sm64ds-decomp/issues/2449")
        self.check(item)
        for kind in ("correctness", "provenance"):
            item["findings"][0]["kind"] = kind
            with self.assertRaisesRegex(sr.ReviewError, "cannot be deferred"):
                self.check(item)

    def test_compiler_constraint_requires_pinned_experiment_and_artifact(self):
        item = review()
        item["findings"] = [finding("compiler_constraint")]
        with self.assertRaises(sr.ReviewError):
            self.check(item)
        proof = {"tested_commit": HEAD, "command": "python tools/linkcheck.py --name Actor_Render",
                 "result": "Typed dispatch changes the call instruction; relocation comparison fails.",
                 "compiler": "2004/b56", "attempted_change": "Replace the shadow with the existing class.",
                 "log": "notes/probes/actor-render.json", "artifact_commit": WORKFLOW,
                 "artifact_path": "notes/probes/actor-render.json"}
        item["findings"][0]["evidence"] = proof
        self.check(item)
        for key in proof:
            broken = copy.deepcopy(item)
            del broken["findings"][0]["evidence"][key]
            with self.subTest(key=key), self.assertRaises(sr.ReviewError):
                self.check(broken)

    def test_authors_and_findings_survive_rejected_attempts_and_adoption(self):
        task = {"input_session": "original", "producer_sessions": ["inherited"],
                "stages": [{"id": "write", "mode": "write"}],
                "outputs": [{"stage_id": "write", "session": "latest", "evidence": {"source_review": None}}],
                "attempts": [{"outputs": [{"stage_id": "write", "session": "earlier"}],
                              "evidence": {"source_review": {"findings": [finding("open")]}}}]}
        self.assertEqual(sr.producers(task), {"original", "inherited", "latest", "earlier"})
        self.assertIn("actor-virtual-interface", sr.previous_findings(task))

    def test_source_scope_includes_ranges_readonly_review_and_enrollment(self):
        for task in ({"resources": ["range:ov096:1:2"]},
                     {"resources": ["file:notes/review.json"], "stages": [
                         {"requires": ["src/actor.cpp"], "produces": []}]},
                     {"resources": ["file:config/arm9/overlays/ov096/delinks.txt"]}):
            self.assertTrue(sr.source_task(task))
        self.assertFalse(sr.source_task({"resources": ["file:tools/checker.py"]}))

    def test_role_aliases_do_not_skip_final_source_verification(self):
        for stages in ([{"id": "a", "role": "worker", "mode": "write"}],
                       [{"id": "a", "role": "integrator", "mode": "verify"}],
                       [{"id": "a", "role": "producer", "mode": "write"},
                        {"id": "b", "role": "integrator", "mode": "write"}]):
            with self.assertRaises(sr.ReviewError):
                sr.review_graph({"stages": stages})
        sr.review_graph({"stages": [{"id": "a", "role": "humanizer", "mode": "verify"}]})


if __name__ == "__main__":
    unittest.main()
