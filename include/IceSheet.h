#ifndef ICESHEET_H
#define ICESHEET_H

#include "dBgActor_c.h"

/* The cartridge RTTI names this class daObjIceBoard_c. IceSheet is the
 * readable compatibility spelling already carried by every configured
 * method. The ROM's __si_class_type_info record gives it one direct base,
 * dBgActor_c at offset zero, and its 32-slot vtable has the same extent as
 * that base. A compiler object may therefore emit `_ZTI8IceSheet`; that
 * compatibility-name metadata is a discard-only passenger of the
 * independently isolated functions, not a claim about the cartridge RTTI.
 *
 * IceSheet adds no fields. The factory allocates exactly 0x320 bytes, and the
 * destructor destroys only dBgActor_c's inherited dBgW_KcMbg and Model
 * members before chaining to dActor_c. Those two independent observations
 * pin both the empty derived layout and the allocation size. */
struct IceSheet : dBgActor_c {
    /* Out of line is load-bearing in the repository's one-function object
     * shape. Each enrolled destructor source must ask mwccarm for the genuine
     * variant and retain the class's already-verified raw data contribution;
     * objisolate keeps only that source's licensed D1 or D0 text. */
    virtual ~IceSheet();

    virtual int InitResources();                  /* slot  0 */
    virtual int CleanupResources();               /* slot  3 */
    virtual int Behavior();                       /* slot  6 */
    virtual int Render();                         /* slot  9 */
    virtual void OnGroundPounded(dActor_c &other);/* slot 21 */
    virtual void OnHitByMegaChar(Player &player); /* slot 27 */
    virtual void Kill();                          /* slot 31 */
};

typedef char IceSheet_size_must_be_0x320[sizeof(IceSheet) == 0x320 ? 1 : -1];

#endif /* ICESHEET_H */
