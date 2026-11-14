/* Overlays (the Nitro SDK's FS_overlay.c), arm9 .text 0x0205db88..0x0205e0f4.
 *
 * FS_LoadOverlayInfo reads an overlay's 0x20-byte table entry, from the
 * in-memory table when one is attached (fsi_ovt9 and fsi_ovt7) and
 * otherwise from the ROM archive (fsi_arc_rom) through
 * FSi_LoadOverlayInfoCore.
 * FS_StartOverlay checks a multiboot child's image against its digest,
 * decompresses it and runs its static initializers. FS_EndOverlay runs the
 * destructors the overlay registered, and FS_UnloadOverlay reads the entry
 * and does that. FS_AttachOverlayTable attaches an in-memory table.
 *
 * The first nine are pret's pokediamond FS_overlay.c functions, in its
 * order. mwccarm emits them in reverse, so FS_UnloadOverlay, pret's last,
 * sits lowest of them and FS_ClearOverlayImage highest. The bodies follow
 * ntrtwl/NitroSDK's fs_overlay.c, but this SDK is older than that one:
 * - there is no FS_LoadOverlayImage, FS_LoadOverlayImageAsync,
 *   FS_LoadOverlay or FSi_GetOverlayBinarySize;
 * - FSi_LoadOverlayInfoCore stops after the header (no file_pos);
 * - FS_StartOverlay keeps the flag byte in a local for the size and
 *   authentication tests, and has no DC_FlushRange step;
 * - FS_EndOverlay takes a destructor in the image or an object in the bss,
 *   where the later SDK takes either anywhere in the overlay.
 * The game keeps its own copies of the loaders: FS_LoadOverlay at
 * 0x02018ad0, func_02018b64 (FS_LoadOverlayImage) and func_02018c00
 * (FS_LoadOverlayInfo).
 */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/card/card_rom.h"
#include "nitro/fs/overlay.h"
#include "nitro/mi/memory.h"
#include "nitro/fs/file.h"
#include "nitro/mb/mb.h"
#include "private/fs_rom.h"
#include "isdbglib.h"
typedef int BOOL;
#define TRUE  1
#define FALSE 0
#define NULL  ((void *)0)

/* The __global_destructor_chain entries the runtime keeps for objects with
 * static storage. */
typedef struct MWiDestructorChain {
    struct MWiDestructorChain *next;
    void (*dtor)(void *);
    void *obj;
} MWiDestructorChain;

#define FS_OVERLAY_FLAG_COMP   0x0001
#define FS_OVERLAY_FLAG_AUTH   0x0002
#define FS_OVERLAY_DIGEST_SIZE 20 /* DGT_HASH2_DIGEST_SIZE */

/* The HMAC key the digests are made with: fsi_digest_key_ptr points at the
 * 64-byte fsi_def_digest_key. They stay in the data link. */
extern const void *fsi_digest_key_ptr;
extern int fsi_digest_key_len;
/* The linker's digests: the overlay table's, then one per overlay up to
 * SDK_OVERLAY_DIGEST_END. */
extern u8 SDK_OVERLAYTABLE_DIGEST[];
extern u8 SDK_OVERLAY_DIGEST[];
extern u8 SDK_OVERLAY_DIGEST_END[];
extern MWiDestructorChain *__global_destructor_chain;

void DGT_Hash2CalcHmac(u8 *out, u8 *msg, int msglen, u8 *key, int keylen);
void MIi_UncompressBackward(void *bottom);
int DGT_SetOverlayTableMode(int flag);

/* Invalidate the I- and D-cache over the overlay's whole region (image and
 * bss), then zero the bss after the image. */
void FS_ClearOverlayImage(FSOverlayInfo *p_ovi) {
    u8 *const addr = FS_GetOverlayAddress(p_ovi);
    const u32 image_size = FS_GetOverlayImageSize(p_ovi);
    const u32 total_size = FS_GetOverlayTotalSize(p_ovi);
    IC_InvalidateRange(addr, total_size);
    DC_InvalidateRange(addr, total_size);
    MI_CpuFill8(addr + image_size, 0, total_size - image_size);
}

/* The overlay's FSFileID: the ROM archive and the header's file_id. */
FSFileID FS_GetOverlayFileID(const FSOverlayInfo *p_ovi) {
    FSFileID ret;
    ret.arc = &fsi_arc_rom;
    ret.file_id = p_ovi->header.file_id;
    return ret;
}

/* Pick the ARM9 or ARM7 overlay table region, bounds-check entry `id`,
 * open the table directly in the archive, read the 0x20-byte header, and
 * set the target. */
