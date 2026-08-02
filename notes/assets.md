# ROM assets: names, IDs, and local editing

SM64DS stores most models, animations, collision, textures, messages, and sound
data in NitroFS. `tools/unpack.py` extracts those named files locally under
`extracted/`; `tools/rombuild.py` carries the filesystem and its `path_order.txt`
back into a built ROM.

The ROM and extracted payloads are copyrighted input, so they remain gitignored.
The tools below generate their catalogs and editable exports under `build/`, which
is also gitignored. Only the original parsers, generators, tests, and documentation
belong in Git.

## Build the local asset catalog

```powershell
python tools/asset_catalog.py generate sm64.nds
python tools/asset_catalog.py references
```

The first command writes two deliberately separate ID layers:

- `build/assets/files.tsv` and `build/generated/NitroFileId.h`: raw NitroFS IDs,
  internal paths, formats, and sizes;
- `build/assets/handles.tsv` and `build/generated/AssetHandle.h`: the 2,058
  game-code handles initialized by overlay 0, resolved to their NitroFS files.

That distinction matters. Calls such as `LoadFile(0x40a)` and the arguments used to
construct `SharedFilePtr` are runtime handles, not FAT/FNT indices. The game maps
handles below `0x8000` through the table at `data_ov000_020bd4b8`; confusing the two
number spaces gives convincing but incorrect asset names. The source-reference
report therefore resolves against `AssetHandle.h`, while `NitroFileId.h` is for
low-level filesystem work.

The second command writes `build/assets/references.tsv`. It finds literal IDs in
known file-loading calls, resolves runtime handles, and proposes names for
anonymous `SharedFilePtr` owners. Proposals are evidence for review, not automatic
renames. Large values such as `0x9807` bypass that table and are reported as
`encoded-or-unresolved` until their archive/flag semantics are proved; masking them
into a plausible path would create false names.

The generated headers are intentionally not added to the compiler path yet. Moving a
matched source from a numeric literal to one of these names should be done only when
the generated input is available to all validation environments and the function is
reverified byte-identical.

## Extract and rebuild messages

The western `data/message/msg_data_*.bin` files are LZ77-compressed, BMG-shaped
banks with a game-specific one-byte font. The runtime's matched message code shows
the control grammar: `FD` is a line break, `FE` begins a command whose second byte
is its total length, and `FF` ends the message. The character and icon map agrees
with the long-standing
[SM64DSe character table](https://github.com/Gota7/SM64DSe-Ultimate/blob/master/assets/basic_eur_us_chars.txt).

```powershell
python tools/message_bank.py extract `
  extracted/dsd/files/data/message/msg_data_eng.bin `
  -o build/messages/eng.json

python tools/message_bank.py build build/messages/eng.json `
  -o build/messages/msg_data_eng.bin

python tools/message_bank.py roundtrip `
  extracted/dsd/files/data/message/msg_data_eng.bin
```

The JSON uses normal readable text where the byte mapping is unambiguous. The
following tokens make every other byte reversible:

- `<icon:A>` and related names represent multi-byte controller glyphs;
- `<cmd:FE05030002>` contains a complete length-prefixed runtime command;
- `<glyph:D0>` preserves an unknown or ambiguous font byte.

The export also keeps the original compressed stream. If no text changes, `build`
returns it byte-for-byte. If text changes, the tool updates all INF1 offsets and
section sizes, pads the bank like retail, and emits valid LZ10. An edited ROM is not
expected to match retail; the exact path is there to prove that extraction itself is
non-destructive.

Japanese banks use a different multi-byte character map and are not accepted as
readable text by this first tool version. Add that map explicitly before using this
editor on a Japanese ROM; do not guess it from western data.
