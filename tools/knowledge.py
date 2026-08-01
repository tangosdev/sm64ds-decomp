"""Accumulated knowledge that makes the LLM tier cheaper and higher-hit: a
signature registry for every function we can describe, from two feeders.

  1. Demangling -- every _ZN... symbol encodes its class and argument types, free.
  2. The already-matched src/ -- each src/<name>.c(pp) defines that function, so its
     real signature (return type + params) is recoverable. We already paid (in LLM
     or template effort) to learn these; this captures them for reuse.

When an agent is about to crack function F, injecting the signatures of F's callees
(and F's own demangled name) removes the single biggest source of iteration:
guessing how many args a callee takes and what they are.

API:
    kb = build_kb()                 # {name: "ret name(params)"} from src/
    sig_for(name, kb) -> str|None   # demangled sig if mangled, else src sig, else None
"""
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402
import demangle as DM

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"


def _parse_def(text, name):
    """Best-effort: pull the signature of `name`'s definition from a src file."""
    esc = re.escape(name)
    # ret name(params) {   -- capture a plain return type token-run before the name
    m = re.search(r"(^|[\n;}])\s*([A-Za-z_][\w\s\*]*?)\b" + esc + r"\s*\(([^;{]*?)\)\s*\{",
                  text, re.S)
    if m:
        ret = " ".join(m.group(2).split())
        params = " ".join(m.group(3).split())
        return f"{ret} {name}({params})".strip()
    # extern "C" or unusual: just grab the param list
    m = re.search(esc + r"\s*\(([^;{]*?)\)\s*\{", text, re.S)
    if m:
        return f"{name}({' '.join(m.group(1).split())})"
    return None


def build_kb():
    kb = {}
    if not SRC.is_dir():
        return kb
    for f in SP.iter_sources():
        sig = _parse_def(f.read_text(errors="ignore"), f.stem)
        if sig:
            kb[f.stem] = sig
    return kb


def sig_for(name, kb=None):
    s = DM.signature(name)
    if s:
        return s
    if kb and name in kb:
        return kb[name]
    return None


if __name__ == "__main__":
    kb = build_kb()
    print(f"signature KB: {len(kb)} functions from src/")
    for a in sys.argv[1:]:
        print(f"  {a} -> {sig_for(a, kb)}")
    if len(sys.argv) == 1:
        for k in list(kb)[:8]:
            print(f"  {k} -> {kb[k]}")
