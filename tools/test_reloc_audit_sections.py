"""The strict-reloc gate must read the relocation table belonging to the function
it was asked about.

Regression test for a silent wrong answer. `object_reloc_dests` selected the reloc
section by NAME -- `get_section_by_name(".rela" + sec.name)` -- and mwccarm names
every function's section ".text", so ".rela.text" is ambiguous and pyelftools answers
with the LAST section of that name. Every function in the object then resolved against
one fixed table, no matter which function was asked about.

mwccarm emits a destructor TU as D2, D0, D1, so that fixed table is D1's. D1 got its
own by luck; D0 and D2 got D1's. Observed live: a `BowserFire::~BowserFire()` D0 whose
seven relocation destinations are all correct was reported WRONG-DEST, because the tool
compared D1's five against D0's offsets.

The byte compare cannot cover for it. Relocated words are wildcarded, which is the
entire reason the destination check exists, so a wrong callee inside a multi-function
object had nothing checking it.

sh_info is the only reliable link from a reloc section to the section it patches.

No compiler and no ROM: the object here is a hand-built ELF, which is also the only
way to pin the two-sections-with-one-name shape independent of a toolchain.
"""
import ast
import pathlib
import struct
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import reloc_audit as RA  # noqa: E402

SHT_PROGBITS, SHT_SYMTAB, SHT_STRTAB, SHT_RELA = 1, 2, 3, 4
STT_FUNC, STB_GLOBAL = 2, 1
R_ARM_CALL = 28


def _strtab(names):
    """(blob, {name: offset}) for an ELF string table."""
    blob, off = b"\0", {"": 0}
    for n in names:
        off[n] = len(blob)
        blob += n.encode() + b"\0"
    return blob, off


def _elf_two_text_sections():
    """An ET_REL object with two sections both named ".text", each with its own
    ".rela.text". Section 4 is `alpha`, section 6 is `beta`; only sh_info tells
    the two relocation tables apart.

    `alpha` stands in for D0 and `beta` for D1: the by-name lookup resolves to the
    LAST ".rela.text", so `beta` gets its own table by accident and `alpha` -- the
    function this tree actually needs to check -- gets `beta`'s. The two relocations
    sit at different offsets (+0x0 and +0x4) so a mix-up cannot pass unnoticed; in
    the real objects D0's and D1's happened to overlap, which is what let the bug
    hide for 74 migrated destructors.
    """
    secnames = [".shstrtab", ".strtab", ".symtab", ".text", ".rela.text"]
    shstr, shoff = _strtab(secnames)
    symnames = ["alpha_target", "beta_target", "alpha", "beta"]
    strtab, stroff = _strtab(symnames)

    text_a = struct.pack("<2I", 0xEB000000, 0xE12FFF1E)          # bl <A>; bx lr
    text_b = struct.pack("<2I", 0xE1A00000, 0xEB000000)          # nop;    bl <B>

    # symtab: null, alpha_target(1), beta_target(2), alpha(3), beta(4)
    def sym(name_off, value, size, info, shndx):
        return struct.pack("<IIIBBH", name_off, value, size, info, 0, shndx)

    symtab = (sym(0, 0, 0, 0, 0)
              + sym(stroff["alpha_target"], 0, 0, (STB_GLOBAL << 4), 0)
              + sym(stroff["beta_target"], 0, 0, (STB_GLOBAL << 4), 0)
              + sym(stroff["alpha"], 0, len(text_a), (STB_GLOBAL << 4) | STT_FUNC, 4)
              + sym(stroff["beta"], 0, len(text_b), (STB_GLOBAL << 4) | STT_FUNC, 6))

    # alpha relocates at +0x0 to symbol 1; beta relocates at +0x4 to symbol 2.
    rela_a = struct.pack("<III", 0x0, (1 << 8) | R_ARM_CALL, 0)
    rela_b = struct.pack("<III", 0x4, (2 << 8) | R_ARM_CALL, 0)

    blobs = [b"", shstr, strtab, symtab, text_a, rela_a, text_b, rela_b]
    types = [0, SHT_STRTAB, SHT_STRTAB, SHT_SYMTAB, SHT_PROGBITS, SHT_RELA,
             SHT_PROGBITS, SHT_RELA]
    names = ["", ".shstrtab", ".strtab", ".symtab", ".text", ".rela.text",
             ".text", ".rela.text"]
    # sh_link: symtab -> strtab(2); rela -> symtab(3).  sh_info: rela -> its .text.
    links = [0, 0, 0, 2, 0, 3, 0, 3]
    infos = [0, 0, 0, 1, 0, 4, 0, 6]
    entsz = [0, 0, 0, 16, 0, 12, 0, 12]

    ehsize, shentsize, nsec = 52, 40, len(blobs)
    offsets, cur = [], ehsize
    for b in blobs:
        offsets.append(cur if b else 0)
        cur += len(b)
    shoff_start = cur

    eh = struct.pack(
        "<16sHHIIIIIHHHHHH",
        b"\x7fELF\x01\x01\x01" + b"\0" * 9,      # ELFCLASS32, ELFDATA2LSB, EV_CURRENT
        1,                                        # ET_REL
        40,                                       # EM_ARM
        1, 0, 0, shoff_start, 0,
        ehsize, 0, 0, shentsize, nsec, 1)         # e_shstrndx = 1

    out = bytearray(eh)
    for b in blobs:
        out += b
    for i in range(nsec):
        out += struct.pack("<10I", shoff[names[i]], types[i], 0, 0, offsets[i],
                           len(blobs[i]), links[i], infos[i], 4, entsz[i])
    return bytes(out)


