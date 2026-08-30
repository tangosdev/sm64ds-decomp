//cpp
// @symbol func_ov075_02117d80
/* recovered: dScEntry_c entry-menu setup for the non-VS path.
 *
 * Logic first recovered by andrewboudreau as a NONMATCHING body against
 * mwccarm 1.2/sp2p3; byte-matched here at 2004/b56 without changing what it
 * does. Two shapes were load-bearing: the mode test is a static inline
 * predicate in a C++ TU (the ROM materializes the bool, which C mode folds
 * away under every spelling - notes 6c/6bl), and the pair of words copied
 * out of data_ov075_0211d810 is an array-member struct copy, which keeps
 * both loads ahead of the flag read-modify-write instead of sinking the
 * second one past the first store.
 */
#include "types.h"

extern "C" {
extern void func_02034414(unsigned short n);
extern int LoadFile(int handle);
extern void func_ov075_02116030(void *c, int v);
extern void func_ov075_02115e8c(void *c, int a, int b, int d, int e);
}
extern unsigned char data_0209d454;
extern int data_ov075_0211d71c;
extern int data_ov075_0211d810[];
struct P2Copy { int words[2]; };

static inline int IsVsMode(const char *c) { return *(unsigned short *)(c + 0xc) == 6; }

extern "C" void func_ov075_02117d80(char *c)
{
    int fh;
    int v0, v1;

    data_0209d454 = data_0209d454 & ~1;
    if (IsVsMode(c))
    {
        func_02034414(0x19);
    }
    else
    {
        data_0209d454 = data_0209d454 & ~4;
        fh = LoadFile(0x9802);
        func_ov075_02116030(&data_ov075_0211d71c, fh);
        func_02034414(0x16);
        data_0209d454 = data_0209d454 | 4;
        *(struct P2Copy *)(c + 0x64) = *(struct P2Copy *)&data_ov075_0211d810[4];
    }
    data_0209d454 = data_0209d454 | 1;
    *(unsigned char *)(c + 0x280) = 2;
    *(unsigned char *)(c + 0x281) = 1;
    func_ov075_02115e8c(c + 0x70, 8, 0, 0x50, 0xa8);
    func_ov075_02115e8c(c + 0x94, 8, 0, 0xb0, 0xa8);
}
