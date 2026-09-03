/* HOST COPY of src/_ZN7Minimap6RenderEv.cpp -- the sixteen-player minimap draw.
 *
 * Byte-for-byte the matched TU, with the per-player reads moved off the four-
 * entry struct members (unk70 @0x70, unk80 @0x80, unk21E @0x21e) onto the
 * sixteen-wide host storage in Minimap_wide.h, keyed by the real slot, plus:
 *
 *   - the OTHER-players blip loop runs all sixteen slots (was `i < 4`); empty
 *     slots carry g_mm_mapID -1, which never matches the current map, so the
 *     four-player picture is unchanged and slots 4..15 draw only when live;
 *   - the OAM icon-table index wraps its SLOT component to 0..3 (mm_icon_slot)
 *     so a high seat reuses a valid colour row instead of walking off the end
 *     of OAM::MM_VS_PLAYER_ICONS / _S (each [4 slots][4 chars]). This is the
 *     exact fault the unfixed build hit: idx*4 with idx 8/12/14/15 indexed past
 *     the sixteen-entry table and handed OAM::RenderSub a garbage pointer.
 *
 * src/ stays byte-frozen; this file is excluded from slice_gate28.txt and links
 * in its place. Everything else -- stars, caps, spike bombs, the arrow matrix,
 * the DISPCNT reads -- is the matched body unchanged, still through the struct.
 *
 * Storage is DEFINED in the Behavior host copy (the writer) and extern here.
 */
#include "types.h"
#include "Minimap_wide.h"

struct Player {
    int unk0, unk4;
    int unk8;                   // 0x008
    u8 padC[0x8e - 0xc];
    s16 unk8E;                  // 0x08e
    u8 pad90[0x6c8 - 0x90];
    u16 unk6C8;                 // 0x6c8
    u8 pad6CA[0x6d9 - 0x6ca];
    u8 unk6D9;                  // 0x6d9
    int HasNoCap();
};

struct Minimap {
    u8 pad0[0x70];
    int unk70[4];               // 0x070  -- now hosted in g_mm_iconX
    int unk80[4];               // 0x080  -- now hosted in g_mm_iconY
    u8 pad90[0x10];
    int unkA0[12];              // 0x0a0
    int unkD0[12];              // 0x0d0
    int unk100[9];              // 0x100
    int unk124[9];              // 0x124
    u8 pad148[0x30];
    int unk178;                 // 0x178
    int unk17C;                 // 0x17c
    int unk180[8];              // 0x180
    int unk1A0[8];              // 0x1a0
    u8 pad1C0[0x40];
    int unk200;                 // 0x200
    int unk204;                 // 0x204
    int unk208;                 // 0x208
    int unk20C;                 // 0x20c
    int unk210;                 // 0x210
    u8 pad214[8];
    s16 unk21C;                 // 0x21c
    s8 unk21E[4];               // 0x21e  -- now hosted in g_mm_mapID
    s8 unk222[12];              // 0x222
    u8 unk22E[12];              // 0x22e
    s8 unk23A[9];               // 0x23a
    u8 pad243[5];
    s8 unk248;                  // 0x248
    s8 unk249[8];               // 0x249
    u8 unk251;                  // 0x251
    u8 pad252[3];
    u8 unk255;                  // 0x255
    u8 unk256;                  // 0x256

    int Render();
};

extern "C" void _ZN3OAM9RenderSubEP7OamAttriiii(void *oam, int x, int y, int a, int pal);
extern "C" void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int on, void *oam, int x, int y, int a, int pal, int scale, int ang);
extern "C" void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int on, void *oam, int x, int y, int a, int pal, void *mtx);

