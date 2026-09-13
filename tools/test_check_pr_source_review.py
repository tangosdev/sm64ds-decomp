"""PR merge-check tests; all API calls are fixtures, with no network or writes."""
import copy
import hashlib
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


def authored_commit(sha, parent="0" * 40):
    """A person's commit on main: it retires review acceptance however small it is."""
    return {"sha": sha, "parents": [{"sha": parent}],
            "author": {"login": "andrewboudreau"}, "committer": {"login": "web-flow"},
            "files": [{"filename": "notes/handoff.md"}]}


def progress_commit(sha, parent, files=("contributions.json", "docs/index.html",
                                        "docs/progress-treemap.svg", "README.md")):
    """The real shape of main's scheduled tip advance; see PR 2564's check history."""
    return {"sha": sha, "parents": [{"sha": parent}],
            "author": {"login": gate.PROGRESS_BOT}, "committer": {"login": gate.PROGRESS_BOT},
            "files": [{"filename": path} for path in files]}


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
        self.assertEqual(gate.evaluate({}, HEAD, (BASE,), ["src/actor.cpp"])["result"], "fail")
        self.assertEqual(gate.evaluate({}, HEAD, (BASE,), ["tools/source_review.py"])["result"], "pass")
        self.assertEqual(gate.evaluate({}, HEAD, (BASE,), ["config/arm9/delinks.txt"])["result"], "fail")

    def test_exact_published_review_passes_and_stale_or_partial_review_fails(self):
        self.assertEqual(gate.evaluate(state(), HEAD, (BASE,), ["src/actor.cpp"])["result"], "pass")
        for head, bases, files in ((WORKFLOW, (BASE,), ["src/actor.cpp"]),
                                   (HEAD, (WORKFLOW,), ["src/actor.cpp"]),
                                   (HEAD, (BASE,), ["src/actor.cpp", "src/unreviewed.cpp"])):
            self.assertEqual(gate.evaluate(state(), head, bases, files)["result"], "fail")

    def test_review_of_any_skipped_progress_commit_is_a_review_of_the_same_source(self):
        """A reviewer may record the branch tip or the source it actually read."""
        for tested in (WORKFLOW, BASE):
            snapshot = state()
            snapshot["tasks"]["actor"]["outputs"][0]["evidence"].update(tested_base=tested)
            snapshot["tasks"]["actor"]["outputs"][0]["evidence"][
                "source_review"]["reviewed_base"] = tested
            self.assertEqual(gate.evaluate(snapshot, HEAD, (WORKFLOW, BASE),
                                           ["src/actor.cpp"])["result"], "pass")
        self.assertEqual(gate.evaluate(state(), HEAD, (WORKFLOW, "e" * 40),
                                       ["src/actor.cpp"])["result"], "fail")

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
            self.assertEqual(gate.evaluate(snapshot, HEAD, (BASE,), ["src/actor.cpp"])["result"], "fail")

    def test_integrator_cannot_invent_an_independent_composition_reviewer(self):
        snapshot = state()
        output = snapshot["tasks"]["actor"]["outputs"][0]
        output["commit"] = BASE
        output["evidence"].update(composition_commit=HEAD, composition_base=BASE,
                                 composition_independent_verification=evidence())
        self.assertEqual(gate.evaluate(snapshot, HEAD, (BASE,), ["src/actor.cpp"])["result"], "fail")

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
            if "/commits/" in path:
                return authored_commit(path.rsplit("/", 1)[1])
            return copy.deepcopy(pr)

        with patch.object(gate, "api", side_effect=api), patch.object(
                gate, "queue_state", return_value=("d" * 40, state())), patch.object(
                gate, "locate_composition", return_value=(BASE, True)), patch.object(
                gate, "changed_paths", return_value=["src/actor.cpp"]):
            result = gate.check_pr("tangosdev/sm64ds-decomp", 2447, publish=True)
        self.assertEqual(result["result"], "fail")
        self.assertEqual(posted[0]["conclusion"], "failure")



