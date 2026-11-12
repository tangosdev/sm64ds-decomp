#include "types.h"
#include "Ov007Work.h"
#include "Ov007MenuMem.h"

/* "global data": the heap name func_ov007_020c3df4 is handed (and does not
 * forward) */
extern char data_ov007_02102dc4[];

extern void func_ov007_020b6a84(void);
extern void func_ov007_020b697c(void);
extern void func_ov007_020b69c4(void);
extern void func_ov007_020b693c(void);
extern void func_ov007_020b6d40(void);

/* Allocates the work block and builds its parts: the settings, textures,
 * palettes, item views and item nodes. */
void Work_Create(void) {
    WORK = func_ov007_020c3df4(data_ov007_02102dc4, sizeof(Ov007Work));
    func_ov007_020b6a84();
    func_ov007_020b697c();
    func_ov007_020b69c4();
    func_ov007_020b693c();
    func_ov007_020b6d40();
    WORK->pad_184 = 0;
}
