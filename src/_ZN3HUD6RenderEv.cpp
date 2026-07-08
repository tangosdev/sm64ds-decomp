//cpp
// _ZN3HUD6RenderEv at 0x020fd5e0
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
extern unsigned char CURRENT_GAMEMODE;
extern unsigned char CONNECTION_ERROR;
extern unsigned char GAME_PAUSED;
extern unsigned char PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE;
extern unsigned char GAME_FROZEN;
extern unsigned char VS_TIME_UP;
extern int SAVE_DATA[];
extern unsigned char BOUNCING_MSG_ARROWS_VISIBLE;
extern unsigned char LEVEL_CLEAR_SCREEN_STATE;

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

int _ZN3HUD6RenderEv(void *self) {
    int b = (CURRENT_GAMEMODE == 1);
    if (b) {
        if (CONNECTION_ERROR != 0) goto end;
        if (((GAME_PAUSED | PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE | GAME_FROZEN) & 0xff) == 0) {
            _ZN3HUD13RenderVsTimerEv(self);
            _ZN3HUD15RenderStarCountEv(self);
            _ZN3HUD15RenderCoinCountEv(self);
            _ZN3HUD19RenderCameraButtonsEv(self);
        } else {
            if (VS_TIME_UP != 0) {
                _ZN3HUD13RenderVsTimerEv(self);
            }
        }
    } else {
        if ((SAVE_DATA[2] & 0x80) == 0) goto end;
        unsigned char v = PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE;
        if (((GAME_PAUSED | v | GAME_FROZEN) & 0xff) == 0) {
            _ZN3HUD17RenderHealthMeterEv(self);
            if (_ZN5Event6GetBitEj(0x1d) == 0) {
                _ZN3HUD15RenderCoinCountEv(self);
                _ZN3HUD14RenderRedCoinsEv(self);
                _ZN3HUD17RenderSilverStarsEv(self);
                _ZN3HUD15RenderTimeTimerEv(self);
            }
            if (BOUNCING_MSG_ARROWS_VISIBLE != 0) {
                _ZN5Stage20RenderBouncingArrowsEv();
            }
        } else {
            if (v != 0) {
                _ZN3HUD15RenderLifeCountEv(self);
                _ZN3HUD15RenderStarCountEv(self);
            }
        }
        unsigned char v2 = PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE;
        if ((GAME_PAUSED | v2 | GAME_FROZEN) & 0xff) {
            if (v2 == 0) goto end;
            if (LEVEL_CLEAR_SCREEN_STATE >= 3) goto end;
        }
        _ZN3HUD15RenderStarCountEv(self);
        _ZN3HUD19RenderCameraButtonsEv(self);
        _ZN3HUD15RenderLifeCountEv(self);
    }
end:
    return 1;
}
}
