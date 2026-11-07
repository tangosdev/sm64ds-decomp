#ifndef SM64DS_KUPPASCRIPT_H
#define SM64DS_KUPPASCRIPT_H

#include "types.h"

struct dActor_c;

/* Script records carry their arguments unaligned, so they are read a byte at
   a time. Used by the interpreter and by every handler of its command tables
   (the camera's, daDemo_c's and the player's). */
s32 ReadUnalignedSignedInt(u8 *);
u32 ReadUnalignedInt(u8 *);
s16 ReadUnalignedShort(u8 *);
u16 ReadUnalignedUshort(u8 *);

/* The cutscene script interpreter (src/KuppaScript.cpp). */
namespace KuppaScript {
int Run(u8 *);
void Process();
void End();
void Reset();
int ContinueIfNecessary();

void StartDanceCutscene(s32);
void StartEndingCutscene();
void StartIntroCutscene();
int IsDanceRotateCutsceneRunning();

dActor_c *FindDemoActor(u32);
dActor_c *FindScriptPlayer(u32);
dActor_c *SpawnDemoActor(u32);
dActor_c *SpawnScriptPlayer(u32);
int RunDemoCommand(u8 *, int, int);
int RunPlayerCommand(u8 *, int, int);
} // namespace KuppaScript

#endif
