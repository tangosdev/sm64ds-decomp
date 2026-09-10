"""Exercise the workflow's real metadata/submission helper without network or secrets."""
import copy
import io
import json
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import Mock, patch
from urllib.error import HTTPError, URLError
from urllib.request import Request

sys.path.insert(0, str(Path(__file__).resolve().parent))
import submit_pr_validation as submitter


REPO = "tangosdev/sm64ds-decomp"
HEAD = "3b71825d7da2b969c4d962fd82b189090743757f"
OLD_BASE = "690637e41a7a302eace631f6065ab1c4fe37326e"
LIVE_BASE = "666df563a2ea5340f41866c3321468b3c2207fd2"
NEXT_BASE = "c" * 40
JOB = "b6ddf043b0f54687bea3a705ddff3df0"


def pull_request():
    return {"number": 2445, "state": "open", "merged": False, "changed_files": 1,
            "head": {"sha": HEAD, "ref": "review/snm", "repo": {
                "full_name": "contributor/sm64ds-decomp", "id": 20}},
            "base": {"sha": LIVE_BASE, "ref": "main", "repo": {
                "full_name": REPO, "id": 10}}}


def event(data=None):
    snapshot = copy.deepcopy(data or pull_request())
    snapshot["base"]["sha"] = OLD_BASE
    return {"number": 2445, "pull_request": snapshot}


def git_ref(sha=LIVE_BASE, branch="main"):
    return {"ref": f"refs/heads/{branch}", "object": {"type": "commit", "sha": sha}}


class Services:
    """Fixtures expose request ordering and record every attempted relay mutation."""

    def __init__(self):
        self.pr = pull_request()
        self.ref = git_ref()
        self.files = [{"filename": "src/actor.cpp", "status": "modified"}]
        self.requests = []
        self.relay_requests = []
        self.after_read = None
        self.reply = {"ok": True, "jobId": JOB, "status": "Queued"}
        self.job_override = {}

    def github(self, path):
        self.requests.append(path)
        if path == f"repos/{REPO}/pulls/2445":
            value = self.pr
        elif path.startswith(f"repos/{REPO}/git/ref/heads/"):
            value = self.ref
        elif path.startswith(f"repos/{REPO}/pulls/2445/files?per_page=100&page="):
            page = int(path.rsplit("=", 1)[1])
            value = self.files[(page - 1) * 100:page * 100]
        else:
            raise AssertionError(f"Unexpected metadata request: {path}")
        result = copy.deepcopy(value)
        if self.after_read:
            self.after_read(path)
        return result

    def relay(self, method, path, payload):
        self.relay_requests.append((method, path, copy.deepcopy(payload)))
        if method == "POST" and path == "/api/pr-validate":
            self.payload = payload
            return copy.deepcopy(self.reply)
        if method == "GET" and path == f"/api/pr-validate/{JOB}":
            return {"id": JOB, **self.payload, **self.job_override}
        raise AssertionError(f"Unexpected relay request: {method} {path}")

    def resolve(self):
        return submitter.resolve(self.github, REPO, "pull_request_target", event())

    def run(self, pin=None):
        return submitter.submit(self.github, self.relay, pin or self.resolve())


