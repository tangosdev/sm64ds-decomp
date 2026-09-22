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
            if "/commits/" in path:
                return reviewable_commit_response(path.rsplit("/", 1)[1])
            return copy.deepcopy(pr)

        with patch.object(gate, "api", side_effect=api), patch.object(
                gate, "queue_state", return_value=("d" * 40, state())), patch.object(
                gate, "changed_paths", return_value=["src/actor.cpp"]):
            result = gate.check_pr("tangosdev/sm64ds-decomp", 2447, publish=True)
        self.assertEqual(result["result"], "fail")
        self.assertEqual(posted[0]["conclusion"], "failure")




def after_final_target_read(calls):
    """Calls made after the last target ref read.

    The tip is read last on purpose. stable_target() then resolves the anchor
    from that tip, and review_anchor() probes whether the head still contains
    it, so the ref read is followed by that walk and that probe and nothing
    else; assert on that shape rather than on a fixed offset.
    """
    last = max(i for i, call in enumerate(calls) if call.endswith("/git/ref/heads/main"))
    return calls[last + 1:]

def reviewable_commit_response(sha, parent="0" * 40, files=("src/actor.cpp",)):
    """A target-branch commit that changed source, so stable_target stops on it."""
    return {"sha": sha, "parents": [{"sha": parent}],
            "files": [{"filename": name} for name in files]}


def inert_commit_response(sha, parent):
    """A bot progress refresh: it moves the tip and no reviewer can act on it."""
    return {"sha": sha, "parents": [{"sha": parent}],
            "files": [{"filename": "contributions.json"},
                      {"filename": "docs/index.html"},
                      {"filename": "docs/progress-treemap.svg"}]}

class TargetBranchReviewTest(unittest.TestCase):
    def run_fixture(self, refs, review_base=BASE, publish=False, source=True,
                    branch="main", final_branch=None, final_head=None, final_state=None,
                    commits=None):
        snapshot = state()
        output = snapshot["tasks"]["actor"]["outputs"][0]["evidence"]
        output["tested_base"] = review_base
        output["source_review"]["reviewed_base"] = review_base
        pr = pull_request(branch)
        commits = commits or {}
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
            if "/compare/" in path:
                # These cases are all about the target ref moving, not about the
                # PR falling behind it, so the head always contains the anchor.
                anchor = path.split("/compare/")[1].split("...")[0]
                return {"base_commit": {"sha": anchor},
                        "merge_base_commit": {"sha": anchor}, "status": "ahead"}
            if "/commits/" in path:
                sha = path.rsplit("/", 1)[1]
                return commits.get(sha, reviewable_commit_response(sha))
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
                if reads > 1 and final_state is not None:
                    current["state"] = final_state
                return current
            raise AssertionError("Unexpected API request: " + path)

        with patch.object(gate, "api", side_effect=api), patch.object(
                gate, "queue_state", return_value=("d" * 40, snapshot)) as queue, patch.object(
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

    def test_target_advance_during_read_only_evaluation_invalidates_success(self):
        result, calls, posts, _ = self.run_fixture([BASE, "e" * 40])
        self.assertEqual(result["result"], "fail")
        self.assertEqual(result["observed_base"], "e" * 40)
        self.assertEqual(posts, [])
        self.assertTrue(all("/commits/" in call or "/compare/" in call
                            for call in after_final_target_read(calls)))

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
        tail = after_final_target_read(calls)
        self.assertTrue(all("/commits/" in call or "/compare/" in call for call in tail[:-1]))
        self.assertTrue(tail[-1].endswith("/check-runs"))

    def test_report_records_whether_the_verdict_reached_a_check_run(self):
        """main() keys its exit code on this, so it has to track publication."""
        tip = "e" * 40
        published, _, posts, _ = self.run_fixture([tip, tip], review_base=tip, publish=True)
        self.assertTrue(published["published"])
        self.assertEqual(len(posts), 1)
        readonly, _, posts, _ = self.run_fixture([tip, tip], review_base=tip)
        self.assertFalse(readonly["published"])
        self.assertEqual(posts, [])
        blocked, _, posts, _ = self.run_fixture([BASE], publish=True, final_head="f" * 40)
        self.assertFalse(blocked["published"])
        self.assertEqual(posts, [])

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
    def __init__(self, before, after, base=BASE, head=HEAD):
        self.base, self.head = base, head
        self.objects, self.overrides, self.calls = {}, {}, []
        self.before_tree, self.after_tree = self.build(before), self.build(after)
        self.pr = pull_request()
        self.pr["head"]["sha"] = head
        self.comparison = {"base_commit": {"sha": base},
                           "merge_base_commit": {"sha": base}, "status": "ahead"}

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
            tree = {self.base: self.before_tree, self.head: self.after_tree}[sha]
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

    def check(self, reviewed=("src/actor.cpp", "include/actor.h")):
        snapshot = state()
        task = snapshot["tasks"]["actor"]
        task["resources"] = ["file:" + path for path in reviewed]
        task["outputs"][0].update(commit=self.head,
                                  evidence=evidence(head=self.head, base=self.base))
        task["outputs"][0]["evidence"]["source_review"]["files"] = list(reviewed)
        with patch.object(gate, "api", side_effect=self.api), patch.object(
                gate, "live_target", return_value=("main", self.base)), patch.object(
                gate, "review_anchor", return_value=(self.base, [self.base], None)), patch.object(
                gate, "queue_state", return_value=("d" * 40, snapshot)):
            return gate.check_pr("tangosdev/sm64ds-decomp", 2445)


# Exact --no-renames path inventory of PR 2445, 315c46d4b..7ed0a7c6b.
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
                          base="315c46d4b180193d9f607f0a62f7d681ca3f76ac",
                          head="7ed0a7c6be6e06a91081a3db75ebd52d2db58753")
        fixture.pr["base"]["sha"] = "6941db21d5708261205537058935c990aafa10e2"
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

    def test_nonancestor_and_unrelated_heads_need_a_new_composition(self):
        for status in ("behind", "diverged", "unrelated"):
            with self.subTest(status=status):
                fixture = TreeAPI({}, {"tools/inert.py": blob("new")})
                fixture.comparison.update(status=status, merge_base_commit={"sha": "e" * 40})
                result = fixture.check()
                self.assertEqual(result["result"], "fail")
                self.assertIn("restack", result["summary"])
        fixture = TreeAPI({}, {"tools/inert.py": blob("new")})
        fixture.comparison = {}
        self.assertEqual(fixture.check()["result"], "fail")

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





