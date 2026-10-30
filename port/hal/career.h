/* hal/career.h -- the pause screen's Career page data (run after093, lane CAREER1): per save file, the PLAYTIME (time in the game on that
   file) and the CLEAR TIME (the playtime when the file first beat the final Bowser, "--" until then).

   WHAT COUNTS AS PLAYTIME. A game tick of the Stage (a course or the castle) on a known save file, with the ROM's pause down
   (data_0209f2c4 == 0) and no level-clear menu up (data_0209f20c == 0). The title, the file select, the star select, the pause, the
   save prompts, the debug menu and the settings page (both hold the game: no tick) do not count; VS does not count. The ROM ticks 60
   times a second, so the time shown is ticks / 60 (frames the host drops are not played time, the same as the cartridge).

   THE CLEAR. The ROM ends the game when the player takes the Grand Star after the final Bowser: daObjKey_c's LAST_STAR path
   (src/game/actors/d_a_obj_key.cpp func_ov089_02131dcc) hands the Player to func_ov002_020c3dbc, which changes its state to EndingFly
   (data_ov002_0211058c). The first EndingFly_Init on a file records the clear time; a later one never moves it.

   WHERE IT LIVES. A side file beside the save, never inside the cartridge save bytes: career.json next to characters.json and lives.json
   (the KeepLives setting's file, hal/char_save.cpp), online.career.json beside the online save, <save>.career.json when SM64DS_SAVE_PATH
   names the save. A guest in someone else's world (a .coopsav) keeps nothing. Copy and erase on the file select carry and clear it: the
   ROM's own writes of a file record to the save chip are watched (ntr/backup.cpp write observer). EraseSaveFile writes the record with
   flags1 bit 0 clear (SaveData::SetDefaultValues); SaveFile always sets it (SaveData::SaveFile), so a clear bit is an erase. A file
   record written on the title's copy page (or the file options page it returns to) with the same bytes as another file is a copy from it. */
#ifndef HAL_CAREER_H
#define HAL_CAREER_H

#ifdef __cplusplus
extern "C" {
#endif

/* one logic frame of the level loop (tests/walk_window.cpp main); ticked = the game ran this frame */
void port_career_frame(int ticked);
/* the page's numbers for file slot 0..2: 1 when the slot has a record. play / clear in ticks (60 a second); has_clear 0 = no clear yet */
int  port_career_get(int slot, unsigned long long *play, int *has_clear, unsigned long long *clear);
/* the file this process is playing (0..2), -1 when none is known (a scripted level boot, VS, a guest world) */
int  port_career_slot_now(void);
/* "1:02:03" (hours unbounded), from ticks */
void port_career_format(unsigned long long ticks, char *out, int cap);

#ifdef __cplusplus
}
#endif
#endif
