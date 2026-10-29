# The model/animation family's original class names

Recovered from the ROM's RTTI, not inferred. Unlike the Actor tables (`notes/actor-vtables.md`),
this corner of arm9 *does* carry `type_info` records — the model and animation classes were
compiled with RTTI on, so the ROM literally stores the original class names and the inheritance
edges between them.

Every row below is derived by following pointers, never by reading a string that happens to sit
next to a vtable. **Adjacency is not ownership**: the name strings live in their own pool at
0x0208e7ac-0x0208e8ff, physically interleaved with the vtables, so a string immediately after a
vtable usually belongs to some *other* class. `_ZTV17dExtAnmMaterial_c` is followed by
`"14dExtAnmModel_c"`, which is **dExtAnmModel_c's** name, not dExtAnmMaterial_c's.

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
| `dExtModel_c` | `_ZTV11dExtModel_c` @0x0208e87c | `0x0208e73c` | `dExtModel_c` |
| `dExtSimpleModel_c` | `_ZTV17dExtSimpleModel_c` @0x0208e90c | `0x0208e794` | `dExtSimpleModel_c` |
| `dExtShadowModel_c` | `_ZTV17dExtShadowModel_c` @0x0208e868 | `0x0208e770` | `dExtShadowModel_c` |
| `dExtCommonModel_c` | `_ZTV17dExtCommonModel_c` @0x0208e8a4 | `0x0208e7a0` | `dExtCommonModel_c` |
| `dExtFrameCtrl_c` | `_ZTV15dExtFrameCtrl_c` @0x0208e7e4 | `0x0208e744` | `dExtFrameCtrl_c` |
| `dExtAnmTexSRT_c` | `_ZTV15dExtAnmTexSRT_c` @0x0208e7c4 | `0x0208e74c` | `dExtAnmTexSRT_c` |
| `dExtAnmTexPat_c` | `_ZTV15dExtAnmTexPat_c` @0x0208e7d4 | `0x0208e758` | `dExtAnmTexPat_c` |
| `dExtAnmMaterial_c` | `_ZTV17dExtAnmMaterial_c` @0x0208e7f4 | `0x0208e764` | `dExtAnmMaterial_c` |
| `dExtAnmModel_c` | `_ZTV14dExtAnmModel_c` @0x0208e980 | `0x0208e924` | `dExtAnmModel_c` |
| `dExtAnm2Model_c` | `_ZTV15dExtAnm2Model_c` @0x0208e9b4 | `0x0208e788` | `dExtAnm2Model_c` |
| `dExtBlendAnmModel_c` | `_ZTV19dExtBlendAnmModel_c` @0x0208e94c | `0x0208e77c` | `dExtBlendAnmModel_c` |

Every class in the table now carries its cartridge name. The tree's earlier
coined spellings were `ModelBase` (`dExtModel_c`), `Model` (`dExtSimpleModel_c`), `ModelAnim`
(`dExtAnmModel_c`), `ModelAnim2` (`dExtAnm2Model_c`), `BlendModelAnim` (`dExtBlendAnmModel_c`),
`ShadowModel`, `CommonModel`, `Animation` (`dExtFrameCtrl_c`), `TextureTransformer`
(`dExtAnmTexSRT_c`), `TextureSequence` (`dExtAnmTexPat_c`) and `MaterialChanger`
(`dExtAnmMaterial_c`). Each vtable's typeinfo word (V-4) points at the record in the same row;
every offset-to-top word is 0.

## The hierarchy, as the ROM states it

    dExtModel_c                                    (root, __class_type_info)
    |-- dExtSimpleModel_c                          (__si_class_type_info)
    |-- dExtShadowModel_c
    `-- dExtCommonModel_c

    dExtFrameCtrl_c                                (root, __class_type_info)
    |-- dExtAnmTexSRT_c                            (__si_class_type_info)
    |-- dExtAnmTexPat_c                            (__si_class_type_info)
    `-- dExtAnmMaterial_c                          (__si_class_type_info)

    dExtAnmModel_c                                 (__vmi_class_type_info, 2 bases)
    |   base[0] dExtSimpleModel_c  offset 0x00
    |   base[1] dExtFrameCtrl_c    offset 0x50
    |-- dExtBlendAnmModel_c                        (__si_class_type_info)
    `-- dExtAnm2Model_c                            (__si_class_type_info)

Two things fall out of this that the source already relies on:

* `dExtFrameCtrl_c` is a **root**: its record is the 8-byte `__class_type_info` kind, so it has
  no base class. The three controllers are `__si_class_type_info` with base `dExtFrameCtrl_c`,
  which is the ROM's own statement that they derive from it.
* `dExtAnmModel_c`'s second base sits at **offset 0x50**, matching the `_ZThn80_` thunks exactly
  (80 = 0x50). That is why `dExtAnmModel_c`, `dExtAnm2Model_c` and `dExtBlendAnmModel_c` reach
  `dExtFrameCtrl_c` through thunk vtables rather than inheriting its slots directly.

## Why this pins dExtFrameCtrl_c's vtable at two slots

`dExtFrameCtrl_c` is declared with a destructor and nothing else — no third virtual. The evidence is
structural rather than a judgement call:

    _ZTV15dExtFrameCtrl_c @0x0208e7e4  [~dExtFrameCtrl_c D1, ~dExtFrameCtrl_c D0]
                          @0x0208e7ec  0x00000000  <- dExtAnmMaterial_c's offset-to-top
                          @0x0208e7f0  0x0208e764  <- dExtAnmMaterial_c's typeinfo
    _ZTV17dExtAnmMaterial_c @0x0208e7f4  [D1, D0]

The zero after dExtFrameCtrl_c's destructor pair is not an empty slot; it is the `offset-to-top` word of
the vtable that follows. Reading it as a pure-virtual slot is the mistake this layout invites.

The decisive check is the secondary-base tables. A secondary-base vtable must mirror the primary's
slot count exactly, and all three are two slots wide:

| thunk vtable | prefix | slots |
|---|---|---|
| `VTable_dExtFrameCtrl_c_dExtBlendAnmModel_cThunk` @0x0208e970 | `[-0x50, dExtBlendAnmModel_c]` | 2 |
| `VTable_dExtFrameCtrl_c_dExtAnmModel_cThunk` @0x0208e9a4 | `[-0x50, dExtAnmModel_c]` | 2 |
| `VTable_dExtFrameCtrl_c_dExtAnm2Model_cThunk` @0x0208e9d8 | `[-0x50, dExtAnm2Model_c]` | 2 |

So `Update(ModelComponents &)` on the controller family is a plain method, not an override of an
inherited virtual.

## Using these names

They are ground truth for *semantics* as well as spelling. `dExtFrameCtrl_c` confirms that the
class is a frame controller — a playback cursor, not the animation data — which is what it
already does. The cartridge gives secondary vtables no name of their own. Each sits inside its
class's `_ZTV` group at +0x24, and dsd needs a symbol at every relocation target, so the tables keep
labels, spelled `VTable_<secondary base>_<class>Thunk` like `VTable_dBgPi_dBgCh_LinThunk` (they were
`VTable_Animation_*Thunk` before the RTTI rename). Future naming work can cite this file
instead of re-deriving the pointers.

Reproduce any row with:

    python -c "import struct; d=open('extracted/arm9_dec.bin','rb').read(); \
      rd=lambda v: struct.unpack_from('<I',d,v-0x02004000)[0]; \
      t=rd(0x0208e7e4-4); n=rd(t+4); o=n-0x02004000; \
      print(hex(t), d[o:d.index(b'\0',o)])"
