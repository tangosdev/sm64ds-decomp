# Lane KLEPTO_BD10 -- DELIVERY

func_ov062_0211bd10, Klepto's e17c MAIN state handler (the per-frame cap-steal
logic, ov062, size 0x508), is decompiled to a byte-matching translation unit
and seated in the port in place of the inert-return stub.

* Worktree `C:\tmp\klepto-decode`, branch `port/klepto-bd10` off the cons tip
  `2d4f3104e`
* Branch tip: see the report; this file is the last commit
* Under `src/` exactly one file is new: `src/func_ov062_0211bd10.cpp`. No
  header, no `config/` file and no other matched source was touched

---

## 1. What was wrong

`port/unmatched/Klepto_StateDispatch.cpp` seated nine matched handlers over
Klepto's ten state-template records and an inert-return stub over the tenth,
0x0211bd10. e17c is the state a default (cap-stealing) Klepto enters from
InitResources and never leaves on its own, so its MAIN half is the one that
runs every frame: on the port the bird hovered at its spawn point, never flew
its path, never watched the player and never dove. The behaviour gap (no cap
steal) was logged once and left.

## 2. The decompilation

`src/func_ov062_0211bd10.cpp`, `//cpp`, the same extern "C" free-function
shape the sibling handlers use (func_ov062_0211c2f4.cpp is the closest
relative: same PathPtr shadow, same helpers, same offset spelling). What the
body does, in order:

1. While the post-steal cooldown (+0x444) runs: turn toward the home point
   (+0x484) or, on a wall, 90 degrees off the current heading; ease pitch and
   roll; when within 500 units of home clear the cooldown, else keep it at
   30; drive the velocity from the heading (func_ov062_0211b2fc) and return.
2. Otherwise compute the pitch to the home point (nothing held) or to the
   current path node (+0x430, something held) and a third of the yaw error
   (AngleDiff / 3); ease pitch (ApproachAngle on SSL, ApproachLinear
   elsewhere) and roll; yaw by 0x200 on SSL or when carrying a carried item,
   0x400 otherwise.
3. Nothing held and the player still has his cap: find the closest player;
   off SSL refuse if he is more than 40 units above the bird or +0x706 is set;
   if he is within 1000 units (SSL) or 700 units (elsewhere) horizontally and
   neither +0x709 nor +0x6fb is set, enter the dive state (e18c). Drive the
   velocity and return.
4. Something held: the flap/glide animation state machine on +0x43c (SetAnim
   on the two BCA files at data_ov062_0211e104 / e114, the +0x440 count-down
   on SSL and the two 0x18/0x19 levels).
5. Off the carried-item levels, with something held, re-pick the path node
   (func_ov062_0211b3ac) and cancel the +0x100 timer when it changes.
6. Drive the velocity; when both timers are 0 and the bird faces its current
   path node to within 0x2000, enter the path-node state (e15c).

Every offset it reads is one the nine matched siblings already read under the
same meaning; no new field was invented.

### Match evidence

    python tools/match.py --c src/func_ov062_0211bd10.cpp --func func_ov062_0211bd10
        --addr 0x0211bd10 --size 0x508 --bin extracted/dsd/arm9_overlays/ov062.bin
        --base 0x02115ee0 --module ov062 --strict-relocs --cpp-check
    === mwccarm 2004/b56 ===
    MATCHING VERSIONS: 2004/b56

Strict relocs ran (no "reloc-destination check unavailable" line); 36 reloc
slots, every destination the one config/arm9/overlays/ov062/relocs.txt
records. Then the pre-push hook's own gate, which links the object and
compares the linked bytes to the ROM:

    python tools/prepush_linkcheck.py --files src/func_ov062_0211bd10.cpp
    [OK  ] func_ov062_0211bd10                          VERIFIED
    prepush-linkcheck: 1 checked - 1 verified, 0 warning(s), 0 blocking

Two source forms were tried on the way. The first draft declared `PathPtr
path;` at function scope, which made mwcc emit the constructor call at entry
(the ROM constructs it inside the final `if`), and copied the player position
through `*(Vector3 *)`, which loads through immediate offsets where the ROM
takes the address first: 4 bytes long. Moving the PathPtr into the block and
copying through a `Vector3 *` matched. The SetAnim call was first spelled as
a local `BlendModelAnim::SetAnim(BCA_File &, int, int, int, unsigned short)`
member (the b880 / bc54 / b930 spelling): it byte-matches but the mangled
name it emits is `..iiit`, not the ROM's `..ii5Fix12IiEt`, so linkcheck
reported BLIND-2 as it does for those three siblings. A `Fix12<int>` template
parameter reproduces the ROM name but changes the frame (0x30, not 0x28), so
the call is spelled by its ROM name under extern "C", the InitResources.cpp
convention, and the link check verifies every slot.

No NONMATCHING banner, no asm, no pragma.

## 3. The port change

| file | what |
|---|---|
| `port/slice_klepto.txt` | `src/func_ov062_0211bd10.cpp` joins the ten handlers; `src/func_ov062_0211b2fc.c` (the velocity-from-heading helper bd10 and ba84 both call) is listed explicitly. Banner updated. |
| `port/unmatched/Klepto_StateDispatch.cpp` | `klepto_bd10_unmatched` deleted; the seat row for 0x0211bd10 points at `func_ov062_0211bd10`. Banner rewritten: all ten handlers are matched src. |
| `port/hal/actor_classes_koopa_chuckya.cpp` | `SM64DS_KLEPTO_PROBE=1`: one line per Behavior tick with the position, the seated state descriptor (e14c..e18c), the held-actor id, the two timers, the carried-item word and the save's lost-cap flag. Off by default, prints nothing. |

## 4. How it was proven

PROOF_PLACEHOLDER

## 5. What is NOT proven

GAPS_PLACEHOLDER
