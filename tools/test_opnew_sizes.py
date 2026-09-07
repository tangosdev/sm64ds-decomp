"""The four traps that have already cost this project landed work, as tests.

`opnew_sizes.py` recovers a class's size from the literal the ROM hands
`fBase_c::operator new`, and attributes it by the vtable the factory installs. Each of
the three ways that attribution has been got wrong before -- taking the first vptr store
instead of the last (#1559/#1560 reverted 2 of 13), reading past the factory's own extent
(commit 3f760a354, the mechanism behind the 41 misnamed classes of #1418-#1423), and
crediting a member subobject's vptr to its owner -- is a case below, driven through
synthetic ARM rather than the ROM so it runs in a checkout with no `extracted/`.

The fourth is this file's own: attribution by address is worth nothing if the HEADER is
then looked up by name. One vtable address carries several `_ZTV` spellings and only one
of them has a file, so the single-spelling join reported NO HEADER for 125 classes the
tree describes -- and their sizes went uncompared. Four cases cover it, and a live
ratchet requires every sized class to remain reachable through some header spelling.

The two end-to-end cases skip themselves when the ROM dump is not installed, the same
way tools/test_build_pin.py does.
"""
import collections
import pathlib
import sys

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import opnew_sizes as O     # noqa: E402

BASE = 0x02000000
NEW = 0x02043444


# --------------------------------------------------------------- ARM encoders

def mov_imm(rd, imm8, rot=0):
    """`mov rD, #imm8 ror 2*rot`."""
    return 0xE3A00000 | (rd << 12) | (rot << 8) | (imm8 & 0xFF)


def mov_reg(rd, rs):
    return 0xE1A00000 | (rd << 12) | rs


def ldr_pc(rd, disp):
    return 0xE59F0000 | (rd << 12) | (disp & 0xFFF)


def str_off(rv, rb, off=0):
    return 0xE5800000 | (rb << 16) | (rv << 12) | (off & 0xFFF)


def bl(at, target):
    return 0xEB000000 | (((target - (at + 8)) >> 2) & 0xFFFFFF)


def image(words, at=BASE, syms=None, extra=b""):
    """An Image over one synthetic module named `m`."""
    import struct
    blob = b"".join(struct.pack("<I", w) for w in words) + extra
    return O.Image({"m": (at, blob)}, {"m": sorted(syms or [])})


def run(img, start, size, callsite):
    stats = collections.Counter()
    events = O._walk(img, "m", start, size, O.State(), callsite, stats, 0, set())
    sz, conf = O.read_size(img, "m", start, size, callsite, stats)
    return sz, conf, events


# --------------------------------------------------------------- the size read

def test_the_size_is_the_r0_literal_at_the_call():
    site = BASE + 4
    img = image([mov_imm(0, 0xCB, 15),          # mov r0, #0x32c
                 bl(site, NEW), 0xE12FFF1E])
    size, conf, _ = run(img, BASE, 12, site)
    assert (size, conf) == (0x32C, "exact")


def test_a_size_that_is_not_a_constant_is_reported_not_guessed():
    site = BASE + 4
    img = image([0xE0800001,                    # add r0, r0, r1 -- unmodelled
                 bl(site, NEW), 0xE12FFF1E])
    size, conf, _ = run(img, BASE, 12, site)
    assert size is None and conf == "not_a_constant"


# ------------------------------------------------- trap 2: take the LAST store

def test_the_last_vptr_store_wins_not_the_first():
    """A factory inlines its base constructors and each stores ITS OWN vtable first.

    Reading the first store makes every derived class report as its base. This is the
    shape `_ZN6CameraC1Ev` has in the live ROM: three stores to [r4], only the third
    is Camera's."""
    site = BASE + 4
    words = [mov_imm(0, 0x1A, 12),              # mov r0, #0x1a000... (any size)
             bl(site, NEW),
             mov_reg(4, 0),                     # r4 = the object
             ldr_pc(0, 0x10), str_off(0, 4),    # base vptr
             ldr_pc(1, 0x0C), str_off(1, 4),    # derived vptr
             0xE12FFF1E, 0x00000000]
    # literals: read at pc+8+disp -> words[3] is at BASE+0xc, +8+0x10 = BASE+0x24
    img = image(words + [0x0BADBA5E, 0x0DE21AED])
    _sz, _c, events = run(img, BASE, len(words) * 4, site)
    stores = [e for e in events if e[0] == "store"]
    assert len(stores) == 2, stores
    vt, how, _ = O.resolve_vptr(img, "m", events, collections.Counter())
    assert (vt, how) == (stores[-1][2], "inline_store")
    assert vt != stores[0][2], "took the base's vtable, the #1559/#1560 defect"


