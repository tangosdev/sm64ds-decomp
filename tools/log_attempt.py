#!/usr/bin/env python3
"""Log one matching attempt (always — even no progress / no near-miss improve).

Examples:

  # AI try that didn't help (new root)
  python tools/log_attempt.py --func func_02001a64 --module arm9 --addr 0x02001a64 \\
      --status no_progress --kind ai \\
      --model grok-4.5 --reasoning high --harness grok-build \\
      --author lunavyqo --session-scope focused --batch-size 1 \\
      --note "permuter stuck"

  # Near-miss with C file → also upserts tip into nearmiss/db.jsonl
  python tools/log_attempt.py --func func_02001a64 \\
      --status near_miss --divergences 6 --prev-best 4 \\
      --src scratch/func_02001a64.c \\
      --kind ai --model grok-4.5 --reasoning high --harness grok-build \\
      --session-scope focused --batch-size 1 \\
      --parent-attempt-id <priorAttemptId>


  # Successful match (also bank with bank.py)
  python tools/log_attempt.py --func … --status matched --kind ai …

Statuses: matched | near_miss | no_progress | compile_error | failed | skipped
"""
from __future__ import annotations

import argparse
import pathlib
from pathlib import Path
import re
import sys

_TOOLS = pathlib.Path(__file__).resolve().parent
if str(_TOOLS) not in sys.path:
    sys.path.insert(0, str(_TOOLS))

from match_attempts import append_attempt, attempt_stats, load_attempts  # noqa: E402
from match_provenance import (  # noqa: E402
    ProvenanceError,
    configure,
    make_id,
)
from nearmiss_db import locked, load_db, save_db, resolve_name  # noqa: E402


def _int(x):
    """Parse an addr/size that may arrive as hex ('0x...'), decimal string, or an int.
    --addr/--size are documented as hex and the match tool passes hex, so base-0 auto-detect."""
    return int(str(x), 0)


def load_symbol(name: str):
    """Return (module, addr, size) from symbols / nearmiss resolve, or None."""
    m = resolve_name(name)
    if not m:
        return None
    addr, size, mod, _thex = m
    return mod, _int(addr), _int(size)


def upsert_near_miss_tip(*, src_file, module, addr, name, divergences, size, source):
    """Best-tip upsert into SM64DS nearmiss/db.jsonl (no ROM re-eval required)."""
    c_source = Path(src_file).read_text(encoding="utf-8", errors="replace")
    key = (module, _int(addr))
    with locked():
        db = load_db()
        cur = db.get(key)
        curdiv = cur.get("divergences") if cur and cur.get("divergences") is not None else 10**9
        if cur is not None and int(divergences) >= int(curdiv):
            return "kept", cur
        rec = {
            "module": module,
            "addr": _int(addr),
            "name": name,
            "size": _int(size) if size is not None else (cur or {}).get("size"),
            "lang": "cpp" if c_source.lstrip().startswith("//cpp") else "c",
            "divergences": int(divergences),
            "c_source": c_source,
            "source": source or "log_attempt",
        }
        if cur and cur.get("target_hex"):
            rec["target_hex"] = cur["target_hex"]
        meta = resolve_name(name)
        if meta:
            _a, _s, _m, thex = meta
            rec["target_hex"] = thex
            rec["size"] = rec.get("size") or _s
        db[key] = rec
        save_db(db)
        return ("improved" if cur else "added"), rec


ADDR_IN_NAME = re.compile(r"func_([0-9a-fA-F]{6,8})$", re.I)


