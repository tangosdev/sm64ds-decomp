# Mod API for the SM64DS PC port

* `sm64ds_mod.h`: the plugin API v1 header (code mods).
* `sample/`: a sample plugin and how to write one (`sample/README.md`).
* `README-sound.md`: replacing music and sound effects (`sound/` in a mod).
* `sdat_list.py`: lists the sound archive members a mod can replace.

Mods are switched on by settings.json "EnabledMods". Mods are for single player
only: in a lobby, a VS match, a party, or the Adventure hub (settings.json
AdventureGhosts) every mod is off for the whole run.

## Packaging note (release kit)

The kit must ship `walk_window.symtab` in the same folder as `walk_window.exe`.
The plugin loader reads game symbols from it (it is written next to the exe after
every build by `port/tools/mk_symtab.py`). Without it, a kit has no symbols for
plugins: the loader falls back to `walk_window.map`, which the kit does not ship,
and logs that no table was found. Generate it from the exe being shipped.

## Textures (`textures_hd/`)

A texture is named by 16 hex digits, the same name the game gives it when
`SM64DS_HD_TEXTURES_DUMP=<folder>` dumps every texture it draws. In a mod's
`textures_hd/` (or the HD texture pack folder) a name can be:

* `<name>.png`: a still picture, the DS size or a whole multiple of it.
* `<name>/`: a moving texture. A folder of PNG frames played in file-name
  order (`0000.png`, `0001.png`, ...), looping. An optional `anim.txt` in the
  folder sets the speed: `fps=24`. Without it the speed is 30 frames a
  second. Every frame should be the same size (a whole multiple of the DS
  texture, like a still).
* `<name>.gif`: a moving texture from an animated GIF, with the GIF's own
  frame delays (a delay of 10 ms or less plays as 100 ms, as in a browser).
  A GIF has at most 256 colours per frame, so smooth gradients band; use a
  frame folder for those.

If one folder holds more than one form of the same name, the frame folder is
used first, then the GIF, then the still. Between mods the later mod in the
list wins, whatever the form.

Moving textures run on real time: the loop plays at its own speed at any
frame rate, with Smooth Motion on or off, and keeps moving while the game is
paused. The frames are read once (in the background, when the game starts
or the mod list changes), not every level. A moving texture may hold at most
64 MB of frames (width x height x 4 bytes x frames) and all of them together
256 MB. Over 64 MB, every second (or third, ...) frame is skipped and the
loop keeps its length; over 256 MB, the texture shows its first frame only.
The game log says when either happens.

## Settings that switch a folder on or off

A files mod may declare settings in its `settings.json` (the launcher draws them
on the mod's page and writes the player's choice to `settings.values.json` next
to it). A bool or choice setting that carries a `"files"` entry switches a
folder of the mod on or off:

```json
[
  { "key": "so-long-bowser", "label": "So long, King Bowser", "type": "bool",
    "default": true, "files": "parts/so-long-bowser" },
  { "key": "quality", "label": "Quality", "type": "choice",
    "choices": ["hd", "ds"], "default": "hd", "files": "parts/quality" }
]
```

* `files` is a plain folder inside the mod (letters, digits, `-` `_` `.` and
  `/` only, no `..`). It is laid out like a mod: `files/`, `sound/`,
  `textures_hd/`, `palettes/` (single-wave `.wav` files included).
* bool: the folder is used when the value is `true`; with no value saved the
  `default` decides (no default means off).
* choice: the folder `<files>/<value>` is used.
* A part counts as a mod of its own, listed straight after its mod, so it
  wins over the mod's own files. The game log says for each such setting
  whether it was on or off and where the value came from (`[mods] <id>: setting
  ...`). The value is read once at start-up.
* A files mod that has none of `files/`, `sound/`, `textures_hd/`, `palettes/`
  (in itself or in an applied part) gets one `WARNING ... replaces nothing` line
  in the log.
