//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" {
void SetBg0Offset(int a, int b);
void LoadFont(u8 arg);
int GetOwnerLanguage(void);
char *_ZN2G213GetBG2CharPtrEv(void);
u32 LoadCompressedFileAt(u16 fileID, void *target);
int LoadFileAt(int handle, int dest);
char *_ZN2G212GetBG2ScrPtrEv(void);
void LoadControllerModeText(int a);
void LoadFont3D(void);
char *_ZN3G2S13GetBG0CharPtrEv(void);
char *_ZN3G2S12GetBG0ScrPtrEv(void);
char *_ZN3G2S12GetBG1ScrPtrEv(void);
char *_ZN3G2S12GetBG2ScrPtrEv(void);
void MultiStore16(u16 val, char *dst, int nbytes);
void SetSubBg0Offset(int a, int b);
void SetSubBg1Offset(int a, int b);
void SetSubBg2Offset(int a, int b);
int SublevelToLevel(int i);

extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern int data_0209fc48;
extern u8 data_0209d454;
extern int data_0209fc68;
}

class Stage {
public:
    static void LoadGraphics2D(bool b, int i);
};

#define BG2CNT     (*(volatile u16 *)0x400000c)
#define BG3CNT     (*(volatile u16 *)0x400000e)
#define BG0CNT_SUB (*(volatile u16 *)0x4001008)
#define BG1CNT_SUB (*(volatile u16 *)0x400100a)
#define BG2CNT_SUB (*(volatile u16 *)0x400100c)

