"""Validate recorded source judgments; this does not judge C++ or prove ROM bytes.

Queue publishers are accountable for the assertions. A passing byte check or an
old review is never substituted for a review of the proposed source and base.
"""
import re


class ReviewError(ValueError):
    pass


def require(condition, message):
    if not condition:
        raise ReviewError(message)


def text(value, label):
    require(isinstance(value, str) and value.strip() and
            not re.search(r"REPLACE_WITH|FULL_.*SHA|not_run|not_reviewed", value),
            label + " must contain a measured result or concrete explanation")
    return value


def commit(value, label):
    require(isinstance(value, str) and re.fullmatch(r"[0-9a-f]{40}|[0-9a-f]{64}", value),
            label + " must be a full immutable commit ID")
    return value


def source_path(path):
    return isinstance(path, str) and path.casefold().startswith(
        ("src/", "include/", "src_tu/", "mods/", "config/tu_manifest.d/",
         "config/arm9/", "config/arm7/", "config/itcm/", "config/dtcm/"))


def task_files(task):
    return sorted({r.split(":", 1)[1] for r in task.get("resources", [])
                   if r.startswith(("file:", "tu:")) and source_path(r.split(":", 1)[1])})


def source_task(task):
    return bool(task_files(task) or any(r.startswith(("class:", "range:"))
                                      for r in task.get("resources", [])) or
                any(source_path(p) for s in task.get("stages", [])
                    for p in s.get("requires", []) + s.get("produces", [])))


def producers(task):
    """Include earlier writers and rejected attempts, not just the latest writer."""
    sessions = set(task.get("producer_sessions", []))
    if task.get("input_session"):
        sessions.add(task["input_session"])
    write_stages = {s["id"] for s in task.get("stages", []) if s["mode"] == "write"}
    outputs = list(task.get("outputs", []))
    for attempt in task.get("attempts", []):
        outputs.extend(attempt.get("outputs", []))
    sessions.update(o["session"] for o in outputs if o.get("stage_id") in write_stages)
    return sessions


def previous_findings(task):
    findings = dict(task.get("inherited_findings", {}))
    evidence = [o.get("evidence", {}) for o in task.get("outputs", [])]
    for attempt in task.get("attempts", []):
        evidence.append(attempt.get("evidence", {}))
        evidence.extend(o.get("evidence", {}) for o in attempt.get("outputs", []))
    for item in evidence:
        review = item.get("source_review") if isinstance(item, dict) else None
        if not isinstance(review, dict) or not isinstance(review.get("findings"), list):
            continue
        for finding in review["findings"]:
            if isinstance(finding, dict) and finding.get("id"):
                findings[finding["id"]] = finding
    return findings


def validate(review, head, base, reviewer, authors=(), required_files=(), prior=()):
    require(isinstance(review, dict), "source_review is required")
    require(review.get("schema") == 1, "source_review.schema must be 1")
    require(review.get("result") == "pass", "source review has not passed")
    require(commit(review.get("reviewed_commit"), "reviewed_commit") == head,
            "source review is for a different candidate")
    require(commit(review.get("reviewed_base"), "reviewed_base") == base,
            "source review is for a different base")
    require(text(review.get("reviewer_session"), "reviewer_session") == reviewer,
            "source reviewer must be the publishing verifier")
    require(reviewer not in authors, "source review requires an independent reviewer")
    text(review.get("summary"), "source review summary")
    require(review.get("completion") in ("complete", "partial"),
            "source review must distinguish complete from partial reconstruction")
    files = review.get("files")
    require(isinstance(files, list) and files and all(isinstance(p, str) and p and
            not p.startswith("/") and "\\" not in p and ":" not in p and
            all(x not in ("", ".", "..") for x in p.split("/")) for p in files),
            "source review needs literal reviewed file paths")
    require({p.casefold() for p in required_files} <= {p.casefold() for p in files},
            "source review does not cover every source/header in scope")
    findings = review.get("findings")
    require(isinstance(findings, list), "source review findings must be a list")
    ids = set()
    for finding in findings:
        require(isinstance(finding, dict), "invalid source finding")
        fid = text(finding.get("id"), "finding id")
        require(fid not in ids, "duplicate source finding id")
        ids.add(fid)
        text(finding.get("location"), "finding location")
        text(finding.get("summary"), "finding summary")
        text(finding.get("reason"), "finding disposition reason")
        kind = finding.get("kind")
        require(kind in ("correctness", "provenance", "reconstruction"), "invalid finding kind")
        if isinstance(prior, dict) and prior.get(fid, {}).get("kind") in ("correctness", "provenance"):
            require(kind == prior[fid]["kind"], "previous correctness/provenance finding was reclassified")
        disposition = finding.get("disposition")
        require(disposition in ("fixed", "compiler_constraint", "deferred"),
                "unresolved source finding: " + fid)
        if disposition == "compiler_constraint":
            require(kind == "reconstruction", "a compiler constraint cannot excuse incorrect claims")
            proof = finding.get("evidence")
            require(isinstance(proof, dict), "compiler constraint requires measured evidence")
            commit(proof.get("tested_commit"), "constraint tested_commit")
            for key in ("command", "result", "log"):
                text(proof.get(key), "constraint " + key)
            require(proof.get("compiler") == "2004/b56", "constraint must identify the pinned compiler")
            text(proof.get("attempted_change"), "constraint attempted_change")
            commit(proof.get("artifact_commit"), "constraint artifact_commit")
            path = text(proof.get("artifact_path"), "constraint artifact_path")
            require(not path.startswith("/") and ":" not in path and "\\" not in path and
                    all(p not in ("", ".", "..") for p in path.split("/")),
                    "constraint artifact_path must be repository-relative")
        if disposition == "deferred":
            require(kind == "reconstruction", "correctness/provenance findings cannot be deferred")
            require(review["completion"] == "partial", "deferred work is not complete reconstruction")
            text(finding.get("owner"), "deferred work owner")
            issue = finding.get("issue")
            require(isinstance(issue, str) and re.fullmatch(
                r"https://github\.com/tangosdev/sm64ds-decomp/issues/[1-9][0-9]*", issue),
                "deferred work needs its repository issue")
    require(set(prior) <= ids, "previous source findings disappeared without a disposition")
    return review


def validate_evidence(task, evidence, head, reviewer, required_files=None, prior=None):
    require(isinstance(evidence, dict), "verification evidence is required")
    require(evidence.get("verdict") == "pass" and evidence.get("tested_commit") == head,
            "passing evidence must identify the exact tested commit")
    require(evidence.get("reviewer_session") == reviewer,
            "evidence reviewer must match the publishing verifier")
    base = commit(evidence.get("tested_base"), "tested_base")
    commit(evidence.get("workflow_commit"), "workflow_commit")
    return validate(evidence.get("source_review"), head, base, reviewer, producers(task),
                    task_files(task) if required_files is None else required_files,
                    previous_findings(task) if prior is None else prior)


def review_graph(task):
    """Every source write must reach independent verification before completion."""
    pending_write = True  # Adopted source also needs review.
    for stage in task["stages"]:
        if stage["role"] == "integrator":
            require(stage["mode"] == "verify" and not pending_write,
                    "integration requires a preceding independent source verification")
        pending_write = stage["mode"] == "write"
    require(not pending_write, "source tasks must finish with independent verification")
