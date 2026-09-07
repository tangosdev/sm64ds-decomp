//cpp
#include "Stage.h"
// _ZN5Stage9LC_RenderEv at 0x02023db8

extern "C" {
extern unsigned char data_0209f2a8;
extern int data_0208ee44;
extern unsigned char data_0209f20c;
extern int LoadFile(int handle);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void*, unsigned int, unsigned int);
extern void Deallocate(void* ptr);
extern char data_02092124;
extern int SublevelToLevel(int i);
extern int GetOwnerLanguage(void);
extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, int, int, int);
extern int data_ov002_0210cea8;
extern int data_ov002_0210cee8;
extern int data_ov002_0210cf28;
extern int data_ov002_0210cf78;
extern int data_ov002_0210cb14;
extern unsigned char data_0209f2b0;
extern int data_ov002_0210ce90;
extern int data_ov002_0210ced0;
extern int data_ov002_0210cf08;
extern int data_ov002_0210cfa0;
extern int data_ov002_0210caf4;
extern unsigned char data_0209f2d4;
extern void _ZN5Stage20RenderBouncingArrowsEv(void);
}

void Stage::LC_Render() {
    data_0209f2a8 = data_0209f2a8 + data_0208ee44;
    if (data_0209f2a8 >= 0xc)
        data_0209f2a8 = 0;

    if (data_0209f20c == 0)
        return;

    {
        void* p = (void*)LoadFile(0x25a);
        _ZN2GX11LoadOBJPlttEPKvjj((void*)((char*)p + (((unsigned int)data_0209f2a8 >> 2) << 5)), 0x1c0, 0x10);
        Deallocate(p);
    }

    if (SublevelToLevel(data_02092124) >= 0xf && SublevelToLevel(data_02092124) < 0x15) {
        if (GetOwnerLanguage() == 5)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cea8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 4)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cee8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 3)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf28, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 2)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf78, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cb14, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
    }

    if (data_0209f2b0 != 0) {
        if (data_0209f20c != 0 && SublevelToLevel(data_02092124) >= 0xf) {
            if (GetOwnerLanguage() == 5)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ce90, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 4)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ced0, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 3)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf08, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 2)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cfa0, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210caf4, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
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
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210caf4, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }

    if (data_0209f2d4 == 3)
        RenderBouncingArrows();
}