class StableTargetTest(unittest.TestCase):
    """The review target must move only when reviewable source moves.

    Anchoring on the raw tip made every published review expire on the progress
    bot's timer: main advances on [skip ci] refreshes of contributions.json and
    docs/, so review evidence went stale with nobody touching the PR. These
    cover both halves -- that inert commits no longer expire a review, and that
    nothing which changes reviewable source is ever walked over.
    """
    SOURCE = "a" * 40
    BOT1 = "b" * 40
    BOT2 = "c" * 40

    def resolve(self, tip, commits):
        calls = []

        def api(path, payload=None, paginate=False, raw=False):
            calls.append(path)
            sha = path.rsplit("/", 1)[1]
            value = commits[sha]
            if isinstance(value, Exception):
                raise value
            return value

        with patch.object(gate, "api", side_effect=api):
            return gate.stable_target("tangosdev/sm64ds-decomp", tip) + (calls,)

    def test_bot_refreshes_are_walked_over_to_the_last_reviewable_commit(self):
        target, window, _ = self.resolve(self.BOT2, {
            self.BOT2: inert_commit_response(self.BOT2, self.BOT1),
            self.BOT1: inert_commit_response(self.BOT1, self.SOURCE),
            self.SOURCE: reviewable_commit_response(self.SOURCE)})
        self.assertEqual(target, self.SOURCE)
        self.assertEqual(window, [self.BOT2, self.BOT1, self.SOURCE])

    def reviewed_at(self, base):
        snapshot = state()
        review = snapshot["tasks"]["actor"]["outputs"][0]["evidence"]
        review["tested_base"] = base
        review["source_review"]["reviewed_base"] = base
        return snapshot

    def test_every_skipped_tip_is_accepted_as_a_tested_base(self):
        # A reviewer names whichever tip was live when they measured. All three
        # carry the same reviewable tree, so all three must be interchangeable
        # -- otherwise the reviewer has to guess which one the check will pick.
        window = [self.BOT2, self.BOT1, self.SOURCE]
        for tested in window:
            with self.subTest(tested_base=tested):
                report = gate.evaluate(self.reviewed_at(tested), HEAD, self.SOURCE,
                                       ["src/actor.cpp"], window)
                self.assertEqual(report["result"], "pass")

    def test_a_base_outside_the_window_is_still_rejected(self):
        report = gate.evaluate(self.reviewed_at("9" * 40), HEAD, self.SOURCE,
                               ["src/actor.cpp"], [self.BOT1, self.SOURCE])
        self.assertEqual(report["result"], "fail")
        self.assertIn("actor: PR base changed since source review", report["errors"])

    def test_the_window_does_not_relax_the_single_base_default(self):
        # evaluate() without a window must still accept exactly one base, so
        # every other caller and test keeps the pre-existing contract.
        self.assertEqual(gate.evaluate(self.reviewed_at(self.BOT1), HEAD, self.SOURCE,
                                       ["src/actor.cpp"])["result"], "fail")
        self.assertEqual(gate.evaluate(self.reviewed_at(self.SOURCE), HEAD, self.SOURCE,
                                       ["src/actor.cpp"])["result"], "pass")

    def test_a_commit_touching_reviewable_source_stops_the_walk(self):
        for path in ("src/actor.cpp", "include/actor.h", "config/tu_manifest.d/ov023/x.json",
                     "config/arm9/overlays/ov070/delinks.txt", "src_tu/x.cpp", "mods/x.cpp"):
            with self.subTest(path=path):
                target, window, _ = self.resolve(self.BOT1, {
                    self.BOT1: {"sha": self.BOT1, "parents": [{"sha": self.SOURCE}],
                                "files": [{"filename": "contributions.json"},
                                          {"filename": path}]}})
                self.assertEqual((target, window), (self.BOT1, [self.BOT1]))

    def test_a_rename_out_of_source_still_stops_the_walk(self):
        target, _, _ = self.resolve(self.BOT1, {
            self.BOT1: {"sha": self.BOT1, "parents": [{"sha": self.SOURCE}],
                        "files": [{"filename": "notes/moved.md",
                                   "previous_filename": "src/actor.cpp"}]}})
        self.assertEqual(target, self.BOT1)

    def test_uncertain_commits_fail_closed_onto_the_live_tip(self):
        cases = {
            "unreadable": RuntimeError("commit unavailable"),
            "merge": {"sha": self.BOT1, "files": [],
                      "parents": [{"sha": self.SOURCE}, {"sha": "8" * 40}]},
            "identity mismatch": {"sha": "7" * 40, "parents": [{"sha": self.SOURCE}], "files": []},
            "files at the API cap": {
                "sha": self.BOT1, "parents": [{"sha": self.SOURCE}],
                "files": [{"filename": f"docs/p{n}.html"} for n in range(gate.COMMIT_FILES_CAP)]},
            "missing files": {"sha": self.BOT1, "parents": [{"sha": self.SOURCE}]},
            "malformed entry": {"sha": self.BOT1, "parents": [{"sha": self.SOURCE}],
                                "files": [{"filename": None}]},
            "unusable parent": {"sha": self.BOT1, "parents": [{"sha": "short"}], "files": []},
        }
        for label, response in cases.items():
            with self.subTest(case=label):
                target, window, _ = self.resolve(self.BOT1, {self.BOT1: response})
                self.assertEqual((target, window), (self.BOT1, [self.BOT1]))

    def test_a_long_run_of_refreshes_stops_at_the_walk_limit(self):
        shas = ["%040x" % n for n in range(gate.TARGET_WALK_LIMIT + 5)]
        commits = {sha: inert_commit_response(sha, shas[n + 1])
                   for n, sha in enumerate(shas[:-1])}
        commits[shas[-1]] = reviewable_commit_response(shas[-1])
        target, window, calls = self.resolve(shas[0], commits)
        self.assertEqual(target, shas[gate.TARGET_WALK_LIMIT])
        self.assertEqual(len(window), gate.TARGET_WALK_LIMIT + 1)
        self.assertEqual(len(calls), gate.TARGET_WALK_LIMIT)

    def test_check_pr_reports_the_anchor_and_the_live_tip_separately(self):
        result, _, _, _ = TargetBranchReviewTest().run_fixture(
            [self.BOT1, self.BOT1], review_base=self.BOT1,
            commits={self.BOT1: inert_commit_response(self.BOT1, self.SOURCE),
                     self.SOURCE: reviewable_commit_response(self.SOURCE)})
        self.assertEqual(result["result"], "pass")
        self.assertEqual(result["base"], self.SOURCE)
        self.assertEqual(result["live_tip"], self.BOT1)
        self.assertEqual(result["base_window"], [self.BOT1, self.SOURCE])


