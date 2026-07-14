typedef unsigned char u8;
typedef int s32;
typedef long long s64;

typedef struct Vector3 { int x, y, z; } Vector3;

typedef struct Clipper {
    void* vtable;       /* 0x00 */
    Vector3 planes[4];  /* 0x04 */
    int unk34[7];       /* 0x34 */
    int minZ;           /* 0x50 */
    int maxZ;           /* 0x54 */
} Clipper;

#pragma opt_propagation off
int _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(Clipper* thiz, Vector3* v, int clip, u8* hint)
{
    if (hint != 0)
    {
        unsigned int first;
        s64 y;
        s64 x;
        s64 z;
        s64 c;
        int negZ;
        int one;
        int zero;
        s64 dot;
        int outside;
        unsigned int i;
        int ok;
        s64 d;

        negZ = -v->z;
        if (negZ < thiz->minZ - clip) goto fail1;
        if (negZ > thiz->maxZ + clip) goto fail1;

        first = *hint & 3;
        x = v->x;
        c = clip;
        dot = ((s64)v->x * thiz->planes[first].x + (s64)v->y * thiz->planes[first].y + (s64)v->z * thiz->planes[first].z + 0x800) >> 12;
        outside = 0;
        if (c < dot) outside = 1;
        if (outside == 0)
        {
            i = (first + 1) & 3;
            y = (s32)*(unsigned int*)&v->y;
            z = (s32)*(unsigned int*)&v->z;
            zero = 0;
            one = 1;
            do {
                d = (z * thiz->planes[i].z + (x * thiz->planes[i].x + y * thiz->planes[i].y) + 0x800) >> 12;
                ok = d <= c ? one : zero;
                if (ok == 0) goto failstore;
                i = (i + 1) & 3;
            } while (i != first);
            return negZ;
        failstore:
            *hint = (u8)i;
        }
    fail1:
        return 0x7FFFFFFF;
    }
    else
    {
        int negZ;

        negZ = -v->z;
        if (negZ < thiz->minZ - clip) goto fail2;
        if (negZ > thiz->maxZ + clip) goto fail2;

        if ((s32)(((s64)v->z * thiz->planes[0].z + 0x800) >> 12)
            + ((s32)(((s64)v->x * thiz->planes[0].x + 0x800) >> 12)
             + (s32)(((s64)v->y * thiz->planes[0].y + 0x800) >> 12)) > clip)
            goto fail2;

        {
            s64 x = v->x, y = v->y, z = v->z;
            if ((s32)((z * thiz->planes[1].z + 0x800) >> 12)
                + ((s32)((x * thiz->planes[1].x + 0x800) >> 12)
                 + (s32)((y * thiz->planes[1].y + 0x800) >> 12)) > clip)
                goto fail2;
            if ((s32)((z * thiz->planes[2].z + 0x800) >> 12)
                + ((s32)((x * thiz->planes[2].x + 0x800) >> 12)
                 + (s32)((y * thiz->planes[2].y + 0x800) >> 12)) > clip)
                goto fail2;
            if ((s32)((z * thiz->planes[3].z + 0x800) >> 12)
                + ((s32)((x * thiz->planes[3].x + 0x800) >> 12)
                 + (s32)((y * thiz->planes[3].y + 0x800) >> 12)) <= clip)
                return negZ;
        }
    fail2:
        return 0x7FFFFFFF;
    }
}
