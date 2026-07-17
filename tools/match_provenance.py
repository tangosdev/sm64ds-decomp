#!/usr/bin/env python3
"""Match method provenance + durable ledger for experimental atlas tracking.

WHO (credit / contributor colors) = classic function field `author` (GitHub login).
  Same rule as sm64ds chaos_db_ci: prefer **git** first-adder of the surviving
  src/ file — never the AI/harness name (no "grok" as author).
HOW  (method)                     = matchProvenance only:

  human:  {"kind":"human", "note"?: str}
  ai:     {"kind":"ai", "model": str, "reasoning": str, "harness": str}

Ledger row (config/match_provenance.jsonl):
  {id, module, addr, name, author?, srcPath?, matchProvenance}

AI how-records require non-empty model, reasoning, harness.
Legacy matchProvenance.by is NOT used for credit if it looks like an agent/harness.

Token form (after normalize): spaces → hyphens, lowercased.
  GOOD: grok-4.5, grok-build   BAD display: "Grok 4.5" (auto-slugified)

Repo root: --repo / DECOMP_ROOT / MATCH_REPO / cwd walk / __file__ walk.
"""
from __future__ import annotations

import json
import os
import pathlib
import re
import subprocess
from typing import Any, Optional

# Module-level defaults; call configure(repo) before banking if scripts live outside the tree.
_REPO: Optional[pathlib.Path] = None
_LEDGER: Optional[pathlib.Path] = None

KINDS = frozenset({"human", "ai"})
# Stable short tokens after normalize (no spaces).
HARNESS_RE = re.compile(r"^[a-z0-9][a-z0-9._/-]{0,63}$")
MODEL_RE = re.compile(r"^[a-z0-9][a-z0-9._+/-]{0,127}$")
REASONING_RE = re.compile(r"^[a-z0-9][a-z0-9._+/-]{0,63}$")

TOKEN_HELP = (
    "use slug tokens (no spaces): model=grok-4.5 reasoning=high harness=grok-build "
    "(display names like 'Grok 4.5' are auto-slugified)"
)

# GitHub noreply: 60808132+lunavyqo@users.noreply.github.com or lunavyqo@users.noreply…
LOGIN_RE = re.compile(r"^(?:\d+\+)?([^@]+)@users\.noreply\.github\.com$", re.I)

# Never treat these as classic credit (agent / model / harness product names).
_AGENT_CREDIT_BLOCKLIST = frozenset(
    {
        "grok",
        "claude",
        "codex",
        "cursor",
        "antigravity",
        "gpt",
        "openai",
        "anthropic",
        "gemini",
        "deepseek",
        "composer",
        "chatgpt",
        "assistant",
        "bot",
        "agent",
        "ai",
        "llm",
        "unknown",
        "none",
        "null",
        "n/a",
        "na",
    }
)


class ProvenanceError(ValueError):
    """Invalid or incomplete match provenance."""


def is_agent_credit(handle: Optional[str]) -> bool:
    """True if this string looks like an agent/harness name, not a human GitHub login."""
    if not handle or not str(handle).strip():
        return True
    s = str(handle).strip().lower()
    if s in _AGENT_CREDIT_BLOCKLIST:
        return True
    # model/harness slugs often contain version digits: grok-4.5, claude-opus-4
    if re.match(
        r"^(grok|claude|gpt|gemini|deepseek|composer|codex|cursor|antigravity|"
        r"sonnet|opus|haiku|llama|mistral|kimi|glm|stepfun|muse)([-._].*)?$",
        s,
    ):
        return True
    if "build" in s and any(x in s for x in ("grok", "claude", "cursor", "codex")):
        return True
    return False


def handle_from_git_identity(name: str, email: str) -> str:
    """git author → GitHub-ish login (sm64ds chaos_db_ci rule)."""
    email = (email or "").strip()
    m = LOGIN_RE.match(email)
    if m:
        return m.group(1)
    if "@" in email:
        local = email.split("@", 1)[0].lower().strip()
        if local and not is_agent_credit(local):
            return local
    name = (name or "").strip()
    if name and not is_agent_credit(name):
        # Prefer single-token logins; collapse spaces for "Luna Vyqo" style poorly
        return re.sub(r"\s+", "", name)
    return name or "unknown"


