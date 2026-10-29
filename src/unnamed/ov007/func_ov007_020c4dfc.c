#include "Ov007StrokePath.h"

extern void func_ov007_020c4fcc(int r0, int r1);
extern void func_ov007_020c19cc(int a, int b, int c, int d);

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file, func_ov007_020c5ef0 and func_ov007_020c684c, only the slots this
 * function reads. */
typedef struct Ov007Picker {
    char pad_00[8];
    int groupCount; /* 0x08 */
    char pad_0c[0x1c];
    Vector3 **groupPoints; /* 0x28: func_ov007_020c6e68, per group */
    char pad_2c[0xc];
    Ov007StrokePath **groups; /* 0x38: committed strokes */
    char pad_3c[0x70];
    /* 0xac/0xb0: the draw limit; whole groups before limitGroup, then that
     * group's first limitIndex points */
    int limitGroup;
    int limitIndex;
    /* 0xb4: Q12 progress toward point limitIndex (func_ov007_020c5014) */
    s16 limitFraction;
} Ov007Picker;

typedef char
    Ov007Picker_limitFraction_must_be_at_0xb4[(int)&((Ov007Picker *)0)->limitFraction == 0xb4 ? 1
                                                                                              : -1];

/* Draws the committed strokes up to the draw limit through
 * func_ov007_020c19cc: each whole group before limitGroup at full progress,
 * then limitGroup itself cut at limitIndex points and limitFraction.
 * c keeps the char * spelling of the callers' declarations; `self` is the
 * typed view. The loop test reads limitGroup volatile: plain, the function
 * compiles 4 bytes short (either read alone being volatile matches). */
void func_ov007_020c4dfc(char *c, int r1, int r2, int r3)
{
    Ov007Picker *self = (Ov007Picker *)c;
    func_ov007_020c4fcc(r2, r3);
    int i = 0;
    while (i < *(volatile int *)&self->limitGroup) {
        Ov007StrokePath **a38 = self->groups;
        Vector3 **a28 = self->groupPoints;
        func_ov007_020c19cc(a38[i]->pointCount, (int)a28[i], r1, 0x1000);
        i++;
    }
    {
        int m = self->limitGroup;
        if (m < self->groupCount) {
            Vector3 **a28 = self->groupPoints;
            func_ov007_020c19cc(self->limitIndex, (int)a28[m], r1, self->limitFraction);
        }
    }
}
