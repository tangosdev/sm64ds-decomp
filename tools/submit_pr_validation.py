"""Submit immutable, live PR metadata to the private validation relay.

Loaded by pr-validate.yml from github.workflow_sha, never from a PR checkout.
Only the relay's app can issue the required verdict; success here means submitted.
"""
import argparse
import json
import os
from pathlib import Path
import re
import sys
from urllib.error import HTTPError, URLError
from urllib.parse import quote, urlsplit
from urllib.request import HTTPRedirectHandler, Request, build_opener


class SubmissionError(RuntimeError):
    """Metadata changed or could not be established; do not claim submission."""


def require(condition, message):
    if not condition:
        raise SubmissionError(message)


def repository(value):
    require(isinstance(value, str)
            and re.fullmatch(r"[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+", value),
            "Invalid repository identity")
    require(all(part not in (".", "..") for part in value.split("/")),
            "Invalid repository identity")
    return value.lower()


def positive_integer(value):
    require(type(value) is int and value > 0, "Expected a positive integer")
    return value


def pr_number(value):
    require(isinstance(value, str) and re.fullmatch(r"[1-9][0-9]*", value),
            "PR number must be a positive decimal integer")
    return positive_integer(int(value))


def commit_sha(value):
    require(isinstance(value, str) and re.fullmatch(r"[0-9a-fA-F]{40}", value),
            "Expected a full GitHub commit SHA")
    return value.lower()


def branch_name(value):
    require(isinstance(value, str) and value and value != "@"
            and not any(ord(char) < 33 or ord(char) == 127 for char in value)
            and not any(char in value for char in "~^:?*[\\")
            and ".." not in value and "@{" not in value and not value.endswith(".")
            and all(part and not part.startswith(".") and not part.endswith(".lock")
                    for part in value.split("/")), "Invalid branch identity")
    return value


def pr_identity(data, repo, number):
    require(isinstance(data, dict), "Malformed pull request metadata")
    require(type(data.get("number")) is int and data["number"] == number,
            "Pull request number changed")
    require(data.get("state") == "open" and data.get("merged") is False,
            "Pull request is not open and unmerged")
    head, base = data.get("head"), data.get("base")
    require(isinstance(head, dict) and isinstance(base, dict),
            "Missing pull request head or target")
    head_repo, base_repo = head.get("repo"), base.get("repo")
    require(isinstance(head_repo, dict) and isinstance(base_repo, dict),
            "Missing pull request repository (including a deleted fork)")
    require(repository(base_repo.get("full_name")) == repo,
            "Pull request targets a different repository")
    # base.sha can be GitHub's cached PR base. Validate its shape, but obtain the
    # submission base from the actual refs/heads/<base.ref> below.
    commit_sha(base.get("sha"))
    return {"repo": repo, "pr": number, "sha": commit_sha(head.get("sha")),
            "baseRef": branch_name(base.get("ref")),
            "baseRepoId": positive_integer(base_repo.get("id")),
            "headRepo": repository(head_repo.get("full_name")),
            "headRepoId": positive_integer(head_repo.get("id")),
            "headRef": branch_name(head.get("ref"))}


def validate_pin(pin):
    fields = {"repo", "pr", "sha", "baseRef", "baseRepoId", "headRepo",
              "headRepoId", "headRef", "baseSha"}
    require(isinstance(pin, dict) and set(pin) == fields, "Malformed submission pin")
    result = dict(pin)
    for key in ("repo", "headRepo"):
        result[key] = repository(pin[key])
    for key in ("pr", "baseRepoId", "headRepoId"):
        result[key] = positive_integer(pin[key])
    for key in ("sha", "baseSha"):
        result[key] = commit_sha(pin[key])
    for key in ("baseRef", "headRef"):
        result[key] = branch_name(pin[key])
    return result


def live_base(github, repo, branch):
    data = github(f"repos/{repo}/git/ref/heads/{quote(branch, safe='')}")
    require(isinstance(data, dict) and data.get("ref") == f"refs/heads/{branch}",
            "Target ref response does not match the requested branch")
    obj = data.get("object")
    require(isinstance(obj, dict) and obj.get("type") == "commit",
            "Target ref does not identify a commit")
    return commit_sha(obj.get("sha"))


