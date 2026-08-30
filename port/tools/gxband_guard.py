"""Build-time guard: a hosted DS BAND must come out of the linker in ROM order.

THE BUG THIS EXISTS FOR IS THE UNDERSIZED-HOSTED-GLOBAL FAMILY, and the shape
that makes it expensive is that the DS treats one symbol as an INTERIOR ADDRESS
of another while the port hosts the two as separate objects.

  * The GX bank-state block, 0x020a6088..0x020a60a4 (run link60 SUB2). The
    SetBankFor* family reaches members of ONE struct at 0x020a6088 out to
    +0x18, and +0x18 IS the DS symbol data_020a60a0. Hosted apart, the member
    stores ran past the end of an eight-byte object (the symbol's own span in
    config/arm9/symbols.txt is TWO bytes), GXS::BeginLoadOBJExtPltt read a
    permanent zero, EndLoadOBJExtPltt restored bank 0, and scene 4's character
    portrait decoded through the wrong palette with 473 bytes of extended
    palette loaded and switched off. Eighteen src TUs name the head symbol and
    func_02053d9c clears the block 26 bytes at a time.
  * The ARM9 DTCM, 0x023c0000..0x023c4000 (run link60 IRQ2). Hosted as 64
    bytes because that was the span the first caller found. Five handlers in
    src write the NitroSDK OSi_IrqCheckFlag word at DTCM_END - 8; at 64 bytes
    that store landed 16,312 bytes past the object, 56 bytes into
    _hal_area_table and so INSIDE .dsstate, which means a save state captured
    the corruption and a restore replayed it.

Both were found by running the game, by different lanes, from symptoms with no
obvious connection to layout: a character portrait in the wrong colours, and an
interrupt nobody had ever delivered. This guard asks the question at LINK TIME
instead, off the map the linker already writes.

WHY LINK TIME AND NOT RUN TIME. hal/cxx_aliases.cpp carries
port_gxbank_layout_check, which reads the section trick back and takes a run
down under SM64DS_FAULTS_FATAL. It works -- it was negative-tested by
mis-aligning a band member on purpose -- but it checks TWO of the band's
thirteen offsets, on ONE code path (hal/sub_screen.cpp's Stage bring-up), in
binaries that link that file. Everything else in the build could ship a broken
band and say nothing. This guard reads config/arm9/symbols.txt for what the
offsets must be and walk_window.map for what the linker chose, covers every
member of every declared band, and covers EVERY TARGET that hosts one --
/MAP is on CMAKE_EXE_LINKER_FLAGS, so every executable in this build writes a
map next to itself. A broken band fails the build rather than one scene.

AND THE TWO OFFSETS IT TESTS CAN BOTH BE RIGHT OVER A BROKEN BAND. Measured,
not argued: with data_020a60a0 hosted at two bytes instead of four,
data_020a609c is still at +0x14 and data_020a60a0 is still at +0x18, so
port_gxbank_layout_check returns 1. walk_window ran scene 4 for 30 frames under
SM64DS_FAULTS_FATAL=1, exit 0, clean, not one diagnostic line, while the band's
last two bytes belonged to data_0209f310. This guard failed all three hosting
targets on the same binaries.

AND THE BAND CAN BREAK IN A TARGET THE CHECK IS NOT IN. The review's third
mutation put align(8) on data_020a6098, which is at +16: walk_window's head is
at 0xb8 so +16 lands on 0xc8 and nothing moves, while smoke_player's head is at
0xb4 so +16 lands on 0xc4 and the member slides. Only smoke_player breaks, and
smoke_player is the one target of the three that carries neither
port_gxbank_layout_check nor its caller. A guard reading walk_window.map alone
would have reported layout OK over it. Reading every map is load-bearing, not
thoroughness. port/ppu_gap_audit.txt section 11a carries the whole experiment.

WHY A NEW TOOL AND NOT AN EXTENSION (the ruling, so the next lane does not
re-litigate it):

  * port/tools/alternatename_guard.py owns the post-link slot in
    build-port.cmd, but its whole domain is the FATE OF AN /alternatename
    DIRECTIVE -- its baseline file, its scoping rules and its selftest are all
    that one question. A config-vs-map layout assertion behind the same exit
    code and the same selftest would make one tool answer two unrelated
    questions, and the first thing to rot would be the selftest's claim to
    cover its subject.
  * port/tools/dsstate_guard.py is the closer relative: it also reads the map
    and it also polices hosted DS globals. But its question is BINARY
    MEMBERSHIP of one span (is this symbol inside .dsstate), it takes no config
    input at all, and it is wired as a POST_BUILD command on walk_window alone
    -- one target, which is the exact coverage limit this lane exists to
    remove. Relative layout derived from the ROM's own symbol table is a
    different contract with a different remedy.
  * What IS reused rather than re-invented: the map-reading discipline this
    tree already paid for. The row shape and the Rva+Base convention come from
    faultmap.load_map, and the refusals (missing, zero-byte, row-less,
    trailer-less) are closure.map_defined's, because a guard that reads a
    truncated map and reports green is the failure this repo keeps writing
    guards against.

WHAT A BAND IS, AND WHERE ITS NUMBERS COME FROM

A band is a half-open DS address range [start, end) declared in BANDS below.
Everything else is derived at run time:

  MEMBERS      every symbol the module's own config symbols.txt places in
               [start, end), in address order. Nothing is hardcoded here: the
               thirteen GX members and their twelve inter-member deltas are
               read out of config/arm9/symbols.txt on every run, so a config
               correction propagates into the guard instead of being contra-
               dicted by it.
  MEMBER COUNT declared, and cross-checked against what config yields. This is
               a tripwire, not the source of truth: if config gains or loses a
               row inside a band, the guard REFUSES rather than quietly
               asserting a different set of deltas than the one written down.
  SPAN         end - start. For a band whose end is itself a config symbol
               (the GX band ends at data_020a60a4) the guard verifies that and
               the span is config-derived end to end. For a band whose end is
               not a symbol -- the DTCM's 0x023c4000 is a hardware extent, not
               a row in any symbols.txt -- the declaration carries a prose
               provenance and the guard prints it rather than pretending the
               config said it.

THE FIVE ARMS, and which one catches which bug:

  A  PRESENCE     every member must appear in the map. A member that vanished
                  (deleted definition, or a host object that stopped being
                  linked) leaves a hole the other arms cannot see.
  B  DELTAS       every consecutive pair of members must sit exactly the
                  config delta apart. This is the arm that catches the GX band
                  being split back into separate objects, and it is the arm
                  that names the symbol, the delta the ROM has and the delta
                  the linker produced.
  C  EXCLUSION    no other symbol may land strictly inside the band's span. A
                  foreign symbol inside the span means the band's own storage
                  does not reach that far, which is the DTCM bug exactly: at
                  64 bytes the next .dsstate object sat at +64 with 16 KB of
                  DS state notionally on top of it.
  D  CONTAINMENT  the span must fit inside the containing image section from
                  the head's offset. Arm C alone has a hole -- an undersized
                  object that happens to be LAST in its section has nothing
                  after it to be caught colliding with -- and this is that
                  hole's floor.
  E  SIZE         where the map publishes a member's own size, it must be at
                  least its ROM span. The map's section table lists ONE ROW
                  PER CONTRIBUTION, so the grouped-section remedy -- which
                  gives every member its own .dsstate$gxbankNN -- also
                  publishes every member's length. Least, not exactly: a
                  member hosted LARGER than its ROM span displaces the next
                  one, and arm B is what says so.

HOW THE BAND'S LAST DELTA IS ASSERTED. Arm B measures twelve deltas between
symbol pairs; the thirteenth is data_020a60a0's own four-byte tail
(0x020a60a0..0x020a60a4), and no member follows it for arm B to measure
against. Arms C and D assert it structurally -- those four bytes must be free
of anything else and must fit in the section -- and on any build using the
grouped-section remedy arm E measures it outright, because .dsstate$gxbank12's
row says four bytes. On this tree's walk_window.map all thirteen members are
sized that way, which is stated as a property of the current layout rather
than as a promise: a band hosted some other way keeps arms A through D and
loses arm E, and the verbose line reports how many members were sized so the
coverage is never a guess.

WHAT THIS GUARD DOES NOT PROVE

  * That a hosted object sharing a contribution row is the right SIZE. The
    publics table gives addresses, not sizes, and only a member with its own
    section row reaches arm E. data_023c0000 is one that does not: it sits in
    the bulk .dsstate contribution with hundreds of other objects, so its 16
    KB is proved by what follows it (arms C and D), not read off the map.
  * Anything about a band nobody declared. This is a declared list of two,
    not a sweep of every hosted global against its config next-symbol delta.
    That sweep is a bigger and different tool: plenty of hosted globals are
    deliberately smaller than their config gap, and turning that into a build
    failure would be a project, not a guard.
  * Anything about a target that does not host the band. A map with no
    definition of the head symbol is reported as SKIPPED, and if NO map hosts
    a band the guard REFUSES: a guard that checked nothing must never read
    green. Between those two is the coverage floor: `hosts` names the maps
    that hosted the band when it was measured, so a band leaving one target
    out of three is loud rather than a smaller green.
  * That a symbol sharing a member's exact address is an alias rather than
    foreign storage. Arm C excuses equal addresses on purpose -- a fired
    /alternatename and MSVC's decorated C++ spelling of the same object both
    publish a second name there, and every real build has them. A zero-sized
    foreign symbol placed exactly on a member would be excused with them. The
    linker does not overlap storage, so what that costs is the ability to
    tell an alias from a zero-length decoy, not the ability to see a member
    being overwritten.

USAGE

    python port/tools/gxband_guard.py --build-dir build/port
    python port/tools/gxband_guard.py --map build/port/walk_window.map
    python port/tools/gxband_guard.py --build-dir build/port --verbose
    python port/tools/gxband_guard.py --selftest

Exit 0 when every declared band is intact in every map that hosts it; non-zero
with the symbol, the expected delta, the measured delta and the remedy
otherwise.
"""

