/* class cMgSmartball_board_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed60, RTTI ov006:0x0213ec58
 * (_ZTI20cMgSmartball_board_c), _ZTS20cMgSmartball_board_c ov006:0x0213ee20.
 * The eleventh and last direct child of cMgSmartball_object_c -- see that
 * header for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x9c, from _Znwj(0x9c) in func_ov006_02115b0c immediately before the
 * call to this class's constructor func_ov006_0210f9f8. Base ends at 0x34, so
 * this class adds 0x68 bytes.
 *
 * FIELD EVIDENCE. RestoreInitial (ov006:0x0210f914) is exhaustive: it
 * zeroes every field this class owns, in address order, with the loop bounds
 * and store widths below, and the region it walks ends at exactly 0x9c. Every
 * bound and width here comes from that pass, not from a guess. SaveSnapshot
 * and Update corroborate all of it -- both indexed the same nine- and
 * eight-element arrays in the pre-migration matched source.
 *
 * THE FIELD NAMES BELOW MOSTLY COME FROM func_ov006_0210ef48, the out-of-scope
 * helper SaveSnapshot calls the instant a ball claims a cell. It reaches this
 * object through its own shadow struct -- `u8 cells[9]` at 0x31, `int
 * lines[8]` at 0x6c, `u8 marks[8]` at 0x8c, and the two scalars at 0x94/0x98
 * -- and it is the only place any of the last four are WRITTEN: on completing
 * a line it sets that line's timer to 0x21d, raises its mark, arms 0x94 with
 * 0xb5 and increments 0x98. That is what settles 0x94 as a fanfare countdown
 * and 0x98 as a count of completed lines rather than a second timer.
 * FIELD NAMES and their evidence are tabulated in
 * notes/smartball-provenance.md.
 *
 * WHAT THE CLASS IS. A 3x3 board of flip-over cells plus the 8 lines through
 * it (3 columns, 3 rows, 2 diagonals). Update proves the line indexing: cases
 * 0-2 light cells {i, i+3, i+6} (a column), cases 3-5 light {3(i-3) ..
 * 3(i-3)+2} (a row), case 6 lights {0,4,8} and case 7 lights {2,4,6} (the two
 * diagonals). That reading is why the nine-element arrays and the
 * eight-element arrays are two different lengths and not one.
 *
 * THE NINE-BYTE CELL ARRAY STRADDLES THE BASE BOUNDARY, and that is the one
 * thing about this class C++ cannot say. It runs 0x31..0x39; the base ends at
 * 0x34, so its first THREE elements are the base's unk_031/unk_032/unk_033
 * and only the last six are ours. A member cannot begin inside the base, and
 * widening the base to swallow it would change cMgSmartball_object_c's size
 * for its other ten children. So only the tail is declared (mCellsTail[6]) and
 * the array is indexed as `(&unk_031)[i]` -- pointer arithmetic off the
 * base's first cell byte, walking through unk_032/unk_033 and into
 * mCellsTail[6]. That spelling is not a stylistic pick: it is the only one of
 * six measured that reproduces. `((u8 *)this)[0x31 + i]` compiles a
 * register-offset store where the ROM has a displacement, and
 * `*(u8 *)((char *)this + i + 0x31)` lets strength reduction build an
 * induction variable the ROM does not have.
 *
 * This class is also what settled the base's 0x31-0x33 span: see
 * cMgSmartball_object_c.h, where those three bytes are now three u8s because
 * three children read them three incompatible ways and a board cell is one of
 * the three.
 *
 * CONSTRUCTED BY func_ov006_0210f9f8, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. It writes this vtable and sets the base's unk_028 to
 * 0x18000 -- the per-child constant every sibling's constructor also writes,
 * each with its own value. */
#ifndef CMGSMARTBALL_BOARD_C_H
#define CMGSMARTBALL_BOARD_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_board_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    u8  mCellsTail[6]; /* 0x034-0x039 -- the TAIL of a 9-element per-cell CLAIMED
                        flag array that BEGINS AT 0x31, inside the base (1 = a
                        ball has flipped this cell). Never indexed through this
                        name: the whole array is reached as (&unk_031)[i],
                        because a member cannot straddle the base boundary.
                        Declared only so the layout below sits at the right
                        offsets. func_ov006_0210ef48 models the same nine bytes
                        as a plain `u8 cells[9]` at 0x31 in its own shadow
                        struct, which is the corroboration for the length. */

    /* Genuine gap -- RestoreInitial's exhaustive zero pass steps straight from
       the 9 bytes ending at 0x39 to the s32 array at 0x3c. UNMODELLED, NOT
       UNREAD; the alignment of the next member accounts for it. */
    u8  pad_03a[2]; /* 0x03a-0x03b */

    s32 mCellFlipAngle[9]; /* 0x03c -- per-cell flip angle. SaveSnapshot advances a
                        claimed cell's angle by 0x400 a frame and clamps at
                        0x8000 (a half turn); Update feeds it to the shared
                        sine table data_02082214 to shear the sprite, and
                        picks a flipped-face sprite once it passes 0x4000. */
    u8  mCellHighlight[9]; /* 0x060 -- per-cell: this cell is part of a line that
                        is currently scoring. Update sets it for each of a
                        scoring line's three cells and then blinks those cells
                        off mFanfareTimer. */

    /* Genuine gap -- the zero pass jumps from the 9 bytes ending at 0x68 to
       the s32 array at 0x6c. UNMODELLED, NOT UNREAD. */
    u8  pad_069[3]; /* 0x069-0x06b */

    s32 mLineTimer[8]; /* 0x06c -- per-line banner countdown, armed with 0x21d by
                        func_ov006_0210ef48 the moment that line completes.
                        SaveSnapshot ages each one while positive and Update
                        draws the line's banner while it is positive. Eight
                        lines, see above. */
    u8  mLineScored[8]; /* 0x08c -- per-line: this line HAS been completed, set
                        alongside its timer by func_ov006_0210ef48 and never
                        cleared until RestoreInitial. Update keeps drawing the
                        banner for such a line after its timer has expired,
                        provided the manager's own 0x4660 flag is up. */
    s32 mFanfareTimer;  /* 0x094 -- armed with 0xb5 by func_ov006_0210ef48 every
                        time a line completes. SaveSnapshot spends it and
                        replays the jingle once a second (% 60) while it lasts;
                        Update reads bits 0x3c of it to blink the highlighted
                        cells. */
    s32 mLinesScored;   /* 0x098 -- how many lines have been completed:
                        func_ov006_0210ef48 increments it once per line it
                        completes, and SaveSnapshot switches on it to pick
                        which of four jingles mFanfareTimer replays. */
};

typedef char cMgSmartball_board_c_size_must_be_0x9c[sizeof(cMgSmartball_board_c) == 0x9c ? 1 : -1];

#endif
