//cpp
extern "C" {

extern unsigned char data_0209f250;
extern unsigned char data_ov002_02111178;
extern signed char data_0209f2f4;

struct OamAttr;

extern OamAttr* _ZN3OAM10LIFE_ICONSE[];
extern OamAttr _ZN3OAM5TIMESE;
extern OamAttr* _ZN3OAM7NUMBERSE[];

struct HUDInfo {
    char pad[0x6d9];
    unsigned char field_6d9;
};
extern HUDInfo* data_0209f394[];

void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, OamAttr* attr, int x, int y, int priority, int rotation, int scaleX, int scaleY, int a, int mode);
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int sub, OamAttr* attr, int x, int y, int priority, int rotation, void* matrix);
void _ZN3OAM9RenderSubEP7OamAttriiii(OamAttr* attr, int x, int y, int priority, int rotation);

}

struct HUD {
    char pad0[0x6e];
    short xBase;
    char pad1[4];
    signed char digits[3];
    void CalculateDigits(unsigned short n);
    void RenderLifeCount();
};

void HUD::RenderLifeCount()
{
    unsigned char idx = data_0209f250;
    HUDInfo* info = data_0209f394[idx];
    unsigned char state = data_ov002_02111178;

    if (state >= 3 && state < 6) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, _ZN3OAM10LIFE_ICONSE[info->field_6d9], xBase, 0xa, -1, 1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, xBase + 0x10, 0xa, -1, 1, 0);
        CalculateDigits((unsigned short)data_0209f2f4);
        int x = xBase + 0x18;
        for (int i = 0; i < 3; i++) {
            signed char d = digits[i];
            if (d >= 0) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM7NUMBERSE[d], x, 2, -1, 1, 0);
                x += 9;
            }
        }
    } else {
        _ZN3OAM9RenderSubEP7OamAttriiii(_ZN3OAM10LIFE_ICONSE[info->field_6d9], xBase, 0xa, -1, 1);
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM5TIMESE, xBase + 0x10, 0xa, -1, 1, 0);
        CalculateDigits((unsigned short)data_0209f2f4);
        int i;
        int x = xBase + 0x18;
        for (i = 0; i < 3; i++) {
            signed char d = digits[i];
            if (d >= 0) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, _ZN3OAM7NUMBERSE[d], x, 2, -1, 1, 0);
                x += 9;
            }
        }
    }
}