extern "C" u8 data_0209f20c;
extern "C" u8 data_0209f2c4;
extern "C" u8 data_0209f294;
extern "C" u8 data_0209f250;
extern "C" u32 data_0209caa0[];
extern "C" u8 data_0209f2d4;
extern "C" u8 data_0209f2d8;
extern "C" Player *data_0209f394[];
extern "C" void *_ZN3OAM18MM_VS_PLAYER_ICONSE[];
extern "C" s16 data_02082214[];
extern "C" char _ZN3OAM8MM_ARROWE[];
extern "C" void *_ZN3OAM20MM_VS_PLAYER_ICONS_SE[];
extern "C" s8 data_ov002_02111148;
extern "C" void *_ZN3OAM15MM_PLAYER_ICONSE[];
extern "C" u8 data_0209f37c[];
extern "C" int data_0208ee44;
extern "C" void *_ZN3OAM15MM_STAR_MARKERSE[];
extern "C" u8 data_0209f288;
extern "C" char _ZN3OAM11MM_RED_COINE[];
extern "C" u8 data_ov002_02111154[];
extern "C" u8 data_0209f370[];
extern "C" void *data_ov002_0210cac8[];
extern "C" void *data_ov002_0210c748[];
extern "C" u8 data_0209d660;
extern "C" u32 data_020a0db0;
extern "C" void *_ZN3OAM12MM_STAR_KEYSE[];
extern "C" char _ZN3OAM13MM_SPIKE_BOMBE[];

