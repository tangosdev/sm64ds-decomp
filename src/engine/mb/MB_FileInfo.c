/*
 * The multiboot parent's download-file table, the Nitro SDK's mb_fileinfo.c:
 * reading a program's segments into a buffer (MB_GetSegmentLength,
 * MB_ReadSegment), registering it for download (MB_RegisterFile) and
 * splitting it into the blocks the parent sends (MBi_MakeBlockInfoTable,
 * MBi_get_blockinfo), with the load-area checks both ends apply.
 *
 * This SDK is older than ntrtwl/NitroSDK's copy: there is no segment cache,
 * so MB_ReadSegment reads the two static modules whole, and the
 * authentication code follows them in the buffer, where
 * MB_RegisterFile finds it for MBi_MakeDownloadFileInfo. MB_UnregisterFile
 * and MBi_get_blocknum are not in the ROM: nothing calls them.
 */

#include "types.h"
#include "nitro/hw/mmap_shared.h"
#include "nitro/os/system.h"
#include "nitro/os/cache.h"
#include "nitro/mi/memory.h"
#include "nitro/card/card_rom.h"
#include "nitro/fs/file.h"
#include "nitro/fs/archive.h"
#include "private/rom_header_view.h"
#include "private/mb_views.h"
#include "private/arm9_a9db8_mb_parent.h"

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

/* mb_child.h and mb_fileinfo.h: where a child may load the downloaded
 * program. */
#define ROM_HEADER_SIZE_FULL 0x160
#define MB_AUTHCODE_SIZE 136
#define MB_DL_SEGMENT_NUM 3
#define MB_MAX_FILE 16
#define MB_GAMEINFO_PARENT_FLAG 0x0001

#define HW_WRAM 0x037f8000

#define MB_LOAD_AREA_LO (HW_MAIN_MEM)
#define MB_LOAD_AREA_HI (HW_MAIN_MEM + 0x002c0000)
#define MB_ARM7_STATIC_RECV_BUFFER MB_LOAD_AREA_HI
#define MB_ARM7_STATIC_RECV_BUFFER_END (HW_MAIN_MEM + 0x00300000)
#define MB_ARM7_STATIC_RECV_BUFFER_SIZE (MB_ARM7_STATIC_RECV_BUFFER_END - MB_LOAD_AREA_HI)
#define MB_ARM7_STATIC_LOAD_AREA_HI (MB_LOAD_AREA_HI + MB_ARM7_STATIC_RECV_BUFFER_SIZE)
#define MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE (0x18000 - 0x1000)
#define MB_BSSDESC_ADDRESS (HW_MAIN_MEM + 0x003fe800)
#define MB_ROM_HEADER_ADDRESS HW_ROM_HEADER_BUF

#define MI_PROCESSOR_ARM9 0
#define MI_PROCESSOR_ARM7 1

typedef enum MbSegmentType {
    MB_SEG_ARM9STATIC = 0,
    MB_SEG_ARM7STATIC,
    MB_SEG_ROMHEADER
} MbSegmentType;

/* The SDK's function-local `static u8 update` in MB_RegisterFile. It is
 * held outside this TU: one byte, then alignment before mb_gameinfo.c's
 * .bss, which the TU build cannot claim yet. */
extern u8 data_020a9d28;

extern BOOL MBi_IsStarted(void);
extern void MBi_MakeGameInfo(char *dst, MBGameRegistry *src, const void *parent);
extern void MB_AddGameInfo(MBGameInfo *newGameInfo);

/* fs/file.h's inline, kept here: an inline body in the shared header would
 * renumber every C++ file that includes it. */
static inline u32 FS_GetPosition(FSFile *p_file) {
    return p_file->prop.file.pos - p_file->prop.file.top;
}

const MbSegmentType MBi_defaultLoadSegList[MB_DL_SEGMENT_NUM] = {
    MB_SEG_ROMHEADER,
    MB_SEG_ARM9STATIC,
    MB_SEG_ARM7STATIC,
};

#define MB_SEGMENT_HEADER_MIN 0x4000
#define MB_SEGMENT_HEADER_MAX 0x8000

static const CARDRomRegion mbi_seg_header_default[] = {
    {0x4000, 0x1000},
    {0x7000, 0x1000},
    {0, 0},
};

