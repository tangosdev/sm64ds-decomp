# Domain buckets under `src/`

Status: active migration policy, 2026-08-27.

The production tree is organized for readers by runtime/game/UI domain.  This is a
navigation policy, not a claim that the recovered names reproduce Nintendo's original
directory names.  Original compiler-input ownership is proven separately through the
translation-unit manifests and production enrollment.

## Target tree

```text
src/
  runtime/
    memory/
    math/
    graphics/
      fader/
    audio/
    filesystem/
  game/
    player/
    camera/
    actors/
    objects/
    stages/
  ui/
    hud/
    menus/
    messages/
  minigames/
  unnamed/
    <module>/
      <address-band>/
```

Named class sources get a class directory below their domain so GitHub presents a
useful method list.  A reconstructed TU that owns several classes stays at the domain
root; a TU is an original compiler input, not necessarily a class.

Address-named sources remain under `unnamed/` because no semantic domain has yet been
proved.  Their module is ROM evidence.  The four-hex-digit address band below the module
is only a browse shard; it prevents arm9 and ov006 from becoming another oversized
GitHub directory and carries no source-ownership claim.

## Classification order

1. Player, camera, scene/stage, and minigame roles take their specific game domain.
2. A class in the ROM actor spawn table belongs under `game/actors/`.
3. Non-actor classes are classified by demonstrated function: memory, math, graphics,
   audio, filesystem, HUD, menus, or messages.
4. Other named classes default to `game/actors/`; unowned free functions default to
   `game/objects/` until stronger evidence supports a narrower home.
5. Address-named functions never receive a semantic classification by guesswork.

The `d` prefix alone does not mean actor.  The Fader RTTI family is the useful counter-
example: it is instantiated by scene-transition code and belongs to
`runtime/graphics/fader/`, not `game/actors/`.  Actor membership comes from the spawn
table rather than spelling.

## Migration and verification

`tools/migrate_src_domains.py` plans the complete rename-only migration and rewrites
tracked literal path references.  Run it without `--apply` first.  The migration does
not change source text, language mode, enrollment ranges, or the matched/converted
credit model.

After any path batch, verify at least:

```text
python tools/enroll.py --dry-run
python tools/port_refcheck.py
python tools/check_references.py
python tools/rombuild.py -j 16
python tools/prepush_attribution.py --base origin/main --head HEAD
```

Keep path-only migration commits separate from C++ rewrites and TU promotions.  A real
C++ TU becomes production-owned only after its exact emitted text, data, RTTI, vtables,
helpers, relocations, section order, modules, and stock ROM have all passed the existing
promotion gates.
