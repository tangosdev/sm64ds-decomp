//cpp
// _ZN5Stage9LC_RenderEv at 0x02023db8

extern "C" {
extern unsigned char LEVEL_CLEAR_TIMER;
extern int GAME_SPEED_RELATED;
extern unsigned char PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE;
extern int LoadFile(int handle);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void*, unsigned int, unsigned int);
extern void Deallocate(void* ptr);
extern char LEVEL_OF_LAST_COLLECTED_STAR;
extern int SublevelToLevel(int i);
extern int GetOwnerLanguage(void);
extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, int, int, int);
extern int data_ov002_0210cea8;
extern int data_ov002_0210cee8;
extern int data_ov002_0210cf28;
extern int data_ov002_0210cf78;
extern int _ZN3OAM18LC_CONGRATULATIONSE;
extern unsigned char NEW_COIN_HIGH_SCORE;
extern int data_ov002_0210ce90;
extern int data_ov002_0210ced0;
extern int data_ov002_0210cf08;
extern int data_ov002_0210cfa0;
extern int _ZN3OAM13LC_HIGH_SCOREE;
extern unsigned char LEVEL_CLEAR_SCREEN_STATE;
extern void _ZN5Stage20RenderBouncingArrowsEv(void);
}

extern "C" void _ZN5Stage9LC_RenderEv(void) {
    LEVEL_CLEAR_TIMER = LEVEL_CLEAR_TIMER + GAME_SPEED_RELATED;
    if (LEVEL_CLEAR_TIMER >= 0xc)
        LEVEL_CLEAR_TIMER = 0;

    if (PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE == 0)
        return;

    {
        void* p = (void*)LoadFile(0x25a);
        _ZN2GX11LoadOBJPlttEPKvjj((void*)((char*)p + (((unsigned int)LEVEL_CLEAR_TIMER >> 2) << 5)), 0x1c0, 0x10);
        Deallocate(p);
    }

    if (SublevelToLevel(LEVEL_OF_LAST_COLLECTED_STAR) >= 0xf && SublevelToLevel(LEVEL_OF_LAST_COLLECTED_STAR) < 0x15) {
        if (GetOwnerLanguage() == 5)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cea8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 4)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cee8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 3)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf28, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 2)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf78, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&_ZN3OAM18LC_CONGRATULATIONSE, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
    }

    if (NEW_COIN_HIGH_SCORE != 0) {
        if (PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE != 0 && SublevelToLevel(LEVEL_OF_LAST_COLLECTED_STAR) >= 0xf) {
            if (GetOwnerLanguage() == 5)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ce90, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 4)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ced0, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 3)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf08, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 2)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cfa0, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&_ZN3OAM13LC_HIGH_SCOREE, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            if (GetOwnerLanguage() == 5)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ce90, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 4)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ced0, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 3)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf08, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 2)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cfa0, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&_ZN3OAM13LC_HIGH_SCOREE, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }

    if (LEVEL_CLEAR_SCREEN_STATE == 3)
        _ZN5Stage20RenderBouncingArrowsEv();
}