BOOL FSi_LoadOverlayInfoCore(FSOverlayInfo *p_ovi, MIProcessor target, FSOverlayID id,
                             FSArchive *p_arc, u32 offset_arm9, u32 len_arm9, u32 offset_arm7,
                             u32 len_arm7) {
    CARDRomRegion pr[1];
    u32 pos;

    if (target == MI_PROCESSOR_ARM9) {
        pr->offset = offset_arm9;
        pr->length = len_arm9;
    } else {
        pr->offset = offset_arm7;
        pr->length = len_arm7;
    }

    pos = (u32)id * sizeof(FSOverlayInfoHeader);

    if (pos >= pr->length) {
        return FALSE;
    } else {
        FSFile file[1];
        FS_InitFile(file);
        FS_OpenFileDirect(file, p_arc, pr->offset + pos, pr->offset + pr->length, (u32)~0);
        FS_ReadFile(file, p_ovi, sizeof(FSOverlayInfoHeader));
        FS_CloseFile(file);
        p_ovi->target = target;
    }

    return TRUE;
}

/* While the ARM9 or ARM7 overlay table is in memory, copy entry `id` out of
 * it and set the target. Otherwise read it from the ROM through
 * FSi_LoadOverlayInfoCore, with the ROM archive and the ROM header's ARM9
 * and ARM7 overlay table regions. */
BOOL FS_LoadOverlayInfo(FSOverlayInfo *p_ovi, MIProcessor target, FSOverlayID id) {
    CARDRomRegion *const pr = (target == MI_PROCESSOR_ARM9) ? &fsi_ovt9 : &fsi_ovt7;

    if (pr->offset) {
        const u32 pos = (u32)id * sizeof(FSOverlayInfoHeader);

        if (pos >= pr->length) {
            return FALSE;
        } else {
            MI_CpuCopy8((const void *)(pr->offset + pos), p_ovi, sizeof(FSOverlayInfoHeader));
            p_ovi->target = target;
            return TRUE;
        }
    } else {
        const CARDRomRegion *const p_ovt9 = CARD_GetRomRegionOVT(MI_PROCESSOR_ARM9);
        const CARDRomRegion *const p_ovt7 = CARD_GetRomRegionOVT(MI_PROCESSOR_ARM7);
        return FSi_LoadOverlayInfoCore(p_ovi, target, id, &fsi_arc_rom, p_ovt9->offset,
                                       p_ovt9->length, p_ovt7->offset, p_ovt7->length);
    }
}

/* HMAC the image with the digest key and compare the 20-byte result with
 * spec_digest a word at a time. */
int FSi_CompareDigest(const u8 *spec_digest, void *src, int len) {
    int i;
    u8 digest[FS_OVERLAY_DIGEST_SIZE];
    u8 digest_key[64];

    MI_CpuFill8(digest, 0, sizeof(digest));
    MI_CpuCopy8(fsi_digest_key_ptr, digest_key, (u32)fsi_digest_key_len);
    DGT_Hash2CalcHmac(digest, src, len, digest_key, fsi_digest_key_len);

    for (i = 0; i < sizeof(digest); i += sizeof(u32)) {
        if (*(const u32 *)(digest + i) != *(const u32 *)(spec_digest + i))
            break;
    }

    return (i == sizeof(digest));
}

/* Once an overlay's image is in RAM, a multiboot child (MB_IsMultiBootChild)
 * checks the digest of an overlay flagged authenticated
 * against its entry at SDK_OVERLAY_DIGEST. It clears the image and crashes when
 * the check fails, and also when the overlay is not flagged authenticated
 * at all. Then the image is decompressed in place when flagged compressed
 * (MIi_UncompressBackward, given the end of the compressed image), and its
 * static initializers run from sinit_init to sinit_init_end. */
void FS_StartOverlay(FSOverlayInfo *p_ovi) {
    const u32 flag = p_ovi->header.flag;
    u32 rare_size =
        (flag & FS_OVERLAY_FLAG_COMP) ? p_ovi->header.compressed : p_ovi->header.ram_size;

    if (MB_IsMultiBootChild()) {
        BOOL ret = FALSE;

        if ((flag & FS_OVERLAY_FLAG_AUTH) != 0) {
            const u32 odt_max =
                (u32)((SDK_OVERLAY_DIGEST_END - SDK_OVERLAY_DIGEST) / FS_OVERLAY_DIGEST_SIZE);
            if (p_ovi->header.id < odt_max) {
                const u8 *spec_digest =
                    (SDK_OVERLAY_DIGEST + FS_OVERLAY_DIGEST_SIZE * p_ovi->header.id);
                ret = FSi_CompareDigest(spec_digest, p_ovi->header.ram_address, (int)rare_size);
            }
        }
        if (!ret) {
            MI_CpuClear8(p_ovi->header.ram_address, rare_size);
            _Z12OS_Terminatev();
            return;
        }
    }

    if ((p_ovi->header.flag & FS_OVERLAY_FLAG_COMP) != 0) {
        MIi_UncompressBackward(p_ovi->header.ram_address + rare_size);
    }

    {
        FSOverlayInitFunc *p = p_ovi->header.sinit_init;
        FSOverlayInitFunc *q = p_ovi->header.sinit_init_end;

        for (; p < q; ++p) {
            if (*p)
                (**p)();
        }
    }
}

