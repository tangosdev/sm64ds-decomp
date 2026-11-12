# Test scenarios

One command starts the game already set up for a test: solo, or a coop room
with every player already in it. Each player starts at his own spot, as his own
character, with his power-up and save state set through the game's own
functions. This is a developer tool. With no `SM64DS_SCENARIO` knob the game
behaves exactly as before.

```sh
python port/tools/scenario/run_scenario.py --list
python port/tools/scenario/run_scenario.py klepto_hats --exe <walk_window.exe> --aroot <asset root> --out <run folder>
python port/tools/scenario/run_scenario.py klepto_hats --exe ... --aroot ... --out ... --mode proof
python port/tools/scenario/run_scenario.py --stop klepto_hats --out <run folder>
python port/tools/scenario/run_scenario.py klepto_hats --print            # show the file the game reads
```

`--aroot` falls back to `SM64DS_ASSET_ROOT`, `--out` to `SCENARIO_OUT`.
`--env K=V` (repeatable) adds an environment knob to every game.

## The two modes

**play** (default): windows visible, sound on, shown without taking focus.
The script starts the relay (coop) and the games, writes `pids.json` in the
run folder and returns. The games run until you close them or run `--stop`.

**proof**: for agents and unattended rows. Games are silent, muted, minimized
and never focused (`SM64DS_NO_FOCUS`, `SM64DS_VOLUME=0`, `SM64DS_MINIMIZED`,
`SM64DS_NO_DIALOG`), run at below-normal priority, and wait for the run-wide
row gate first (one slot for up to two games, both slots for three or four;
`--lane NAME` names the ticket, the gate script is `--rowgate` or
`SCENARIO_ROWGATE`; with neither set the row runs ungated). Each
player takes a picture from the game's own frame dump `shot_ms` after he is
placed. The row stops as soon as every picture is written (or after `secs`),
kills only the processes and relay it started, releases the gate, and prints
the `[scenario]` log lines, the fatal/fault count and the pictures (also
converted to PNG into `--pics` if given).

A coop room gets its own relay on 127.0.0.1 at a port derived from the
script's process id (kept well away from the ports people play on) and a
fresh room code, so a scenario never joins anyone else's room. Each player
gets his own folder `<out>/<scenario>/c<k>/` with his own exe copy, empty
settings and his own save.

## Scenario file (`scenarios/<name>.json`)

```json
{
  "about": "one line shown by --list",
  "mode": "coop",
  "save": {"preset": "basement", "stars": {"0": "40"}, "flags": ["rabbits_awake"]},
  "room": {"collision": 1, "damage": 1, "interact": 1, "tp": 1},
  "proof": {"secs": 150, "shot_ms": 2000, "census": [239]},
  "env": {"SM64DS_SOME_KNOB": "1"},
  "players": [
    {"character": "mario", "level": 16, "near": {"class": 239, "dist": 250}, "cap": "on"},
    {"character": "yoshi", "level": 16}
  ]
}
```

Top level:

| field | meaning |
|---|---|
| `about` | description for `--list` |
| `mode` | `solo` or `coop`; default coop for 2+ players, solo for 1 |
| `save` | save state every player starts with (below) |
| `room` | coop room rules: `collision`, `damage`, `interact`, `tp` (0 or 1) |
| `proof.secs` | proof row time limit (default 150 coop, 90 solo) |
| `proof.shot_ms` | picture delay after the player is placed (default 2000) |
| `proof.census` | actor class ids to list in the log (position, facing, param, area) |
| `env` | extra environment knobs for every game |
| `players` | one entry per player, seat 0 first (seat 0 hosts the room) |

Per player:

| field | meaning |
|---|---|
| `character` | `mario`, `luigi`, `wario`, `yoshi` |
| `level` | level id to start in (1 castle grounds, 2 castle, 4 basement, 5 upstairs, 6 Bob-omb Battlefield, 16 Shifting Sand Land, ...). Coop rooms boot on the castle grounds and the game warps there with its own level load |
| `act` | star/act to load (default 1) |
| `entrance` | entrance id inside the level (default 0) |
| `area` | area to show at the spot (doors and some rooms need it) |
| `pos` | `[x, y, z]` spot in level units |
| `drop` | with `pos`: put the player on the floor below the point |
| `yaw_deg` | facing in degrees (0 is +z) |
| `near` | spot next to an actor: `class` (id), `param` (only actors with this param), `index` (which match, default 0), `dist` (default 200), `dy` (height offset), `angle_deg` (around the actor, 0 is in front of it), `drop` (default on). The player faces the actor. The spot is retried every 30 frames while the actor is not loaded yet (`tries`, default 8) |
| `power` | `wing` (Mario), `flower` (the character's own), `balloon`, `vanish`, `metal`, `fire` |
| `power_ticks` | power-up timer in frames |
| `hat` | character whose cap the player wears (the game's own cap swap) |
| `cap` | `on`, `lost` (the save's lost-cap state) or `klepto` (Klepto takes it with his own steal) |
| `health` | health in wedges (1 to 8) |
| `lives` | life count |
| `settle` | frames to wait in the level before placing (default 45) |
| `save` | save state for this player only, merged over the top-level one |
| `env` | extra environment knobs for this player only |

Save state (`save`, top level or per player):

| field | meaning |
|---|---|
| `preset` | starting point, see below |
| `star_count` | total stars, filled course by course seven at a time |
| `stars` | exact star bits per course: `{"<course>": "<hex bits>"}`, course 0 is Bob-omb Battlefield |
| `flags` | named save bits, see below |
| `characters` | unlocked characters (`luigi`, `wario`, `yoshi`) |
| `cannons` | course numbers with an open cannon |

Stars go through the game's own `CollectStar`, cannons through `OpenCannon`,
characters through `SetCharacterIntro`. The keys, doors and other events have
no setter function in the game (it sets the bit in place), so the scenario
sets the same bit the game's own code sets.

Presets:

| preset | what it holds |
|---|---|
| `fresh` | a new file |
| `early` | 1 star, one-time messages already seen |
| `basement` | 10 stars, Luigi and Wario, basement key used on its door |
| `upstairs` | 30 stars, all characters, both keys and doors, moat drained |
| `all_open` | 105 stars, everything above, rabbits awake, every cannon open |

Named flags: `basement_key`, `basement_door`, `upstairs_key`,
`upstairs_door`, `moat_drained`, `rabbits_awake`, `messages_seen` (the
castle welcome line and the power-up hints; a course's own entry text shows
only while the course has no star, so give it one).

Lists from a preset and the scenario are merged; single values from the
scenario win. In coop every console applies the same preset, since coop save
bits are merged between consoles.

## The game knob

`SM64DS_SCENARIO=<file>#<player>` points the game at the plain text file the
launcher writes (`--print` shows it): an `[all]` section, then one `[p<N>]`
section per player, `key = value` lines. The game reads it in
`port/hal/scenario.cpp`, logs every step with a `[scenario]` prefix, and
refuses to touch the save unless the save is a scenario's own
(`SM64DS_SAVE_PATH` solo, `SM64DS_COOP_SAVE` coop), so a real save file is
never changed.
