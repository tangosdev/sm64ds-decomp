# Particle naming: what the original source called it

The particle front end the game code calls (`Particle::SysTracker`, now `dPa_c`;
`Particle::System::New`, `NewSimple`, ...) was named by the SM64DS hacking
community before this decomp existed. None of those names is in the ROM. This
note records what *is* in the ROM, what a sibling EAD codebase with real names
says about the same code, and the renames that follow.

## ROM evidence

The only particle class names the cartridge carries are thirteen RTTI strings,
all nested in one class:

    dPa_c::level_c::callback_c                (base, two virtuals)
    dPa_c::level_c::{simple,scale,splash,bubble,fitWater,fitWaterSimple,
                     checkWater,checkWaterRipple,checkYogan,edStarKira,
                     clip,cleanParticle}Callback_c

`level_c` has no RTTI of its own; it is proven as the scope these are nested in.
`yogan` (lava) and `kira` (sparkle) show the original mixed romaji into English
names, the way EAD's Zelda code does.

## One translation unit

arm9 `.text` 0x02021a04..0x02023204 is one contiguous run, in ROM order:

| span | tree name today |
|---|---|
| 0x02021a04..0x02021e40 | `dPa_c::level_c` + `level_c::emitter_c` (was `Particle::SysTracker::Contents`/`Entry`) |
| 0x02021e40..0x020226d4 | `dPa_c::level_c::*Callback_c` (ROM RTTI) |
| 0x020226d4..0x02022ee0 | `Particle::SetSelfDestructFlag`, the `System::New*` helpers, `System::New`, `FromUniqueID`, `NewSimple` |
| 0x02022ee0..0x02023204 | `Particle::RenderAll`, `SysTracker::Update`/`Initialise`, `Texture` VRAM allocs, `~SysTracker`, `SysTracker()` |

mwccarm emits a TU's functions in reverse source order, so read upward this is
one source file that opens with the owner's constructor and ends with
`level_c`: the shape of an EAD `d_pa.cpp`. The particle library proper
(`Particle::Manager`, the emitter, draw and animation code, SPL's ancestor)
is elsewhere, at 0x02049xxx..0x0204dxxx.

## The sibling: Twilight Princess `d_particle.cpp`

TP (EAD, 2006) has a `dPa_control_c::level_c`, with names from Nintendo's
symbol maps and assert strings (`mFreeNo < EMITTER_MAX`). Its routines are
our 0x02021a04 run almost line for line:

| SM64DS (0x02021a04 run) | TP `dPa_control_c::level_c` |
|---|---|
| u32 counter at +0, pre-incremented, skips 0 | `field_0x0`, same |
| u8 at +4, steps `(n + 1) % 0x40` looking for a free slot | `mFreeNo`, `(mFreeNo + 1) % EMITTER_MAX` |
| 0x40 x 0x1c slots at +8 | `emitter_c mEmitter[0x80]` at +8, 0x18 each |
| 16 bucket heads, `id & 0xf` | `mTable[0x100]`, `(u8)id` |
| slot: id, effect, status, emitter, callback, prev, next | `mId, mNameId, mStatus, mEmitter, mCallback, mPre, mNext` |
| find by id, 0 = none | `get(u32)` |
| allocate + initialise + link, return id | `entry(...)` |
| head-insert into bucket | `addTable(emitter_c*)` |
| unlink, then tear the slot down | `cutTable(emitter_c*)` |
| per frame: clear status if requested, else cut and set free index | `execute()` |
| cut every live slot, set free index | `cleanup()` |
| slot set-up / tear-down | `emitter_c::entry` / `emitter_c::cleanup` |

"Level" emitters are the ones a caller keeps alive by asking again every frame
under the id `entry()` returned; that is exactly what `System::New(uniqueID, ...)`
callers do.

## Kept-alive vs fire-and-forget effects

There are two ways to spawn an effect:

- **Level emitter, `System::New(uniqueID, effect, x, y, z, dir, callback)`.**
  The caller stores the returned id and passes it back every frame. A live id
  makes `New` set `mStatus = 1` (the `FromUniqueID` hit in
  `src/engine/particle/Particle.cpp`). Once per frame, `level_c::execute()`
  clears `mStatus` on every slot that was renewed and cuts every slot that was
  not. A cut runs `cutTable` -> `emitter_c::cleanup`, which restores the
  definition's callback word, sets bit 0 of the emitter's `callbackFlags` and
  frees the slot. The effect lasts as long as the actor keeps asking, and stops
  the first frame it does not. Examples: waterfall mist, Bullet Bill smoke, the
  1-Up trail, the tornado, weather.
- **Fire-and-forget, `NewSimple(effect, x, y, z)`.** Goes straight to
  `Particle::Manager::AddSystem` with no slot. The effect runs for the lifetime
  its definition carries and ends by itself. Examples: break puffs, the item
  vanish sparkle, landing dust.

The ROM's SPA table agrees. Each definition has an emitter lifetime, where 0
means "emit forever" (`tools/spa_dump.py`, `emitterLifetime`). The table below
crosses that lifetime with the spawn style of every literal effect id that
`tools/particle_id_sites.py` finds in `src/`. "System::New" includes the
`New*` callback wrappers.

| ROM emitter lifetime | Spawned with | IDs |
|---|---|---|
| forever (140 in the table) | `System::New` only | 94 |
| timed (181 in the table) | `NewSimple` only | 134 |
| timed | `System::New` only | 28 |
| timed | both | 4 |

No forever effect is ever passed to `NewSimple`: an untracked emitter with no
end would never stop. The other 46 forever ids have no literal site, so they
are probably reached through computed ids.

The 32 timed ids that go through `System::New` are not kept alive in practice.
26 of them have a 1-frame emitter: one-burst hits and sparks (`kPtclAttackHit*`,
`kPtclWarioAttackHit*`, `kPtclTongueHit*`, `kPtclCannonShot*`,
`kPtclMgSnowballHit*`). Five go through a water callback wrapper
(`kPtclMegaWaterEntry*`, `kPtclMegaSplash`, `kPtclHeavyStepRipple`,
`kPtclShallowWaterLand`). `NewSimple` has no direction or callback parameter,
which is the likely reason these use `New`, but that has not been checked
call by call.

The per-id lifetime ("forever" / "emitter Nf") is the last column of
`notes/particle-ids.md`.

Open: what `callbackFlags` bit 0 does to the emitter. The likely reading is
"stop emitting and let the live particles fade" rather than "delete now". That
is a reading of the code and has not been watched in melonDS, so don't name the
bit until it has.

Reproduce the counts (cwd = repo root):

```python
import sys, collections; sys.path.insert(0, 'tools')
import spa_dump as S, particle_id_sites as P
t = S.parse(open('extracted/arm9_dec.bin', 'rb').read())
forever = {d['id']: d['decoded']['emitterLifetime'] == 0 for d in t['definitions']}
by = collections.defaultdict(set)
for s in P.scan('src'):
    by[s['id']].add('simple' if s['via'] == 'NewSimple' else 'tracked')
print(collections.Counter(('forever' if forever[i] else 'timed', '+'.join(sorted(v)))
                          for i, v in by.items() if i in forever))
```

## Renames, by evidence tier

Done (branch `names/particle-dpa-level`):

- `Particle::SysTracker::Contents` -> `dPa_c::level_c` (ROM scope + algorithm)
- `Contents::Entry` -> `dPa_c::level_c::emitter_c`, its methods and fields ->
  TP's names (`get`, `entry`, `addTable`, `cutTable`, `execute`, `cleanup`,
  `mFreeNo`, `mId`, `mNameId`, `mStatus`, `mEmitter`, `mCallback`, `mPre`, `mNext`)