import argparse
import os
import re
import shutil
import sys
import tempfile


# ---------------------------------------------------------------------------
# THE DECLARED BANDS.
#
# start/end are DS addresses; everything derived from them is read out of the
# config at run time. `members` is the count config must yield -- a tripwire on
# the config moving under the declaration, not a second source of truth.
#
# `hosts` is the same idea for coverage: the measured set of maps that host the
# band today, floored so a SHRINK is loud. Without it the green line reads
# "3/21 maps" and would go on reading green at 2/21 the day a refactor drops
# the band from a target, which is a coverage regression wearing a pass. It is
# the repo's baseline pattern, kept in the declaration rather than in a
# sidecar file because it is three names per band and not a list of hundreds.
# Widening it is a measurement anyone can take; narrowing it needs the reason
# written down.
# ---------------------------------------------------------------------------
BANDS = (
    {
        'key': 'gxbank',
        'module': 'arm9',
        'start': 0x020a6088,
        'end': 0x020a60a4,
        'end_source': 'config',
        'members': 13,
        'hosts': ('smoke_player.map', 'walk_window.map',
                  'walk_window_hires.map'),
        'host': 'port/hal/cxx_aliases.cpp',
        'what': 'the GX bank-state block',
        'remedy':
            'lay the band out in ROM order with the grouped-section mechanism '
            'in port/hal/cxx_aliases.cpp -- the GXBANK() macro puts each\n'
            '     member in .dsstate$gxbankNN with align(2) on the members and '
            'align(4) on the head, so the linker emits them adjacent in\n'
            '     suffix order. Splitting any member back into a plain object '
            'is what this failure looks like. Read the banner above GXBANK\n'
            '     in that file, and port/ppu_gap_audit.txt section 11 for what '
            'the split cost on screen.',
    },
    {
        'key': 'dtcm',
        'module': 'dtcm',
        'start': 0x023c0000,
        'end': 0x023c4000,
        # NOT a config row: config/arm9/dtcm/delinks.txt describes only the
        # 0x20 bytes of DTCM .data the ROM initializes. The 16 KB is the ARM9
        # data TCM's own extent, which is what the five OSi_IrqCheckFlag
        # writers address into at DTCM_END - 8.
        'end_source':
            'the ARM9 DTCM extent (0x023c0000..0x023c4000), not a symbols.txt '
            'row; see port/irq2_map.txt section 8 and the note above '
            'data_023c0000 in port/hal/player_bridges.cpp',
        'members': 1,
        'hosts': ('smoke_player.map', 'walk_window.map',
                  'walk_window_hires.map'),
        'host': 'port/hal/player_bridges.cpp',
        'what': 'the ARM9 data TCM',
        'remedy':
            'size data_023c0000 by the TCM extent in '
            'port/hal/player_bridges.cpp (unsigned char[0x4000], align 8), not '
            'by the\n     span of whichever caller was found first. The '
            'handlers write DTCM_END - 8; anything shorter puts that store in '
            'another\n     symbol, and inside .dsstate it is captured by save '
            'states too.',
    },
    {
        # run rel0215 lane prop15, the SECOND instance of the same disease and
        # the reason the vsstar row above is not a one-off.
        #
        # dsd splits the four-byte run at 0x0209b2ec into data_0209b2ec (1
        # byte) and data_0209b2ed (3); 0x0209b2f0 is a different object. These
        # are the VS ranking counters, and src/func_ov075_021165b0.c walks BOTH
        # names over the SAME four bytes -- `pb = data_0209b2ec` for i<4 to
        # increment them, then `pb = data_0209b2ed` for i in 1..3 to read them
        # back -- so the two names must be the first two BYTES of one block.
        # src/func_ov075_02116e00.c clears data_0209b2ec[0..3] the same way.
        #
        # The pair had no host at all until the propagation put those bodies in
        # the link; it is a grouped-section block in
        # hal/scene_vs_menu.cpp now, and this band is what keeps it one.
        'key': 'vsrank',
        'module': 'arm9',
        'start': 0x0209b2ec,
        'end': 0x0209b2f0,
        'end_source': 'config',
        'members': 2,
        'hosts': ('smoke_player.map', 'walk_window.map',
                  'walk_window_hires.map'),
        'host': 'port/hal/scene_vs_menu.cpp',
        'what': 'the VS ranking counters',
        'remedy':
            'keep the pair in .dsstate$hvsrank0000 / $hvsrank0001 with '
            '__declspec(align(4)) on the head and __declspec(align(1))\n'
            '     on the member, the GXBANK arrangement in '
            'port/hal/cxx_aliases.cpp. Hosting data_0209b2ec back in\n'
            '     hal/auto_bss.cpp as a generic int[8] is what this failure '
            'looks like: the increment loop and the read-back loop\n'
            '     then walk different memory, silently.',
    },
    {
        # run rel0215 lane prop15. THE VS CARRIED-STAR ARRAY, and it is the
        # third band because it was ALREADY BROKEN and nothing said so.
        #
        # The DS run is four bytes, 0x0209f310..0x0209f313, split by dsd into
        # data_0209f310 (1 byte) and data_0209f311 (the other 3); 0x0209f314 is
        # a different symbol, the level area table, hosted in
        # hal/camera_bridges.cpp. Every writer indexes data_0209f310[player],
        # and the win test's summand source NumVsStarsObtained starts at f310
        # for player 0 and then walks from &data_0209f311 -- so the two names
        # must be the first two BYTES of one block, or the writers and the
        # reader are looking at different memory.
        #
        # They were not. Measured off the 0.2.15 release build's own maps,
        # before the fix, on all three hosting targets: f311 sat at f310+4.
        # hal/actor_classes_star.cpp had the grouped-section half of the idiom
        # (two $NNNN contributions in suffix order) and not the alignment half,
        # so the linker's default four-byte contribution alignment padded the
        # one-byte head out to four. Players 1..3 wrote +1..+3 and were read
        # back from +4..+6, three bytes of padding no writer touches. Nothing
        # was clobbered and nothing ever faulted, which is exactly why this
        # needed a gate rather than a run.
        'key': 'vsstar',
        'module': 'arm9',
        'start': 0x0209f310,
        'end': 0x0209f314,
        'end_source': 'config',
        'members': 2,
        'hosts': ('smoke_player.map', 'walk_window.map',
                  'walk_window_hires.map'),
        'host': 'port/hal/actor_classes_star.cpp',
        'what': 'the VS carried-star array',
        'remedy':
            'keep the pair in .dsstate$hvsstar0000 / $hvsstar0001 with '
            '__declspec(align(4)) on the head and __declspec(align(1)) on\n'
            '     the member, which is the GXBANK arrangement in '
            'port/hal/cxx_aliases.cpp. Dropping either align() puts\n'
            '     data_0209f311 at f310+4, and NumVsStarsObtained then reads '
            'bytes no writer touches for players 1..3 --\n'
            '     silently, with no fault and no corruption.',
    },
    {
        # fc5c width reconciliation. THE PER-SLOT READY BYTES, the fourth
        # band and the one whose breakage was LIVE: dsd splits the four-byte
        # run at 0x0209fc5c into data_0209fc5c (1 byte) and data_0209fc5d
        # (3); 0x0209fc60 is the next object. func_020308d0 (the ROM's seat)
        # writes fc5c[0..3] as bytes, Player::Behavior's VS gate reads
        # fc5c[mPlayerNo] as a byte, _Z19LoadEntranceObjects... keeps or
        # discards each spawned Player on the same byte, and
        # func_ov075_021165b0 walks BOTH names over the one four-byte run.
        #
        # Until the pair moved to hal/scene_vs_menu.cpp it was
        # hal/auto_bss.cpp's generic `int data_0209fc5c[8]`, written at int
        # stride by hal/level_boot.cpp's VS seat: byte fc5c[1] read 0, and
        # player 2 froze in St_LevelEnter_Main on every VS map while the
        # int-view probe insisted the slot was live.
        'key': 'ready',
        'module': 'arm9',
        'start': 0x0209fc5c,
        'end': 0x0209fc60,
        'end_source': 'config',
        'members': 2,
        'hosts': ('smoke_player.map', 'walk_window.map',
                  'walk_window_hires.map'),
        'host': 'port/hal/scene_vs_menu.cpp',
        'what': 'the per-slot ready bytes',
        'remedy':
            'keep the pair in .dsstate$hready0000 / $hready0001 with '
            '__declspec(align(4)) on the head and __declspec(align(1))\n'
            '     on the member, the GXBANK arrangement in '
            'port/hal/cxx_aliases.cpp. Hosting data_0209fc5c anywhere as an\n'
            '     int[] is what this failure looks like: the seat and the '
            'ROM\'s byte readers then walk different memory, and the\n'
            '     symptom is a second player that spawns and never ticks.',
    },
)


