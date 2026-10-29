//cpp
extern "C" void func_ov007_020bfe4c(void *o, int x, int y, int z, void *out);

/* Array form on purpose: a three-int-member struct copies field by field,
   while the ROM copies these with one ldm/stm. */
struct Vec3 {
    int w[3];
};

/* A committed stroke; names as in include/Ov007StrokePath.h, only the slots
   this function reads. */
struct Ov007StrokePath {
    char pad[8];
    unsigned short pointCount; /* 0x8 */
    char pad2[0x24 - 0xa];
    int *xCoords; /* 0x24 */
    int *yCoords; /* 0x28 */
};

/* The overlay's camera (func_ov007_020c023c), as func_ov007_020c6e68 spells it. */
struct Ov007Camera {
    char pad[0x2c];
    int distance; /* 0x2c: func_ov007_020bffb8 places the eye this far from the target */
};

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
   file and func_ov007_020c684c, only the slots this function touches. */
struct Ov007Picker {
    char pad0[8];
    int groupCount; /* 0x8 */
    char pad1[0x28 - 0xc];
    Vec3 **groupPoints;      /* 0x28 */
    Vec3 **groupPointCopies; /* 0x2c */
    char pad2[0x38 - 0x30];
    Ov007StrokePath **groups; /* 0x38 */
    char pad3[0x58 - 0x3c];
    int hitGroup;   /* 0x58: picked group, -1 = none (see func_ov007_020c5a6c) */
    int hitIndex;   /* 0x5c: picked point in that group, -1 = none */
    Vec3 hitOffset; /* 0x60 */
    char pad4[0x70 - 0x6c];
    Vec3 pos;     /* 0x70 */
    Vec3 restPos; /* 0x7c */
    char pad5[0xa8 - 0x88];
    Ov007Camera *camera; /* 0xa8 */
};

/* Re-unprojects every committed stroke point through the camera, resets the
   live points to it, and puts pos back at its rest position. */
extern "C" void func_ov007_020c5dec(Ov007Picker *c) {
    int i;
    for (i = 0; i < c->groupCount; i++) {
        int j;
        for (j = 0; j < c->groups[i]->pointCount; j++) {
            Ov007Camera *o = c->camera;
            Ov007StrokePath *group = c->groups[i];
            func_ov007_020bfe4c(o, group->xCoords[j], group->yCoords[j], -o->distance,
                                &c->groupPointCopies[i][j]);
            c->groupPoints[i][j] = c->groupPointCopies[i][j];
        }
    }
    c->pos = c->restPos;
    c->hitGroup = c->hitIndex = -1;
    c->hitOffset.w[0] = c->hitOffset.w[1] = c->hitOffset.w[2] = 0;
}
