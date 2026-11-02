//cpp
/* Overlays (the Nitro SDK's FS_overlay.c), arm9 .text 0x0205db88..0x0205e0f4.
 *
 * FS_LoadOverlayInfo reads an overlay's 0x20-byte table entry, from the
 * in-memory table when one is attached (data_020a8064 and data_020a806c,
 * pret's fsi_ovt9 and fsi_ovt7) and otherwise from the ROM archive
 * (data_020a8074, fsi_arc_rom) through FSi_LoadOverlayInfoCore.
 * FS_StartOverlay checks a multiboot child's image against its digest,
 * decompresses it and runs its static initializers. FS_EndOverlay runs the
 * destructors the overlay registered, and FS_UnloadOverlay reads the entry
 * and does that. func_0205db88 attaches an in-memory table.
 *
 * The first nine are pret's pokediamond FS_overlay.c functions, in its
 * order. mwccarm emits them in reverse, so FS_UnloadOverlay, pret's last,
 * sits lowest of them and FS_ClearOverlayImage highest. This SDK is older
 * than pret's: there is no FS_LoadOverlayImage, FS_LoadOverlayImageAsync,
 * FS_LoadOverlay or FSi_GetOverlayBinarySize, FSi_LoadOverlayInfoCore stops
 * after the header (no file_pos), and FS_StartOverlay has no DC_FlushRange
 * step. The game keeps its own copies of the loaders: FS_LoadOverlay at
 * 0x02018ad0, func_02018b64 (FS_LoadOverlayImage) and func_02018c00
 * (FS_LoadOverlayInfo).
 */
#include "types.h"
#include "CP15.h"
#include "IRQ.h"
#include "nitro/fs/overlay.h"

