/* sm64ds_mod.h -- the plugin API for SM64DS PC port code mods. API version 1.

   A code mod is a folder mods/<id>/ next to the game with a mod.json that says
   "kind": "code", "api": 1 (and, for the record, "game": "<release it was built
   on>"; optionally "minGameVersion": "<oldest release it works on>"), and ONE DLL in plugin/. The
   game loads the DLL at start-up, finds the exported function

       int __cdecl sm64ds_mod_main(const SM64DSModApi *api);

   and calls it once. Return 0 for OK; anything else switches the plugin off.
   Everything the plugin can do goes through the struct passed to that call.

   THE RULES (they are enforced, not suggestions)

   * THE API VERSION IS THE PIN. mod.json's "api" is the API version the
     plugin was built against. The game loads it when that number is between
     SM64DS_MOD_API_MIN_VERSION and SM64DS_MOD_API_VERSION of the running game,
     on any release; a plugin that names a newer API than the game offers (or
     one older than the game still supports) is not loaded, and the boot
     summary line says why. The API version covers this struct (the fields,
     their order and meaning), the callback signatures and when they run, and
     the rules in this comment. A change that only ADDS (a new field at the
     end, a new callback) raises SM64DS_MOD_API_VERSION; a change that moves
     or re-means something existing raises SM64DS_MOD_API_MIN_VERSION too, and
     plugins built for the old number stop loading.
   * OPTIONAL "minGameVersion". mod.json may name the oldest game release the plugin
     works on (for example when it relies on a game symbol that first appears
     there). The game refuses the plugin on an older release.
   * symbol() IS BY NAME, NOT BY ADDRESS. The game resolves a name against its
     own symbol table, so a plugin that uses it follows the build it runs on.
     The API version does NOT promise that a given name exists or keeps its
     signature in every release: check for NULL, and set "minGameVersion" when you
     need a name that is not in older releases.
   * 32-BIT, WINDOWS, __cdecl. The game is a 32-bit x86 program. Build the DLL
     as 32-bit (MSVC "x86" tools) with the default __cdecl convention. The
     interface below is plain C: no C++ types cross it.
   * ONE DLL per mod. More than one in plugin/ is refused.
   * NOT IN MULTIPLAYER. Plugins are never loaded in a lobby or VS match (a
     plugin on one side only would desync the match). The log says so.
     A game that is already running with your plugin and then starts a coop
     room, a VS match or the online world at its title SWITCHES THE PLUGIN
     OFF in place instead of restarting, and on again when the player is
     back to solo: the game stops calling your callbacks, file filters and
     actor factories, and puts back the bytes your sm64ds_mod_main and
     on_level_load callbacks changed in the game's code and read-only data
     (it compares them before and after each of those calls). So make every
     code patch from sm64ds_mod_main or on_level_load. Patches made from
     on_frame or an actor factory, and writes to the game's writable data,
     are not undone. A plugin that registers on_file cannot be switched; the
     game then restarts for the room as before.
   * NO STATE ACROSS SAVE STATES. The game's F8/F9 save states (and rollback,
     which only exists in multiplayer, where no plugin runs) snapshot and
     restore the game's memory. Memory your plugin owns (its globals, anything
     from api->alloc) is NOT part of that. After a restore the game goes back
     in time and your plugin does not: keep your state derivable from the game
     (re-read it every frame) rather than remembering it.
   * A PLUGIN THAT FAULTS IS SWITCHED OFF. Every call into a plugin is guarded.
     If one crashes (access violation, stack overflow and the like), it is
     disabled for the rest of the run, one line is logged, and the game carries
     on. A file filter that faults has the file put back as it was before the
     call. A plugin that corrupts the game's own memory before crashing cannot
     be cleaned up after.
   * THE GUARD CANNOT STOP A PLUGIN THAT DOES NOT FAULT. A plugin that blocks
     (never returns from a callback, loops forever, waits on a lock), calls
     ExitProcess or abort, or deadlocks inside DllMain freezes or ends the whole
     game, and nothing in the game can stop it. The same goes for a thread it
     starts. Keep every callback short, and do not start threads.
   * Callbacks run on the game's main thread. Do not block, and do not call the
     API from another thread.

   GROWING THE STRUCT. `size` is first and holds sizeof(SM64DSModApi) as the
   GAME was built. A newer plugin on an older game must check
   SM64DS_MOD_API_HAS(api, field) before using a field added after version 1.
   Within API version 1 the existing fields never move or change meaning.

   ORDER. Mods are loaded in the launcher's priority order (the last one wins
   when two touch the same thing). The same order applies to callbacks: for
   every frame, level load and file, lower priority plugins run first, so a
   later plugin sees what an earlier one did. */
#ifndef SM64DS_MOD_H
#define SM64DS_MOD_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The newest API this header describes, and the oldest the game still loads. */
#define SM64DS_MOD_API_VERSION 1
#define SM64DS_MOD_API_MIN_VERSION 1

typedef struct SM64DSModApi SM64DSModApi;

/* called once per game frame, after the game's own frame logic for that frame
   (the actors' update and draw submission) and before the picture is shown */
typedef void (__cdecl *SM64DSFrameFn)(void *user);

