// COOP PHASE 2 CLASS PROFILES. Run mp1, lane COOPMOVE1. Included once by coop_actors.cpp.
//
// One row per actor class id that is kept in step between consoles. Columns:
//   id          actor class id (+0x0c), the registry's number
//   name        for logs
//   period_ms   how often the owner publishes a moving actor (a still one heartbeats at 1.5 s)
//   band        world units: inside it a receiver leaves its own copy alone
//   snap        world units: past it a receiver writes the owner's pose; between band and snap it eases 25 percent
//   flags       kFPhase = the class has counters that must be written WITH the pose (every record applies)
//   nf, f[]     phase fields: {offset into the actor, width 1/2/4, lo, hi}; offsets 0xd0..0x47f, aligned to the width, ENTIRELY inside
//               the class's allocation (coop_actor_sizes.inc: a field past it drops the row at boot), and a received value must
//               lie in lo..hi (signed for width 4) or the whole record is refused
//
// The field lists are DATA FOUND BY DISCOVERY, not guesses: SM64DS_COOP_MOVE_DUMP dumps every synced actor's
// words at two ticks and coop_drift.py's `fields` command keeps the words that change with level age and are not
// pointers. SM64DS_COOP_MOVE_PROFILE=<file> overrides or adds rows at run time (same columns, one per line:
// `id period band snap flags nf off:size ...`), which is how the discovery is iterated without a rebuild.
// A class that is not in this table is not touched.
//
// ADDING A CLASS (COOPCASTLE1: Boo 209, MIPS 187, ...): append one SIZE(...) line to coop_actor_sizes.inc (a class with phase fields or a
// carried-test word MUST have one) and one ROW(...) line in the section that fits, enemies
// {period 100, band 40, snap 400, flags 0, nf 0}; a Boo or rabbit that flees or hides keeps the same numbers. No code change
// is needed: ownership, publish, apply, snapshot and the carried-test (profile column `hold`, set in prof_index) are generic.
// To try a row without a rebuild put it in an SM64DS_COOP_MOVE_PROFILE file. The actor is never touched when it is dead,
// carried, in a yoshi mouth, and its damage to the local player is never suppressed (enemies hurt players in coop).
//
// Registry names are the port's (hal/actor_classes.inc); several ids run a sibling class (the "name shift"), so a
// header's field list is only trusted after the discovery run agrees.