static const CARDRomRegion *mbi_seg_header = mbi_seg_header_default;

typedef struct MBiSegmentHeaderInfo {
    u32 rom_src;
    u32 mem_src;
    u32 mem_dst;
    u32 len;
} MBiSegmentHeaderInfo;

static void MBi_MakeDownloadFileInfo(MbDownloadFileInfoHeader *mbdlFileInfop, const void *buf,
                                     const void *auth_code);
static void MBi_SetSegmentInfo(const RomHeaderView *mbRomHeaderp, const MbSegmentType *loadSegListp,
                               MbSegmentInfo *seg_infop, u32 *child_recv_buff_addr);
static void MBi_ReadSegmentHeader(const MBiSegmentHeaderInfo *p, u32 top, u32 bottom, BOOL clean);
static BOOL IsAbleToLoad(u8 segment_no, u32 address, u32 size);

u32 MB_GetSegmentLength(FSFile *file) {
    enum { ROM_HEADER_SIZE_SMALL = 0x60 };

    u8 rom[ROM_HEADER_SIZE_SMALL];
    const RomHeaderView *p = NULL;

    if (file) {
        const u32 bak_pos = FS_GetPosition(file);

        if (FS_ReadFile(file, &rom, sizeof(rom)) >= sizeof(rom)) {
            p = (const RomHeaderView *)rom;
        }

        (void)FS_SeekFile(file, (int)bak_pos, FS_SEEK_SET);
    } else {
        p = (const RomHeaderView *)HW_ROM_HEADER_BUF;
    }

    if (p) {
        return ROM_HEADER_SIZE_FULL + MB_AUTHCODE_SIZE + p->main.size + p->sub.size;
    }
    return 0;
}

BOOL MB_ReadSegment(FSFile *file, void *buf, u32 len) {
    BOOL ret = FALSE;

    if (len >= ROM_HEADER_SIZE_FULL) {
        BOOL is_own_binary = FALSE;
        u32 rest = len;
        u8 *dst = (u8 *)buf;
        u32 bak_pos;
        const RomHeaderView *p_rom = (const RomHeaderView *)buf;
        FSFile own_bin[1];

        if (file) {
            bak_pos = FS_GetPosition(file);

            if (FS_ReadFile(file, buf, ROM_HEADER_SIZE_FULL) < ROM_HEADER_SIZE_FULL)
                rest = 0;
        } else {
            RomHeaderView *mem_rom = (RomHeaderView *)HW_ROM_HEADER_BUF;

            is_own_binary = TRUE;
            p_rom = mem_rom;

            if (!mem_rom->own_size)
                mem_rom->own_size = 16 * 1024 * 1024;

            FS_InitFile(own_bin);
            (void)FS_OpenFileDirect(own_bin, FS_FindArchive("rom", 3), 0,
                                    (u32)(mem_rom->own_size + MB_AUTHCODE_SIZE), (u32)~0);
            file = own_bin;
            bak_pos = FS_GetPosition(file);

            MI_CpuCopy8(mem_rom, dst, ROM_HEADER_SIZE_FULL);

            *(u32 *)((u32)dst + 0x60) |= 0x00406000;
        }

        if (rest >= ROM_HEADER_SIZE_FULL + p_rom->main.size + p_rom->sub.size) {
            dst += ROM_HEADER_SIZE_FULL, rest -= ROM_HEADER_SIZE_FULL;
            (void)FS_SeekFile(file, (int)(bak_pos + p_rom->main_rom_offset), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, (int)p_rom->main.size);
            dst += p_rom->main.size, rest -= p_rom->main.size;

            (void)FS_SeekFile(file, (int)(bak_pos + p_rom->sub_rom_offset), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, (int)p_rom->sub.size);
            dst += p_rom->sub.size, rest -= p_rom->sub.size;
        }

        if (rest >= MB_AUTHCODE_SIZE) {
            (void)FS_SeekFile(file, (int)(bak_pos + p_rom->own_size), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_AUTHCODE_SIZE);
            ret = TRUE;
        }

        (void)FS_SeekFile(file, (int)bak_pos, FS_SEEK_SET);

        if (is_own_binary) {
            (void)FS_CloseFile(own_bin);

            if (ret) {
                const CARDRomRegion *p_header;
                MBiSegmentHeaderInfo info;
                info.rom_src = p_rom->main_rom_offset;
                p_header = mbi_seg_header;
                info.mem_src = p_rom->main.ram_address - p_rom->main_rom_offset;
                info.mem_dst = ((u32)buf + ROM_HEADER_SIZE_FULL) - p_rom->main_rom_offset;
                info.len = len;

                MBi_ReadSegmentHeader(&info, MB_SEGMENT_HEADER_MIN, MB_SEGMENT_HEADER_MAX, TRUE);

                for (; p_header->length != 0; ++p_header) {
                    MBi_ReadSegmentHeader(&info, p_header->offset,
                                          (u32)(p_header->offset + p_header->length), FALSE);
                }
            }
        }

        if (ret) {
            DC_FlushRange(buf, len);
        }
    }

    return ret;
}

