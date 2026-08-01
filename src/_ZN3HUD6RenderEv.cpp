//cpp
// @symbol _ZN3HUD6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HUD.h"
// _ZN3HUD6RenderEv at 0x020fd5e0
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
extern unsigned char data_0209f2d8;
extern unsigned char data_0209f2c4;
extern unsigned char data_0209f20c;
extern unsigned char data_0209f294;
extern unsigned char data_0209f204;
extern int data_0209caa0[];
extern unsigned char data_0209f284;
extern unsigned char data_0209f2d4;

void _ZN3HUD13RenderVsTimerEv(void *);
void _ZN3HUD15RenderStarCountEv(void *);
void _ZN3HUD15RenderCoinCountEv(void *);
void _ZN3HUD19RenderCameraButtonsEv(void *);
void _ZN3HUD17RenderHealthMeterEv(void *);
int _ZN5Event6GetBitEj(unsigned int);
void _ZN3HUD14RenderRedCoinsEv(void *);
void _ZN3HUD17RenderSilverStarsEv(void *);
void _ZN3HUD15RenderTimeTimerEv(void *);
void _ZN5Stage20RenderBouncingArrowsEv(void);
void _ZN3HUD15RenderLifeCountEv(void *);
}

int HUD::Render()
{
    int b = (data_0209f2d8 == 1);
    if (b) {
        if (data_0209fc9c != 0) goto end;
        if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) == 0) {
            _ZN3HUD13RenderVsTimerEv(((void *)this));
            _ZN3HUD15RenderStarCountEv(((void *)this));
            _ZN3HUD15RenderCoinCountEv(((void *)this));
            _ZN3HUD19RenderCameraButtonsEv(((void *)this));
        } else {
            if (data_0209f204 != 0) {
                _ZN3HUD13RenderVsTimerEv(((void *)this));
            }
        }
    } else {
        if ((data_0209caa0[2] & 0x80) == 0) goto end;
        unsigned char v = data_0209f20c;
        if (((data_0209f2c4 | v | data_0209f294) & 0xff) == 0) {
            _ZN3HUD17RenderHealthMeterEv(((void *)this));
            if (_ZN5Event6GetBitEj(0x1d) == 0) {
                _ZN3HUD15RenderCoinCountEv(((void *)this));
                _ZN3HUD14RenderRedCoinsEv(((void *)this));
                _ZN3HUD17RenderSilverStarsEv(((void *)this));
                _ZN3HUD15RenderTimeTimerEv(((void *)this));
            }
            if (data_0209f284 != 0) {
                _ZN5Stage20RenderBouncingArrowsEv();
            }
        } else {
            if (v != 0) {
                _ZN3HUD15RenderLifeCountEv(((void *)this));
                _ZN3HUD15RenderStarCountEv(((void *)this));
            }
        }
        unsigned char v2 = data_0209f20c;
        if ((data_0209f2c4 | v2 | data_0209f294) & 0xff) {
            if (v2 == 0) goto end;
            if (data_0209f2d4 >= 3) goto end;
        }
        _ZN3HUD15RenderStarCountEv(((void *)this));
        _ZN3HUD19RenderCameraButtonsEv(((void *)this));
        _ZN3HUD15RenderLifeCountEv(((void *)this));
    }
end:
    return 1;
}
