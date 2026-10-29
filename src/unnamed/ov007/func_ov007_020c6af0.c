extern void MultiCopy_Int(int *src, int *dst, int len);

struct M3 {
    int w[3];
};

/* A committed stroke; names as in include/Ov007StrokePath.h, only the count. */
struct Ov007StrokePath {
    char pad_00[8];
    unsigned short pointCount; /* 0x08 */
};

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c684c, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad8[8];
    int groupCount; /* 0x8 */
    char pad[0x1c];
    int **groupPoints;      /* 0x28 */
    int **groupPointCopies; /* 0x2c */
    char pad2[8];
    struct Ov007StrokePath **groups; /* 0x38 */
    char pad3[0x34];
    int pos[3];     /* 0x70 */
    int restPos[3]; /* 0x7c */
} Ov007Picker;

/* Copies each group's live points into groupPointCopies (MultiCopy_Int is
 * (src, dst)) and snapshots pos as restPos. */
void func_ov007_020c6af0(Ov007Picker *c) {
    int i;
    for (i = 0; i < c->groupCount; i++) {
        MultiCopy_Int(c->groupPoints[i], c->groupPointCopies[i], c->groups[i]->pointCount * 0xc);
    }
    *(struct M3 *)c->restPos = *(struct M3 *)c->pos;
}
