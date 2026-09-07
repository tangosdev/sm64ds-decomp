"""Read the shared mwccarm lever catalogue (notes/levers.jsonl) and select the levers that
apply to a given target.

The catalogue is a distilled INDEX. The narrative notes (notes/mwccarm-codegen.md and the
sibling repos' equivalents) remain the evidence; each entry's `source` points back into them.

Why selection rather than a dump: the catalogue was derived almost entirely on an all-ARM
title, and some entries name ARM-only machinery (conditional execution, ARM immediate-offset
ranges). Handing those to a model working a Thumb function presents guesses as facts. So a
caller asks for the levers matching its arch, and entries marked `inapplicable` for that arch
are dropped rather than silently included.

Cross-repo: another decomp in this family reads this same file out of a sibling checkout,
    python tools/levers.py --repo ../sm64ds-decomp --arch thumb --format prompt
so a lever proven in one project is available to all of them instead of living in one repo's
notes. jsonl + stdlib json on purpose - a reader must not need pyyaml installed.

Usage:
  python tools/levers.py --arch thumb                 # human-readable table
  python tools/levers.py --arch arm --format prompt   # the block a driver injects
  python tools/levers.py --match "lsl #0x10"          # filter by symptom text
"""
import argparse
import json
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
CATALOGUE = "notes/levers.jsonl"


def load(repo=None):
    """Every lever in the catalogue. `repo` points at another checkout that carries one."""
    root = pathlib.Path(repo).resolve() if repo else REPO
    path = root / CATALOGUE
    if not path.is_file():
        sys.exit(f"no lever catalogue at {path}")
    out = []
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line:
            continue
        try:
            rec = json.loads(line)
        except ValueError as e:
            sys.exit(f"{path}: unparseable line: {e}")
        if "_comment" in rec:  # the header line documents the schema; not a lever
            continue
        out.append(rec)
    return out


def select(levers, arch=None, match=None, include_untested=True):
    """Levers applicable to `arch`, optionally filtered by text in the symptom or lever.

    An entry is dropped for an arch when it does not list that arch, or when it explicitly
    marks that arch `inapplicable` - the ARM-only mechanisms that have no Thumb equivalent.
    `include_untested=False` keeps only what has been PROVEN on the requested arch, which for
    Thumb currently means almost nothing; that is the honest state, not a bug.
    """
    out = []
    for lv in levers:
        if arch:
            if arch not in (lv.get("arch") or []):
                continue
            if lv.get(arch) == "inapplicable":
                continue
            if not include_untested and lv.get(arch) not in ("proven", None):
                continue
        if match:
            hay = f"{lv.get('symptom','')} {lv.get('lever','')} {lv.get('id','')}".lower()
            if match.lower() not in hay:
                continue
        out.append(lv)
    return out


def render_prompt(levers, arch=None):
    """The block a driver injects. Untested-on-this-arch entries are marked inline so the
    model weights them accordingly rather than treating every line as established fact."""
    if not levers:
        return ""
    head = "STRUCTURAL LEVERS (pick by what the diff shows"
    head += f"; selected for {arch}):" if arch else "):"
    lines = [head]
    for lv in levers:
        flag = ""
        if arch and lv.get(arch) == "untested":
            flag = " [unproven on this ISA - derived on ARM, try it but do not trust it]"
        lines.append(f"- {lv['symptom']}{flag}\n  -> {lv['lever']}")
    return "\n".join(lines)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", default=None, help="read another checkout's catalogue")
    ap.add_argument("--arch", default=None, choices=["arm", "thumb"])
    ap.add_argument("--match", default=None, help="only levers whose symptom/lever mentions this")
    ap.add_argument("--proven-only", action="store_true",
                    help="drop entries not proven on --arch (for Thumb this is nearly everything)")
    ap.add_argument("--format", default="table", choices=["table", "prompt", "json"])
    args = ap.parse_args()

    levers = select(load(args.repo), args.arch, args.match, not args.proven_only)
    if args.format == "prompt":
        print(render_prompt(levers, args.arch))
    elif args.format == "json":
        print(json.dumps(levers, indent=2))
    else:
        print(f"{len(levers)} lever(s)" + (f" for {args.arch}" if args.arch else ""))
        for lv in levers:
            status = f" [{args.arch}: {lv.get(args.arch)}]" if args.arch else ""
            print(f"\n  {lv['id']}{status}   ({lv.get('confidence','?')}, {lv.get('source','?')})")
            print(f"    when : {lv['symptom']}")
            print(f"    try  : {lv['lever']}")


if __name__ == "__main__":
    main()
