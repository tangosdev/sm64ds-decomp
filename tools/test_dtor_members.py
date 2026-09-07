"""What the member-type harvester has to get right, and what already went wrong once.

`dtor_members.py` reads (owner, offset, member type) out of the cartridge: a destructor
calling a destructor, with the member's offset in r0.  The offset comes from a small
abstract interpreter, so the interpreter's blind spots are the tool's blind spots, and
the two that matter -- a `this` spilled to the stack, and an offset too large for an ARM
immediate -- are cases below, driven through synthetic ARM so they run in a checkout
with no `extracted/`.

The rest are the joins.  Every one of them has cost this project work before:

  * a NAME join instead of an address join.  A survey reported 127 classes with no
    header and 125 were RTTI aliases sharing one vtable address with a spelling the
    tree does have a file for (`opnew_sizes.resolve_header_name`, PR #1556's two
    retractions).  `test_alias_join_regression` is the guard.
  * one definition per class.  A header defines the same class twice, C++ and a flat
    C stand-in, and reading only the last block reported 15 already-typed members as
    outstanding work.
  * "the include appears in the file" as a stand-in for "the include is visible".  A
    header's C++ branch carries its own includes, textually above the C definition
    below the `#else` and invisible to it; three headers failed with
    `undefined identifier` on exactly that.
  * a bare type name in a C-reachable header.  In C the tag is not a type name, so a
    member needs `typedef struct X X;` as well as a C definition; without both, the
    header fails to compile and the size assert then reports `illegal constant
    expression` -- the shape that failed PR #1659's CI.
"""
import collections
import pathlib
import struct
import sys
import textwrap

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import dtor_members as D            # noqa: E402
import opnew_sizes as O             # noqa: E402

BASE = 0x02000000
DTOR = 0x02100000


# --------------------------------------------------------------- ARM encoders

def push(regs):
    return 0xE92D4000 | sum(1 << r for r in regs if r != 14)


def mov_reg(rd, rs):
    return 0xE1A00000 | (rd << 12) | rs


def add_imm(rd, rn, imm8, rot=0):
    return 0xE2800000 | (rn << 16) | (rd << 12) | (rot << 8) | (imm8 & 0xFF)


def add_reg(rd, rn, rm):
    return 0xE0800000 | (rn << 16) | (rd << 12) | rm


def ldr_pc(rd, disp):
    return 0xE59F0000 | (rd << 12) | (disp & 0xFFF)


def str_sp(rv, off):
    return 0xE58D0000 | (rv << 12) | (off & 0xFFF)


def ldr_sp(rd, off):
    return 0xE59D0000 | (rd << 12) | (off & 0xFFF)


def bl(at, target):
    return 0xEB000000 | (((target - (at + 8)) >> 2) & 0xFFFFFF)


BX_LR = 0xE12FFF1E


def offsets(words, extra=()):
    """r0 at every instruction of a synthetic destructor, as an offset from `this`."""
    blob = b"".join(struct.pack("<I", w) for w in list(words) + list(extra))
    img = O.Image({"m": (BASE, blob)}, {"m": [(BASE, len(blob), "_ZN1XD1Ev")]})
    flow = D.ThisFlow(img, "m", BASE, len(words) * 4).run()
    return {a: (v[1] if v and v[0] == "this" else None) for a, v in flow.items()}


# ------------------------------------------------------- the offset comes out

def test_the_offset_is_the_add_immediate_before_the_call():
    site = BASE + 8
    got = offsets([mov_reg(4, 0), add_imm(0, 4, 0x49, 15), bl(site, DTOR), BX_LR])
    assert got[site] == 0x124                          # #0x49 ror 30 == 0x124


def test_a_member_at_offset_zero_is_seen_as_zero_not_as_unknown():
    """`~Derived()` chains to its base with a bare `mov r0, r4`.  Reading that as
    "no offset" would drop every inheritance edge instead of bucketing it BASE-AT-0."""
    site = BASE + 8
    got = offsets([mov_reg(4, 0), mov_reg(0, 4), bl(site, DTOR), BX_LR])
    assert got[site] == 0


