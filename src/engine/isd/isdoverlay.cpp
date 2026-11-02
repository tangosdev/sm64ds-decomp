//cpp
/* isdoverlay.c: the IS-NITRO debugger's overlay table, from Nintendo's
 * IS-Debugger SDK. It is the only source file the ROM names. The string
 * "isdoverlay.c" at 0x020868fc is the FILE argument of every assert here,
 * and the assert expressions after it (0x0208690c..0x02086a30) give the
 * field and parameter names this file uses: m_header.m_nInfoCount,
 * m_info[i].m_VMA, m_info[i].m_SIZE, nVMA, nIndex and nWrapSize.
 *
 * The debugger keeps two tables, one for each processor, and data_020a9ec0
 * points at them. Only the two editors here read that pointer. FS_LoadOverlayImage
 * (func_02018b64) records each overlay it loads with func_0206d32c, and
 * FS_EndOverlay removes it again with func_0206cf98.
 *
 * Why these six functions are one file:
 * - mwccarm emits .text in reverse source order, and the assert line
 *   numbers agree: func_0206d940 asserts at line 0x42, func_0206d8c4 at
 *   0x55, func_0206d32c at 0x121..0x151 and func_0206cf98 at
 *   0x1a9..0x1ca. In ROM order these functions run cf98, d32c, d8c4, d940.
 * - The two editors are the only callers of the other four, and the
 *   isdoverlay.c strings are referenced from these six functions and
 *   nowhere else.
 * - Bottom edge: func_0206cf7c tests the debugger flags word for the
 *   print helpers below it. It asserts nothing and nothing here calls it.
 *   Top edge: func_0206d9cc and func_0206da18 are the hand-written IRQ
 *   primitives. CP15 starts at 0x0206da64.
 *
 * The cartridge has .exceptix records for all six functions
 * (0x0207385c..0x02073898), so the original file was built with
 * exceptions on. `#pragma exceptions on` is left out on purpose. rombuild
 * isolates a text-only multi-symbol object like this one and refuses any
 * section other than .text, the same refusal that keeps the pragma out of
 * NMWException.cpp. Without the pragma the records stay ROM bytes. The
 * .text is the same either way.
 *
 * The ROM calls the file .c. This one is .cpp because the shared headers
 * it uses (cstd.h's cstd::__assert, CP15.h's inlines) are C++. The extern
 * "C" block keeps the address names unmangled.
 */
#include "types.h"
#include "CP15.h"
#include "cstd.h"

/* Both editors need these in force. mwccarm applies a pragma's final
 * state to the whole file, so they cover the four helpers as well, and
 * the helpers compile the same with them. optimize_for_size is the reason
 * func_0206d8c4 and func_0206d940 enter their loops at the test. */
#pragma opt_common_subs off
#pragma opt_loop_invariants off
#pragma optimize_for_size on

/* LM is a plain pointer cast; LM2 passes an address through a 64-bit
 * integer. LM2 is a matching lever carried over from the shards. The original source almost
 * certainly wrote these updates as plain `+=` and `&=`. EB(t, n) is entry
 * n of table t, reached as entry 0 of a table based n entries further on,
 * which is the form the ROM's address arithmetic takes. */
#define LM(x) ((int*)(x))
#define EB(t, n) ((struct OverlayTable *)((char *)(t) + ((n) << 4)))
#define LM2(x) ((int*)(long long)(unsigned)(x))

