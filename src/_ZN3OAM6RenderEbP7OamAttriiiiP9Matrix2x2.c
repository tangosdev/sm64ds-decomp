#include "types.h"
// @symbol _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_OAM.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "OAM.h"
typedef struct OamAttr {
    u32 yb : 8, objMode : 2, mode : 2, mosaic : 1, dep : 1, shape : 2, xc : 9, aff : 5, size : 2;
    u32 tile : 10, prio : 2, pal : 4, hi2 : 16;
} OamAttr;
#define ATTR01(o) (*(u32 *)(o))
typedef struct Matrix2x2 { s32 m00, m01, m10, m11; } Matrix2x2;

extern int _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(OamAttr *attr, int *count, Matrix2x2 *mtx);

extern u8 data_0209e660;
extern OamAttr data_0209e674[];
extern OamAttr data_0209ea74[];

void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
    int draw, OamAttr *obj, int px, int py, int pal, int prio, Matrix2x2 *mtx)
{
    OamAttr *res;
    int *counter;
    int *affctr;
    int cnt;
    int h;
    int idx;
    int w;
    int x;
    int y;

    if (!draw || data_0209e660 == 1) {
        counter = &data_0209e664;
        cnt = *counter;
        if (cnt >= 0x80) return;
        res = &data_0209e674[cnt];
        affctr = &data_0209e668;
    } else {
        counter = &data_0209e670;
        cnt = *counter;
        if (cnt >= 0x80) return;
        res = &data_0209ea74[cnt];
        affctr = &data_0209e66c;
    }

    x = obj->xc;
    if (x >= 0x100) x -= 0x200;
    y = *(s8 *)obj;
    x += px;
    y += py;
    w = _ZN3OAM11GetObjWidthEii((ATTR01(obj) << 16) >> 30, ATTR01(obj) >> 30);
    h = _ZN3OAM12GetObjHeightEii((ATTR01(obj) << 16) >> 30, ATTR01(obj) >> 30);

    if (mtx != 0) {
        if (obj->objMode != 1) {
            x -= w >> 1;
            y -= h >> 1;
            w <<= 1;
            h <<= 1;
        }
    }

    if (x + w < 0) return;
    if (x > 0x100) return;
    if (y + h < 0) return;
    if (y > 0xc0) return;

    if (pal == -1)
        pal = obj->pal;
    if (prio == -1)
        prio = obj->prio;

    {
    u32 flags;
    if (mtx != 0) {
        idx = _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(res - *counter, affctr, mtx);
        if (idx == -1) return;
        if (obj->objMode == 1)
            flags = 0x100;
        else
            flags = 0x300;
    } else {
        idx = 0;
        flags = ATTR01(obj) & 0x30000000;
    }

    {
        u32 mode, mosaic, mask, dep, tiles;
        tiles = obj->tile;
        dep = obj->dep;
        mask = ATTR01(obj) & 0xc000c000;
        mosaic = obj->mosaic;
        mode = obj->mode;

        if (flags == 0x100 || flags == 0x300) {
            if (mode == 3)
                ATTR01(res) = flags | (mask | ((y & 0xff) | (idx << 25) | (mode << 10) | (mosaic << 12)) | ((x & 0x1ff) << 16));
            else
                ATTR01(res) = flags | (mask | ((y & 0xff) | ((idx << 25) | (dep << 13)) | (mode << 10) | (mosaic << 12)) | ((x & 0x1ff) << 16));
        } else {
            if (mode == 3)
                ATTR01(res) = flags | (mask | ((y & 0xff) | (mode << 10) | (mosaic << 12)) | ((x & 0x1ff) << 16));
            else
                ATTR01(res) = flags | (mask | ((y & 0xff) | (dep << 13) | (mode << 10) | (mosaic << 12)) | ((x & 0x1ff) << 16));
        }
        *(u16 *)((u32 *)res + 1) = tiles | (prio << 10) | (pal << 12);
    }
    }
    *counter += 1;
}