def test_this_survives_a_spill_to_the_stack():
    """mwccarm spills `this` in a destructor with enough live values.  Without stack
    slots the reload is unknown and every later member of that class is lost."""
    site = BASE + 20
    got = offsets([push([4]), str_sp(0, 4), bl(BASE + 8, DTOR),
                   ldr_sp(1, 4), add_imm(0, 1, 0x64), bl(site, DTOR), BX_LR])
    assert got[site] == 0x64


def test_an_offset_too_big_for_an_immediate_is_read_from_the_literal_pool():
    """0x9fc4 has no rotated-8-bit encoding, so mwccarm spills it to the pool and does
    `ldr r1, [pc, #k]; add r0, r4, r1`.  `Stage::~Stage` is the live case; 92 sites
    reported no offset at all until the pool was read."""
    site = BASE + 12
    words = [mov_reg(4, 0), ldr_pc(1, 0x08), add_reg(0, 4, 1), bl(site, DTOR), BX_LR]
    got = offsets(words, extra=[0x00009FC4])           # pc+8+8 == BASE+0x14
    assert got[site] == 0x9FC4


def test_a_pool_word_that_is_an_address_is_refused_not_reported():
    """The same `ldr`/`add` shape carries real addresses too.  0x02099434 as a member
    offset would be nonsense, and MAX_OFFSET is what stops it becoming a finding."""
    site = BASE + 12
    words = [mov_reg(4, 0), ldr_pc(1, 0x08), add_reg(0, 4, 1), bl(site, DTOR), BX_LR]
    got = offsets(words, extra=[0x02099434])
    assert got[site] is not None and not 0 <= got[site] < D.MAX_OFFSET


def test_a_branch_join_keeps_only_what_both_paths_agree_on():
    """`if (p) ... ;` puts a conditional branch between the add and the call.  A
    linear walk would carry the fall-through value across the join and invent an
    offset the cartridge does not prove."""
    site = BASE + 16
    words = [mov_reg(4, 0), add_imm(0, 4, 0x10),
             0x0A000000,                               # beq -> BASE+0x10, past the add
             add_imm(0, 4, 0x20), bl(site, DTOR), BX_LR]
    assert offsets(words)[site] is None                # 0x10 and 0x20 disagree

    # ...and the same join where both paths do agree still yields the offset
    words[3] = add_imm(0, 4, 0x10)
    assert offsets(words)[site] == 0x10


# ---------------------------------------------------------------- name decode

def test_a_destructor_symbol_decodes_to_its_class_and_variant():
    assert D.demangle_dtor("_ZN7daKrb_cD1Ev") == ("daKrb_c", 1)
    assert D.demangle_dtor("_ZN10dBgW_KcMbgD0Ev") == ("dBgW_KcMbg", 0)
    assert D.demangle_dtor("_ZN8dGraph_c10callback_cD2Ev") == ("dGraph_c__callback_c", 2)
    assert D.demangle_dtor("_ZN5ModelD1Ev") == ("Model", 1)
    assert D.demangle_dtor("_ZN5Model8LoadFileEv") == (None, None)


def test_the_target_module_field_is_parsed_in_all_three_spellings():
    assert D.target_modules("main") == ["arm9"]
    assert D.target_modules("overlay(2)") == ["ov002"]
    assert D.target_modules("overlays(0,4)") == ["ov000", "ov004"]


# ------------------------------------------------------------------- the joins

def test_alias_join_regression():
    """THE defect this file exists to keep out.

    One vtable address carries several `_ZTV` spellings, because a class being renamed
    keeps its old one as an alias, and usually only one of them has a header.  Joining
    on the single spelling the attribution happened to carry reported NO HEADER for 125
    classes the tree describes perfectly well.  The candidate set is every spelling at
    the address, and the class's own key wins when it has one.
    """
    hdr = {"MontyMole": {"header": "x.h"}, "daStar_c": {"header": "y.h"}}
    # the attribution carried the RTTI name; only the tree's name has a file
    assert O.resolve_header_name("daChoropu_c", ["FortressWall", "MontyMole"], hdr) \
        == ("MontyMole", "vtable alias")
    # a class with its own header is never redirected to an alias's
    assert O.resolve_header_name("daStar_c", ["PowerStar"], hdr) == ("daStar_c", "name")
    # and no alias with a header is still an honest miss, not a silent one
    assert O.resolve_header_name("Nowhere", ["AlsoNowhere"], hdr) == (None, None)