def resolve(github, repo, event_name, event):
    """Select the current head/target; a PR event must still identify this head."""
    repo = repository(repo)
    require(isinstance(event, dict), "Malformed workflow event")
    if event_name == "pull_request_target":
        number = positive_integer(event.get("number"))
        expected = pr_identity(event.get("pull_request"), repo, number)
    elif event_name == "workflow_dispatch":
        inputs = event.get("inputs")
        require(isinstance(inputs, dict) and set(inputs) == {"pr"},
                "Manual dispatch accepts only the PR number")
        number = pr_number(inputs["pr"])
        expected = None
    else:
        raise SubmissionError("Unsupported workflow event")
    current = pr_identity(github(f"repos/{repo}/pulls/{number}"), repo, number)
    require(expected is None or current == expected,
            "PR head or target changed since the triggering event; dispatch again")
    return dict(current, baseSha=live_base(github, repo, current["baseRef"]))


# This is the existing workflow's inert set. Everything else remains relevant:
# in particular source, headers, config, tools and the attribution ledgers.
# src_tu is checked separately; the relay's stock ROM profile does not enroll it.
INERT = re.compile(r"^(docs/|notes/|src_tu/|\.github/ISSUE_TEMPLATE/)"
                   r"|(^|/)[^/]+\.(md|png|jpg|jpeg|gif|svg)$|^LICENSE$")