# 0001:0003dc80       ?Render@Trap@@QAEHXZ       0043ec80 f   obj.obj
# Row shape and the Rva+Base convention: port/tools/faultmap.py's load_map.
# The flag column is `f`, `f i`, or absent on data rows, and the object may be
# `lib:member.obj`, so it is anchored at end of line.
ROW = re.compile(
    r"^\s+([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})\s+"
    r"((?:[fi]\s+)*)(\S+)\s*$")
#  0004:00000000 00001bc0H .dsstate                DATA
SECT = re.compile(
    r"^\s+([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+([0-9a-fA-F]+)H\s+(\S+)\s+"
    r"(\S+)\s*$")
PREFERRED = re.compile(r"Preferred load address is ([0-9a-fA-F]+)")
# `data_020a6088 kind:bss addr:0x020a6088`
CONFIG_ROW = re.compile(r"^(\S+)\s+kind:\S+\s+addr:0x([0-9a-fA-F]+)")

# config symbols.txt per module, relative to the repo root.
MODULE_SYMBOLS = {
    'arm9': ('config', 'arm9', 'symbols.txt'),
    'itcm': ('config', 'arm9', 'itcm', 'symbols.txt'),
    'dtcm': ('config', 'arm9', 'dtcm', 'symbols.txt'),
}


class Refused(Exception):
    """The guard cannot answer the question it was asked. Never a green."""


def default_root():
    """The checkout this script lives in: <root>/port/tools/gxband_guard.py."""
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


# ---------------------------------------------------------------------------
# config side
# ---------------------------------------------------------------------------

def config_symbols(root, module):
    """[(name, addr)] out of a module's config symbols.txt, address order."""
    rel = MODULE_SYMBOLS.get(module)
    if rel is None:
        raise Refused("unknown module %r in a band declaration; known: %s"
                      % (module, ', '.join(sorted(MODULE_SYMBOLS))))
    path = os.path.join(root, *rel)
    if not os.path.isfile(path):
        raise Refused("no config symbols at %s -- the band's expected deltas "
                      "come from that file and cannot be guessed" % path)
    out = []
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f:
            m = CONFIG_ROW.match(line)
            if m:
                out.append((m.group(1), int(m.group(2), 16)))
    if not out:
        raise Refused("%s parsed to zero symbol rows" % path)
    out.sort(key=lambda r: r[1])
    return out


def band_members(root, band):
    """[(name, addr)] for a band, derived from config, with the declaration
    cross-checked against it."""
    syms = config_symbols(root, band['module'])
    members = [(n, a) for n, a in syms
               if band['start'] <= a < band['end']]
    if not members:
        raise Refused(
            "band %s (0x%08x..0x%08x) has NO symbols in %s's config -- either "
            "the range is wrong or the config moved"
            % (band['key'], band['start'], band['end'], band['module']))
    if members[0][1] != band['start']:
        raise Refused(
            "band %s starts at 0x%08x but the first config symbol in range is "
            "%s at 0x%08x -- the head of a band must be a symbol"
            % (band['key'], band['start'], members[0][0], members[0][1]))
    if len(members) != band['members']:
        raise Refused(
            "band %s declares %d members and the config now yields %d (%s) -- "
            "config moved under the declaration. Re-read the range in %s "
            "before trusting any delta this guard would have asserted."
            % (band['key'], band['members'], len(members),
               ', '.join(n for n, _ in members), os.path.join(
                   *MODULE_SYMBOLS[band['module']])))
    if band['end_source'] == 'config':
        if not any(a == band['end'] for _, a in syms):
            raise Refused(
                "band %s declares its end 0x%08x as config-derived, but no "
                "symbol sits there. Either point end_source at where the "
                "number really comes from or fix the range."
                % (band['key'], band['end']))
    return members


# ---------------------------------------------------------------------------
# map side
# ---------------------------------------------------------------------------

class MapFile(object):
    def __init__(self, path, base, rows, sections):
        self.path = path
        self.base = base
        self.rows = rows              # [(name, va, sect, sect_off, obj, stat)]
        # sections is the map's own section table, which lists ONE ROW PER
        # CONTRIBUTION, not one per image section: .dsstate comes out as 8476
        # rows named .dsstate$aaa, .dsstate$camrec0000, .dsstate$gxbank00 and
        # so on, each with its own start and length. Keeping them all is what
        # lets arm E read a member's SIZE off the map, and it keeps arm D from
        # naming the wrong contribution in a failure message.
        self.sections = sections      # [(idx, start, length, name, klass)]
        self.by_name = {}
        for name, va, sect, sect_off, obj, stat in rows:
            self.by_name.setdefault(name, (va, sect, sect_off, obj, stat))

    def section_end(self, idx):
        """Highest end offset of any contribution in image section idx."""
        ends = [s + l for i, s, l, _n, _k in self.sections if i == idx]
        return max(ends) if ends else None

    def contribution_at(self, idx, off):
        """(name, start, length) of the contribution containing an offset."""
        for i, s, l, n, _k in self.sections:
            if i == idx and s <= off < s + l:
                return (n, s, l)
        return None

    def contribution_starting_at(self, idx, off):
        """(name, length) of the contribution that STARTS at an offset.

        A member laid out by the grouped-section remedy owns its own row, so
        this is where its real size is published. A member sharing a plain
        .dsstate row with hundreds of other objects has no row of its own and
        no size in the map at all.
        """
        for i, s, l, n, _k in self.sections:
            if i == idx and s == off:
                return (n, l)
        return None

    def lookup(self, ds_name):
        """(spelling, va, sect, sect_off, obj) for a DS symbol, or None.

        32-bit MSVC decorates C symbols with a leading underscore, data
        included, so config's data_020a6088 is _data_020a6088 in the map.
        Both spellings are tried and the one that hit is reported, because a
        failure message naming a spelling nobody can find in the map is a
        second bug on top of the first.
        """
        for spelling in (ds_name, '_' + ds_name):
            hit = self.by_name.get(spelling)
            if hit is not None:
                return (spelling,) + hit[:4]
        return None


