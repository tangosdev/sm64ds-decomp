#!/usr/bin/env python3
"""Enroll a text-verified manifest TU into the production ROM build.

Promotion changes NOTHING about codegen. Text-verification already proved every
function's bytes and relocations from a single compile of the reconstructed source;
promotion only changes which object the linker takes them from -- one TU instead of
N per-function objects. That is the entire point: the cartridge's own build emitted
each class's constructors and destructors from one translation unit, and while the
tree carries a file per mangled symbol, every structor has to be hand-placed under a
``// @symbol`` marker. Promoting the TU hands that job back to the compiler.

The mechanical steps, all of which this performs and none of which it guesses at:

* replace the entry's per-function ``delinks.txt`` entries with one ``complete``
  entry spanning the manifest's sections;
* ``git mv`` the ``src_tu/`` source to its ``promoted_source`` path (R100, so the
  file's own credit follows) and ``git rm`` every ``legacy_source``;
* rewrite the manifest entry to ``status: promoted`` with ``source`` at the
  production path, matching the entries already enrolled;
* add one ``attribution.json`` override per absorbed symbol, so a many-to-one
  consolidation reads as "consolidated with credit intact" instead of N lost.

It deliberately does NOT compile anything. The proof that a promotion is sound is
``rombuild.py`` reporting 106/106 with ``mismatching: 0`` afterwards, and running it
once over a batch is far cheaper than once per entry. Refusals here are only about
facts that can be checked without a compiler: a missing file, a delinks entry that
does not look the way the manifest says it does, a section span that does not cover
the functions claimed inside it.
"""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import tu_manifest as TUM  # noqa: E402

CONFIG = REPO / "config"


class PromoteError(RuntimeError):
    """A promotion was refused on a fact that needs no compiler to check."""


def delinks_path(module):
    """The delinks.txt for a manifest module name ("arm9" or "ovNNN")."""
    if module in ("arm9", "main"):
        return CONFIG / "arm9" / "delinks.txt"
    return CONFIG / "arm9" / "overlays" / module / "delinks.txt"


def _entry_re(source):
    """One whole delinks entry: the path line plus its indented body."""
    return re.compile(re.escape(source) + r":\n(?:[ \t]+[^\n]*\n)+\n?")


def plan(entry):
    """Everything the promotion will touch, or a PromoteError explaining why not."""
    ident = entry.get("id", "<unknown>")
    if entry.get("status") == "promoted":
        raise PromoteError(f"{ident}: already promoted")
    src = entry.get("source", "")
    dest = entry.get("promoted_source", "")
    if not src.startswith("src_tu/") or not dest.startswith("src/"):
        raise PromoteError(f"{ident}: expected src_tu/ source and src/ promoted_source, "
                           f"got {src!r} -> {dest!r}")
    # MEASURED, not defensive: `ChillBully+daIDonketu_c.cpp` linked fine as a src_tu
    # compile and then broke the whole ROM at the link --
    #   mwldarm.exe: File not found: ChillBully
    #   mwldarm.exe: Expecting: (
    # because dsd writes bare, unquoted object names into the LCF and mwldarm's parser
    # treats `+` as punctuation. src_tu/ names a multi-class TU with `+` and nothing
    # ever links those, so the convention is safe exactly until promotion. No promoted
    # source has ever carried one; keep it that way here, where it costs a second.
    if not re.fullmatch(r"[A-Za-z0-9_./-]+", dest):
        raise PromoteError(f"{ident}: {dest!r} has a character the linker command file "
                           f"cannot carry unquoted; rename the file before promoting")
    if not (REPO / src).is_file():
        raise PromoteError(f"{ident}: {src} is not on disk")
    if (REPO / dest).exists():
        raise PromoteError(f"{ident}: {dest} already exists")

    sections = [s for s in entry.get("sections", []) if s.get("name") == ".text"]
    if not sections:
        raise PromoteError(f"{ident}: no .text section in the manifest")
    if entry.get("data") or entry.get("bss"):
        # Production isolation zeroes an object's data and rebinds the symbols to the
        # cartridge's addresses; a TU that OWNS delinked data is a different problem
        # and is not what this path models.
        raise PromoteError(f"{ident}: entry owns data/bss; not a text-only promotion")

    funcs = entry.get("functions", [])
    if not funcs:
        raise PromoteError(f"{ident}: entry licenses no functions")
    spans = [(int(s["start"], 16), int(s["end"], 16)) for s in sections]
    legacy = []
    for f in funcs:
        addr, size = int(f["address"], 16), int(f["size"], 16)
        if not any(lo <= addr and addr + size <= hi for lo, hi in spans):
            raise PromoteError(f"{ident}: {f['symbol']} at 0x{addr:08x}+0x{size:x} "
                               f"falls outside the manifest's .text span(s)")
        source = f.get("legacy_source")
        if not source:
            raise PromoteError(f"{ident}: {f['symbol']} has no legacy_source")
        if source not in legacy:
            legacy.append(source)

    dl = delinks_path(entry["module"])
    if not dl.is_file():
        raise PromoteError(f"{ident}: {dl} does not exist")
    text = dl.read_text(encoding="utf-8")
    for source in legacy:
        n = len(_entry_re(source).findall(text))
        if n != 1:
            raise PromoteError(f"{ident}: {source} has {n} entries in "
                               f"{dl.relative_to(REPO)}, expected exactly 1")
    return {"id": ident, "source": src, "dest": dest, "delinks": dl,
            "legacy": legacy, "spans": spans, "functions": funcs}


