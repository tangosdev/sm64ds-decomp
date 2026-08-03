"""Call MeshColliderBase's methods by name instead of through their mangled symbols.

include/MeshColliderBase.h declares the real class, but 177 call sites still reach it
through hand-written externs of the mangled names:

    extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
    ...
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);

becomes

    #include "MeshColliderBase.h"
    ...
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled())
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();

Two shapes, because the class has two kinds of member here.

  * The instance methods -- Enable/Disable/IsEnabled -- are called with the collider
    as the first argument, which is the `this` the mangled name implies.

  * UpdatePosAndAngs and friends are STATIC, and call sites never call them: they take
    the address and hand it to func_020393d4 to be stored as a BeforeClsn callback.
    Those are declared `extern int NAME;` -- as a variable, not a function -- and used
    as `&NAME`, so they become `&MeshColliderBase::UpdatePosWithTransform`.

This is byte-neutral: it changes which spelling produces the symbol, not the symbol.
Every file is verified after the edit and reverted if it stops reproducing.

Supersedes the hand-written attempt in #1033, which conflicted with #1032 after both
edited the same lines -- and covered 100 files where the tree now has 177.

Usage:
    python tools/meshclsn_methods.py                # report only
    python tools/meshclsn_methods.py --apply
"""
import argparse
import concurrent.futures
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import match as M          # noqa: E402
import modules as MOD      # noqa: E402
from enroll import candidates  # noqa: E402

# mangled symbol -> method name, for the instance methods.
INSTANCE = {
    "_ZN16MeshColliderBase9IsEnabledEv": "IsEnabled",
    "_ZN16MeshColliderBase7DisableEv": "Disable",
    "_ZN16MeshColliderBase6EnableEP5Actor": "Enable",
}
# mangled symbol -> method name, for the statics used as callback addresses.
STATIC = {
    "_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_":
        "UpdatePosAndAngs",
    "_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_":
        "UpdatePosWithVelocity",
    "_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_":
        "UpdatePosWithTransform",
    "_ZN16MeshColliderBase25UpdateAngsWithAngularVelYERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_":
        "UpdateAngsWithAngularVelY",
}


def split_args(s):
    """Split a call's argument text on top-level commas."""
    out, depth, cur = [], 0, ""
    for ch in s:
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def call_text(text, sym):
    """Find `sym(...)` and return (start, end, args) for each, innermost-safe."""
    hits = []
    for m in re.finditer(r"\b" + re.escape(sym) + r"\s*\(", text):
        i = m.end() - 1
        depth = 0
        for j in range(i, len(text)):
            if text[j] == "(":
                depth += 1
            elif text[j] == ")":
                depth -= 1
                if depth == 0:
                    hits.append((m.start(), j + 1, text[i + 1:j]))
                    break
    return hits


def transform(text):
    """Return the rewritten text, or None if nothing applies."""
    changed = False

    # Statics first: they are declared as variables and used via &NAME.
    for sym, method in STATIC.items():
        if sym not in text:
            continue
        # Declared either as a variable (`extern T SYM;`) or as a nullary function
        # (`extern T SYM();`), and inside an extern "C" block the `extern` keyword is
        # usually absent. All three shapes have to go, or the rewritten name lands in
        # a declaration and mwccarm rejects it as an illegal storage class.
        text = re.sub(r"^[ \t]*(?:extern\s+)?[A-Za-z_][^;{}()]*\b" + re.escape(sym)
                      + r"\s*(?:\([^;]*\))?\s*;[ \t]*\n?", "", text, flags=re.M)
        text = re.sub(r"\b" + re.escape(sym) + r"\b",
                      f"MeshColliderBase::{method}", text)
        changed = True

    # Instance methods: first argument is the `this` the mangled name implies.
    for sym, method in INSTANCE.items():
        if sym not in text:
            continue
        # Same shapes as the statics: with or without `extern`, since inside an
        # extern "C" block the keyword is usually omitted. Leaving one behind clashes
        # with the header's declaration ("was declared as ... now declared as
        # extern "C" ...") and the file stops compiling.
        text = re.sub(r"^[ \t]*(?:extern\s+)?[A-Za-z_][^;{}()]*\b" + re.escape(sym)
                      + r"\s*\([^;]*\)\s*;[ \t]*\n?", "", text, flags=re.M)
        while True:
            hits = call_text(text, sym)
            if not hits:
                break
            start, end, argtext = hits[0]
            args = split_args(argtext)
            if not args:
                break
            recv, rest = args[0], args[1:]
            call = f"((MeshColliderBase *)({recv}))->{method}({', '.join(rest)})"
            text = text[:start] + call + text[end:]
            changed = True

    if not changed:
        return None
    # An emptied `extern "C" { }` block is noise; drop it.
    text = re.sub(r'extern "C" \{\s*\n\}\n', "", text)
    if '#include "MeshColliderBase.h"' not in text:
        m = re.search(r'^#include "[^"]+"\s*$', text, re.M)
        if m:
            text = text[:m.end()] + '\n#include "MeshColliderBase.h"' + text[m.end():]
        else:
            text = '#include "MeshColliderBase.h"\n' + text
    return text


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=10)
    args = ap.parse_args()

    info = {}
    for (d, name, rel, addr, size, sec) in candidates()[0]:
        info[rel] = (name, addr, size, d)
    mods = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}

    syms = list(INSTANCE) + list(STATIC)
    targets = []
    for f in sorted((REPO / "src").rglob("*")):
        if not f.is_file() or f.suffix not in (".c", ".cpp"):
            continue
        t = f.read_text(encoding="utf-8", errors="ignore")
        if any(s in t for s in syms) and transform(t):
            targets.append(f)

    print(f"files calling MeshColliderBase through mangled names: {len(targets)}")
    if not args.apply:
        print("\n(report only -- pass --apply to rewrite and byte-verify)")
        return 0

    def one(f):
        rel = f.relative_to(REPO).as_posix()
        original = f.read_text(encoding="utf-8", errors="ignore")
        new = transform(original)
        if new is None:
            return rel, "unchanged"
        f.write_text(new, encoding="utf-8", newline="\n")
        if rel not in info:
            return rel, "rewritten (not enrolled, unverified)"
        name, addr, size, d = info[rel]
        label = d.relative_to(REPO / "config").as_posix()
        label = "arm9" if label == "arm9" else label.split("/")[-1]
        flags = M.DEFAULT_FLAGS
        if new.startswith("//cpp") or f.suffix == ".cpp":
            flags = flags.replace("-lang c99", "-lang c++")
        tgt = (M.target_bytes(addr, size) if label == "arm9"
               else M.target_bytes(addr, size, mods[label]["bin"], mods[label]["base"]))
        for v in M.SWEEP:
            obj = M.compile_c(f, v, flags)
            if obj is None:
                continue
            code, rl = M.extract_func(obj, name)
            if code is None:
                continue
            ok, _ = M.compare(tgt, code, rl, verbose=False)
            if ok:
                return rel, "rewritten"
        f.write_text(original, encoding="utf-8", newline="\n")
        return rel, "reverted (stopped matching)"

    counts = {}
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, verdict in ex.map(one, targets):
            counts[verdict] = counts.get(verdict, 0) + 1
            if verdict.startswith("reverted"):
                print(f"  {verdict}: {rel}")
    print()
    for k, v in sorted(counts.items(), key=lambda kv: -kv[1]):
        print(f"  {v:5d}  {k}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
