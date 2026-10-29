extern int func_ov007_020c3ba8(int r4);
#pragma opt_propagation off

typedef struct {
    int x, y, z;
} Vec3;

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c684c, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[8];
    int groupCount; /* 0x08 */
    char pad_0c[0x20];
    int **groupPointCopies; /* 0x2c */
    char pad_30[4];
    int **groupLinks; /* 0x34: per group, one link per point; +0xc is its dir */
    char pad_38[4];
    int **pointLists; /* 0x3c: per group; +4 is the point count */
    char pad_40[0x3c];
    Vec3 restPos; /* 0x7c */
} Ov007Picker;

/* Offsets the link dirs where func_ov007_020c3ba8(1000) < r2 * 10: sb 0
 * sets them to the offset, 1 adds it, 2 adds it to the rest offset
 * from restPos. */
void func_ov007_020c56bc(char* sl, int sb, int r2, int r3, int p4, int fp)
{
    Ov007Picker *self = (Ov007Picker *)sl;
    int i, j;
    int lim, neg3, d1, negp4, d2, jinit, base3e8;
    int* r6;
    int r5;
    int r4;

    i = 0;
    if (self->groupCount <= 0)
        return;

    r4 = r2 * 10;
    neg3 = -r3;
    negp4 = -p4;
    lim = r4 - 1;
    jinit = 0;
    d1 = r3 - neg3;
    d2 = p4 - negp4;
    base3e8 = 0x3e8;

    do {
        j = jinit;
        if (self->pointLists[i][1] > 0) {
            r4 = j;
            do {
                int r0v;

                if (func_ov007_020c3ba8(base3e8) <= lim) {
                    r6 = (int *)(self->groupLinks[i][j]) + 3;
                    r5 = fp * (neg3 + func_ov007_020c3ba8(d1));
                    r0v = fp * (negp4 + func_ov007_020c3ba8(d2));

                    switch (sb) {
                    case 0:
                        r6[0] = r5;
                        r6[1] = r0v;
                        break;
                    case 1:
                        {
                            int* q;
                            r6[0] = r6[0] + r5;
                            q = (int*)(int)((char*)r6 + 4);
                            *q = *q + r0v;
                        }
                        break;
                    case 2:
                        r6[0] = r5 + (self->restPos.x -
                                      *(int *)((char *)(self->groupPointCopies[i]) + r4));
                        r6[1] = r0v + (self->restPos.y -
                                       *(int *)((char *)(self->groupPointCopies[i]) + r4 + 4));
                        break;
                    }
                }

                r4 += 0xc;
                j++;
            } while (j < self->pointLists[i][1]);
        }
        i++;
    } while (i < self->groupCount);
}
