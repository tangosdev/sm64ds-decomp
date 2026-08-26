#!/usr/bin/env python3
"""`tubuild.py create`, plus an in-memory normalizer for legacy files whose
function DEFINITION sits inside an `extern "C" { }` block.

tubuild's splitter matches `extern "C" {` on its own line and consumes to the
balancing brace, appending every interior line to `externs`.  When the definition
is inside that block it is consumed as a declaration, `body_start` is never set,
and `create` aborts with "scanned to end of file without finding a function body".
64 of the 173 safe-pool TUs hit this (16 of the 100 in Tier 1).

This reads `src/` only and writes only what tubuild writes: the normalization is
done on a copy of the text, and the assemble + build_manifest_entry + manifest
upsert path is delegated back to tubuild so the shadow file and the manifest
schema stay the tool's own.

    python tools/tu_create.py <module>/<tail> [--force] [--manifest PATH]

Known limitation, and it is tubuild's not ours: a definition whose return type is
spelled `struct X*` / `enum X` / `class X*` is misparsed the same way, because
the splitter's first-word test sees `struct` in _DECL_KEYWORDS and eats the whole
function.  Normalizing does not help there -- hand-assemble those.
"""
import re
import sys
import argparse
import importlib.util
import pathlib

REPO = pathlib.Path(__file__).resolve().parent.parent
_spec = importlib.util.spec_from_file_location("tubuild", REPO / "tools" / "tubuild.py")
tb = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(tb)

_BLK = re.compile(r'^\s*extern\s+"C"\s*\{\s*$')


def normalize(text):
    """Close an `extern "C"` block before the first definition inside it, and give
    that definition its own `extern "C"` prefix.  Semantics-preserving: the
    definition keeps C linkage, the declarations around it keep theirs."""
    lines = text.splitlines()
    out, i, n = [], 0, len(lines)
    while i < n:
        if _BLK.match(lines[i]):
            depth, k, body = 1, i + 1, None
            while k < n and depth > 0:
                d0 = depth
                depth += lines[k].count("{") - lines[k].count("}")
                if d0 == 1 and body is None and lines[k].count("{") > lines[k].count("}"):
                    body = k
                k += 1
            close = k - 1
            if body is None:                       # declaration-only block: untouched
                out.extend(lines[i:close + 1])
                i = close + 1
                continue
            s = body                               # walk back onto the signature line
            while s > i + 1:
                prev = lines[s - 1].strip()
                if (not prev or prev.endswith(";") or prev.endswith("}")
                        or prev.startswith("//") or prev.startswith("/*")
                        or prev.endswith("*/")):
                    break
                s -= 1
            out.append(lines[i])
            out.extend(lines[i + 1:s])
            out.append("}")
            out.append('extern "C" ' + lines[s].lstrip())
            out.extend(lines[s + 1:close])
            i = close + 1
            continue
        out.append(lines[i])
        i += 1
    return "\n".join(out) + "\n"


_orig_split = tb.split_legacy_source


def _patched_split(text):
    r = _orig_split(text)
    if not r["error"]:
        return r
    r2 = _orig_split(normalize(text))
    if not r2["error"]:
        r2["notes"] = list(r2.get("notes", [])) + [
            '// tu_create.py: the definition was inside an extern "C" block; the block '
            'was closed before it and the definition given explicit C linkage']
    return r2


tb.split_legacy_source = _patched_split


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("id", help="<module>/<tail>, as `tubuild.py inspect` prints it")
    ap.add_argument("--force", action="store_true")
    ap.add_argument("--manifest", default=None)
    a = ap.parse_args()
    a.manifest = pathlib.Path(a.manifest) if a.manifest else None
    tb.cmd_create(a)


if __name__ == "__main__":
    sys.exit(main())