class SubmissionTests(unittest.TestCase):
    def test_stale_event_and_api_base_use_live_branch_for_actual_post(self):
        # Regression: run 34531793337 sent OLD_BASE for this exact PR head.
        services = Services()
        services.pr["base"]["sha"] = OLD_BASE
        result = services.run()
        expected = {"repo": REPO, "pr": 2445, "sha": HEAD,
                    "baseSha": LIVE_BASE, "validationRelevant": True}
        self.assertEqual(services.relay_requests[0], ("POST", "/api/pr-validate", expected))
        self.assertEqual(result, {"jobId": JOB, **expected})
        self.assertEqual(services.relay_requests[1], ("GET", f"/api/pr-validate/{JOB}", None))
        self.assertEqual(services.requests[-3:], [f"repos/{REPO}/git/ref/heads/main",
                                                f"repos/{REPO}/pulls/2445",
                                                f"repos/{REPO}/git/ref/heads/main"])

    def test_manual_dispatch_resolves_current_fork_head_without_base_input(self):
        services = Services()
        services.pr["head"]["sha"] = NEXT_BASE
        pin = submitter.resolve(services.github, REPO, "workflow_dispatch", {"inputs": {"pr": "2445"}})
        result = services.run(pin)
        self.assertEqual(result["sha"], NEXT_BASE)
        self.assertEqual(result["baseSha"], LIVE_BASE)
        self.assertEqual(pin["headRepo"], "contributor/sm64ds-decomp")

    def test_terminal_same_head_can_be_submitted_again(self):
        services = Services()
        first = services.run()
        second = services.run()
        self.assertEqual(first["sha"], second["sha"])
        self.assertEqual(sum(method == "POST" for method, _, _ in services.relay_requests), 2)
        # There is no local "this head already passed/failed" shortcut. The relay
        # decides whether an in-flight job can be reused or a new one is needed.

    def test_inert_pr_still_submits_with_advisory_false(self):
        services = Services()
        services.files = [{"filename": "notes/review.md", "status": "modified"}]
        self.assertIs(services.run()["validationRelevant"], False)
        self.assertEqual(services.relay_requests[0][0], "POST")

    def test_inert_cached_file_list_cannot_skip_live_base_validation(self):
        services = Services()
        services.pr["base"]["sha"] = OLD_BASE
        services.files = [{"filename": "notes/review.md", "status": "modified"}]
        self.assertIs(services.run()["validationRelevant"], True)
        self.assertFalse(any("/files?" in path for path in services.requests))

    def test_changed_event_head_or_target_ref_is_rejected(self):
        for key, value in (("sha", NEXT_BASE), ("ref", "another-branch")):
            with self.subTest(key=key):
                services = Services()
                side = "head" if key == "sha" else "base"
                services.pr[side][key] = value
                with self.assertRaises(submitter.SubmissionError):
                    services.run()
                self.assertEqual(services.relay_requests, [])

    def test_head_target_fork_or_closed_race_after_resolution_blocks_post(self):
        mutations = [lambda pr: pr["head"].update(sha=NEXT_BASE),
                     lambda pr: pr["base"].update(ref="release"),
                     lambda pr: pr["base"]["repo"].update(full_name="other/repo"),
                     lambda pr: pr["base"]["repo"].update(id=11),
                     lambda pr: pr["head"]["repo"].update(id=21),
                     lambda pr: pr["head"]["repo"].update(full_name="different/fork"),
                     lambda pr: pr["head"].update(ref="replacement"),
                     lambda pr: pr["head"].update(repo=None),
                     lambda pr: pr.update(state="closed"),
                     lambda pr: pr.update(merged=True)]
        for mutation in mutations:
            for during_files in (False, True):
                with self.subTest(mutation=mutation, during_files=during_files):
                    services = Services()
                    pin = services.resolve()
                    if during_files:
                        services.after_read = lambda path: mutation(services.pr) if "/files?" in path else None
                    else:
                        mutation(services.pr)
                    with self.assertRaises(submitter.SubmissionError):
                        services.run(pin)
                    self.assertEqual(services.relay_requests, [])

    def test_base_races_before_and_during_final_pr_read_block_post(self):
        for during_final_read in (False, True):
            services = Services()
            pin = services.resolve()
            if during_final_read:
                def advance(path):
                    if path.endswith("/pulls/2445"):
                        pr_reads = services.requests.count(path)
                        if pr_reads == 3:
                            services.ref = git_ref(NEXT_BASE)
                services.after_read = advance
            else:
                services.ref = git_ref(NEXT_BASE)
            with self.assertRaisesRegex(submitter.SubmissionError, "Target branch advanced"):
                services.run(pin)
            self.assertEqual(services.relay_requests, [])

    def test_cached_base_sha_changing_to_live_sha_is_not_a_target_race(self):
        services = Services()
        services.pr["base"]["sha"] = OLD_BASE
        pin = services.resolve()
        services.pr["base"]["sha"] = LIVE_BASE
        self.assertEqual(services.run(pin)["baseSha"], LIVE_BASE)

    def test_branch_with_slash_is_encoded_as_one_ref_parameter(self):
        services = Services()
        services.pr["base"]["ref"] = "release/v1"
        services.ref = git_ref(branch="release/v1")
        pin = submitter.resolve(services.github, REPO, "workflow_dispatch", {"inputs": {"pr": "2445"}})
        services.run(pin)
        self.assertIn(f"repos/{REPO}/git/ref/heads/release%2Fv1", services.requests)

    def test_invalid_dispatch_inputs_do_not_read_metadata_or_submit(self):
        inputs = [{}, {"pr": "2445", "baseSha": OLD_BASE}, {"pr": "2445", "sha": HEAD}]
        inputs.extend({"pr": value} for value in (None, True, 2445, "", "0", "-1", "+1",
                                                 " 2445", "2445\n", "2.5", "1e3", "01", "1/../2"))
        for value in inputs:
            with self.subTest(value=value):
                services = Services()
                with self.assertRaises(submitter.SubmissionError):
                    submitter.resolve(services.github, REPO, "workflow_dispatch", {"inputs": value})
                self.assertEqual(services.requests, [])

    def test_malformed_pr_or_target_ref_never_submits(self):
        malformed = [None, [], {}, {**pull_request(), "number": True},
                     {**pull_request(), "number": 2446}, {**pull_request(), "merged": None},
                     {**pull_request(), "head": None}, {**pull_request(), "base": None}]
        for side in ("head", "base"):
            for value in (None, [], "", "a" * 39, "g" * 40, HEAD + "\n"):
                data = pull_request()
                data[side]["sha"] = value
                malformed.append(data)
        for value in malformed:
            services = Services()
            services.pr = value
            with self.subTest(value=value), self.assertRaises(submitter.SubmissionError):
                services.run()
            self.assertEqual(services.relay_requests, [])
        for value in (None, [], {}, git_ref(branch="wrong"),
                      {"ref": "refs/heads/main", "object": {"type": "tree", "sha": LIVE_BASE}},
                      git_ref("short")):
            services = Services()
            services.ref = value
            with self.subTest(value=value), self.assertRaises(submitter.SubmissionError):
                services.run()
            self.assertEqual(services.relay_requests, [])

    def test_invalid_pins_fail_before_any_network_request(self):
        pin = Services().resolve()
        values = [None, {}, {**pin, "extra": True}]
        for key, value in (("pr", True), ("sha", "short"), ("baseSha", "g" * 40),
                           ("baseRef", "main\nhead=bad"), ("repo", "../bad"),
                           ("headRepoId", 0), ("headRef", "bad..branch")):
            values.append({**pin, key: value})
        for value in values:
            services = Services()
            with self.subTest(value=value), self.assertRaises(submitter.SubmissionError):
                submitter.submit(services.github, services.relay, value)
            self.assertEqual(services.requests + services.relay_requests, [])

    def test_existing_inflight_job_must_have_identical_pins(self):
        for key, value in (("baseSha", OLD_BASE), ("sha", NEXT_BASE), ("pr", 2446),
                           ("pr", True), ("repo", "other/repo"), ("id", "bad")):
            services = Services()
            services.reply["status"] = "Running"
            services.job_override[key] = value
            with self.subTest(key=key), self.assertRaises(submitter.SubmissionError):
                services.run()
            self.assertEqual([method for method, _, _ in services.relay_requests], ["POST", "GET"])

    def test_malformed_relay_acknowledgements_fail(self):
        for reply in (None, [], {}, {"ok": False, "jobId": JOB, "status": "Queued"},
                      {"ok": 1, "jobId": JOB, "status": "Queued"},
                      {"ok": True, "jobId": "../state", "status": "Queued"},
                      {"ok": True, "jobId": JOB, "status": "unknown"},
                      {"ok": True, "jobId": JOB, "status": []}):
            services = Services()
            services.reply = reply
            with self.subTest(reply=reply), self.assertRaises(submitter.SubmissionError):
                services.run()
            self.assertEqual(len(services.relay_requests), 1)

    def test_closed_pr_and_unknown_event_fail_before_submission(self):
        for change in ({"state": "closed"}, {"merged": True}):
            services = Services()
            services.pr.update(change)
            with self.assertRaises(submitter.SubmissionError):
                services.run()
            self.assertEqual(services.relay_requests, [])
        services = Services()
        with self.assertRaises(submitter.SubmissionError):
            submitter.resolve(services.github, REPO, "pull_request", event())
        self.assertEqual(services.requests, [])

    def test_malformed_job_readback_cannot_confirm_submission(self):
        for job in (None, [], {}, {"id": JOB}):
            services = Services()
            relay = Mock(side_effect=[services.reply, job])
            with self.assertRaises(submitter.SubmissionError):
                submitter.submit(services.github, relay, services.resolve())
            self.assertEqual(relay.call_count, 2)


