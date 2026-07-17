#!/usr/bin/env python3
"""Append-only log of every matching *attempt* (experimental).

Unlike match_provenance (final how-record on bank) and nearmiss/db.jsonl
(best tip **C** only), this log keeps **every try** as metadata — including no
progress, no near-miss improvement, compile failures, and failed verifies.


Store: config/match_attempts.jsonl  (one JSON object per line, never rewritten)

Record (schemaVersion 1 — attempt tree):
  {
    "schemaVersion": 1,
    "functionId": "arm9:0x02001a64",   # atlas id (stable); same as historical "id"
    "id": "arm9:0x02001a64",          # alias of functionId (backward compatible)
    "attemptId": "a1b2c3…",           # UNIQUE this node (UUID hex); never reuse
    "parentAttemptId": null,          # prior attemptId you built on, or null
    "module": "arm9",
    "addr": 33561188,
    "name": "func_02001a64",
    "status": "no_progress",
    "kind": "ai",
    "model": "grok-4.5",
    "reasoning": "high",
    "harness": "grok-build",
    "author": "lunavyqo",
    "base": {"kind": "scratch"},      # scratch | previous_attempt | near_miss_draft | matched_sibling
    "divergences": null,
    "prevBestDivergences": 4,
    "improvedNearMiss": false,
    "srcPath": "scratch/…",
    "label": "wave-cheap-01",
    "sessionScope": "focused",
    "batchSize": 1,
    "note": "optional short note"
  }

sessionScope:
  focused — this session was only about this function (batchSize should be 1)
  batch   — this function was one of several in the same matching session

Do not embed full c_source here (size). Point srcPath or keep drafts elsewhere.
"""
from __future__ import annotations

import json
import pathlib
import uuid
from typing import Any, Optional

from match_provenance import (
    ProvenanceError,
    make_id,
    normalize_provenance,
    repo as get_repo,
)

STATUSES = frozenset(
    {
        "matched",
        "near_miss",
        "no_progress",
        "compile_error",
        "failed",
        "skipped",
    }
)

SESSION_SCOPES = frozenset({"focused", "batch"})

BASE_KINDS = frozenset(
    {
        "scratch",
        "previous_attempt",
        "near_miss_draft",
        "matched_sibling",
    }
)

SCHEMA_VERSION = 1


def attempts_path() -> pathlib.Path:
    return get_repo() / "config" / "match_attempts.jsonl"


def new_attempt_id() -> str:
    """Unique id for one attempt node (UUID hex — not a1/try2)."""
    return uuid.uuid4().hex