class TargetBranchReviewTest(unittest.TestCase):
    def setUp(self):
        gate._refresh_parents.clear()

    def assert_target_resolved_last(self, calls, published=False):
        """Nothing but the progress walk may follow the final branch tip read."""
        ref = max(i for i, path in enumerate(calls) if path.endswith("/git/ref/heads/main"))
        after = [path for path in calls[ref + 1:] if "/commits/" not in path]
        self.assertEqual(after, ["repos/tangosdev/sm64ds-decomp/check-runs"] if published else [])

    def run_fixture(self, refs, review_base=BASE, publish=False, source=True,
                    branch="main", final_branch=None, final_head=None, final_state=None,
                    progress=(), retains_base=True):
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
            if "/commits/" in path:
                sha = path.rsplit("/", 1)[1]
                value = dict(progress).get(sha)
                if isinstance(value, Exception):
                    raise value
                return value if value is not None else authored_commit(sha)
            if "/pulls/" in path:
                reads += 1
                current = copy.deepcopy(pr)
                if reads > 1 and final_branch is not None:
                    current["base"]["ref"] = final_branch
                if reads > 1 and final_head is not None:
                    current["head"]["sha"] = final_head
                if reads > 1 and final_state is not None:
                    current["state"] = final_state
                return current
            raise AssertionError("Unexpected API request: " + path)

        def composition(repo, base, head):
            return base, retains_base

        with patch.object(gate, "api", side_effect=api), patch.object(
                gate, "queue_state", return_value=("d" * 40, snapshot)) as queue, patch.object(
                gate, "locate_composition", side_effect=composition), patch.object(
                gate, "changed_paths", return_value=["src/actor.cpp" if source else "tools/inert.py"]):
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

    def test_scheduled_progress_push_does_not_retire_a_current_review(self):
        """PR 2564: the same head passed, main's timer fired, the same head failed.

        The tip advanced by a bot commit touching only contributions.json. No
        review could read it differently, so it cannot invalidate one.
        """
        tip = "e" * 40
        for review_base in (BASE, tip):
            with self.subTest(review_base=review_base):
                result, _, _, _ = self.run_fixture([tip, tip], review_base=review_base,
                                                   progress={tip: progress_commit(tip, BASE)})
                self.assertEqual(result["result"], "pass")
                self.assertEqual(result["base"], BASE)
                self.assertEqual(result["target_tip"], tip)

    def test_consecutive_progress_pushes_collapse_to_one_reviewable_target(self):
        first, second = "e" * 40, "f" * 40
        result, _, _, _ = self.run_fixture(
            [second, second], review_base=BASE,
            progress={second: progress_commit(second, first),
                      first: progress_commit(first, BASE)})
        self.assertEqual(result["result"], "pass")
        self.assertEqual((result["base"], result["target_tip"]), (BASE, second))

    def test_a_person_s_commit_on_main_still_retires_review_acceptance(self):
        """The written policy: a changed head/base must lose that acceptance."""
        tip = "e" * 40
        result, _, _, _ = self.run_fixture([tip, tip], review_base=BASE)
        self.assertEqual(result["result"], "fail")
        self.assertEqual(result["base"], tip)

    def test_only_source_inert_single_parent_bot_commits_are_skipped(self):
        tip = "e" * 40
        cases = {
            "changes source": progress_commit(tip, BASE, ("contributions.json", "src/actor.cpp")),
            "changes enrollment": progress_commit(tip, BASE, ("config/arm9/symbols.txt",)),
            "renames source away": dict(progress_commit(tip, BASE, ("notes/moved.md",)), files=[
                {"filename": "notes/moved.md", "previous_filename": "src/actor.cpp"}]),
            "human author": dict(progress_commit(tip, BASE), author={"login": "andrewboudreau"}),
            "human committer": dict(progress_commit(tip, BASE), committer={"login": "web-flow"}),
            "merge commit": dict(progress_commit(tip, BASE),
                                 parents=[{"sha": BASE}, {"sha": WORKFLOW}]),
            "capped file list": progress_commit(
                tip, BASE, tuple(f"docs/page-{n}.html" for n in range(gate.COMMIT_FILE_CAP))),
            "different commit": dict(progress_commit(tip, BASE), sha="0" * 40),
            "no file list": {k: v for k, v in progress_commit(tip, BASE).items() if k != "files"},
            "unnamed file": dict(progress_commit(tip, BASE), files=[{"status": "modified"}]),
            "malformed parent": dict(progress_commit(tip, BASE), parents=[{"sha": "short"}]),
            "unreadable commit": RuntimeError("commit unavailable"),
        }
        for name, reply in cases.items():
            with self.subTest(case=name):
                gate._refresh_parents.clear()
                result, _, _, _ = self.run_fixture([tip, tip], review_base=BASE,
                                                   progress={tip: reply})
                self.assertEqual(result["result"], "fail")
                self.assertEqual(result["base"], tip)

    def test_progress_push_during_review_does_not_void_the_verdict(self):
        later, tip = "f" * 40, "e" * 40
        result, _, posts, _ = self.run_fixture(
            [tip, later], review_base=BASE, publish=True,
            progress={tip: progress_commit(tip, BASE), later: progress_commit(later, tip)})
        self.assertEqual(result["result"], "pass")
        self.assertEqual(posts[0]["conclusion"], "success")
        self.assertIn(":" + BASE + ":", posts[0]["external_id"])

    def test_nothing_reviewable_is_decided_before_base_currency(self):
        """A PR with no source in scope has no review to keep current."""
        result, _, _, reads = self.run_fixture([BASE, BASE], source=False, retains_base=False)
        self.assertEqual(result["result"], "pass")
        self.assertEqual(reads, 0)
        stale_source, _, _, _ = self.run_fixture([BASE, BASE], retains_base=False)
        self.assertEqual(stale_source["result"], "fail")
        self.assertIn("restack", stale_source["summary"])

    def test_target_advance_during_read_only_evaluation_invalidates_success(self):
        result, calls, posts, _ = self.run_fixture([BASE, "e" * 40])
        self.assertEqual(result["result"], "fail")
        self.assertEqual(result["observed_base"], "e" * 40)
        self.assertEqual(posts, [])
        self.assert_target_resolved_last(calls)

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

    def test_closed_pr_cannot_publish_a_verdict_for_its_previous_open_state(self):
        result, _, posts, _ = self.run_fixture([BASE], publish=True, final_state="closed")
        self.assertEqual(result["result"], "fail")
        self.assertEqual(posts, [])

    def test_target_advance_before_publication_invalidates_success(self):
        result, calls, posts, _ = self.run_fixture([BASE, "e" * 40], publish=True)
        self.assertEqual(result["result"], "fail")
        self.assertIn("Target branch changed", result["summary"])
        self.assertEqual(posts[0]["conclusion"], "failure")
        self.assert_target_resolved_last(calls, published=True)

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


