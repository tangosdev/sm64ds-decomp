typedef long long s64;

extern void func_ov007_020ca578(char* self, int i, int j, int t);

/* Reconstructed layouts, not recovered class identity; names as in
 * func_ov007_020c83fc and func_ov007_020ca308, only the slots this
 * file touches. */
typedef struct Ov007Follower {
    char pad_00[0x10];
    int force[3]; /* 0x10: accumulated per frame, cleared at the end of each step */
    char pad_1c[4];
    int flags; /* 0x20: bit 0 skips the step (target held) */
} Ov007Follower;

typedef char
    Ov007Follower_flags_must_be_at_0x20[(int)&((Ov007Follower *)0)->flags == 0x20 ? 1 : -1];

typedef struct Ov007PointList {
    Ov007Follower **items; /* 0x0 */
} Ov007PointList;

typedef struct Ov007Grid {
    char pad_00[8];
    Ov007PointList **rowLists; /* 0x08: one per row, columns points each */
} Ov007Grid;

/* data_ov007_02104bd4[0], the 0x38-byte grid mesh func_ov007_020cbbb0
 * builds; 0x0c..0x23 is its template copy. */
typedef struct Ov007Mesh {
    char pad_00[4];
    Ov007Grid *grid; /* 0x04: func_ov007_020c73f0 */
    char pad_08[0x10];
    /* 0x18/0x1c: the z force this file adds to a poked point, at strength 0
     * and at full strength (0x1000); func_ov007_020bed60's pokes */
    int pushMin;
    int pushMax;
} Ov007Mesh;

typedef char Ov007Mesh_pushMax_must_be_at_0x1c[(int)&((Ov007Mesh *)0)->pushMax == 0x1c ? 1 : -1];

/* Pushes grid point (i, j) along z, lerping pushMin..pushMax by t (Q12),
 * unless the point is held. self keeps the callers' char * spelling; `mesh`
 * is the typed view. */
void func_ov007_020ca578(char* self, int i, int j, int t)
{
    Ov007Mesh *mesh = (Ov007Mesh *)self;
    Ov007Follower *lr;
    int lo, hi;
    int val;

    lr = mesh->grid->rowLists[j]->items[i];

    if (lr->flags & 1)
        return;

    hi = mesh->pushMax;
    lo = mesh->pushMin;

    if (t >= 0x1000) {
        val = hi;
    } else if (t <= 0) {
        val = lo;
    } else {
        val = (int)(((s64)(0x1000 - t) * lo + (s64)t * hi) >> 0xc);
    }

    lr->force[2] += val;
}
