//cpp
/* dScMgSlot1_c -- the slot machine minigame scene, its bet icon, and the
 * small helpers the scene drives.
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder.
 *
 * Blocked: the helpers are unnamed in symbols.txt and take their objects as
 * raw bytes. The three small embedded objects at 0x4684, 0x4690 and 0x469c
 * are padding in dScMgSlot1_c.h, so their helpers below use offsets.
 */

#include "dScMgSlot1_c.h"
#include "dScMgSlot3_c.h"
#include "Sound.h"
#include "OAM.h"
#include "dScMgBase_c.h"
#include "decl_common.h"
#include "private/ov006_slotgrid.h"
#include "types.h"


/* The reel state as func_ov006_0210c500 reads it. dScMgSlot1_c.h still has
 * one 0x15-byte array plus padding there, so it is not typed on the class. */
struct SlotReels {
    u8 pad[0x46c0];
    u8 reels[3][0x15];  /* symbol strip per reel */
    u8 stops[3];        /* where each reel stopped */
    u8 pad2[6];
    u8 unk_4708;
    u8 matchSymbol;
    u8 pad3;
    u8 wilds;           /* count of symbol 5, which matches anything */
};

extern "C" {
int GetGameLanguage(void);
extern u8 data_0209d454;
extern void RenderOamMainScreen(void*, int, int, int, int);
extern void* data_ov006_0213e6a8;
extern void func_ov004_020b1b08(void *c);
extern void func_ov001_020ab3f0(void *c);
void func_ov004_020af948(void* a, int b, int c, void* m);
extern void* data_ov006_0213e528[];
extern void** data_ov006_0213e5ec[];
int TouchArea_Update(void *c, int x);
void func_ov006_0210c2d4(void *c);
void func_ov004_020b1b40(int x);
extern void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);
extern int func_ov004_020ad8b8(void);
extern void func_ov006_0210c234(unsigned char* o);
extern void func_ov006_0210c180(void *o);
extern void func_ov006_0210c1a8(void *o);
extern void func_ov006_0210c218(void *o, s16 x, s16 y);
extern void func_ov006_0210c278(void *o);
extern void func_ov006_0210c2c0(void *o, int v);
extern int func_ov006_0210c500(void *self);
}

// @symbol _ZN12dScMgSlot1_c13OnYoshiTryEatEi
/* Slot 18. 0x4706 is padding in the header, so it is written by offset. */
void dScMgSlot1_c::OnYoshiTryEat(int i)
{
    char *raw = (char *)this;

    if (i == 4) {
        *(u8 *)(raw + 0x4706) = unk_4709;
    } else if (i == 3) {
        func_ov006_0210c638(raw);
    }
    func_ov006_0210c354(&mBetIcon);
    unk_46b4 = 0;
}

// @symbol func_ov006_0210c638
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c638(void *self)
{
    char *raw = (char *)self;
    *(s8 *)(raw + 0x4706) = -1;
    *(u8 *)(raw + 0x4707) = 1;
    *(u8 *)(raw + 0x4708) = 0;
    dScMgSlot1_c *scene = (dScMgSlot1_c *)self;
    scene->unk_0a8 = func_ov004_020ad8b8();
    scene->unk_0ac = scene->unk_0a8;
}
}

// @symbol func_ov006_0210c500
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_0210c500(void *self)
{
    struct SlotReels *p = (struct SlotReels *)self;
    int ok;
    int lead;
    int i;

    p->wilds = 0;
    ok = 1;
    for (i = 0; i < 3; i++) {
        if (p->reels[i][p->stops[i]] == 5) {
            p->wilds++;
        } else {
            lead = p->reels[i][p->stops[i]];
            break;
        }
    }
    if (p->wilds >= 3) {
        lead = 5;
    } else if (i < 2) {
        for (i = i + 1; i < 3; i++) {
            u8 sym = p->reels[i][p->stops[i]];
            if (sym == 5) {
                p->wilds++;
            } else if (lead != sym) {
                ok = 0;
            }
        }
    }
    if (ok) {
        if (p->unk_4708 < 2) {
            u8 *q = (u8 *)p + 0x4708;
            *q += 1;
        }
        p->matchSymbol = lead;
    }
    return ok;
}
}

// @symbol _ZN12dScMgSlot1_c15OnHitByMegaCharEv
/* Slot 27. Takes no argument: the ROM body never reads r1. The qualified
 * base call is the ROM's direct branch. */
void dScMgSlot1_c::OnHitByMegaChar()
{
    SetSubBg1Offset(0, 0);
    dScMgBase_c::OnHitByMegaChar();
}

// @symbol _ZN12dScMgSlot1_c19OnHitFromUnderneathEv
/* Slot 28. Nothing reads the return value, and neither body sets it on
 * purpose, so `int` is a guess; `void` compiles the same. */
int dScMgSlot1_c::OnHitFromUnderneath()
{
    dScMgBase_c::OnHitFromUnderneath();
    SetSubBg1Offset(0x100, 0);
}