def first_matchers(root: Optional[pathlib.Path] = None) -> dict[str, str]:
    """{'src/…': handle} — first git adder of each surviving src/ path (sm64ds).

    Renames carry credit; delete+add starts a new lineage.
    """
    root = root or repo()
    try:
        out = subprocess.run(
            [
                "git",
                "-c",
                "diff.renameLimit=0",
                "log",
                "--reverse",
                "--diff-filter=ADR",
                "-M",
                "--format=%x01%an%x02%ae",
                "--name-status",
                "--",
                "src/",
            ],
            cwd=root,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=120,
            check=False,
        ).stdout
    except (OSError, subprocess.SubprocessError):
        return {}
    origin: dict[str, str] = {}
    handle: Optional[str] = None
    for line in out.splitlines():
        if line.startswith("\x01"):
            name, _, email = line[1:].partition("\x02")
            handle = handle_from_git_identity(name, email)
        elif handle and line and line[0] in "ADR":
            parts = line.split("\t")
            code = parts[0]
            if code.startswith("A") and len(parts) >= 2:
                origin.setdefault(parts[1].strip(), handle)
            elif code.startswith("D") and len(parts) >= 2:
                origin.pop(parts[1].strip(), None)
            elif code.startswith("R") and len(parts) >= 3:
                old, new = parts[1].strip(), parts[2].strip()
                origin[new] = origin.pop(old, handle)
    return origin


def git_config_handle(root: Optional[pathlib.Path] = None) -> Optional[str]:
    """Current repo git user (for brand-new files not yet committed)."""
    root = root or repo()
    try:
        email = subprocess.run(
            ["git", "config", "user.email"],
            cwd=root,
            capture_output=True,
            text=True,
            check=False,
        ).stdout.strip()
        name = subprocess.run(
            ["git", "config", "user.name"],
            cwd=root,
            capture_output=True,
            text=True,
            check=False,
        ).stdout.strip()
    except OSError:
        return None
    if not email and not name:
        return None
    h = handle_from_git_identity(name, email)
    return None if is_agent_credit(h) else h


def attribution_overrides(root: Optional[pathlib.Path] = None) -> dict[str, str]:
    """Optional attribution.json overrides: {"overrides": {"src/x.c": "login"}}."""
    root = root or repo()
    p = root / "attribution.json"
    if not p.is_file():
        return {}
    try:
        data = json.loads(p.read_text(encoding="utf-8"))
        ov = data.get("overrides", {}) if isinstance(data, dict) else {}
        return {
            k: v
            for k, v in ov.items()
            if isinstance(k, str)
            and k.startswith("src/")
            and isinstance(v, str)
            and v
            and not is_agent_credit(v)
        }
    except Exception:
        return {}


def resolve_credit_author(
    src_path: Optional[str] = None,
    *,
    explicit: Optional[str] = None,
    root: Optional[pathlib.Path] = None,
    first: Optional[dict[str, str]] = None,
) -> Optional[str]:
    """Resolve classic credit (GitHub login) like sm64ds.

    Priority:
      1. attribution.json overrides for src_path
      2. explicit CLI --author only if it is not an agent name
      3. git first-adder of src_path (surviving match lineage)
      4. git config user (new / uncommitted promote)
    Never returns agent/harness product names (e.g. grok).
    """
    root = root or repo()
    overrides = attribution_overrides(root)
    if src_path:
        sp = src_path.lstrip("./")
        if sp in overrides:
            return overrides[sp]
    if explicit and str(explicit).strip() and not is_agent_credit(explicit):
        return str(explicit).strip()
    if explicit and is_agent_credit(explicit):
        # fall through — do not credit "grok"
        pass
    if src_path:
        sp = src_path.lstrip("./")
        fm = first if first is not None else first_matchers(root)
        if sp in fm and not is_agent_credit(fm[sp]):
            return fm[sp]
    cfg = git_config_handle(root)
    if cfg:
        return cfg
    return None

def is_repo_root(path: pathlib.Path) -> bool:
    """True if path looks like electroplankton-decomp (or sibling layout)."""
    p = path
    return (p / "tools" / "match.py").is_file() and (p / "config").is_dir()


def resolve_repo(explicit: Optional[pathlib.Path | str] = None) -> pathlib.Path:
    """Find decomp root. Prefer explicit / env / cwd walk / __file__ walk."""
    candidates: list[pathlib.Path] = []
    tried: list[str] = []

    def consider(label: str, p: Optional[pathlib.Path]) -> Optional[pathlib.Path]:
        if p is None:
            return None
        try:
            r = p.expanduser().resolve()
        except OSError:
            tried.append(f"{label}: {p} (unresolvable)")
            return None
        tried.append(f"{label}: {r}")
        if is_repo_root(r):
            return r
        return None

    if explicit is not None:
        hit = consider("--repo/explicit", pathlib.Path(explicit))
        if hit:
            return hit
        raise SystemExit(
            f"ERROR: not a decomp root: {explicit}\n"
            f"Expected tools/match.py and config/ under that path.\n"
            f"Tried:\n  " + "\n  ".join(tried)
        )

    for key in ("DECOMP_ROOT", "MATCH_REPO"):
        env = os.environ.get(key)
        if env:
            hit = consider(f"env {key}", pathlib.Path(env))
            if hit:
                return hit

    cwd = pathlib.Path.cwd()
    for cand in [cwd, *cwd.resolve().parents]:
        hit = consider("cwd-walk", cand)
        if hit:
            return hit

    here = pathlib.Path(__file__).resolve().parent  # …/tools or /tmp/…
    for cand in [here, *here.parents]:
        hit = consider("__file__-walk", cand)
        if hit:
            return hit

    raise SystemExit(
        "ERROR: could not find decomp repository root.\n"
        "Pass --repo /path/to/electroplankton-decomp or set DECOMP_ROOT.\n"
        "Markers: tools/match.py + config/\n"
        "Tried:\n  " + "\n  ".join(tried[-12:])
    )


