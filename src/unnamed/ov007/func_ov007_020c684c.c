typedef struct {
    int x, y, z;
} Vec3;

/* The 0xd0-byte touch picker func_ov007_020c690c allocates and resets here.
 * Names follow func_ov007_020c5a6c, func_ov007_020c5dec and
 * func_ov007_020c690c; only the slots this file touches are spelled. The
 * word at +0x00 has no reader found yet and stays raw on `o`. */
typedef struct Ov007Picker {
    char pad_00[4];
    int mode; /* 0x04: func_ov007_020c5a6c switches on it (0 move, 1 drag, 2 link) */
    char pad_08[0x50];
    int hitGroup;   /* 0x58: picked group, -1 = none */
    int hitIndex;   /* 0x5c: picked point in that group, -1 = none */
    Vec3 hitOffset; /* 0x60 */
    char pad_6c[4];
    Vec3 pos; /* 0x70 */
    /* 0x7c: func_ov007_020c6af0 snapshots pos here when it restores the rest
     * points from groupPointCopies; func_ov007_020c5dec copies it back into
     * pos, func_ov007_020c6314 measures rest link lengths from it */
    Vec3 restPos;
    /* 0x88: func_ov007_020c5188 adds it to pos for one simulation step and
     * then restores pos; func_ov007_020baa10 sweeps it per menu state */
    Vec3 posOffset;
    char pad_94[4];
    /* 0x98: the position of the Point at +0x94 that anchors `link`'s from
     * end (func_ov007_020c690c); func_ov007_020c64c4 copies pos into it and
     * mode 2 of func_ov007_020c5a6c sets it to the hit point */
    Vec3 anchorPos;
    char pad_a4[8];
    int limitGroup; /* 0xac: func_ov007_020c5a6c, flagged picks stop before it */
    int limitIndex; /* 0xb0 */
    char pad_b4[4];
    /* 0xb8/0xc4: the screen rectangle func_ov007_020c510c unprojects; pos
     * (func_ov007_020c5948) and every point (func_ov007_020c5188) are clamped
     * to it on x and y */
    Vec3 boundsMin;
    Vec3 boundsMax;
} Ov007Picker;

typedef char Ov007Picker_size_must_be_0xd0[sizeof(Ov007Picker) == 0xd0 ? 1 : -1];

void func_ov007_020c684c(char *o)
{
    Ov007Picker *p = (Ov007Picker *)o;
    Vec3 tmp;
    Vec3 *tp;
    p->mode = 1;
    *(int *)(o) = 0;
    p->pos.z = 0;
    p->pos.y = p->pos.z;
    p->pos.x = p->pos.y;
    p->posOffset.z = 0;
    p->posOffset.y = p->posOffset.z;
    p->posOffset.x = p->posOffset.y;
    p->restPos = p->pos;
    p->anchorPos = p->pos;
    tp = (Vec3 *)(&tmp);
    tp->x = 0;
    tp->y = 0;
    tp->z = 0;
    p->boundsMax = *tp;
    p->boundsMin = p->boundsMax;
    p->hitIndex = -1;
    p->hitGroup = p->hitIndex;
    p->hitOffset.z = 0;
    p->hitOffset.y = p->hitOffset.z;
    p->hitOffset.x = p->hitOffset.y;
    p->limitGroup = 0;
    p->limitIndex = 0;
}