def validate_attempt(rec: dict) -> Optional[str]:
    """Return None if ok, else error string."""
    if not isinstance(rec, dict):
        return "attempt must be an object"
    # functionId preferred; accept legacy id-only rows after normalize fills both.
    fid = rec.get("functionId") or rec.get("id")
    if not fid:
        return "attempt missing functionId/id"
    for req in ("name", "module", "status", "kind", "sessionScope", "batchSize"):
        if req not in rec or rec[req] in (None, ""):
            return (
                f"attempt missing {req!r} "
                f"(sessionScope+batchSize required on every run, like model/harness)"
            )
    if rec["status"] not in STATUSES:
        return f"status must be one of {sorted(STATUSES)}, got {rec['status']!r}"
    kind = str(rec["kind"]).strip().lower()
    if kind not in ("ai", "human"):
        return f'kind must be "ai" or "human", got {kind!r}'
    if kind == "ai":
        try:
            normalize_provenance(
                {
                    "kind": "ai",
                    "model": rec.get("model") or "",
                    "reasoning": rec.get("reasoning") or "",
                    "harness": rec.get("harness") or "",
                }
            )
        except ProvenanceError as e:
            return f"ai attempt method invalid: {e}"
    if "addr" in rec and rec["addr"] is not None:
        try:
            int(rec["addr"])
        except (TypeError, ValueError):
            return "addr must be an integer"
    for key in ("divergences", "prevBestDivergences"):
        if key in rec and rec[key] is not None:
            try:
                int(rec[key])
            except (TypeError, ValueError):
                return f"{key} must be int or null"
    if "improvedNearMiss" in rec and rec["improvedNearMiss"] is not None:
        if not isinstance(rec["improvedNearMiss"], bool):
            return "improvedNearMiss must be bool"
    scope = str(rec.get("sessionScope", "")).strip().lower()
    if scope not in SESSION_SCOPES:
        return f"sessionScope must be focused|batch, got {rec.get('sessionScope')!r}"
    try:
        bs = int(rec["batchSize"])
        if bs < 1:
            return "batchSize must be >= 1"
        if scope == "focused" and bs != 1:
            return "sessionScope=focused requires batchSize=1"
        if scope == "batch" and bs < 2:
            return "sessionScope=batch requires batchSize >= 2"
    except (TypeError, ValueError):
        return "batchSize must be int"

    # Identity (schemaVersion 1)
    aid = rec.get("attemptId")
    if not aid or not str(aid).strip():
        return "attemptId required (unique UUID for this try; never a1/try2)"
    if str(aid).strip().lower() in ("a1", "a2", "try1", "try2", "1", "2"):
        return "attemptId looks non-unique; use a UUID (tools generate one if omitted)"
    parent = rec.get("parentAttemptId", None)
    if parent is not None and parent != "" and not str(parent).strip():
        return "parentAttemptId must be null or a non-empty attemptId"
    if parent is not None and parent != "" and str(parent).strip() == str(aid).strip():
        return "parentAttemptId must not equal attemptId"
    base = rec.get("base")
    if base is not None:
        if not isinstance(base, dict):
            return "base must be an object"
        bk = str(base.get("kind", "")).strip().lower()
        if bk not in BASE_KINDS:
            return f"base.kind must be one of {sorted(BASE_KINDS)}, got {bk!r}"
    return None


