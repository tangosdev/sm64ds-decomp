#include "Ov007StrokePath.h"

extern void func_ov007_020c6d20(char *self, int idx);
extern void func_ov007_020c28ac(void *p);

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c5a6c, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[8];
    int groupCount; /* 0x08 */
    int maxGroups;  /* 0x0c: a0; this function stops committing at it */
    char pad_10[0x10];
    Ov007StrokePath *path; /* 0x20: the stroke being drawn (func_ov007_020c2c68) */
} Ov007Picker;

typedef char Ov007Picker_size_must_be_0x24[sizeof(Ov007Picker) == 0x24 ? 1 : -1];

/* Commits the stroke being drawn as the next group (func_ov007_020c6d20) and
 * clears the path. Returns 0 when there is nothing to commit or no free
 * group, 2 when this commit used the last group, else 1. The parameter keeps
 * the callers' char * spelling; `self` is the typed view. */
int func_ov007_020c6550(char *c)
{
    Ov007Picker *self = (Ov007Picker *)c;
    int status = 1;
    Ov007StrokePath *obj = self->path;
    int left;
    if (obj->pointCount == 0 || (left = *(volatile int *)&self->groupCount) >= self->maxGroups) {
        status = 0;
    } else {
        func_ov007_020c6d20(c, left);
        {
            int *p = &self->groupCount;
            *p = *p + 1;
        }
        if (self->groupCount >= self->maxGroups)
            status = 2;
        func_ov007_020c28ac(obj);
    }
    return status;
}
