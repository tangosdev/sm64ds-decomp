#include "types.h"
#pragma opt_common_subs off
#pragma opt_loop_invariants off
#pragma optimize_for_size on
#define LM(x) ((int*)(long long)(int)(x))
#define LM2(x) ((int*)(long long)(unsigned)(x))

struct Elem {
    u32 a, b, c, d;
};

struct Arr {
    u32 count;
    int pad[3];
    struct Elem elems[1];
};

typedef struct {
    u32 irqState;
    u32 fiqState;
    u32 mpuRegion7;
} SavedIntState;

extern struct Arr *data_020a9ec0[2];
extern const char data_020868fc;
extern const char data_0208690c;
extern const char data_0208692c;
extern const char data_0208696c;
extern const char data_0208698c;

extern void func_0206d890(SavedIntState *state);
extern void func_0206d868(const SavedIntState *state);
extern u32 func_0206dab4(u32 val, void *addr);
extern void func_0206d940(struct Arr *s, u32 index);
extern void func_0206d8c4(struct Arr *s, u32 index);
extern void _ZN4cstd8__assertEPKcPKcPKci(const char *, const char *, const char *, int);

int func_0206cf98(int type, u32 addr, u32 size) {
    int result;
    unsigned long long end;
    unsigned long long addr64;
    int found;
    u32 i;
    SavedIntState state;
    struct Arr *table;

    table = ((unsigned)type >= 2) ? 0 : data_020a9ec0[type];
    if (!table) { result = 0; goto epilogue; }
    if (size == 0) { result = 1; goto epilogue; }

    func_0206d890(&state);
    while (func_0206dab4(0x80, &table->pad[0]) != 0) { }

    *LM(&table->pad[1]) |= 2;
    addr64 = (u32)addr;
    if (addr64 + (u32)size > 0x100000000ULL) {
        size = (u32)(0x100000000ULL - addr64);
    }
    end = addr64 + (u32)size;

    found = 0;
    for (i = 0; i < table->count && !found; i++) {
        u32 ea = table->elems[i].a;
        if (ea > addr) {
            if ((unsigned long long)(u32)ea + (u32)table->elems[i].c <= end) {
                size -= ea - addr;
                addr = ea;
                found = 1;
            }
        }
        if (ea <= addr) {
            if (addr < (unsigned long long)(u32)ea + (u32)table->elems[i].c) {
                found = 1;
            }
        }
        if (!found) continue;

        if (!(ea <= addr)) {
            _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x1a9, &data_0208690c, 1);
        }
        if (!((unsigned long long)(u32)table->elems[i].a + (u32)table->elems[i].c > addr)) {
            _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x1aa, &data_0208692c, 1);
        }

        if (addr != table->elems[i].a) {
            u32 ni;
            struct Arr *dstb;
            struct Arr *srcb;
            func_0206d940(table, i);
            ni = i + 1;
            dstb = (struct Arr *)((char *)table + (i << 4));
            srcb = (struct Arr *)((char *)table + (ni << 4));
            i = ni;
            dstb->elems[0] = srcb->elems[0];
            dstb->elems[0].c = addr - dstb->elems[0].a;
            *LM(&srcb->elems[0].a) += dstb->elems[0].c;
            *LM(&srcb->elems[0].b) += dstb->elems[0].c;
            *LM(&srcb->elems[0].c) -= dstb->elems[0].c;
        }

        if (!(addr == table->elems[i].a)) {
            _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x1b7, &data_0208696c, 1);
        }

        if (table->elems[i].c <= size) {
            func_0206d8c4(table, i);
            while (i < table->count) {
                u32 wc = table->elems[i].c;
                u32 wa = table->elems[i].a;
                if ((unsigned long long)(u32)wa + (u32)wc <= end) {
                    func_0206d8c4(table, i);
                    continue;
                }
                {
                    long long ov = (long long)end - (long long)(u32)wa;
                    if (ov <= 0) break;
                    if (!(wc > (u32)ov)) {
                        _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x1ca, &data_0208698c, 1);
                    }
                    *LM(&((struct Arr *)((char *)table + (i << 4)))->elems[0].a) += (u32)ov;
                    *LM(&((struct Arr *)((char *)table + (i << 4)))->elems[0].b) += (u32)ov;
                    *LM(&((struct Arr *)((char *)table + (i << 4)))->elems[0].c) -= (u32)ov;
                }
            }
        } else {
            struct Arr *eb = (struct Arr *)((char *)table + (i << 4));
            *LM(&eb->elems[0].a) += size;
            *LM(&eb->elems[0].b) += size;
            *LM(&eb->elems[0].c) -= size;
        }
    }

    *LM2(&table->pad[1]) &= ~2;
    table->pad[0] = 0;
    func_0206d868(&state);
    result = 1;
epilogue:
    return result;
}
