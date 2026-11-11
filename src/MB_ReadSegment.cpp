//cpp
#include "nitro/os/cache.h"
#include "private/fs_views.h"
#include "nitro/mi/memory.h"

typedef struct FSFile FSFile;

extern "C" {

typedef struct {
    char pad00[0x20];
    unsigned int wr;
    char pad24[4];
    unsigned int rd;
    unsigned int f2c;
    unsigned int f30;
    char pad34[8];
    unsigned int f3c;
    char pad40[0x20];
    unsigned int f60;
    char pad64[0x1c];
    unsigned int f80;
} Ctx;

typedef struct {
    unsigned int a, b, c, d;
} Args;

extern int FS_ReadFile(FSFile *p_file, void *dst, int len);
extern void FS_InitFile(FSFile *p_file);
extern FSArchive *FS_FindArchive(const char *name, int name_len);
extern int FS_OpenFileDirect(char *p_file, FSArchive *p_arc, unsigned int image_top,
                             unsigned int image_bottom, unsigned int file_index);
extern int FS_SeekFile(FSFile *p_file, int offset, FSSeekFileMode origin);
extern int FS_CloseFile(char *self);
extern void MBi_ReadSegmentHeader(void *s, unsigned int start, unsigned int end, int flag);

/* One entry of the zero-size-terminated list at mbi_seg_header. */
typedef struct {
    unsigned int start, size;
} Region;

extern char data_0209a080[];
extern Region *mbi_seg_header;

/* Copies one listed range through MBi_ReadSegmentHeader (flag 0: MI_CpuCopy8). Taking
 * the entry by pointer makes the body reload size after the loop test, as the
 * ROM does. */
static inline void CopyRegion(Args *args, const Region *r) {
    MBi_ReadSegmentHeader(args, r->start, r->start + r->size, 0);
}

int MB_ReadSegment(FSFile *a, Ctx *b, unsigned int c) {
    int result;
    unsigned int remain;
    char *cur;
    int base;
    Ctx *ctx;
    int flag;
    unsigned int threshold;
    FSArchive *node;
    Region *rgn;
    Args args;
    FSFile file;

    result = 0;
    if (c >= 0x160) {
        flag = 0;
        remain = c;
        cur = (char *)b;
        ctx = b;

        if (a != 0) {
            base = a->prop.file.pos - a->prop.file.top;
            if (FS_ReadFile(a, b, 0x160) < 0x160) {
                remain = 0;
            }
        } else {
            flag = 1;
            ctx = (Ctx *)0x027FFE00;
            if (ctx->f80 == 0) {
                ctx->f80 = 0x1000000;
            }
            FS_InitFile(&file);
            node = FS_FindArchive(data_0209a080, 3);
            FS_OpenFileDirect((char *)&file, node, 0, ctx->f80 + 0x88, -1);
            a = &file;
            base = file.prop.file.pos - file.prop.file.top;
            MI_CpuCopy8(ctx, cur, 0x160);
            *(unsigned int *)(cur + 0x60) |= 0x406000;
        }

        threshold = ctx->f2c + 0x160 + ctx->f3c;
        if (remain >= threshold) {
            cur += 0x160;
            remain -= 0x160;
            FS_SeekFile(a, base + ctx->wr, FS_SEEK_SET);
            FS_ReadFile(a, cur, ctx->f2c);
            cur += ctx->f2c;
            remain -= ctx->f2c;

            FS_SeekFile(a, base + ctx->f30, FS_SEEK_SET);
            FS_ReadFile(a, cur, ctx->f3c);
            cur += ctx->f3c;
            remain -= ctx->f3c;
        }

        if (remain >= 0x88) {
            FS_SeekFile(a, base + ctx->f80, FS_SEEK_SET);
            FS_ReadFile(a, cur, 0x88);
            result = 1;
        }

        FS_SeekFile(a, base, FS_SEEK_SET);

        if (flag) {
            FS_CloseFile((char *)&file);
            if (result) {
                args.a = ctx->wr;
                rgn = mbi_seg_header;
                args.b = ctx->rd - ctx->wr;
                args.c = ((unsigned int)b + 0x160) - ctx->wr;
                args.d = c;
                MBi_ReadSegmentHeader(&args, 0x4000, 0x8000, 1);
                for (; rgn->size != 0; rgn++)
                    CopyRegion(&args, rgn);
            }
        }

        if (result) {
            DC_FlushRange(b, c);
        }
    }

    return result;
}
}