/* With interrupts disabled, unlink every __global_destructor_chain entry
 * whose destructor lies in the overlay's image or whose object lies in its
 * bss, run the ones unlinked, and repeat until a pass finds none. Then take
 * the overlay off the IS-NITRO debugger's list. */
void FS_EndOverlay(FSOverlayInfo *p_ovi) {
    for (;;) {
        MWiDestructorChain *head = NULL, *tail = NULL;
        const u32 region_top = (u32)FS_GetOverlayAddress(p_ovi);
        const u32 image_bottom = region_top + FS_GetOverlayImageSize(p_ovi);
        const u32 region_bottom =
            image_bottom + (FS_GetOverlayTotalSize(p_ovi) - FS_GetOverlayImageSize(p_ovi));

        {
            OSIntrMode bak_psr = OS_DisableInterrupts();
            MWiDestructorChain *prev = NULL;
            MWiDestructorChain *base = __global_destructor_chain;
            MWiDestructorChain *p = base;

            while (p) {
                MWiDestructorChain *next = p->next;
                const u32 dtor = (u32)p->dtor;
                const u32 obj = (u32)p->obj;

                if (((dtor >= region_top) && (dtor < image_bottom)) ||
                    ((obj >= image_bottom) && (obj < region_bottom))) {

                    if (!tail) {
                        head = p;
                    } else {
                        tail->next = p;
                    }
                    if (base == p) {
                        base = __global_destructor_chain = next;
                    }
                    tail = p, p->next = NULL;
                    if (prev) {
                        prev->next = next;
                    }
                } else {
                    prev = p;
                }
                p = next;
            }
            OS_RestoreInterrupts(bak_psr);
        }

        if (!head) {
            break;
        }

        do {
            MWiDestructorChain *next = head->next;
            if (head->dtor) {
                (*head->dtor)(head->obj);
            }
            head = next;
        } while (head);
    }

    _Z29_ISDbgLib_UnregistOverlayInfo11OVERLAYPROCjj(
        (p_ovi->target == MI_PROCESSOR_ARM9) ? OVERLAYPROC_ARM9 : OVERLAYPROC_ARM7,
        (u32)FS_GetOverlayAddress(p_ovi), FS_GetOverlayImageSize(p_ovi));
}

BOOL FS_UnloadOverlayImage(FSOverlayInfo *p_ovi) {
    FS_EndOverlay(p_ovi);
    return TRUE;
}

/* Read the overlay's FSOverlayInfo and unload its image. */
BOOL FS_UnloadOverlay(MIProcessor target, FSOverlayID id) {
    {
        FSOverlayInfo ovi;
        if (!FS_LoadOverlayInfo(&ovi, target, id) || !FS_UnloadOverlayImage(&ovi))
            return FALSE;
    }
    return TRUE;
}

/* Attach an in-memory overlay table for target, so FS_LoadOverlayInfo
 * reads entries from it. An ARM9 table is first checked against the
 * digest at SDK_OVERLAYTABLE_DIGEST, with DGT_SetOverlayTableMode(1) in effect
 * around the check, and a mismatch crashes. Then ptr and size go into the
 * target's region with interrupts disabled. */
void FS_AttachOverlayTable(MIProcessor target, const void *ptr, u32 len) {
    CARDRomRegion *const pr = (target == MI_PROCESSOR_ARM9) ? &fsi_ovt9 : &fsi_ovt7;

    if ((ptr != NULL) && (target == MI_PROCESSOR_ARM9)) {
        int bak_ovt_mode = DGT_SetOverlayTableMode(TRUE);
        if (!FSi_CompareDigest((const u8 *)SDK_OVERLAYTABLE_DIGEST, (void *)ptr, (int)len)) {
            _Z12OS_Terminatev();
        }
        DGT_SetOverlayTableMode(bak_ovt_mode);
    }

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();

        pr->offset = (u32)ptr;
        pr->length = len;

        OS_RestoreInterrupts(bak_psr);
    }
}
