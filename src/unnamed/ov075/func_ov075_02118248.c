#include "types.h"
extern u8 data_0209fc58;
extern int data_ov075_0211d810;
extern int data_ov075_0211d870;
extern int data_ov075_0211d8b8;

extern void func_ov075_02119dc4(int a, int* data);
extern int func_0203da9c(void);
extern int TouchArea_Update(void* c, int id);
extern int func_ov075_02116d9c(int unused, int id);
extern void func_ov001_020ab41c(void* c);
extern void func_ov075_0211a148(char* c, int a, int b);
extern void func_02012790(int x);

/* dScEntry_c as this step reads it; mIcons follows dScEntry_c.h. An icon's
 * byte 0x10 is the dThIcon_c blink that func_ov001_020ab5b0 clears,
 * dThIcon_c::Behavior toggles when its countdown expires, and
 * func_ov001_020ab41c sets when clear. mSelectedIcon (0x281) is the icon the
 * pad picks: func_ov075_02117918 loads it from the player's save slot, lights
 * mIcons[mSelectedIcon] and picks the BG2 panel from it, and a confirm press
 * (func_ov075_02116d9c) acts on the icon it names. */
typedef struct EntryIcon {
    u8 pad_00[0x10];
    u8 mBlink; /* 0x10 */
    u8 pad_11[0x13];
} EntryIcon;

typedef struct EntryScene {
    u8 pad_000[0x70];
    EntryIcon mIcons[9]; /* 0x070 */
    u8 pad_1b4[0xcd];
    u8 mSelectedIcon; /* 0x281 */
} EntryScene;

void func_ov075_02118248(char* self)
{
    EntryScene *scene = (EntryScene *)self;
    int id;
    if (data_0209fc58 != 0) {
        func_ov075_02119dc4((int)self, &data_ov075_0211d810);
        return;
    }
    id = func_0203da9c();
    if (TouchArea_Update(&scene->mIcons[0], id) != 0 ||
        (func_ov075_02116d9c((int)self, id) != 0 && scene->mSelectedIcon == 0)) {
        scene->mIcons[1].mBlink = 0;
        func_ov001_020ab41c(&scene->mIcons[0]);
        func_ov075_0211a148(self, (int)&data_ov075_0211d870, 0x14);
        func_02012790(0x126);
        return;
    }
    if (TouchArea_Update(&scene->mIcons[1], id) == 0) {
        if (func_ov075_02116d9c((int)self, id) == 0)
            return;
        if (scene->mSelectedIcon != 1)
            return;
    }
    scene->mIcons[0].mBlink = 0;
    func_ov001_020ab41c(&scene->mIcons[1]);
    func_ov075_0211a148(self, (int)&data_ov075_0211d8b8, 0x14);
    func_02012790(0x119);
}