def object_id(value):
    return hashlib.sha1(value.encode()).hexdigest()


def blob(value, mode="100644", kind="blob"):
    return mode, kind, object_id(value)


class TreeAPI:
    """Git metadata fixtures; tree contents are never fetched or executed."""
    def __init__(self, before, after, base=BASE, head=HEAD, common=None):
        self.base, self.head, self.common = base, head, "9" * 40
        self.objects, self.overrides, self.calls = {}, {}, []
        self.before_tree, self.after_tree = self.build(before), self.build(after)
        self.common_tree = self.build(common) if common is not None else self.before_tree
        self.pr = pull_request()
        self.pr["head"]["sha"] = head
        self.comparison = {"base_commit": {"sha": base},
                           "merge_base_commit": {"sha": base}, "status": "ahead"}
        if common is not None:
            # Main moved on after this branch left it: the merge base is neither end.
            self.comparison.update(merge_base_commit={"sha": self.common}, status="diverged")

    def build(self, leaves):
        root = {}
        for path, entry in leaves.items():
            parts = path.split("/")
            node = root
            for part in parts[:-1]:
                node = node.setdefault(part, {})
            node[parts[-1]] = entry

        def save(node):
            entries = []
            for name, value in sorted(node.items()):
                mode, kind, sha = ("040000", "tree", save(value)) if isinstance(value, dict) else value
                entries.append({"path": name, "mode": mode, "type": kind, "sha": sha})
            sha = object_id(json.dumps(entries, sort_keys=True))
            self.objects[sha] = entries
            return sha
        return save(root)

    def response(self, sha, recursive):
        entries = []
        def visit(current, prefix=""):
            for row in self.objects[current]:
                entry = dict(row, path=prefix + row["path"])
                entries.append(entry)
                if recursive and row["type"] == "tree":
                    visit(row["sha"], entry["path"] + "/")
        visit(sha)
        return {"sha": sha, "tree": entries, "truncated": False}

    def api(self, path, payload=None, paginate=False):
        self.calls.append(path)
        if "/files?" in path:
            raise AssertionError("Cached PR files must not determine exact-base scope")
        if "/compare/" in path:
            return copy.deepcopy(self.comparison)
        if "/git/commits/" in path:
            sha = path.rsplit("/", 1)[1]
            tree = {self.base: self.before_tree, self.head: self.after_tree,
                    self.common: self.common_tree}[sha]
            return copy.deepcopy(self.overrides.get(sha, {"sha": sha, "tree": {"sha": tree}}))
        if "/git/trees/" in path:
            sha = path.rsplit("/", 1)[1].split("?")[0]
            recursive = "?recursive=1" in path
            value = self.overrides.get((sha, recursive), self.response(sha, recursive))
            if isinstance(value, Exception):
                raise value
            return copy.deepcopy(value)
        if path.endswith("/git/ref/heads/agents/coordination"):
            return {"object": {"sha": "d" * 40}}
        if "/pulls/" in path:
            return copy.deepcopy(self.pr)
        raise AssertionError("Unexpected API request: " + path)

    def paths(self):
        with patch.object(gate, "api", side_effect=self.api):
            return gate.changed_paths("tangosdev/sm64ds-decomp", self.base, self.head)

    def scope(self):
        """What the PR changes, measured the way check_pr measures it."""
        with patch.object(gate, "api", side_effect=self.api):
            merge_base, retains = gate.locate_composition(
                "tangosdev/sm64ds-decomp", self.base, self.head)
            return gate.changed_paths("tangosdev/sm64ds-decomp", merge_base, self.head), retains

    def check(self, reviewed=("src/actor.cpp", "include/actor.h")):
        snapshot = state()
        task = snapshot["tasks"]["actor"]
        task["resources"] = ["file:" + path for path in reviewed]
        task["outputs"][0].update(commit=self.head,
                                  evidence=evidence(head=self.head, base=self.base))
        task["outputs"][0]["evidence"]["source_review"]["files"] = list(reviewed)
        with patch.object(gate, "api", side_effect=self.api), patch.object(
                gate, "target_branch", return_value=("main", (self.base,))), patch.object(
                gate, "queue_state", return_value=("d" * 40, snapshot)):
            return gate.check_pr("tangosdev/sm64ds-decomp", 2445)