int Minimap::Render()
{
    u8 a = data_0209f20c;
    u8 b = data_0209f2c4;
    u8 d = data_0209f294;
    int i;
    u8 idx = data_0209f250;

    if ((u8)(d | (b | a)) == 0 || !(data_0209caa0[2] & 0x80) || (a != 0 && (u32)data_0209f2d4 < 3)) {
        Player *pl = data_0209f394[idx];
        int vs = (data_0209f2d8 == 1);
        if (vs != 0) {
            _ZN3OAM9RenderSubEP7OamAttriiii(
                _ZN3OAM18MM_VS_PLAYER_ICONSE[pl->unk8 + mm_icon_slot(idx) * 4],
                g_mm_iconX[idx], g_mm_iconY[idx], -1, 2);
            {
                u16 ang = this->unk21C + ((pl->unk8E ^ 0xffff) + 0x8001);
                int t = ((u16)(s16)ang >> 4) * 2;
                s16 sn = data_02082214[t + 1];
                this->unk200 = (s16)(((s64)sn * this->unk210 + 0x800) >> 0xc);
                s16 cn = data_02082214[t];
                this->unk204 = (s16)(((s64)cn * this->unk210 + 0x800) >> 0xc);
                this->unk208 = -this->unk204;
                this->unk20C = this->unk200;
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, _ZN3OAM8MM_ARROWE,
                    g_mm_iconX[idx], g_mm_iconY[idx], -1, 2, this->unk210, ang);
            }
            {
                for (i = 0; i < kMinimapWidePlayers; i++) {
                    if (i != idx) {
                        int y = g_mm_iconY[i];
                        if ((u16)(y + 0x10) < 0xe0) {
                            int x = g_mm_iconX[i];
                            if ((u16)(x + 0x10) < 0x120) {
                                if (data_ov002_02111148 == g_mm_mapID[i]) {
                                    _ZN3OAM9RenderSubEP7OamAttriiii(
                                        _ZN3OAM20MM_VS_PLAYER_ICONS_SE[mm_icon_slot(i) * 4 + data_0209f394[i]->unk8],
                                        x, y, -1, 2);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            u16 t = pl->unk6C8;
            if (t == 0 || ((t / 10) & 1) == 0) {
                if (this->unk255 == 0) {
                    int icon;
                    if (pl->HasNoCap() != 0)
                        icon = pl->unk6D9 * 4 + 3;
                    else
                        icon = pl->unk8 + pl->unk6D9 * 4;
                    _ZN3OAM9RenderSubEP7OamAttriiii(_ZN3OAM15MM_PLAYER_ICONSE[icon],
                        g_mm_iconX[idx], g_mm_iconY[idx], -1, 2);
                }
                if (data_0209caa0[2] & 0x80) {
                    u8 s = this->unk251;
                    if (s != 0) {
                        u16 ang;
                        if (s == 1)
                            ang = (pl->unk8E ^ 0xffff) + 0x8001;
                        else
                            ang = this->unk21C + ((pl->unk8E ^ 0xffff) + 0x8001);
                        {
                            int t2 = ((u16)(s16)ang >> 4) * 2;
                            s16 sn = data_02082214[t2 + 1];
                            this->unk200 = (s16)(((s64)sn * this->unk210 + 0x800) >> 0xc);
                            s16 cn = data_02082214[t2];
                            this->unk204 = (s16)(((s64)cn * this->unk210 + 0x800) >> 0xc);
                            this->unk208 = -this->unk204;
                            this->unk20C = this->unk200;
                            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, _ZN3OAM8MM_ARROWE,
                                g_mm_iconX[idx], g_mm_iconY[idx], -1, 2, this->unk210, ang);
                        }
                    }
                }
            }
        }

        {
            u8 *p = data_0209f37c;
            for (i = 0; i < 12; p++, i++) {
                s8 lvl = this->unk222[i];
                if (lvl >= 0) {
                    if (*p != 4) {
                        if (data_ov002_02111148 == lvl) {
                            *(u8 *)(((int)((char*)this + i) + 0x22e)) += data_0208ee44;
                            if ((u32)this->unk22E[i] >= 0xc)
                                this->unk22E[i] = 0;
                            {
                                int flag = 0;
                                u8 k = *p;
                                if (k != 3) {
                                    if (this->unk22E[i] % 12 > 4)
                                        flag = 1;
                                }
                                int icon = flag + k * 2;
                                {
                                    int y = this->unkD0[i];
                                    if ((u16)(y + 0x10) < 0xe0) {
                                        int x = this->unkA0[i];
                                        if ((u16)(x + 0x10) < 0x120) {
                                            _ZN3OAM9RenderSubEP7OamAttriiii(_ZN3OAM15MM_STAR_MARKERSE[icon],
                                                x, y, -1, 2);
                                        }
                                    }
                                }
                            }
                        } else {
                            this->unk22E[i] = 0;
                        }
                    } else {
                        if (data_0209f288 != 0) {
                            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, _ZN3OAM11MM_RED_COINE,
                                this->unkA0[i], this->unkD0[i], -1, 2, 0);
                        }
                    }
                } else {
                    this->unk22E[i] = 0;
                }
            }
        }

        {
            u8 *p1 = data_ov002_02111154;
            u8 *p2 = data_0209f370;
            int vs2 = (data_0209f2d8 == 1);
            void **tbl = (vs2 != 0) ? data_ov002_0210cac8 : data_ov002_0210c748;
            for (i = 0; i < 9; i++, p1++, p2++) {
                if (*p1 != 0) {
                    if (data_ov002_02111148 == this->unk23A[i]) {
                        int y = this->unk124[i];
                        if ((u16)(y + 0x10) < 0xe0) {
                            int x = this->unk100[i];
                            if ((u16)(x + 0x10) < 0x120) {
                                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, tbl[*p2], x, y, -1, 2, 0);
                            }
                        }
                    }
                }
            }
        }

        if (!(data_0209caa0[1] & 0x40) && (data_0209caa0[2] & 0x20000)) {
            *(u8 *)(((int)this + 0x256)) += 1;
            if ((u32)this->unk256 >= 5)
                this->unk256 = 0;
            if (this->unk248 >= 0) {
                if (data_0209d660 == 0 || !(data_020a0db0 & 8)) {
                    int sel = ((u32)this->unk256 < 2) ? 1 : 0;
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, _ZN3OAM12MM_STAR_KEYSE[sel],
                        this->unk178, this->unk17C, -1, 2, 0);
                }
            }
        }

        for (i = 0; i < 8; i++) {
            if (data_ov002_02111148 == this->unk249[i]) {
                int y = this->unk1A0[i];
                if ((u16)(y + 0x10) < 0xe0) {
                    int x = this->unk180[i];
                    if ((u16)(x + 0x10) < 0x120) {
                        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, _ZN3OAM13MM_SPIKE_BOMBE, x, y, -1, 2, 0);
                    }
                }
            }
        }
    }

    return 1;
}
