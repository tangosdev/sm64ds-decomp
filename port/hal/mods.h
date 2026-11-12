/* hal/mods.h: the mod list (run mods1, lane MODFILES1).

   settings.json's "EnabledMods" is the ordered list of installed mods that are
   on. mods.cpp reads it ONCE, at boot (the first call to any function below
   latches it; hdtex_configure makes that first call during start-up), checks
   each id and mod.json, and keeps the ones that passed. This header is the
   whole read API for the pieces that act on a mod: the file layer
   (hal/mods.cpp itself), the HD texture index (ntr/hdtex.cpp, through a hook
   because the ntr libraries link no hal code), the palette lookup
   (hal/fs_mods.cpp) and wave 2's plugin and sound lanes.

   ORDER: index 0 is the FIRST id in EnabledMods and index count-1 the LAST.
   A later mod wins over an earlier one, so walk from count-1 down to 0 to ask
   "who answers first". Code mods (kind "code") are in the list too: a code mod
   may also carry files/, textures_hd/ and palettes/ and gets them applied the
   same as a files mod. Whether its plugin is loaded (api and game checks) is
   the plugin lane's call.

   SM64DS_MODS=0 in the environment empties the list (a bug report switch).

   Only the two game targets link hal/mods.cpp, so these functions do not exist
   in the narrow smoke targets: every caller is either in a game-only file or
   goes through a hook that stays null there. */
#ifndef PORT_HAL_MODS_H
#define PORT_HAL_MODS_H

#ifdef __cplusplus
extern "C" {
#endif

/* How many mods are on and passed validation. */
int port_mods_count(void);
/* The i-th enabled mod (0 = lowest priority). Null when i is out of range. */
const char *port_mods_id(int i);
/* The mod's folder, no trailing slash: <bundle>/mods/<id>. */
const char *port_mods_folder(int i);
/* "files" or "code", straight from mod.json. */
const char *port_mods_kind(int i);
/* mod.json's "game" and "api" fields as the launcher wrote them ("" / 0 when
   absent); only code mods need them. */
const char *port_mods_game(int i);
int port_mods_api(int i);
/* mod.json's optional "minGameVersion": the oldest game release the plugin works on
   ("" when absent). Code mods only. */
const char *port_mods_min_game(int i);
/* 1 when this run is multiplayer: the launcher's lobby / VS match environment
   (any of SM64DS_COMMS_ROLE/RELAY/HOST/CODE/SLOT, SM64DS_VS_MAP/MODE/PLAYERS
   non-empty, SM64DS_PARTY nonzero) or the Adventure hub (settings.json
   AdventureGhosts, SM64DS_ADVENTURE). In that case the list keeps, for the whole
   run, only the mods whose mod.json says "onlineCompatible": true AND that are
   cosmetic by content (kind "files", every folder one of textures_hd/, hd_ui/,
   sound/, palettes/, no "character" block); every other mod is off and no code
   mod is ever in the list. */
int port_mods_multiplayer(void);
/* The bundle folder mods/ sits in (the folder settings.json was found in). */
const char *port_mods_bundle(void);
/* HD UI art by name: the full path of <mod>/hd_ui/<rel> from the last enabled mod that has it (returns 1), or 0 when none does (draw the port's
   own art). <rel> is a plain relative path with no "..". Documented at the definition in mods.cpp. */
int port_mods_ui_file(const char *rel, char *out, unsigned cap);

/* The enabled mods' stage/ folders (run studio1, lane PORTLVLM1): the i-th entry, latest mod first and by name inside one
   mod. Fills path with the full path of a <name>.lvlm level-edits file (returns 1, for hal/stage_mods.cpp) or a <name>_geom
   folder of <stage>.bmd / <stage>.kcl (returns 2, for hal/stage_geom.cpp) and *mod with the mod's id; 0 past the end. Taken
   once, from the list as the game started: the relatch at the title does not change it. Documented at the definition. */
int port_mods_stage_entry(int i, char *path, unsigned cap, const char **mod);

/* SEAMCODE1: why a code mod in the list stops an in-place switch of the mod list (a loaded plugin that cannot be switched off and
   on in place, hal/mod_plugins.cpp), or 0 when none does. Every in-place start and leave asks this instead of refusing any code mod. */
const char *port_mods_code_refusal(void);

/* Switch the mod list IN PLACE at the title, with no level loaded (run after093, lane SEAMMODS1; one-process plan M3a): reads
   EnabledMods again (and the multiplayer rule as it stands), rebuilds the files/ layer the card serves, re-indexes textures_hd and
   drops the texture caches. Refuses, changing nothing, when a code mod on either side cannot be switched in place, when the set of mods shipping hd_ui/,
   sound/ or a character changes, or when files/ would change a file the game read before or at the title; returns 1 switched,
   0 refused (the caller then restarts the way it does today). Logs one line either way. Game thread only. */
int port_mods_relatch_at_title(const char *reason);

#ifdef __cplusplus
}
#endif
#endif
