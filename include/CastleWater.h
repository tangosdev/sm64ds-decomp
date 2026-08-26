/* Hand-written from matched-function evidence:
 * class CastleWater, ov009 0x02111a70-0x02111dc4 (8 functions, no other class
 * in the TU -- tu_map.py).
 *
 * The ROM knows this class by TWO names. Its symbols mangle from `CastleWater`
 * (_ZN11CastleWater8BehaviorEv and friends), which is why the struct is spelled
 * that way here -- the mangling has to come out right. Its RTTI record calls it
 * `daObjMcWater_c`, EAD's own name, and include/daObjMcWater_c.h is the
 * generated view under that name. The two describe the same object; this header
 * is the one methods can be defined against.
 *
 * It is a dBgActor_c: InitResources calls dBgActor_c::UpdateModelPosAndRotY and
 * dBgActor_c::UpdateClsnPosAndRot on itself, and mPos/mAngleY sit at dActor_c's
 * offsets. Its destructor independently proves the base's Model and moving-mesh
 * members at 0x0d4 and 0x124.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef CASTLEWATER_H
#define CASTLEWATER_H
#include "dBgActor_c.h"
#include "TextureTransformer.h"

struct CastleWater : dBgActor_c {
    /* dBgActor_c ends at 0x31e; alignment places the member at 0x320. */
    u8 pad_31e[0x2];
    TextureTransformer mTexTransformer; /* 0x320 */
    /* CastleWater_Spawn's allocation literal proves the trailing extent. */
    u8 pad_334[0x4];

    /* Slots 16/17. The compiler-facing name is CastleWater; the ROM RTTI
       record and compatible vtable alias use EAD's daObjMcWater_c spelling. */
    virtual ~CastleWater();

    virtual int InitResources();      /* slot 0 */
    virtual int CleanupResources();   /* slot 3 */
    virtual int Behavior();           /* slot 6 */
    virtual int Render();             /* slot 9 */
};

typedef char CastleWater_size_must_be_0x338[
    sizeof(CastleWater) == 0x338 ? 1 : -1];

#endif
