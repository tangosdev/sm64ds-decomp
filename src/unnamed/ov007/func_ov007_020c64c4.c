// @symbol func_ov007_020c64c4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int LenVec3(struct Vector3 *a);

/* A point and a spring between two points, as func_ov007_020c5a6c spells them. */
typedef struct Ov007Point {
    struct Vector3 *pos; /* 0x0 */
} Ov007Point;

typedef struct Ov007Link {
    Ov007Point *from;   /* 0x0 */
    Ov007Point *to;     /* 0x4 */
    int length;         /* 0x8 */
    struct Vector3 dir; /* 0xc */
} Ov007Link;

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c684c, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[4];
    int mode;       /* 0x04: 2 = link mode */
    int groupCount; /* 0x08 */
    char pad_0c[0x64];
    struct Vector3 pos; /* 0x70 */
    char pad_7c[0x1c];
    struct Vector3 anchorPos; /* 0x98: the anchor Point's position */
    Ov007Link *link;          /* 0xa4: from the anchor Point to posPoint */
} Ov007Picker;

/* Re-measures every group's rest links (func_ov007_020c6314 with `b`); in
 * link mode also drops the anchor onto pos and re-measures `link`. The
 * parameter keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is a local. */
void func_ov007_020c64c4(char* c, int b) {
    Ov007Picker *self = (Ov007Picker *)c;
    Ov007Link *link;
    int i;
    for (i = 0; i < self->groupCount; i++) {
        func_ov007_020c6314(c, i, b);
    }
    if (self->mode != 2)
        return;
    self->anchorPos = self->pos;
    link = self->link;
    SubVec3(link->from->pos, link->to->pos, &link->dir);
    link->length = LenVec3(&link->dir);
}
