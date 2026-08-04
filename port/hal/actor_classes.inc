/* The registry's class rows, one line per hosted actor class.
 *
 * Included by hal/actor_registry.cpp into the table body. Each row is
 *
 *     { actorID, "NAME", <ROM SpawnInfo>, <host factory>, <vtable fill> }
 *
 * and each needs three things landed together: the SpawnInfo in
 * port/ov002_syms.txt (or the ov009 whole mount), the class's matched src
 * closure in slice_gate16.txt, and a vtable fill in hal/actor_classes.cpp
 * written to the vtable law.
 *
 * The factory column is the class's own matched <Class>_Spawn, which the ROM
 * stores in the SpawnInfo's +0 word; registering the class overwrites that
 * word with this host address and leaves the rest of Nintendo's record alone.
 */
extern "C" {
/* ---- TREE (286, ov002) x21 ---- */
extern unsigned char Tree_SpawnInfo[];      /* ov002 0x0210abdc */
void *Tree_Spawn(void);                     /* ov002 0x020ec32c */
void hal_fill_tree_vtable(void);
/* ---- AMBIENT_SOUND_EFFECTS (350, ov002) x5 ----
   The class whose methods and vtable the config names EnemySwitchTag; see the
   header of that section in hal/actor_classes.cpp for how the ROM settles it. */
extern unsigned char AmbientSoundEffects_SpawnInfo[];   /* ov002 0x0210b47c */
void *AmbientSoundEffects_Spawn(void);                  /* ov002 0x020f1b94 */
void hal_fill_ambient_sound_vtable(void);
/* ---- BLACK_BRICK_BLOCK (17, ov002) x1 ---- */
extern unsigned char BlackBrickBlock_SpawnInfo[];       /* ov002 0x02108a78 */
void *BlackBrickBlock_Spawn(void);                      /* ov002 0x020b409c */
void hal_fill_black_brick_block_vtable(void);
/* ---- SIGN_POST (184, ov002) x5 ---- */
extern unsigned char SignPost_SpawnInfo[];              /* ov002 0x02109ad4 */
void *SignPost_Spawn(void);                             /* ov002 0x020bc3c8 */
void hal_fill_sign_post_vtable(void);
/* ---- ONE_UP_MUSHROOM (276, ov002) x3 ---- */
extern unsigned char OneUpMushroom_SpawnInfo[];         /* ov002 0x02108388 */
void *OneUpMushroom_Spawn(void);                        /* ov002 0x020b0580 */
void hal_fill_one_up_mushroom_vtable(void);
/* ---- gate 17: the level overlay's own four ----
   The SpawnInfo records come from the PER-SYMBOL ov009 mount (ov009_syms.txt),
   not the whole one: the registry rewrites the factory word in place and the
   ActorBase constructor reads the two priorities back out of the same record,
   so it has to be storage the actor code and the registry agree on. */
extern unsigned char Bird_SpawnInfo[];                  /* ov009 0x02113934 */
void *Bird_Spawn(void);                                 /* ov009 0x02111a30 */
void hal_fill_bird_vtable(void);
extern unsigned char CastleWater_SpawnInfo[];           /* ov009 0x021139f4 */
void *CastleWater_Spawn(void);                          /* ov009 0x02111d8c */
void hal_fill_castle_water_vtable(void);
extern unsigned char MetalNet_SpawnInfo[];              /* ov009 0x02113abc */
void *MetalNet_Spawn(void);                             /* ov009 0x02112048 */
void hal_fill_metal_net_vtable(void);
extern unsigned char Flag_SpawnInfo[];                  /* ov009 0x02113b7c */
void *Flag_Spawn(void);                                 /* ov009 0x021121f0 */
void hal_fill_flag_vtable(void);
/* ---- gate 18: ov085, the first ACTOR overlay ----
   Not the engine overlay and not the level's -- six classes several levels
   share, of which the castle grounds names two. The SpawnInfo records come
   from the per-symbol ov085 mount (ov085_syms.txt). */
extern unsigned char Rabbit_SpawnInfo[];                /* ov085 0x021300d4 */
void *Rabbit_Spawn(void);                               /* ov085 0x0212cc2c */
void hal_fill_rabbit_vtable(void);
extern unsigned char LakituBro_SpawnInfo[];             /* ov085 0x02130320 */
void *LakituBro_Spawn(void);                            /* ov085 0x0212ed54 */
void hal_fill_lakitu_bro_vtable(void);
/* ---- gate 19: ov098's CANNON ---- */
extern unsigned char Cannon_SpawnInfo[];                /* ov098 0x0213c67c */
void *Cannon_Spawn(void);                               /* ov098 0x0213b43c */
void hal_fill_cannon_vtable(void);
/* ---- gate 20: ov002's last two ----
   Both SpawnInfo records come from the ov002 per-symbol mount that has
   carried Player/Tree/SignPost since gate 14. The class-method names are the
   neighbour's in both cases and the *_SpawnInfo names are the right ones --
   the ROM's RTTI settles it, see slice_gate20.txt. */
extern unsigned char Exit_SpawnInfo[];                  /* ov002 0x02108690 */
void *Exit_Spawn(void);                                 /* ov002 0x020b0f24 */
void hal_fill_exit_vtable(void);
extern unsigned char WaterfallMist_SpawnInfo[];         /* ov002 0x0210947c */
void *WaterfallMist_Spawn(void);                        /* ov002 0x020b6ee8 */
void hal_fill_waterfall_mist_vtable(void);
/* ---- gate 21: ov100's BUTTERFLY and FISH ----
   The third actor overlay (ov100_syms.txt). Both class names check out
   against the ROM's own RTTI; the one shifted pair in ov100 is Door/StarDoor
   and that is gate 22's problem. */
extern unsigned char Butterfly_SpawnInfo[];             /* ov100 0x02147e78 */
void *Butterfly_Spawn(void);                            /* ov100 0x02141ea4 */
void hal_fill_butterfly_vtable(void);
extern unsigned char Fish_SpawnInfo[];                  /* ov100 0x02148498 */
void *Fish_Spawn(void);                                 /* ov100 0x02146d38 */
void hal_fill_fish_vtable(void);
/* ---- gate 22: ov100's DOOR ----
   The last class the castle grounds names. Its vtable is the one config left
   as data_ov100_02148188 while giving `_ZTV4Door` to the star door; the ROM's
   RTTI settles it, see slice_gate22.txt. */
extern unsigned char Door_SpawnInfo[];                  /* ov100 0x02148164 */
void *Door_Spawn(void);                                 /* ov100 0x0214589c */
void hal_fill_door_vtable(void);
/* ---- gate 23: ov102's QUESTION_BLOCK ----
   The fifth overlay, and the last id on the castle grounds' skip list that is
   not a 2D system. Its name checks out against the ROM's RTTI
   (daObjHatenaBlock_c) and its SpawnInfo's own +4 halfword. */
extern unsigned char QuestionBlock_SpawnInfo[];         /* ov102 0x0214e3e8 */
void *QuestionBlock_Spawn(void);                        /* ov102 0x0214a8ec */
void hal_fill_question_block_vtable(void);
}