def _index(tmp, name, text):
    (tmp / "include").mkdir(parents=True, exist_ok=True)
    (tmp / "include" / name).write_text(textwrap.dedent(text), encoding="utf-8")
    return D.header_index(tmp)


def test_both_definitions_of_a_dual_header_are_read(tmp_path):
    """A header defines the class twice, C++ then a flat C stand-in.  Keeping only the
    block that came last in the file reported 15 typed members as untyped work."""
    idx = _index(tmp_path, "Wiggler.h", """\
        #ifdef __cplusplus
        struct Wiggler : dEnemyBase_c {
            dBgCh_Actr mWithMeshClsn;                      /* 0x708 */
        };
        typedef char Wiggler_size_must_be_0x8e8[sizeof(struct Wiggler) == 0x8e8 ? 1 : -1];
        #else
        struct Wiggler {
            u8  mWithMeshClsn[0x1bc];      /* 0x708 */
        };
        #endif
        """)
    at = idx["Wiggler"]["fields"][0x708]
    assert ("dBgCh_Actr", "mWithMeshClsn") in at
    assert ("u8", "mWithMeshClsn[0x1bc]") in at
    assert idx["Wiggler"]["assert"] == 0x8E8


def test_an_offset_comment_may_carry_prose_after_the_number(tmp_path):
    """`/* 0x501c -- 0x50, destroyed last */` is the same evidence as `/* 0x501c */`.

    The field reader demanded `*/` immediately after the offset, so every field whose
    comment said anything at all was invisible -- which is most hand-written ones.
    Four already-typed members read as missing on that alone (dScMgJump_c's `Model`,
    dScMgSnowball_c's, dScMB_c's `FaderColor`, ModelAnim2's `Animation`), and six more
    `u8` array markers were hidden with them.  `check_header_offsets.DECL`, the gate
    this file has to agree with, has always stopped at the number; now so does this.
    """
    idx = _index(tmp_path, "dScMgJump_c.h", """\
        struct dScMgJump_c : dScMgD3DBase_c {
            s16   unk_5014;        /* 0x5014 */
            Model mModel;          /* 0x501c -- 0x50, destroyed last, see banner */
        };
        typedef char dScMgJump_c_size_must_be_0x5834[sizeof(dScMgJump_c) == 0x5834 ? 1 : -1];
        """)
    assert idx["dScMgJump_c"]["fields"][0x5014] == [("s16", "unk_5014")]
    assert idx["dScMgJump_c"]["fields"][0x501C] == [("Model", "mModel")]


def test_a_nested_struct_does_not_swallow_the_fields_below_it(tmp_path):
    """`struct State { ... };` opened before the first real field took the class with it.

    The reader closed a definition on a `}` in column zero, and a nested block's `};`
    is indented -- so it entered `State` and never came back out.  Bullet, FlyGuy,
    HootTheOwl, Player, Snufit and Swoop each read as having ZERO declared fields, and
    all 26 of their already-typed members were reported as outstanding work.
    """
    idx = _index(tmp_path, "Bullet.h", """\
        struct Bullet : dEnemyBase_c {
            struct State {
                u8  pad_00[0x8];
                void (Bullet::*mMain)();      /* 0x08 */
            };

            dCcAc_c    mdCcAc_c;        /* 0x110 */
            dBgCh_Actr mWithMeshClsn;   /* 0x144 */
            Model      mModel;          /* 0x300 */
        };
        typedef char Bullet_size_must_be_0x35c[sizeof(Bullet) == 0x35c ? 1 : -1];
        """)
    assert idx["Bullet"]["fields"][0x110] == [("dCcAc_c", "mdCcAc_c")]
    assert idx["Bullet"]["fields"][0x300] == [("Model", "mModel")]
    # the nested type's own 0x08 is not one of Bullet's offsets
    assert 0x08 not in idx["Bullet"]["fields"]
    assert idx["Bullet"]["bases"] == ["dEnemyBase_c"]


