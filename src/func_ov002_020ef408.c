#include "types.h"
extern s16 data_02082214[];

extern s32 func_ov002_020efebc(void *c);
extern u16 DecIfAbove0_Short(u16 *p);
extern u8 DecIfAbove0_Byte(u8 *p);
extern void func_ov002_020efbdc(void *c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
extern s32 func_ov002_020efe68(void *c);
extern void _Z14ApproachLinearRsss(s16 *p, s16 target, s16 step);
extern s32 func_ov002_020eff04(void *c);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);

void func_ov002_020ef408(void *arg0)
{
    char *c = (char *)arg0;
    s32 raw;
    s16 v;

    if (func_ov002_020efebc(c) != 0) {
        if (*(u8 *)(c + 0x42a) == 0) {
            if (DecIfAbove0_Short((u16 *)(c + 0x428)) == 0) {
                func_ov002_020efbdc(c);
                return;
            }
        } else {
            *(s16 *)(c + 0x428) = 0x12c;
        }
    }
    if (DecIfAbove0_Byte((u8 *)(c + 0x448)) != 0)
        return;
    if (DecIfAbove0_Byte((u8 *)(c + 0x449)) != 0) {
        raw = *(s16 *)(c + 0x44a);
        v = data_02082214[((u16)(int)(long long)raw >> 4) * 2];
        *(s32 *)(c + 0x60) = *(s32 *)(c + 0x444) + (s32)(u32)((((long long)v << 15) + 0x800) >> 12);
        *(u16 *)((int)(((long long)(int)(c + 0x44a)))) =
            (u16)(*(u16 *)((int)(((long long)(int)(c + 0x44a)))) + 0x3000);
        return;
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    if (func_ov002_020efe68(c) != 0)
        _Z14ApproachLinearRsss((s16 *)(c + 0x8c), 0x3000, 0x100);
    if (DecIfAbove0_Byte((u8 *)(c + 0x42b)) != 0)
        return;
    if (func_ov002_020eff04(c) != 0) {
        func_ov002_020efbdc(c);
        return;
    }
    _ZN9ActorBase18MarkForDestructionEv(c);
}