class ScopedTargetTest(unittest.TestCase):
    """A source merge must expire only the reviews it actually touches.

    stable_target() already walked over the progress bot. It could not walk
    over a real merge, so every merge to main expired every other open PR's
    review at once: measured 2026-09-21, merging #2884 turned 9 of 13 open PRs
    red, and none of the 9 shared a file with it. These cover the relaxation
    and, more importantly, everything it must still refuse.
    """
    TIP = "e" * 40
    MID = "f" * 40
    MB = "1" * 40

    def anchor(self, commits, pr_paths, scope=None, tip=None):
        calls, cache = [], {}

        def api(path, payload=None, paginate=False, raw=False):
            calls.append(path)
            if "/compare/" in path:
                base = path.split("/compare/")[1].split("...")[0]
                return {"base_commit": {"sha": base},
                        "merge_base_commit": {"sha": self.MB},
                        "status": "identical" if base == self.MB else "diverged"}
            if "/commits/" in path:
                value = commits[path.rsplit("/", 1)[1]]
                if isinstance(value, Exception):
                    raise value
                return value
            raise AssertionError("Unexpected API request: " + path)

        with patch.object(gate, "api", side_effect=api), patch.object(
                gate, "changed_paths", return_value=pr_paths):
            result = gate.review_anchor("tangosdev/sm64ds-decomp", tip or self.TIP,
                                        HEAD, scope, cache)
        return result, calls, cache

    def merge(self, sha, parent, files):
        return {"sha": sha, "parents": [{"sha": parent}],
                "files": [{"filename": name} for name in files]}

    def test_a_merge_that_misses_this_pr_no_longer_expires_its_review(self):
        # main landed real source work; this PR touches none of those files.
        (base, window, scope), _, _ = self.anchor(
            {self.TIP: self.merge(self.TIP, self.MB, ["src/other.cpp", "include/other.h"])},
            ["src/actor.cpp", "config/arm9/overlays/ov072/delinks.txt"])
        self.assertEqual(base, self.MB)
        self.assertEqual(window, [self.TIP, self.MB])
        self.assertEqual(scope, frozenset({"src/actor.cpp",
                                           "config/arm9/overlays/ov072/delinks.txt"}))

    def test_every_commit_walked_over_is_accepted_as_a_tested_base(self):
        # Same contract the unscoped window has: a reviewer names whichever tip
        # was live when they measured, and must not have to guess which one.
        commits = {self.TIP: self.merge(self.TIP, self.MID, ["src/other.cpp"]),
                   self.MID: self.merge(self.MID, self.MB, ["src/third.cpp"])}
        (base, window, _), _, _ = self.anchor(commits, ["src/actor.cpp"])
        self.assertEqual(base, self.MB)
        self.assertEqual(window, [self.TIP, self.MID, self.MB])

    def test_a_merge_touching_one_of_this_prs_files_still_demands_a_restack(self):
        for overlap in ("src/actor.cpp", "config/arm9/overlays/ov072/delinks.txt"):
            with self.subTest(path=overlap):
                with self.assertRaises(gate.sr.ReviewError) as caught:
                    self.anchor({self.TIP: self.merge(self.TIP, self.MB,
                                                      ["src/other.cpp", overlap])},
                                ["src/actor.cpp", "config/arm9/overlays/ov072/delinks.txt"])
                self.assertIn("restack", str(caught.exception))

    def test_overlap_is_matched_case_insensitively_like_the_rest_of_the_check(self):
        with self.assertRaises(gate.sr.ReviewError):
            self.anchor({self.TIP: self.merge(self.TIP, self.MB, ["src/Actor.cpp"])},
                        ["src/actor.cpp"])

    def test_a_rename_on_main_counts_against_both_of_its_paths(self):
        # Source leaving src/ still moves the file this review names.
        for entry in ({"filename": "notes/moved.md", "previous_filename": "src/actor.cpp"},
                      {"filename": "src/actor.cpp", "previous_filename": "notes/moved.md"}):
            with self.subTest(rename=entry["filename"]):
                with self.assertRaises(gate.sr.ReviewError):
                    self.anchor({self.TIP: {"sha": self.TIP, "parents": [{"sha": self.MB}],
                                            "files": [entry]}}, ["src/actor.cpp"])

    def test_an_unreadable_commit_still_fails_closed_under_a_scope(self):
        cases = {
            "unreadable": RuntimeError("commit unavailable"),
            "merge": {"sha": self.TIP, "files": [],
                      "parents": [{"sha": self.MB}, {"sha": "8" * 40}]},
            "identity mismatch": {"sha": "7" * 40, "parents": [{"sha": self.MB}], "files": []},
            "files at the API cap": {
                "sha": self.TIP, "parents": [{"sha": self.MB}],
                "files": [{"filename": f"docs/p{n}.html"} for n in range(gate.COMMIT_FILES_CAP)]},
            "missing files": {"sha": self.TIP, "parents": [{"sha": self.MB}]},
            "malformed entry": {"sha": self.TIP, "parents": [{"sha": self.MB}],
                                "files": [{"filename": None}]},
            "unusable parent": {"sha": self.TIP, "parents": [{"sha": "short"}], "files": []},
        }
        for label, response in cases.items():
            with self.subTest(case=label):
                with self.assertRaises(gate.sr.ReviewError):
                    self.anchor({self.TIP: response}, ["src/actor.cpp"])

    def test_a_pr_with_no_reviewable_source_does_not_walk_main_at_all(self):
        # Nothing here needs review, so no commit on main can expire it and
        # there is nothing to compare against: take the PR's own base and stop.
        (base, window, scope), calls, _ = self.anchor(
            {self.TIP: self.merge(self.TIP, self.MB, ["src/other.cpp"])},
            ["docs/class-reference.html", "tools/fdiff.py"])
        self.assertEqual((base, window, scope), (self.MB, [self.TIP, self.MB], frozenset()))
        # Only the unscoped probe of the tip itself, never a walk down main.
        self.assertEqual([call for call in calls if "/commits/" in call],
                         ["repos/tangosdev/sm64ds-decomp/commits/" + self.TIP])

    def test_a_walk_that_never_reaches_the_prs_base_fails_closed(self):
        shas = ["%040x" % n for n in range(gate.SCOPED_WALK_LIMIT + 5)]
        commits = {sha: self.merge(sha, shas[n + 1], ["src/other.cpp"])
                   for n, sha in enumerate(shas[:-1])}
        with self.assertRaises(gate.sr.ReviewError):
            self.anchor(commits, ["src/actor.cpp"], tip=shas[0])

    def test_mains_history_is_read_once_per_run_not_once_per_pr(self):
        # The scoped walk reaches much further back than the bot cadence did,
        # so this cache is what keeps a 5-minute refresh inside the rate limit.
        commits = {self.TIP: self.merge(self.TIP, self.MID, ["src/other.cpp"]),
                   self.MID: self.merge(self.MID, self.MB, ["src/third.cpp"])}
        _, calls, cache = self.anchor(commits, ["src/actor.cpp"])
        self.assertEqual(len([c for c in calls if "/commits/" in c]), 2)
        self.assertEqual(set(cache), {self.TIP, self.MID})

    def test_an_unrelated_header_on_main_is_walked_over_by_design(self):
        # A documented gap, not an oversight: main edits a header this PR's
        # sources may include, the paths do not intersect, and the walk steps
        # over it. Compile and link drift is the ROM build gate's measurement,
        # taken against the actual merge; this check judges whether source is
        # admissible. Scoping by include graph is unavailable here -- this
        # workflow is privileged and must never check out or run PR code.
        (base, _, _), _, _ = self.anchor(
            {self.TIP: self.merge(self.TIP, self.MB, ["include/decl_common.h"])},
            ["src/actor.cpp"])
        self.assertEqual(base, self.MB)


