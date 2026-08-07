"""Give an object marker the member type its destructor relocation proves.

COMMITTED DELIBERATELY. An earlier run of this transform lived only in build/, so
the "131 of 180" it produced could not be reproduced or audited -- the same defect
notes/plan-gen-header.md 1 identifies as the corpus's original sin, where a
generator ran outside the repo and left 241 headers nobody could regenerate. The
universe a worklist covers is part of its result.

A `u8 mModelAnim; /* 0x0d4 */` followed by `u8 pad_0d5[0x63]` is the original
generator's marker for "an object starts here and I do not know what". Where the
class's own destructor calls `_ZN9ModelAnimD1Ev((char *)t + 0xd4)`, the type is
named by a relocation the ROM build checks -- not by the placeholder name.

D1, never D2, is what makes this sound: a true member of declared type T calls T's
COMPLETE-object destructor, while an inlined derived member would call the base's
D2 at the same offset. All 455 exact calls behind these markers are D1.

Batch 1 is the safe shape only: span == sizeof(T), so the marker and its pad are
replaced by one member and NO declared field disappears. Classes with a TU that
locally redefines Model/ModelAnim/Animation/ModelBase are excluded -- the header
would collide with the stand-in and the TU needs rewriting in the same commit.

    python build/type_markers.py --dry-run
    python build/type_markers.py --apply
"""
import argparse
import collections
import json
import pathlib
import re

PAD = re.compile(r"^\s*u8\s+pad_[0-9a-fA-F]+\s*\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]\s*;\s*$")
SKIPPABLE = re.compile(r"^\s*($|/\*|\*|//)")
LOCAL_DEF = re.compile(r"^struct\s+(Model|ModelAnim|BlendModelAnim|Animation|ModelBase)\s*\{",
                       re.M)


def collision_classes(results):
    """Classes with a TU that locally redefines one of the target types AND includes
    the class header. Adopting the real type there is a redefinition error, so the TU
    has to be rewritten in the same change -- out of scope for batch 1."""
    names = {r["cls"] for r in results}
    hit = set()
    for p in pathlib.Path("src").glob("*.c*"):
        t = p.read_text(errors="replace")
        if not LOCAL_DEF.search(t):
            continue
        for c in names:
            if f'#include "{c}.h"' in t:
                hit.add(c)
    return hit


def locate(path, field, offset):
    """Line number of `u8 <field>; /* <offset> */` in `path`, or None.

    marker_types.json records the line as it was when the evidence ran; earlier
    batches have since inserted lines into some of these headers, so the number is
    not reliable and the (name, offset) pair is.
    """
    want = re.compile(r"^\s*u8\s+" + re.escape(field) + r"\s*;\s*/\*\s*"
                      + re.escape(offset.lower().replace("0x", "")).rjust(1) + r"")
    hits = []
    for n, line in enumerate(pathlib.Path(path).read_text(errors="replace").splitlines(), 1):
        m = re.match(r"^\s*u8\s+" + re.escape(field) + r"\s*;\s*/\*\s*(0x[0-9a-fA-F]+)", line)
        if m and int(m.group(1), 16) == int(offset, 16):
            hits.append(n)
    return hits[0] if len(hits) == 1 else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--findings", default="build/marker_evidence.json")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--only", default=None,
                    help="comma-separated Class or Class.field. Restricts the batch to "
                         "these, and turns OFF the local-stand-in filter -- naming one "
                         "explicitly means its TUs are being fixed in the same change.")
    args = ap.parse_args()

    only = set(args.only.split(",")) if args.only else None
    results = json.load(open(args.findings))["results"]
    confident = [r for r in results if r["verdict"] == "confident"]
    collide = set() if only else collision_classes(confident)

    picked, skipped = [], collections.Counter()
    for r in confident:
        if only is not None and r["cls"] not in only \
                and f"{r['cls']}.{r['field']}" not in only:
            skipped["not in --only"] += 1
            continue
        if r["cls"] in collide:
            skipped["class has a TU redefining the type locally"] += 1
            continue
        if r["span"] is None:
            skipped["trailing marker, extent unknown"] += 1
            continue
        if r["span"] != r["proposed_size"]:
            skipped["span != sizeof(type) -- batch 2/3"] += 1
            continue
        picked.append(r)

    by_file = collections.defaultdict(list)
    for r in picked:
        path, line = r["where"].rsplit(":", 1)
        by_file[path].append((int(line), r))

    changed = 0
    for path, items in sorted(by_file.items()):
        p = pathlib.Path(path)
        lines = p.read_text(errors="replace").splitlines()
        for lineno, r in sorted(items, reverse=True):
            i = lineno - 1
            m = re.match(r"^(\s*)u8(\s+)" + re.escape(r["field"]) + r"\s*;(.*)$", lines[i])
            if not m:
                print(f"  !! {path}:{lineno} not a bare marker: {lines[i]!r}")
                continue
            indent, gap, tail = m.groups()
            typ = r["proposed"]
            gap = " " * max(1, len("u8") + len(gap) - len(typ))
            ev = next((e for e in r["evidence"] if e.get("kind") == "exact"), None)
            why = (f"{ev['callee']} at +{r['offset']}" if ev else "member structor")
            lines[i] = (f"{indent}/* {typ} member, named by {why} -- a relocation the "
                        f"ROM build checks.\n"
                        f"{indent}   D1 and not D2, so it is this type and not an "
                        f"inlined base. Was a u8 marker. */\n"
                        f"{indent}{typ}{gap}{r['field']};{tail}")
            # the pad the marker stood on is exactly the rest of the object
            j, in_comment = i + 1, False
            while j < len(lines):
                ln = lines[j]
                if in_comment:
                    if "*/" in ln:
                        in_comment = False
                    j += 1
                    continue
                if SKIPPABLE.match(ln):
                    if "/*" in ln and "*/" not in ln:
                        in_comment = True
                    j += 1
                    continue
                break
            if j < len(lines) and PAD.match(lines[j]):
                del lines[j]
            else:
                print(f"  !! {path}:{lineno} expected a pad after the marker")
                continue
            changed += 1
        # a member of type T needs T's definition, not a forward declaration
        text = "\n".join(lines)
        want = []
        for _, r in sorted(items):
            inc = f'#include "{r["proposed"]}.h"'
            if inc not in text and inc not in want:
                want.append(inc)
        if want:
            first = min(n for n, _ in items) - 1
            heads = [n for n, ln in enumerate(lines[:first])
                     if ln.startswith("#include ")]
            k = heads[-1] if heads else None
            if k is None:
                print(f"  !! {path}: no #include to anchor {want}")
            else:
                lines[k + 1:k + 1] = want
        if args.apply:
            p.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")

    kinds = collections.Counter(r["proposed"] for r in picked)
    print(f"{'applied' if args.apply else 'would change'}: {changed} markers in "
          f"{len(by_file)} headers   {dict(kinds)}")
    print(f"\nheld back ({sum(skipped.values())}):")
    for k, v in skipped.most_common():
        print(f"  {v:>3}  {k}")
    if collide:
        print(f"\ncollision classes ({len(collide)}): {', '.join(sorted(collide))}")


if __name__ == "__main__":
    main()