def validation_relevant(github, repo, number, changed_files):
    """Only a complete, nonempty list of wholly inert paths earns False."""
    # GitHub caps the PR files endpoint at 3000. Uncertain scope must validate.
    if type(changed_files) is not int or not 0 < changed_files <= 3000:
        return True
    files = []
    for page in range(1, (changed_files + 99) // 100 + 1):
        batch = github(f"repos/{repo}/pulls/{number}/files?per_page=100&page={page}")
        if not isinstance(batch, list) or len(batch) > 100:
            return True
        files.extend(batch)
    if len(files) != changed_files:
        return True
    seen = set()
    for entry in files:
        if not isinstance(entry, dict):
            return True
        name = entry.get("filename")
        if not isinstance(name, str) or name in seen:
            return True
        seen.add(name)
        paths = [name]
        if entry.get("status") == "renamed" and not entry.get("previous_filename"):
            return True
        if "previous_filename" in entry:
            paths.append(entry["previous_filename"])
        if entry.get("status") not in ("added", "removed", "modified", "renamed",
                                       "copied", "changed", "unchanged"):
            return True
        for path in paths:
            if (not isinstance(path, str) or not path or "\\" in path
                    or any(ord(char) < 32 or ord(char) == 127 for char in path)
                    or any(part in ("", ".", "..") for part in path.split("/"))
                    or not INERT.search(path)):
                return True
    return False


def check_current(github, pin):
    repo, number = pin["repo"], pin["pr"]
    current = github(f"repos/{repo}/pulls/{number}")
    expected = {key: value for key, value in pin.items() if key != "baseSha"}
    require(pr_identity(current, repo, number) == expected,
            "PR head, fork or target changed after resolution; dispatch again")
    return current


def submit(github, relay, pin):
    """Check current identity around classification, then submit and read back pins."""
    pin = validate_pin(pin)
    current = check_current(github, pin)
    # The files endpoint shares the PR's cached comparison base. When that base
    # is stale, its path list cannot establish that the live-base change is inert.
    relevant = (commit_sha(current["base"]["sha"]) != pin["baseSha"]
                or validation_relevant(github, pin["repo"], pin["pr"],
                                       current.get("changed_files")))
    payload = {key: pin[key] for key in ("repo", "pr", "sha", "baseSha")}
    payload["validationRelevant"] = relevant
    # The API offers no atomic PR+branch snapshot. Read the target on both sides
    # of the final PR read, then POST immediately. Merge still needs fresh gates.
    for final_read in range(2):
        require(live_base(github, pin["repo"], pin["baseRef"]) == pin["baseSha"],
                "Target branch advanced after resolution; dispatch again")
        if final_read == 0:
            check_current(github, pin)
    response = relay("POST", "/api/pr-validate", payload)
    require(isinstance(response, dict) and response.get("ok") is True
            and isinstance(response.get("jobId"), str)
            and re.fullmatch(r"[0-9a-f]{32}", response["jobId"])
            and response.get("status") in ("Queued", "Running", "Passed"),
            "Relay did not acknowledge a validation job")
    job_id = response["jobId"]
    job = relay("GET", f"/api/pr-validate/{job_id}", None)
    # Relay deduplication is by repo+head, not base. Never mistake an older
    # in-flight job for a submission against this base; do not cancel it either.
    require(isinstance(job, dict) and job.get("id") == job_id,
            "Relay returned malformed job metadata")
    require(job.get("repo") == pin["repo"] and type(job.get("pr")) is int
            and job["pr"] == pin["pr"] and job.get("sha") == pin["sha"]
            and job.get("baseSha") == pin["baseSha"],
            "Relay job has different PR/head/base pins; retry after it finishes")
    return {"jobId": job_id, **payload}


class NoRedirect(HTTPRedirectHandler):
    def redirect_request(self, req, fp, code, msg, headers, newurl):
        # Tokens are for exactly the configured service, never a redirect target.
        return None


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        require(key not in result, "Duplicate JSON object key")
        result[key] = value
    return result


def load_json(text):
    try:
        return json.loads(text, object_pairs_hook=unique_object,
                          parse_constant=lambda value: require(False, "Invalid JSON constant"))
    except (ValueError, UnicodeError) as exc:
        raise SubmissionError("Malformed JSON response or input") from exc


def request_json(request):
    try:
        with build_opener(NoRedirect()).open(request, timeout=30) as response:
            require(200 <= response.status < 300, "Unexpected HTTP response status")
            body = response.read(2_000_001)
            require(len(body) <= 2_000_000, "JSON response too large")
            return load_json(body)
    except HTTPError as exc:
        raise SubmissionError(f"Service request failed (HTTP {exc.code})") from None
    except (URLError, OSError, ValueError):
        # Exception URLs and response bodies may contain configured secret data.
        raise SubmissionError("Service request failed; no submission confirmed") from None


def github_client(token):
    require(isinstance(token, str) and token and "\r" not in token and "\n" not in token,
            "Missing or malformed GitHub token")

    def get(path):
        return request_json(Request(f"https://api.github.com/{path}", headers={
            "Authorization": f"Bearer {token}", "Accept": "application/vnd.github+json",
            "X-GitHub-Api-Version": "2022-11-28", "User-Agent": "sm64ds-pr-validation"}))

    return get


def relay_client(url, token):
    try:
        parts = urlsplit(url)
    except ValueError:
        raise SubmissionError("Malformed relay URL") from None
    require(parts.scheme == "https" and parts.netloc and not parts.username
            and not parts.password and not parts.query and not parts.fragment
            and not any(ord(char) <= 32 or ord(char) == 127 for char in url),
            "Expected an HTTPS relay URL without credentials, query or fragment")
    require(isinstance(token, str) and token and "\r" not in token and "\n" not in token,
            "Missing or malformed relay token")

    def call(method, path, payload):
        body = json.dumps(payload).encode("utf-8") if payload is not None else None
        return request_json(Request(url.rstrip("/") + path, data=body, method=method,
                                    headers={"X-Api-Key": token,
                                             "Content-Type": "application/json"}))

    return call


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("resolve", "submit"))
    args = parser.parse_args()
    try:
        github = github_client(os.environ.get("GH_TOKEN"))
        if args.command == "resolve":
            event = load_json(Path(os.environ["GITHUB_EVENT_PATH"]).read_text(encoding="utf-8"))
            pin = resolve(github, os.environ["GITHUB_REPOSITORY"],
                          os.environ["GITHUB_EVENT_NAME"], event)
            with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as output:
                output.write(f"head={pin['sha']}\npin={json.dumps(pin, separators=(',', ':'))}\n")
            print(f"Pinned {pin['repo']}#{pin['pr']} head {pin['sha']} "
                  f"against live {pin['baseRef']} {pin['baseSha']}")
        else:
            pin = load_json(os.environ["VALIDATION_PIN"])
            relay = relay_client(os.environ["RELAY"], os.environ["TOKEN"])
            result = submit(github, relay, pin)
            print(json.dumps(result, sort_keys=True))
            print("Submitted. Only the relay's PR validation check supplies the verdict.")
        return 0
    except (SubmissionError, KeyError, OSError) as exc:
        message = str(exc) if isinstance(exc, SubmissionError) else "Missing workflow input or file"
        print(f"Validation submission failed: {message}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