def configure(repo: Optional[pathlib.Path | str] = None) -> pathlib.Path:
    """Set module-level REPO / LEDGER. Returns resolved repo path."""
    global _REPO, _LEDGER
    root = resolve_repo(repo)
    _REPO = root
    _LEDGER = root / "config" / "match_provenance.jsonl"
    return root


def repo() -> pathlib.Path:
    if _REPO is None:
        configure()
    assert _REPO is not None
    return _REPO


def ledger_path() -> pathlib.Path:
    if _LEDGER is None:
        configure()
    assert _LEDGER is not None
    return _LEDGER


# Back-compat names used by older imports / docs.
def _lazy_repo() -> pathlib.Path:
    return repo()


def _lazy_ledger() -> pathlib.Path:
    return ledger_path()


# Property-like module attributes for `from match_provenance import REPO` won't
# work as Path objects; bank.py should call configure() and use repo().


def make_id(module: str, addr: int) -> str:
    return f"{module}:0x{addr:08x}"


def slugify_token(value: str, *, field: str) -> str:
    """Turn display names into stable tokens.

    'Grok 4.5' → 'grok-4.5', 'Grok Build' → 'grok-build'
    """
    s = value.strip()
    s = re.sub(r"\s+", "-", s)
    s = s.lower()
    # collapse duplicate hyphens
    while "--" in s:
        s = s.replace("--", "-")
    s = s.strip("-._/")
    if not s:
        raise ProvenanceError(f"ai.{field} empty after normalize")
    return s


def suggest_token(value: str) -> str:
    try:
        return slugify_token(value, field="?")
    except ProvenanceError:
        return "<empty>"


def validate_match_provenance(rec: Any) -> Optional[str]:
    """Return None if ok, else a short error string.

    Accepts the inner matchProvenance object (not the full ledger row).
    Does not normalize — call normalize_provenance for that.
    """
    if rec is None:
        return "matchProvenance is null/missing"
    if not isinstance(rec, dict):
        return "matchProvenance must be an object"
    kind = rec.get("kind")
    if kind not in KINDS:
        return f'kind must be "human" or "ai", got {kind!r}'

    if kind == "human":
        for k, v in rec.items():
            if k == "kind":
                continue
            # `by` is legacy (credit → author); allow on input, stripped in normalize.
            if k not in ("by", "note"):
                return f"human provenance unknown field: {k!r}"
            if v is not None and not isinstance(v, str):
                return f"human.{k} must be a string"
        return None

    for req in ("model", "reasoning", "harness"):
        v = rec.get(req)
        if not isinstance(v, str) or not v.strip():
            return f"ai provenance requires non-empty string {req!r} ({TOKEN_HELP})"
    model = rec["model"].strip()
    reasoning = rec["reasoning"].strip()
    harness = rec["harness"].strip()
    if not MODEL_RE.match(model):
        sug = suggest_token(model)
        return (
            f"ai.model invalid token: {model!r} "
            f"(try {sug!r}; {TOKEN_HELP})"
        )
    if not REASONING_RE.match(reasoning):
        sug = suggest_token(reasoning)
        return f"ai.reasoning invalid token: {reasoning!r} (try {sug!r})"
    if not HARNESS_RE.match(harness):
        sug = suggest_token(harness)
        return (
            f"ai.harness invalid token: {harness!r} "
            f"(try {sug!r}; lowercase slug, no spaces)"
        )
    for k in rec:
        # legacy `by` tolerated on input
        if k not in ("kind", "model", "reasoning", "harness", "by"):
            return f"ai provenance unknown field: {k!r}"
    return None


