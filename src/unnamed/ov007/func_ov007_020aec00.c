#include "types.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"

/* Totals each save file's stars from the boot-args table: every course's
 * stars plus level 0x1d's. */
void func_ov007_020aec00(void) {
    Ov007Settings *settings;
    int i;
    int sum;
    int k;
    i = 0;
    settings = gMenuWork->settings;
    for (; i < 3; i++) {
        Ov007BootArgs *s = settings->bootArgs;
        sum = 0;
        if (s->fileUsed[i] != 0) {
            for (k = 0; k < 15; k++) {
                sum += s->otherStars[i][k];
                sum += s->firstStar[i][k];
            }
            sum += s->level1dStars[i];
        } else {
            sum = -1;
        }
        settings->starCounts[i] = sum;
    }
}
