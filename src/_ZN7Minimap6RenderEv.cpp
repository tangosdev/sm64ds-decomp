//cpp
#include "types.h"
#include "Minimap.h"
#pragma opt_strength_reduction off
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
    int j;
    u8 idx = data_0209f250;

    if ((u8)(d | (b | a)) == 0 || !(data_0209caa0[2] & 0x80) || (a != 0 && (u32)data_0209f2d4 < 3)) {
        Player *pl = data_0209f394[idx];
        int vs = (data_0209f2d8 == 1);
        if (vs != 0) {
            _ZN3OAM9RenderSubEP7OamAttriiii(
                _ZN3OAM18MM_VS_PLAYER_ICONSE[pl->unk8 + idx * 4],
                this->mPlayerIconX[idx], this->mPlayerIconY[idx], -1, 2);
            {
                u16 ang = (s16)this->mAngle + ((pl->unk8E ^ 0xffff) + 0x8001);
                int t = ((u16)(s16)ang >> 4) * 2;
                s16 sn = data_02082214[t + 1];
                this->mArrowMatrixA = (s16)(((s64)sn * this->mArrowScale + 0x800) >> 0xc);
                s16 cn = data_02082214[t];
                this->mArrowMatrixB = (s16)(((s64)cn * this->mArrowScale + 0x800) >> 0xc);
                this->mArrowMatrixC = -this->mArrowMatrixB;
                this->mArrowMatrixD = this->mArrowMatrixA;
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, _ZN3OAM8MM_ARROWE,
                    this->mPlayerIconX[idx], this->mPlayerIconY[idx], -1, 2, this->mArrowScale, ang);
            }
            {
                for (i = 0, j = 0; i < 4; i++, j += 4) {
                    if (i != idx) {
                        int y = this->mPlayerIconY[i];
                        if ((u16)(y + 0x10) < 0xe0) {
                            int x = this->mPlayerIconX[i];
                            if ((u16)(x + 0x10) < 0x120) {
                                if (data_ov002_02111148 == this->mPlayerMapIDs[i]) {
                                    _ZN3OAM9RenderSubEP7OamAttriiii(
                                        _ZN3OAM20MM_VS_PLAYER_ICONS_SE[j + data_0209f394[i]->unk8],
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
                if (this->mInIntroCutscene == 0) {
                    int icon;
                    if (pl->HasNoCap() != 0)
                        icon = pl->unk6D9 * 4 + 3;
                    else
                        icon = pl->unk8 + pl->unk6D9 * 4;
                    _ZN3OAM9RenderSubEP7OamAttriiii(_ZN3OAM15MM_PLAYER_ICONSE[icon],
                        this->mPlayerIconX[idx], this->mPlayerIconY[idx], -1, 2);
                }
                if (data_0209caa0[2] & 0x80) {
                    u8 s = this->mArrowType;
                    if (s != 0) {
                        u16 ang;
                        if (s == 1)
                            ang = (pl->unk8E ^ 0xffff) + 0x8001;
                        else
                            ang = (s16)this->mAngle + ((pl->unk8E ^ 0xffff) + 0x8001);
                        {
                            int t2 = ((u16)(s16)ang >> 4) * 2;
                            s16 sn = data_02082214[t2 + 1];
                            this->mArrowMatrixA = (s16)(((s64)sn * this->mArrowScale + 0x800) >> 0xc);
                            s16 cn = data_02082214[t2];
                            this->mArrowMatrixB = (s16)(((s64)cn * this->mArrowScale + 0x800) >> 0xc);
                            this->mArrowMatrixC = -this->mArrowMatrixB;
                            this->mArrowMatrixD = this->mArrowMatrixA;
                            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, _ZN3OAM8MM_ARROWE,
                                this->mPlayerIconX[idx], this->mPlayerIconY[idx], -1, 2, this->mArrowScale, ang);
                        }
                    }
                }
            }
        }

        {
            u8 *p = data_0209f37c;
            for (i = 0; i < 12; p++, i++) {
                s8 lvl = this->mStarMapIDs[i];
                if (lvl >= 0) {
                    if (*p != 4) {
                        if (data_ov002_02111148 == lvl) {
                            this->mStarIconAnimFrame[i] += data_0208ee44;
                            if ((u32)this->mStarIconAnimFrame[i] >= 0xc)
                                this->mStarIconAnimFrame[i] = 0;
                            {
                                int flag = 0;
                                u8 k = *p;
                                if (k != 3) {
                                    if (this->mStarIconAnimFrame[i] % 12 > 4)
                                        flag = 1;
                                }
                                int icon = flag + k * 2;
                                {
                                    int y = this->mStarIconY[i];
                                    if ((u16)(y + 0x10) < 0xe0) {
                                        int x = this->mStarIconX[i];
                                        if ((u16)(x + 0x10) < 0x120) {
                                            _ZN3OAM9RenderSubEP7OamAttriiii(_ZN3OAM15MM_STAR_MARKERSE[icon],
                                                x, y, -1, 2);
                                        }
                                    }
                                }
                            }
                        } else {
                            this->mStarIconAnimFrame[i] = 0;
                        }
                    } else {
                        if (data_0209f288 != 0) {
                            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, _ZN3OAM11MM_RED_COINE,
                                this->mStarIconX[i], this->mStarIconY[i], -1, 2, 0);
                        }
                    }
                } else {
                    this->mStarIconAnimFrame[i] = 0;
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
                    if (data_ov002_02111148 == this->mCapMapIDs[i]) {
                        int y = this->mCapIconY[i];
                        if ((u16)(y + 0x10) < 0xe0) {
                            int x = this->mCapIconX[i];
                            if ((u16)(x + 0x10) < 0x120) {
                                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, tbl[*p2], x, y, -1, 2, 0);
                            }
                        }
                    }
                }
            }
        }

        if (!(data_0209caa0[1] & 0x40) && (data_0209caa0[2] & 0x20000)) {
            this->mStarKeyBlinkTimer += 1;
            if ((u32)this->mStarKeyBlinkTimer >= 5)
                this->mStarKeyBlinkTimer = 0;
            if (this->mStarKeyMapID >= 0) {
                if (data_0209d660 == 0 || !(data_020a0db0 & 8)) {
                    int sel = ((u32)this->mStarKeyBlinkTimer < 2) ? 1 : 0;
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, _ZN3OAM12MM_STAR_KEYSE[sel],
                        this->mStarKeyIconX, this->mStarKeyIconY, -1, 2, 0);
                }
            }
        }

        for (i = 0; i < 8; i++) {
            if (data_ov002_02111148 == this->mSpikeBombMapIDs[i]) {
                int y = this->mSpikeBombIconY[i];
                if ((u16)(y + 0x10) < 0xe0) {
                    int x = this->mSpikeBombIconX[i];
                    if ((u16)(x + 0x10) < 0x120) {
                        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, _ZN3OAM13MM_SPIKE_BOMBE, x, y, -1, 2, 0);
                    }
                }
            }
        }
    }

    return 1;
}
