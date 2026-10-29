extern int func_0205de9c(char *arg0, int arg1, int arg2);
extern void func_0205a588(int a, int b, int c);
extern void Crash(void);
extern void func_020048d8(int a);

extern char data_0209aff8[];
extern char data_0209a7ec[];

/* NitroSDK FS_StartOverlay: once an overlay's image is in RAM, check its
 * digest when the halfword at 0x027ffc40 reads 2 and the overlay is
 * flagged authenticated (func_0205de9c against its 20-byte entry at
 * data_0209a7ec), clearing the image and crashing on failure; decompress it
 * in place when flagged compressed (func_020048d8, the backward
 * decompressor, given the end of the compressed image); then run its static
 * initializers from sinit_init to sinit_init_end. FSOverlayInfo is the SDK's
 * header layout; FS_LoadOverlay passes the record func_02018c00 filled. */
typedef void (*FSOverlayInitFunc)(void);

typedef struct FSOverlayInfo {
    unsigned id;                       /* 0x00 */
    char *ram_address;                 /* 0x04 */
    unsigned ram_size;                 /* 0x08 */
    unsigned bss_size;                 /* 0x0c */
    FSOverlayInitFunc *sinit_init;     /* 0x10 */
    FSOverlayInitFunc *sinit_init_end; /* 0x14 */
    unsigned file_id;                  /* 0x18 */
    unsigned compressed : 24;          /* 0x1c -- compressed image size, 0 if none */
    unsigned flag : 8;                 /* 0x1f -- bit 0 compressed, bit 1 authenticated */
} FSOverlayInfo;

void func_0205dd9c(char *self) {
    FSOverlayInfo *ovi = (FSOverlayInfo *)self;
    unsigned char flags = ovi->flag;
    unsigned int r4;
    if (flags & 1) {
        r4 = ovi->compressed;
    } else {
        r4 = ovi->ram_size;
    }
    if (*(unsigned short*)0x27ffc40 == 2) {
        int r0 = 0;
        if (flags & 2) {
            int lim = (int)(data_0209aff8 - data_0209a7ec) / 20;
            if (ovi->id < (unsigned int)lim) {
                char *p = data_0209a7ec + ovi->id * 0x14;
                r0 = func_0205de9c(p, (int)ovi->ram_address, r4);
            }
        }
        if (r0 == 0) {
            func_0205a588((int)ovi->ram_address, 0, r4);
            Crash();
            return;
        }
    }
    if (ovi->flag & 1) {
        func_020048d8((int)ovi->ram_address + r4);
    }
    {
        FSOverlayInitFunc *p = ovi->sinit_init;
        FSOverlayInitFunc *end = ovi->sinit_init_end;
        while (p < end) {
            FSOverlayInitFunc fn = *p;
            if (fn != 0) fn();
            p++;
        }
    }
}