NAME_INDEX = {"alpha_target": ("arm9", 0x02001000),
              "beta_target": ("arm9", 0x02002000)}


def test_each_function_gets_its_own_relocation_table():
    obj = _elf_two_text_sections()

    # The one the by-name lookup got wrong: ".rela.text" resolves to beta's table,
    # so alpha came back holding beta_target at +0x4.
    dests, _ = RA.object_reloc_dests(obj, "alpha", NAME_INDEX)
    assert dests == [(0x0, "alpha_target", "arm9", 0x02001000)], dests

    dests, _ = RA.object_reloc_dests(obj, "beta", NAME_INDEX)
    assert dests == [(0x4, "beta_target", "arm9", 0x02002000)], dests


def test_the_object_really_does_reproduce_the_hazard():
    """Without this, the test above could be passing for the wrong reason -- a
    synthetic object whose sections happen not to collide would prove nothing. Run
    the original by-name lookup and require it to give alpha the wrong answer."""
    import io
    from elftools.elf.elffile import ELFFile

    elf = ELFFile(io.BytesIO(_elf_two_text_sections()))
    syms = list(elf.get_section_by_name(".symtab").iter_symbols())
    alpha = next(s for s in syms if s.name == "alpha")
    sec = elf.get_section(alpha["st_shndx"])

    by_name = elf.get_section_by_name(".rel" + sec.name) or \
        elf.get_section_by_name(".rela" + sec.name)
    assert by_name.header["sh_info"] != alpha["st_shndx"], (
        "the by-name lookup resolved to alpha's own table, so this object does not "
        "reproduce the bug and the regression test above is vacuous")

    by_info = RA.rel_section_for(elf, alpha["st_shndx"])
    assert by_info.header["sh_info"] == alpha["st_shndx"]


def test_a_function_with_no_relocations_reports_none_rather_than_a_neighbours():
    """The same bug's other face: a function whose own table is empty must come back
    empty, not inherit the first ".rela.text" in the file."""
    obj = _elf_two_text_sections()
    dests, size = RA.object_reloc_dests(obj, "beta", {})
    assert size == 8
    assert [d[1] for d in dests] == ["beta_target"]


def test_missing_function_still_reports_why():
    dests, reason = RA.object_reloc_dests(_elf_two_text_sections(), "gamma", {})
    assert dests is None
    assert reason == "func-not-in-obj"


def _relocs_looked_up_by_name(tree):
    """Line numbers of `…get_section_by_name(<something mentioning .rel>)` CALLS.

    Read the AST, not the text. Both files that matter here now carry comments
    explaining the bug, and a grep counts the explanation as an instance of it --
    the same trap the langmode metric fell into. Only a call node is a call.
    """
    def mentions_rel(node):
        if isinstance(node, ast.Constant) and isinstance(node.value, str):
            return ".rel" in node.value
        if isinstance(node, ast.BinOp):           # ".rel" + sec.name
            return mentions_rel(node.left) or mentions_rel(node.right)
        if isinstance(node, ast.JoinedStr):       # f".rel{...}"
            return any(mentions_rel(v) for v in node.values)
        return False

    hits = []
    for node in ast.walk(tree):
        if not isinstance(node, ast.Call):
            continue
        fn = node.func
        if getattr(fn, "attr", None) != "get_section_by_name":
            continue
        if node.args and mentions_rel(node.args[0]):
            hits.append(node.lineno)
    return hits


def test_no_tool_selects_a_relocation_section_by_name():
    """Guard against the defect reappearing anywhere in tools/."""
    root = pathlib.Path(__file__).resolve().parent
    offenders = []
    for path in sorted(root.glob("*.py")):
        if path.name.startswith("test_"):
            continue
        src = path.read_text(encoding="utf-8", errors="ignore")
        try:
            tree = ast.parse(src)
        except SyntaxError:
            continue
        offenders += [f"{path.name}:{n}" for n in _relocs_looked_up_by_name(tree)]
    assert not offenders, (
        "relocation sections must be found by sh_info, not by name -- mwccarm gives "
        f"every function's table the same name: {offenders}")


def test_the_guard_can_actually_see_the_defect():
    """A guard that cannot fail proves nothing. Plant the original line and the two
    spellings it could come back as, and require each to be caught."""
    for planted in ('rel = elf.get_section_by_name(".rel" + sec.name)',
                    'rel = elf.get_section_by_name(".rela.text")',
                    'rel = elf.get_section_by_name(f".rela{sec.name}")'):
        assert _relocs_looked_up_by_name(ast.parse(planted)), planted
    # ...and does not fire on a lookup that is not a relocation section.
    assert not _relocs_looked_up_by_name(
        ast.parse('st = elf.get_section_by_name(".symtab")'))
