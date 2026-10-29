/* NitroSDK FS_LoadOverlayImage: open the overlay's file by its file_id,
 * clear the overlay's RAM image (func_0205e0b0, FS_ClearOverlayImage), read
 * the whole file to ram_address (func_02018d48), then pass the target
 * processor, RAM address, ROM offset and RAM size to func_0206d32c.
 *
 * FSOverlayInfo is the SDK layout: the 0x20-byte header func_02018c00
 * copies from the overlay table, then the target processor it stores at
 * +0x20. Only the fields read here are spelled out. FSFile follows
 * func_0205bc88.c's view (prop.file at 0x1c), padded to the SDK's 0x48. */
typedef struct FSOverlayInfo {
    char pad_00[4];
    void *ram_address; /* 0x04 */
    unsigned ram_size; /* 0x08 */
    char pad_0c[0x14];
    int target; /* 0x20 -- MIProcessor: 0 = ARM9 */
} FSOverlayInfo;

typedef struct FSFileProp {
    unsigned own_id; /* 0x00 */
    unsigned top;    /* 0x04 */
    unsigned bottom; /* 0x08 */
    unsigned pos;    /* 0x0c */
} FSFileProp;

typedef struct FSFile {
    char pad_00[0x1c];
    FSFileProp prop; /* 0x1c -- prop.file */
    char pad_2c[0x1c];
} FSFile;

typedef char FSFile_size_must_be_0x48[sizeof(FSFile) == 0x48 ? 1 : -1];

typedef struct Pair { int a; int b; } Pair;

extern void FS_InitFile(void *s);
extern void func_0205e088(Pair *dst, void *src);
extern int func_02018dc4(void *a, Pair bc);
extern void func_0205e0b0(void *info);
extern int func_02018d48(void *a, int b, int c);
extern void func_0206d32c(int a, int b, int c, int d);
extern int FS_CloseFile(void *self);

int func_02018b64(char *self)
{
    FSFile file;
    Pair v;
    FSOverlayInfo *ovi = (FSOverlayInfo *)self;

    FS_InitFile(&file);
    func_0205e088(&v, self);
    if (func_02018dc4(&file, v) == 0)
        return 0;

    func_0205e0b0(self);
    func_02018d48(&file, (int)ovi->ram_address, file.prop.bottom - file.prop.top);

    {
        int d = ovi->ram_size;
        int c = file.prop.top;
        int b = (int)ovi->ram_address;
        /* the plain `ovi->target != 0` changes the .text bytes */
        int flag = (int)((ovi->target == 0) ? (char *)0 : (char *)1);
        func_0206d32c(flag, b, c, d);
    }

    FS_CloseFile(&file);
    return 1;
}