// ---- enemies: they chase, bounce and decide; owner by proximity, 10 Hz near ----
ROW(200, "GOOMBA",              100, 16, 250, 2, 0)
ROW(201, "GOOMBA_SMALL",        100, 16, 250, 2, 0)
ROW(202, "GOOMBA_LARGE",        100, 16, 250, 2, 0)
ROW(206, "BOB_OMB",             100, 16, 250, 2, 0)
ROW(219, "CHAIN_CHOMP",         50, 12, 250, 2, 0)
ROW(337, "UNCHAINED_CHOMP",     100, 16, 250, 2, 0)
ROW(270, "FLAME_CHOMP",         100, 16, 250, 2, 0)
ROW(230, "SKEETER",             100, 16, 250, 2, 0)
ROW(312, "SPINDRIFT",           100, 16, 250, 2, 0)
ROW(255, "SCUTTLEBUG",          100, 16, 250, 2, 0)
ROW(232, "FLY_GUY",             100, 16, 250, 2, 0)
ROW(215, "BULLY",               100, 16, 250, 2, 0)
ROW(260, "SPINY",               100, 16, 250, 2, 0)
ROW(237, "SWOOP",               100, 16, 250, 2, 0)
ROW(223, "MR_BLIZZARD",         100, 16, 250, 2, 0)
ROW(310, "MONTY_MOLE",          100, 16, 250, 2, 0)
ROW(220, "ROLLING_IRON_BALL",   50, 12, 80, 2, 0)
ROW(214, "LAVA_BUBBLE",         100, 16, 120, 2, 0)
ROW(266, "AMP",                 100, 16, 120, 2, 0)
// ---- movers on a fixed route: deterministic, phase fields first, 1 Hz ----
ROW(29,  "ROTATING_UP_DOWN_PLATFORM", 100, 8, 150, 2, 0)
ROW(30,  "ROTATING_UP_DOWN_PLATFORM_UTM", 100, 8, 150, 2, 0)
ROW(31,  "PATH_LIFT",           100, 8, 150, 2, 0)
ROW(32,  "UP_DOWN_LIFT_BBH",    100, 8, 150, 2, 0)
ROW(33,  "UP_DOWN_LIFT_HMC",    100, 8, 150, 2, 0)
ROW(52,  "TOWER_STEP",          100, 8, 150, 2, 0)
ROW(55,  "SLIDING_PLATFORM_WF", 100, 8, 150, 3, 2, {0x31c, 4, 0, 0x00ffffff}, {0x320, 4, 0, 0x0007ffff})
ROW(53,  "MOVING_BAR_BIG",      100, 8, 150, 3, 2, {0x330, 4, 0, 6}, {0x334, 4, -0x100, 0x400})
ROW(54,  "MOVING_BAR_SMALL",    100, 8, 150, 3, 2, {0x330, 4, 0, 6}, {0x334, 4, -0x100, 0x400})
ROW(73,  "LAVA_BRIDGE",         100, 8, 150, 2, 0)
ROW(77,  "LAVA_SEESAW",         100, 8, 150, 2, 0)
ROW(39,  "SEESAW_BOB",          100, 8, 150, 3, 1, {0x324, 2, 0, 0xffff})   // the tilt is mAngleX (pose) plus mAngleXSpeed, the s16 Behavior / the stood-on callback integrate (src/game/actors/d_a_obj_seesaw.cpp)
ROW(81,  "ROTATING_FIREBAR",    100, 8, 150, 2, 0)
ROW(88,  "PYRAMID_LIFT",        100, 8, 150, 2, 0)
ROW(124, "SLIDING_PLATFORM_RR", 100, 8, 150, 3, 2, {0x31c, 4, 0, 0x00ffffff}, {0x320, 4, 0, 0x0007ffff})
ROW(131, "UP_DOWN_LIFT_RR",     100, 8, 150, 2, 0)
ROW(138, "SLIDING_PLATFORM_BDW", 100, 8, 150, 3, 2, {0x31c, 4, 0, 0x00ffffff}, {0x320, 4, 0, 0x0007ffff})
ROW(144, "SQUARE_METAL_NET_LIFT", 100, 8, 150, 2, 0)
ROW(146, "SLIDING_PLATFORM_BFS_RECTANGLE", 100, 8, 150, 3, 2, {0x31c, 4, 0, 0x00ffffff}, {0x320, 4, 0, 0x0007ffff})
ROW(147, "SLIDING_PLATFORM_BFS_SQUARE", 100, 8, 150, 3, 2, {0x31c, 4, 0, 0x00ffffff}, {0x320, 4, 0, 0x0007ffff})
ROW(154, "SLIDING_PLATFORM_BS_WIDE", 100, 8, 150, 3, 2, {0x31c, 4, 0, 0x00ffffff}, {0x320, 4, 0, 0x0007ffff})
ROW(155, "SLIDING_PLATFORM_BS_LONG", 100, 8, 150, 3, 2, {0x31c, 4, 0, 0x00ffffff}, {0x320, 4, 0, 0x0007ffff})
ROW(157, "ARROW_PATH_LIFT",     100, 8, 150, 2, 0)
ROW(148, "EXTENDING_PLATFORM",  100, 8, 150, 2, 0)
ROW(173, "BIG_MOVING_ICE_BLOCK", 100, 8, 150, 2, 0)
ROW(174, "BOBBING_LOG_EMM",     100, 8, 150, 2, 0)
ROW(175, "SQUARE_PATH_LIFT",    100, 8, 150, 2, 0)
ROW(63,  "SKI_LIFT",            100, 8, 150, 2, 0)
ROW(68,  "WORK_ELEVATOR",       100, 8, 150, 2, 0)
ROW(74,  "FLOAT_ON_LAVA_PLATFORM", 100, 8, 150, 2, 0)
ROW(318, "FLAMETHROWER",        100, 8, 150, 2, 0)
// ---- TTC and clock mechanisms (rotators: the angles are the state) ----
ROW(108, "TTC_ROTATING_CUBE",   100, 8, 150, 2, 0)
ROW(109, "TTC_ROTATING_PRISM",  100, 8, 150, 2, 0)
ROW(110, "TTC_PENDULUM",        100, 8, 150, 2, 0)
ROW(113, "TTC_MOVING_BAR",      100, 8, 150, 2, 0)
ROW(114, "TTC_ROTATING_GEAR",   100, 8, 150, 2, 0)
ROW(115, "TTC_ROTATING_TRIANGLE", 100, 8, 150, 2, 0)
ROW(116, "TTC_MOVING_CUBE_A",   100, 8, 150, 2, 0)
ROW(117, "TTC_MOVING_CUBE_B",   100, 8, 150, 2, 0)
ROW(119, "ROTATING_CLOCK_HAND", 100, 8, 150, 2, 0)
ROW(120, "SPINNING_PLATFORM",   100, 8, 150, 2, 0)
ROW(121, "ROTATING_COG_BIG",    100, 8, 150, 2, 0)
ROW(122, "ROTATING_COG_SMALL",  100, 8, 150, 2, 0)
ROW(127, "SWINGING_PLATFORM",   100, 8, 150, 2, 0)
ROW(294, "CLOCK_PENDULUM",      100, 8, 150, 2, 0)
// ---- castle (COOPCASTLE1): Boos hide and chase by their own camera test, MIPS flees; same numbers as the enemies ----
ROW(209, "BOO",                 100, 40, 400, 2, 0)
ROW(187, "RABBIT",              100, 8, 300, 2, 0)
// ---- bosses (run mp1, lane BOSSCORE1): the pose rides this table, HP / state / defeat ride hal/coop_boss.cpp ----
ROW(165, "WHOMP_KING",          100, 12, 200, 6, 0)
ROW(189, "KING_BOB_OMB",        100, 16, 250, 6, 0)
// ---- the other bosses: pose through the mover path, target through hal/coop_target.cpp; HP / phase rows are per-family lanes ----
ROW(176, "EYEROK",              100, 16, 250, 6, 0)
ROW(198, "GOOMBOSS",            100, 16, 250, 6, 0)
ROW(210, "BIG_BOO",             100, 16, 250, 6, 0)
ROW(216, "BIG_BULLY",           100, 16, 250, 6, 0)
ROW(218, "CHIEF_CHILLY",        100, 16, 250, 6, 0)
ROW(248, "WIGGLER",             100, 16, 250, 6, 0)
ROW(279, "BOWSER",              100, 16, 250, 6, 0)
// ---- MOVERS9: the movers and enemies the census listed without a row (COOPSCOUT9A item 10). Pose rows: nf 0, no state fields
// (state fields wait for the always-on [syncstat] numbers to show a divergence). Enemies {100, 16, 250}, rooted / slow things {100, 8, 150}.
ROW(262, "MR_I",                100, 16, 250, 2, 0)
ROW(250, "PIRANHA_PLANT",       100, 16, 250, 2, 0)
ROW(251, "FIRE_PIRANHA_PLANT_BIG", 100, 16, 250, 2, 0)
ROW(236, "SNUFIT",              100, 16, 250, 2, 0)
ROW(240, "POKEY",               100, 16, 250, 2, 0)
ROW(238, "HEAVE_HO",            100, 16, 250, 2, 0)
ROW(315, "CLAM",                100, 16, 250, 2, 0)
ROW(234, "HOOT_THE_OWL",        100, 16, 250, 2, 0)
ROW(18,  "ICE_BLOCK",           100, 8, 150, 2, 0)
ROW(194, "CRATE",               100, 16, 250, 2, 0)
// (193 CRAZED_CRATE has no row: it hops at random and the pose pin made it WORSE, p95 31 vs 29 units at SSL and 44 vs 27 at LLL, MOVERS9 rows)
ROW(96,  "ARROW_LIFT",          100, 8, 150, 2, 0)
ROW(72,  "TILTING_PLATFORM_LLL", 100, 8, 150, 2, 0)
// COOPROWS1: the push block (192) only moves while pushed, so its place is its whole state (hal/coop_switch.cpp redraws it).
ROW(192, "PUSH_BLOCK",          100, 8, 150, 2, 0)