#define PORT_ACTOR_CLASS_ROWS                                                 \
    {286, "TREE", Tree_SpawnInfo, Tree_Spawn, hal_fill_tree_vtable},          \
    {350, "AMBIENT_SOUND", AmbientSoundEffects_SpawnInfo,                     \
     AmbientSoundEffects_Spawn, hal_fill_ambient_sound_vtable},               \
    {17, "BLACK_BRICK_BLOCK", BlackBrickBlock_SpawnInfo,                      \
     BlackBrickBlock_Spawn, hal_fill_black_brick_block_vtable},               \
    {184, "SIGN_POST", SignPost_SpawnInfo, SignPost_Spawn,                    \
     hal_fill_sign_post_vtable},                                              \
    {276, "ONE_UP_MUSHROOM", OneUpMushroom_SpawnInfo, OneUpMushroom_Spawn,    \
     hal_fill_one_up_mushroom_vtable},                                        \
    {338, "CASTLE_WATER", CastleWater_SpawnInfo, CastleWater_Spawn,           \
     hal_fill_castle_water_vtable},                                           \
    {339, "METAL_NET", MetalNet_SpawnInfo, MetalNet_Spawn,                    \
     hal_fill_metal_net_vtable},                                              \
    {342, "FLAG", Flag_SpawnInfo, Flag_Spawn, hal_fill_flag_vtable},          \
    {343, "BIRD", Bird_SpawnInfo, Bird_Spawn, hal_fill_bird_vtable},          \
    {187, "RABBIT", Rabbit_SpawnInfo, Rabbit_Spawn, hal_fill_rabbit_vtable},  \
    {235, "LAKITU_BRO", LakituBro_SpawnInfo, LakituBro_Spawn,                 \
     hal_fill_lakitu_bro_vtable},                                             \
    {207, "CANNON", Cannon_SpawnInfo, Cannon_Spawn, hal_fill_cannon_vtable}, \
    {349, "EXIT", Exit_SpawnInfo, Exit_Spawn, hal_fill_exit_vtable},         \
    {197, "WATERFALL_MIST", WaterfallMist_SpawnInfo, WaterfallMist_Spawn,    \
     hal_fill_waterfall_mist_vtable},                                        \
    {336, "BUTTERFLY", Butterfly_SpawnInfo, Butterfly_Spawn,                 \
     hal_fill_butterfly_vtable},                                             \
    {344, "FISH", Fish_SpawnInfo, Fish_Spawn, hal_fill_fish_vtable},             {353, "DOOR", Door_SpawnInfo, Door_Spawn, hal_fill_door_vtable},             {20, "QUESTION_BLOCK", QuestionBlock_SpawnInfo, QuestionBlock_Spawn,          hal_fill_question_block_vtable},
