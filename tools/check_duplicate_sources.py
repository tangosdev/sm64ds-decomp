"""Fail when src/ holds two files for one symbol.

A stem with both a `.c` and a `.cpp` is not a harmless leftover. `config/**/
delinks.txt` names exactly one path per address range, so the other file is
compiled by nothing -- and every gate in this tree is driven from delinks.txt.
`eligible.py` never sees it. `rombuild.py` never sees it. The byte gate is only
ever pointed at files the build knows about. So a dead half can say anything at
all and no check in the repo will notice, while a reader who opens it has no way
to tell it is not the source of the shipped bytes.

That is not hypothetical. When this check was written, six stems were doubled:

  * `_ZN13PoleBillboard8BehaviorEv` -- NEITHER half was enrolled, the `.c` did
    not reproduce the ROM and the `.cpp` did. A correct implementation sat in the
    tree uncompiled while the one that does not match was what a reader found.
  * `_ZN15TtcRotatingGear8BehaviorEv` and `_ZN5Stage7PS_InitEv` -- both halves
    reproduced the ROM, and the `.cpp` halves DO NOT LINK. They declare their
    callees as C++ (`struct G2x { static void SetBlendBrightness(...); }`,
    `Platform::UpdateClsnPosAndRot`, `RandomIntInternal`), so the compiler
    mangles those names itself and produces symbols that exist nowhere in the
    ROM. Seven undefined references between the two. A byte match cannot see
    this -- match.py wildcards relocated words -- and only the ROM link does.

The lesson worth keeping: a duplicate is not a tidiness problem, it is a file
that has never been checked by anything.

    python tools/check_duplicate_sources.py
"""
import collections
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent

stems = collections.defaultdict(list)
for path in (REPO / "src").rglob("*"):
    if path.suffix in (".c", ".cpp"):
        stems[path.with_suffix("")].append(path)

dupes = sorted((k, sorted(v)) for k, v in stems.items() if len(v) > 1)

# Which half, if any, the build actually names.
named = set()
for delinks in REPO.glob("config/**/delinks.txt"):
    for line in delinks.read_text(errors="replace").splitlines():
        if line.startswith("src/") and line.rstrip().endswith(":"):
            named.add((REPO / line.rstrip()[:-1]).resolve())

for stem, paths in dupes:
    print(f"DUPLICATE {stem.relative_to(REPO).as_posix()}")
    for p in paths:
        role = "named by delinks.txt" if p.resolve() in named else "compiled by NOTHING"
        print(f"    {p.relative_to(REPO).as_posix():70s} {role}")

if dupes:
    print(f"\n{len(dupes)} stem(s) carry more than one source file. Keep exactly one.")
    print("A dead half is not checked by any gate in this repo -- see this file's docstring.")
else:
    print(f"duplicate-sources: {len(stems)} stems, none doubled")

sys.exit(1 if dupes else 0)