def normalize_provenance(rec: dict) -> dict:
    """Slugify AI tokens; strip legacy `by` (credit lives on author). Raises ProvenanceError."""
    if not isinstance(rec, dict):
        raise ProvenanceError("matchProvenance must be an object")
    kind = (rec.get("kind") or "").strip().lower()
    if kind == "human":
        out: dict[str, Any] = {"kind": "human"}
        if rec.get("note"):
            out["note"] = str(rec["note"]).strip()
        err = validate_match_provenance(out)
        if err:
            raise ProvenanceError(err)
        return out
    if kind == "ai":
        out = {
            "kind": "ai",
            "model": slugify_token(str(rec.get("model") or ""), field="model"),
            "reasoning": slugify_token(
                str(rec.get("reasoning") or ""), field="reasoning"
            ),
            "harness": slugify_token(str(rec.get("harness") or ""), field="harness"),
        }
        err = validate_match_provenance(out)
        if err:
            raise ProvenanceError(err)
        return out
    raise ProvenanceError(f'kind must be "human" or "ai", got {rec.get("kind")!r}')


def load_ledger(path: Optional[pathlib.Path] = None) -> dict[str, dict]:
    """Load provenance ledger keyed by id (module:0xaddr). Last write wins."""
    path = path or ledger_path()
    by_id: dict[str, dict] = {}
    if not path.is_file():
        return by_id
    for line_no, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        try:
            row = json.loads(line)
        except json.JSONDecodeError as e:
            raise ProvenanceError(f"{path}:{line_no}: invalid JSON: {e}") from e
        rid = row.get("id")
        if not rid and "module" in row and "addr" in row:
            addr = row["addr"]
            if isinstance(addr, str):
                addr = int(addr, 0)
            rid = make_id(str(row["module"]), int(addr))
        if not rid:
            raise ProvenanceError(f"{path}:{line_no}: row needs id or module+addr")
        prov = row.get("matchProvenance") or row.get("provenance")
        if prov is None:
            raise ProvenanceError(f"{path}:{line_no}: missing matchProvenance")
        # Credit: row.author only if human; never promote agent names from
        # legacy matchProvenance.by (that was wrongly filled with "grok" etc.).
        author = row.get("author")
        if author and is_agent_credit(str(author)):
            author = None
        if not author and isinstance(prov, dict) and prov.get("by"):
            by = str(prov["by"]).strip()
            if by and not is_agent_credit(by):
                author = by
        try:
            clean = normalize_provenance(prov)
        except ProvenanceError as e:
            raise ProvenanceError(f"{path}:{line_no}: {e}") from e
        entry = {
            "id": str(rid),
            "module": row.get("module"),
            "addr": row.get("addr"),
            "name": row.get("name"),
            "srcPath": row.get("srcPath"),
            "matchProvenance": clean,
        }
        if author and not is_agent_credit(str(author)):
            entry["author"] = str(author).strip()
        by_id[str(rid)] = entry
    return by_id


def lookup_provenance(
    module: str,
    addr: int,
    ledger: Optional[dict[str, dict]] = None,
) -> Optional[dict]:
    """Return matchProvenance dict for (module, addr), or None."""
    if ledger is None:
        ledger = load_ledger()
    row = ledger.get(make_id(module, addr))
    if not row:
        return None
    return row.get("matchProvenance")


def append_ledger_row(
    *,
    module: str,
    addr: int,
    name: str,
    provenance: dict,
    src_path: Optional[str] = None,
    author: Optional[str] = None,
    path: Optional[pathlib.Path] = None,
) -> dict:
    """Validate, append one JSONL row, return the row. Raises ProvenanceError.

    `author` is classic chaos-viewer credit (GitHub login from git, not agent).
    `provenance` is how only — never put the operator or agent in how.by.
    """
    path = path or ledger_path()
    prov = normalize_provenance(provenance)
    rid = make_id(module, addr)
    credit = resolve_credit_author(src_path, explicit=author)
    row: dict[str, Any] = {
        "id": rid,
        "module": module,
        "addr": int(addr),
        "name": name,
        "matchProvenance": prov,
    }
    if credit:
        row["author"] = credit
    if src_path:
        row["srcPath"] = src_path
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as f:
        f.write(json.dumps(row, separators=(",", ":"), ensure_ascii=False) + "\n")
    return row


def provenance_from_cli_args(
    *,
    kind: str,
    model: Optional[str] = None,
    reasoning: Optional[str] = None,
    harness: Optional[str] = None,
    note: Optional[str] = None,
) -> dict:
    """Build a how-only provenance object from CLI fields. Raises ProvenanceError.

    Operator credit is not part of provenance — pass `author` to append_ledger_row.
    """
    kind = (kind or "").strip().lower()
    if kind == "human":
        rec: dict[str, Any] = {"kind": "human"}
        if note:
            rec["note"] = note
        return normalize_provenance(rec)
    if kind == "ai":
        return normalize_provenance(
            {
                "kind": "ai",
                "model": model or "",
                "reasoning": reasoning or "",
                "harness": harness or "",
            }
        )
    raise ProvenanceError(f'kind must be "human" or "ai", got {kind!r}')
