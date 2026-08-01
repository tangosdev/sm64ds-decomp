#include "types.h"
#pragma opt_common_subs off
#pragma opt_loop_invariants off
#pragma optimize_for_size on
#define LM(x) ((int*)(long long)(int)(x))
#define EB(t, n) ((struct Arr *)((char *)(t) + ((n) << 4)))
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
extern const char data_020869bc;
extern const char data_020869dc;

extern void func_0206d890(SavedIntState *state);
extern void func_0206d868(const SavedIntState *state);
extern u32 func_0206dab4(u32 val, void *addr);
extern void func_0206d940(struct Arr *s, u32 index);
extern void func_0206d8c4(struct Arr *s, u32 index);
extern void _ZN4cstd8__assertEPKcPKcPKci(const char *, const char *, const char *, int);

int func_0206d32c(int type, u32 addr, u32 lma, u32 size) {
    int result;
    unsigned long long end;
    unsigned long long addr64;
    int found;
    unsigned long long vma64;
    u32 i;
    u32 k;
    u32 j;
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
    i = 0;
    vma64 = addr;
    for (; i < table->count && !found; i++) {
        u32 ea = table->elems[i].a;
        if (ea > addr) {
            found = 1;
            func_0206d940(table, i);
            table->elems[i].a = addr;
            table->elems[i].b = lma;
            table->elems[i].c = size;
            table->elems[i].d = 0;
            j = i + 1;
            while (i + 1 < table->count) {
                if ((unsigned long long)table->elems[j].a + (u32)table->elems[j].c <= end) {
                    func_0206d8c4(table, i + 1);
                    continue;
                }
                {
                    long long ov = (long long)end - (long long)table->elems[j].a;
                    if (ov <= 0) break;
                    if (!(table->elems[j].c > (u32)ov)) {
                        _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x121, &data_0208698c, 1);
                    }
                    *LM(&EB(table, j)->elems[0].a) += (u32)ov;
                    *LM(&EB(table, j)->elems[0].b) += (u32)ov;
                    *LM(&EB(table, j)->elems[0].c) -= (u32)ov;
                    if (!(table->elems[j].c != 0)) {
                        _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x127, &data_020869bc, 1);
                    }
                    if (!(table->elems[j].c < 0xf0000000)) {
                        _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x128, &data_020869dc, 1);
                    }
                    break;
                }
            }
        } else if (ea <= addr) {
            if (vma64 < (unsigned long long)(u32)ea + (u32)table->elems[i].c) {
                if (!(ea <= addr)) {
                    _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x12e, &data_0208690c, 1);
                }
                if (!((unsigned long long)(u32)table->elems[i].a + (u32)table->elems[i].c > vma64)) {
                    _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x12f, &data_0208692c, 1);
                }
                if (addr != table->elems[i].a) {
                    u32 ni;
                    struct Arr *dstb;
                    struct Arr *srcb;
                    func_0206d940(table, i);
                    ni = i + 1;
                    dstb = EB(table, i);
                    srcb = EB(table, ni);
                    i = ni;
                    dstb->elems[0] = srcb->elems[0];
                    dstb->elems[0].c = addr - dstb->elems[0].a;
                    *LM(&srcb->elems[0].a) += dstb->elems[0].c;
                    *LM(&srcb->elems[0].b) += dstb->elems[0].c;
                    *LM(&srcb->elems[0].c) -= dstb->elems[0].c;
                }
                if (!(addr == table->elems[i].a)) {
                    _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x13c, &data_0208696c, 1);
                }
                if (table->elems[i].c <= size) {
                    found = 1;
                    table->elems[i].b = lma;
                    table->elems[i].c = size;
                    k = i + 1;
                    while (i + 1 < table->count) {
                        u32 wc = table->elems[k].c;
                        u32 wa = table->elems[k].a;
                        if ((unsigned long long)wa + (u32)wc <= end) {
                            func_0206d8c4(table, i + 1);
                            continue;
                        }
                        {
                            long long ov = (long long)end - (long long)wa;
                            if (ov <= 0) break;
                            if (!(wc > (u32)ov)) {
                                _ZN4cstd8__assertEPKcPKcPKci(&data_020868fc, (const char *)0x151, &data_0208698c, 1);
                            }
                            *LM(&EB(table, k)->elems[0].a) += (u32)ov;
                            *LM(&EB(table, k)->elems[0].b) += (u32)ov;
                            *LM(&EB(table, k)->elems[0].c) -= (u32)ov;
                        }
                    }
                } else {
                    found = 1;
                    *LM(&EB(table, i)->elems[0].a) += size;
                    *LM(&EB(table, i)->elems[0].b) += size;
                    *LM(&EB(table, i)->elems[0].c) -= size;
                    func_0206d940(table, i);
                    table->elems[i].a = addr;
                    table->elems[i].b = lma;
                    table->elems[i].c = size;
                    table->elems[i].d = 0;
                }
            }
        }
    }

    if (!found) {
        func_0206d940(table, i);
        table->elems[i].a = addr;
        table->elems[i].b = lma;
        table->elems[i].c = size;
        table->elems[i].d = 0;
    }

    *LM2(&table->pad[1]) &= ~2;
    table->pad[0] = 0;
    func_0206d868(&state);
    result = 1;
epilogue:
    return result;
}
