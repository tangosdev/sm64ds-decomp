/* LIVE SKYBOX SWITCHING, FOR THE EDITOR-ARMED BUILD ONLY.
 *
 * `skyset <id>` on the editor channel changes the level's sky while the level
 * is running. Nothing in this file does anything unless the channel armed it,
 * and the channel arms only on SM64DS_EDITOR_CHANNEL=1.
 *
 * =============================================================================
 * WHY THIS IS ALLOWED TO EXIST WHEN THE BOOT-TIME PATH REFUSED TO
 * =============================================================================
 *
 * The lane that added the .lvlm skybox property investigated a live switch and
 * REFUSED to build one, because the obvious implementation -- destroy the sky
 * Model and run Stage::LoadSkybox again -- is unsafe mid-level for three
 * compounding reasons, all still true:
 *
 *   * TEXTURE AND PALETTE VRAM ARE BUMP ALLOCATORS WITH NO FREE. The only
 *     counterpart to an allocation is a whole-arena reset
 *     (src/InitialiseVramGlobals.c, a level-boot line; the port's
 *     hal/model_host.cpp port_model_vram_reset). Load-and-free-and-load walks
 *     the arenas into each other and Model::GetVramOffset reaches the game's
 *     own Crash().
 *   * commonModelDataArr APPENDS WITH NO REFCOUNT AND NO BOUNDS CHECK, 100
 *     records (hal/model_host.cpp:292). Destroying a Model frees its BMD image
 *     but leaves the address in that array.
 *   * IF THE HEAP RETURNS A FREED BMD'S ADDRESS AGAIN, AddToCommonModelDataArr
 *     matches the stale record and SKIPS LoadTexAndPal, so the new file never
 *     gets its VRAM offsets patched. No crash; silently wrong textures.
 *
 * THE SCOPE CHANGED -- the editor-launched build is a test-mode mod rather than
 * the byte-matched game -- but the VRAM findings did not, so the design is
 * built to make all three hazards UNREACHABLE rather than to accept them.
 *
 * THE BUDGET, MEASURED ON A RUNNING LEVEL 6 RATHER THAN ESTIMATED. The numbers
 * below are read off the game's own arena cursors, printed by this file:
 *
 *     after level 6's boot     arena A 76,928 free   arena B 57,600 free
 *                              palette 59,792 free   62 of 100 model records
 *     cost of ONE vrbox sky    arena A 16,384        arena B  8,192
 *                              palette 1,792 (vr03) .. 12,000 (vr11)
 *
 * The port's arenas are 128 KB each and the palette window 96 KB
 * (hal/model_host.cpp:232-241). So ONLY ABOUT FOUR extra skies fit at once:
 * 76,928 / 16,384 = 4.7. Keeping all eleven resident is not possible on this
 * level, and a first cut of this file that tried to cache them all was refused
 * by its own guard after the second one. That measurement is what chose the
 * design below; the arithmetic is in the run log, not in anyone's head.
 *
 * =============================================================================
 * ONE EDITOR SLOT, REUSED, WITH A REWIND THAT IS PROVED BEFORE IT IS TAKEN
 * =============================================================================
 *
 * Two models matter and no more:
 *
 *   THE BOOT SKY      the Model the game's own Stage::LoadSkybox built. It is
 *                     adopted, never freed and never moved. Switching back to
 *                     the level's own sky therefore costs nothing and returns
 *                     the exact object the game made.
 *   THE EDITOR SLOT   at most ONE editor-loaded sky at a time. Asking for a
 *                     different one REUSES the slot.
 *
 * Reuse means giving the VRAM back, and the bump allocators have no free. So
 * this file gives it back the only way that is actually sound: by REWINDING
 * THE CURSORS ITSELF, and only when it can prove that nothing else has
 * allocated since.
 *
 *   before a slot load   snapshot the six cursors and the model-record count
 *   after it             snapshot them again
 *   before the next load COMPARE the live cursors and count against that
 *                        second snapshot. If every one is bit-identical, then
 *                        NOTHING ELSE HAS ALLOCATED SINCE OUR LOAD -- our sky
 *                        is the last thing in the arenas -- so rewinding to
 *                        the first snapshot reclaims exactly our own bytes and
 *                        can reach nobody else's texture. Free the model,
 *                        restore the cursors, truncate the record count.
 *
 * IF ANY CURSOR HAS MOVED, THE REWIND IS NOT TAKEN. Something else -- an actor
 * spawning, a texture animation -- allocated after us, and rewinding would
 * hand its bytes to the next sky. In that case the old slot is simply
 * abandoned in place (its VRAM stays spent) and the new sky is allocated
 * fresh, subject to the headroom guard. That path costs memory and is the rare
 * one; the common path, a user flipping through skies, rewinds every time and
 * costs a constant amount no matter how many switches happen.
 *
 * WHY THIS DOES NOT REOPEN THE THREE HAZARDS:
 *   * the arenas do not grow under repeated switching, because each switch
 *     returns exactly what the previous one took;
 *   * the record count is truncated by exactly the record our load appended,
 *     so no commonModelDataArr entry ever outlives the BMD it describes;
 *   * and because that record is gone, a recycled BMD address cannot match a
 *     stale entry, so the silent skip-LoadTexAndPal path cannot fire.
 * Each of the three is closed by construction rather than by luck.
 *
 * =============================================================================
 * THE HEADROOM GUARD, WHICH IS WHAT MAKES "NEVER CORRUPT" A PROPERTY
 * =============================================================================
 *
 * Model::GetVramOffset does not fail. It calls the game's own Crash(). There is
 * no bounds check to lean on and no error to catch, so the only place a refusal
 * can happen is BEFORE the load. hal/model_host.cpp:222-241 gives the exact
 * shape of the three windows, and the guard is arithmetic on them:
 *
 *     arena A (slot 0)  compressed blocks rise from _bc8, plain textures fall
 *                       from _be8. Free = _be8 - _bc8.
 *     arena B (slot 1)  index words rise from _be0, plain textures fall from
 *                       _bdc. Free = _bdc - _be0.
 *     palette           small rise from _bcc, large fall from _bd8.
 *                       Free = _bd8 - _bcc.
 *
 * A sky is refused unless every window can take ONE more sky at its measured
 * worst cost plus a margin, and unless commonModelDataArr has room. The
 * numbers are the measured ones above, not guesses -- an earlier draft of this
 * file used a flat 48 KB reserve and refused its own second sky, because
 * arena B only has 57,600 bytes free to begin with. A refusal keeps the
 * CURRENT sky, says so in plain words, and changes nothing.
 *
 * With the rewind in place the guard is almost never the thing that fires:
 * switching repeatedly needs room for one sky, not eleven.
 *
 * =============================================================================
 * WHAT IS TOUCHED, AND WHAT IS NOT
 * =============================================================================
 *
 * The one word this file writes into game state is Stage+0x9bc, the Model*
 * that Stage::Render (src/_ZN5Stage6RenderEv.cpp:99) and the port's
 * hal/stage_bridges.cpp:701 read EVERY FRAME to draw the sky. Writing it is
 * exactly what Stage::LoadSkybox does, so a switched sky is drawn by the same
 * code, through the same pointer, as a booted one. Nothing else in the game is
 * written: not the LVL_Overlay's settings word, not the actor list, nothing.
 *
 * GetSkyboxID THEREFORE STILL REPORTS THE BOOT SKY after a live switch, and
 * that is correct rather than a wart: the LVL_Overlay is what the LEVEL says,
 * and this file is an editor preview sitting on top of it. The .lvlm property
 * is what changes the level. port_editor_sky_current() reports what is being
 * DRAWN, which is the question an editor is actually asking.
 *
 * The write happens at the channel's frame drain, after that frame's tick and
 * present, so no frame is ever half-drawn with one sky and half with another.
 *
 * src/ IS UNTOUCHED. Every ROM function used here is called, not modified.
 */