def parse_map(path):
    """MapFile for an MSVC /MAP, or Refused.

    The refusals are closure.map_defined's, for its reason: a failed link
    truncates or zeroes the map, and a guard that measures the wreckage
    reports green over a build that does not exist.
    """
    if not os.path.isfile(path):
        raise Refused("no map at %s -- the link did not run" % path)
    size = os.path.getsize(path)
    if size == 0:
        raise Refused("%s is ZERO BYTES -- a failed link truncates the map, "
                      "and a band cannot be checked against nothing" % path)
    base = None
    rows = []
    sections = []
    in_publics = False
    after_trailer = False
    saw_trailer = False
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f:
            if base is None:
                m = PREFERRED.search(line)
                if m:
                    base = int(m.group(1), 16)
                    continue
            if 'Publics by Value' in line:
                in_publics = True
                continue
            if line.lstrip().startswith('entry point at'):
                saw_trailer = True
                after_trailer = True
                continue
            if not in_publics:
                m = SECT.match(line)
                if m:
                    sections.append((int(m.group(1), 16), int(m.group(2), 16),
                                     int(m.group(3), 16), m.group(4),
                                     m.group(5)))
                continue
            m = ROW.match(line)
            if not m:
                continue
            sect, sect_off, sym, va, _flags, obj = m.groups()
            rows.append((sym, int(va, 16), int(sect, 16), int(sect_off, 16),
                         obj, after_trailer))
    if base is None:
        raise Refused("no 'Preferred load address' header in %s -- that is not "
                      "an MSVC /MAP" % path)
    if not rows:
        raise Refused("%s is %d bytes with NO symbol rows -- a stub from a "
                      "failed link, not a map to check" % (path, size))
    if not saw_trailer:
        raise Refused("%s has symbol rows but no 'entry point at' trailer, so "
                      "it stopped mid-write -- a TRUNCATED map. Relink."
                      % path)
    return MapFile(path, base, rows, sections)


# ---------------------------------------------------------------------------
# the four arms
# ---------------------------------------------------------------------------

def check_band(band, members, mp):
    """(status, failures, sized) for one band against one map.

    status is 'ok' or 'skipped'; failures is a list of prose lines; sized is
    how many members arm E could read a size for. A band whose head is absent
    is SKIPPED: that target does not host it.
    """
    head_name, head_addr = members[0]
    head = mp.lookup(head_name)
    if head is None:
        return 'skipped', [], 0

    fails = []
    _, head_va, head_sect, head_off, _obj = head
    span = band['end'] - band['start']

    # A: presence, and B: consecutive deltas.
    placed = [(head_name, head_addr, head_va)]
    for name, addr in members[1:]:
        hit = mp.lookup(name)
        if hit is None:
            fails.append(
                "MEMBER MISSING: %s (DS 0x%08x, +0x%x into the band) is not in "
                "the map at all.\n"
                "     Its host definition is gone or was not linked, so the "
                "band has a hole no delta can measure."
                % (name, addr, addr - head_addr))
            continue
        placed.append((name, addr, hit[1]))
    for i in range(1, len(placed)):
        pname, paddr, pva = placed[i - 1]
        name, addr, va = placed[i]
        want = addr - paddr
        got = va - pva
        if got != want:
            fails.append(
                "DELTA WRONG: %s -> %s is %s bytes in the link, %d in the ROM"
                ".\n"
                "     config says 0x%08x -> 0x%08x (+%d); the map has 0x%08x "
                "-> 0x%08x (%+d)."
                % (pname, name, got, want, paddr, addr, want, pva, va, got))

    # C: exclusion. Nothing else may sit strictly inside the span. An address
    # EQUAL to a member's is not an intruder: a fired /alternatename or an
    # MSVC-decorated C++ alias of the same storage publishes a second name at
    # the same address, and that is the healthy shape.
    member_vas = set(va for _, _, va in placed)
    lo, hi = head_va, head_va + span
    for name, va, sect, sect_off, obj, stat in mp.rows:
        if lo < va < hi and va not in member_vas:
            fails.append(
                "INTRUDER IN THE BAND: %s sits at +%d inside %s's %d-byte "
                "span%s.\n"
                "     It is in %s. The band's own storage does not reach that "
                "far, so every write past +%d lands on that symbol."
                % (name, va - head_va, head_name, span,
                   ' (a static)' if stat else '', obj, va - head_va))

    # D: containment. The floor under arm C for a band that is last in its
    # section and so has nothing after it to collide with.
    end_off = mp.section_end(head_sect)
    if end_off is None:
        fails.append(
            "NO SECTION ROWS for image section %04x, which is where %s landed "
            "-- the map's section table did not parse, so the span cannot be "
            "proved to fit." % (head_sect, head_name))
    else:
        room = end_off - head_off
        if room < span:
            where = mp.contribution_at(head_sect, head_off)
            fails.append(
                "BAND OVERRUNS ITS SECTION: %s needs %d bytes and image "
                "section %04x has %d left after it.\n"
                "     %s is at %04x:%08x (contribution %s) and the section "
                "ends at %08x. Nothing follows the band, so no intruder was\n"
                "     caught, and the storage still is not there."
                % (head_name, span, head_sect, room, head_name, head_sect,
                   head_off, where[0] if where else '?', end_off))

    # E: contribution size, where the map publishes one. The grouped-section
    # remedy gives every member its OWN row in the map's section table
    # (.dsstate$gxbank00 and friends), and that row carries the member's real
    # SIZE rather than just its address. Where a member has such a row, its
    # size is checked against the config delta directly -- which is how the
    # band's LAST delta gets measured rather than only inferred: nothing
    # follows data_020a60a0 for arm B to measure, but .dsstate$gxbank12's row
    # says four bytes. A member sharing a plain .dsstate row with hundreds of
    # other objects has no size in the map and is silently out of this arm's
    # reach; the count is reported so the coverage is visible.
    sized = 0
    bounds = [a for _, a in members] + [band['end']]
    for i, (name, addr) in enumerate(members):
        hit = mp.lookup(name)
        if hit is None:
            continue
        _, _va, msect, moff, _obj = hit
        own = mp.contribution_starting_at(msect, moff)
        if own is None:
            continue
        sized += 1
        want = bounds[i + 1] - addr
        if own[1] < want:
            fails.append(
                "MEMBER TOO SMALL: %s owns contribution %s, which is %d "
                "byte(s); the ROM gives it %d.\n"
                "     config has 0x%08x..0x%08x. A member hosted shorter than "
                "its ROM span puts the tail of every write to it on the next "
                "symbol."
                % (name, own[0], own[1], want, addr, bounds[i + 1]))
    return 'ok', fails, sized


def report_failure(band, mp, fails, out):
    out.write("\n")
    out.write("gxband_guard: BAND BROKEN -- %s, %s (0x%08x..0x%08x, %d bytes)\n"
              % (band['key'], band['what'], band['start'], band['end'],
                 band['end'] - band['start']))
    out.write("  map: %s\n" % mp.path)
    out.write("  laid out by: %s\n" % band['host'])
    for f in fails:
        out.write("   * %s\n" % f)
    out.write("  WHY IT MATTERS: the DS reaches these as ONE object -- a "
              "member is an interior address of the head, and\n"
              "     every caller that writes by struct offset writes at the "
              "ROM's offset whatever the linker did here.\n")
    out.write("  REMEDY: %s\n" % band['remedy'])
    out.write("  SPAN PROVENANCE: %s\n"
              % ("config %s (both ends are symbol rows)"
                 % os.path.join(*MODULE_SYMBOLS[band['module']])
                 if band['end_source'] == 'config' else band['end_source']))


