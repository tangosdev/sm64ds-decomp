#include "Ov007Work.h"

extern void func_ov007_020c421c(char *x);

extern int *data_ov007_0210342c;
/* The extern keeps the tree's int * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

void func_ov007_020b2728(void) {
    int i;
    for (i = 0; i < 3; i++) {
        func_ov007_020c421c((char *)WORK->screenSprites[i]);
    }
}