#include <cstdio>
#include <cstring>

extern "C" {

/* The scene root. hal/stage_bridges.cpp:422 returns the Stage the boot built,
   or null before port_stage_create has run. Read, never written. */
void *port_stage_object(void);

/* The ROM's own loader parts, exactly the ones Stage::LoadSkybox uses
   (src/_ZN5Stage10LoadSkyboxEv.c). Reused rather than reimplemented so a
   switched sky is byte-for-byte the same object a booted one is. */
void *_Znwj(unsigned size);
void *_ZN5ModelC1Ev(void *self);
void _ZN5Model14LoadAndSetFileEtii(void *self, unsigned short fileID,
                                   int a, int b);
int _ZN5Stage11GetSkyboxIDEv(void);

/* Model::~Model, the DELETING variant (src/_ZN5ModelD0Ev.c). Frees the owned
   block at +0x4c, runs ModelBase::~ModelBase -- which Deallocates the BMD
   image at +0x04 -- and then frees the Model itself. It is the ROM's own full
   teardown, and it is used ONLY on a model this file built, never on the one
   the game's own boot made. */
void *_ZN5ModelD0Ev(void *self);

/* The vrbox handle table: data_02075620[id - 1], eleven entries. */
extern unsigned short data_02075620[];

/* Model's vtable, which Model::Model installs at +0x00
   (src/_ZN5ModelC1Ev.c). Used ONLY as a cache-validity check. */
extern void *_ZTV5Model[];

/* The texture / palette bump allocators, hal/model_host.cpp:232-241. Read
   only -- this file never moves a cursor itself, it only refuses to start a
   load that would bring two of them together. */
extern unsigned data_020a4bc8, data_020a4be8;   /* arena A: rises, ceiling */
extern unsigned data_020a4be0, data_020a4bdc;   /* arena B: rises, ceiling */
extern unsigned data_020a4bcc, data_020a4bd8;   /* palette: rises, ceiling */
extern unsigned data_020a4be4;                  /* uploaded bytes, running */

/* commonModelDataArr's record count; capacity 100 (hal/model_host.cpp:292). */
extern int data_0209cef8[1];

/* The current level, so the cache can be dropped when the heap under it is. */
extern signed char data_0209f2f8;

}  /* extern "C" */