# ------------------------------ trap 3: the scan stops at the function's extent

def test_the_scan_stops_at_the_functions_own_size():
    """The next function is almost always the NEXT class's D1, storing its own vtable.

    An unbounded window attributes that table here -- the shifted-name mechanism of
    commit 3f760a354. The window is the config `size=`, and nothing past it is read."""
    site = BASE + 4
    ours = [mov_imm(0, 0x40), bl(site, NEW), mov_reg(4, 0),
            ldr_pc(0, 0x14), str_off(0, 4), 0xE12FFF1E]
    # the neighbour: same shape, a different literal, immediately after
    nxt = [ldr_pc(0, 0x00), str_off(0, 4), 0xE12FFF1E, 0xDEADBEEF]
    img = image(ours + nxt + [0x0AAAAAAA])
    _sz, _c, events = run(img, BASE, len(ours) * 4, site)
    stores = [e for e in events if e[0] == "store"]
    assert len(stores) == 1, "read past the factory into the next function"
    assert all(a < BASE + len(ours) * 4 for _k, a, _v in events)


# -------------------------------- a member subobject's vptr is not the object's

def test_a_store_to_a_nonzero_offset_is_not_this_objects_vptr():
    """`add rD, this, #n` then a ctor there is a MEMBER. Its vtable is the member's."""
    site = BASE + 4
    words = [mov_imm(0, 0x40), bl(site, NEW), mov_reg(4, 0),
             ldr_pc(0, 0x04), str_off(0, 4, 0x50), 0xE12FFF1E]
    img = image(words + [0x0BADBA5E])
    _sz, _c, events = run(img, BASE, len(words) * 4, site)
    assert not [e for e in events if e[0] == "store"]


def test_an_add_does_not_carry_the_object_but_a_subs_zero_does():
    """`subs rD, rObj, #0` is mwcc's copy-and-null-test; `add rD, rObj, #n` is a member."""
    site = BASE + 4
    sub0 = 0xE2504000                                   # subs r4, r0, #0
    add50 = 0xE2805050                                  # add r5, r0, #0x50
    words = [mov_imm(0, 0x40), bl(site, NEW), sub0, add50,
             ldr_pc(1, 0x08), str_off(1, 4), str_off(1, 5), 0xE12FFF1E]
    img = image(words + [0x0BADBA5E])
    _sz, _c, events = run(img, BASE, len(words) * 4, site)
    stores = [e for e in events if e[0] == "store"]
    assert len(stores) == 1 and stores[0][2] == 0x0BADBA5E


# ----------------------------------- trap 1: identity comes from the vtable ADDRESS

def test_class_for_vtable_never_reads_a_name_to_choose():
    """Two overlays host a record at the same address; the module decides, not the name."""
    by_addr = {0x1000: [("ov014", "daObjWanwanShutter_c"), ("ov015", "daObjBk_Fall_Block_c")]}
    st = collections.Counter()
    assert O.class_for_vtable(by_addr, "ov015", 0x1000, st)[0] == "daObjBk_Fall_Block_c"
    assert O.class_for_vtable(by_addr, "ov014", 0x1000, st)[0] == "daObjWanwanShutter_c"


def test_an_unknown_vtable_is_a_refusal_not_a_guess():
    st = collections.Counter()
    name, why = O.class_for_vtable({}, "m", 0x1000, st)
    assert name is None and why == "no_rtti_record_at_vtable"


# ------------------------------------------------------------------- demangling

def test_demangle_ztv():
    assert O.demangle_ztv("_ZTV6Camera") == "Camera"
    assert O.demangle_ztv("_ZTV7da1up_c") == "da1up_c"
    assert O.demangle_ztv("_ZTVN8dGraph_c10callback_cE") == "dGraph_c__callback_c"
    assert O.demangle_ztv("data_0208e3a4") is None
    assert O.demangle_ztv("_ZTV99Truncated") is None


# ---------------------------------------------------------- the header readers

def test_header_sizes_reads_the_assert_and_notices_one_that_is_missing(tmp_path):
    inc = tmp_path / "include"
    inc.mkdir()
    (inc / "A.h").write_text(
        "struct A {\n  int x;\n};\n"
        "typedef char A_size_must_be_0x330[sizeof(struct A) == 0x330 ? 1 : -1];\n")
    (inc / "B.h").write_text("struct B : A {\n  int y;\n};\n")
    got = O.header_sizes(tmp_path)
    assert got["A"]["assert"] == 0x330
    assert got["B"]["assert"] is None and got["B"]["header"].name == "B.h"


