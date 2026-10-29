extern void Crash(void);

/* NitroSDK MbSegmentInfo: one row of a download file's segment table (same
 * names as func_02067604's and func_020678a4's views). */
typedef struct MbSegmentInfo {
    unsigned recv_addr;  /* 0x00 -- where the child receives it */
    unsigned load_addr;  /* 0x04 -- where it finally runs */
    unsigned size;       /* 0x08 */
    unsigned target : 1; /* 0x0c -- set for the ARM7 segment */
    unsigned rsv : 31;
} MbSegmentInfo;

/* A static module's load region in the ROM header: NitroSDK RomHeader's
 * main_ram_address/main_size (ARM9) and sub_ram_address/sub_size (ARM7)
 * pairs. */
typedef struct RomStaticRegion {
    unsigned ram_address; /* 0x00 */
    unsigned size;        /* 0x04 */
} RomStaticRegion;

/* NitroSDK RomHeader, the two static modules' load regions read here. */
typedef struct RomHeaderView {
    unsigned char pad_00[0x28];
    RomStaticRegion main; /* 0x28 -- ARM9 static */
    unsigned char pad_30[0x38 - 0x30];
    RomStaticRegion sub; /* 0x38 -- ARM7 static */
} RomHeaderView;

/* NitroSDK MBi_SetSegmentInfo(mrh, seg_type, seg_info, child_recv_buff_addr):
 * fill one segment row for func_020678a4 (MBi_MakeDownloadFileInfo). Segment
 * type 0 is the ARM9 static, which must lie in 0x02000000..0x022c0000; 1 is
 * the ARM7 static, which may sit in main RAM (staged at *recvCursor when it
 * reaches past 0x02300000) or in ARM7 WRAM 0x037f8000..0x0380f000 (always
 * staged); 2 is the 0x160-byte ROM header at 0x027ffe00. A segment the child
 * cannot load is fatal. */
void func_020676e0(unsigned *rom, unsigned *seg_type, unsigned *row, unsigned *recvCursor) {
    RomHeaderView *mrh = (RomHeaderView *)rom;
    MbSegmentInfo *seg_info = (MbSegmentInfo *)row;
    unsigned mode = *seg_type;
    RomStaticRegion *region;
    unsigned addr, size, end;
    unsigned bad, special;

    switch (mode) {
    case 0:
        region = &mrh->main;
        addr = region->ram_address;
        if (addr >= 0x2000000u && addr < 0x22c0000u) {
            size = region->size;
            if (addr + size <= 0x22c0000u) {
                seg_info->size = size;
                seg_info->load_addr = region->ram_address;
                seg_info->recv_addr = seg_info->load_addr;
                seg_info->target = 0;
                return;
            }
        }
        Crash();
        return;

    case 1:
        region = &mrh->sub;
        addr = region->ram_address;
        size = region->size;
        bad = 0;
        special = bad;
        {
            unsigned e = addr + size;
            end = e;
        }
        if (addr >= 0x2000000u && addr < 0x23fe800u) {
            if (end > 0x2300000u) {
                if (end < 0x23fe800u && size <= 0x40000u)
                    special = 1;
                else
                    bad = 1;
            }
        } else if (addr >= 0x37f8000u && addr < 0x380f000u) {
            if (end <= 0x380f000u)
                special = 1;
            else
                bad = 1;
        } else {
            bad = 1;
        }
        if (bad == 1)
            Crash();
        seg_info->size = region->size;
        seg_info->load_addr = region->ram_address;
        if (special == 0)
            seg_info->recv_addr = seg_info->load_addr;
        else {
            seg_info->recv_addr = *recvCursor;
            *recvCursor = *recvCursor + seg_info->size;
        }
        seg_info->target = 1;
        return;

    case 2:
        seg_info->size = 0x160;
        seg_info->load_addr = 0x027ffe00u;
        seg_info->recv_addr = seg_info->load_addr;
        seg_info->target = 0;
        break;
    }
}