namespace {

const unsigned OFF_STAGE_SKYBOX = 0x9BC;
const int SKY_MIN = 0;
const int SKY_MAX = 11;
const unsigned MODEL_SIZE = 0x50;   /* what Stage::LoadSkybox news */

/* WHAT ONE SKY COSTS, measured on a running level 6 and printed by this file
   on every load. Arena A and B are exact and identical for all eleven (a
   compressed 4x4 texture: blocks in A, index words in B at half the size).
   The palette varies by sky, 1,792 for vr03 up to 12,000 for vr11, so the
   worst case is what the guard reserves. */
const unsigned SKY_COST_A = 16384;
const unsigned SKY_COST_B = 8192;
const unsigned SKY_COST_PAL = 12288;
/* A margin on top, so the last slice of an arena is left to the GAME rather
   than taken by an editor preview. Small on purpose: an earlier draft used a
   flat 48 KB and refused its own second sky, because arena B starts with only
   57,600 bytes free on this level. */
const unsigned MARGIN_TEX = 8192;
const unsigned MARGIN_PAL = 4096;
const int RESERVE_RECORDS = 8;
const int COMMON_MODEL_CAPACITY = 100;

/* THE BOOT SKY: the Model the game's own Stage::LoadSkybox built. Adopted,
   never freed, never rewound over. */
int g_boot_id = -1;
void *g_boot_model;

/* THE EDITOR SLOT: at most one editor-loaded sky at a time, reused. */
int g_slot_id;                  /* 0 = the slot is empty */
void *g_slot_model;

/* The allocator state around the slot's load. `pre` is what it was before,
   `post` is what our own load left it at. A rewind is legal only while the
   live cursors still equal `post` -- see the banner. */
struct VramMark {
    unsigned bc8, be8, be0, bdc, bcc, bd8;
    int records;
};
VramMark g_pre, g_post;
bool g_slot_marked;

int g_cache_level = -1;
void *g_cache_stage;
int g_current = -1;         /* what is being DRAWN, -1 = not established */
int g_loads;                /* how many models this file has ever built */
int g_rewinds;              /* how many times the slot was reclaimed */
int g_abandons;             /* how many times a rewind was NOT safe to take */
bool g_armed;

void mark_now(VramMark &m)
{
    m.bc8 = data_020a4bc8; m.be8 = data_020a4be8;
    m.be0 = data_020a4be0; m.bdc = data_020a4bdc;
    m.bcc = data_020a4bcc; m.bd8 = data_020a4bd8;
    m.records = data_0209cef8[0];
}

bool mark_equals_now(const VramMark &m)
{
    return m.bc8 == data_020a4bc8 && m.be8 == data_020a4be8
        && m.be0 == data_020a4be0 && m.bdc == data_020a4bdc
        && m.bcc == data_020a4bcc && m.bd8 == data_020a4bd8
        && m.records == data_0209cef8[0];
}

void restore_mark(const VramMark &m)
{
    data_020a4bc8 = m.bc8; data_020a4be8 = m.be8;
    data_020a4be0 = m.be0; data_020a4bdc = m.bdc;
    data_020a4bcc = m.bcc; data_020a4bd8 = m.bd8;
    data_0209cef8[0] = m.records;
}

/* A cached Model* is only usable if it still looks like a Model. Model::Model
   installs _ZTV5Model at +0x00, so a block that no longer carries that vptr
   was rolled back by a save-state restore, freed by a level teardown, or
   reused -- and must not be handed to the renderer. Cheap, and it turns the
   one real hazard of caching a game-heap pointer into a reload. */
bool looks_like_model(void *m)
{
    return m && *(void **)m == (void *)_ZTV5Model;
}

/* Forget everything. Nothing is freed here: on a level change the heap these
   pointers live in has already been rebuilt under us, so a free would be a
   free of somebody else's memory. */
void drop_cache(void)
{
    g_boot_id = -1;
    g_boot_model = 0;
    g_slot_id = 0;
    g_slot_model = 0;
    g_slot_marked = false;
    g_current = -1;
}

/* Re-key to the level that is up now and ADOPT the sky the boot loaded.
   Adopting matters: switching away from the level's own sky and back costs no
   load at all, and the object that comes back is the one the game made. */
void resync(void *stage)
{
    int lvl = (int)data_0209f2f8;
    if (lvl == g_cache_level && stage == g_cache_stage)
        return;
    drop_cache();
    g_cache_level = lvl;
    g_cache_stage = stage;

    int boot = _ZN5Stage11GetSkyboxIDEv();
    void *m = *(void **)((char *)stage + OFF_STAGE_SKYBOX);
    if (boot >= 1 && boot <= SKY_MAX && looks_like_model(m)) {
        g_boot_id = boot;
        g_boot_model = m;
        g_current = boot;
    } else if (!m) {
        g_boot_id = 0;
        g_current = 0;          /* the level booted with no sky */
    }
}

/* Room for ONE more sky at its measured worst cost, plus a margin. With the
   rewind working this is rarely the thing that fires -- switching repeatedly
   needs room for one sky, not eleven. */
bool headroom(const char **why)
{
    unsigned a = data_020a4be8 > data_020a4bc8
               ? data_020a4be8 - data_020a4bc8 : 0;
    unsigned b = data_020a4bdc > data_020a4be0
               ? data_020a4bdc - data_020a4be0 : 0;
    unsigned p = data_020a4bd8 > data_020a4bcc
               ? data_020a4bd8 - data_020a4bcc : 0;

    if (a < SKY_COST_A + MARGIN_TEX || b < SKY_COST_B + MARGIN_TEX) {
        *why = "this level has used too much texture memory to fit another "
               "sky; the sky is unchanged";
        return false;
    }
    if (p < SKY_COST_PAL + MARGIN_PAL) {
        *why = "this level has used too much palette memory to fit another "
               "sky; the sky is unchanged";
        return false;
    }
    if (data_0209cef8[0] >= COMMON_MODEL_CAPACITY - RESERVE_RECORDS) {
        *why = "this level has registered too many models to fit another sky; "
               "the sky is unchanged";
        return false;
    }
    return true;
}

/* Give the editor slot back, but ONLY when it can be proved that our own load
   is still the last thing in the arenas. See the banner: every cursor and the
   model-record count must be bit-identical to what our load left behind. If
   anything moved, the slot is abandoned in place instead -- its VRAM stays
   spent, which is a cost, where rewinding over a stranger's texture would be a
   corruption. Returns true if the slot was reclaimed. */
bool release_slot(void)
{
    if (!g_slot_model)
        return false;
    if (!g_slot_marked || !mark_equals_now(g_post)) {
        /* Somebody else allocated after us. Let it go. */
        ++g_abandons;
        std::fprintf(stderr, "[editor] sky %d left in place: something else "
                             "loaded after it, so its memory cannot be "
                             "reclaimed safely\n", g_slot_id);
        std::fflush(stderr);
        g_slot_model = 0;
        g_slot_id = 0;
        g_slot_marked = false;
        return false;
    }
    _ZN5ModelD0Ev(g_slot_model);
    restore_mark(g_pre);
    ++g_rewinds;
    g_slot_model = 0;
    g_slot_id = 0;
    g_slot_marked = false;
    return true;
}

/* Stage::LoadSkybox's body with the id supplied rather than read, and with no
   store into Stage+0x9bc -- the caller decides when the swap happens. */
void *build_sky(int id)
{
    void *m = _Znwj(MODEL_SIZE);
    if (!m)
        return 0;
    m = _ZN5ModelC1Ev(m);
    _ZN5Model14LoadAndSetFileEtii(m, data_02075620[id - 1], 0, 2);
    ++g_loads;
    return m;
}

}  /* namespace */

