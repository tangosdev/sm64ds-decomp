//cpp
// @symbol func_ov006_02126ee4
/* recovered: dScMgSnowball_c course generator, ov006 0x02126ee4 (2764 bytes).
 * Paints mTileMap row by row from the bottom of the course upwards, carving a
 * corridor between a left and a right edge cursor that random-walk one lane at
 * a time. Each row draws two kinds (0 hold, 1 pull in, 2 push out), clamps
 * them against the corridor width and against the dead zones at both ends of
 * the course, then paints solid 1 outside the corridor, a two-tile cap at each
 * wall and 0 in between. Obstacle tile 12 drops in on a cooldown and takes an
 * mArray1 slot with it. A second pass recolours the two goal rows, and a third
 * walks every solid tile to seed decoration tiles 0x1d..0x20 and the mArray2
 * props.
 *
 * FIVE SPELLINGS ARE LOAD-BEARING, all measured against the ROM on 2004/b56;
 * every one of them was worth whole blocks of registers, never one or two
 * instructions:
 *   - mArray1 and mArray2 carry their 8-byte stride in the TYPE (the header
 *     now spells them `s32 [0x80][2]`). Written as byte arrays with the
 *     stride in the expression, mwccarm strength-reduces `slot * 8` into a
 *     second induction variable, which costs the third pass the register the
 *     ROM keeps 0x7fff in and spills the constant 1 to the frame: +9 words.
 *   - `row` is declared AFTER rightKind. Declaration order picks the
 *     callee-saved register here: declared earlier it takes sb and rotates
 *     leftKind/rightKind/left/right one register each, 145 wrong words.
 *   - the goal-row pass, the prop pass and the first `left` fill each own
 *     their loop counter (goalRow/j, propRow/col, k). Sharing one `i` with
 *     the rest of the function swaps the counter and the derived row pointer
 *     between r1 and r2 in each of them.
 *   - the wide-corridor obstacle owns `wideLane` rather than sharing `lane`
 *     with the other two obstacle sites, which is what puts the lane in r2
 *     and the mArray1 scan counter in r1 the way the ROM has them.
 *   - `right - left` in the else arm stays an EXPRESSION. As a named local it
 *     is allocated a frame slot ahead of the three induction temporaries and
 *     every later slot shifts; the ROM spills it last, at sp+0x28.
 * The run of literal stores at sp+0x2c..0xc4 is not a table: it is mwccarm
 * hoisting one value per textual constant out of the row loop and spilling
 * all of them, which falls out of writing the constants inline. */
#include "dScMgSnowball_c.h"

extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
}

