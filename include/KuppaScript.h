#ifndef SM64DS_KUPPASCRIPT_H
#define SM64DS_KUPPASCRIPT_H

#include "types.h"

struct dActor_c;

/* Script records carry their arguments unaligned, so they are read a byte at
   a time. Used by the interpreter and by every handler of its command tables
   (the camera's, daDemo_c's and the player's). */
s32 ReadUnalignedSignedInt(u8 *p);
u32 ReadUnalignedInt(u8 *p);
s16 ReadUnalignedShort(u8 *p);
u16 ReadUnalignedUshort(u8 *p);

/* The cutscene script interpreter (src/KuppaScript.cpp). */
namespace KuppaScript {
int Run(u8 *script);
void Process();
void End();
void Reset();
int ContinueIfNecessary();

void StartDanceCutscene(s32 setting);
void StartEndingCutscene();
void StartIntroCutscene();
int IsDanceRotateCutsceneRunning();

dActor_c *FindDemoActor(u32 param1);
dActor_c *FindScriptPlayer(u32 character);
dActor_c *SpawnDemoActor(u32 param1);
dActor_c *SpawnScriptPlayer(u32 character);
int RunDemoCommand(u8 *command, int first, int last);
int RunPlayerCommand(u8 *command, int first, int last);
} // namespace KuppaScript

#endif
