/* The game's checked wrappers around the NitroSDK FS calls. Each forwards its
 * arguments to the SDK function and, on failure, hands its own name string to
 * func_02018e68 and crashes. The names come from those strings, which sit in
 * .rodata at the addresses the externs below give:
 *   0x0208eb8c "myFS_OpenFile"          0x0208ec74 "myFS_OpenFileFast"
 *   0x0208eb9c "myFS_ReadFile"          0x0208ecc4 "myFS_OpenFileDirect"
 *   0x0208ecd8 "myFS_ConvertPathToFileID"
 * func_02018e68 records the string in data_0209d3c0 and sleeps the thread
 * when func_0203d7d4(0) is nonzero. myFS_ReadFile treats a short read as a
 * failure. */
#include "types.h"
#include "private/fs_views.h"

struct BC {
    int x, y;
};

extern int func_0203d7d4(int);
extern void OS_SleepThread(int);
extern void *data_0209d3c0;
extern int func_0205d644(void *out_file_id, const char *path);
extern void Crash(void);
extern int data_0208ecd8[];
extern int FS_OpenFileDirect(char *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom,
                             u32 file_index);
extern int data_0208ecc4;
extern char data_0208ec74;
int func_0205d568(int a, struct BC bc);
extern int func_0205d518(void *file, const char *path);
extern int data_0208eb8c[];
extern int FS_ReadFile(int a, int b, int c);
extern void func_0205d4a0(int a, void *buf, int n);
extern char data_0208eb9c;

void func_02018e68(void *c) {
    if (func_0203d7d4(0) == 0)
        return;
    data_0209d3c0 = c;
    OS_SleepThread(0);
}

/* recovered: the ROM reaches func_0205d644 with r0 and r1 still holding this
 * function's own two arguments (push {r4, lr} / bl func_0205d644 / movs r4, r0),
 * so both parameters are real and are forwarded. Every caller in the image
 * already passes two: func_020182bc, func_02018434, func_020189f0. */
int myFS_ConvertPathToFileID(void *out_file_id, const char *path) {
    int r = func_0205d644(out_file_id, path);
    if (!r) {
        func_02018e68(data_0208ecd8);
        Crash();
    }
    return r;
}

int myFS_OpenFileDirect(char *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom,
                        u32 file_index) {
    int r = FS_OpenFileDirect(p_file, p_arc, image_top, image_bottom, file_index);
    if (r == 0) {
        func_02018e68(&data_0208ecc4);
        Crash();
    }
    return r;
}

int myFS_OpenFileFast(int a, int b, int c, ...) {
    int r = func_0205d568(a, *(struct BC *)&b);
    if (r == 0) {
        func_02018e68(&data_0208ec74);
        Crash();
    }
    return r;
}

/* recovered: both arguments ride through. The body is push {r4, lr} / bl
 * func_0205d518 / movs r4, r0, so r0 (the FSFile) and r1 (the path) reach the
 * callee untouched. func_02018934 already calls this with two. */
int myFS_OpenFile(void *file, const char *path) {
    int r = func_0205d518(file, path);
    if (!r) {
        func_02018e68(data_0208eb8c);
        Crash();
    }
    return r;
}

int myFS_ReadFile(int a, int b, int c) {
    char buf[0x100];
    int r = FS_ReadFile(a, b, c);
    if (r != c) {
        func_02018e68(&data_0208eb9c);
        func_0205d4a0(a, buf, 0x100);
        Crash();
    }
    return r;
}