class ExitCodeTest(unittest.TestCase):
    """A published verdict is reported once, by its own check run.

    The job conclusion used to restate it, so every failing PR carried two red
    marks for one finding -- measured 2026-09-17, 17 open PRs and 34 red checks
    for 17 distinct verdicts. Only a failure with no check run to speak for it,
    a read-only run or a publication that could not be created, still exits
    nonzero.
    """

    def run_main(self, reports, argv, numbers=None):
        replies = iter(reports)
        pages = [[{"number": n} for n in (numbers or [2447])]]
        with patch.object(gate, "api", return_value=pages), patch.object(
                gate, "check_pr", side_effect=lambda *a, **k: next(replies)):
            return gate.main(argv)

    def test_published_failure_does_not_also_fail_the_job(self):
        self.assertEqual(
            self.run_main([{"result": "fail", "published": True}], ["--publish"]), 0)

    def test_unpublished_failure_still_exits_nonzero(self):
        for report in ({"result": "fail", "published": False}, {"result": "fail"}):
            for argv in (["--publish"], []):
                with self.subTest(report=report, argv=argv):
                    self.assertEqual(self.run_main([dict(report)], argv), 1)

    def test_passing_verdict_exits_zero_either_way(self):
        self.assertEqual(
            self.run_main([{"result": "pass", "published": True}], ["--publish"]), 0)
        self.assertEqual(self.run_main([{"result": "pass"}], []), 0)

    def test_one_unpublished_failure_among_published_ones_still_exits_nonzero(self):
        reports = [{"result": "fail", "published": True},
                   {"result": "fail", "published": False}]
        self.assertEqual(self.run_main(reports, ["--publish"], numbers=[2447, 2448]), 1)


if __name__ == "__main__":
    unittest.main()