class ClassificationTests(unittest.TestCase):
    def classify(self, paths, count=None):
        services = Services()
        services.files = paths
        services.pr["changed_files"] = len(paths) if count is None else count
        return services.run()["validationRelevant"]

    def test_existing_inert_set_and_source_tool_credit_paths(self):
        for path in ("docs/index.html", "notes/review.json", "src_tu/actor.cpp",
                     ".github/ISSUE_TEMPLATE/bug.yml", "README.md", "image.svg", "LICENSE"):
            with self.subTest(path=path):
                self.assertFalse(self.classify([{"filename": path, "status": "modified"}]))
        for path in ("src/a.cpp", "include/A.h", "config/arm9/symbols.txt", "tools/a.py",
                     "attribution.json", "match_provenance.jsonl", ".gitattributes",
                     ".github/workflows/pr-validate.yml", "unknown/file"):
            with self.subTest(path=path):
                self.assertTrue(self.classify([{"filename": path, "status": "modified"}]))

    def test_renamed_source_to_inert_path_remains_relevant(self):
        self.assertTrue(self.classify([{"filename": "notes/retired.md", "status": "renamed",
                                        "previous_filename": "src/actor.cpp"}]))
        self.assertTrue(self.classify([{"filename": "notes/renamed.md", "status": "renamed"}]))
        self.assertFalse(self.classify([{"filename": "notes/new.md", "status": "renamed",
                                         "previous_filename": "notes/old.md"}]))

    def test_all_pages_are_classified(self):
        files = [{"filename": f"notes/{i}.md", "status": "modified"} for i in range(101)]
        self.assertFalse(self.classify(files))
        files[-1]["filename"] = "src/actor.cpp"
        self.assertTrue(self.classify(files))

    def test_empty_incomplete_capped_and_duplicate_lists_default_relevant(self):
        entry = {"filename": "notes/readme.md", "status": "modified"}
        self.assertTrue(self.classify([]))
        self.assertTrue(self.classify([entry], 2))
        self.assertTrue(self.classify([entry, entry]))
        for count in (3001, -1, True, "1"):
            self.assertTrue(self.classify([entry], count))

    def test_malformed_paths_cannot_earn_inert_status(self):
        for value in (None, "", 1, [], "notes/../src/a.cpp", "notes\\readme.md",
                      "notes/readme.md\n", "/notes/readme.md", "notes//readme.md"):
            self.assertTrue(self.classify([{"filename": value, "status": "modified"}]))
        for entry in ({"filename": "notes/a.md"}, {"filename": "notes/a.md", "status": "unexpected"},
                      {"filename": "notes/a.md", "status": []},
                      None, [], {"filename": "notes/a.md", "status": "renamed", "previous_filename": []}):
            self.assertTrue(self.classify([entry]))

    def test_files_api_failure_does_not_submit_an_inert_verdict(self):
        services = Services()
        pin = services.resolve()
        github = Mock(side_effect=[services.pr, submitter.SubmissionError("API unavailable")])
        with self.assertRaises(submitter.SubmissionError):
            submitter.submit(github, services.relay, pin)
        self.assertEqual(services.relay_requests, [])

    def test_malformed_file_page_is_relevant(self):
        for page in ({"message": "not a list"}, None, [{}] * 101):
            github = Mock(return_value=page)
            self.assertTrue(submitter.validation_relevant(github, REPO, 2445, 1))


