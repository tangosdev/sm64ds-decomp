# tools/trace/mg — minigame play kit

Scripts for the minigame runtime checks (`notes/minigames/runtime-checks.md`):
play a minigame in melonDS while a GDB-stub watcher logs every call to the
shared minigame functions. Windows only (input and capture use user32/gdi32).

## Files
- `mgwatch.py` — attaches to the melonDS ARM9 stub (`../rsp.py`), arms one
  breakpoint per check symbol (states, banners, jingles, credits, bets, presenter
  animations, ...) and logs r0-r3/lr as JSON lines. A hit counts only when the
  8 bytes at pc match that overlay's image, since ov005 and ov006 share a load
  address. Per-frame pollers are cleared after a few hits.
  - `--unlock` patches the minigame unlock-bit read in RAM, so every minigame
    shows as unlocked. Leave and re-enter the Rec Room after it applies.
  - `build/melonds/STOP` (create it) detaches cleanly.
  - `build/melonds/PEEK`: write lines of `addr len` (both hex) and the answer
    lands in `PEEK.out`. Reads of more than about 0xc8 bytes time out; split them.
  - `build/melonds/POKE`: write lines of `addr hexbytes` to patch RAM; the old and
    new bytes land in `POKE.out`. Writing `1eff2fe1` (`bx lr`) over a draw
    function's first instruction for one screenshot shows what it draws.
  - The loop polls for STOP, PEEK and POKE every 50 ms, so a bot can read the game
    about ten times a second. A PEEK or POKE waits while a breakpoint stop is
    pending, so the two stop replies cannot cross.
- `input.ps1 -Action key|tap|drag|size` — keys and bottom-screen touches
  in DS coordinates (0..255, 0..191). `size` sets the window to 900x1180, the
  size the touch mapping was measured at.
- `shot.ps1 -Out <png> [-Delay s]` — captures the melonDS window.
- `ts.sh X Y [wait] [name]` — tap, wait, capture to `build/melonds/shots/`.
- `slotbot.py probe|play` — times the taps in Toad's face-slot game
  (`dScMgSlot3_c`) by tracking piece colours.
- `mgpeek.py [offset len] ...` — reads the live scene through `gMgScene` and
  PEEK (offsets from the scene, hex). `mgpeek.py 0 10` shows the vtable and
  `param1`, whose low byte is the variant `IsAltVariant` tests.
- `teresabot.py <scene> [seconds]` — plays Hide and Boo Seek (`dScMgTeresa_c`):
  reads the Boo records through PEEK and holds the stylus on the Boo while
  rubbing is enabled, then screenshots the find.
- `snowbot.py [seconds] [steer]` — pushes Snowball's ball up the course with
  repeated upward swipes, reading the ball through PEEK. It gets most of the way
  in 40 s; obstacles cost time, as it does not read the course.

## Setup
Everything runtime lives in `build/melonds/` (gitignored): melonDS 1.1
(Windows x86_64), a copy of the EU ROM as `sm64_eu.nds`, and its save.

In `build/melonds/melonDS.toml`:

```toml
[Instance0.Gdb]
Enabled = true            # melonDS can rewrite this to false; check before each launch

[Instance0.Gdb.ARM9]
Port = 3333
BreakOnStartup = true

[JIT]
Enable = false            # the stub needs the interpreter

[Instance0.Keyboard]      # a fresh config can leave every key at -1
A = 88                    # X
B = 90                    # Z
X = 83                    # S
Y = 65                    # A
L = 81                    # Q
R = 87                    # W
Start = 16777220          # Enter
Select = 16777219         # Backspace
Up = 16777235
Down = 16777237
Left = 16777234
Right = 16777236
```

## Run
```sh
build/melonds/melonDS.exe build/melonds/sm64_eu.nds &
python -u tools/trace/mg/mgwatch.py --unlock
powershell -File tools/trace/mg/input.ps1 -Action size
powershell -File tools/trace/mg/input.ps1 -Action tap -X 128 -Y 96 -Ms 250
```

## Gotchas
- The stub takes one client per launch. To reattach, restart melonDS.
- The title screen needs a long tap (250 ms).
- `input.ps1` taps F24 to get foreground rights. Alt would open melonDS's
  menu bar, and the next key would then drive the menu.
- Menu row of a game = tab + 4 x position on the page (Mario 0, Luigi 1,
  Wario 2, Yoshi 3); rows 24-35 are the second pages.