- `Particle::SysTracker` -> `dPa_c` (`include/Particle__SysTracker.h` folded
  into `include/dPa_c.h`). It owns the `level_c` at +8 and the bank of
  `level_c` callbacks at +0x750, and its constructor, destructor, `Initialise`
  and `Update` are in the same TU. The scenes' embedded instance is
  `mParticle` (TP's `dComIfG_play_c::mParticle`), was `mSysTracker` /
  `mParticles`. `Initialise` and `Update` keep their names: nothing ties them
  to TP's `createCommon` / `calc3D` beyond role.
- `System::New`'s last parameter was the coined, never-defined
  `Particle::Callback *`; it is now `dPa_c::level_c::callback_c *`, the type
  `level_c::entry` stores it as (the ROM RTTI base every callback derives
  from). The casts in the wrappers and at the call sites went with it.
- The `Particle.cpp` wrappers and the `dPa_c` callback bank they feed are
  named for what each one passes: the fixed-effect ones for their effect
  (`NewSplash`, `NewBubble`, `NewRedFire`, `NewFlamethrowerRedA`, ...,
  `MegaRunningDustAt`, members `mSplashSystemID`/`mSplashCallback`, ...),
  the caller-effect ones for their ROM-RTTI callback class
  (`NewCheckWater`, `NewCheckWaterRipple`, `NewFitWater`, `NewCheckYogan`,
  `NewCleanParticle`). They were `func_020226fc`..`func_02022d44` and
  `NewUnkCallback818`. The table is in notes/system-provenance.md. These are
  descriptive, not recovered: TP's equivalents are `dPa_control_c::set*`
  members, not names we can borrow one for one.

Weaker (TP/TWW naming only; no structural proof of the name):

- `System::New(id, effect, x, y, z, dir, cb)` returning the level id ~ TP
  `dPa_control_c::set(u32 key, ...)`; `NewSimple` ~ `setSimple`;
  `FromUniqueID` ~ `getEmitter(u32)`. In the original these were almost
  certainly `dPa_c` members (same TU), not statics of the emitter class.
- The callbacks' two virtuals (`SpawnParticles`, `OnUpdate`) are community
  names; TP's `dPa_levelEcallBack` has `setup`/`execute`, but the mapping is
  not proven.

Effect IDs: the ROM carries no names (TP's come from JPA resource strings;
SM64DS's SPA has none). `include/ParticleIds.h` names 275 of the 321 IDs for
the code that spawns them; `notes/particle-ids.md` has the evidence per ID and
the ones left bare.
