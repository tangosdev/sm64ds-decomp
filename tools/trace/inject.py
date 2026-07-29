"""Phase-3 annotation injection.

Formats a function's Phase-2 annotation (from traces/annotations.jsonl) into a
compact block suitable for PREPENDING to a refine (glm_refine.py / refine_run.js)
or fan-out (sched_run.js) prompt. Additive and opt-in: the loop tooling stays
untouched; an A/B harness calls block_for(name) and prepends the result (empty
string when there is no annotation, so the control arm is a no-op).

The block deliberately leads with runtime-observed FACTS and frames them as
hints, not ground truth -- the asm remains the oracle.
"""
import json
import pathlib

REPO = pathlib.Path(__file__).resolve().parents[2]
ANNOTATIONS = REPO / "traces" / "annotations.jsonl"


def load_annotations(path=None):
    path = pathlib.Path(path) if path else ANNOTATIONS
    out = {}
    if path.is_file():
        for line in path.read_text(encoding="utf-8").splitlines():
            if line.strip():
                try:
                    r = json.loads(line)
                    out[r["name"]] = r
                except (json.JSONDecodeError, KeyError):
                    pass
    return out


def block_for(name, annotations=None):
    """Return the injection block for `name`, or '' if no annotation exists."""
    anns = annotations if annotations is not None else load_annotations()
    a = anns.get(name)
    if not a:
        return ""
    lines = [
        "=== RUNTIME TRACE HINTS (observed in a real emulator; the retail asm is",
        "still the oracle -- use these to fix the C SHAPE, esp. arg arity/types) ===",
    ]
    if a.get("signature"):
        lines.append(f"Observed signature: {a['signature']}")
    if a.get("args"):
        lines.append("Arg roles (r0-r3 at entry):")
        for reg, desc in a["args"].items():
            lines.append(f"  {reg}: {desc}")
    if a.get("semantic_name"):
        lines.append(f"Likely purpose: {a['semantic_name']}")
    if a.get("behavior"):
        lines.append(f"Behavior: {a['behavior']}")
    if a.get("draft_errors"):
        lines.append(f"Trace vs current draft: {a['draft_errors']}")
    if a.get("confidence"):
        lines.append(f"Confidence: {a['confidence']}")
    lines.append("=== END TRACE HINTS ===\n")
    return "\n".join(lines)


if __name__ == "__main__":
    import sys
    anns = load_annotations()
    if len(sys.argv) > 1:
        print(block_for(sys.argv[1], anns) or f"(no annotation for {sys.argv[1]})")
    else:
        print(f"{len(anns)} annotation(s) available:")
        for n, a in anns.items():
            print(f"  {n}: {a.get('semantic_name')} [{(a.get('confidence') or '').split(' -- ')[0]}]")
