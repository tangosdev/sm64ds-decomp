# System-class field provenance

Evidence tables for the classes that are *not* actors: the particle engine's
tracker, the minimap, the level scene, and the BMD model file. An actor is
documented by its own `InitResources` / `Behavior` / `Render`; these four are
driven from call sites scattered across the whole tree, so each row below cites
the file that proves it rather than a single owning source.

Every name here is byte-neutral: it renames a field, it does not change a type
that the compiler can see a width for. Where the evidence proves only a *kind*
and not a *role*, the field keeps its offset in the name (`mCallback_7d4`) so
the name does not overclaim.

---

## Particle::SysTracker — `include/Particle.h`, and the copy inside `include/Stage.h`

The struct is spelled `Particle` in `include/Particle.h` for historical
reasons; the ROM's mangled names call it `Particle::SysTracker`. There is one
instance and its address lives in the global `data_0209ee74`:
`_ZN8Particle10SysTrackerC1Ev` writes it, `_ZN8Particle10SysTrackerD1Ev`
clears it, and every free function in the namespace reaches the object through
it. That is what makes this a system class: the evidence is spread over eleven
files, none of which is a method of the class.

### The head of the object

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x000 | `mResourceFile` | `_ZN8Particle10SysTracker10InitialiseEv` stores either `data_02075f14` (the file, used in place) or a heap buffer it fills with `DecompressLZ16`, then hands it to the manager via `func_0204a17c`. `_ZN8Particle10SysTrackerD1Ev` frees it through `Memory::Deallocate` *only* when it is not `data_02075f14` — i.e. only when the tracker owns the decompressed copy. Was `pad_000[0x4]`. |
| 0x004 | `mManager` | The `Particle::Manager`. `Initialise` allocates it with `func_0204a4c8(...)` and pokes `0x8000` into its `+0x30`; `_ZN8Particle9RenderAllEv` and `_ZN8Particle10SysTracker6UpdateEv` pass it straight to the render/update entry points; `_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_` and `src/func_02021d1c.cpp` call `Particle::Manager::AddSystem` on it. Was `unk_004` typed `s32`, now `void *`. |
| 0x008 | `mContents` | A real nested class, not a guess: `_ZN8Particle6System12FromUniqueIDEj` calls `_ZNK8Particle10SysTracker8Contents8FindDataEj(this + 8, uniqueID)` — the mangled callee names `Particle::SysTracker::Contents` and puts it at `+8`. The constructor and destructor run `func_02021c90` / `func_02021b98` on the same address, and `SysTracker::Update` runs `func_02021bec`. Was `unk_008`. |

### The callback bank, 0x750 upward

`Particle::System::New(uniqueID, effectID, x, y, z, dir, callback)` is called
throughout the tree in one shape:

```c
*(u32*)(base + N) = System::New(*(u32*)(base + N), effect, x, y, z, dir, base + N + 4);
```

so slot `N` is a *last system unique ID* and `N + 4` is a `Particle::Callback`
subobject. Three call sites give both halves a role:

| ID slot | Callback | Effect | Proving file |
| --- | --- | --- | --- |
| 0x750 `mRunningSlidingDustSystemID` | 0x754 `mRunningSlidingDustCallback` | 0xda | `src/_ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_.c` |
| 0x768 `mBigSplashSystemID` | 0x76c `mBigSplashCallback` | 0xdd | `src/_ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_.c` |
| 0x78c `mRippleSystemID` | 0x790 `mRippleCallback` | 0x109 | `src/_ZN8Particle6System9NewRippleE5Fix12IiES2_S2_.c` |

Two more callbacks are named without an ID slot, because their `New*` wrapper
takes the unique ID from the caller instead:

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x810 | `mWeatherCallback` | `_ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj` builds `data_0209ee74 + 0x810` and passes it as the callback. |
| 0x814 | `mWeatherCallbackCount` | The same function writes its `numWeatherEffectsNow` argument to `callback + 4`, which is this byte; the constructor seeds it `1`. It is a member of the 0x810 object, not an independent field. |
| 0x818 | `mCallback_818` | `_ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f` passes it. Nothing names the effect, so the field does not claim one. |

The remaining slots are named only as far as the evidence goes. Two structural
facts carry them:

* **The constructor zeroes exactly ten `s32`s in the tail** — 0x750, 0x75c,
  0x768, 0x774, 0x780, 0x78c, 0x798, 0x7a4, 0x7b0, 0x7c0 — and no others. That
  is precisely the set of words sitting four bytes below a callback subobject,
  and it includes all three ID slots proven above. Those ten are the
  `mSystemID_*` fields. `0x7fc` and `0x804` are *not* in that set: the
  constructor seeds them `0x3000` and (finally) `0x4b000`, Fix12 3.0 and 75.0,
  so they are parameters of the callbacks at 0x7f8 and 0x800, and they are
  named `mCallbackParam_*`.
* **Every callback subobject is constructed by storing the base
  `Particle::Callback` vtable `data_0208f3b4` and then overwriting it with a
  derived vtable**, which is exactly what
  `_ZN8Particle14SimpleCallbackC2Ev` does. That is the "this is a Callback"
  evidence for the `mCallback_*` fields. The derived vtables also pair objects
  up: 0x76c and 0x778 both take `data_0208f3e4`, and 0x7f8 and 0x800 both take
  `data_0208f454`, so each pair is two instances of one callback class — which
  is why 0x7fc and 0x804 (their `+4` parameters) exist in the same shape.

Constructed by helper, so the class is known but not the role:
`_ZN8Particle14SimpleCallbackC2Ev` builds 0x754, 0x760, 0x79c, 0x7a8;
`func_020226a4` builds 0x76c, 0x778, 0x784, 0x790, 0x808; `func_020225fc`
builds 0x7b4, 0x7c4, 0x7d4, 0x7e4.

### Shadow structs collapsed onto the header

`data_0209ee74` was declared six different ways across the tree — `char*`,
`void*`, and three private one-off structs (`ParticleSysTracker` twice, with
different fields, and `SomeGlobal`). All but one now declare it
`struct Particle *` and reach members by name; every one was re-verified
byte-identical. The exception is
`src/_ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE.cpp`, which
opens `namespace Particle { ... }` — including `Particle.h` there makes the
struct name and the namespace name collide, so it keeps its local shadow.

`src/_ZN8Particle14SimpleCallbackC2Ev.cpp` was typed `struct Particle *self`
and reached the callback's own `s16` as `&self->unk_004`. That was never a
`SysTracker`: it is a `Particle::SimpleCallback`, and the two only agreed
because both objects start with a pointer-sized word. It now takes `char *`.

### Deliberately left alone

* `include/Particle__SysTracker.h` is a *third* shadow of this same class and is
  what `SysTracker::Initialise` and `SysTracker::Update` include. It still
  spells `unk_004` / `unk_008`. Merging the three shadows is its own change
  with its own blast radius, and this pass owns only `include/Particle.h` and
  `include/Stage.h`.
* Seven of the ten `mSystemID_*` slots and eleven of the `mCallback_*`
  subobjects keep their offset in the name. The evidence proves what they
  *are*; nothing surviving in the tree says which effect each one drives.
