//cpp
// @symbol _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh
/* recovered: named members + shared header, real C++ method */
#include "Clipper.h"

#pragma opt_propagation off

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in Clipper.h is the real one and callers may use it. */
extern "C" int _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(Clipper *thiz, Vector3 *v, int clip, u8 *hint)
{
    if (hint != 0)
    {
        unsigned int first;
        long long y;
        long long x;
        long long z;
        long long c;
        int negZ;
        int one;
        int zero;
        long long dot;
        int outside;
        unsigned int i;
        int ok;
        long long d;

        negZ = -v->z;
        if (negZ < thiz->mNearZ - clip) goto fail1;
        if (negZ > thiz->mFarZ + clip) goto fail1;

        first = *hint & 3;
        x = v->x;
        c = clip;
        dot = ((long long)v->x * thiz->mPlaneNormals[first].x + (long long)v->y * thiz->mPlaneNormals[first].y + (long long)v->z * thiz->mPlaneNormals[first].z + 0x800) >> 12;
        outside = 0;
        if (c < dot) outside = 1;
        if (outside == 0)
        {
            i = (first + 1) & 3;
            y = (int)*(unsigned int *)&v->y;
            z = (int)*(unsigned int *)&v->z;
            zero = 0;
            one = 1;
            do {
                d = (z * thiz->mPlaneNormals[i].z + (x * thiz->mPlaneNormals[i].x + y * thiz->mPlaneNormals[i].y) + 0x800) >> 12;
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
        if (negZ < thiz->mNearZ - clip) goto fail2;
        if (negZ > thiz->mFarZ + clip) goto fail2;

        if ((int)(((long long)v->z * thiz->mPlaneNormals[0].z + 0x800) >> 12)
            + ((int)(((long long)v->x * thiz->mPlaneNormals[0].x + 0x800) >> 12)
             + (int)(((long long)v->y * thiz->mPlaneNormals[0].y + 0x800) >> 12)) > clip)
            goto fail2;

        {
            long long x = v->x, y = v->y, z = v->z;
            if ((int)((z * thiz->mPlaneNormals[1].z + 0x800) >> 12)
                + ((int)((x * thiz->mPlaneNormals[1].x + 0x800) >> 12)
                 + (int)((y * thiz->mPlaneNormals[1].y + 0x800) >> 12)) > clip)
                goto fail2;
            if ((int)((z * thiz->mPlaneNormals[2].z + 0x800) >> 12)
                + ((int)((x * thiz->mPlaneNormals[2].x + 0x800) >> 12)
                 + (int)((y * thiz->mPlaneNormals[2].y + 0x800) >> 12)) > clip)
                goto fail2;
            if ((int)((z * thiz->mPlaneNormals[3].z + 0x800) >> 12)
                + ((int)((x * thiz->mPlaneNormals[3].x + 0x800) >> 12)
                 + (int)((y * thiz->mPlaneNormals[3].y + 0x800) >> 12)) <= clip)
                return negZ;
        }
    fail2:
        return 0x7FFFFFFF;
    }
}
