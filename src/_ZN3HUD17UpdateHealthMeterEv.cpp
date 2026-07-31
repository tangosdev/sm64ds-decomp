//cpp
// @symbol _ZN3HUD17UpdateHealthMeterEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HUD.h"
extern "C" {
extern int _ZN5Event6GetBitEj(unsigned int bit);
extern void func_02012790(int id);
extern int _ZN6Player7IsInAirEv(void *p);

extern unsigned char data_0209f250;
extern unsigned char data_0209f2c4;
extern unsigned char data_0209f20c;
extern unsigned char data_0209f294;
extern char *data_0209f394[];
extern int data_0208ee44;
extern unsigned char data_ov002_0211117c;
}

void HUD::UpdateHealthMeter()
{
    int chr = data_0209f250;
    char *player = data_0209f394[chr];
    unsigned int health = (data_02092144[chr] >> 8) & 0xff;
    if ((unsigned char)(data_0209f2c4 | data_0209f20c | data_0209f294) != 0)
        return;
    if (mHealthMeterHoldTimer != 0)
        *(unsigned short *)((((int)((char *)this)) + 0x6a) & 0xFFFFFFFFFFFFFFFFLL) -= data_0208ee44;
    if (mHealthTickTimer != 0)
        *(unsigned short *)((((int)((char *)this)) + 0x6c) & 0xFFFFFFFFFFFFFFFFLL) -= data_0208ee44;
    if (_ZN5Event6GetBitEj(0x1d)) {
        unsigned char s = mHealthMeterState;
        if (s != 0) {
            if (s < 5)
                mHealthMeterState = 5;
        }
    }
    switch (mHealthMeterState) {
    case 0:
        if (*(unsigned char *)(player + 0x706) != 0) {
            mHealthMeterY = 0x39;
            mHealthMeterState = 1;
            return;
        }
        if (data_ov002_0211117c <= health)
            return;
        mHealthMeterY = 0x39;
        data_ov002_0211117c = *(volatile unsigned char *)&data_ov002_0211117c - 1;
        mHealthTickTimer = 0x1e;
        mHealthMeterHoldTimer = 0x5a;
        mHealthMeterState = 1;
        return;
    case 1:
        if (mHealthMeterHoldTimer == 0) {
            if (mHealthMeterY != 0x19) {
                *(short *)((((int)((char *)this)) + 0x68) & 0xFFFFFFFFFFFFFFFFLL) -= 4;
                if (mHealthMeterY <= 0x19)
                    mHealthMeterY = 0x19;
            }
        }
        if (mHealthTickTimer != 0)
            return;
        {
            unsigned char cur = data_ov002_0211117c;
            if (cur < health) {
                if ((int)(health - cur) > 1) {
                    data_ov002_0211117c = cur + 1;
                    func_02012790(0xd);
                    mHealthTickTimer = 0x10;
                    return;
                }
                data_ov002_0211117c = health;
                func_02012790(0xd);
                if (*(unsigned char *)(player + 0x706) != 0)
                    return;
                if (health != 8)
                    return;
                if (data_ov002_02111178 == 1) {
                    data_ov002_02111178 = 3;
                    mHealthMeterState = 2;
                } else {
                    mHealthMeterHoldTimer = 0x3c;
                    mHealthMeterState = 4;
                }
                return;
            }
            if (cur > health) {
                data_ov002_0211117c = cur - 1;
                mHealthTickTimer = 0x1e;
                return;
            }
            if (cur != 8)
                return;
            if (*(unsigned char *)(player + 0x706) != 0)
                return;
            if (_ZN6Player7IsInAirEv(player))
                return;
            if (data_ov002_02111178 == 1) {
                data_ov002_02111178 = 3;
                mHealthMeterState = 2;
            } else {
                mHealthMeterHoldTimer = 0x3c;
                mHealthMeterState = 4;
            }
        }
        return;
    case 2:
        if (data_ov002_02111178 == 4) {
            mHealthMeterHoldTimer = 0x3c;
            mHealthMeterState = 3;
        }
        return;
    case 3:
        if (mHealthMeterHoldTimer != 0)
            return;
        GiveLives(-1);
        func_02012790(0x40);
        data_ov002_02111178 = 5;
        mHealthMeterHoldTimer = 0x3c;
        mHealthMeterState = 4;
        return;
    case 4:
    case 5:
        if (mHealthMeterHoldTimer != 0)
            return;
        *(short *)((((int)((char *)this)) + 0x68) & 0xFFFFFFFFFFFFFFFFLL) -= 4;
        if (mHealthMeterY >= -0x18)
            return;
        mHealthMeterY = -0x18;
        if (mHealthMeterState == 4)
            mHealthMeterState = 0;
        else
            mHealthMeterState = 6;
        return;
    case 6:
        if (_ZN5Event6GetBitEj(0x1d) == 0)
            mHealthMeterState = 1;
        return;
    }
}