def test_a_class_is_found_under_the_name_its_vtable_alias_carries():
    """THE `--gap` defect.  Attribution is by address; the header lookup was by name.

    One vtable address routinely carries several `_ZTV` symbols -- 0x021280b0 is
    `_ZTV12FortressWall`, `_ZTV9MontyMole` and `_ZTV11daChoropu_c` at once -- and only
    one of those spellings has a file.  Keying the header lookup on whichever spelling
    the attribution happened to carry reported NO HEADER for 125 classes `include/`
    describes, so their sizes were never compared to the header that describes them.
    """
    hdr = {"MontyMole": {"assert": 0x18C, "header": pathlib.Path("MontyMole.h")}}
    assert O.resolve_header_name("daChoropu_c", ["FortressWall", "MontyMole"], hdr) \
        == ("MontyMole", "vtable alias")


def test_a_class_that_has_its_own_header_is_never_redirected_to_an_alias():
    """The class's own key wins, so a real header is never shadowed by a sibling's."""
    hdr = {"A": {"assert": 1, "header": None}, "B": {"assert": 2, "header": None}}
    assert O.resolve_header_name("A", ["B"], hdr) == ("A", "name")


def test_no_header_survives_only_when_no_spelling_has_one():
    hdr = {"Other": {"assert": 1, "header": None}}
    assert O.resolve_header_name("BigBooIcon", ["BigBooIcon"], hdr) == (None, None)
    assert O.resolve_header_name("BigBooIcon", [], hdr) == (None, None)


def test_every_ZTV_name_at_an_address_is_kept_not_just_the_last(tmp_path):
    """The collapse that caused it: a dict keyed on (module, addr) keeps one name."""
    cfg = tmp_path / "config" / "arm9"
    (cfg / "overlays").mkdir(parents=True)
    (cfg / "symbols.txt").write_text(
        "_ZTV12FortressWall kind:data(any) addr:0x021280b0\n"
        "_ZTV9MontyMole kind:data(any) addr:0x021280b0\n"
        "_ZTV11daChoropu_c kind:data(any) addr:0x021280b0\n"
        "_ZTV6Camera kind:data(any) addr:0x02100000\n")
    st = collections.Counter()
    got = O.load_vtable_symbols(tmp_path, {"arm9": (0, b"")}, st)
    assert len(got[("arm9", 0x021280B0)]) == 3
    assert st["vtable_addresses_with_more_than_one_name"] == 1


def test_subclass_index_is_transitive(tmp_path):
    inc = tmp_path / "include"
    inc.mkdir()
    (inc / "H.h").write_text(
        "struct A {\n int x;\n};\n"
        "struct B : public A {\n int y;\n};\n"
        "struct C : B {\n int z;\n};\n")
    kids = O.subclass_index(tmp_path)
    assert kids["A"] == {"B", "C"}
    assert kids["B"] == {"C"}


# ------------------------------------------------------------------- end to end

def _rom():
    return (REPO / "extracted" / "arm9_dec.bin").is_file() and \
           (REPO / "build" / "rtti.json").is_file()


def test_the_live_census_reproduces():
    if not _rom():
        return
    doc = O.build(REPO, REPO / "extracted")
    st = doc["meta"]["stats"]
    for k, want in O.GATE.items():
        assert st.get(k) == want, (k, want, st.get(k))
    assert not doc["cross_check_warnings"], doc["cross_check_warnings"][:5]


def test_every_site_the_ROM_has_is_attributed_to_a_class():
    """A pass that silently matches nothing reports a clean, confident, wrong answer."""
    if not _rom():
        return
    doc = O.build(REPO, REPO / "extracted")
    assert doc["meta"]["stats"]["sites_unattributed"] == 0
    assert all(s["size"] is not None for s in doc["sites"])


def test_no_live_sized_class_is_headerless():
    """The ratchet on the alias join: it silently reverts to 127 if the join regresses.

    The last two exceptions were retired by legitimate class reconstruction:
    `RecRoomCupboard` gained its own header, and `BigBooIcon` resolves through the
    `daTrsIcon_c` vtable alias to that class's header.  The probe is off: the header
    LOOKUP is what is under test, not sizeof.  The full bucket is the assertion value
    so a future exception names the class, ROM size, and failed path immediately."""
    if not _rom():
        return
    doc = O.build(REPO, REPO / "extracted")
    buckets, _hdr, _st = O.gap(REPO, doc, probe=False)
    assert buckets["NO HEADER"] == [], buckets["NO HEADER"]