def test_a_member_the_base_declares_is_inherited_not_missing(tmp_path):
    """mwccarm INLINES a base destructor into the derived one.

    So the cartridge shows `~ArmedRotatingPlatform` destroying the `Model` at +0xd4 and
    the `dBgW_KcMbg` at +0x124 -- both of them dBgActor_c's own members, declared in
    dBgActor_c's header.  Reading only the derived class's own field list called every
    one of those an untyped hole; 144 of the 181 findings in the first census were
    exactly this, across the dBgActor_c and dEnemyBase_c families.
    """
    idx = _index(tmp_path, "family.h", """\
        struct dBgActor_c : dActor_c {
            Model      mModel;          /* 0x0d4 */
            dBgW_KcMbg mMeshCollider;   /* 0x124 */
        };
        typedef char dBgActor_c_size_must_be_0x320[sizeof(dBgActor_c) == 0x320 ? 1 : -1];
        struct ArmedRotatingPlatform : dBgActor_c {
            s16 mAngVelY;               /* 0x31e */
        };
        typedef char ArmedRotatingPlatform_size_must_be_0x320[1];
        """)
    own, inh = D.declared_at(idx, "ArmedRotatingPlatform", 0x0D4)
    assert own == [] and inh == [("Model", "mModel")]
    # and the class's own field is still its own, not the base's
    own, inh = D.declared_at(idx, "ArmedRotatingPlatform", 0x31E)
    assert own == [("s16", "mAngVelY")] and inh == []


def test_the_base_walk_survives_a_cycle(tmp_path):
    """Two headers naming each other as bases must not recurse forever."""
    idx = _index(tmp_path, "cycle.h", """\
        struct A : B {
            s32 unk_004;   /* 0x004 */
        };
        typedef char A_size_must_be_0x8[1];
        struct B : A {
            s32 unk_000;   /* 0x000 */
        };
        typedef char B_size_must_be_0x8[1];
        """)
    assert D.declared_at(idx, "A", 0x000) == ([], [("s32", "unk_000")])


def test_only_an_unconditional_include_counts_as_visible():
    """`#include "Model.h"` inside the `#ifdef __cplusplus` branch is textually above
    the flat C definition and invisible to it.  Three headers were typed against an
    include that could not be reached and failed with `undefined identifier 'Model'`."""
    lines = textwrap.dedent("""\
        #ifndef X_H
        #define X_H
        #include "types.h"
        #ifdef __cplusplus
        #include "Model.h"
        struct X { Model mModel; };
        #else
        struct X { u8 mModel; };
        #endif
        """).splitlines()
    seen, anchor = D.unconditional_includes(lines, len(lines))
    assert seen == ["types.h"]                 # NOT Model.h
    assert lines[anchor] == '#include "types.h"'


def test_a_c_usable_type_needs_a_typedef_as_well_as_a_definition(tmp_path):
    """In C the tag is not a type name.  `Model` has `typedef struct Model Model;` and
    `ShadowModel` does not, which is why one can be written into a C-reachable header
    and the other cannot -- and why PR #1659 failed on `illegal constant expression`."""
    inc = tmp_path / "include"
    inc.mkdir(parents=True)
    (inc / "Model.h").write_text(textwrap.dedent("""\
        #ifdef __cplusplus
        struct Model : ModelBase { };
        #else
        struct Model {
            void **vtable;
        };
        typedef struct Model Model;
        #endif
        """), encoding="utf-8")
    (inc / "ShadowModel.h").write_text(textwrap.dedent("""\
        #ifdef __cplusplus
        struct ShadowModel : ModelBase { };
        #else
        struct ShadowModel {
            void **vtable;
        };
        #endif
        """), encoding="utf-8")
    (inc / "dBgW_KcMbg.h").write_text(textwrap.dedent("""\
        #ifdef __cplusplus
        struct dBgW_KcMbg : dBgW_Kc { };
        #endif
        """), encoding="utf-8")
    usable = D.c_usable_types(tmp_path)
    assert "Model" in usable
    assert "ShadowModel" not in usable          # C definition, no typedef
    assert "dBgW_KcMbg" not in usable           # no C definition at all


