#!/usr/bin/env python3
"""Add (or remove) hal/level_boot.cpp's level-31 row -- run link100, lane TAIL2.

The row itself is one line, one thunk and one entry in port_level_mount_fns[],
and the block above port_level_table[] already carries the derivation and says
so ("A row for it is one line here, one thunk and one CMake token; nothing
needs re-deriving"). The mount is not the question -- what kept level 31 out is
a frame-zero fault in the PLAYER's own render, which the same block measured
and which nothing in this lane's grant touches. So this is a script rather than
a committed edit: apply it, measure, and keep the row only if the fault is gone.

  python port/tools/tail2_apply_lvl31.py <root> [--remove]

APPENDED AT THE END, indices 50, for the reason both blocks above the table
give: a row inserted mid-table renumbers every thunk below it and nothing in
the build says so. port/tools/mount_pairing_guard.py is what checks it.
"""
import pathlib
import sys

ROW = """    /* ---- link100 TAIL2 ---- LEVEL 31, the one stage id the ROM has and the
       port did not mount. Every field is the w21 block's own derivation above
       (ov039 0x02111438, course 23, subCount 1, flags 0x00, handles
       074d/074b/074e/074f), re-read here against config/arm9 rather than
       copied: data_020758c8[31] -> ov039 and data_02092208[31] -> 0x02111438.
       ov039 has been on PORT_LEVEL_OVERLAYS since wave 21 and its data is
       mounted; only the row and its thunk were missing. own_sinits stays 0. */
    {31, "Wing Cap tower (habatake, course 23)", "ov039", 0x02111438,
     port_ov039_patch, port_ov039_at,
     &port_ov039_ds_base, &port_ov039_ds_end, 0},
"""
THUNK = """/* ---- link100 TAIL2 ---- index 50, the level-31 row appended above. */
static void *port_mount_row_lvl31(void) { return port_level_mount_at(50); }
"""
FN = "    port_mount_row_lvl31,\n"

TABLE_END = """     &port_ov050_ds_base, &port_ov050_ds_end, 0},
};
"""
THUNK_ANCHOR = """static void *(*const port_level_mount_fns[PORT_LEVEL_COUNT])(void) = {"""
FN_END = """    port_mount_row_lvl42vs,
};
"""


def main():
    root = pathlib.Path(sys.argv[1]).resolve()
    remove = "--remove" in sys.argv
    p = root / "port/hal/level_boot.cpp"
    t = p.read_text()
    if remove:
        for s in (ROW, THUNK, FN):
            if s not in t:
                sys.exit(f"cannot remove: this text is not in the file:\n{s}")
            t = t.replace(s, "", 1)
        p.write_text(t)
        print("level 31 row, thunk and mount_fns entry REMOVED")
        return
    if ROW in t:
        sys.exit("the level-31 row is already there")
    assert t.count(TABLE_END) == 1, "the table's last row moved"
    t = t.replace(TABLE_END, TABLE_END.replace("};\n", "") + ROW + "};\n", 1)
    assert t.count(THUNK_ANCHOR) == 1
    t = t.replace(THUNK_ANCHOR, THUNK + "\n" + THUNK_ANCHOR, 1)
    assert t.count(FN_END) == 1
    t = t.replace(FN_END, "    port_mount_row_lvl42vs,\n" + FN + "};\n", 1)
    p.write_text(t)
    print("level 31 row added at index 50, thunk port_mount_row_lvl31 paired")


if __name__ == "__main__":
    main()
