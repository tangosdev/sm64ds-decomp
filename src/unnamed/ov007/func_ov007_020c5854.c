#include "types.h"

extern int func_ov007_020c704c(int elem, void *sb, void *fp, void *r7, void *r8);

/* A group's point list; names as in func_ov007_020c6e68 and
 * func_ov007_020c5ef0 (items, count). */
typedef struct Ov007PointList {
    int *items; /* 0x0: the group's points */
    int count;  /* 0x4 */
} Ov007PointList;

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[8];
    int groupCount; /* 0x08: committed strokes */
    char pad_0c[0x30];
    Ov007PointList **pointLists; /* 0x3c: func_ov007_020c798c, per group */
    char pad_40[0x30];
    Vector3 pos; /* 0x70: posPoint's position */
} Ov007Picker;

typedef char Ov007Picker_pos_must_be_at_0x70[(int)&((Ov007Picker *)0)->pos == 0x70 ? 1 : -1];

inline long long mul_add(long long a, int b, long long c) {
  return a * b + c;
}

/* Eases the picker's pos x/y towards sb by scale (Q12) and sets its z, then
 * runs func_ov007_020c704c over every point of every group; returns how many
 * of them it answered 2 for. sl keeps the char * spelling of the definition;
 * `self` is the typed view. */
int func_ov007_020c5854(char *sl, int *sb, void *fp, void *r8, void *r7, int scale)
{
    Ov007Picker *self = (Ov007Picker *)sl;
    long long v;
    int i, j, cnt = 0;
    v = mul_add(scale, sb[0], ((long long)(0x1000 - scale)) * self->pos.x);
    self->pos.x = (int)(v >> 12);
    v = mul_add(scale, sb[1], ((long long)(0x1000 - scale)) * self->pos.y);
    self->pos.y = (int)(v >> 12);

    self->pos.z = sb[2];
    for (i = 0; i < self->groupCount; i++) {
        j = 0;
        if (self->pointLists[i]->count > 0) {
            do {
                if (func_ov007_020c704c(self->pointLists[i]->items[j], sb, fp, r7, r8) == 2)
                    cnt++;
                j++;
            } while (j < self->pointLists[i]->count);
        }
    }
  return cnt;
}