def main() -> None:
    ap = argparse.ArgumentParser(
        description="Append one match attempt to config/match_attempts.jsonl"
    )
    ap.add_argument("--repo", type=pathlib.Path, default=None)
    ap.add_argument("--func", help="Symbol name")
    ap.add_argument("--module", default=None)
    ap.add_argument("--addr", help="Address hex")
    ap.add_argument("--src", type=pathlib.Path, help="Path to attempt C (logged as srcPath)")
    ap.add_argument(
        "--status",
        required=True,
        choices=[
            "matched",
            "near_miss",
            "no_progress",
            "compile_error",
            "failed",
            "skipped",
        ],
    )
    ap.add_argument("--kind", default="ai", choices=("ai", "human"))
    ap.add_argument("--model")
    ap.add_argument("--reasoning")
    ap.add_argument("--harness")
    ap.add_argument("--author", "--by", dest="author")
    ap.add_argument("--divergences", type=int, default=None)
    ap.add_argument("--prev-best", type=int, default=None, dest="prev_best")
    ap.add_argument(
        "--improved",
        action=argparse.BooleanOptionalAction,
        default=None,
        help="Override improvedNearMiss (default: compare divergences vs --prev-best)",
    )
    ap.add_argument("--label", help="Batch / wave / session label (e.g. wave-cheap-01)")
    ap.add_argument(
        "--session-scope",
        choices=("focused", "batch"),
        required=True,
        help="REQUIRED every run: focused = solo session; batch = multi-function session",
    )
    ap.add_argument(
        "--batch-size",
        type=int,
        default=None,
        dest="batch_size",
        help="REQUIRED for batch (≥2); defaults to 1 when --session-scope=focused",
    )
    ap.add_argument(
        "--attempt-id",
        dest="attempt_id",
        help="Optional unique id for this node (UUID hex). Generated if omitted.",
    )
    ap.add_argument(
        "--parent-attempt-id",
        dest="parent_attempt_id",
        default=None,
        help="attemptId of the node you built on (omit/null for new root)",
    )
    ap.add_argument(
        "--base-kind",
        dest="base_kind",
        choices=[
            "scratch",
            "previous_attempt",
            "near_miss_draft",
            "matched_sibling",
        ],
        default=None,
        help="What this try was based on (default: scratch or previous_attempt)",
    )
    ap.add_argument(
        "--base-divergences",
        type=int,
        default=None,
        dest="base_divergences",
        help="Score of the base you started from, if known",
    )
    ap.add_argument(
        "--used-near-miss-draft",
        action=argparse.BooleanOptionalAction,
        default=None,
        dest="used_near_miss_draft",
        help="This try (or lineage) used a near-miss / NONMATCHING tip",
    )
    ap.add_argument(
        "--used-ghidra-draft",
        action=argparse.BooleanOptionalAction,
        default=None,
        dest="used_ghidra_draft",
        help="This try (or lineage) used a Ghidra scaffold",
    )
    ap.add_argument("--note")
    ap.add_argument(
        "--stats",
        action="store_true",
        help="After write, print attempt counts for this function",
    )
    args = ap.parse_args()

    root = configure(args.repo)
    print(f"REPO = {root}")

    name = args.func
    if not name and args.src:
        name = args.src.stem
    if not name:
        print("ERROR: need --func or --src", file=sys.stderr)
        sys.exit(2)

    module = args.module
    addr = int(args.addr, 0) if args.addr else None
    info = load_symbol(name)
    if info:
        sym_mod, sym_addr, _ = info
        module = module or sym_mod
        addr = sym_addr if addr is None else addr
    if addr is None:
        m = ADDR_IN_NAME.search(name)
        if m:
            addr = int(m.group(1), 16)
    module = module or "arm9"
    if addr is None:
        print("ERROR: need --addr or resolvable symbol", file=sys.stderr)
        sys.exit(2)

    src_rel = None
    if args.src:
        p = args.src if args.src.is_absolute() else root / args.src
        try:
            src_rel = p.resolve().relative_to(root.resolve()).as_posix()
        except ValueError:
            src_rel = str(args.src)

    try:
        row = append_attempt(
            module=module,
            addr=addr,
            name=name,
            status=args.status,
            kind=args.kind,
            model=args.model,
            reasoning=args.reasoning,
            harness=args.harness,
            author=args.author,
            divergences=args.divergences,
            prev_best_divergences=args.prev_best,
            improved_near_miss=args.improved,
            src_path=src_rel,
            label=args.label,
            note=args.note,
            session_scope=args.session_scope,
            batch_size=args.batch_size,
            attempt_id=args.attempt_id,
            parent_attempt_id=args.parent_attempt_id,
            base_kind=args.base_kind,
            base_divergences=args.base_divergences,
            used_near_miss_draft=args.used_near_miss_draft,
            used_ghidra_draft=args.used_ghidra_draft,
        )
    except ProvenanceError as e:
        print(f"ERROR: {e}", file=sys.stderr)
        sys.exit(2)

    print(
        f"Logged attempt functionId={row['functionId']} attemptId={row['attemptId']} "
        f"parent={row.get('parentAttemptId')} status={row['status']} "
        f"scope={row.get('sessionScope')} batchSize={row.get('batchSize')} "
        f"improved={row.get('improvedNearMiss')} kind={row['kind']}"
    )

    # SM64DS-shaped tip store: keep full C of the *best* near-miss only.
    if (
        args.status == "near_miss"
        and args.src is not None
        and args.divergences is not None
        and args.divergences > 0
    ):
        src_file = args.src if args.src.is_absolute() else root / args.src
        if src_file.is_file():
            size = None
            info = load_symbol(name)
            if info:
                size = info[2]
            action, tip = upsert_near_miss_tip(
                src_file=src_file,
                module=module,
                addr=addr,
                name=name,
                divergences=args.divergences,
                size=size,
                source=args.label or "log_attempt",
            )
            print(
                f"Near-miss tip {action}: nearmiss/db.jsonl "
                f"div={tip.get('divergences')} c_bytes={len(tip.get('c_source') or '')}"
            )
        else:
            print(
                f"WARNING: --src {src_file} missing; attempt logged but tip C not saved",
                file=sys.stderr,
            )
    elif args.status == "near_miss" and args.divergences and args.divergences > 0:
        print(
            "NOTE: near_miss without --src: attempt metadata only. "
            "Pass --src path.c to save tip C in nearmiss/db.jsonl (sm64ds-style).",
            file=sys.stderr,
        )

    if args.stats:
        all_rows = load_attempts(function_id=make_id(module, addr))
        print(f"stats: {attempt_stats(all_rows)}")


if __name__ == "__main__":
    main()
