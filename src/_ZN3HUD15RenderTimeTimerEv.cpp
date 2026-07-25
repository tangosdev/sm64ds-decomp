//cpp
// _ZN3HUD15RenderTimeTimerEv at 0x020fb96c (ov002)
struct OamAttr;

extern "C" {

extern unsigned char data_ov002_02111184;
extern int data_0209d4c8;

extern struct OamAttr data_ov002_0210ce80;
extern struct OamAttr _ZN3OAM4TIMEE;
extern struct OamAttr _ZN3OAM7MINUTESE;
extern struct OamAttr data_ov002_0210c6c0;
extern struct OamAttr* data_ov000_020aba70[];

extern int GetOwnerLanguage(void);
extern unsigned long long _ZN5Timer7GetTimeEv(void* thiz);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, struct OamAttr* attr, int x, int y, int a, int b, int sx, int sy, int c, int d);
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int sub, struct OamAttr* attr, int x, int y, int a, int b, void* m);

void _ZN3HUD15RenderTimeTimerEv(void* thisptr)
{
    unsigned long long t;
    unsigned long long min;
    unsigned long long sec;
    unsigned long long centi;

    if (data_ov002_02111184 == 0)
        return;

    t = _ZN5Timer7GetTimeEv(&data_0209d4c8) << 6;
    sec = t / 33514000;
    centi = t / 33514 % 1000 / 10;
    min = sec / 60;
    sec = sec % 60;

    if (min >= 100) {
        min = 99;
        sec = 59;
        centi = 99;
    }

    if (min / 10 != 0) {
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210ce80, 0xa4, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM4TIMEE, 0xa4, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        }
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, data_ov000_020aba70[min / 10], 0xb8, 0x16, -1, 1, 0);
    } else {
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210ce80, 0xac, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM4TIMEE, 0xac, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        }
    }

    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, data_ov000_020aba70[min % 10], 0xc0, 0x16, -1, 1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM7MINUTESE, 0xc4, 0x1e, -1, 1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, data_ov000_020aba70[sec / 10], 0xcf, 0x16, -1, 1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, data_ov000_020aba70[sec % 10], 0xd7, 0x16, -1, 1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &data_ov002_0210c6c0, 0xdb, 0x1e, -1, 1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, data_ov000_020aba70[centi / 10], 0xe8, 0x16, -1, 1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, data_ov000_020aba70[centi % 10], 0xf0, 0x16, -1, 1, 0);
}

}