def normalize_attempt(rec: dict) -> dict:
    """Clean + validate. Raises ProvenanceError.

    Privacy: do not record wall-clock times (no loggedAt/ts).
    """
    function_id = str(rec.get("functionId") or rec.get("id") or "").strip()
    if not function_id:
        raise ProvenanceError("functionId (or id) required")

    attempt_id = rec.get("attemptId")
    if attempt_id is None or str(attempt_id).strip() == "":
        attempt_id = new_attempt_id()
    else:
        attempt_id = str(attempt_id).strip()

    parent_raw = rec.get("parentAttemptId", None)
    if parent_raw in (None, "", "null"):
        parent_attempt_id = None
    else:
        parent_attempt_id = str(parent_raw).strip()

    out: dict[str, Any] = {
        "schemaVersion": int(rec.get("schemaVersion") or SCHEMA_VERSION),
        "functionId": function_id,
        "id": function_id,  # legacy alias — same as functionId
        "attemptId": attempt_id,
        "parentAttemptId": parent_attempt_id,
        "module": str(rec["module"]),
        "addr": int(rec["addr"]) if rec.get("addr") is not None else None,
        "name": str(rec["name"]),
        "status": str(rec["status"]),
        "kind": str(rec["kind"]).strip().lower(),
    }
    # Privacy: drop any wall-clock fields if a caller still sends them.
    # (loggedAt / ts must not be persisted.)
    if out["kind"] == "ai":
        how = normalize_provenance(
            {
                "kind": "ai",
                "model": rec.get("model") or "",
                "reasoning": rec.get("reasoning") or "",
                "harness": rec.get("harness") or "",
            }
        )
        out["model"] = how["model"]
        out["reasoning"] = how["reasoning"]
        out["harness"] = how["harness"]
    if rec.get("author"):
        out["author"] = str(rec["author"]).strip()
    for key in ("divergences", "prevBestDivergences"):
        if rec.get(key) is not None:
            out[key] = int(rec[key])
    if "improvedNearMiss" in rec and rec["improvedNearMiss"] is not None:
        out["improvedNearMiss"] = bool(rec["improvedNearMiss"])
    elif (
        out.get("divergences") is not None
        and out.get("prevBestDivergences") is not None
    ):
        out["improvedNearMiss"] = int(out["divergences"]) < int(
            out["prevBestDivergences"]
        )
    else:
        out["improvedNearMiss"] = False
    if rec.get("srcPath"):
        out["srcPath"] = str(rec["srcPath"])
    if rec.get("label"):
        out["label"] = str(rec["label"]).strip()
    if rec.get("note"):
        out["note"] = str(rec["note"]).strip()
    # Draft source trackers (independent booleans; lineage may inherit true).
    if "usedNearMissDraft" in rec and rec["usedNearMissDraft"] is not None:
        out["usedNearMissDraft"] = bool(rec["usedNearMissDraft"])
    if "usedGhidraDraft" in rec and rec["usedGhidraDraft"] is not None:
        out["usedGhidraDraft"] = bool(rec["usedGhidraDraft"])

    # base of work (attempt tree)
    base_in = rec.get("base")
    if isinstance(base_in, dict) and base_in.get("kind"):
        bk = str(base_in["kind"]).strip().lower()
        base_out: dict[str, Any] = {"kind": bk}
        if base_in.get("attemptId"):
            base_out["attemptId"] = str(base_in["attemptId"]).strip()
        elif parent_attempt_id and bk == "previous_attempt":
            base_out["attemptId"] = parent_attempt_id
        if base_in.get("divergences") is not None:
            base_out["divergences"] = int(base_in["divergences"])
        out["base"] = base_out
    elif parent_attempt_id:
        out["base"] = {"kind": "previous_attempt", "attemptId": parent_attempt_id}
    else:
        out["base"] = {"kind": "scratch"}

    # Session focus — required every run (same expectation as model/harness).
    scope_raw = rec.get("sessionScope")
    batch_size = rec.get("batchSize")
    if batch_size is not None and batch_size != "":
        batch_size = int(batch_size)
    else:
        batch_size = None
    if scope_raw is not None and str(scope_raw).strip():
        scope = str(scope_raw).strip().lower()
    elif batch_size is not None:
        scope = "focused" if batch_size <= 1 else "batch"
    else:
        raise ProvenanceError(
            "sessionScope required on every attempt (focused|batch), "
            "same as model/harness for AI runs"
        )
    if scope not in SESSION_SCOPES:
        raise ProvenanceError(f"sessionScope must be focused|batch, got {scope!r}")
    if batch_size is None:
        if scope == "focused":
            batch_size = 1
        else:
            raise ProvenanceError("sessionScope=batch requires batchSize >= 2")
    if scope == "focused":
        batch_size = 1
    elif batch_size < 2:
        raise ProvenanceError(
            "sessionScope=batch requires batchSize >= 2 "
            "(use sessionScope=focused for a single-function session)"
        )
    out["sessionScope"] = scope
    out["batchSize"] = int(batch_size)

    err = validate_attempt(out)
    if err:
        raise ProvenanceError(err)
    return out


