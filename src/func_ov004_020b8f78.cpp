//cpp
#include "types.h"
struct Sub { virtual void f(); };

extern "C" {
int TouchArea_Update(void *p, int f);
void func_ov001_020ab3f0(void *p);
void func_02012790(int a);
int GetSoundMode(void);
void SetSoundMode(int mode);
void func_ov001_020ab41c(void *c);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
u8 DecIfAbove0_Byte(u8 *p);
void func_ov004_020b91fc(char *c);
}

/* THE RETURN TYPE IS int AND IT IS LOAD-BEARING ON THE HOST, run mg14 lane
 * RESULTS. The tail is `return *(u8 *)(c + 0x124);` and mwccarm compiles that
 * to `ldrb r0, [r4, #0x124]` whichever type this says, so the ROM bytes are
 * identical either way -- verified MATCH at 2004/b56 and the 1.2 trio with
 * both spellings. MSVC is not: with a u8 return it emits `mov al, [esi+124h]`
 * and leaves the top 24 bits of EAX holding whatever the switch above left
 * there (0xFFFFFF on the state-0 path, from the two `sub eax,1` compares), and
 * the ONE caller -- func_ov004_020aeb24, the results panel's tick -- declares
 * this int and tests the full register. So the minigame results panel took its
 * early return on every frame, the three ApproachLinear2 slides never ran, and
 * the play-again buttons sat at their off-screen seeds forever.
 *
 * That is the whole defect behind "the play-again row never appears". Do not
 * narrow this back to u8 to match the field it returns. */
extern "C" int func_ov004_020b8f78(char *c)
{
    u8 st = *(u8 *)(c + 0x124);
    switch (st) {
    case 1:
        if (TouchArea_Update(c + 0xb4, -1)) {
            func_ov001_020ab3f0(c + 0xb4);
            *(u8 *)(c + 0x125) = 0x14;
            *(u8 *)(c + 0x124) = 2;
            func_02012790(0x65);
        } else {
            if (*(u8 *)(c + 0x59) != 0) {
                int r6 = 0;
                int mode = GetSoundMode();
                if (TouchArea_Update(c + 0x48, -1) || TouchArea_Update(c + 0x24, -1)
                    || TouchArea_Update(c + 0xd8, -1)) {
                    if (mode == 2) mode = 0;
                    else mode = (mode + 1) & 0xff;
                    *(u8 *)(c + 0x34) = 0;
                    func_ov001_020ab3f0(c + 0x24);
                    r6 = 1;
                } else if (TouchArea_Update(c, -1)) {
                    if (mode == 0) mode = 2;
                    else mode = (mode - 1) & 0xff;
                    *(u8 *)(c + 0x10) = 0;
                    func_ov001_020ab3f0(c);
                    r6 = 1;
                }
                if (r6 != 0) {
                    SetSoundMode(mode);
                    *(int *)(c + 0x68) = mode;
                    func_ov001_020ab41c(c + 0x48);
                    func_02012790(0x64);
                }
            }
            if (*(u8 *)(c + 0x7d) != 0 && *(u8 *)(c + 0xa1) != 0) {
                int r5 = 0;
                if (TouchArea_Update(c + 0x6c, -1)) {
                    r5 = -1;
                } else if (TouchArea_Update(c + 0x90, -1)) {
                    r5 = 1;
                } else if (TouchArea_Update(c + 0xfc, -1)) {
                    r5 = (*(u8 *)(c + 0x7c) != 0) ? 1 : -1;
                }
                if (r5 < 0) {
                    if (*(u8 *)(c + 0x7c) != 0) {
                        func_02012790(0x67);
                    } else {
                        TurnBacklightOn();
                        func_02012790(0x66);
                    }
                    func_ov001_020ab41c(c + 0x6c);
                    *(u8 *)(c + 0xa0) = 0;
                } else if (r5 > 0) {
                    if (*(u8 *)(c + 0xa0) != 0) {
                        func_02012790(0x67);
                    } else {
                        TurnBacklightOff();
                        func_02012790(0x66);
                    }
                    *(u8 *)(c + 0x7c) = 0;
                    func_ov001_020ab41c(c + 0x90);
                }
            }
        }
        break;
    case 2:
        if (DecIfAbove0_Byte((u8 *)(c + 0x125)) == 0)
            func_ov004_020b91fc(c);
        break;
    }

    {
        int n = *(int *)(c + 0x120);
        int r6 = 0;
        if (n > 0) {
            char *r5 = c;
            do {
                ((Sub *)r5)->f();
                r6++;
                r5 += 0x24;
            } while (r6 < *(int *)(c + 0x120));
        }
    }
    return *(u8 *)(c + 0x124);
}
