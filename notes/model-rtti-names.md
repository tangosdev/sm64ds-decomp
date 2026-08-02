# The model/animation family's original class names

Recovered from the ROM's RTTI, not inferred. Unlike the Actor tables (`notes/actor-vtables.md`),
this corner of arm9 *does* carry `type_info` records — the model and animation classes were
compiled with RTTI on, so the ROM literally stores the original class names and the inheritance
edges between them.

Every row below is derived by following pointers, never by reading a string that happens to sit
next to a vtable. **Adjacency is not ownership**: the name strings live in their own pool at
0x0208e7ac-0x0208e8ff, physically interleaved with the vtables, so a string immediately after a
vtable usually belongs to some *other* class. `_ZTV15MaterialChanger` is followed by
`"14dExtAnmModel_c"`, which is **ModelAnim's** name, not MaterialChanger's.

## How to read a record

    vtable V     ->  typeinfo ptr at V-4
    typeinfo T   ->  [0] type_info vtable   (which kind of record this is)
                     [4] name pointer       (length-prefixed, Itanium style)
                     [8] base / base count  (kind-dependent)

Three `type_info` vtables appear, and the one a record points at tells you its shape:

| type_info vtable | kind | record |
|---|---|---|
| `0x0209a774` | `__class_type_info` | 8 bytes, no base — a root class |
| `0x0209a764` | `__si_class_type_info` | 12 bytes, one base at `[8]` |
| `0x0209a754` | `__vmi_class_type_info` | `[8]`=flags, `[12]`=base count, then `{base, offset<<8\|flags}` pairs |

## The names

| class in tree | vtable | typeinfo | original name |
|---|---|---|---|
| *(unnamed base)* | — | `0x0208e73c` | `dExtModel_c` |
| `Model` | `_ZTV5Model` @0x0208e90c | `0x0208e794` | `dExtSimpleModel_c` |
| `ShadowModel` | `_ZTV11ShadowModel` @0x0208e868 | `0x0208e770` | `dExtShadowModel_c` |
| `CommonModel` | `_ZTV11CommonModel` @0x0208e8a4 | `0x0208e7a0` | `dExtCommonModel_c` |
| `Animation` | `_ZTV9Animation` @0x0208e7e4 | `0x0208e744` | `dExtFrameCtrl_c` |
| `TextureTransformer` | `_ZTV18TextureTransformer` @0x0208e7c4 | `0x0208e74c` | `dExtAnmTexSRT_c` |
| `TextureSequence` | `_ZTV15TextureSequence` @0x0208e7d4 | `0x0208e758` | `dExtAnmTexPat_c` |
| `MaterialChanger` | `_ZTV15MaterialChanger` @0x0208e7f4 | `0x0208e764` | `dExtAnmMaterial_c` |
| `ModelAnim` | `_ZTV9ModelAnim` @0x0208e980 | `0x0208e924` | `dExtAnmModel_c` |
| `ModelAnim2` | `_ZTV10ModelAnim2` @0x0208e9b4 | `0x0208e788` | `dExtAnm2Model_c` |
| `BlendModelAnim` | `_ZTV14BlendModelAnim` @0x0208e94c | `0x0208e77c` | `dExtBlendAnmModel_c` |

`dExtModel_c` has a root record of its own and three children, but no vtable is attributed to it
in `symbols.txt` yet — it is the shared model base that `Model`, `ShadowModel` and `CommonModel`
all descend from, still un-promoted in the tree.

## The hierarchy, as the ROM states it

    dExtModel_c                                    (root, __class_type_info)
    |-- dExtSimpleModel_c      Model
    |-- dExtShadowModel_c      ShadowModel
    `-- dExtCommonModel_c      CommonModel

    dExtFrameCtrl_c            Animation           (root, __class_type_info)
    |-- dExtAnmTexSRT_c        TextureTransformer
    |-- dExtAnmTexPat_c        TextureSequence
    `-- dExtAnmMaterial_c      MaterialChanger

    dExtAnmModel_c             ModelAnim           (__vmi_class_type_info, 2 bases)
    |   base[0] dExtSimpleModel_c  offset 0x00
    |   base[1] dExtFrameCtrl_c    offset 0x50
    |-- dExtBlendAnmModel_c    BlendModelAnim
    `-- dExtAnm2Model_c        ModelAnim2

Two things fall out of this that the source already relies on:

* `Animation` is a **root** — its record is the 8-byte `__class_type_info` kind, so it has no base
  class. The three controllers are `__si_class_type_info` with base `dExtFrameCtrl_c`, which is the
  ROM's own statement that they derive from `Animation`.
* `ModelAnim`'s second base sits at **offset 0x50**, matching the `_ZThn80_` thunks exactly
  (80 = 0x50). That is why `ModelAnim`, `ModelAnim2` and `BlendModelAnim` reach `Animation`
  through thunk vtables rather than inheriting its slots directly.

## Why this pins Animation's vtable at two slots

`Animation` is declared with a destructor and nothing else — no third virtual. The evidence is
structural rather than a judgement call:

    _ZTV9Animation        @0x0208e7e4  [~Animation D1, ~Animation D0]
                          @0x0208e7ec  0x00000000  <- MaterialChanger's offset-to-top
                          @0x0208e7f0  0x0208e764  <- MaterialChanger's typeinfo
    _ZTV15MaterialChanger @0x0208e7f4  [D1, D0]

The zero after Animation's destructor pair is not an empty slot; it is the `offset-to-top` word of
the vtable that follows. Reading it as a pure-virtual slot is the mistake this layout invites.

The decisive check is the secondary-base tables. A secondary-base vtable must mirror the primary's
slot count exactly, and all three are two slots wide:

| thunk vtable | prefix | slots |
|---|---|---|
| `VTable_Animation_BlendModelAnimThunk` @0x0208e970 | `[-0x50, dExtBlendAnmModel_c]` | 2 |
| `VTable_Animation_ModelAnimThunk` @0x0208e9a4 | `[-0x50, dExtAnmModel_c]` | 2 |
| `VTable_Animation_ModelAnim2Thunk` @0x0208e9d8 | `[-0x50, dExtAnm2Model_c]` | 2 |

So `Update(ModelComponents &)` on the controller family is a plain method, not an override of an
inherited virtual.

## Using these names

They are ground truth for *semantics*, not a rename mandate. `dExtFrameCtrl_c` confirms that
`Animation` is a frame controller — a playback cursor, not the animation data — which is what the
class already does. Future naming work can cite this file instead of re-deriving the pointers.

Reproduce any row with:

    python -c "import struct; d=open('extracted/arm9_dec.bin','rb').read(); \
      rd=lambda v: struct.unpack_from('<I',d,v-0x02004000)[0]; \
      t=rd(0x0208e7e4-4); n=rd(t+4); o=n-0x02004000; \
      print(hex(t), d[o:d.index(b'\0',o)])"
