#ifndef SLIDEDECORATIONSILVERSTAR_H
#define SLIDEDECORATIONSILVERSTAR_H

#include "types.h"
#include "Model.h"

#ifdef __cplusplus

#include "dActor_c.h"

/* The four slide-decoration actor IDs share one implementation and vtable;
 * this name is the one preserved by the matching symbols. The ROM's D1/D0
 * destructors prove both the dActor_c base and the owned Model at +0xd4. */
struct SlideDecorationSilverStar : dActor_c {
    u8 pad_0d0[0x4];        /* 0x0d0 */
    Model mModel;            /* 0x0d4 */
    u8 mVariant;             /* 0x124 */

    virtual ~SlideDecorationSilverStar();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Render();
};

#else

/* Flat layout for the remaining C translation units. */
struct SlideDecorationSilverStar {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0xc6];
    Model mModel;            /* 0x0d4 */
    u8  mVariant;            /* 0x124 */
};

#endif

typedef char SlideDecorationSilverStar_size_must_be_0x128[
    sizeof(struct SlideDecorationSilverStar) == 0x128 ? 1 : -1];

#endif /* SLIDEDECORATIONSILVERSTAR_H */