extern "C" void func_ov006_02126ee4(dScMgSnowball_c *self)
{
    int k;
    int i;
    int left;
    int right;
    int leftKind;
    int rightKind;
    int row;
    int margin;
    int narrow;
    int wide;
    int prevLeft;
    int prevRight;
    int cooldown;
    int propRow;
    int lane;
    int slot;
    int roll;
    u16 tile;
    int goalRow;
    int col;
    int j;
    int wideLane;

    for (i = 0; i < 0x80; i++) {
        self->mArray1Active[i] = 0;
        self->mArray1Kind[i] = 0;
        self->mArray1Hit[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        self->mArray2Active[i] = 0;
    }
    self->unk_ab5c = 0;

    if (self->Virtual8C()) {
        narrow = 0xb;
        wide = 0xe;
        margin = 6;
    } else {
        narrow = 0xa;
        wide = 0xd;
        margin = 7;
    }

    rightKind = leftKind = 0;
    left = 2;
    right = 0xd;
    cooldown = 0x12;

    for (row = self->mScrollLimit - 1; row >= 0; row--) {
        prevLeft = leftKind;
        prevRight = rightKind;
        leftKind = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;
        rightKind = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;

        if (row >= self->mScrollLimit - 8) {
            rightKind = leftKind = 0;
        } else if (row <= self->mGoalY / 16) {
            rightKind = leftKind = 0;
        } else if (right - left <= narrow) {
            if (leftKind == 1) {
                leftKind = 0;
            }
            if (rightKind == 1) {
                rightKind = 0;
            }
        } else if (right - left == narrow + 1 && leftKind == 1 && rightKind == 1) {
            rightKind = leftKind = 0;
        } else if (right - left >= wide) {
            if (leftKind == 2) {
                leftKind = 0;
            }
            if (rightKind == 2) {
                rightKind = 0;
            }
        } else if (right - left == wide - 1 && leftKind == 2 && rightKind == 2) {
            rightKind = leftKind = 0;
        }

        switch (leftKind) {
        case 0:
            break;
        case 1:
            if (prevLeft == 2) {
                leftKind = 0;
            } else if (left >= margin) {
                if (prevLeft == 0) {
                    leftKind = 2;
                    left--;
                } else {
                    leftKind = 0;
                }
            }
            break;
        case 2:
            if (prevLeft == 1) {
                leftKind = 0;
            } else if (left < 1) {
                if (prevLeft == 0) {
                    leftKind = 1;
                } else {
                    leftKind = 0;
                }
            } else {
                left--;
            }
            break;
        default:
            leftKind = 0;
            break;
        }

        switch (rightKind) {
        case 0:
            break;
        case 1:
            if (prevRight == 2) {
                rightKind = 0;
            } else if (right <= 0x10 - margin) {
                if (prevRight == 0) {
                    rightKind = 2;
                    right++;
                } else {
                    rightKind = 0;
                }
            }
            break;
        case 2:
            if (prevRight == 1) {
                rightKind = 0;
            } else if (right >= 0xf) {
                if (prevRight == 0) {
                    rightKind = 1;
                } else {
                    rightKind = 0;
                }
            } else {
                right++;
            }
            break;
        default:
            rightKind = 0;
            break;
        }

        for (k = 0; k < left; k++) {
            self->mTileMap[k][row] = 1;
        }
        switch (leftKind) {
        case 0:
            self->mTileMap[left][row] = 2;
            self->mTileMap[left + 1][row] = 0;
            break;
        case 1:
            self->mTileMap[left][row] = 3;
            self->mTileMap[left + 1][row] = 4;
            break;
        case 2:
            self->mTileMap[left][row] = 5;
            self->mTileMap[left + 1][row] = 6;
            break;
        }
        for (i = left + 2; i < right - 1; i++) {
            self->mTileMap[i][row] = 0;
        }
        switch (rightKind) {
        case 0:
            self->mTileMap[right - 1][row] = 0;
            self->mTileMap[right][row] = 7;
            break;
        case 1:
            self->mTileMap[right - 1][row] = 8;
            self->mTileMap[right][row] = 9;
            break;
        case 2:
            self->mTileMap[right - 1][row] = 10;
            self->mTileMap[right][row] = 11;
            break;
        }
        for (i = right + 1; i < 0x10; i++) {
            self->mTileMap[i][row] = 1;
        }

        if (leftKind == 1) {
            left++;
        }
        if (rightKind == 1) {
            right--;
        }
        if (row == self->mGoalY / 16 - 1) {
            self->unk_ab5c = (right + left + 1) * 16 / 2 << 12;
        }

        if (row > self->mGoalY / 16 + 0x11) {
            if (cooldown > 0) {
                cooldown--;
            } else if (self->Virtual8C() && row < self->mScrollLimit / 2) {
                if (right - left >= 0xd) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15) == 0) {
                        if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) == 0) {
                            lane = left + 2;
                        } else {
                            lane = right - 2;
                        }
                        self->mTileMap[lane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i][0] = (lane * 16 + 8) << 12;
                                self->mArray1[i][1] = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                }
            } else {
                if (right - left >= 0xd) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) == 0) {
                        wideLane = left + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * (right - left - 3) >> 15) + 2;
                        self->mTileMap[wideLane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i][0] = (wideLane * 16 + 8) << 12;
                                self->mArray1[i][1] = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                } else if (right - left >= 0xa) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) == 0) {
                        if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) == 0) {
                            lane = left + 2;
                        } else {
                            lane = right - 2;
                        }
                        self->mTileMap[lane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i][0] = (lane * 16 + 8) << 12;
                                self->mArray1[i][1] = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (self->Virtual8C()) {
        for (i = 0; i < 0x80; i++) {
            if (self->mArray1Active[i] == 1) {
                if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) == 0) {
                    self->mArray1Kind[i] = 1;
                }
            }
        }
    }

    for (goalRow = self->mGoalY / 16 - 1; goalRow <= self->mGoalY / 16; goalRow++) {
        for (j = 0; j < 0x10; j++) {
            tile = self->mTileMap[j][goalRow];
            if (tile == 0) {
                self->mTileMap[j][goalRow] = 13;
            } else if (tile == 2) {
                self->mTileMap[j][goalRow] = 14;
            } else if (tile == 7) {
                self->mTileMap[j][goalRow] = 15;
            }
        }
    }

    slot = 0;
    for (propRow = self->mScrollLimit - 1; propRow >= 0; propRow--) {
        for (col = 0; col < 0x10; col++) {
            if (self->mTileMap[col][propRow] == 1) {
                roll = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15;
                if (roll < 4) {
                    self->mTileMap[col][propRow] = (u16)(roll + 0x1d);
                } else if (roll == 4) {
                    if (slot < 0x80) {
                        self->mArray2[slot][0] = (col * 16 + 8) << 12;
                        self->mArray2[slot][1] = (propRow * 16 + 8) << 12;
                        self->mArray2Active[slot] = 1;
                        self->mArray2Kind[slot] = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15;
                        slot++;
                    }
                }
            }
        }
    }
}
