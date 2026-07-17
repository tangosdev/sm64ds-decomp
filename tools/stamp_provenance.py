#!/usr/bin/env python3
"""Stamp match provenance (EP-style how-record; does not replace tools/bank.py fan-out): require provenance, optionally verify, record ledger.

A successful match must permanently record how it was matched:

  AI:    --kind ai --model … --reasoning … --harness …
  Human: --kind human [--by …] [--note …]

Missing/incomplete AI provenance aborts (exit 2). This is the only supported
banking path for provenance; do not hand-edit src/ alone without a ledger row.

Repo root is discovered via --repo, DECOMP_ROOT / MATCH_REPO, cwd walk, then
__file__ walk — so copies of this script under /tmp still work if you pass
--repo or run from the decomp tree.

Examples:

  # Human match already in src/ (stamp provenance only)
  python tools/bank.py --src src/arm9/func_02001b44.c \\
      --kind human --by lunavyqo

  # AI match: who = git first-adder (like sm64ds); model/harness = how only
  python tools/bank.py --src src/arm9/func_0200abcd.c --kind ai \\
      --model "Grok 4.5" --reasoning high --harness "Grok Build" --no-verify

  # From a tools copy outside the tree:
  python /tmp/ep-bank-tools/bank.py --repo /path/to/electroplankton-decomp \\
      --src src/arm9/func_0200abcd.c --kind ai --author lunavyqo \\
      --model grok-4.5 --reasoning high --harness grok-build --no-verify

  # Verify + promote scratch → src + ledger
  python tools/bank.py --c scratch/foo.c --func func_0200abcd \\
      --kind ai --author lunavyqo --model claude-opus-4 --reasoning high \\
      --harness cursor-agent --promote
"""
from __future__ import annotations

import argparse
import pathlib
import re
import shutil
import subprocess
import sys

# Import sibling module whether we live in <repo>/tools or a temp copy.
_TOOLS_DIR = pathlib.Path(__file__).resolve().parent
if str(_TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(_TOOLS_DIR))

from match_attempts import append_attempt, has_attempt_with_status  # noqa: E402
from match_provenance import (  # noqa: E402
    ProvenanceError,
    append_ledger_row,
    configure,
    is_agent_credit,
    load_ledger,
    make_id,
    provenance_from_cli_args,
    repo as get_repo,
    resolve_credit_author,
    TOKEN_HELP,
)
from nearmiss_db import locked, load_db, save_db  # noqa: E402


FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)"
)
ADDR_IN_NAME = re.compile(r"func_([0-9a-fA-F]{6,8})$", re.I)


def load_symbol(name: str) -> tuple[str, int, int] | None:
    """Return (module, addr, size) for symbol name, or None."""
    config = get_repo() / "config"
    for sym in config.rglob("symbols.txt"):
        rel = sym.parent.relative_to(config).as_posix()
        if rel in ("arm9", "arm7"):
            module = rel
        else:
            m = re.fullmatch(r"arm9/overlays/(ov\d+)", rel)
            if not m:
                continue
            module = m.group(1)
        for line in sym.read_text(errors="ignore").splitlines():
            m = FUNC_RE.match(line)
            if not m or m.group(1) != name:
                continue
            return module, int(m.group(3), 16), int(m.group(2), 16)
    return None


def resolve_from_src(src: pathlib.Path) -> tuple[str, str, int, int]:
    """name, module, addr, size from src path + symbols."""
    name = src.stem
    info = load_symbol(name)
    if info:
        module, addr, size = info
        return name, module, addr, size
    m = ADDR_IN_NAME.search(name)
    if not m:
        raise SystemExit(f"cannot resolve symbol {name!r} from {src}")
    addr = int(m.group(1), 16)
    parts = src.resolve().relative_to(get_repo().resolve()).parts
    module = "arm9"
    if len(parts) >= 2 and parts[0] == "src":
        if parts[1] in ("arm9", "arm7") or parts[1].startswith("ov"):
            module = parts[1]
    return name, module, addr, 0


def run_match(
    c_path: pathlib.Path,
    func: str,
    addr: int,
    size: int,
    module: str,
    version: str,
) -> None:
    if size <= 0:
        raise SystemExit("--size required for verify (symbol not found or size 0)")
    match_py = get_repo() / "tools" / "match.py"
    cmd = [
        sys.executable,
        str(match_py),
        "--c",
        str(c_path),
        "--func",
        func,
        "--addr",
        hex(addr),
        "--size",
        hex(size),
        "--module",
        module if module in ("arm9", "arm7") else "arm9",
        "--version",
        version,
        "--brief",
    ]
    r = subprocess.run(cmd, cwd=get_repo(), capture_output=True, text=True, timeout=180)
    out = (r.stdout or "") + (r.stderr or "")
    if r.returncode != 0 or "MATCH" not in out:
        print(out, file=sys.stderr)
        raise SystemExit("match verification failed — not banking")
    print("\n".join(out.strip().splitlines()[-5:]))


def default_src_path(module: str, name: str, ext: str = "c") -> pathlib.Path:
    return get_repo() / "src" / module / f"{name}.{ext}"