def run(root, maps, bands=BANDS, verbose=False, out=sys.stdout, sweep=False):
    """0 when every declared band is intact in every map that hosts it.

    sweep says the caller handed over EVERY map in a build directory, which is
    what makes an absent floor map meaningful: on a partial run a floor map
    that was never read says nothing, and on a sweep it says the target is
    gone. A floor map that WAS read and does not host the band fails either
    way.
    """
    try:
        resolved = [(b, band_members(root, b)) for b in bands]
    except Refused as e:
        out.write("gxband_guard: REFUSED -- %s\n" % e)
        return 2
    if not maps:
        out.write("gxband_guard: REFUSED -- no maps to check. A guard with "
                  "nothing to read is not a green.\n")
        return 2

    parsed = []
    for path in maps:
        try:
            parsed.append(parse_map(path))
        except Refused as e:
            out.write("gxband_guard: REFUSED -- %s\n" % e)
            return 2

    broken = 0
    floors = 0
    summary = []
    read_names = set(os.path.basename(mp.path) for mp in parsed)
    for band, members in resolved:
        hosts = 0
        deltas = 0
        hosted_names = set()
        for mp in parsed:
            status, fails, sized = check_band(band, members, mp)
            if status == 'skipped':
                if verbose:
                    out.write("  %-14s %-28s SKIPPED (does not host %s)\n"
                              % (band['key'], os.path.basename(mp.path),
                                 members[0][0]))
                continue
            hosts += 1
            hosted_names.add(os.path.basename(mp.path))
            deltas += len(members) - 1
            if fails:
                report_failure(band, mp, fails, out)
                broken += 1
            elif verbose:
                out.write("  %-14s %-28s OK (%d members, %d measured deltas, "
                          "%d sized, %d-byte span clear)\n"
                          % (band['key'], os.path.basename(mp.path),
                             len(members), len(members) - 1, sized,
                             band['end'] - band['start']))
        if hosts == 0:
            out.write(
                "gxband_guard: REFUSED -- NO map defines %s, the head of band "
                "%s.\n  %d map(s) read and not one hosts the band, so this "
                "guard checked nothing. Either the host TU stopped being "
                "linked\n  or the band was renamed; both are changes this "
                "guard must not read as green.\n"
                % (members[0][0], band['key'], len(parsed)))
            return 2
        # THE COVERAGE FLOOR. A band silently leaving a target is a coverage
        # regression that every other arm reads as green, because a map that
        # does not host a band is correctly skipped. The floor is what makes
        # the difference between "this target never had it" and "this target
        # just lost it".
        floor = tuple(band.get('hosts', ()))
        lost = [n for n in floor if n in read_names and n not in hosted_names]
        absent = [n for n in floor if n not in read_names]
        if lost or (sweep and absent):
            out.write(
                "\ngxband_guard: COVERAGE FLOOR BROKEN -- band %s is declared "
                "hosted by %d map(s) and came back with %d.\n"
                % (band['key'], len(floor), len(hosted_names & set(floor))))
            for n in lost:
                out.write("   * %s was read and no longer defines %s\n"
                          % (n, members[0][0]))
            for n in (absent if sweep else []):
                out.write("   * %s is not in the build at all\n" % n)
            out.write(
                "  Either the band left that target on purpose, in which case "
                "narrow the `hosts` floor in this file and\n"
                "  say in the commit which measurement says so, or a refactor "
                "dropped it and the target is now shipping an\n"
                "  unchecked band. The floor exists because a shrink from 3 "
                "hosting maps to 2 is otherwise a green.\n")
            floors += 1
        summary.append("%s %d/%d maps (floor %d), %d members, %d deltas"
                       % (band['key'], hosts, len(parsed), len(floor),
                          len(members), deltas))
        if not broken and not floors and verbose:
            out.write("  %-14s hosted by %d of %d maps, %d deltas asserted\n"
                      % (band['key'], hosts, len(parsed), deltas))

    if broken or floors:
        parts = []
        if broken:
            parts.append("%d band/map pair(s) BROKEN" % broken)
        if floors:
            parts.append("%d coverage floor(s) BROKEN" % floors)
        out.write("\ngxband_guard: %s.\n" % ', '.join(parts))
        return 1
    if not verbose:
        out.write("gxband_guard: %s -- layout OK\n" % '; '.join(summary))
    return 0


def find_maps(build_dir):
    """Every .map in the build directory that still has its executable.

    ORPHANS ARE SKIPPED, and that is a stale-artifact rule rather than
    tidiness. A target that gets renamed or deleted leaves its last .map
    behind in the build directory forever, and a guard that reads it is
    failing (or passing) a build over a link nobody ran today. The exe beside
    it is what makes a map this build's. Top level only: CMake writes the
    executables and their maps at the root of the binary directory, and
    walking deeper would pick up try_compile scratch.
    """
    if not os.path.isdir(build_dir):
        raise Refused("no build directory at %s" % build_dir)
    out, orphans = [], []
    for name in sorted(os.listdir(build_dir)):
        if not name.lower().endswith('.map'):
            continue
        path = os.path.join(build_dir, name)
        if os.path.isfile(path[:-4] + '.exe'):
            out.append(os.path.normpath(path))
        else:
            orphans.append(name)
    if not out:
        raise Refused("no .map files with an executable beside them in %s -- "
                      "/MAP is on CMAKE_EXE_LINKER_FLAGS, so a build with no "
                      "maps is a build that did not link%s"
                      % (build_dir,
                         " (%d orphan map(s) ignored)" % len(orphans)
                         if orphans else ""))
    return out, orphans


# ---------------------------------------------------------------------------
# selftest
# ---------------------------------------------------------------------------

MAP_HEAD = """\
 walk_window

 Timestamp is 68a00000 (Sat Aug 16 00:00:00 2026)

 Preferred load address is 00400000

 Start         Length     Name                   Class
"""

MAP_PUBHDR = """
  Address         Publics by Value              Rva+Base       Lib:Object

"""

MAP_TRAILER = """
 entry point at        0001:00000000

 Static symbols

"""


def _fabmap(path, sections, publics, statics=(), trailer=True, header=True):
    """A fabricated real-shaped map.

    sections: [(index, name, start_off, length, klass)]
    publics/statics: [(name, index, sect_off, va, obj)]
    """
    text = MAP_HEAD if header else " walk_window\n\n"
    for idx, name, start, length, klass in sections:
        text += " %04x:%08x %08xH %-22s %s\n" % (idx, start, length, name,
                                                 klass)
    text += MAP_PUBHDR
    for name, idx, off, va, obj in publics:
        text += " %04x:%08x       %-30s %08x     %s\n" % (idx, off, name, va,
                                                          obj)
    if trailer:
        text += MAP_TRAILER
        for name, idx, off, va, obj in statics:
            text += " %04x:%08x       %-30s %08x     %s\n" % (idx, off, name,
                                                              va, obj)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(text)
    return path


def _fabconfig(root, module, rows):
    """Write a fabricated config symbols.txt. rows: [(name, addr)]."""
    path = os.path.join(root, *MODULE_SYMBOLS[module])
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w', encoding='utf-8') as f:
        for name, addr in rows:
            f.write("%s kind:bss addr:0x%08x\n" % (name, addr))
    return path


class _Out(object):
    def __init__(self):
        self.buf = []

    def write(self, s):
        self.buf.append(s)

    def text(self):
        return ''.join(self.buf)


