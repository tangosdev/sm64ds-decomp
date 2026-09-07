//cpp
// @symbol func_ov006_020d1ba0
#include "dScMgAmida_c.h"

extern "C" {
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
void func_02012718(int sfx, int pan);
int func_020126e8(int x);
int func_02012468(int handle, int a, int sfx, int b, int c, int d, int pan, short e);
void func_02012790(int sfx);
void func_ov004_020ae5c4(dScMgBase_c *self, int x0, int y0, int x1, int y1, int colour, int size);
int func_ov006_020d2580(dScMgAmida_c *self, dScMgAmida_c::Point p1, dScMgAmida_c::Point p2);
void func_ov006_020d1450(dScMgAmida_c *self);
void func_ov006_020d3624(dScMgAmida_c *self);
void func_ov006_020d3668(dScMgAmida_c *self);
}
namespace Sound { void PlayBank2_2D(unsigned int); }

extern "C" void func_ov006_020d1ba0(dScMgAmida_c *self)
{
    int oldX;
    int oldY;

    if (self->mLineCount >= 0xff) return;
    if (self->unk_53dd == 1) return;

    oldX = self->mPen.x;
    oldY = self->mPen.y;

    if (data_020a0de8[data_020a0e40 * 4] != 0) {
        int newX = data_020a0dea[data_020a0e40 * 4];
        int newY = data_020a0deb[data_020a0e40 * 4];
        if (newX < 0) return;
        if (newX >= 0x100) return;
        if (newY < 0) return;
        if (newY >= self->mLineEndY) return;
        if (self->unk_53de != 0) return;
        self->mPen.x = newX;
        self->mPen.y = newY;
        if (self->mPenStart.x == -1 || self->mPenStart.y == -1) {
            self->mPenStart.x = newX;
            self->mPenStart.y = newY;
            func_02012718(0xdc, self->mPenStart.x << 12);
        }
        if (oldX == -1 && oldY == -1) return;
        if (oldX == self->mPen.x && oldY == self->mPen.y) {
            self->mPenTapped = 1;
            return;
        }
        func_ov004_020ae5c4(self, oldX, oldY, self->mPen.x, self->mPen.y, self->unk_53e4, 1);
        if (self->mPenTapped == 1) {
            self->mPenSoundHandle = func_02012468(self->mPenSoundHandle, 2, 0xde, 4, 0, 0, func_020126e8(self->mPen.x << 12), 0);
            return;
        }
        self->mPenSoundHandle = func_02012468(self->mPenSoundHandle, 2, 0xdd, 4, 0, 0, func_020126e8(self->mPen.x << 12), 0);
        return;
    } else {
        int released;
        if (data_020a0de8[data_020a0e40 * 4] == 0 && data_020a0de9[data_020a0e40 * 4] != 0) {
            released = 1;
        } else {
            released = 0;
        }
        if (released != 0) {
            if (self->mPenStart.x != -1 && self->mPenStart.y != -1 && oldX != -1 && oldY != -1) {
                if (self->mLineStartSet == 0) {
                    int d1 = oldX - self->mPenStart.x;
                    if (d1 < 0) d1 = -d1;
                    if (d1 < 0x10) {
                        int d2 = oldY - self->mPenStart.y;
                        if (d2 < 0) d2 = -d2;
                        if (d2 >= 0x10) goto classify;
                    } else {
                    classify:
                        if (oldX >= 0x21 && oldX <= 0x3f) {
                            if (self->mPenStart.x >= 0x41 && self->mPenStart.x <= 0x5f) {
                                self->mLineStart.x = 0x60;
                                self->mLineStart.y = self->mPenStart.y;
                                self->mLineEnd.x = 0x20;
                                self->mLineEnd.y = self->mPen.y;
                            }
                        } else if (oldX >= 0x41 && oldX <= 0x5f) {
                            if (self->mPenStart.x >= 0x21 && self->mPenStart.x <= 0x3f) {
                                self->mLineStart.x = 0x20;
                                self->mLineStart.y = self->mPenStart.y;
                                self->mLineEnd.x = 0x60;
                                self->mLineEnd.y = self->mPen.y;
                            }
                        } else if (oldX >= 0x61 && oldX <= 0x7f) {
                            if (self->mPenStart.x >= 0x81 && self->mPenStart.x <= 0x9f) {
                                self->mLineStart.x = 0xa0;
                                self->mLineStart.y = self->mPenStart.y;
                                self->mLineEnd.x = 0x60;
                                self->mLineEnd.y = self->mPen.y;
                            }
                        } else if (oldX >= 0x81 && oldX <= 0x9f) {
                            if (self->mPenStart.x >= 0x61 && self->mPenStart.x <= 0x7f) {
                                self->mLineStart.x = 0x60;
                                self->mLineStart.y = self->mPenStart.y;
                                self->mLineEnd.x = 0xa0;
                                self->mLineEnd.y = self->mPen.y;
                            }
                        } else if (oldX >= 0xa1 && oldX <= 0xbf) {
                            if (self->mPenStart.x >= 0xc1 && self->mPenStart.x <= 0xdf) {
                                self->mLineStart.x = 0xe0;
                                self->mLineStart.y = self->mPenStart.y;
                                self->mLineEnd.x = 0xa0;
                                self->mLineEnd.y = self->mPen.y;
                            }
                        } else if (oldX >= 0xc1 && oldX <= 0xdf) {
                            if (self->mPenStart.x >= 0xa1 && self->mPenStart.x <= 0xbf) {
                                self->mLineStart.x = 0xa0;
                                self->mLineStart.y = self->mPenStart.y;
                                self->mLineEnd.x = 0xe0;
                                self->mLineEnd.y = self->mPen.y;
                            }
                        }
                        if (self->mLineStart.x >= 0 && self->mLineEnd.x >= 0) {
                            if (func_ov006_020d2580(self, self->mLineStart, self->mLineEnd) == 0) {
                                self->mLineStartSet = 1;
                                self->mLineEndSet = 1;
                            }
                        }
                    }
                } else if (self->mLineEndSet == 0) {
                    int dx, dsx, adx, adsx;
                    dx = oldX - self->mLineStart.x;
                    adx = dx < 0 ? -dx : dx;
                    dsx = self->mPenStart.x - self->mLineStart.x;
                    adsx = dsx < 0 ? -dsx : dsx;
                    if (adx >= adsx) {
                        dx = dx < 0 ? -dx : dx;
                        if (dx >= 0x20) {
                            if (oldX < self->mLineStart.x) {
                                self->mLineEnd.x = self->mLineStart.x - 0x40;
                            } else {
                                self->mLineEnd.x = self->mLineStart.x + 0x40;
                            }
                            self->mLineStart.y = self->mLineEnd.y;
                            self->mLineEnd.y = self->mPen.y;
                        }
                    } else {
                        dsx = dsx < 0 ? -dsx : dsx;
                        if (dsx >= 0x20) {
                            if (self->mPenStart.x < self->mLineStart.x) {
                                self->mLineEnd.x = self->mLineStart.x - 0x40;
                            } else {
                                self->mLineEnd.x = self->mLineStart.x + 0x40;
                            }
                            self->mLineEnd.y = self->mPenStart.y;
                        }
                    }
                    if (self->mLineStart.x >= 0 && self->mLineEnd.x >= 0) {
                        if (func_ov006_020d2580(self, self->mLineStart, self->mLineEnd) == 0) {
                            self->mLineEndSet = 1;
                        }
                    }
                }
            }

            if (self->mLineStartSet == 0 || self->mLineEndSet == 0) {
                if (self->unk_53de == 0 && self->unk_53dd == 0) func_02012790(0xe);
                func_ov006_020d1450(self);
                self->mLineStart.x = -1;
                self->mLineStart.y = -1;
                self->mLineEnd.x = -1;
                self->mLineEnd.y = -1;
                return;
            }
            if (self->mLineEnd.x == self->mLineStart.x) {
                func_ov006_020d3624(self);
                int row = 0;
                int col;
                int rowoff = 0;
                int one = 0;
                int val = 0;
                for (; row < 0x100; row++) {
                    for (col = 0; col < 0x158; col++) {
                        (self->unk_470c + rowoff)[col] = val;
                    }
                    rowoff += 0x158;
                }
            } else {
                dScMgAmida_c::Point a(self->mLineStart);
                dScMgAmida_c::Point b(self->mLineEnd);
                int hit;
                if (a.x < b.x) {
                    a.x += 2;
                    b.x -= 2;
                } else {
                    a.x -= 2;
                    b.x += 2;
                }
                {
                    if (func_ov006_020d2580(self, a, b) == 1) {
                        if (self->unk_53de == 0 && self->unk_53dd == 0) func_02012790(0xe);
                        func_ov006_020d1450(self);
                        self->mLineStart.x = -1;
                        self->mLineStart.y = -1;
                        self->mLineEnd.x = -1;
                        self->mLineEnd.y = -1;
                        return;
                    }
                }
                if (self->mLineStart.x < self->mLineEnd.x) {
                    u8 *row = self->unk_4710 + self->mLineEnd.x * 0x158;
                    hit = *(row + self->mLineEnd.y + 0x218);
                    if (hit == 0) {
                        hit = *(self->unk_4710 + self->mLineStart.x * 0x158 + self->mLineStart.y - 0x98);
                    }
                } else {
                    u8 *row = self->unk_4710 + self->mLineEnd.x * 0x158;
                    hit = *(row + self->mLineEnd.y - 0x98);
                    if (hit == 0) {
                        hit = *(self->unk_4710 + self->mLineStart.x * 0x158 + self->mLineStart.y + 0x218);
                    }
                }
                if (hit == 0) {
                    func_ov006_020d1450(self);
                    self->mLineCommit = 1;
                    func_ov004_020ae5c4(self, a.x, a.y, b.x, b.y, self->unk_53e4, 1);
                    if (a.x < b.x) {
                        func_ov004_020ae5c4(self, a.x - 1, a.y, a.x - 1, a.y, self->unk_53e4, 1);
                        func_ov004_020ae5c4(self, b.x + 1, b.y, b.x + 1, b.y, self->unk_53e4, 1);
                    } else {
                        func_ov004_020ae5c4(self, a.x + 1, a.y, a.x + 1, a.y, self->unk_53e4, 1);
                        func_ov004_020ae5c4(self, b.x - 1, b.y, b.x - 1, b.y, self->unk_53e4, 1);
                    }
                    self->mLineCommit = 0;
                    func_ov006_020d3668(self);
                    self->mLineCount++;
                    Sound::PlayBank2_2D(0xdf);
                } else {
                    if (self->unk_53de == 0 && self->unk_53dd == 0) func_02012790(0xe);
                    func_ov006_020d1450(self);
                    self->mLineStart.x = -1;
                    self->mLineStart.y = -1;
                    self->mLineEnd.x = -1;
                    self->mLineEnd.y = -1;
                    return;
                }
            }
        }
        self->mPen.x = -1;
        self->mPen.y = -1;
        self->mPenStart.x = -1;
        self->mPenStart.y = -1;
        self->mLineStartSet = 0;
        self->mLineEndSet = 0;
        self->mPenTapped = 0;
    }
}