def resolve_input_path(p: pathlib.Path) -> pathlib.Path:
    """Resolve --src / --c relative to repo (not the script location)."""
    if p.is_absolute():
        return p
    return get_repo() / p


def main() -> None:
    ap = argparse.ArgumentParser(
        description="Bank a match with required provenance",
        epilog=f"AI token form: {TOKEN_HELP}",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    ap.add_argument(
        "--repo",
        type=pathlib.Path,
        default=None,
        help="Decomp root (or set DECOMP_ROOT / MATCH_REPO). Required if tools run from /tmp.",
    )
    ap.add_argument("--src", type=pathlib.Path, help="Already-promoted source under src/")
    ap.add_argument("--c", type=pathlib.Path, help="Candidate C to verify / promote")
    ap.add_argument("--func", help="Function symbol name (default: stem of --c/--src)")
    ap.add_argument("--addr", help="Address (hex); default from symbols.txt")
    ap.add_argument("--size", help="Size (hex); default from symbols.txt")
    ap.add_argument("--module", default=None, help="Module (default arm9 / from symbols)")
    ap.add_argument("--version", default="1.2/sp2p3")
    ap.add_argument("--kind", required=True, choices=("human", "ai"))
    ap.add_argument(
        "--model",
        help="AI model id (required for --kind ai). Spaces ok; slugified (Grok 4.5 → grok-4.5)",
    )
    ap.add_argument("--reasoning", help="AI reasoning/effort level (required for ai)")
    ap.add_argument(
        "--harness",
        help="AI harness/pipeline id (required for ai). Spaces ok; slugified (Grok Build → grok-build)",
    )
    ap.add_argument(
        "--author",
        "--by",
        dest="author",
        help=(
            "Optional override for classic credit (GitHub login). "
            "Default: git first-adder of src/ (sm64ds-style), never agent names like grok"
        ),
    )
    ap.add_argument("--note", help="Optional note on human how-record only")
    ap.add_argument(
        "--promote",
        action="store_true",
        help="Copy --c into src/<module>/<name>.c after verify",
    )
    ap.add_argument(
        "--no-verify",
        action="store_true",
        help="Skip match.py (operator asserts already matched)",
    )
    ap.add_argument(
        "--force",
        action="store_true",
        help="Append ledger even if id already has provenance",
    )
    ap.add_argument(
        "--dry-run",
        action="store_true",
        help="Print resolved repo, paths, and provenance; do not write ledger",
    )
    ap.add_argument(
        "--session-scope",
        choices=("focused", "batch"),
        required=True,
        help="REQUIRED: solo (focused) or multi-function (batch) matching session",
    )
    ap.add_argument(
        "--batch-size",
        type=int,
        default=None,
        dest="batch_size",
        help="Functions in the session (1 if focused; required ≥2 if batch)",
    )
    args = ap.parse_args()

    try:
        root = configure(args.repo)
    except SystemExit:
        raise
    print(f"REPO = {root}")

    try:
        prov = provenance_from_cli_args(
            kind=args.kind,
            model=args.model,
            reasoning=args.reasoning,
            harness=args.harness,
            note=args.note,
        )
    except ProvenanceError as e:
        print(f"ERROR: provenance invalid: {e}", file=sys.stderr)
        print(
            "AI matches require: --kind ai --model … --reasoning … --harness …",
            file=sys.stderr,
        )
        print(f"Hint: {TOKEN_HELP}", file=sys.stderr)
        sys.exit(2)

    if not args.src and not args.c:
        print("ERROR: provide --src and/or --c", file=sys.stderr)
        sys.exit(2)

    src_path: pathlib.Path | None = None
    c_path: pathlib.Path | None = None
    if args.src:
        src_path = resolve_input_path(args.src)
        if not src_path.is_file():
            print(f"ERROR: --src not found: {src_path}", file=sys.stderr)
            sys.exit(2)
    if args.c:
        c_path = resolve_input_path(args.c)
        if not c_path.is_file():
            print(f"ERROR: --c not found: {c_path}", file=sys.stderr)
            sys.exit(2)

    name = args.func
    if not name:
        name = (src_path or c_path).stem  # type: ignore[union-attr]

    module = args.module
    addr = int(args.addr, 0) if args.addr else None
    size = int(args.size, 0) if args.size else None

    info = load_symbol(name)
    if info:
        sym_mod, sym_addr, sym_size = info
        module = module or sym_mod
        addr = sym_addr if addr is None else addr
        size = sym_size if size is None else size
    if src_path and (module is None or addr is None):
        n2, m2, a2, s2 = resolve_from_src(src_path)
        name = name or n2
        module = module or m2
        addr = a2 if addr is None else addr
        size = s2 if size is None else size

    if not module or addr is None:
        print("ERROR: need --module and --addr (or resolvable symbol)", file=sys.stderr)
        sys.exit(2)

    rid = make_id(module, addr)

    def _rel_src(p: pathlib.Path | None) -> str | None:
        if not p or not p.is_file():
            return None
        try:
            return p.resolve().relative_to(get_repo().resolve()).as_posix()
        except ValueError:
            return None

    # Resolve credit early for dry-run (src may only exist after --promote).
    credit_preview = resolve_credit_author(_rel_src(src_path), explicit=args.author)
    if args.author and is_agent_credit(args.author):
        print(
            f"WARNING: --author {args.author!r} looks like an agent/harness name; "
            f"using git credit {credit_preview!r} instead (sm64ds-style who).",
            file=sys.stderr,
        )

    if args.dry_run:
        print(f"dry-run id={rid} name={name} module={module} addr=0x{addr:08x}")
        print(f"dry-run src={src_path} c={c_path}")
        print(f"dry-run author={credit_preview!r}  (git who; not matchProvenance)")
        print(f"dry-run matchProvenance={json_dumps(prov)}  (how only)")
        print("dry-run: no ledger write")
        return

    existing = load_ledger()
    if rid in existing and not args.force:
        print(
            f"ERROR: {rid} already has provenance "
            f"({existing[rid]['matchProvenance']}). Use --force to append.",
            file=sys.stderr,
        )
        sys.exit(2)

    verify_c = c_path or src_path
    assert verify_c is not None
    if not args.no_verify:
        if size is None or size <= 0:
            print("ERROR: --size required for verify", file=sys.stderr)
            sys.exit(2)
        print(f"Verifying {name} @ {module}:0x{addr:08x} size 0x{size:x} …")
        run_match(verify_c, name, addr, size, module, args.version)
    else:
        print("Skipping verify (--no-verify)")

    if args.promote:
        if not c_path:
            print("ERROR: --promote needs --c", file=sys.stderr)
            sys.exit(2)
        ext = c_path.suffix.lstrip(".") or "c"
        dest = default_src_path(module, name, ext)
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(c_path, dest)
        print(f"Promoted → {dest.relative_to(get_repo())}")
        src_rel = dest.relative_to(get_repo()).as_posix()
    elif src_path:
        try:
            src_rel = src_path.resolve().relative_to(get_repo().resolve()).as_posix()
        except ValueError:
            src_rel = str(src_path)
    else:
        dest = c_path  # type: ignore[assignment]
        try:
            src_rel = dest.resolve().relative_to(get_repo().resolve()).as_posix()
        except ValueError:
            src_rel = None
        print(
            "NOTE: not promoting to src/ (pass --promote). "
            "Ledger still records provenance.",
        )

    if src_rel and not src_rel.startswith("src/"):
        print(
            f"WARNING: path {src_rel!r} is not under src/ — "
            "atlas only counts files under src/ as matched.",
            file=sys.stderr,
        )

    credit = resolve_credit_author(src_rel, explicit=args.author)
    if not credit:
        print(
            "WARNING: could not resolve git credit author "
            "(commit the src file or set git user.name/email). "
            "Ledger how-record will still write; who may be empty.",
            file=sys.stderr,
        )
    row = append_ledger_row(
        module=module,
        addr=addr,
        name=name,
        provenance=prov,
        src_path=src_rel,
        author=credit,  # already git-resolved; never agent slug
    )
    who = row.get("author") or "(no author)"
    print(
        f"Banked {row['id']}: author={who} how={json_dumps(row['matchProvenance'])}"
    )
    # Banking is NOT a new try. One prompt/session try should already be in
    # match_attempts.jsonl (log_attempt / MATCH_RESULT). Only append a matched
    # row if this function has never been logged as matched (legacy bank-only flow).
    fid = make_id(module, addr)
    if has_attempt_with_status(fid, "matched"):
        print(
            "Skip attempt log: matched try already recorded for this function "
            "(bank is not a separate try)."
        )
    else:
        try:
            append_attempt(
                module=module,
                addr=addr,
                name=name,
                status="matched",
                kind=args.kind,
                model=args.model,
                reasoning=args.reasoning,
                harness=args.harness,
                author=credit,
                src_path=src_rel,
                note="banked",
                session_scope=args.session_scope,
                batch_size=args.batch_size,
            )
            print(
                "Logged attempt status=matched → config/match_attempts.jsonl "
                "(no prior matched try; bank-only path)"
            )
        except ProvenanceError as e:
            print(f"WARNING: banked but failed to log attempt: {e}", file=sys.stderr)
    # Matched functions leave the near-miss tip store (sm64ds prune-matched).
    # Drop tip from SM64DS near-miss DB once matched/stamped.
    with locked():
        db = load_db()
        key = (module, int(addr))
        if key in db or any(
            int(r.get("addr", -1)) == int(addr) and r.get("module") == module
            for r in db.values()
        ):
            # keys may be (mod, addr) tuples after load_db
            db.pop(key, None)
            for k in list(db.keys()):
                r = db[k]
                if r.get("module") == module and int(r.get("addr", -1)) == int(addr):
                    db.pop(k, None)
            save_db(db)
            print(f"Pruned near-miss tip for {module}:{addr:#x} from nearmiss/db.jsonl")
    print("OK — regenerate atlas with: python tools/chaos_db_ci.py")


def json_dumps(obj: dict) -> str:
    import json

    return json.dumps(obj, ensure_ascii=False)


if __name__ == "__main__":
    main()
