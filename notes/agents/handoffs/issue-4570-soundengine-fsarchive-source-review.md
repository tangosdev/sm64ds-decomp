# Source review handoff: SoundEngine FSArchive view

- Task: `issue-4570-soundengine-fsarchive-shadow-1103`, issue [#4570](https://github.com/tangosdev/sm64ds-decomp/issues/4570).
- Producer session: `codex-soundengine-fsarchive-1103a`.
- Tested base: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a` (`origin/main`).
- Scope: `src/engine/sound/SoundEngine.cpp`; no shared-header or TU-manifest edits.

## Source finding

The TU had a file-scope `FSArchive` shadow containing only `char opaque[0x50]`,
plus local copies of the archive read/write callback typedefs. The shared
`include/private/fs_views.h` defines the same SDK `FSArchive`, with an explicit
0x50-byte size assertion and field offsets used by the filesystem TUs. In
SoundEngine, the archive is embedded at offset zero in local `S82` and passed to
`FS_RegisterArchiveName` and `FS_LoadArchive`; those calls do not inspect the
archive fields here. `S82`'s following NARC pointers remain local views.

The change includes `private/fs_views.h`, removes the opaque duplicate and its
callback typedefs, and uses the existing shared declarations. The shared record
has no constructor or destructor, so this change adds no object lifecycle calls.

## Verification

- Before-edit control: `tubuild.py verify arm9/SoundEngine --no-write` — 138/138
  functions matched; object isolation, relocation destinations, and emission
  order clean.
- Candidate: same command — 138/138 matched; object isolation, relocation
  destinations, and emission order clean. The TU manifest required no local
  label changes.
- `prepush_linkcheck.py --files src/engine/sound/SoundEngine.cpp` — 138
  verified, zero warnings, zero blockers.
- `rombuild.py -j16 --no-rom` — PASS: 106/106 modules exact, 11,395/11,395
  source functions reproduce, zero mismatches; intact-TU gates pass; 2,200 data
  symbols verified, 690 partial, zero differing, zero unnamed.
- `check_decl_agreement.py --changed origin/main` — no new declaration
  disagreements or local redeclarations.
- `source_style.py` — one file, zero findings or deferrals.
- `check_dead_references.py` — no new dead references or broken Markdown links.
- `port_refcheck.py` — 217 references resolve.
- `pad_layout_audit.py` — header shadows decrease from 28 to 27.
- `git diff --check` — clean.

## Review boundary

The full shared `FSArchive` layout is grounded in the FS implementation's
field reads. This edit makes the SoundEngine view agree with that shared type;
it does not recover the original TU boundary or identify the names of `S82`'s
remaining fields. Independent source review and hosted PR validation remain
pending.
