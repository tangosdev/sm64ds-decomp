#!/usr/bin/env python3
"""THE THUNK TRAP, closed: verify port_level_table[i] pairs with the thunk that
mounts row i.

hal/level_boot.cpp keeps three lists that have to agree positionally:

    port_level_table[]      the rows      -- {id, name, "ovNNN", lvl_overlay, ...}
    port_level_mount_fns[]  the thunks    -- one function pointer per row
    port_mount_row_*()      each thunk    -- returns port_level_mount_at(N)

port_level_mounts_install() pairs table[i] with mount_fns[i] POSITIONALLY, and
its only assertion compares the ROW's lvl_overlay against the ROM's
data_02092208 -- never the thunk. So a row inserted mid-table renumbers every
thunk below it, registers each level id against the NEXT level's mount, and
every check in the build still passes: the indices are all in range, every
lvl_overlay still matches the ROM, the link succeeds, and the only symptom is
that levels quietly load each other's stages.

That is not hypothetical. Wave 8 wrote its sixteen rows at indices 17..32
against a base where levels 36 and 40 had not yet taken 17 and 18; the merge
moved every one of them up by two, and nothing in the build would have said so.
It was caught by a naming convention, by eye.

This guard is the check that was missing. For every i it asserts

    mount_fns[i] is the thunk whose body says port_level_mount_at(i)

and, where the thunk is named port_mount_row_lvlN, that N is table[i].id -- so
the naming convention that caught wave 8 by eye is now enforced by the build.

    python port/tools/mount_pairing_guard.py [repo-root]

Exit 0 with a one-line verdict, 1 and a per-row diagnosis on any disagreement.
Source-parsing on purpose: the alternative is calling each thunk at boot to see
which index it uses, and a thunk that mounts is not safe to call speculatively:
every one of them patches an overlay image in place, and port_level_mount_at
re-patches ov065's Ttc level-window words to whatever level id it is handed.
(The example this line used to give -- "level 27's carries the ov035/Ttc data
seat behind it" -- retired with lane w3-a2, which moved that call off the thunk
and into port_level_mount_at. The reason stands; the thunks are uniform now.)
"""

import os
import re
import sys


def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", " ", s, flags=re.S)
    s = re.sub(r"//[^\n]*", " ", s)
    return s


def block(src, decl):
    """The brace-balanced initialiser of `decl`, comments already gone."""
    i = src.index(decl)
    i = src.index("{", i)
    depth, j = 0, i
    while True:
        if src[j] == "{":
            depth += 1
        elif src[j] == "}":
            depth -= 1
            if depth == 0:
                return src[i + 1:j]
        j += 1


def main():
    root = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else ".")
    path = os.path.join(root, "port", "hal", "level_boot.cpp")
    raw = open(path, "r", encoding="utf-8", errors="replace").read()
    src = strip_comments(raw)

    # thunk name -> the index its body mounts
    thunks = dict(re.findall(
        r"static\s+void\s*\*\s*(port_mount_row_\w+)\s*\(\s*void\s*\)"
        r"[^;{]*\{[^}]*?port_level_mount_at\s*\(\s*(\d+)\s*\)",
        src, re.S))
    thunks = {k: int(v) for k, v in thunks.items()}

    rows = re.findall(r"\{\s*(\d+)\s*,\s*\"([^\"]*)\"\s*,\s*\"(ov\d+)\"",
                      block(src, "port_level_table[]"))
    fns = [t.strip() for t in
           block(src, "port_level_mount_fns[PORT_LEVEL_COUNT]").split(",")
           if t.strip()]

    bad = []
    if len(rows) != len(fns):
        print(f"mount_pairing_guard: FAIL -- {len(rows)} table rows but "
              f"{len(fns)} thunk slots")
        return 1

    for i, (lid, name, ov) in enumerate(rows):
        fn = fns[i]
        if fn not in thunks:
            bad.append(f"  row {i} (level {lid}, {ov}): mount_fns[{i}] is "
                       f"{fn}, which is not a port_level_mount_at thunk")
            continue
        got = thunks[fn]
        if got != i:
            bad.append(f"  row {i} (level {lid}, {ov}): mount_fns[{i}] is "
                       f"{fn}, which mounts row {got} -- level {lid} would "
                       f"register against row {got}'s level")
        m = re.fullmatch(r"port_mount_row_lvl(\d+)", fn)
        if m and int(m.group(1)) != int(lid):
            bad.append(f"  row {i}: thunk {fn} names level {m.group(1)} but "
                       f"the row is level {lid}")

    if bad:
        print("mount_pairing_guard: FAIL")
        print("\n".join(bad))
        return 1

    named = sum(1 for f in fns if re.fullmatch(r"port_mount_row_lvl\d+", f))
    print(f"mount_pairing_guard: OK -- {len(rows)} rows, {len(rows)} thunks, "
          f"every mount_fns[i] mounts row i ({named} also name their level "
          f"and all agree)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