def rewrite_delinks(p):
    """N per-function entries out, one spanning entry in, in address order."""
    text = p["delinks"].read_text(encoding="utf-8")
    for source in p["legacy"]:
        text = _entry_re(source).sub("", text, count=1)
    body = "".join(f"    .text start:0x{lo:08x} end:0x{hi:08x}\n" for lo, hi in p["spans"])
    entry = f"{p['dest']}:\n    complete\n{body}\n"
    # Address order is not cosmetic: it is how a reader of delinks.txt finds the entry
    # that owns an address, and dsd emits the file sorted.
    after = min(hi for _lo, hi in p["spans"])
    nxt = None
    for m in re.finditer(
            r"^[^\s:][^\n]*:\n[ \t]+complete\n[ \t]+\.text start:0x([0-9a-fA-F]{8})",
            text, re.M):
        if int(m.group(1), 16) >= after:
            nxt = m
            break
    text = (text[:nxt.start()] + entry + text[nxt.start():]) if nxt \
        else text.rstrip("\n") + "\n\n" + entry
    p["delinks"].write_text(text, encoding="utf-8", newline="")


def rewrite_manifest(entry, p):
    entry["source"] = p["dest"]
    entry["status"] = "promoted"
    path = CONFIG / "tu_manifest.d" / f"{entry['id']}.json"
    path.write_text(json.dumps(entry, indent=2, ensure_ascii=False) + "\n",
                    encoding="utf-8", newline="")


def rewrite_attribution(plans, lineage):
    """One override per absorbed symbol, so the consolidation keeps its authors.

    Without these, prepush_attribution reports every legacy basename as CREDIT LOST
    and the merge gate needs a label to pass -- for a change that took nothing away
    from anyone.
    """
    path = REPO / "attribution.json"
    data = json.loads(path.read_text(encoding="utf-8"))
    ov = data.setdefault("overrides", {})
    added = 0
    for p in plans:
        for f in p["functions"]:
            stem = pathlib.PurePosixPath(f["legacy_source"]).stem
            who = lineage.get(f"src/{stem}")
            if not who:
                continue
            key = f"{p['dest']}#{f['symbol']}"
            if key not in ov:
                ov[key] = who
                added += 1
    data["overrides"] = dict(sorted(ov.items()))
    path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n",
                    encoding="utf-8", newline="")
    return added


def git(*args):
    subprocess.run(["git", *args], cwd=REPO, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("ids", nargs="+", help="manifest entry id(s), e.g. ov045/PoleLift")
    ap.add_argument("--dry-run", action="store_true",
                    help="report what each promotion would touch and change nothing")
    args = ap.parse_args()

    data = TUM.load()
    by_id = {e["id"]: e for e in data["entries"]}
    plans, entries, refused = [], [], []
    for ident in args.ids:
        entry = by_id.get(ident)
        if entry is None:
            refused.append(f"{ident}: no such manifest entry")
            continue
        try:
            plans.append(plan(entry))
            entries.append(entry)
        except PromoteError as exc:
            refused.append(str(exc))

    for why in refused:
        print(f"  refused  {why}")
    if not plans:
        print("tu_promote: nothing to promote.")
        return 1 if refused else 0

    for p in plans:
        print(f"  promote  {p['id']:38s} {len(p['functions'])} function(s), "
              f"{len(p['legacy'])} legacy source(s) -> {p['dest']}")
    if args.dry_run:
        print(f"tu_promote: {len(plans)} entry(ies) would be promoted (dry run).")
        return 0

    import prepush_attribution as PA
    lineage = PA.lineage("HEAD")
    for p, entry in zip(plans, entries):
        rewrite_delinks(p)
        # `git mv` will not create the destination directory, and a promoted_source
        # may name one src/ does not have yet (src/stage/ was the first).
        (REPO / p["dest"]).parent.mkdir(parents=True, exist_ok=True)
        git("mv", p["source"], p["dest"])
        for source in p["legacy"]:
            git("rm", "-q", source)
        rewrite_manifest(entry, p)
    added = rewrite_attribution(plans, lineage)
    print(f"tu_promote: {len(plans)} entry(ies) promoted, "
          f"{sum(len(p['functions']) for p in plans)} function(s) consolidated, "
          f"{added} attribution override(s) added.")
    print("tu_promote: now run `python tools/rombuild.py -j16 --no-rom` -- "
          "106/106 with mismatching 0 is the proof.")
    return 1 if refused else 0


if __name__ == "__main__":
    sys.exit(main())