def selftest():
    """Every arm proven by a mutation that only that arm can catch.

    The fixture is the GX band's real shape at a fabricated address: a head, a
    run of two-byte members and a four-byte tail, hosted contiguously in a
    fabricated .dsstate. Each arm gets a healthy control and a break, and the
    breaks are the ones the tree has actually produced -- a member split back
    out of the grouped section (SUB2), an object sized by its first caller
    (IRQ2), a host TU that stopped being linked, and a config that moved under
    the declaration.
    """
    passed = failed = 0
    # QUIET ON GREEN, like every other selftest in build-port.cmd's
    # pre-configure block: one line. On a failure the whole battery prints,
    # the failing arms with the output they were judging.
    log = []

    def case(name, want_rc, rc, out, wants=(), unwanted=()):
        nonlocal passed, failed
        why = []
        if rc != want_rc:
            why.append("rc %s, wanted %s" % (rc, want_rc))
        for w in wants:
            if w not in out:
                why.append("output missing %r" % w)
        for u in unwanted:
            if u in out:
                why.append("output should not contain %r" % u)
        if why:
            failed += 1
            log.append("  FAIL %-38s %s" % (name, '; '.join(why)))
            log.append("       ---- what it was judging ----")
            for line in out.rstrip().splitlines():
                log.append("       %s" % line)
        else:
            passed += 1
            log.append("  ok   %s" % name)

    def go(root, maps, bands, verbose=False):
        o = _Out()
        rc = run(root, maps, bands=bands, verbose=verbose, out=o)
        return rc, o.text()

    tmp = tempfile.mkdtemp(prefix='gxband_selftest_')
    try:
        # --- the fixture band: 13 members, 12 x u16 then a u32 tail --------
        start = 0x020a6088
        addrs = [start + 2 * i for i in range(12)] + [start + 24]
        end = start + 28
        cfg_rows = ([('data_%08x' % (start - 4), start - 4)] +
                    [('data_%08x' % a, a) for a in addrs] +
                    [('data_%08x' % end, end),
                     ('data_%08x' % (end + 4), end + 4)])
        _fabconfig(tmp, 'arm9', cfg_rows)
        _fabconfig(tmp, 'dtcm', [('data_023c0000', 0x023c0000)])

        gx = dict(BANDS[0])
        gx['members'] = 13
        dtcm = dict(BANDS[1])
        bands_gx = (gx,)
        bands_dtcm = (dtcm,)

        # host layout: .dsstate section 0004, band head at offset 0x100
        HEAD_OFF = 0x100
        VA = 0x00404000

        def band_rows(offsets):
            """offsets: [(member index, byte offset from head)]."""
            return [('_data_%08x' % addrs[i], 4, HEAD_OFF + off,
                     VA + HEAD_OFF + off, 'cxx_aliases.cpp.obj')
                    for i, off in offsets]

        healthy = [(i, a - start) for i, a in enumerate(addrs)]
        # The SHARED shape: one bulk contribution row, the way the map lists
        # plain .dsstate. No member owns a row, so arm E is silent here and
        # arms A-D carry the fixtures below.
        SECTIONS = [(1, '.text', 0, 0x1000, 'CODE'),
                    (4, '.dsstate', 0, 0x1000, 'DATA')]
        FILLER = [('_data_0208e6ec', 4, 0x400, VA + 0x400, 'other.obj')]

        def grouped(offsets, sizes):
            """The GROUPED shape: a row per member, which is what the remedy
            produces and what arm E reads sizes off."""
            rows = [(1, '.text', 0, 0x1000, 'CODE'),
                    (4, '.dsstate$bulk', 0, HEAD_OFF, 'DATA')]
            for i, off in offsets:
                rows.append((4, '.dsstate$gxbank%02d' % i, HEAD_OFF + off,
                             sizes[i], 'DATA'))
            rows.append((4, '.dsstate$after', HEAD_OFF + 0x40,
                         0x1000 - HEAD_OFF - 0x40, 'DATA'))
            return rows

        SIZES_OK = [2] * 12 + [4]

        # 1. HEALTHY -------------------------------------------------------
        m = _fabmap(os.path.join(tmp, 'healthy.map'), SECTIONS,
                    band_rows(healthy) + FILLER)
        rc, out = go(tmp, [m], bands_gx)
        case('healthy band passes', 0, rc, out,
             wants=['layout OK'])

        # 1b. the same map, verbose, says what it checked -------------------
        rc, out = go(tmp, [m], bands_gx, verbose=True)
        case('verbose names members and deltas', 0, rc, out,
             wants=['13 members, 12 measured deltas', '28-byte span clear'])

        # 2. ARM B: a member split back out of the grouped section ----------
        #    SUB2's negative test mis-aligned one member and moved 609c to
        #    +26 and 60a0 to +30 -- padding inserted before 609c, so
        #    everything from there on slides and exactly ONE pair reads
        #    wrong. That is the shape reproduced here (+4 from member 10 on).
        split = [(i, off if i < 10 else off + 4) for i, off in healthy]
        m = _fabmap(os.path.join(tmp, 'split.map'), SECTIONS,
                    band_rows(split) + FILLER)
        rc, out = go(tmp, [m], bands_gx)
        case('arm B catches a slid member', 1, rc, out,
             wants=['DELTA WRONG', 'data_020a609a -> data_020a609c',
                    '6 bytes in the link, 2 in the ROM', 'cxx_aliases.cpp'])

        # 2b. one member displaced ALONE breaks the pair on each side of it,
        #     and both must be named: a guard that stops at the first bad
        #     delta hides how far the damage runs.
        lone = [(i, off + 4 if i == 5 else off) for i, off in healthy]
        m = _fabmap(os.path.join(tmp, 'lone.map'), SECTIONS,
                    band_rows(lone) + FILLER)
        rc, out = go(tmp, [m], bands_gx)
        case('arm B reports every broken pair', 1, rc, out,
             wants=['data_020a6090 -> data_020a6092',
                    'data_020a6092 -> data_020a6094',
                    '-2 bytes in the link'])

        # 3. ARM B: the two-byte head, the original bug ---------------------
        #    Head hosted as its own object, the rest packed elsewhere: the
        #    first delta is enormous.
        apart = [(0, 0)] + [(i, 0x800 + (a - start)) for i, a in
                            list(enumerate(addrs))[1:]]
        m = _fabmap(os.path.join(tmp, 'apart.map'), SECTIONS,
                    band_rows(apart) + FILLER)
        rc, out = go(tmp, [m], bands_gx)
        case('arm B catches the head hosted alone', 1, rc, out,
             wants=['DELTA WRONG', 'data_020a6088 -> data_020a608a'])

        # 4. ARM A: a member that is not in the map at all ------------------
        gone = [(i, off) for i, off in healthy if i != 7]
        m = _fabmap(os.path.join(tmp, 'gone.map'), SECTIONS,
                    band_rows(gone) + FILLER)
        rc, out = go(tmp, [m], bands_gx)
        case('arm A catches a missing member', 1, rc, out,
             wants=['MEMBER MISSING', 'data_020a6096'])

        # 5. ARM C: a foreign symbol inside the span ------------------------
        #    +26 is inside data_020a60a0's four-byte tail, which is the one
        #    delta arm B cannot measure (no member follows it). This is the
        #    thirteenth delta being asserted, and the casualty name is the
        #    one the SUB2 review read off the base map.
        m = _fabmap(os.path.join(tmp, 'intruder.map'), SECTIONS,
                    band_rows(healthy) + FILLER +
                    [('_data_020a8114', 4, HEAD_OFF + 26, VA + HEAD_OFF + 26,
                      'player_bridges.cpp.obj')])
        rc, out = go(tmp, [m], bands_gx)
        case('arm C catches an intruder in the tail', 1, rc, out,
             wants=['INTRUDER IN THE BAND', 'data_020a8114', '+26'])

        # 5a. THE hi-1 BOUND, both sides. The reviewer's miss: every intruder
        #     fixture sat comfortably inside the span, so an off-by-one at the
        #     top would have survived. +27 is the last byte of the band and
        #     must fail; +28 is the first byte after it and must pass.
        m = _fabmap(os.path.join(tmp, 'lastbyte.map'), SECTIONS,
                    band_rows(healthy) + FILLER +
                    [('_data_020a8114', 4, HEAD_OFF + 27, VA + HEAD_OFF + 27,
                      'player_bridges.cpp.obj')])
        rc, out = go(tmp, [m], bands_gx)
        case('arm C catches the LAST byte of the span', 1, rc, out,
             wants=['INTRUDER IN THE BAND', '+27'])
        m = _fabmap(os.path.join(tmp, 'pastend.map'), SECTIONS,
                    band_rows(healthy) + FILLER +
                    [('_data_020a8114', 4, HEAD_OFF + 28, VA + HEAD_OFF + 28,
                      'player_bridges.cpp.obj')])
        rc, out = go(tmp, [m], bands_gx)
        case('the byte AFTER the span is not an intruder', 0, rc, out,
             wants=['layout OK'], unwanted=['INTRUDER'])

        # 5b. a STATIC intruder counts too ----------------------------------
        m = _fabmap(os.path.join(tmp, 'static_intruder.map'), SECTIONS,
                    band_rows(healthy) + FILLER,
                    statics=[('_pk001_gap_02000000', 4, HEAD_OFF + 26,
                              VA + HEAD_OFF + 26, 'ov002_data.c.obj')])
        rc, out = go(tmp, [m], bands_gx)
        case('arm C sees static symbols', 1, rc, out,
             wants=['INTRUDER IN THE BAND', 'pk001_gap', '(a static)'])

        # 5b2. AN INTRUDER OUT OF THE BAND'S OWN OBJECT. cxx_aliases.cpp is
        #      where a new hosted global is likeliest to be added, so the
        #      object column must never become an excuse. Same failure as any
        #      other intruder, and the report names the object either way.
        #      The name is invented rather than borrowed: a fixture that spells
        #      a real symbol next to the wrong object reads as a claim about
        #      where that symbol lives.
        m = _fabmap(os.path.join(tmp, 'ownobj.map'), SECTIONS,
                    band_rows(healthy) + FILLER +
                    [('_data_02fixture', 4, HEAD_OFF + 26, VA + HEAD_OFF + 26,
                      'cxx_aliases.cpp.obj')])
        rc, out = go(tmp, [m], bands_gx)
        case('an intruder from the band own object still fails', 1, rc, out,
             wants=['INTRUDER IN THE BAND', 'cxx_aliases.cpp.obj',
                    'data_02fixture'])

        # 5c. an ALIAS at a member address is NOT an intruder ---------------
        #     A fired /alternatename publishes the LHS at the RHS's address,
        #     and MSVC publishes ?data_020a608c@@3PAEA beside the C spelling.
        #     Reading either as an intruder would fail every real build.
        m = _fabmap(os.path.join(tmp, 'alias.map'), SECTIONS,
                    band_rows(healthy) + FILLER +
                    [('?data_020a608c@@3PAEA', 4, HEAD_OFF + 4,
                      VA + HEAD_OFF + 4, 'cxx_aliases.cpp.obj')])
        rc, out = go(tmp, [m], bands_gx)
        case('an alias at a member address passes', 0, rc, out,
             wants=['layout OK'], unwanted=['INTRUDER'])

        # 6. ARM D: undersized and LAST in its section ----------------------
        #    The hole arm C cannot see. Section is only 8 bytes longer than
        #    the head's offset, so 28 bytes cannot be there, and there is
        #    nothing after the band to collide with.
        tight = [(1, '.text', 0, 0x1000, 'CODE'),
                 (4, '.dsstate', 0, HEAD_OFF + 8, 'DATA')]
        m = _fabmap(os.path.join(tmp, 'tail.map'), tight,
                    [('_data_%08x' % addrs[0], 4, HEAD_OFF, VA + HEAD_OFF,
                      'cxx_aliases.cpp.obj')] +
                    [('_data_0208e6ec', 1, 0x10, VA - 0x1000, 'other.obj')])
        rc, out = go(tmp, [m], bands_gx)
        case('arm D catches a band past its section end', 1, rc, out,
             wants=['BAND OVERRUNS ITS SECTION', '.dsstate', 'needs 28 bytes'])

        # 6b. ARM E: the grouped layout publishes every member's size --------
        m = _fabmap(os.path.join(tmp, 'grouped.map'), grouped(healthy,
                                                              SIZES_OK),
                    band_rows(healthy) + FILLER)
        rc, out = go(tmp, [m], bands_gx, verbose=True)
        case('arm E sizes every member when grouped', 0, rc, out,
             wants=['13 sized'])

        # 6c. ARM E is the only arm that can see a short TAIL member. Nothing
        #     follows data_020a60a0 for arm B to measure and nothing is
        #     wedged in for arm C to trip on; the map's own contribution row
        #     is the whole evidence. This is the thirteenth delta.
        m = _fabmap(os.path.join(tmp, 'shorttail.map'),
                    grouped(healthy, [2] * 13),
                    band_rows(healthy) + FILLER)
        rc, out = go(tmp, [m], bands_gx)
        case('arm E catches a short tail member', 1, rc, out,
             wants=['MEMBER TOO SMALL', 'data_020a60a0',
                    'which is 2 byte(s); the ROM gives it 4'],
             unwanted=['DELTA WRONG', 'INTRUDER', 'OVERRUNS'])

        # 6d. NON-UNIFORM DELTAS, which no real fixture here pins. Every GX
        #     delta is 2, so a guard that compared against a constant 2
        #     instead of against the config delta would pass every arm above.
        #     This band's config deltas are 2, 4, 2 and 8, and the break is a
        #     map laid out at a uniform 2: it can only fail if the expected
        #     numbers really came out of the config file.
        nu = os.path.join(tmp, 'nonuniform')
        os.makedirs(nu, exist_ok=True)
        NU_START, NU_OFFS = 0x02100000, [0, 2, 6, 8, 16]
        NU_END = NU_START + 20
        _fabconfig(nu, 'arm9',
                   [('data_%08x' % (NU_START + o), NU_START + o)
                    for o in NU_OFFS] + [('data_%08x' % NU_END, NU_END)])
        nu_band = dict(BANDS[0], key='nonuniform', start=NU_START, end=NU_END,
                       members=5, hosts=())

        def nu_rows(offsets):
            return [('_data_%08x' % (NU_START + NU_OFFS[i]), 4, HEAD_OFF + off,
                     VA + HEAD_OFF + off, 'cxx_aliases.cpp.obj')
                    for i, off in enumerate(offsets)]

        m = _fabmap(os.path.join(tmp, 'nonuni_ok.map'), SECTIONS,
                    nu_rows(NU_OFFS) + FILLER)
        rc, out = go(nu, [m], (nu_band,), verbose=True)
        case('non-uniform deltas pass when laid out to config', 0, rc, out,
             wants=['5 members, 4 measured deltas'])
        m = _fabmap(os.path.join(tmp, 'nonuni_flat.map'), SECTIONS,
                    nu_rows([0, 2, 4, 6, 8]) + FILLER)
        rc, out = go(nu, [m], (nu_band,))
        case('a uniform layout FAILS a non-uniform band', 1, rc, out,
             wants=['DELTA WRONG', 'data_02100002 -> data_02100006',
                    '2 bytes in the link, 4 in the ROM'])

        # 6e. THE COVERAGE FLOOR: a band that quietly leaves a target is a
        #     regression every other arm reads as green, because a map that
        #     does not host a band is correctly skipped.
        floored = dict(gx, hosts=('healthy.map', 'gone_target.map'))
        good = _fabmap(os.path.join(tmp, 'healthy.map'), SECTIONS,
                       band_rows(healthy) + FILLER)
        nohost2 = _fabmap(os.path.join(tmp, 'gone_target.map'), SECTIONS,
                          FILLER)
        rc, out = go(tmp, [good, nohost2], (floored,))
        case('a floor map that stopped hosting FAILS', 1, rc, out,
             wants=['COVERAGE FLOOR BROKEN',
                    'gone_target.map was read and no longer defines'])
        o = _Out()
        rc = run(tmp, [good], bands=(floored,), out=o, sweep=True)
        case('a floor map missing from a SWEEP fails', 1, rc, o.text(),
             wants=['COVERAGE FLOOR BROKEN', 'not in the build at all'])
        o = _Out()
        rc = run(tmp, [good], bands=(floored,), out=o, sweep=False)
        case('a partial run does not judge absent floor maps', 0, rc, o.text(),
             wants=['layout OK'], unwanted=['COVERAGE FLOOR'])

        # 7. THE DTCM SHAPE: one member, the whole check is C ---------------
        DT_OFF = 0x2000
        DT_VA = 0x00500000
        dt_sections = [(4, '.dsstate', 0, 0x8000, 'DATA')]
        m = _fabmap(os.path.join(tmp, 'dtcm_ok.map'), dt_sections,
                    [('_data_023c0000', 4, DT_OFF, DT_VA, 'pb.cpp.obj'),
                     ('_hal_area_table', 4, DT_OFF + 0x4000, DT_VA + 0x4000,
                      'pb.cpp.obj')])
        rc, out = go(tmp, [m], bands_dtcm)
        case('dtcm at full extent passes', 0, rc, out, wants=['layout OK'])

        #    and the real bug: 64 bytes, the neighbour 16,312 bytes early.
        m = _fabmap(os.path.join(tmp, 'dtcm_short.map'), dt_sections,
                    [('_data_023c0000', 4, DT_OFF, DT_VA, 'pb.cpp.obj'),
                     ('_hal_area_table', 4, DT_OFF + 0x40, DT_VA + 0x40,
                      'pb.cpp.obj')])
        rc, out = go(tmp, [m], bands_dtcm)
        case('dtcm hosted at 64 bytes FAILS', 1, rc, out,
             wants=['INTRUDER IN THE BAND', '_hal_area_table', '+64',
                    'DTCM_END - 8'])

        # 8. REFUSALS -------------------------------------------------------
        #    a map that hosts nothing must never read green
        m = _fabmap(os.path.join(tmp, 'nohost.map'), SECTIONS, FILLER)
        rc, out = go(tmp, [m], bands_gx)
        case('no map hosts the band -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'checked nothing'])

        #    ...but a map that hosts nothing is fine ALONGSIDE one that does
        good = _fabmap(os.path.join(tmp, 'healthy2.map'), SECTIONS,
                       band_rows(healthy) + FILLER)
        rc, out = go(tmp, [m, good], bands_gx, verbose=True)
        case('a non-hosting map is skipped, not fatal', 0, rc, out,
             wants=['SKIPPED', 'hosted by 1 of 2 maps'])

        #    truncated map: rows but no trailer
        m = _fabmap(os.path.join(tmp, 'trunc.map'), SECTIONS,
                    band_rows(healthy), trailer=False)
        rc, out = go(tmp, [m], bands_gx)
        case('truncated map -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'TRUNCATED'])

        #    zero-byte map
        z = os.path.join(tmp, 'zero.map')
        open(z, 'w').close()
        rc, out = go(tmp, [z], bands_gx)
        case('zero-byte map -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'ZERO BYTES'])

        #    not an MSVC map at all
        m = _fabmap(os.path.join(tmp, 'noheader.map'), SECTIONS,
                    band_rows(healthy), header=False)
        rc, out = go(tmp, [m], bands_gx)
        case('no load-address header -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'Preferred load address'])

        #    no maps at all
        rc, out = go(tmp, [], bands_gx)
        case('no maps -> REFUSED', 2, rc, out, wants=['REFUSED'])

        # 9. CONFIG DRIFT ---------------------------------------------------
        #    a config row appearing inside a declared band must refuse, not
        #    silently assert a different set of deltas
        drift = os.path.join(tmp, 'drift')
        os.makedirs(drift, exist_ok=True)
        _fabconfig(drift, 'arm9', cfg_rows + [('data_020a6099', start + 17)])
        good = _fabmap(os.path.join(tmp, 'healthy3.map'), SECTIONS,
                       band_rows(healthy) + FILLER)
        rc, out = go(drift, [good], bands_gx)
        case('a config row inside the band -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'declares 13 members', 'yields 14'])

        #    a band whose end is claimed config-derived but is not a symbol
        noend = os.path.join(tmp, 'noend')
        os.makedirs(noend, exist_ok=True)
        _fabconfig(noend, 'arm9', [(n, a) for n, a in cfg_rows if a != end])
        rc, out = go(noend, [good], bands_gx)
        case('config-derived end with no symbol -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'end_source'])

        #    a missing config file is a refusal, not an empty band
        empty = os.path.join(tmp, 'empty')
        os.makedirs(empty, exist_ok=True)
        rc, out = go(empty, [good], bands_gx)
        case('missing config symbols.txt -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'cannot be guessed'])

        # 9b. THE ORPHAN RULE in find_maps: a map whose target is gone is not
        #     this build's, and reading it fails (or passes) a build over a
        #     link nobody ran today.
        bd = os.path.join(tmp, 'bdir')
        os.makedirs(bd, exist_ok=True)
        live = _fabmap(os.path.join(bd, 'walk_window.map'), SECTIONS,
                       band_rows(healthy) + FILLER)
        open(os.path.join(bd, 'walk_window.exe'), 'wb').close()
        _fabmap(os.path.join(bd, 'smoke_deleted.map'), SECTIONS,
                band_rows(split) + FILLER)          # a BROKEN orphan
        found, orphans = find_maps(bd)
        o = _Out()
        o.write("found=%s orphans=%s" % ([os.path.basename(f) for f in found],
                                         orphans))
        case('an orphan map is not read', 0, 0, o.text(),
             wants=["found=['walk_window.map']", "orphans=['smoke_deleted"])
        rc, out = go(tmp, found, bands_gx)
        case('the live map alone decides', 0, rc, out, wants=['layout OK'])

        #     ...and a directory of nothing but orphans REFUSES
        bd2 = os.path.join(tmp, 'bdir2')
        os.makedirs(bd2, exist_ok=True)
        _fabmap(os.path.join(bd2, 'gone.map'), SECTIONS, band_rows(healthy))
        try:
            find_maps(bd2)
            case('all-orphan build dir -> REFUSED', 2, 0, 'no refusal')
        except Refused as e:
            case('all-orphan build dir -> REFUSED', 2, 2, "REFUSED %s" % e,
                 wants=['orphan map(s) ignored'])

        # 10. THE DECLARATION ITSELF, against the real config ---------------
        #     Not a fixture: the shipped BANDS must resolve against this
        #     tree's config, or the guard is dead on arrival.
        real_root = default_root()
        for band in BANDS:
            try:
                members = band_members(real_root, band)
                ok = len(members) == band['members']
                log.append("  %s band %s resolves: %d members, 0x%08x..0x%08x"
                           % ('ok  ' if ok else 'FAIL', band['key'],
                              len(members), band['start'], band['end']))
                passed += 1 if ok else 0
                failed += 0 if ok else 1
            except Refused as e:
                failed += 1
                log.append("  FAIL band %s does not resolve: %s"
                           % (band['key'], e))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)

    if failed:
        print('\n'.join(log))
    print("selftest %s (%d arms, %d failed)"
          % ("PASS" if not failed else "FAIL", passed + failed, failed))
    return 1 if failed else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--map', action='append', default=[],
                    help='a linked .map to check; repeatable')
    ap.add_argument('--build-dir',
                    help='check every .map in this directory (every target)')
    ap.add_argument('--root', default=default_root(),
                    help='repo root holding config/arm9/symbols.txt')
    ap.add_argument('--verbose', action='store_true',
                    help='one line per band per map, including skips')
    ap.add_argument('--selftest', action='store_true',
                    help='run the fixture battery and exit')
    args = ap.parse_args()

    if args.selftest:
        return selftest()

    maps = [os.path.normpath(m) for m in args.map]
    sweep = False
    if args.build_dir:
        try:
            found, orphans = find_maps(args.build_dir)
        except Refused as e:
            print("gxband_guard: REFUSED -- %s" % e)
            return 2
        maps += found
        sweep = True
        if orphans and args.verbose:
            print("  %d orphan map(s) ignored (no executable beside them): %s"
                  % (len(orphans), ', '.join(orphans)))
    if not maps:
        default = os.path.join(args.root, 'build', 'port', 'walk_window.map')
        maps = [default]
    return run(args.root, maps, verbose=args.verbose, sweep=sweep)


if __name__ == '__main__':
    sys.exit(main())