extern "C" {

struct OverlayInfo {
    u32 m_VMA;     /* load address */
    u32 romOffset; /* func_02018b64 passes the overlay's ROM offset */
    u32 m_SIZE;
    u32 unused; /* zeroed on insert, never read */
};

/* An entry copied whole. C copies any struct with an ldm/stm block move,
 * and so does C++ for a class whose only member is an array. A class of
 * scalar members C++ copies member by member instead (notes/cpp-class-form.md).
 * The block move's register pressure is what gives func_0206cf98 and
 * func_0206d32c their 0x1c- and 0x44-byte frames. Copied as an
 * OverlayInfo, the same source compiles with 0x14 and 0x34, three
 * instructions short in each. */
struct OverlayInfoWords {
    u32 w[4];
};

inline void CopyInfo(struct OverlayInfo *d, const struct OverlayInfo *s) {
    *(struct OverlayInfoWords *)d = *(const struct OverlayInfoWords *)s;
}

/* The tag names are coined. The asserts name only m_header,
 * m_nInfoCount and m_info. */
struct OverlayTableHeader {
    u32 m_nInfoCount;
    int lock;  /* swp lock word, taken with 0x80 and released with 0 */
    int flags; /* bit 1 is set while an edit is in progress */
    int pad;
};

struct OverlayTable {
    struct OverlayTableHeader m_header;
    struct OverlayInfo m_info[1];
};

typedef struct {
    u32 irqState;
    u32 fiqState;
    u32 mpuRegion7;
} SavedIntState;

extern struct OverlayTable *data_020a9ec0[2];
extern const char data_020868fc; /* "isdoverlay.c" */
extern const char data_0208690c;
extern const char data_0208692c;
extern const char data_0208696c;
extern const char data_0208698c;
extern const char data_020869bc;
extern const char data_020869dc;
extern const char data_02086a08;
extern const char data_02086a30;

extern int func_0206da18(void); /* IRQ::SetIRQState(false) entry */
extern int func_0206d9cc(void); /* IRQ::SetFIQState(false) entry */
/* local extern: IRQ::SetFIQState(bool) and IRQ::SetIRQState(bool) are entry
 * labels inside func_0206d9cc and func_0206da18. func_0206d868 hands them the
 * saved word unnormalised; a bool parameter adds the `!= 0` the ROM lacks. */
extern void _ZN3IRQ11SetFIQStateEb(int state);
extern void _ZN3IRQ11SetIRQStateEb(int state);
extern u32 func_0206dab4(u32 value, void *lock);

void func_0206d940(struct OverlayTable *pInfo, u32 nIndex);
void func_0206d8c4(struct OverlayTable *pInfo, u32 nIndex);
void func_0206d890(SavedIntState *state);
void func_0206d868(const SavedIntState *s);

// @symbol func_0206d940
/* Open a gap at nIndex: move entries nIndex.. up one and count one more. */
void func_0206d940(struct OverlayTable *pInfo, u32 nIndex) {
    unsigned int n;
    int i;
    if (nIndex > pInfo->m_header.m_nInfoCount) {
        cstd::__assert(&data_020868fc, (const char *)0x42, &data_02086a30, 1);
    }
    n = pInfo->m_header.m_nInfoCount;
    if (n - nIndex != 0) {
        for (i = (int)n - 1; i >= (int)nIndex; i--)
            CopyInfo(&pInfo->m_info[i + 1], &pInfo->m_info[i]);
    }
    pInfo->m_header.m_nInfoCount++;
}

// @symbol func_0206d8c4
/* Delete entry nIndex: move the entries above it down one. */
void func_0206d8c4(struct OverlayTable *pInfo, u32 nIndex) {
    unsigned int n;
    if (nIndex >= pInfo->m_header.m_nInfoCount) {
        cstd::__assert(&data_020868fc, (const char *)0x55, &data_02086a08, 1);
    }
    pInfo->m_header.m_nInfoCount--;
    for (n = pInfo->m_header.m_nInfoCount; nIndex < n; nIndex++)
        CopyInfo(&pInfo->m_info[nIndex], &pInfo->m_info[nIndex + 1]);
}

// @symbol func_0206d890
/* Disable IRQ and FIQ, and open MPU data region 7 (0x02600029) for the
 * edit. Saves all three previous states for func_0206d868. */
void func_0206d890(SavedIntState *state) {
    state->irqState = func_0206da18();
    state->fiqState = func_0206d9cc();
    state->mpuRegion7 = CP15::MPUGetDataRegion7();
    CP15::MPUDataRegion7(0x02600029);
}

// @symbol func_0206d868
/* Put back what func_0206d890 saved. */
void func_0206d868(const SavedIntState *s) {
    CP15::MPUDataRegion7(s->mpuRegion7);
    _ZN3IRQ11SetFIQStateEb(s->fiqState);
    _ZN3IRQ11SetIRQStateEb(s->irqState);
}

// @symbol func_0206d32c
/* Record [nVMA, nVMA + size) at ROM offset lma in table type, in address
 * order. Entries it covers are deleted or trimmed, an entry it starts
 * inside is split, and an entry with the same start takes the new ROM
 * offset and size. func_02018b64 (FS_LoadOverlayImage) calls it for each
 * overlay it loads. Returns 0 for a bad type.
 *
 * The asserts index the entries after i as m_info[i + j]. Here j is the
 * absolute index i + 1. */
int func_0206d32c(int type, u32 nVMA, u32 lma, u32 size) {
    unsigned long long end;
    unsigned long long addr64;
    int found;
    unsigned long long vma64;
    u32 i;
    u32 k;
    u32 j;
    SavedIntState state;
    struct OverlayTable *pInfo;

    pInfo = ((unsigned)type >= 2) ? 0 : data_020a9ec0[type];
    if (!pInfo)
        return 0;
    if (size == 0)
        return 1;

    func_0206d890(&state);
    while (func_0206dab4(0x80, &pInfo->m_header.lock) != 0) {
    }

    *LM(&pInfo->m_header.flags) |= 2;
    addr64 = (u32)nVMA;
    if (addr64 + (u32)size > 0x100000000ULL) {
        size = (u32)(0x100000000ULL - addr64);
    }
    end = addr64 + (u32)size;

    found = 0;
    i = 0;
    vma64 = nVMA;
    for (; i < pInfo->m_header.m_nInfoCount && !found; i++) {
        u32 ea = pInfo->m_info[i].m_VMA;
        if (ea > nVMA) {
            found = 1;
            func_0206d940(pInfo, i);
            pInfo->m_info[i].m_VMA = nVMA;
            pInfo->m_info[i].romOffset = lma;
            pInfo->m_info[i].m_SIZE = size;
            pInfo->m_info[i].unused = 0;
            j = i + 1;
            while (i + 1 < pInfo->m_header.m_nInfoCount) {
                if ((unsigned long long)pInfo->m_info[j].m_VMA + (u32)pInfo->m_info[j].m_SIZE <=
                    end) {
                    func_0206d8c4(pInfo, i + 1);
                    continue;
                }
                {
                    long long nWrapSize = (long long)end - (long long)pInfo->m_info[j].m_VMA;
                    if (nWrapSize <= 0)
                        break;
                    if (!(pInfo->m_info[j].m_SIZE > (u32)nWrapSize)) {
                        cstd::__assert(&data_020868fc, (const char *)0x121, &data_0208698c, 1);
                    }
                    *LM(&EB(pInfo, j)->m_info[0].m_VMA) += (u32)nWrapSize;
                    *LM(&EB(pInfo, j)->m_info[0].romOffset) += (u32)nWrapSize;
                    *LM(&EB(pInfo, j)->m_info[0].m_SIZE) -= (u32)nWrapSize;
                    if (!(pInfo->m_info[j].m_SIZE != 0)) {
                        cstd::__assert(&data_020868fc, (const char *)0x127, &data_020869bc, 1);
                    }
                    if (!(pInfo->m_info[j].m_SIZE < 0xf0000000)) {
                        cstd::__assert(&data_020868fc, (const char *)0x128, &data_020869dc, 1);
                    }
                    break;
                }
            }
        } else if (ea <= nVMA) {
            if (vma64 < (unsigned long long)(u32)ea + (u32)pInfo->m_info[i].m_SIZE) {
                if (!(ea <= nVMA)) {
                    cstd::__assert(&data_020868fc, (const char *)0x12e, &data_0208690c, 1);
                }
                if (!((unsigned long long)(u32)pInfo->m_info[i].m_VMA +
                          (u32)pInfo->m_info[i].m_SIZE >
                      vma64)) {
                    cstd::__assert(&data_020868fc, (const char *)0x12f, &data_0208692c, 1);
                }
                if (nVMA != pInfo->m_info[i].m_VMA) {
                    u32 ni;
                    struct OverlayTable *dstb;
                    struct OverlayTable *srcb;
                    func_0206d940(pInfo, i);
                    ni = i + 1;
                    dstb = EB(pInfo, i);
                    srcb = EB(pInfo, ni);
                    i = ni;
                    CopyInfo(&dstb->m_info[0], &srcb->m_info[0]);
                    dstb->m_info[0].m_SIZE = nVMA - dstb->m_info[0].m_VMA;
                    *LM(&srcb->m_info[0].m_VMA) += dstb->m_info[0].m_SIZE;
                    *LM(&srcb->m_info[0].romOffset) += dstb->m_info[0].m_SIZE;
                    *LM(&srcb->m_info[0].m_SIZE) -= dstb->m_info[0].m_SIZE;
                }
                if (!(nVMA == pInfo->m_info[i].m_VMA)) {
                    cstd::__assert(&data_020868fc, (const char *)0x13c, &data_0208696c, 1);
                }
                if (pInfo->m_info[i].m_SIZE <= size) {
                    found = 1;
                    pInfo->m_info[i].romOffset = lma;
                    pInfo->m_info[i].m_SIZE = size;
                    k = i + 1;
                    while (i + 1 < pInfo->m_header.m_nInfoCount) {
                        u32 wc = pInfo->m_info[k].m_SIZE;
                        u32 wa = pInfo->m_info[k].m_VMA;
                        if ((unsigned long long)wa + (u32)wc <= end) {
                            func_0206d8c4(pInfo, i + 1);
                            continue;
                        }
                        {
                            long long nWrapSize = (long long)end - (long long)wa;
                            if (nWrapSize <= 0)
                                break;
                            if (!(wc > (u32)nWrapSize)) {
                                cstd::__assert(&data_020868fc, (const char *)0x151, &data_0208698c,
                                               1);
                            }
                            *LM(&EB(pInfo, k)->m_info[0].m_VMA) += (u32)nWrapSize;
                            *LM(&EB(pInfo, k)->m_info[0].romOffset) += (u32)nWrapSize;
                            *LM(&EB(pInfo, k)->m_info[0].m_SIZE) -= (u32)nWrapSize;
                        }
                    }
                } else {
                    found = 1;
                    *LM(&EB(pInfo, i)->m_info[0].m_VMA) += size;
                    *LM(&EB(pInfo, i)->m_info[0].romOffset) += size;
                    *LM(&EB(pInfo, i)->m_info[0].m_SIZE) -= size;
                    func_0206d940(pInfo, i);
                    pInfo->m_info[i].m_VMA = nVMA;
                    pInfo->m_info[i].romOffset = lma;
                    pInfo->m_info[i].m_SIZE = size;
                    pInfo->m_info[i].unused = 0;
                }
            }
        }
    }

    if (!found) {
        func_0206d940(pInfo, i);
        pInfo->m_info[i].m_VMA = nVMA;
        pInfo->m_info[i].romOffset = lma;
        pInfo->m_info[i].m_SIZE = size;
        pInfo->m_info[i].unused = 0;
    }

    *LM2(&pInfo->m_header.flags) &= ~2;
    pInfo->m_header.lock = 0;
    func_0206d868(&state);
    return 1;
}

// @symbol func_0206cf98
/* Remove [nVMA, nVMA + size) from table type (0 or 1; FS_EndOverlay passes
 * its overlay's target processor). An entry inside the range is deleted,
 * and an entry crossing either end is split or trimmed. Interrupts are off
 * and the table's lock word is held for the edit. Returns 0 for a bad
 * type. */
int func_0206cf98(int type, u32 nVMA, u32 size) {
    unsigned long long end;
    unsigned long long addr64;
    int found;
    u32 i;
    SavedIntState state;
    struct OverlayTable *pInfo;

    pInfo = ((unsigned)type >= 2) ? 0 : data_020a9ec0[type];
    if (!pInfo)
        return 0;
    if (size == 0)
        return 1;

    func_0206d890(&state);
    while (func_0206dab4(0x80, &pInfo->m_header.lock) != 0) {
    }

    *LM(&pInfo->m_header.flags) |= 2;
    addr64 = (u32)nVMA;
    if (addr64 + (u32)size > 0x100000000ULL) {
        size = (u32)(0x100000000ULL - addr64);
    }
    end = addr64 + (u32)size;

    found = 0;
    for (i = 0; i < pInfo->m_header.m_nInfoCount && !found; i++) {
        u32 ea = pInfo->m_info[i].m_VMA;
        if (ea > nVMA) {
            if ((unsigned long long)(u32)ea + (u32)pInfo->m_info[i].m_SIZE <= end) {
                size -= ea - nVMA;
                nVMA = ea;
                found = 1;
            }
        }
        if (ea <= nVMA) {
            if (nVMA < (unsigned long long)(u32)ea + (u32)pInfo->m_info[i].m_SIZE) {
                found = 1;
            }
        }
        if (!found)
            continue;

        if (!(ea <= nVMA)) {
            cstd::__assert(&data_020868fc, (const char *)0x1a9, &data_0208690c, 1);
        }
        if (!((unsigned long long)(u32)pInfo->m_info[i].m_VMA + (u32)pInfo->m_info[i].m_SIZE >
              nVMA)) {
            cstd::__assert(&data_020868fc, (const char *)0x1aa, &data_0208692c, 1);
        }

        if (nVMA != pInfo->m_info[i].m_VMA) {
            u32 ni;
            struct OverlayTable *dstb;
            struct OverlayTable *srcb;
            func_0206d940(pInfo, i);
            ni = i + 1;
            dstb = EB(pInfo, i);
            srcb = EB(pInfo, ni);
            i = ni;
            CopyInfo(&dstb->m_info[0], &srcb->m_info[0]);
            dstb->m_info[0].m_SIZE = nVMA - dstb->m_info[0].m_VMA;
            *LM(&srcb->m_info[0].m_VMA) += dstb->m_info[0].m_SIZE;
            *LM(&srcb->m_info[0].romOffset) += dstb->m_info[0].m_SIZE;
            *LM(&srcb->m_info[0].m_SIZE) -= dstb->m_info[0].m_SIZE;
        }

        if (!(nVMA == pInfo->m_info[i].m_VMA)) {
            cstd::__assert(&data_020868fc, (const char *)0x1b7, &data_0208696c, 1);
        }

        if (pInfo->m_info[i].m_SIZE <= size) {
            func_0206d8c4(pInfo, i);
            while (i < pInfo->m_header.m_nInfoCount) {
                u32 wc = pInfo->m_info[i].m_SIZE;
                u32 wa = pInfo->m_info[i].m_VMA;
                if ((unsigned long long)(u32)wa + (u32)wc <= end) {
                    func_0206d8c4(pInfo, i);
                    continue;
                }
                {
                    long long nWrapSize = (long long)end - (long long)(u32)wa;
                    if (nWrapSize <= 0)
                        break;
                    if (!(wc > (u32)nWrapSize)) {
                        cstd::__assert(&data_020868fc, (const char *)0x1ca, &data_0208698c, 1);
                    }
                    *LM(&EB(pInfo, i)->m_info[0].m_VMA) += (u32)nWrapSize;
                    *LM(&EB(pInfo, i)->m_info[0].romOffset) += (u32)nWrapSize;
                    *LM(&EB(pInfo, i)->m_info[0].m_SIZE) -= (u32)nWrapSize;
                }
            }
        } else {
            struct OverlayTable *eb = EB(pInfo, i);
            *LM(&eb->m_info[0].m_VMA) += size;
            *LM(&eb->m_info[0].romOffset) += size;
            *LM(&eb->m_info[0].m_SIZE) -= size;
        }
    }

    *LM2(&pInfo->m_header.flags) &= ~2;
    pInfo->m_header.lock = 0;
    func_0206d868(&state);
    return 1;
}
}
