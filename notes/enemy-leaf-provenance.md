# dEnemyBase_c leaf classes, set B — field provenance

Every name below is taken from what the matched bodies in `src/` actually do with
the offset. Where the bodies only *write* a field and nothing in the tree reads it
back, the field stays `unk_NNN` and the reason is recorded — a name nobody can
check is a claim the next reader would trust for nothing.

Classes covered here: JetStream, Goomboss, ChainChomp, BobOmb, Whomp,
RollingIronBall, KoopaShell, Klepto, Rabbit, ChiefChilly.

A recurring source of `unk_` in these headers is the `#else` C twin, which
restates the whole `fBase_c -> dBase_c -> dActor_c -> dEnemyBase_c` chain flat.
Those names are not independent claims: they are copied from the base headers that
own the offsets (`include/fBase_c.h`, `include/dActor_c.h`), and the `#ifdef
__cplusplus` branch of the same header inherits them properly. They are listed
once, here, rather than per class.

| offset | name | owner |
| --- | --- | --- |
| 0x008 | `param1` | `include/fBase_c.h` |
| 0x08c..0x096 | `mAngleX/Y/Z`, `mPrevAngleX/Y/Z` | `include/dActor_c.h` |
| 0x0b4 | `mClipOffsetY` | `include/dActor_c.h` |
| 0x0b8 | `mClipRadius` | `include/dActor_c.h` |
| 0x0bc | `mClipDistance` | `include/dActor_c.h` |
| 0x0c0 | `mFarDistance` | `include/dActor_c.h` |
| 0x0c4 | `mClipResult` | `include/dActor_c.h` |
| 0x0ce | `mDeathTableID` | `include/dActor_c.h` |

---

## JetStream (`include/JetStream.h`, ov064)

| offset | name | evidence |
| --- | --- | --- |
| 0x300 | `mState` (`State *`) | `src/_ZN9JetStream8BehaviorEv.cpp` loads the pointer word at 0x300 every frame, tests the word at `+0x08` of what it points at, and if non-zero calls it as a pointer-to-member on `this`. That is the same object `Bullet::State` describes (`include/Bullet.h`, handler at +0x08) and the same `mState` spelling `Chuckya`, `ChiefChilly` and `daBakubaku_c` already use for it. Only `+0x08` is evidenced; the first two words stay padding. |

Left `unk_`:

* **0x314, 0x318** — `src/_ZN9JetStream13InitResourcesEv.cpp` decodes both out of
  the spawn word: `unk_314 = (param1 >> 12) & 0xf`, `unk_318 = param1 & 1` and then
  `if ((param1 & 0xf) > 1) unk_318 = 0`. Nothing in the tree reads either back, so
  the bytes say how they are *built* but nothing about what they *select*. A name
  here would be invention.

Byte-neutral cleanups made in the same pass (each re-verified with
`build_pin.verify`, `2004/b56`):

* `Behavior` — dropped the local `struct C { char pad[0x300]; Obj* obj; }`
  stand-in in favour of the declared `mState`; collapsed
  `*(short*)((char*)&mAngleX) = *(short*)((char*)&mPrevAngleX)` (and Y, Z) to plain
  member assignments; `(unsigned short*)((char*)&(*(u8 *)&unk_100))` to
  `(unsigned short*)&unk_100`; `func_ov064_0211987c(((C*)this))` to `(this)`.
* `InitResources` — `(dCcAc_c*)((char*)&(*(u8 *)&mdCcAc_c))` to `&mdCcAc_c`,
  `(dActor_c*)((char*)this)` to `(dActor_c*)this`.