static void MBi_ReadSegmentHeader(const MBiSegmentHeaderInfo *p, u32 top, u32 bottom, BOOL clean) {
    if (top < MB_SEGMENT_HEADER_MIN)
        top = MB_SEGMENT_HEADER_MIN;

    if (bottom > MB_SEGMENT_HEADER_MAX)
        bottom = MB_SEGMENT_HEADER_MAX;

    if (top < p->rom_src)
        top = p->rom_src;

    if (bottom > p->rom_src + p->len)
        bottom = p->rom_src + p->len;

    if (top < bottom) {
        if (clean) {
            MI_CpuClear8((void *)(p->mem_dst + top), (u32)(bottom - top));
        } else {
            MI_CpuCopy8((const void *)(p->mem_src + top), (void *)(p->mem_dst + top),
                        (u32)(bottom - top));
        }
    }
}

BOOL MB_RegisterFile(MBGameRegistry *game_reg, const void *buf) {
    MbDownloadFileInfoHeader *mdfi;
    u8 fileID;
    const u8 *auth_code;
    OSIntrMode enabled;
    u8 i;

    auth_code = (const u8 *)buf + ROM_HEADER_SIZE_FULL + ((const RomHeaderView *)buf)->main.size +
                ((const RomHeaderView *)buf)->sub.size;
    fileID = 0xff;

    enabled = OS_DisableInterrupts();

    if (!MBi_IsStarted()) {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    if (pPwork->file_num + 1 > MB_MAX_FILE) {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    for (i = 0; i < MB_MAX_FILE; i++) {
        if (pPwork->fileinfo[i].game_reg == game_reg) {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        if (pPwork->fileinfo[i].active == 0) {
            fileID = i;
            break;
        }
    }

    if (i == MB_MAX_FILE) {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    pPwork->fileinfo[fileID].game_reg = game_reg;

    mdfi = &pPwork->fileinfo[fileID].dl_fileinfo.header;

    MBi_MakeDownloadFileInfo(mdfi, buf, auth_code);

    if (FALSE == MBi_MakeBlockInfoTable(&pPwork->fileinfo[fileID].blockinfo_table, mdfi)) {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    MBi_MakeGameInfo((char *)&pPwork->fileinfo[fileID].game_info, game_reg, &mbc->user);
    pPwork->fileinfo[fileID].game_info.fileNo = fileID;

    MB_AddGameInfo(&pPwork->fileinfo[fileID].game_info);
    pPwork->fileinfo[fileID].game_info.seqNoFixed = data_020a9d28++;
    pPwork->fileinfo[fileID].gameinfo_child_bmp = MB_GAMEINFO_PARENT_FLAG;
    pPwork->fileinfo[fileID].src_adr = (u32 *)buf;

    pPwork->fileinfo[fileID].active = 1;
    pPwork->file_num++;

    (void)OS_RestoreInterrupts(enabled);

    return TRUE;
}

static void MBi_MakeDownloadFileInfo(MbDownloadFileInfoHeader *mbdlFileInfop, const void *buf,
                                     const void *auth_code) {
    const RomHeaderView *mbRomHeaderp;
    const MbSegmentType *loadSegListp;
    MbSegmentInfo *seg_infop;
    int seg_num;
    u32 child_recv_buff_addr = MB_ARM7_STATIC_RECV_BUFFER;

    mbRomHeaderp = (const RomHeaderView *)buf;

    mbdlFileInfop->arm9EntryAddr = mbRomHeaderp->main_entry_address;
    mbdlFileInfop->arm7EntryAddr = mbRomHeaderp->sub_entry_address;
    seg_infop = mbdlFileInfop->seg;
    loadSegListp = MBi_defaultLoadSegList;

    for (seg_num = 0; seg_num < MB_DL_SEGMENT_NUM; seg_num++) {
        MBi_SetSegmentInfo(mbRomHeaderp, loadSegListp, seg_infop, &child_recv_buff_addr);
        seg_infop++;
        loadSegListp++;
    }

    {
        MBDownloadFileInfo *pMdfi = (MBDownloadFileInfo *)mbdlFileInfop;
        MI_CpuCopy8(auth_code, &pMdfi->auth_code[0], MB_AUTHCODE_SIZE);
    }
}

/* The SDK's OS_Panic calls are OS_Terminate in a final ROM. */
static void MBi_SetSegmentInfo(const RomHeaderView *mbRomHeaderp, const MbSegmentType *loadSegListp,
                               MbSegmentInfo *seg_infop, u32 *child_recv_buff_addr) {
    const RomStaticRegion *romRegp;

    switch (*loadSegListp) {
    case MB_SEG_ARM9STATIC:
        romRegp = &mbRomHeaderp->main;

        if ((romRegp->ram_address >= MB_LOAD_AREA_LO) && (romRegp->ram_address < MB_LOAD_AREA_HI) &&
            ((romRegp->ram_address + romRegp->size) <= MB_LOAD_AREA_HI)) {

            seg_infop->size = romRegp->size;
            seg_infop->load_addr = romRegp->ram_address;

            seg_infop->recv_addr = seg_infop->load_addr;
            seg_infop->target = MI_PROCESSOR_ARM9;
        } else {
            OS_Terminate();
        }
        break;

    case MB_SEG_ARM7STATIC: {
        BOOL error_flag = FALSE;
        BOOL reload_flag = FALSE;
        u32 load_last_addr;

        romRegp = &mbRomHeaderp->sub;
        load_last_addr = romRegp->ram_address + romRegp->size;

        if ((romRegp->ram_address >= MB_LOAD_AREA_LO) &&
            (romRegp->ram_address < MB_BSSDESC_ADDRESS)) {

            if (load_last_addr <= MB_ARM7_STATIC_LOAD_AREA_HI) {
            } else if ((load_last_addr < MB_BSSDESC_ADDRESS) &&
                       (romRegp->size <= MB_ARM7_STATIC_RECV_BUFFER_SIZE)) {
                reload_flag = TRUE;
            } else {
                error_flag = TRUE;
            }
        } else if ((romRegp->ram_address >= HW_WRAM) &&
                   (romRegp->ram_address < HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE)) {

            if (load_last_addr <= (HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE)) {
                reload_flag = TRUE;
            } else {
                error_flag = TRUE;
            }
        } else {
            error_flag = TRUE;
        }

        if (error_flag == TRUE) {
            OS_Terminate();
        }

        {
            seg_infop->size = romRegp->size;
            seg_infop->load_addr = romRegp->ram_address;

            if (!reload_flag) {
                seg_infop->recv_addr = seg_infop->load_addr;
            } else {
                seg_infop->recv_addr = *child_recv_buff_addr;
                *child_recv_buff_addr += seg_infop->size;
            }
        }

        seg_infop->target = MI_PROCESSOR_ARM7;
    } break;
    case MB_SEG_ROMHEADER:
        seg_infop->size = ROM_HEADER_SIZE_FULL;
        seg_infop->load_addr = (u32)MB_ROM_HEADER_ADDRESS;
        seg_infop->recv_addr = seg_infop->load_addr;
        seg_infop->target = MI_PROCESSOR_ARM9;
        break;
    }
}

BOOL MBi_MakeBlockInfoTable(MB_BlockInfoTable *table, MbDownloadFileInfoHeader *mdfi) {
    u16 *seg_headB = table->block_head;
    u32 *seg_src = table->seg_head_offset;
    u8 i;
    u32 src_ofs = 0;

    if (!mdfi)
        return FALSE;

    for (i = 0; i < MB_DL_SEGMENT_NUM; ++i) {
        MbSegmentInfo *si = &mdfi->seg[i];
        seg_src[i] = src_ofs;
        src_ofs += si->size;
    }

    seg_headB[0] = 0;

    for (i = 0; i < MB_DL_SEGMENT_NUM; ++i) {
        MbSegmentInfo *si = &mdfi->seg[i];
        u16 next_block_head =
            (u16)(seg_headB[i] + (u16)((si->size + mbc->block_size_max - 1) / mbc->block_size_max));

        if (FALSE == IsAbleToLoad(i, si->load_addr, si->size)) {
            return FALSE;
        }

        if (i < MB_DL_SEGMENT_NUM - 1) {
            seg_headB[i + 1] = next_block_head;
        } else {
            table->block_num = next_block_head;
        }
    }

    return TRUE;
}

BOOL MBi_get_blockinfo(MB_BlockInfo *bi, MB_BlockInfoTable *table, u32 block,
                       MbDownloadFileInfoHeader *mdfi) {
    s8 i;
    u32 seg_block, block_adr_offset;

    if (block >= table->block_num) {
        return FALSE;
    }

    for (i = MB_DL_SEGMENT_NUM - 1; i >= 0; i--) {
        if (block >= table->block_head[i]) {
            break;
        }
    }

    if (i < 0)
        return FALSE;

    seg_block = block - table->block_head[i];
    block_adr_offset = seg_block * mbc->block_size_max;

    {
        MbSegmentInfo *si = &mdfi->seg[i];
        bi->size = si->size - block_adr_offset;

        if (bi->size > mbc->block_size_max)
            bi->size = (u32)mbc->block_size_max;

        bi->offset = (u32)(block_adr_offset + table->seg_head_offset[i]);
        bi->child_address = (u32)(block_adr_offset + (u32)(si->recv_addr));
        bi->segment_no = (u8)i;
    }

    return TRUE;
}

BOOL MBi_IsAbleToRecv(u8 segment_no, u32 address, u32 size) {
    MbSegmentType seg_type;

    seg_type = MBi_defaultLoadSegList[segment_no];

    switch (seg_type) {
    case MB_SEG_ROMHEADER:
        if (address >= MB_ROM_HEADER_ADDRESS &&
            address + size <= MB_ROM_HEADER_ADDRESS + ROM_HEADER_SIZE_FULL) {
            return TRUE;
        }
        break;
    case MB_SEG_ARM9STATIC:
        if (address >= MB_LOAD_AREA_LO && address + size <= MB_LOAD_AREA_HI) {
            return TRUE;
        }
        break;
    case MB_SEG_ARM7STATIC:
        if (address >= MB_ARM7_STATIC_RECV_BUFFER &&
            address + size <= MB_ARM7_STATIC_RECV_BUFFER_END) {
            return TRUE;
        } else if (address >= MB_LOAD_AREA_LO && address + size <= MB_ARM7_STATIC_RECV_BUFFER_END) {
            return TRUE;
        }
        break;
    default:
        return FALSE;
    }

    return FALSE;
}

static BOOL IsAbleToLoad(u8 segment_no, u32 address, u32 size) {
    MbSegmentType seg_type;

    seg_type = MBi_defaultLoadSegList[segment_no];

    switch (seg_type) {
    case MB_SEG_ROMHEADER:
    case MB_SEG_ARM9STATIC:
        return MBi_IsAbleToRecv(segment_no, address, size);
    case MB_SEG_ARM7STATIC:
        if (address >= MB_LOAD_AREA_LO && address < MB_BSSDESC_ADDRESS) {
            u32 end_adr = address + size;

            if (MB_ARM7_STATIC_RECV_BUFFER_END > address &&
                MB_ARM7_STATIC_RECV_BUFFER_END < end_adr) {
                return FALSE;
            } else if (end_adr <= MB_ARM7_STATIC_LOAD_AREA_HI) {
                return TRUE;
            } else if (end_adr < MB_BSSDESC_ADDRESS && size <= MB_ARM7_STATIC_RECV_BUFFER_SIZE) {
                return TRUE;
            } else {
                return FALSE;
            }
        } else if (address >= HW_WRAM && address < HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE) {
            u32 end_adr = address + size;
            if (end_adr <= (HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE)) {
                return TRUE;
            } else {
                return FALSE;
            }
        }
        break;
    default:
        return FALSE;
    }

    return FALSE;
}