void Stage::LoadGraphics2D(bool b, int i)
{
    char *r4;
    char *r7;

    BG3CNT &= ~3;
    BG3CNT = (BG3CNT & 0x43) | 0x1100;
    BG3CNT &= ~0x40;
    SetBg0Offset(0, 0);

    BG2CNT = (BG2CNT & ~3) | 1;
    BG2CNT &= ~0x40;
    BG2CNT = (BG2CNT & 0x43) | 0xd10;

    if (b == 0) {
        LoadFont(0);
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xb00f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xac0f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xa80f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa40f, _ZN2G213GetBG2CharPtrEv());
        } else {
            LoadCompressedFileAt(0xa00f, _ZN2G213GetBG2CharPtrEv());
        }
        LoadFileAt(0x980b, 0x5000000);
        r4 = _ZN2G212GetBG2ScrPtrEv();
        LoadCompressedFileAt(0x9808, r4);
        LoadCompressedFileAt(0x9809, r4 + 0x800);
        LoadCompressedFileAt(0x980a, r4 + 0x1000);
        LoadControllerModeText(0x280);
    } else {
        LoadFont3D();
    }

    if ((int)(data_0209f2d8 == 0) != 0
        && (data_0209caa0[2] & 0x80) == 0
        && (int)(data_0209fc48 != 0) == 0) {
        BG0CNT_SUB &= ~3;
        BG0CNT_SUB &= ~0x40;
        BG0CNT_SUB = (BG0CNT_SUB & 0x43) | 0xd000;
        BG1CNT_SUB &= ~3;
        BG1CNT_SUB &= ~0x40;
        BG1CNT_SUB = (BG1CNT_SUB & 0x43) | 0xd400;
        BG1CNT_SUB &= ~3;
        BG1CNT_SUB &= ~0x40;
        BG2CNT_SUB = (BG2CNT_SUB & 0x43) | 0x1800;

        LoadCompressedFileAt(0x239, _ZN3G2S13GetBG0CharPtrEv());
        LoadFileAt(0x23c, 0x5000400);
        LoadCompressedFileAt(0x23b, _ZN3G2S12GetBG0ScrPtrEv());
        LoadCompressedFileAt(0x23a, _ZN3G2S12GetBG1ScrPtrEv());
        LoadCompressedFileAt(0x23d, (void *)0x6600000);
        LoadFileAt(0x23e, 0x5000600);
        {
            char *scr2 = _ZN3G2S12GetBG2ScrPtrEv();
            volatile u16 sp0 = 0x10d0;
            MultiStore16(sp0, scr2, 0x800);
        }
        data_0209d454 = 0x17;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        SetSubBg2Offset(0, 0);
        return;
    }

    BG0CNT_SUB &= ~3;
    BG0CNT_SUB &= ~0x40;
    BG0CNT_SUB = (BG0CNT_SUB & 0x43) | 0x1200;
    BG1CNT_SUB &= ~3;
    BG1CNT_SUB &= ~0x40;
    BG1CNT_SUB = (BG1CNT_SUB & 0x43) | 0xc00;
    BG2CNT_SUB = (BG2CNT_SUB & ~3) | 2;
    BG2CNT_SUB &= ~0x40;
    BG2CNT_SUB = (BG2CNT_SUB & 0x43) | 0x1300;

    if (GetOwnerLanguage() == 5) {
        LoadCompressedFileAt(0xb00d, _ZN3G2S13GetBG0CharPtrEv());
    } else if (GetOwnerLanguage() == 4) {
        LoadCompressedFileAt(0xac0d, _ZN3G2S13GetBG0CharPtrEv());
    } else if (GetOwnerLanguage() == 3) {
        LoadCompressedFileAt(0xa80d, _ZN3G2S13GetBG0CharPtrEv());
    } else if (GetOwnerLanguage() == 2) {
        LoadCompressedFileAt(0xa40d, _ZN3G2S13GetBG0CharPtrEv());
    } else {
        LoadCompressedFileAt(0xa00d, _ZN3G2S13GetBG0CharPtrEv());
    }

    LoadFileAt(0x9807, 0x5000400);
    r4 = _ZN3G2S12GetBG1ScrPtrEv();
    LoadCompressedFileAt(0x9804, r4 + 0x800);
    LoadCompressedFileAt(0x9803, r4 + 0x1000);

    if (GetOwnerLanguage() == 5) {
        LoadCompressedFileAt(0xb009, r4 + 0x1800);
        LoadCompressedFileAt(0xb00b, r4 + 0x2000);
    } else if (GetOwnerLanguage() == 4) {
        LoadCompressedFileAt(0xac09, r4 + 0x1800);
        LoadCompressedFileAt(0xac0b, r4 + 0x2000);
    } else if (GetOwnerLanguage() == 3) {
        LoadCompressedFileAt(0xa809, r4 + 0x1800);
        LoadCompressedFileAt(0xa80b, r4 + 0x2000);
    } else if (GetOwnerLanguage() == 2) {
        LoadCompressedFileAt(0xa409, r4 + 0x1800);
        LoadCompressedFileAt(0xa40b, r4 + 0x2000);
    } else {
        LoadCompressedFileAt(0xa009, r4 + 0x1800);
        LoadCompressedFileAt(0xa00b, r4 + 0x2000);
    }

    r7 = _ZN3G2S12GetBG2ScrPtrEv();
    LoadCompressedFileAt(0x9805, r7);
    LoadCompressedFileAt(0x9806, r7 + 0x1000);

    if (b != 0) {
        if (data_0209fc68 != 0) {
            return;
        }
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xb00f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xac0f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xa80f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa40f, _ZN2G213GetBG2CharPtrEv());
        } else {
            LoadCompressedFileAt(0xa00f, _ZN2G213GetBG2CharPtrEv());
        }
        LoadFileAt(0x980b, 0x5000000);
        LoadCompressedFileAt(0x25c, _ZN2G212GetBG2ScrPtrEv() + 0x1800);
        LoadCompressedFileAt(0x9801, r4 + 0x2800);
        return;
    }

    if (SublevelToLevel(i) == 0x1d) {
        volatile u16 sp2 = 0;
        MultiStore16(sp2, r4, 0x40);
        LoadCompressedFileAt(0x230, r4 + 0x40);
        LoadCompressedFileAt(0x22f, _ZN2G212GetBG2ScrPtrEv() + 0x1800);
        LoadCompressedFileAt(0x25c, _ZN2G212GetBG2ScrPtrEv() + 0x2800);
        LoadCompressedFileAt(0x9802, r4 + 0x2800);
        return;
    }

    LoadCompressedFileAt(0x25c, _ZN2G212GetBG2ScrPtrEv() + 0x1800);
    LoadCompressedFileAt(0x230, r4);
}