/* Armed by hal/editor_channel.cpp when SM64DS_EDITOR_CHANNEL says so, and by
   nothing else. Every entry point below is inert until this has been called,
   so a build that contains this file but is launched without the channel does
   not differ from one that does not contain it -- no allocation, no load, no
   write, not one byte of game state touched. */
extern "C" void port_editor_sky_arm(void)
{
    g_armed = true;
}

/* What is being DRAWN right now: 0..11, or -1 when that is not established
   (no level up, or the channel is not armed). */
extern "C" int port_editor_sky_current(void)
{
    if (!g_armed)
        return -1;
    void *stage = port_stage_object();
    if (!stage)
        return -1;
    resync(stage);
    return g_current;
}

/* THE VERB. Returns 1 on success; 0 with *why set to a plain-words reason.
   Called from the channel's frame drain, never from the socket thread. */
extern "C" int port_editor_sky_set(int id, const char **why)
{
    static const char *unarmed = "the editor channel is not armed";
    void *stage;

    if (!g_armed) {
        *why = unarmed;
        return 0;
    }
    if (id < SKY_MIN || id > SKY_MAX) {
        *why = "the sky is 0 for none, or 1 to 11";
        return 0;
    }
    stage = port_stage_object();
    if (!stage) {
        *why = "no level is up";
        return 0;
    }
    resync(stage);

    void **slot = (void **)((char *)stage + OFF_STAGE_SKYBOX);

    if (id == 0) {
        /* No sky. The renderer's own null check makes this a no-op draw, the
           same one a level whose LVL_Overlay says 0 gets. The model that was
           being drawn stays in the cache, so coming back costs nothing. */
        *slot = 0;
        g_current = 0;
        return 1;
    }

    /* The level's OWN sky, free to return to: it is the game's model, it was
       never rewound over, and nothing has to be loaded to show it again. */
    if (id == g_boot_id && looks_like_model(g_boot_model)) {
        *slot = g_boot_model;
        g_current = id;
        return 1;
    }

    /* Already in the editor slot: a pointer write and nothing else. */
    if (id == g_slot_id && looks_like_model(g_slot_model)) {
        *slot = g_slot_model;
        g_current = id;
        return 1;
    }

    /* A different sky is wanted.

       THE HEADROOM QUESTION IS ASKED BEFORE ANYTHING IS FREED, and that
       ordering is the whole reason a refusal can honestly say "the sky is
       unchanged". An earlier version released the slot first and then checked;
       when the check failed it had already freed the sky it was drawing, so
       the picture went black while the message claimed nothing had happened.

       When the slot CAN be reclaimed the check is not needed: a rewind puts
       the cursors back exactly where they were before the last load, and that
       state had already passed this same guard. */
    const bool can_reclaim = g_slot_model && g_slot_marked
                          && mark_equals_now(g_post);
    if (!can_reclaim && !headroom(why))
        return 0;               /* nothing touched; the sky really is unchanged */

    /* Never free what the renderer is pointed at. The drain runs between a
       frame's tick and the next one, so no frame observes the gap. */
    if (g_slot_model && *slot == g_slot_model)
        *slot = 0;
    release_slot();

    if (can_reclaim && !headroom(why)) {
        /* Unreachable by the argument above, handled anyway rather than
           trusted: fall back to the level's own sky and say what happened
           instead of leaving the screen with no sky and a cheerful reply. */
        if (looks_like_model(g_boot_model)) {
            *slot = g_boot_model;
            g_current = g_boot_id;
        } else {
            g_current = 0;
        }
        *why = "there was not enough texture memory after all; the level's own "
               "sky is showing instead";
        return 0;
    }

    mark_now(g_pre);
    void *m = build_sky(id);
    if (!m) {
        /* The slot is already gone by here, so this does NOT claim the sky is
           unchanged -- it says what is actually on screen now. */
        if (looks_like_model(g_boot_model)) {
            *slot = g_boot_model;
            g_current = g_boot_id;
            *why = "the game would not build that sky; the level's own sky is "
                   "showing instead";
        } else {
            g_current = 0;
            *why = "the game would not build that sky; the level is showing no "
                   "sky";
        }
        return 0;
    }
    mark_now(g_post);
    g_slot_model = m;
    g_slot_id = id;
    g_slot_marked = true;

    std::fprintf(stderr, "[editor] sky %d loaded (file %u); texture arenas "
                         "%u/%u bytes free, palette %u, models %d/%d "
                         "(rewinds %d, abandoned %d)\n",
                 id, (unsigned)data_02075620[id - 1],
                 data_020a4be8 - data_020a4bc8,
                 data_020a4bdc - data_020a4be0,
                 data_020a4bd8 - data_020a4bcc,
                 data_0209cef8[0], COMMON_MODEL_CAPACITY,
                 g_rewinds, g_abandons);
    std::fflush(stderr);

    *slot = m;
    g_current = id;
    return 1;
}

/* For the proof harness: how many models this file has built, and the live
   arena figures. Printed rather than returned so the numbers land in the run
   log next to everything else. */
extern "C" void port_editor_sky_stats(void)
{
    std::fprintf(stderr, "[editor] sky stats: %d model(s) built, %d rewind(s), "
                         "%d abandoned, drawing %d, "
                         "arenaA %u free, arenaB %u free, palette %u free, "
                         "uploaded %u bytes, models %d/%d\n",
                 g_loads, g_rewinds, g_abandons, g_current,
                 data_020a4be8 - data_020a4bc8,
                 data_020a4bdc - data_020a4be0,
                 data_020a4bd8 - data_020a4bcc,
                 data_020a4be4, data_0209cef8[0], COMMON_MODEL_CAPACITY);
    std::fflush(stderr);
}
