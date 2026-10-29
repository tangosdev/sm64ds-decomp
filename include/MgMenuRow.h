#ifndef MGMENUROW_H
#define MGMENUROW_H
#include "types.h"

/* The five saved records a minigame starts with, words 5..9 of a menu row.
   The ROM keeps this part of the selected row as its own symbol,
   gMgMenuRowMid (gMgMenuRow+0x14), and dScMiniGm_c copies it in with one
   struct copy. */
struct MgRecordDefaults {
    s32 v[5];
};

/* One 0x34-byte minigame menu row: an entry of dScMiniGm_c's sMiniGmRows
   table, and gMgMenuRow, the row of the game being played, which
   dScMiniGm_c::UpdateGameSelect copies in before it fades to the game's
   scene. Values below are from the 36 ROM rows. */
struct MgEntry {
    /* 0x00: scene ID; Mg_InitMusic tests the Sound room's 0x16f */
    u16 id;
    u16 pad;
    /* 0x04: the scene's fBase_c param1 */
    s32 sceneParam;
    /* 0x08: Mg_GetScoringMode. 5 is lower-is-better (a time in frames);
       dScMiniGm_c labels the records of modes 1, 2 and 4 */
    s32 scoringMode;
    /* 0x0c: Mg_SubmitScore caps a score here when nonzero;
       dScMiniGm_c::InitResources resets records above it */
    s32 scoreCap;
    /* 0x10: 1 for every mode-2 row and for row 7, games that save their own
       record pair (Mg_SaveRecordPair): AfterCleanupResources then skips the
       top-five commit, and InitResources checks only record 0 against
       scoreCap */
    s32 ownRecords;
    /* 0x14 */
    MgRecordDefaults recordDefaults;
    /* 0x28: dScMgBase_c::BeforeInitResources loads it into mMusicTrack */
    s32 musicTrack;
    /* 0x2c: Mg_GetMusicMode; 0 stops the music */
    s32 musicMode;
    /* 0x30: dScMiniGm_c::mStartLevel; the dScMgBase_c constructor loads it
       into mLevel */
    s32 level;
};
typedef char MgEntry_size_must_be_0x34[sizeof(MgEntry) == 0x34 ? 1 : -1];

#endif