def append_attempt(
    *,
    module: str,
    addr: int,
    name: str,
    status: str,
    kind: str = "ai",
    model: Optional[str] = None,
    reasoning: Optional[str] = None,
    harness: Optional[str] = None,
    author: Optional[str] = None,
    divergences: Optional[int] = None,
    prev_best_divergences: Optional[int] = None,
    improved_near_miss: Optional[bool] = None,
    src_path: Optional[str] = None,
    label: Optional[str] = None,
    note: Optional[str] = None,
    session_scope: Optional[str] = None,
    batch_size: Optional[int] = None,
    attempt_id: Optional[str] = None,
    parent_attempt_id: Optional[str] = None,
    base_kind: Optional[str] = None,
    base_divergences: Optional[int] = None,
    used_near_miss_draft: Optional[bool] = None,
    used_ghidra_draft: Optional[bool] = None,
    path: Optional[pathlib.Path] = None,
) -> dict:
    """Append one attempt. Always records — including no_progress. Raises ProvenanceError."""
    rid = make_id(module, addr)
    rec: dict[str, Any] = {
        "schemaVersion": SCHEMA_VERSION,
        "functionId": rid,
        "id": rid,
        "attemptId": attempt_id or new_attempt_id(),
        "parentAttemptId": parent_attempt_id,
        "module": module,
        "addr": int(addr),
        "name": name,
        "status": status,
        "kind": kind,
    }
    if kind == "ai":
        rec["model"] = model or ""
        rec["reasoning"] = reasoning or ""
        rec["harness"] = harness or ""
    if author:
        rec["author"] = author
    if divergences is not None:
        rec["divergences"] = divergences
    if prev_best_divergences is not None:
        rec["prevBestDivergences"] = prev_best_divergences
    if improved_near_miss is not None:
        rec["improvedNearMiss"] = improved_near_miss
    if src_path:
        rec["srcPath"] = src_path
    if label:
        rec["label"] = label
    if note:
        rec["note"] = note
    if session_scope is not None:
        rec["sessionScope"] = session_scope
    if batch_size is not None:
        rec["batchSize"] = batch_size
    if used_near_miss_draft is not None:
        rec["usedNearMissDraft"] = bool(used_near_miss_draft)
    if used_ghidra_draft is not None:
        rec["usedGhidraDraft"] = bool(used_ghidra_draft)
    if base_kind:
        base: dict[str, Any] = {"kind": base_kind}
        if parent_attempt_id and base_kind == "previous_attempt":
            base["attemptId"] = parent_attempt_id
        if base_divergences is not None:
            base["divergences"] = base_divergences
        rec["base"] = base

    clean = normalize_attempt(rec)
    out = path or attempts_path()
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("a", encoding="utf-8") as f:
        f.write(json.dumps(clean, separators=(",", ":"), ensure_ascii=False) + "\n")
    return clean


def load_attempts(
    path: Optional[pathlib.Path] = None,
    *,
    function_id: Optional[str] = None,
) -> list[dict]:
    """Load all attempts (optionally filter by function id). Order = file order."""
    path = path or attempts_path()
    if not path.is_file():
        return []
    rows: list[dict] = []
    for line_no, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        try:
            row = json.loads(line)
        except json.JSONDecodeError as e:
            raise ProvenanceError(f"{path}:{line_no}: invalid JSON: {e}") from e
        fid = row.get("functionId") or row.get("id")
        if function_id and fid != function_id:
            continue
        rows.append(row)
    return rows


def attempt_stats(rows: list[dict]) -> dict[str, Any]:
    """Aggregate counts for a list of attempt rows."""
    by_status: dict[str, int] = {}
    by_scope: dict[str, int] = {}
    improved = 0
    matched_focused = 0
    matched_batch = 0
    models: set[str] = set()
    for r in rows:
        s = r.get("status") or "?"
        by_status[s] = by_status.get(s, 0) + 1
        scope = r.get("sessionScope") or "unknown"
        by_scope[scope] = by_scope.get(scope, 0) + 1
        if r.get("improvedNearMiss"):
            improved += 1
        if r.get("status") == "matched":
            if scope == "focused":
                matched_focused += 1
            elif scope == "batch":
                matched_batch += 1
        if r.get("model"):
            models.add(str(r["model"]))
    return {
        "total": len(rows),
        "byStatus": by_status,
        "bySessionScope": by_scope,
        "improvedNearMiss": improved,
        "matchedFocused": matched_focused,
        "matchedBatch": matched_batch,
        "modelsTried": sorted(models),
    }


def has_attempt_with_status(
    function_id: str,
    status: str,
    path: Optional[pathlib.Path] = None,
) -> bool:
    """True if the attempt log already has a row for this function with status."""
    for r in load_attempts(path, function_id=function_id):
        if r.get("status") == status:
            return True
    return False