/* called when a level has finished loading: the level's objects exist and the
   first frame has not run. level_id is the game's own level number (0 = the
   test level, 1 = castle grounds, and so on). Minigame and menu scenes are not
   reported in API version 1. */
typedef void (__cdecl *SM64DSLevelFn)(int level_id, void *user);

/* edit a ROM file's bytes as the game loads them. *data points at the
   DECOMPRESSED file (size bytes). Change the bytes in place and return size
   unchanged, or return a different size with *data pointing at a new buffer
   from api->alloc (the game then frees the old buffer; do not free it
   yourself, and never put a buffer from anything but api->alloc in *data).
   The game checks: a new buffer must be a live api->alloc block holding at
   least the returned size, otherwise the file is left as it was and the plugin
   is switched off. If the filter faults, the file's bytes are restored too.
   Return size and leave *data alone to change nothing. */
typedef uint32_t (__cdecl *SM64DSFileFn)(const char *rom_path, uint8_t **data,
                                         uint32_t size, void *user);

/* an actor's spawn function: allocates, constructs and returns the actor (the
   same shape as the game's own factories). Return NULL to refuse the spawn. */
typedef void *(__cdecl *SM64DSActorFactory)(void);

struct SM64DSModApi {
    uint32_t size;                 /* sizeof(SM64DSModApi) in the game */
    uint32_t api_version;          /* SM64DS_MOD_API_VERSION */
    const char *game_release;      /* the running game's release, e.g. "0.5.6" */
    const char *mod_id;            /* this mod's id (its folder name) */
    const char *mod_folder;        /* mods/<id>, no trailing slash */
    void *game_private;            /* the game's; never touch */

    /* Write a line to the game log, prefixed "[mod:<id>] ". printf format. */
    void (__cdecl *log)(const SM64DSModApi *api, const char *fmt, ...);

    /* The player's value for `key` from mods/<id>/settings.values.json, as
       text (numbers and true/false come back as their text). NULL if the file
       or the key is missing. The pointer stays valid for the whole run. */
    const char *(__cdecl *setting)(const SM64DSModApi *api, const char *key);

    /* Register callbacks (each at most 16 times per plugin). Call from
       sm64ds_mod_main. `user` is handed back to the callback. Return 0 = OK. */
    int (__cdecl *on_frame)(const SM64DSModApi *api, SM64DSFrameFn cb, void *user);
    int (__cdecl *on_level_load)(const SM64DSModApi *api, SM64DSLevelFn cb, void *user);

    /* Edit the ROM file `rom_path` (path as under extracted/dsd/files/, e.g.
       "data/enemy/kuribo/kuribo_model.bmd", case-insensitive; archive members
       by their name inside the archive work too) as it is loaded. The filter
       runs after the files/ layer of every enabled mod, so it sees the bytes
       the game would have used. Return 0 = registered, nonzero = bad
       arguments or too many registrations. A path that names no ROM file is
       found out later, when the game first loads files: it is logged once and
       ignored. The filter can run more than once for the same file (always
       starting from the pristine bytes); keep it free of state. */
    int (__cdecl *on_file)(const SM64DSModApi *api, const char *rom_path,
                           SM64DSFileFn filter, void *user);

    /* Replace the spawn function of actor `actor_id` with `factory`. Returns
       the function that was in place (the game's own, or an earlier plugin's)
       so the new one can wrap it, or NULL if the game has no such actor.
       At most 16 replacements per plugin (and 32 across all plugins). NULL
       means "no such actor" OR "the replacement table is full" (a line in the
       log says which); do not install your factory then, nothing was changed.
       */
    SM64DSActorFactory (__cdecl *replace_actor)(const SM64DSModApi *api,
                                                unsigned actor_id,
                                                SM64DSActorFactory factory);

    /* The address of a game function or global by its name in this exact build
       (the linker map name, e.g. "_port_level_id" for a C function; C++ names
       are the decorated "?..." names). NULL if unknown, and NULL for every
       name when the symbol table next to the exe was made for another build
       (checked against the exe's timestamp and size; one log line). A bare C name is also
       tried with the leading underscore. Cast to the right signature yourself:
       the game is 32-bit, the C functions here are __cdecl unless they are
       __fastcall methods. */
    void *(__cdecl *symbol)(const SM64DSModApi *api, const char *name);

    /* The only allocator that may cross the boundary (see SM64DSFileFn). free
       takes only a block from alloc that you still own; anything else (another
       pointer, a block already freed or handed to the game) is ignored and
       logged. */
    void *(__cdecl *alloc)(const SM64DSModApi *api, size_t size);
    void (__cdecl *free)(const SM64DSModApi *api, void *ptr);
};

#define SM64DS_MOD_API_HAS(api, field) \
    ((api)->size >= offsetof(SM64DSModApi, field) + sizeof((api)->field))

/* The one export the game looks for. */
#ifdef _MSC_VER
#define SM64DS_MOD_EXPORT __declspec(dllexport)
#else
#define SM64DS_MOD_EXPORT
#endif
SM64DS_MOD_EXPORT int __cdecl sm64ds_mod_main(const SM64DSModApi *api);

#ifdef __cplusplus
}
#endif
#endif