// @symbol func_ov006_0210c478
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c478(char *icon) {
    func_ov001_020ab5b0(icon, 1, 0x90, 0xb0, 0x10, 8);
    *(int *)(icon + 0x18) = 6;
}
}

// @symbol _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv
void dScMgSlot1_c::betIcon_c::Behavior()
{
    int val;
    if (TouchArea_Update(this, -1))
        func_ov006_0210c2d4(this);
    val = unk_020;
    if (val != 0) {
        if ((val & 3) == 0) {
            func_ov004_020b1b40(1);
            Sound::PlayBank2_2D(0x149);
        }
        unk_020 -= 1;
    }
    dThIcon_c::Behavior();
}

// @symbol _ZN12dScMgSlot1_c9betIcon_c6RenderEv
void dScMgSlot1_c::betIcon_c::Render()
{
    OAM::RenderSub((OamAttr *)data_ov006_0213e528[unk_010], unk_004, unk_006, -1, 1);
    int i;
    int y = 0xb0;
    int zero = 0;
    for (i = 0; i < unk_01c; i++) {
        func_ov004_020af948(data_ov006_0213e5ec[GetGameLanguage()][2], 0xb0 + i * 0x10, y, (void *)zero);
    }
}

// @symbol func_ov006_0210c354
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c354(void *raw)
{
    dThIcon_c *icon = (dThIcon_c *)raw;
    icon->unk_01c = 0;
    icon->unk_020 = 0;
    if (icon->unk_014 >= 1)
        icon->unk_011 = 1;
}
}

// @symbol func_ov006_0210c2d4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2d4(void *c_)
{
    dThIcon_c *icon = (dThIcon_c *)c_;
    void *scene;
    int count;
    unsigned int sid;

    if (icon->unk_01c >= 3) {
        return;
    }
    if (icon->unk_010 != 0) {
        return;
    }
    scene = data_ov004_020beb68;
    count = (scene != 0) ? *(int *)((char *)scene + 0xa8) : 0;
    if (count <= 0) {
        return;
    }
    icon->unk_01c += 1;
    func_ov004_020b1b08((void *)1);
    func_ov001_020ab3f0(icon);
    sid = 0x163;
    Sound::PlayBank2_2D(sid);
}
}

// @symbol func_ov006_0210c2c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2c0(void *raw, int value) {
    int *words = (int *)raw;
    words[8] = (words[7] * value) << 2;
}
}

// @symbol func_ov006_0210c2b0
/* 0x4684 and 0x4690 each hold a blinking sprite: s16 x and y, a timer at
 * +4 and a visible flag at +8. This clears one, 0210c218 starts one, 0210c278
 * ticks it and 0210c234 draws it. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2b0(char *sprite)
{
    *(int *)(sprite + 0x4) = 0;
    *(char *)(sprite + 0x8) = 0;
}
}

// @symbol func_ov006_0210c278
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c278(void *o_)
{
    u8 *raw = (u8 *)o_;
    if (*(int *)(raw + 4) <= 0) return;
    int *timer = (int *)(raw + 4);
    *timer -= 1;
    if ((*(int *)(raw + 4) & 7) == 0) {
        *(raw + 8) ^= 1;
    }
}
}

// @symbol func_ov006_0210c234
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c234(unsigned char* raw){
    if (*(u8 *)(raw + 8) == 0) return;
    RenderOamMainScreen(&data_ov006_0213e6a8, *(short *)raw, *(short *)(raw + 2), -1, -1);
}
}

// @symbol func_ov006_0210c218
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c218(void *raw, s16 x, s16 y)
{
    u8 *sprite = (u8 *)raw;
    *(u16 *)(sprite + 0) = x;
    *(u16 *)(sprite + 2) = y;
    *(int *)(sprite + 4) = 0x48;
    *(u8 *)(sprite + 8) = 1;
}
}

// @symbol func_ov006_0210c208
/* 0x469c blinks bit 1 of data_0209d454 the same way: a timer and an on
 * flag. This clears it, 0210c180 starts it and 0210c1a8 ticks it. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c208(char *blink)
{
    *(int *)(blink + 0x0) = 0;
    *(char *)(blink + 0x4) = 0;
}
}

// @symbol func_ov006_0210c1a8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c1a8(void *o_)
{
    int *timer = (int *)o_;
    if (*timer <= 0)
        return;
    *timer -= 1;
    if ((*timer & 7) != 0)
        return;
    *((u8 *)timer + 4) ^= 1;
    if (*((u8 *)timer + 4) != 0)
        data_0209d454 |= 2;
    else
        data_0209d454 &= ~2;
}
}

// @symbol func_ov006_0210c180
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c180(void *raw){
    char *blink = (char *)raw;
    *(int *)blink = 0x48;
    *(u8 *)(blink + 4) = 1;
    data_0209d454 |= 2;
}
}