# Exact --no-renames path inventory of PR 2445, 666df563a..3b71825d7.
PR2445_PATHS = [('M', 'attribution.json'),
 ('M', 'config/arm9/overlays/ov072/delinks.txt'),
 ('M', 'config/arm9/overlays/ov072/symbols.txt'),
 ('M', 'config/converted-baseline.json'),
 ('A', 'config/tu_manifest.d/ov072/daBgSnmBdy_c.json'),
 ('D', 'include/SnowmanBody.h'),
 ('A', 'include/daBgSnmBdy_c.h'),
 ('M', 'include/decl_common.h'),
 ('A', 'notes/agents/handoffs/pr-2445-source-review-fixes.md'),
 ('A', 'notes/agents/handoffs/pr2445-composition-0910.md'),
 ('A', 'notes/agents/handoffs/prod-snmbdy-0907.md'),
 ('M', 'notes/cpp-tu-current-state.md'),
 ('M', 'notes/data/c-cpp-classification.tsv'),
 ('M', 'notes/data/class-build-worklist.tsv'),
 ('M', 'notes/data/tu-merge-candidates.json'),
 ('M', 'notes/data/tu-promotion-queue.tsv'),
 ('M', 'notes/ead-debug-name-crossref.md'),
 ('M', 'notes/handoff-marker-typing.md'),
 ('D', 'src/_ZN11SnowmanBody10HurtPlayerEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody10InitState0Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody10InitState1Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody10InitState2Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody10InitState3Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody10InitState4Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody10InitState5Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody11AdvancePathEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody11UpdateModelEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody13CallStateInitEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody13InitResourcesEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody15UpdateRollAngleEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody16CleanupResourcesEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody16OnPendingDestroyEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody17CallStateBehaviorEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody18IsPlayerNearCenterEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody21UpdateGroundCollisionEP10dBgCh_Actr.cpp'),
 ('D', 'src/_ZN11SnowmanBody6RenderEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody6State0Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody6State1Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody6State2Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody6State3Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody6State4Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody6State5Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBody8BehaviorEv.cpp'),
 ('D', 'src/_ZN11SnowmanBody8SetStateEi.cpp'),
 ('D', 'src/_ZN11SnowmanBodyD0Ev.cpp'),
 ('D', 'src/_ZN11SnowmanBodyD1Ev.cpp'),
 ('A', 'src/actors/daBgSnmBdy_c.cpp'),
 ('D', 'src/d_a_bg_snm_bdy.c'),
 ('M', 'symbols/actor_renames.tsv')]