class TransportAndWorkflowTests(unittest.TestCase):
    def test_successful_json_transport_uses_bounded_nonredirecting_request(self):
        with patch.object(submitter, "build_opener") as opener:
            response = opener.return_value.open.return_value.__enter__.return_value
            response.status = 200
            response.read.return_value = b'{"ok":true}'
            self.assertEqual(submitter.request_json(Request("https://example.invalid")), {"ok": True})
            self.assertIsInstance(opener.call_args.args[0], submitter.NoRedirect)
            self.assertEqual(opener.return_value.open.call_args.kwargs, {"timeout": 30})
            response.read.assert_called_once_with(2_000_001)

    def test_github_client_uses_metadata_get_and_read_token(self):
        with patch.object(submitter, "request_json", return_value={}) as request:
            submitter.github_client("fixture-token")(f"repos/{REPO}/pulls/2445")
            sent = request.call_args.args[0]
            self.assertEqual(sent.get_method(), "GET")
            self.assertEqual(sent.full_url, f"https://api.github.com/repos/{REPO}/pulls/2445")
            self.assertEqual(sent.get_header("Authorization"), "Bearer fixture-token")

    def test_json_rejects_malformed_duplicate_and_nonfinite_values(self):
        for text in ("", "not json", '{"ok":true,"ok":false}', '{"sha":NaN}', b"\xff"):
            with self.subTest(text=text), self.assertRaises(submitter.SubmissionError):
                submitter.load_json(text)

    def test_http_failures_do_not_expose_response_or_url_secrets(self):
        request = Request("https://private.example.invalid/secret-url")
        for error in (HTTPError(request.full_url, 403, "secret-token", {}, io.BytesIO(b"secret-body")),
                      URLError("secret-token"), TimeoutError("secret-token")):
            with patch.object(submitter, "build_opener") as opener:
                opener.return_value.open.side_effect = error
                with self.assertRaises(submitter.SubmissionError) as caught:
                    submitter.request_json(request)
                self.assertNotIn("secret", str(caught.exception))
        self.assertIsNone(submitter.NoRedirect().redirect_request(request, None, 302, "", {},
                                                                  "https://other.invalid"))

    def test_json_transport_rejects_non_success_and_oversized_body(self):
        for status, body in ((500, b'{}'), (200, b'x' * 2_000_001), (200, b'not json')):
            with patch.object(submitter, "build_opener") as opener:
                response = opener.return_value.open.return_value.__enter__.return_value
                response.status = status
                response.read.return_value = body
                with self.assertRaises(submitter.SubmissionError):
                    submitter.request_json(Request("https://example.invalid"))

    def test_relay_transport_sends_pins_as_json_with_token_only_in_header(self):
        with patch.object(submitter, "request_json", return_value={}) as request:
            relay = submitter.relay_client("https://example.invalid/", "fixture-token")
            relay("POST", "/api/pr-validate", {"sha": HEAD, "baseSha": LIVE_BASE})
            sent = request.call_args.args[0]
            self.assertEqual(sent.full_url, "https://example.invalid/api/pr-validate")
            self.assertEqual(json.loads(sent.data), {"sha": HEAD, "baseSha": LIVE_BASE})
            self.assertEqual(sent.get_header("X-api-key"), "fixture-token")
            self.assertNotIn("fixture-token", sent.full_url)
        for url in ("http://example.invalid", "https://user:secret@example.invalid", "https://example.invalid?q=1"):
            with self.assertRaises(submitter.SubmissionError):
                submitter.relay_client(url, "fixture-token")

    def test_bad_config_cannot_put_tokens_in_error_messages(self):
        for token in (None, "", "secret\nheader", "secret\rheader"):
            for factory in (submitter.github_client,
                            lambda value: submitter.relay_client("https://example.invalid", value)):
                with self.assertRaises(submitter.SubmissionError) as caught:
                    factory(token)
                self.assertNotIn("secret", str(caught.exception))
        for url in ("https://[secret", "https://example.invalid/\nsecret"):
            with self.assertRaises(submitter.SubmissionError) as caught:
                submitter.relay_client(url, "fixture-token")
            self.assertNotIn("secret", str(caught.exception))

    def test_workflow_resolve_output_is_consumed_by_actual_submit_entrypoint(self):
        services = Services()
        with tempfile.TemporaryDirectory() as directory:
            event_path, output_path = Path(directory) / "event.json", Path(directory) / "output"
            event_path.write_text(json.dumps(event()), encoding="utf-8")
            env = {"GH_TOKEN": "fixture-token", "GITHUB_EVENT_NAME": "pull_request_target",
                   "GITHUB_REPOSITORY": REPO, "GITHUB_EVENT_PATH": str(event_path),
                   "GITHUB_OUTPUT": str(output_path), "RELAY": "https://relay.invalid",
                   "TOKEN": "fixture-relay-token"}
            with patch.dict(os.environ, env, clear=True), patch.object(submitter, "github_client", return_value=services.github), \
                    patch.object(sys, "argv", ["submit_pr_validation.py", "resolve"]), patch("sys.stdout", new_callable=io.StringIO):
                self.assertEqual(submitter.main(), 0)
            outputs = dict(line.split("=", 1) for line in output_path.read_text(encoding="utf-8").splitlines())
            self.assertEqual(outputs["head"], HEAD)
            env["VALIDATION_PIN"] = outputs["pin"]
            with patch.dict(os.environ, env, clear=True), patch.object(submitter, "github_client", return_value=services.github), \
                    patch.object(submitter, "relay_client", return_value=services.relay), \
                    patch.object(sys, "argv", ["submit_pr_validation.py", "submit"]), patch("sys.stdout", new_callable=io.StringIO) as output:
                self.assertEqual(submitter.main(), 0)
                self.assertNotIn("fixture", output.getvalue())
            self.assertEqual(services.relay_requests[0][2]["baseSha"], LIVE_BASE)


if __name__ == "__main__":
    unittest.main()
