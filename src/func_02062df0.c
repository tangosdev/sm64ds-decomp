#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);
extern void _ZN4CP1519InvalidateDataCacheEjj(u32 addr, u32 len);
extern int func_020623ec(int a0, int a1, int a2, int a3, u16 a4, u16 a5);
extern void MultiCopyHalf(const void *src, void *dst, u32 count);
extern void func_020627e8(void *self, int x);
extern void func_02062990(void *self, u32 a, u32 b);
extern void func_02062d10(void *self);

#define H(o) (*(u16 *)(self + (o)))

int func_02062df0(u8 *self, u8 *sb, u8 *r8)
{
    int slot;
    u16 flag5;
    u16 f86;
    int did;
    int fp;
    int state;
    int r;

    slot = WM_GetSystemWork();
    r = WM_CheckStateEx(2, 9, 0xa);
    if (r != 0)
        return r;

    _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(slot + 4) + 0x10, 4);
    if (*(u32 *)(*(u32 *)(slot + 4) + 0x10) == 0)
        return 3;

    if (self == 0)
        return 6;
    if (sb == 0)
        return 6;
    if (r8 == 0)
        return 6;

    _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(slot + 4) + 0x184, 2);
    flag5 = *(u16 *)(*(u32 *)(slot + 4) + 0x184);
    if (flag5 == 0) {
        _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(slot + 4) + 0x86, 2);
        f86 = *(u16 *)(*(u32 *)(slot + 4) + 0x86);
    }

    state = H(0x41c);
    if (state == 3)
        return 1;
    if (state != 1 && state != 2)
        return 3;

    slot = 5;

    if (flag5 == 0) {
        did = 0;
        fp = did;

        if (state == 2) {
            u16 n;
            int cr;
            H(0x41c) = 1;
            n = (u16)((H(0x408) + 3) % 4);
            cr = func_020623ec((int)func_02062d10, (int)(self + ((u32)n << 8)),
                               H(0x414), (u16)(H(0x40e) & f86), H(0x416), 1);
            if (cr == 7) {
                *(u16 *)(self + ((u32)n << 1) + 0x400) = 0xffff;
                H(0x40a) = (u16)((H(0x40a) + 1) % 4);
            } else if (cr != 0 && cr != 2) {
                H(0x41c) = 3;
                return 1;
            }
        }

        if (H(0x40c) != H(0x40a)) {
            *(u16 *)(self + ((u32)H(0x40c) << 8)) |= 1;
            MultiCopyHalf(self + ((u32)H(0x40c) << 8), r8, 0x100);
            did = 1;
            slot = 0;
            H(0x41a) = *(u16 *)(self + ((u32)H(0x40c) << 1) + 0x400);
            H(0x40c) = (u16)((H(0x40c) + 1) % 4);

            if (H(0x418) == 0 && f86 != 0 && *(u16 *)(self + ((u32)H(0x408) << 8)) == 1)
                fp = did;
            else
                fp = 0;
        }

        func_020627e8(self, 0);
        if (did != 0) {
            func_02062990(self, 0, (u32)sb);
            if (H(0x418) == 0)
                func_020627e8(self, fp);
        }
    } else {
        int changed;

        changed = 0;
        if (state == 2) {
            H(0x41c) = 1;
            changed = 1;
        } else {
            if (H(0x40c) != H(0x408)) {
                if ((*(u16 *)(self + ((u32)H(0x40c) << 8)) & 1) == 0) {
                    *(u16 *)(self + ((u32)H(0x40c) << 8)) |= 1;
                } else {
                    MultiCopyHalf(self + ((u32)H(0x40c) << 8), r8, 0x100);
                    changed = 1;
                    slot = 0;
                    H(0x41a) = *(u16 *)(self + ((u32)H(0x40c) << 1) + 0x400);
                    H(0x40c) = (u16)((H(0x40c) + 1) % 4);
                }
            }
        }

        if (changed != 0) {
            u8 *addr = self + ((u32)H(0x40a) << 8) + 0x20;
            int cr;
            MultiCopyHalf(sb, addr, H(0x410));
            cr = func_020623ec((int)func_02062d10, (int)addr,
                               H(0x410), H(0x40e), H(0x416), 1);
            H(0x40a) = (u16)((H(0x40a) + 1) % 4);
            if (cr != 2 && cr != 0) {
                H(0x41c) = 3;
                slot = 1;
            }
        }
    }

    return slot;
}