class ExactTreeScopeTest(unittest.TestCase):
    def test_stale_401_cached_files_do_not_override_49_exact_paths(self):
        before = {path: blob("old " + path) for status, path in PR2445_PATHS if status != "A"}
        after = {path: blob("new " + path) for status, path in PR2445_PATHS if status != "D"}
        fixture = TreeAPI(before, after,
                          base="666df563a2ea5340f41866c3321468b3c2207fd2",
                          head="3b71825d7da2b969c4d962fd82b189090743757f")
        fixture.pr["base"]["sha"] = "690637e41a7a302eace631f6065ab1c4fe37326e"
        fixture.pr["changed_files"] = 401
        fixture.comparison["files"] = [{"filename": f"src/stale-{n}.cpp"} for n in range(300)]
        self.assertEqual(fixture.paths(), sorted(path for _, path in PR2445_PATHS))
        reviewed = [path for _, path in PR2445_PATHS if gate.sr.source_path(path)]
        self.assertEqual(fixture.check(reviewed)["result"], "pass")
        self.assertEqual(fixture.check(reviewed[:-1])["result"], "fail")

    def test_unreviewed_added_changed_and_deleted_sources_fail(self):
        for before, after in (({}, {"src/unreviewed.cpp": blob("new")}),
                              ({"src/unreviewed.cpp": blob("old")}, {}),
                              ({"src/unreviewed.cpp": blob("old")},
                               {"src/unreviewed.cpp": blob("new")})):
            with self.subTest(before=before, after=after):
                result = TreeAPI(before, after).check()
                self.assertEqual(result["result"], "fail")
                self.assertIn("src/unreviewed.cpp", result["missing_files"])

    def test_renames_retain_source_paths_even_when_destination_is_notes(self):
        for destination in ("src/actor.cpp", "notes/actor.txt"):
            with self.subTest(destination=destination):
                fixture = TreeAPI({"src/old_actor.c": blob("body")},
                                  {destination: blob("body")})
                self.assertEqual(set(fixture.paths()), {"src/old_actor.c", destination})
                self.assertEqual(fixture.check()["result"], "fail")

    def test_mode_type_binary_and_directory_replacements_are_changed(self):
        before = {"src/actor.cpp": blob("same"), "assets/image.bin": blob("old"),
                  "src/directory/old.cpp": blob("old"), "vendor/code": blob("one", "160000", "commit")}
        after = {"src/actor.cpp": blob("same", "100755"), "assets/image.bin": blob("new"),
                 "src/directory": blob("link", "120000"), "vendor/code": blob("two", "160000", "commit")}
        self.assertEqual(set(TreeAPI(before, after).paths()), set(before) | set(after))
        self.assertEqual(TreeAPI(before, after).check()["result"], "fail")
        self.assertEqual(TreeAPI({"src/link": blob("same", "120000")},
                                 {"src/link": blob("same")}).paths(), ["src/link"])

    def test_large_diffs_do_not_use_compare_or_pr_file_caps(self):
        for count in (301, 3001):
            with self.subTest(count=count):
                after = {f"notes/item-{n}.md": blob(str(n)) for n in range(count)}
                after["src/unreviewed.cpp"] = blob("last source")
                fixture = TreeAPI({}, after)
                fixture.pr["changed_files"] = 1
                fixture.comparison["files"] = [{"filename": "notes/inert.md"}]
                self.assertEqual(len(fixture.paths()), count + 1)
                self.assertEqual(fixture.check()["result"], "fail")

    def test_truncated_recursive_response_uses_complete_individual_trees(self):
        fixture = TreeAPI({"src/actor.cpp": blob("old"), "shared/stable.h": blob("same")},
                          {"src/actor.cpp": blob("new"), "shared/stable.h": blob("same")})
        for sha in (fixture.before_tree, fixture.after_tree):
            fixture.overrides[sha, True] = {"sha": sha, "tree": [], "truncated": True}
        self.assertEqual(fixture.paths(), ["src/actor.cpp"])
        shared = next(row["sha"] for row in fixture.objects[fixture.before_tree]
                      if row["path"] == "shared")
        self.assertEqual(sum(path.endswith("/git/trees/" + shared) for path in fixture.calls), 1)

    def test_identical_trees_need_no_file_enumeration(self):
        fixture = TreeAPI({"src/actor.cpp": blob("same")}, {"src/actor.cpp": blob("same")})
        self.assertEqual(fixture.paths(), [])
        self.assertFalse(any("/git/trees/" in path for path in fixture.calls))

    def test_truncated_or_missing_nonrecursive_response_fails_closed(self):
        for bad in (None, {}, RuntimeError("missing subtree"),
                    {"tree": [], "truncated": True}):
            with self.subTest(bad=bad):
                fixture = TreeAPI({}, {"src/actor.cpp": blob("new")})
                sha = fixture.after_tree
                fixture.overrides[sha, True] = {"sha": sha, "tree": [], "truncated": True}
                fixture.overrides[sha, False] = dict(bad, sha=sha) if isinstance(bad, dict) else bad
                self.assertEqual(fixture.check()["result"], "fail")

    def test_malformed_tree_responses_cannot_grant_tooling_exemption(self):
        def change_row(data, key, value):
            data["tree"][-1][key] = value
        mutations = [
            lambda data: data.update(sha="f" * 40),
            lambda data: data.pop("truncated"),
            lambda data: data.update(truncated="false"),
            lambda data: data.update(tree=None),
            lambda data: data["tree"].append(dict(data["tree"][-1])),
            lambda data: change_row(data, "sha", "bad"),
            lambda data: change_row(data, "mode", "100000"),
            lambda data: change_row(data, "type", "tree"),
            lambda data: change_row(data, "path", "../notes/file"),
            lambda data: change_row(data, "path", "notes//file"),
            lambda data: change_row(data, "path", "/notes/file"),
            lambda data: change_row(data, "path", "notes\\file"),
            lambda data: data["tree"].pop(0),
        ]
        for mutation in mutations:
            with self.subTest(mutation=mutation):
                fixture = TreeAPI({}, {"notes/file": blob("new")})
                data = fixture.response(fixture.after_tree, True)
                mutation(data)
                fixture.overrides[fixture.after_tree, True] = data
                self.assertEqual(fixture.check()["result"], "fail")

    def test_wrong_commit_or_tree_identity_fails_closed(self):
        for data in ({}, {"sha": HEAD, "tree": {"sha": "bad"}},
                     {"sha": BASE, "tree": {"sha": "e" * 40}}):
            with self.subTest(data=data):
                fixture = TreeAPI({}, {"src/actor.cpp": blob("new")})
                fixture.overrides[HEAD] = data
                self.assertEqual(fixture.check()["result"], "fail")

    def test_nonancestor_source_heads_need_a_new_composition(self):
        for status in ("behind", "diverged"):
            with self.subTest(status=status):
                fixture = TreeAPI({}, {"src/unreviewed.cpp": blob("new")})
                fixture.comparison.update(status=status)
                result = fixture.check()
                self.assertEqual(result["result"], "fail")
                self.assertIn("restack", result["summary"])
        for comparison in ({}, {"base_commit": {"sha": BASE}, "status": "unrelated",
                                "merge_base_commit": {"sha": BASE}}):
            with self.subTest(comparison=comparison):
                fixture = TreeAPI({}, {"tools/inert.py": blob("new")})
                fixture.comparison = comparison
                self.assertEqual(fixture.check()["result"], "fail")

    def test_scope_is_measured_from_the_merge_base_not_the_branch_tip(self):
        """Merging can only introduce what the branch changed since the merge base.

        Main landing src/landed.cpp is not this PR deleting it, and main's tip is
        not this PR's base. The complete tree-derived path set still holds.
        """
        fixture = TreeAPI(before={"src/actor.cpp": blob("old"), "src/landed.cpp": blob("main")},
                          after={"src/actor.cpp": blob("new"), "tools/helper.py": blob("new")},
                          common={"src/actor.cpp": blob("old")})
        paths, retains = fixture.scope()
        self.assertEqual(paths, ["src/actor.cpp", "tools/helper.py"])
        self.assertFalse(retains)
        self.assertIn("src/landed.cpp", fixture.paths())  # The old base...head measure.
        self.assertIn("restack", fixture.check()["summary"])

    def test_missing_subtree_and_tree_cycles_fail_closed(self):
        for cycle in (False, True):
            with self.subTest(cycle=cycle):
                fixture = TreeAPI({}, {"src/actor.cpp": blob("new")})
                sha = fixture.after_tree
                fixture.overrides[sha, True] = {"sha": sha, "tree": [], "truncated": True}
                child = fixture.objects[sha][0]["sha"]
                if cycle:
                    fixture.overrides[sha, False] = {"sha": sha, "truncated": False, "tree": [
                        {"path": "src", "mode": "040000", "type": "tree", "sha": sha}]}
                else:
                    fixture.overrides[child, False] = RuntimeError("subtree unavailable")
                self.assertEqual(fixture.check()["result"], "fail")

    def test_nonrecursive_nested_paths_fail_closed(self):
        fixture = TreeAPI({}, {"src/actor.cpp": blob("new")})
        sha = fixture.after_tree
        fixture.overrides[sha, True] = {"sha": sha, "tree": [], "truncated": True}
        fixture.overrides[sha, False] = {"sha": sha, "truncated": False, "tree": [
            {"path": "src/actor.cpp", "mode": "100644", "type": "blob", "sha": "e" * 40}]}
        self.assertEqual(fixture.check()["result"], "fail")

    def test_changed_source_gitlink_requires_review(self):
        fixture = TreeAPI({"src/foreign": blob("old", "160000", "commit")},
                          {"src/foreign": blob("new", "160000", "commit")})
        self.assertEqual(fixture.paths(), ["src/foreign"])
        self.assertEqual(fixture.check()["result"], "fail")

    def test_transport_failure_does_not_become_an_empty_diff(self):
        fixture = TreeAPI({}, {"tools/inert.py": blob("new")})
        fixture.overrides[fixture.after_tree, True] = RuntimeError("transport failed")
        self.assertEqual(fixture.check()["result"], "fail")



if __name__ == "__main__":
    unittest.main()
