#include "Ov007Display.h"
#include "Ov007MenuMem.h"

/* The per-record frees; the long-branch veneers (func_ov007_020c105c,
 * 020c05f8, 020c8b04, 020c3544, 020c338c, 020c0a9c) all tail-call the
 * menu's free hook, func_ov007_020c3d1c. */
extern void func_ov007_020c105c(void *p);
extern void func_ov007_020c05f8(void *p);
extern void Transition_Destroy(void *p);
extern void func_ov007_020c3544(void *p);
extern void func_ov007_020c338c(void *p);
extern void func_ov007_020c121c(void *p);
#define func_ov007_020c1180 _Z19func_ov007_020c1180P12Ov007OamNode
extern void func_ov007_020c1180(Ov007OamNode *node);
extern void func_ov007_020c0a9c(void *p);

/* Frees the display record and everything its tables hold. One untyped
 * `arr` walks all six tables: a typed local per table compiles to different
 * register choices in six words. */
void func_ov007_020b73b4(Ov007Display *d) {
    void **arr;
    int i;

    func_ov007_020c3d1c(d->oamMain);
    func_ov007_020c3d1c(d->oamSub);
    func_ov007_020c105c(d->oamListMain);
    func_ov007_020c105c(d->oamListSub);
    func_ov007_020c05f8(d->bgListMain);
    func_ov007_020c05f8(d->bgListSub);

    if (d->transitionCount > 0) {
        arr = (void **)d->transitions;
        for (i = 0; i < d->transitionCount; i++)
            if (arr[i])
                Transition_Destroy(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (d->characterCount > 0) {
        arr = (void **)d->characters;
        for (i = 0; i < d->characterCount; i++)
            if (arr[i])
                func_ov007_020c3544(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (d->paletteCount > 0) {
        arr = (void **)d->palettes;
        for (i = 0; i < d->paletteCount; i++)
            if (arr[i])
                func_ov007_020c338c(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (d->spriteBankCount > 0) {
        arr = (void **)d->spriteBanks;
        for (i = 0; i < d->spriteBankCount; i++)
            if (arr[i])
                func_ov007_020c121c(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (d->oamNodeCount > 0) {
        arr = (void **)d->oamNodes;
        for (i = 0; i < d->oamNodeCount; i++)
            if (arr[i])
                func_ov007_020c1180(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (d->bgLayerCount > 0) {
        arr = (void **)d->bgLayers;
        for (i = 0; i < d->bgLayerCount; i++)
            if (arr[i])
                func_ov007_020c0a9c(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    func_ov007_020c3d1c(d);
}