def test_c_reachability_follows_the_include_graph(tmp_path):
    inc, src = tmp_path / "include", tmp_path / "src"
    inc.mkdir(parents=True)
    src.mkdir(parents=True)
    (inc / "A.h").write_text('#include "B.h"\n', encoding="utf-8")
    (inc / "B.h").write_text("struct B { int x; };\n", encoding="utf-8")
    (inc / "Lonely.h").write_text("struct Lonely { int x; };\n", encoding="utf-8")
    (src / "t.c").write_text('#include "A.h"\nint f(void) { return 0; }\n',
                             encoding="utf-8")
    (src / "u.cpp").write_text('#include "Lonely.h"\n', encoding="utf-8")
    got = D.c_reachable(tmp_path)
    assert inc / "A.h" in got and inc / "B.h" in got     # transitively
    assert inc / "Lonely.h" not in got                   # only a .cpp reaches it


# ------------------------------------------------------------- the ROM itself

def _rom():
    try:
        ext = O.find_extracted(REPO)
    except SystemExit:
        return None
    return ext if (ext / "arm9_dec.bin").is_file() else None


def test_the_frozen_census_reproduces():
    """End-to-end, and skipped where the ROM dump is not installed -- the same way
    tools/test_build_pin.py gates its own."""
    ext = _rom()
    if ext is None:
        return
    doc = D.harvest(REPO, ext)
    st = doc["meta"]["stats"]
    bad = [
        "%s: expected %d, got %s (%+d)" %
        (key, expected, actual, actual - expected)
        for key, expected in D.GATE.items()
        for actual in [st.get(key, 0)]
        if actual != expected
    ]
    assert not bad, "census drift:\n  " + "\n  ".join(bad)


def test_every_call_site_the_rom_shows_yields_an_offset():
    """The interpreter is complete over this cartridge: not one of the 2,426 dtor->dtor
    calls inside a named destructor fails to produce an offset.  A fall here means a
    codegen shape was introduced or the pool read was lost."""
    ext = _rom()
    if ext is None:
        return
    st = D.harvest(REPO, ext)["meta"]["stats"]
    assert st["sites_offset_recovered"] == st["sites_inside_a_named_destructor"]


def test_no_offset_carries_two_different_member_types():
    """Two types at one (owner, offset) would mean the attribution is wrong somewhere,
    the way pairing a class to a factory by name was wrong in PR #1556."""
    ext = _rom()
    if ext is None:
        return
    doc = D.harvest(REPO, ext)
    clash = [p for p in doc["pairs"] if p["conflicting_types"]]
    assert not clash, clash[:5]


def test_the_applied_members_agree_with_what_the_rom_says():
    """Every member this pass wrote is still exactly what the cartridge proves, at the
    offset it proves, with a size assert behind it.  A header edited by hand afterwards
    fails here rather than at the next person's `check_header_offsets` run."""
    ext = _rom()
    if ext is None:
        return
    doc = D.harvest(REPO, ext)
    mods = O.load_modules(ext, collections.Counter())
    buckets, hdr, _al, sizes = D.gap(REPO, doc, mods, collections.Counter())
    for rec in buckets["ALREADY TYPED"]:
        decl = hdr[rec["owner_header"]]["fields"][int(rec["offset"], 16)]
        assert any(t == rec["member"] for t, _ in decl), rec
        assert rec["member"] in sizes, rec


if __name__ == "__main__":                                             # pragma: no cover
    import pytest
    sys.exit(pytest.main([__file__, "-q"]))