extern "C" {

struct FSArchive;

/* NitroSDK CARDRomRegion. */
typedef struct CARDRomRegion {
    u32 offset;
    u32 length;
} CARDRomRegion;

/* NitroSDK FSFileID: the archive and the file's index in it. */
struct Pair {
    void *arc;
    int file_id;
};

/* The __global_destructor_chain entries the runtime keeps for objects with
 * static storage. */
struct MWiDestructorChain {
    struct MWiDestructorChain *next;
    u32 dtor;
    u32 obj;
};

enum Flag { F0, F1 };

extern CARDRomRegion data_020a8064;    /* fsi_ovt9 */
extern CARDRomRegion data_020a806c;    /* fsi_ovt7 */
extern struct FSArchive data_020a8074; /* fsi_arc_rom */
extern void *data_0209a04c;            /* fsi_digest_key_ptr */
extern int data_0209a050;              /* fsi_digest_key_len */
/* The 20-byte digest func_0205db88 checks an attached table against, then
   one 20-byte digest per overlay up to data_0209aff8. */
extern char data_0209a7d8[];
extern char data_0209a7ec[];
extern char data_0209aff8[];
extern struct MWiDestructorChain *__global_destructor_chain;

void CpuCopy8(const void *src, void *dst, u32 size);
void func_0205a588(void *dst, int value, int size);
void DGT_Hash2CalcHmac(void *out, void *msg, int msglen, void *key, int keylen);
void MIi_UncompressBackward(void *bottom);
void FS_InitFile(void *s);
int FS_OpenFileDirect(char *p_file, struct FSArchive *p_arc, u32 image_top, u32 image_bottom,
                      u32 file_index);
int FS_ReadFile(void *s, void *b, int c);
int FS_CloseFile(void *self);
int func_0206cf98(int a, u32 b, u32 c);
int func_0205e904(s32 newVal);
void Crash(void);

// @symbol FS_ClearOverlayImage
/* Invalidate the I- and D-cache over the overlay's whole region (image and
 * bss), then zero the bss after the image. */
void FS_ClearOverlayImage(FSOverlayInfo *p_ovi) {
    u32 im_start = (u32)p_ovi->header.ram_address;
    u32 ram_size = p_ovi->header.ram_size;
    u32 bss_size = p_ovi->header.bss_size;
    u32 total_size = ram_size + bss_size;
    CP15::InvalidateInstructionCache(im_start, total_size);
    CP15::InvalidateDataCache(im_start, total_size);
    func_0205a588((void *)(im_start + ram_size), 0, total_size - ram_size);
}

// @symbol FS_GetOverlayFileID
/* The overlay's FSFileID: the ROM archive and the header's file_id. */
struct Pair FS_GetOverlayFileID(const FSOverlayInfo *p_ovi) {
    struct Pair ret;
    ret.arc = &data_020a8074;
    ret.file_id = p_ovi->header.file_id;
    return ret;
}

// @symbol FSi_LoadOverlayInfoCore
/* Pick the ARM9 or ARM7 overlay table region, bounds-check entry `id`,
 * open the table directly in the archive, read the 0x20-byte header, and
 * set the target. */
int FSi_LoadOverlayInfoCore(FSOverlayInfo *p_ovi, int target, int id, struct FSArchive *arc,
                            u32 offset_arm9, u32 len_arm9, u32 offset_arm7, u32 len_arm7) {
    char file[0x48];
    u32 pos;
    u32 offset;
    u32 length;
    if (target == 0) {
        length = len_arm9;
        offset = offset_arm9;
    } else {
        length = len_arm7;
        offset = offset_arm7;
    }
    pos = id << 5;
    if (pos >= length)
        return 0;
    FS_InitFile(file);
    FS_OpenFileDirect(file, arc, offset + pos, offset + length, -1);
    FS_ReadFile(file, p_ovi, 0x20);
    FS_CloseFile(file);
    p_ovi->target = target;
    return 1;
}

// @symbol FS_LoadOverlayInfo
/* While the ARM9 or ARM7 overlay table is in memory, copy entry `id` out of
 * it and set the target. Otherwise read it from the ROM through
 * FSi_LoadOverlayInfoCore, with the ROM archive and the ROM header's ARM9
 * and ARM7 overlay table regions (0x027ffe50, 0x027ffe58). */
int FS_LoadOverlayInfo(FSOverlayInfo *p_ovi, int target, int id) {
    CARDRomRegion *pr = (target == 0) ? &data_020a8064 : &data_020a806c;
    u32 offset = pr->offset;
    if (offset != 0) {
        u32 pos = id << 5;
        if (pos >= pr->length)
            return 0;
        CpuCopy8((void *)(offset + pos), p_ovi, 0x20);
        p_ovi->target = target;
        return 1;
    }
    CARDRomRegion *ovt9 = (CARDRomRegion *)0x27ffe50;
    CARDRomRegion *ovt7 = (CARDRomRegion *)0x27ffe58;
    return FSi_LoadOverlayInfoCore(p_ovi, target, id, &data_020a8074, ovt9->offset, ovt9->length,
                                   ovt7->offset, ovt7->length);
}

// @symbol FSi_CompareDigest
/* HMAC the image with the digest key and compare the 20-byte result with
 * spec_digest a word at a time. */
int FSi_CompareDigest(char *spec_digest, void *src, int len) {
    int digest[5];
    int digest_key[16];
    unsigned int i;
    int *p;
    func_0205a588(digest, 0, 0x14);
    CpuCopy8(data_0209a04c, digest_key, data_0209a050);
    DGT_Hash2CalcHmac(digest, src, len, digest_key, data_0209a050);
    p = digest;
    for (i = 0; i < 0x14; i += 4) {
        if (*p != *(int *)(spec_digest + i))
            break;
        p++;
    }
    return i == 0x14;
}

// @symbol FS_StartOverlay
/* Once an overlay's image is in RAM, a multiboot child (the halfword at
 * 0x027ffc40 reads 2) checks the digest of an overlay flagged authenticated
 * against its entry at data_0209a7ec. It clears the image and crashes when
 * the check fails, and also when the overlay is not flagged authenticated
 * at all. Then the image is decompressed in place when flagged compressed
 * (MIi_UncompressBackward, given the end of the compressed image), and its
 * static initializers run from sinit_init to sinit_init_end. */
void FS_StartOverlay(FSOverlayInfo *p_ovi) {
    unsigned char flags = p_ovi->header.flag;
    unsigned int rare_size;
    if (flags & 1) {
        rare_size = p_ovi->header.compressed;
    } else {
        rare_size = p_ovi->header.ram_size;
    }
    if (*(unsigned short *)0x27ffc40 == 2) {
        int ret = 0;
        if (flags & 2) {
            int odt_max = (int)(data_0209aff8 - data_0209a7ec) / 20;
            if (p_ovi->header.id < (unsigned int)odt_max) {
                char *spec_digest = data_0209a7ec + p_ovi->header.id * 0x14;
                ret = FSi_CompareDigest(spec_digest, p_ovi->header.ram_address, rare_size);
            }
        }
        if (ret == 0) {
            func_0205a588(p_ovi->header.ram_address, 0, rare_size);
            Crash();
            return;
        }
    }
    if (p_ovi->header.flag & 1) {
        MIi_UncompressBackward(p_ovi->header.ram_address + rare_size);
    }
    {
        FSOverlayInitFunc *p = p_ovi->header.sinit_init;
        FSOverlayInitFunc *q = p_ovi->header.sinit_init_end;
        while (p < q) {
            FSOverlayInitFunc fn = *p;
            if (fn != 0)
                fn();
            p++;
        }
    }
}

// @symbol FS_EndOverlay
/* An older revision than pret's: with interrupts disabled, unlink every
 * __global_destructor_chain entry whose destructor lies in the overlay's
 * image or whose object lies in its bss, run the ones unlinked, and repeat
 * until a pass finds none. Then hand the target, address and size to
 * func_0206cf98. */
void FS_EndOverlay(FSOverlayInfo *p_ovi) {
    struct MWiDestructorChain *prev;
    struct MWiDestructorChain *head;
    struct MWiDestructorChain *tail;
    u32 region_top, image_bottom, region_bottom;
    u32 saved;
    struct MWiDestructorChain *base;
    struct MWiDestructorChain *p;
    struct MWiDestructorChain *next;
    struct MWiDestructorChain **chain = &__global_destructor_chain;

    for (;;) {
        head = 0;
        region_top = (u32)p_ovi->header.ram_address;
        image_bottom = (u32)p_ovi->header.ram_address + p_ovi->header.ram_size;
        /* the bss end: the image end plus the total size less the image */
        region_bottom =
            image_bottom +
            (u32)((int)(p_ovi->header.ram_size + p_ovi->header.bss_size) - p_ovi->header.ram_size);
        tail = 0;
        saved = IRQ::Disable();
        prev = 0;
        base = *chain;
        p = base;
        while (p) {
            u32 dtor, obj;
            dtor = p->dtor;
            next = p->next;
            obj = p->obj;
            if ((dtor >= region_top && dtor < image_bottom) ||
                (obj >= image_bottom && obj < region_bottom)) {
                if (tail)
                    tail->next = p;
                else
                    head = p;
                if (base == p) {
                    *chain = next;
                    base = next;
                }
                p->next = 0;
                tail = p;
                if (prev)
                    prev->next = next;
            } else {
                prev = p;
            }
            p = next;
        }
        IRQ::Restore(saved);
        if (head == 0)
            break;
        do {
            void (*dtor)(u32);
            dtor = (void (*)(u32))head->dtor;
            next = head->next;
            if (dtor)
                dtor(head->obj);
            head = next;
        } while (head);
    }
    {
        u32 target = p_ovi->target;
        u32 size = p_ovi->header.ram_size;
        u32 addr = (u32)p_ovi->header.ram_address;
        enum Flag flag = (target == 0) ? F0 : F1;
        func_0206cf98(flag, addr, size);
    }
}

// @symbol FS_UnloadOverlayImage
int FS_UnloadOverlayImage(FSOverlayInfo *p_ovi) {
    FS_EndOverlay(p_ovi);
    return 1;
}

// @symbol FS_UnloadOverlay
/* Read the overlay's FSOverlayInfo and unload its image. */
int FS_UnloadOverlay(int target, int id) {
    FSOverlayInfo ovi;
    if (!FS_LoadOverlayInfo(&ovi, target, id) || !FS_UnloadOverlayImage(&ovi))
        return 0;
    return 1;
}

// @symbol func_0205db88
/* Attach an in-memory overlay table for target, so FS_LoadOverlayInfo
 * reads entries from it. An ARM9 table is first checked against the
 * digest at data_0209a7d8, with func_0205e904(1) in effect
 * around the check, and a mismatch crashes. Then ptr and size go into the
 * target's region with interrupts disabled. pret's FS_overlay.c has no
 * function like it, so it keeps its address name. */
void func_0205db88(int target, void *ptr, u32 size) {
    CARDRomRegion *pr;
    if (target == 0) {
        pr = &data_020a8064;
    } else {
        pr = &data_020a806c;
    }
    if (ptr != 0 && target == 0) {
        u32 r4 = func_0205e904(1);
        if (!FSi_CompareDigest(data_0209a7d8, ptr, size)) {
            Crash();
        }
        func_0205e904(r4);
    }
    u32 irqState = IRQ::Disable();
    pr->offset = (u32)ptr;
    pr->length = size;
    IRQ::Restore(irqState);
}
}
