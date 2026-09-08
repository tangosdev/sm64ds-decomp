"""Build-time guard against DEFEATED /alternatename aliases in the port.

/alternatename:_LHS=_RHS tells the linker "if _LHS is undefined, use _RHS".
The directive only fires while _LHS is UNDEFINED. The moment a real
definition of _LHS enters the link -- a newly sliced src/ TU, a hostgen
output, a host copy -- the alias goes inert and every reference that used to
route to _RHS silently binds to the new definition instead. Nothing warns:
the link succeeds, the bytes are wrong.

Wave 5 hit this class twice (w5b_review.md R1/R2):

  R1  slicing src/RollingRock_Spawn.c defeated
      /alternatename:_RollingRock_Spawn=_data_ov010_02112d64 and
      PeachPainting::InitResources' SharedFilePtr became a code address --
      level 2 faulted (c0000005 at _RollingRock_Spawn+2).
  R2  slicing src/func_ov065_02117994.c (Swoop's helper, a genuinely
      different body) defeated the ov062 sibling alias and three ov062 TUs
      silently ran Swoop's code. No crash, censuses unchanged.

The same flip is latent for _ZN6EyerokD0Ev and the data_ov075 aliases if
their overlays ever land. This guard turns the whole class into a build
failure.

HOW THE MAP TELLS THE TWO CASES APART. When an alias FIRES, link.exe
publishes the LHS as a public at the RHS's address, so both names appear in
the map AT THE SAME ADDRESS -- that is the healthy shape and it covers
nearly every alias in the build. When the alias is DEFEATED, the LHS is a
real definition with its OWN address (and the RHS keeps its own, or is
/OPT:REF-stripped entirely). So the mechanical test is per directive:

  LHS absent from the map ................................. OK (unused)
  LHS present, RHS present, SAME address .................. OK (alias fired)
  LHS present at a DIFFERENT address than RHS ............. FAIL (defeated)
  LHS present, RHS absent ................................. FAIL (defeated;
      the LHS definition satisfied every reference and the RHS died to
      /OPT:REF -- exactly R1's shape when the data symbol goes unreferenced)

WHAT COUNTS AS A LINKER INPUT. An /alternatename reaches link.exe two ways,
and only the first one exists in this tree:

  (a) #pragma comment(linker, "/alternatename:...") in a .c/.cpp/.h under
      port/, OR IN A FILE ONE OF THOSE #includes. The compiler copies the
      string into the object's .drectve section and the linker reads it from
      there, and it does that for an included file exactly as it does for the
      including one -- the preprocessor has already pasted the text in by the
      time the pragma is seen. That is why the reader does not classify a file
      by its extension alone: see THE .inc HOLE below. A backslash-continued
      pragma counts: the reader carries "this line is inside a pragma" across
      the continuation and matches each directive on the physical line its
      text sits on. It does NOT splice the lines into one string, so this is
      not the preprocessor and does not claim to be; the shapes that
      distinction loses are recorded in WHAT THE COMMENT PASS STILL DOES NOT
      DO below. Until run link60 the pragma test ran a line at a time and
      never saw four continued directives at all -- two were firing, two had
      been defeated since the day they landed and are now carried in the
      baseline with that history. Only pragma lines are parsed; a mention that
      is not a pragma is not a linker input, and is counted with the doc prose
      rather than dropped.
  (b) a DIRECTIVE FILE handed to the linker directly: a response file
      (@file), a /DEF: module-definition file, or an /alternatename written
      straight into the link options. THE PORT HAS NONE OF THESE.
      REGISTERED_DIRECTIVE_FILES is therefore empty, and that emptiness is
      re-derived from every CMake file under port/ on every run by
      audit_linker_inputs() rather than believed.

WHY (b) IS A REGISTERED LIST AND NOT A FILE SWEEP. This guard used to walk
EVERY port/**.txt and treat any line whose lstrip started with
"/alternatename:" as a real linker input. port/ is mostly prose (lane maps,
seat notes, syms files), so a lane that quoted a directive at the start of a
line turned its own documentation into a build input. That was never
theoretical:

  * port/stage_lifecycle_map.txt:995 quotes lane SL0's FaderWipe vtable
    alias mid-paragraph, and the sweep parsed it.
  * port/ov007_seat.txt:444 quotes the ov007/ov002 alias to explain why that
    TU stays out of the slice, and the sweep parsed it.
  * lane LK4 deleted the real Heap::SetDefault alias from
    hal/lk4_solidheap_seat.cpp and replaced it with a receiver-bridging
    face. The build stayed broken afterwards, because the QUOTE of the dead
    directive in port/stage_lifecycle_map.txt:1084 was still being read as a
    live one: LHS defined at its own address, RHS at another, so the guard
    called a deleted alias defeated. It only went green once that quote was
    annotated out of the sweep's way. Prose had become load-bearing.

Docs are not linker inputs. The sweep is gone. A line that quotes a
directive in a file nothing hands the linker is prose, and the guard prints
how many it saw and where -- doc files and SOURCE COMMENTS alike. The source
side used to be dropped without a count, which is the same missing number in a
different file. A count nobody prints is how the next lane plants the next
landmine.

That rule covers a mention with no #pragma on the line. It now also covers a
whole pragma that has been COMMENTED OUT, which it did not for the whole of
this file's life before run link100 (six commits from 2026-08-13). See THE
COMMENT GAP, CLOSED.

Loudness moved to where it belongs. If CMake hands the linker a directive
file that is NOT in REGISTERED_DIRECTIVE_FILES, the guard FAILS and names
the file and the registration mechanism; a registered file CMake no longer
feeds fails the same way, so the list cannot rot in either direction; and a
registration pointing OUTSIDE port/ fails too, because the directive scan
only walks port/ and such an entry would clear the complaint while nothing
ever read the file.

WHAT THAT DERIVATION COVERS, EXACTLY. It walks every CMakeLists.txt and
*.cmake under port/ -- one file today, and the walk is the point -- and
catches the LITERAL spellings written inside any link-option or link-library
command: a response file, a /DEF: file, an /alternatename put straight into
the link options, and a raw .rsp or .def item handed to target_link_libraries
without the @ or /DEF: that would make the linker read it. That last one is
reported rather than judged: the spelling is ambiguous enough that "did
anybody check?" has to be answered out loud. The context match is deliberately
loose. LINK_OPTIONS anywhere in a command or property name catches
target_link_options, add_link_options and INTERFACE_LINK_OPTIONS alike, which
the old word-boundary regex missed because an underscore is a word character;
LINKER_FLAGS catches the whole CMAKE_*_LINKER_FLAGS family; LINK_FLAGS and
STATIC_LIBRARY_OPTIONS are named outright. CMake comments are stripped first:
the CMakeLists prose discusses /alternatename in a dozen places and none of
that reaches the linker.

It still does not follow INDIRECTION. A directive file whose path arrives
through a CMake variable set somewhere else, through a generator expression,
through /WHOLEARCHIVE, or through an add_subdirectory pointing outside port/,
is a route a literal reader does not see. None of those exist in the tree
today, which is why the registry is empty and the build is green, but "cannot
dodge the guard" would still be a bigger claim than the code earns. Treat this
as a checked fact about the literal spellings and a promise about nothing
else.

THE COMMENT GAP, CLOSED. This reader used to be a pure line scanner, so it
could not tell code from a comment: PRAGMA_RE matched anywhere in a line, and
a pragma that had been COMMENTED OUT was scanned as a live directive. Two
existed in the tree when this was fixed, and neither is in the build:

    hal/actor_classes_koopa_chuckya.cpp:221
    /* #pragma comment(linker,
       "/alternatename:?data_0209e650@@3PAHA=_data_0209e650") */

    hal/actor_classes_ov010.cpp:7
    //     #pragma comment(linker,
    //         "/alternatename:__ZTV14daObjC1_Trap_c=__ZTV4Trap")

The first is a directive the prose above it says should stay dead. The second
is worse: it is a QUOTE, inside a header comment, of a directive that has since
been CORRECTED -- the alias now binds _data_ov010_02112ae4 (the live pragma is
hal/actor_classes.cpp:2438), and the comment preserves the wrong old answer for
the record. Both were inside the scanned count, and they were inert for two
DIFFERENT reasons, which is worth writing down because the first one was got
wrong here once and the arithmetic below only balances with it right:

  * ov010's LHS __ZTV14daObjC1_Trap_c is absent from the map, so evaluate()
    took the "unused alias" exit. Inert by luck, as recorded.
  * koopa_chuckya's LHS ?data_0209e650@@3PAHA IS in the map, at 0004:0000c868,
    the same address as its RHS -- so the phantom read as FIRED. Not luck: the
    SAME pair is declared for real at hal/actor_classes_ov035.cpp:191, and
    evaluate() returns one row per DECLARATION, so the commented-out copy rode
    the live one's verdict. It was inflating the fired count by one.

That is why closing this gap moves the live count by -2 and the fired count by
only -1 (measured on three trees below).

THE FAILURE DIRECTION WAS THE SAFE ONE, which is why this was recorded for a
while rather than blocking: a phantom directive inflates the scanned count and
can raise a LOUD false FAIL over an alias no object carries, but it can never
produce silently wrong bytes, because this guard only refuses and emits nothing
into the link. What made it worth closing is that a false FAIL is still a build
break nobody can fix in the right place: the day ?data_0209e650@@3PAHA becomes
a defined symbol, the build fails over a directive that is not in the build,
and the fix is to edit a comment. That is the LK4 prose class reborn one file
over, and it is the shape this guard exists to abolish.

HOW IT IS CLOSED. strip_comments() blanks comment text before the pragma and
directive patterns run, carrying block-comment state across lines, honouring a
backslash that continues a // comment onto the next line, and stepping over
string and char literals so a "/*" written inside one does not open a comment.
Literals themselves are left in place, because a real directive lives inside
one. On top of that PRAGMA_RE is ANCHORED: a #pragma is a preprocessor
directive and must be the first token on its logical line, so a "#pragma
comment(linker," that appears mid-line is inside something -- a comment the
stripper has already blanked, or a string literal quoting a directive at a
reader. Seven mid-line spellings exist in the tree and all seven are prose; two
of them carry a complete directive (the two above) and the other five quote
/DELAYLOAD, /INCLUDE or an elided "/alternatename:...", so only two were ever
counted as live. The anchor is BELT AND BRACES, not the fix: with comments
stripped, anchored and unanchored produce the identical 2399 directives on this
tree today. It earns its place against the one shape the stripper cannot see,
a whole pragma inside a string LITERAL, which the fixture pins.
A commented-out directive is counted with the rest of the prose, by file and
line, exactly as a bare quote is.

WHAT THE COMMENT PASS STILL DOES NOT DO. It is a comment stripper, not a
preprocessor: it does not evaluate #if, so a directive inside an #if 0 block
is still read as live, and it does not splice continuation lines into one
logical line. Both smaller continuation shapes stay as they were, measured and
zero-occurrence: a directive whose text straddles a continuation boundary is
not recovered ("/alternatename:_a= \\ _b" yields the garbage pair _a=\\ rather
than _a=_b, so the real directive is missed AND a junk one is recorded), and a
backslash inside the directive string lands in the RHS token the same way.
Neither is hypothetical-safe by argument, just absent: no scanned pair in the
tree contains a backslash.

THE .inc HOLE, CLOSED. DOC_EXTS used to list '.inc' beside '.txt' and '.md',
so every .inc under port/ was prose: counted, named, and never measured against
the map. For a lane note that is right. For an .inc the preprocessor pastes
into a compiled TU it is exactly wrong, and the tree has one:

    hal/scene_vs_menu.cpp:857   #include "vs_aliases.inc"

hal/vs_aliases.inc carries 76 /alternatename pragmas and every one of them
lands in scene_vs_menu.cpp.obj's .drectve -- route (a) above, no different from
a pragma typed into the .cpp. They were live directives the defeat check never
saw. Nothing was broken when this was found (all 76 fire: LHS and RHS present
at one address, 0 unused, 0 defeated, on all three of the maps this fix was
proved against) but they are the one set in the tree where the R1/R2 flip would
have left the guard green, and several are arm9 method bridges of exactly the
shape a slice defines: ?LookAt_@G3i@@..., ?PerspectiveW_@G3i@@...,
?Func_020156DC@Clipper@@... .

Five .inc files exist under port/ and the include closure reaches all five --
hal/actor_classes.inc, hal/camera_states.inc, hal/player_states.inc,
hal/vs_aliases.inc, hal/vs_data_patch.inc. Only vs_aliases.inc carries
directives; actor_classes.inc mentions /alternatename twice in block comments
with no pair on the line, so it is not even prose by this reader's count, and
the other three do not mention it at all. So the hole was exactly 76 wide, and
the other four are checked rather than assumed.

So a file is no longer classified by its extension alone. included_inputs()
walks every .c/.cpp/.h under port/ for quoted #includes, resolves them against
the including file's directory and then port/, and iterates to a fixed point so
an .inc that includes another .inc is reached too. Anything that walk reaches is
a linker input and is scanned like source; an .inc nothing includes stays prose
under DOC_EXTS, which is why '.inc' is still listed there. The include scan
reads comment-stripped lines, so a commented-out #include does not pull a file
into the build that the compiler never sees.

THE THUMB BIT, when a check compares the addresses INSIDE names. data_ and
func_ spellings carry a ROM address, and comparing the two sides of an alias by
that address is the cheapest test there is for the class lane ALIASCHK spent a
night on: an alias whose two sides name different ROM objects. It has exactly
one trap, and the tree contains it:

    hal/method_faces.cpp:803
    /alternatename:_func_020527e9=_func_020527e8

That reads as an address disagreement and is not one. config/arm9/symbols.txt
:2188 has `func_020527e8 kind:function(thumb,size=0x16) addr:0x020527e8`, and
0x020527e9 is the same function with the THUMB flag set -- the low bit of the
veneer's own literal at arm9 0x0203c178 (`func_0203c178 kind:function(arm,
size=0xc)`, the `ldr ip,[pc]; bx ip; .word 0x020527e9` shape), which is how ARM
encodes "branch here in thumb state", not part of the address. So rom_address()
masks bit 0, and address_disagreements() compares masked values. Without the
mask this check FAILS the build on that one line, which is the whole reason it
is written down here rather than left for the next reader to rediscover.

NOTHING TRIPPED ON IT BEFORE, and the reason is the reason this check is new.
No check in this tree compared name-embedded addresses on both sides of an
alias: evaluate() compares MAP addresses ('0001:000edd30' strings), not the
numbers inside names, so the thumb pair was never in front of a comparison.
Nor was any sibling. Of the seventeen tools under port/tools that read
/alternatename, exactly one -- alias_audit.py -- also converts an embedded ROM
address (FUNC_ADDR/FUNC_OV/DATA_NAME at its lines 117-119, unmasked), and it
reads bindings ONLY out of hal/cxx_aliases.cpp and only for placeholder names
(the G0/G1/VT family), so func_020527e9, which lives in hal/method_faces.cpp
and is not a placeholder, never enters it. That tool is outside this lane's
owned files; its exposure is latent and is reported rather than patched here.
So the mask is not a repair of a live failure. It is what lets the comparison
exist at all: add the check without it and the build breaks on a correct line.

HOW BIG THE CHECK ACTUALLY IS, measured rather than assumed. 1666 of the 2399
directives this guard now scans carry a ROM address on BOTH sides, and all 1666
agree once bit 0 is masked -- on lane EXCEPT's tree, on ALIASCHK's and on
FOLD2's alike. It is not a one-row curiosity guarding one line; it is a
name-level identity test over two thirds of the directive set that happens to
have exactly one masked row in it today.

WHAT IT DELIBERATELY DOES NOT JUDGE: the module qualifier. 103 of those 1666
rows bind spellings whose qualifiers differ -- 'arm9' against an overlay
(_func_020b6584 = _func_ov002_020b6584), or one overlay against another
(_data_ov075_0211c800 = _data_ov064_0211c800) -- and every one of them names
the same address and passes. Most are the port's ordinary idiom, one ROM word
two lanes spelled differently. A few are real overlay crossings, the class
lane ALIASCHK catalogued, and this check is BLIND to them by construction: two
overlays can occupy the same address and still be different objects. Address
agreement here means "the ROM did not already say these are different", never
"these are the same object". Claiming more than that would be the fake-green
this guard exists to refuse.

The map is required: this guard needs the LINKED symbol table, so it runs
POST-LINK (build-port.cmd wires it after ninja; the pre-configure guards
cannot see what the linker resolved). A missing, empty, or truncated map is
itself a FAIL -- a failed link truncates walk_window.map to zero bytes, and
measuring nothing must never read as clean. --selftest needs no map and no
build, and build-port.cmd runs it BEFORE configure alongside closure.py's
and facegen.py's, so a broken guard fails in seconds rather than after a
full link.

THE BASELINE. The full binary carries a reviewed set of aliases whose LHS
is deliberately defined -- the port's weak-symbol idiom: fallback stubs that
only fire in reduced binaries (tests/fault_probe.h, the cxxname_bridge
fallbacks, the l7 particle stub), and MSVC-mangle faces/forwarders that
coexist with the Itanium body on purpose (the w4-c LoadFile forwarder
shape). Those read as "defeated" in the map but are the design. They are
frozen in alternatename_baseline.txt, the inferred_stub_guard ratchet
pattern: the guard FAILS on any defeated pair NOT in the baseline (that is
the R1/R2 arrival shape -- a newly sliced TU defining an alias LHS). The
baseline SHRINKS whenever a pair stops being defeated, and it may GROW only
for a pair the map shows was defeated all along while this reader could not
see it, with the evidence in the commit -- measurement catching up, never a
red build being unblocked. Both directions are edited by hand, one line at a
time, because --update rewrites the whole block off a single live run and
would carry in whatever else happened to be defeated that day.

Two things about the counts the baseline is measured against. evaluate()
returns one row PER DECLARATION, not per pair, so the same alias declared in
two TUs is two rows: today 23 rows collapse to 22 unique pairs, because
_port_last_frame=_port_fault_no_frame is declared in both tests/fault_probe.h
:700 and unmatched/func_02043fdc_hostcopy.cpp:540. The printed
"baseline-known" figure is the UNIQUE count. And a baseline row naming a pair
that does not exist anywhere is not fatal: it cannot mask a real defeat, since
filtering is by exact pair and a fiction matches nothing, so it surfaces only
as the "no longer defeated" note. That is the right severity and also the way
dead rows accumulate quietly, so the note is worth reading rather than
scrolling past.

Exit 0: no defeated alias outside the baseline. Exit 1: at least one new
defeated alias, an alias whose two sides name different ROM addresses, an
unregistered directive source, or an unreadable map; each listed with its
source file and the fix wave 5 used: delete the dead alternatename and, if the
old routing is still needed, compile the referencing TUs with a per-source
-DLHS=RHS rename (see the R1/R2 blocks in port/CMakeLists.txt).
"""

import argparse
import os
import re
import sys
import tempfile

PORT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# A real linker directive. Group 1 = LHS, group 2 = RHS. Decorated names
# (leading-underscore cdecl, ?...@@ MSVC C++) are matched as-is against the
# map, which lists decorated names too.
ALT_RE = re.compile(r'/alternatename:([^\s"=]+)=([^\s")]+)')
# ANCHORED on purpose. A #pragma is a preprocessor directive and has to be the
# first token on its logical line, so a "#pragma comment(linker," found
# mid-line is inside something: a comment (strip_comments has already blanked
# those) or a string literal quoting a directive at a reader. Seven mid-line
# occurrences exist in the tree and all seven are prose; with comments stripped
# the anchor changes NOTHING on this tree today (2399 directives either way),
# so it is here for the one shape the stripper cannot see -- a whole pragma
# inside a string literal. See THE COMMENT GAP, CLOSED.
PRAGMA_RE = re.compile(r'^\s*#\s*pragma\s+comment\s*\(\s*linker\s*,')
INCLUDE_RE = re.compile(r'^\s*#\s*include\s+"([^"]+)"')

# The ROM address a config-style spelling carries. Both decorations reach it:
# _data_02082214 and ?data_02082214@@3PAUS4@@A name one object.
ROM_ADDR_RE = re.compile(r'(?:^|[^0-9A-Za-z])(?:data|func)_(?:ov\d{3}_)?'
                         r'(0[12][0-9a-fA-F]{6})(?![0-9a-fA-F])')

SRC_EXTS = ('.c', '.cpp', '.h')

# Mechanism (b): files the linker reads directives OUT OF. Port-relative,
# forward slashes. Empty on purpose: the port feeds link.exe objects and
# libraries and nothing else. check_registration() re-derives that from
# CMake every run, so this stays a checked fact rather than a claim.
REGISTERED_DIRECTIVE_FILES = ()

# Files a quoted directive can hide in. This is the old sweep's surface
# (.txt) plus the two other doc formats under port/, so the lines the sweep
# used to turn into build input still get counted and named. They are not
# linker inputs and are never measured against the map.
#
# '.inc' is here as a FALLBACK ONLY, and it is the one extension in this tuple
# that does not settle the question by itself. An .inc a compiled TU #includes
# is a linker input -- the preprocessor pastes its pragmas into that TU's
# .drectve exactly as if they had been typed there -- and hal/vs_aliases.inc's
# 76 live directives went unmeasured for as long as this tuple was the whole
# rule. included_inputs() decides first; DOC_EXTS only catches the .inc nothing
# includes. See THE .inc HOLE, CLOSED.
DOC_EXTS = ('.txt', '.md', '.inc')

# CMake constructs that hand link.exe something whose CONTENTS it reads as
# directives, i.e. every way an /alternatename can enter the link without
# passing through a compiled TU. Scanned only inside link-option and
# link-library commands.
CMAKE_COMMENT_RE = re.compile(r'#.*$')
# Substring matches on purpose. The old spelling anchored \blink_options\b and
# named the CMAKE_*_LINKER_FLAGS variables one at a time, so INTERFACE_LINK_-
# OPTIONS and STATIC_LIBRARY_OPTIONS walked straight past it: an underscore is
# a word character, so \b never fires after one.
LINKOPT_CTX_RE = re.compile(
    r'LINK_OPTIONS|LINK_FLAGS|LINKER_FLAGS|STATIC_LIBRARY_OPTIONS|'
    r'link_libraries', re.IGNORECASE)
RSP_RE = re.compile(r'(?:^|[\s"\'])@([A-Za-z0-9_.${}/\\-]+)')
DEF_RE = re.compile(r'/DEF:([^\s"\')]+)', re.IGNORECASE)
# A directive-file path written as a bare item, with no @ or /DEF: to make the
# linker read it. The leading delimiter class excludes @ and :, so the two
# spellings above are not reported twice.
RAW_DIRECTIVE_FILE_RE = re.compile(
    r'(?:^|[\s"\'(])([A-Za-z0-9_.${}/\\-]+\.(?:rsp|def))\b', re.IGNORECASE)

# Directories the CMake walk skips. CMakeFiles is the generator's own output;
# a tree configured in place would otherwise feed the guard the build system's
# generated .cmake files, which port/ does not hand anybody.
CMAKE_SKIP_DIRS = ('__pycache__', 'CMakeFiles')

# included_inputs() reads every source file under port/ and main() wants the
# answer twice. Keyed by the port tree, so the selftest's temp trees never see
# each other's answer. A caller that EDITS a tree between calls has to clear
# it; nothing in this file does, and a guard run is one shot over a tree the
# link has already finished with.
_INCLUDED_CACHE = {}


def strip_comments(lines):
    """Yield each line with its COMMENT text blanked, columns preserved.

    The smallest thing that can tell code from prose. It carries block-comment
    state across lines, honours a backslash that continues a // comment onto
    the next line, and steps OVER string and char literals so a "/*" written
    inside one does not open a comment. The literals are left in place: a real
    directive lives inside one, and blanking them would delete the very text
    this guard reads.

    Comment characters become spaces rather than disappearing, so a column and
    a line number still mean what they meant in the file on disk.

    Not a preprocessor. #if 0 is not evaluated and continuation lines are not
    spliced; both limits are written down in the module docstring.
    """
    in_block = False
    in_line = False              # a // comment a backslash carried down here
    for raw in lines:
        out = []
        i, n = 0, len(raw)
        line_comment = in_line
        in_line = False
        while i < n:
            ch = raw[i]
            if in_block:
                if ch == '*' and i + 1 < n and raw[i + 1] == '/':
                    in_block = False
                    out.append('  ')
                    i += 2
                    continue
                out.append(ch if ch in '\r\n' else ' ')
                i += 1
                continue
            if line_comment:
                out.append(ch if ch in '\r\n' else ' ')
                i += 1
                continue
            if ch == '/' and i + 1 < n and raw[i + 1] == '*':
                in_block = True
                out.append('  ')
                i += 2
                continue
            if ch == '/' and i + 1 < n and raw[i + 1] == '/':
                line_comment = True
                out.append('  ')
                i += 2
                continue
            if ch == '"' or ch == "'":
                quote = ch
                out.append(ch)
                i += 1
                while i < n:
                    c = raw[i]
                    out.append(c)
                    i += 1
                    if c == '\\' and i < n:
                        out.append(raw[i])
                        i += 1
                        continue
                    if c == quote or c in '\r\n':
                        break
                continue
            out.append(ch)
            i += 1
        # A // comment ending in a backslash swallows the next line too. The
        # backslash is inside the comment and has already been blanked, so the
        # test has to be on the line as it was written.
        if line_comment and raw.rstrip('\r\n').rstrip().endswith('\\'):
            in_line = True
        yield ''.join(out)


def resolve_include(includer_abs, target, port_dir):
    """Port-relative path a quoted #include names, or None.

    Quoted includes resolve against the including file's own directory first,
    then against port/ (the port compiles with port/ on the include path). A
    target that lands outside port/ is not this guard's to read: the walk only
    covers port/, and pretending otherwise is how a registration that escapes
    the tree reads clean.
    """
    target = target.replace('\\', '/')
    for base in (os.path.dirname(includer_abs), port_dir):
        cand = os.path.normpath(os.path.join(base, target))
        try:
            rel = os.path.relpath(cand, port_dir)
        except ValueError:                # different drive on Windows
            continue
        rel = rel.replace('\\', '/')
        if rel.startswith('../') or rel == '..':
            continue
        if os.path.isfile(cand):
            return rel
    return None


def included_inputs(port_dir):
    """Port-relative paths that reach the link through an #include.

    A file is a linker input if a compiled TU can see it, and the preprocessor
    is how it gets seen: hal/scene_vs_menu.cpp:857 includes hal/vs_aliases.inc,
    so that file's 76 pragmas are in scene_vs_menu.cpp.obj's .drectve and are
    every bit as live as a pragma typed into the .cpp. Extension is not the
    question; reachability is.

    Seeded from every .c/.cpp/.h under port/ (the same population SRC_EXTS
    already scans) and iterated to a fixed point, so an .inc that includes
    another .inc is reached. Only files that are NOT already SRC_EXTS are
    returned, because those are scanned anyway. Include lines are read
    comment-stripped: a commented-out #include pulls nothing into the build.

    MEMOISED per port tree, and a file with no 'include' text at all is not
    stripped. Both are speed, not scope: this walk reads every source file in
    port/ and main() wants the answer twice (once through collect_directives,
    once for the summary line), which turned a one-second post-link guard into
    a fifteen-second one on the tree it was written against.
    """
    cache_key = os.path.normcase(os.path.abspath(port_dir))
    hit = _INCLUDED_CACHE.get(cache_key)
    if hit is not None:
        return hit
    seeds = []
    for root, dirs, files in os.walk(port_dir):
        dirs[:] = sorted(d for d in dirs if d not in ('__pycache__',))
        for name in sorted(files):
            if name.lower().endswith(SRC_EXTS):
                seeds.append(os.path.join(root, name))

    def includes_of(abspath):
        found = []
        lines = None
        try:
            with open(abspath, 'r', encoding='utf-8', errors='replace') as f:
                # INCLUDE_RE cannot match text with no 'include' in it, and
                # stripping comments is the expensive half of this walk.
                # Blanking a comment never CREATES the word, so the skip is
                # exact. The re-read is a seek for the same reason as above.
                if 'include' in f.read():
                    f.seek(0)
                    lines = f.readlines()
        except OSError:
            return found
        if lines is None:
            return found
        for code in strip_comments(lines):
            m = INCLUDE_RE.match(code)
            if m:
                rel = resolve_include(abspath, m.group(1), port_dir)
                if rel:
                    found.append(rel)
        return found

    reached = set()
    queue = list(seeds)
    seen_abs = set(os.path.normcase(p) for p in seeds)
    while queue:
        abspath = queue.pop()
        for rel in includes_of(abspath):
            if rel.lower().endswith(SRC_EXTS):
                continue                  # already in the scanned population
            if rel in reached:
                continue
            reached.add(rel)
            nxt = os.path.join(port_dir, rel)
            nxt_key = os.path.normcase(nxt)
            if nxt_key not in seen_abs:
                seen_abs.add(nxt_key)
                queue.append(nxt)
    _INCLUDED_CACHE[cache_key] = reached
    return reached


def rom_address(name):
    """The ROM address a data_/func_ spelling carries, bit 0 MASKED, or None.

    Bit 0 is the THUMB flag, not part of the address. config/arm9/symbols.txt
    has `func_020527e8 kind:function(thumb,size=0x16)`, and the veneer at arm9
    0x0203c178 stores 0x020527e9 -- one function, one address, one bit saying
    which instruction set to enter. hal/method_faces.cpp:803 aliases the two
    spellings together, so any check that compares these numerically and does
    not mask fails the build on a line that is correct.
    """
    m = ROM_ADDR_RE.search(name)
    if m is None:
        return None
    return int(m.group(1), 16) & ~1


def address_disagreements(directives):
    """Rows whose two sides each name a ROM address, and a DIFFERENT one.

    A name-level test, so it needs no map and no build. It is the cheap half of
    what lane ALIASCHK did by hand: an alias binds one spelling onto another,
    and when both spellings carry their own address, the ROM has already said
    whether they are the same object. 1666 of the tree's 2399 directives have
    both sides addressed and all 1666 agree once bit 0 is masked -- exactly one
    of them, the thumb pair, NEEDS the mask, which is why rom_address() masks.

    It compares the ADDRESS and not the module qualifier, so 103 rows that
    bind an arm9 spelling onto an overlay one, or one overlay onto another, at
    the same address pass here by construction. Two overlays can share an
    address and be different objects; agreement means "the ROM has not already
    said these are different", never "these are the same object".

    Returns [(lhs, rhs, relpath, lineno, lhs_addr, rhs_addr)].
    """
    out = []
    for lhs, rhs, rel, ln in directives:
        la, ra = rom_address(lhs), rom_address(rhs)
        if la is None or ra is None or la == ra:
            continue
        out.append((lhs, rhs, rel, ln, la, ra))
    return out


def collect_directives(port_dir, registered=None):
    """Sort every /alternatename under port_dir into two buckets.

    Returns (directives, quoted).

      directives  [(lhs, rhs, relpath, lineno)], what the LINK carries, and
                  the only bucket measured against the map. A pragma split
                  over lines with a backslash counts here like any other: the
                  compiler joins the continuation, so this reader does too,
                  and each directive is reported at the physical line its text
                  sits on rather than at the #pragma that opened it.
      quoted      [(relpath, lineno)], a complete directive written out
                  somewhere nothing hands the linker: a DOC file, a comment in
                  a source file, or a whole pragma that has been commented out.
                  Prose either way. Named rather than dropped, because a count
                  nobody prints is how the next landmine gets planted.

    A file counts as source if its extension says so OR if a compiled TU
    #includes it -- hal/vs_aliases.inc is 76 live directives inside an .inc.
    Every line is read comment-stripped, so a commented-out pragma is prose and
    a "/*" inside a string literal is not a comment.
    """
    if registered is None:
        registered = REGISTERED_DIRECTIVE_FILES
    registered = set(registered)
    included = included_inputs(port_dir)
    out = []
    quoted = []
    for root, dirs, files in os.walk(port_dir):
        dirs[:] = sorted(d for d in dirs if d not in ('__pycache__',))
        for name in sorted(files):
            path = os.path.join(root, name)
            rel = os.path.relpath(path, port_dir).replace('\\', '/')
            lower = name.lower()
            is_registered = rel in registered
            is_src = lower.endswith(SRC_EXTS) or rel in included
            is_doc = (not is_src) and lower.endswith(DOC_EXTS)
            if not (is_registered or is_src or is_doc):
                continue
            # Every branch below is behind `'/alternatename:' not in line`, so
            # a file without the string anywhere produces nothing either way.
            # Skipping it whole is what keeps the comment pass off the four
            # thousand files that have no directive in them. The re-read is a
            # seek rather than a splitlines() because readlines() splits on
            # newlines ALONE, and splitlines() would also break a line at a
            # form feed and shift every line number after it.
            raw_lines = None
            try:
                with open(path, 'r', encoding='utf-8', errors='replace') as f:
                    if '/alternatename:' in f.read():
                        f.seek(0)
                        raw_lines = f.readlines()
            except OSError as e:
                print('alternatename_guard: cannot read %s: %s' % (rel, e))
                sys.exit(1)
            if raw_lines is None:
                continue
            # Two views of the same line. The RAW one is what names a quoted
            # directive; the comment-stripped one is what the compiler sees.
            # A registered directive file is not C and has no C comments.
            codes = (list(strip_comments(raw_lines)) if is_src else raw_lines)
            continued = False
            for lineno, (line, code) in enumerate(zip(raw_lines, codes), 1):
                was_continued = continued
                continued = (is_src and code.rstrip().endswith('\\')
                             and (PRAGMA_RE.search(code) or was_continued))
                if '/alternatename:' not in line:
                    continue
                if is_registered:
                    live = line               # a real directive file
                elif (is_src and '/alternatename:' in code
                      and (PRAGMA_RE.search(code) or was_continued)):
                    # Mechanism (a): a real pragma, or a line the backslash
                    # above spliced onto one. Both are the same logical
                    # directive by the time cl.exe reads it, so both are the
                    # same input here. `code` and not `line`, so a pragma
                    # inside a comment is not one of them.
                    live = code
                else:
                    # Prose: a doc file, a mention in a source comment, or a
                    # whole pragma that has been commented out. Not an input.
                    # Counted, never measured.
                    if ALT_RE.search(line):
                        quoted.append((rel, lineno))
                    continue
                for m in ALT_RE.finditer(live):
                    out.append((m.group(1), m.group(2), rel, lineno))
                # One line can carry a live directive AND a quoted one:
                #     #pragma comment(...)   // was /alternatename:_c=_d
                if len(ALT_RE.findall(line)) > len(ALT_RE.findall(live)):
                    quoted.append((rel, lineno))
    return out, quoted


def cmake_files(port_dir):
    """Every CMake file under port_dir, as (abspath, port-relative path).

    One file today. The walk is what stops that being load-bearing: an
    include()d .cmake or an add_subdirectory inside port/ would otherwise be
    a directive route nobody read.
    """
    out = []
    for root, dirs, files in os.walk(port_dir):
        dirs[:] = sorted(d for d in dirs if d not in CMAKE_SKIP_DIRS)
        for name in sorted(files):
            low = name.lower()
            if low == 'cmakelists.txt' or low.endswith('.cmake'):
                path = os.path.join(root, name)
                out.append((path, os.path.relpath(path, port_dir)
                            .replace('\\', '/')))
    return out


def audit_linker_inputs(port_dir):
    """Re-derive mechanism (b) from every CMake file under port_dir.

    Returns [(mechanism, token, relpath, lineno)] for the LITERAL constructs
    that hand link.exe something it reads as directives: a response file, a
    /DEF: file, an /alternatename in the link options, and a bare .rsp/.def
    item in a link-library command. Indirect routes are out of reach and
    listed in the module docstring. CMake comments are stripped first: the
    CMakeLists prose discusses /alternatename in a dozen places and none of
    that reaches the linker.
    """
    found = []
    for path, rel in cmake_files(port_dir):
        depth = 0
        with open(path, 'r', encoding='utf-8', errors='replace') as f:
            for lineno, raw in enumerate(f, 1):
                line = CMAKE_COMMENT_RE.sub('', raw)
                if not (depth or LINKOPT_CTX_RE.search(line)):
                    continue
                for m in DEF_RE.finditer(line):
                    found.append(('/DEF: module-definition file', m.group(1),
                                  rel, lineno))
                for m in RSP_RE.finditer(line):
                    found.append(('@ response file', m.group(1), rel, lineno))
                for m in RAW_DIRECTIVE_FILE_RE.finditer(line):
                    found.append(('raw directive-file item, with no @ or '
                                  '/DEF: to make the linker read it',
                                  m.group(1), rel, lineno))
                if '/alternatename:' in line:
                    found.append(('/alternatename: in the link options',
                                  line.strip(), rel, lineno))
                depth = max(0, depth + line.count('(') - line.count(')'))
    return found


def port_relative(token):
    """Normalize a CMake token to the port-relative form the registry uses.

    Only a leading './' is noise. The old spelling was lstrip('./'), which
    strips EVERY leading dot and slash, so '../aliases.rsp' -- a file outside
    port/ that this guard never walks -- normalized onto the in-port name
    'aliases.rsp', and registering the in-port file silently covered a
    different file the linker was actually being handed. A leading '..' or
    '/' names another file and has to keep saying so.
    """
    norm = token.replace('\\', '/')
    while norm.startswith('./'):
        norm = norm[2:]
    return norm


def escapes_port(rel):
    """True if a registry entry does not name a file under port/.

    collect_directives only walks port/, so a registration that escapes it
    would clear the unregistered-source complaint while nothing ever read the
    file: the quietest possible way to defeat this guard, and the obvious
    wrong fix once port_relative starts telling the truth about '..'.
    """
    return (rel.startswith('/')
            or bool(re.match(r'^[A-Za-z]:', rel))
            or any(part == '..' for part in rel.split('/')))


def check_registration(port_dir, registered=None):
    """Return a list of complaint lines; empty means the registry is honest.

    Three ways to be wrong, all fatal. CMake feeds a directive source that is
    not registered, so the guard would never read it. That is the silent hole
    the old txt sweep was flailing at. Or a registered file is not fed any more
    (or is gone), so the registry describes a build that no longer exists. Or a
    registration points outside port/, where nothing scans it.
    """
    if registered is None:
        registered = REGISTERED_DIRECTIVE_FILES
    registered = list(registered)
    bad = []
    seen = set()
    for mechanism, token, rel_cmake, lineno in audit_linker_inputs(port_dir):
        norm = port_relative(token)
        seen.add(norm)
        if norm in registered:
            continue
        bad.append('port/%s:%d hands the linker a directive '
                   'source that is NOT registered:' % (rel_cmake, lineno))
        bad.append('    mechanism : %s' % mechanism)
        bad.append('    token     : %s' % token)
        bad.append('    register it in REGISTERED_DIRECTIVE_FILES in '
                   'port/tools/alternatename_guard.py (port-relative path),')
        bad.append('    or move the directive into a compiled TU as '
                   '#pragma comment(linker, "/alternatename:...").')
    for rel in registered:
        if escapes_port(rel):
            bad.append('REGISTERED_DIRECTIVE_FILES lists %s, which is not '
                       'under port/.' % rel)
            bad.append('    the directive scan only walks port/, so this '
                       'registration would read clean while nothing ever '
                       'scanned the file.')
            bad.append('    move the directive file into port/, or move the '
                       'directive into a compiled TU.')
        elif rel not in seen:
            bad.append('REGISTERED_DIRECTIVE_FILES lists %s but no CMake '
                       'file under port/ feeds it to the linker any '
                       'more.' % rel)
            bad.append('    drop it from the list, or restore the link '
                       'option that fed it.')
        elif not os.path.isfile(os.path.join(port_dir, rel)):
            bad.append('REGISTERED_DIRECTIVE_FILES lists %s but that file '
                       'does not exist under port/.' % rel)
    return bad


def parse_map_publics(map_path):
    """Return {symbol: 'SECTION:OFFSET'} from the MSVC map's publics section.

    Rows look like
        0001:000edd30       _func_020b5e58             004eed30 f   obj
    The section runs from the 'Publics by Value' header to the 'entry point
    at' line; the trailing 'Static symbols' block is deliberately excluded
    (TU-local names must not shadow an alias LHS).
    """
    publics = {}
    addr_re = re.compile(r'^[0-9a-fA-F]{4}:[0-9a-fA-F]{8,16}$')
    in_publics = False
    with open(map_path, 'r', encoding='utf-8', errors='replace') as f:
        for line in f:
            if 'Publics by Value' in line:
                in_publics = True
                continue
            if line.lstrip().startswith('entry point at'):
                break
            if not in_publics:
                continue
            parts = line.split()
            if len(parts) >= 2 and addr_re.match(parts[0]):
                publics.setdefault(parts[1], parts[0])
    return publics


def evaluate(directives, publics):
    """Return (defeated, fired) for a directive set against a map.

    defeated rows are (lhs, rhs, relpath, lineno, lhs_addr, rhs_addr).
    """
    defeated = []
    fired = 0
    for lhs, rhs, rel, ln in directives:
        la = publics.get(lhs)
        if la is None:
            continue                      # unused alias: fine
        ra = publics.get(rhs)
        if ra is not None and la == ra:
            fired += 1                    # alias fired: LHS rides RHS's address
            continue
        defeated.append((lhs, rhs, rel, ln, la, ra))
    return defeated, fired


def baseline_path():
    return os.path.join(PORT_DIR, 'tools', 'alternatename_baseline.txt')


def load_baseline():
    pairs = set()
    try:
        with open(baseline_path(), 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    pairs.add(line)
    except OSError:
        pass
    return pairs


def _old_txt_sweep(port_dir):
    """The DELETED sweep, transcribed, so the selftest can show what changed.

    Every port/**.txt line whose lstrip started with the directive was read
    as a linker input. This is the selftest's control arm and nothing on the
    live path calls it.
    """
    out = []
    for root, dirs, files in os.walk(port_dir):
        dirs[:] = sorted(d for d in dirs if d not in ('__pycache__',))
        for name in sorted(files):
            if not name.lower().endswith('.txt'):
                continue
            path = os.path.join(root, name)
            rel = os.path.relpath(path, port_dir).replace('\\', '/')
            with open(path, 'r', encoding='utf-8', errors='replace') as f:
                for lineno, line in enumerate(f, 1):
                    if not line.lstrip().startswith('/alternatename:'):
                        continue
                    for m in ALT_RE.finditer(line):
                        out.append((m.group(1), m.group(2), rel, lineno))
    return out


def selftest():
    """A synthetic port tree carrying every shape the scoping rule sorts.

    The fixture reproduces lane LK4's build break. A real directive is
    DELETED from the source that used to carry it and only a QUOTE of it
    survives in a lane map, while the map shows the LHS defined at its own
    address (LK4 replaced the alias with a receiver-bridging face). Under
    the old sweep that quote is a directive and the pair reads DEFEATED --
    the historical failure, reproduced. Under the new scope it is prose and
    the same tree is clean, in BOTH the bare and the "(DELETED by lane LK4)"
    annotated spelling, which is what makes that annotation optional.

    On top of that: a real pragma in a .cpp is still scanned, and so is one
    the backslash splits over two lines AND one split over three, where the
    middle line is neither the #pragma nor the directive and the continuation
    state has to survive it; a mention in a source COMMENT is prose and gets
    counted rather than dropped; a directive in a REGISTERED directive file is
    scanned even though it is neither a pragma nor a source; and a CMake file
    that feeds the linker a response file nobody registered FAILS, naming the
    file and the mechanism.

    The registration arms cover the routes the derivation reaches: any CMake
    file under port/ rather than only the top CMakeLists, target_link_options
    and target_link_libraries and INTERFACE_LINK_OPTIONS and
    STATIC_LIBRARY_OPTIONS as link contexts, a raw .rsp/.def item, and a
    parent-directory path that must not normalize onto a registered in-port
    name.

    Every arm here is one claim. Two arms exist because a reviewer's mutation
    walked through the gap between a claim and its check: a two-line fixture
    passed while three-line blocks were dropped, and one context arm stood in
    for two named contexts. An arm that covers a class by example is an arm
    that will be true of the wrong thing eventually.
    """
    ok = True

    def expect(cond, what, got):
        nonlocal ok
        if not cond:
            print("FAIL: %s: %s" % (what, got))
            ok = False

    lhs, rhs = '__ZN4Heap10SetDefaultEv', '?SetDefault@Heap@@QAEHXZ'
    directive = '/alternatename:%s=%s' % (lhs, rhs)
    # The LK4 map shape: LHS is a real definition, RHS lives elsewhere.
    publics = {lhs: '0001:0001cdb0', rhs: '0001:00060a20',
               '_real_lhs': '0001:00000100', '_real_rhs': '0001:00000100',
               '_rsp_lhs': '0001:00000200', '_rsp_rhs': '0001:00000200',
               '_cont_lhs': '0001:00000300', '_cont_rhs': '0001:00000300',
               '_cont3a_lhs': '0001:00000400', '_cont3a_rhs': '0001:00000400',
               '_cont3b_lhs': '0001:00000500', '_cont3b_rhs': '0001:00000500'}

    with tempfile.TemporaryDirectory() as td:
        port = os.path.join(td, 'port')
        os.makedirs(os.path.join(port, 'hal'))

        # (a) a REAL directive, the only mechanism the port uses.
        with open(os.path.join(port, 'hal', 'aliases.cpp'), 'w') as f:
            f.write('// a quoted mention on its own is not an input:\n')
            f.write('// /alternatename:_prose_lhs=_prose_rhs\n')
            f.write('#pragma comment(linker, '
                    '"/alternatename:_real_lhs=_real_rhs")\n')
            f.write('#pragma comment(linker, \\\n')
            f.write('    "/alternatename:_cont_lhs=_cont_rhs")\n')
            # A THREE-line block. The continuation state has to survive a
            # middle line that is not itself a #pragma, which a two-line
            # fixture cannot show.
            f.write('#pragma comment(linker, \\\n')
            f.write('    "/alternatename:_cont3a_lhs=_cont3a_rhs " \\\n')
            f.write('    "/alternatename:_cont3b_lhs=_cont3b_rhs")\n')

        # (b) LK4's shape: the source that carried the directive no longer
        # does, and two lane maps quote it, bare and annotated.
        with open(os.path.join(port, 'hal', 'lk4_seat.cpp'), 'w') as f:
            f.write('// SetDefault USED TO BE THE FIRST LINE OF THIS BLOCK.\n')
            f.write('// The receiver-bridging face below replaces it.\n')
            f.write('extern "C" int %s(void *thiz) { return 0; }\n' % lhs)
        with open(os.path.join(port, 'bare_map.txt'), 'w') as f:
            f.write('  3  the cause is a mis-bridged receiver, in\n')
            f.write('     hal/lk4_seat.cpp:45:\n')
            f.write('         %s\n' % directive)
            f.write('     The RHS is __thiscall and takes ECX.\n')
        with open(os.path.join(port, 'annotated_map.txt'), 'w') as f:
            f.write('         (DELETED by lane LK4) %s\n' % directive)

        # A CMakeLists that feeds the linker nothing but flags: the shape
        # the empty registry is derived from.
        clean_cml = ('set(CMAKE_EXE_LINKER_FLAGS '
                     '"${CMAKE_EXE_LINKER_FLAGS} /MAP")\n'
                     '# prose: /alternatename:_x=_y and /DEF:not_real.def\n'
                     'target_link_options(walk_window PRIVATE '
                     '/DYNAMICBASE:NO)\n')
        cml = os.path.join(port, 'CMakeLists.txt')
        with open(cml, 'w') as f:
            f.write(clean_cml)

        directives, quoted = collect_directives(port)
        old = _old_txt_sweep(port)

        # 1. The historical break: old scope parses the quote, new does not.
        expect([(d[0], d[2]) for d in old] == [(lhs, 'bare_map.txt')],
               'old sweep reads the LK4 quote as a directive', old)
        old_defeated, _ = evaluate(old, publics)
        expect(len(old_defeated) == 1 and old_defeated[0][0] == lhs,
               'old sweep FAILS on the deleted alias', old_defeated)
        new_defeated, new_fired = evaluate(directives, publics)
        expect(new_defeated == [], 'new scope has no defeats', new_defeated)

        # 2. Both real pragmas are scanned -- the one-liner and the one the
        #    backslash splits -- and both fire.
        expect([(d[0], d[1]) for d in directives]
               == [('_real_lhs', '_real_rhs'), ('_cont_lhs', '_cont_rhs'),
                   ('_cont3a_lhs', '_cont3a_rhs'),
                   ('_cont3b_lhs', '_cont3b_rhs')],
               'every pragma is scanned and nothing else is', directives)
        expect(new_fired == 4, 'every real alias fires', new_fired)

        # 3. Both quote spellings are inert, and both are still counted.
        #    This is what lets the "(DELETED by lane LK4)" annotation stop
        #    being load-bearing: the bare line is prose too.
        expect(sorted(quoted) == [('CMakeLists.txt', 2),
                                  ('annotated_map.txt', 1),
                                  ('bare_map.txt', 3),
                                  ('hal/aliases.cpp', 2)],
               'bare and annotated quotes, a CMake comment and a source '
               'comment are prose and named', quoted)

        # 3b. The line-continued pragma is a real input and IS scanned, at the
        #     line its text sits on rather than the #pragma that opened it.
        #     cl.exe splices the continuation; a reader that does not is
        #     measuring a different file from the one being compiled. Four of
        #     these were unmeasured in the port until run link60, two of them
        #     defeated the whole time.
        expect(('_cont_lhs', '_cont_rhs', 'hal/aliases.cpp', 5) in directives,
               'the continued pragma is scanned as a real directive',
               directives)
        expect(('hal/aliases.cpp', 5) not in quoted,
               'a real input is never reported as prose', quoted)

        # 3bb. A THREE-line block, where the middle line is neither the
        #      #pragma nor the directive. The continuation state has to be
        #      carried by the previous line's backslash and not by PRAGMA_RE,
        #      so a two-line fixture passes while 3+ line blocks are dropped.
        expect(('_cont3b_lhs', '_cont3b_rhs', 'hal/aliases.cpp', 8)
               in directives,
               'a directive on the third line of a continued pragma is '
               'scanned', directives)

        # 3c. A complete directive in a SOURCE comment is prose, and it is
        #     counted. It used to be dropped without a number while the same
        #     line in a .txt got one, which is the file's own stated failure
        #     mode in the surface nobody was looking at.
        expect(('hal/aliases.cpp', 2) in quoted,
               'a directive in a source comment is counted as prose', quoted)

        # 4. A registered directive file IS scanned, pragma or not.
        rsp = 'link_aliases.rsp'
        with open(os.path.join(port, rsp), 'w') as f:
            f.write('/alternatename:_rsp_lhs=_rsp_rhs\n')
        reg, reg_quoted = collect_directives(port, registered=(rsp,))
        expect(('_rsp_lhs', '_rsp_rhs', rsp, 1) in reg,
               'a registered directive file is scanned', reg)
        expect(reg_quoted == quoted,
               'registering a file does not change the prose count',
               reg_quoted)

        # 5. Registration is checked against CMake, not assumed. Clean tree
        #    with an empty registry: silent.
        expect(check_registration(port, registered=()) == [],
               'clean tree registers clean',
               check_registration(port, registered=()))

        # 6. CMake feeds a response file nobody registered: LOUD.
        with open(cml, 'w') as f:
            f.write(clean_cml)
            f.write('target_link_options(walk_window PRIVATE @%s)\n' % rsp)
        loud = check_registration(port, registered=())
        expect(any(rsp in l for l in loud)
               and any('response file' in l for l in loud)
               and any('REGISTERED_DIRECTIVE_FILES' in l for l in loud),
               'unregistered response file fails loudly, naming file and '
               'mechanism', loud)
        expect(check_registration(port, registered=(rsp,)) == [],
               'registering it clears the complaint',
               check_registration(port, registered=(rsp,)))

        # 7. /DEF: and an inline directive are caught the same way.
        with open(cml, 'w') as f:
            f.write(clean_cml)
            f.write('target_link_options(walk_window PRIVATE '
                    '/DEF:exports.def)\n')
            f.write('set(CMAKE_EXE_LINKER_FLAGS '
                    '"${CMAKE_EXE_LINKER_FLAGS} /alternatename:_a=_b")\n')
        loud = check_registration(port, registered=())
        expect(any('exports.def' in l for l in loud)
               and any('module-definition' in l for l in loud),
               'a /DEF: file is caught', loud)
        expect(any('in the link options' in l for l in loud),
               'an inline /alternatename in link options is caught', loud)

        # 8. A registry naming a file CMake does not feed is also wrong.
        with open(cml, 'w') as f:
            f.write(clean_cml)
        expect(any('feeds it to the linker any more' in l
                   for l in check_registration(port, registered=(rsp,))),
               'a stale registration fails',
               check_registration(port, registered=(rsp,)))

        # 9. A PARENT-DIRECTORY path must not normalize onto a registered
        #    in-port name. The old lstrip('./') stripped every leading dot and
        #    slash, so registering link_aliases.rsp silently covered
        #    ../link_aliases.rsp -- a different file, outside the tree this
        #    guard walks, read by the linker and by nothing else.
        with open(cml, 'w') as f:
            f.write(clean_cml)
            f.write('target_link_options(walk_window PRIVATE @../%s)\n' % rsp)
        loud = check_registration(port, registered=(rsp,))
        expect(any('../%s' % rsp in l for l in loud),
               'a parent-directory directive file does not collide with the '
               'registered in-port name', loud)

        # 10. And the collision cannot be silenced by registering the escaping
        #     path instead: nothing walks outside port/, so that registration
        #     would read clean over a file no one scanned.
        esc = check_registration(port, registered=('../%s' % rsp,))
        expect(any('not under port/' in l for l in esc),
               'a registration that escapes port/ is refused', esc)

        # 11. The derivation walks EVERY CMake file under port/, not just the
        #     top CMakeLists, and names the one it found the route in.
        with open(cml, 'w') as f:
            f.write(clean_cml)
        os.makedirs(os.path.join(port, 'cmake'))
        sub = os.path.join(port, 'cmake', 'link_extra.cmake')
        with open(sub, 'w') as f:
            f.write('target_link_options(walk_window PRIVATE @%s)\n' % rsp)
        loud = check_registration(port, registered=())
        expect(any('cmake/link_extra.cmake' in l for l in loud),
               'a .cmake file under port/ is walked and named by its own '
               'path', loud)
        os.remove(sub)

        # 12. target_link_libraries is a link context: a response file handed
        #     there reaches link.exe exactly as it would from link options.
        with open(cml, 'w') as f:
            f.write(clean_cml)
            f.write('target_link_libraries(walk_window PRIVATE @%s)\n' % rsp)
        loud = check_registration(port, registered=())
        expect(any(rsp in l for l in loud),
               'target_link_libraries is scanned for directive files', loud)

        # 13. A raw .rsp/.def item with no @ or /DEF: in front of it is
        #     reported too. Whether link.exe reads that one depends on the
        #     spelling, which is the reason to say it out loud rather than
        #     decide quietly.
        with open(cml, 'w') as f:
            f.write(clean_cml)
            f.write('target_link_libraries(walk_window PRIVATE %s)\n' % rsp)
        loud = check_registration(port, registered=())
        expect(any('raw directive-file item' in l for l in loud),
               'a raw directive-file item is reported', loud)

        # 14. INTERFACE_LINK_OPTIONS is a link context. The old word-boundary
        #     regex missed it because an underscore is a word character, so
        #     the boundary never fired after INTERFACE_.
        with open(cml, 'w') as f:
            f.write(clean_cml)
            f.write('set_property(TARGET walk_window PROPERTY '
                    'INTERFACE_LINK_OPTIONS @%s)\n' % rsp)
        loud = check_registration(port, registered=())
        expect(any(rsp in l for l in loud),
               'INTERFACE_LINK_OPTIONS is a link context', loud)

        # 14b. So is STATIC_LIBRARY_OPTIONS. The docstring names both and the
        #      regex lists both, but arm 14 pinned only the first, so dropping
        #      STATIC_LIBRARY_OPTIONS from the pattern went unnoticed. One
        #      claim, one arm each.
        with open(cml, 'w') as f:
            f.write(clean_cml)
            f.write('set_property(TARGET ntr PROPERTY '
                    'STATIC_LIBRARY_OPTIONS @%s)\n' % rsp)
        loud = check_registration(port, registered=())
        expect(any(rsp in l for l in loud),
               'STATIC_LIBRARY_OPTIONS is a link context', loud)

        # 15. The @ and /DEF: spellings are reported ONCE, by their own
        #     mechanism, not a second time as raw items.
        with open(cml, 'w') as f:
            f.write(clean_cml)
            f.write('target_link_options(walk_window PRIVATE @%s '
                    '/DEF:exports.def)\n' % rsp)
        mechs = [m for m, _, _, _ in audit_linker_inputs(port)]
        expect(mechs.count('@ response file') == 1
               and mechs.count('/DEF: module-definition file') == 1
               and not any('raw directive-file item' in m for m in mechs),
               'the @ and /DEF: spellings are not double-reported as raw '
               'items', mechs)

    # ----------------------------------------------------------------- 16
    # THE .inc HOLE. An .inc a compiled TU #includes is a linker input; an
    # .inc nothing includes is prose. The tree's real shape is
    # hal/scene_vs_menu.cpp:857 including hal/vs_aliases.inc, whose 76
    # directives were never measured while DOC_EXTS decided the question.
    with tempfile.TemporaryDirectory() as td:
        port = os.path.join(td, 'port')
        os.makedirs(os.path.join(port, 'hal'))
        with open(os.path.join(port, 'hal', 'scene.cpp'), 'w') as f:
            f.write('#include "vs_aliases.inc"\n')
            f.write('// a commented include pulls nothing in:\n')
            f.write('// #include "orphan.inc"\n')
        with open(os.path.join(port, 'hal', 'vs_aliases.inc'), 'w') as f:
            f.write('#include "deeper.inc"\n')
            f.write('#pragma comment(linker, '
                    '"/alternatename:_inc_lhs=_inc_rhs")\n')
        # An .inc that only another .inc includes is still reached: the
        # closure iterates rather than looking one hop deep.
        with open(os.path.join(port, 'hal', 'deeper.inc'), 'w') as f:
            f.write('#pragma comment(linker, '
                    '"/alternatename:_deep_lhs=_deep_rhs")\n')
        # Nothing includes this one. It stays prose under DOC_EXTS.
        with open(os.path.join(port, 'hal', 'orphan.inc'), 'w') as f:
            f.write('#pragma comment(linker, '
                    '"/alternatename:_orphan_lhs=_orphan_rhs")\n')

        d, q = collect_directives(port)
        pairs = [(a, b, r, n) for a, b, r, n in d]
        expect(('_inc_lhs', '_inc_rhs', 'hal/vs_aliases.inc', 2) in pairs,
               'a pragma in an #included .inc is a linker input', pairs)
        expect(('_deep_lhs', '_deep_rhs', 'hal/deeper.inc', 1) in pairs,
               'the include closure iterates: an .inc reached only through '
               'another .inc is scanned', pairs)
        expect(not any(a == '_orphan_lhs' for a, _, _, _ in pairs),
               'an .inc nothing includes is not a linker input', pairs)
        expect(('hal/orphan.inc', 1) in q,
               'the orphan .inc is still counted as prose, not dropped', q)
        expect(sorted(included_inputs(port))
               == ['hal/deeper.inc', 'hal/vs_aliases.inc'],
               'a commented-out #include does not reach a file',
               sorted(included_inputs(port)))

    # ----------------------------------------------------------------- 17
    # THE COMMENT GAP. A commented-out pragma is prose; a real one beside it
    # is not. Both live shapes are here (// and /* */), plus the two the
    # anchor is for: a whole pragma quoted inside a string literal, and a
    # "/*" inside a string literal that must not open a comment.
    with tempfile.TemporaryDirectory() as td:
        port = os.path.join(td, 'port')
        os.makedirs(os.path.join(port, 'hal'))
        with open(os.path.join(port, 'hal', 'comments.cpp'), 'w') as f:
            # 1  the ov010 shape: a QUOTE of a directive that has since been
            #    corrected, sitting in a header comment.
            f.write('//     #pragma comment(linker, '
                    '"/alternatename:_cmt_lhs=_cmt_rhs")\n')
            # 2-3  the koopa_chuckya shape: /* opens on the line with the
            #      #pragma, the directive text is on the NEXT line, and the
            #      block has to still be open when that line is read.
            f.write('/* #pragma comment(linker,\n')
            f.write('   "/alternatename:_blk_lhs=_blk_rhs") */\n')
            # 4  a whole pragma quoted inside a string literal. PRAGMA_RE
            #    unanchored matches this; anchored it does not.
            f.write('static const char *note = "#pragma comment(linker, '
                    '\\"/alternatename:_str_lhs=_str_rhs\\")";\n')
            # 5  a "/*" INSIDE a string literal must not open a comment. If
            #    it did, every directive below would vanish.
            f.write('static const char *not_a_comment = "/* still code */";\n')
            # 6  the real one, and it must survive all of the above.
            f.write('#pragma comment(linker, '
                    '"/alternatename:_live_lhs=_live_rhs")\n')
            # 7  a // comment a backslash carries onto line 8.
            f.write('// carried down by the backslash: \\\n')
            f.write('#pragma comment(linker, '
                    '"/alternatename:_swallowed_lhs=_swallowed_rhs")\n')
            # 9  a live directive and a quoted one on ONE line.
            f.write('#pragma comment(linker, '
                    '"/alternatename:_both_lhs=_both_rhs")  '
                    '// was /alternatename:_old_lhs=_old_rhs\n')

        d, q = collect_directives(port)
        got = [(a, b, n) for a, b, _, n in d]
        expect(got == [('_live_lhs', '_live_rhs', 6),
                       ('_both_lhs', '_both_rhs', 9)],
               'only the pragmas that are actually code are scanned', got)
        expect(sorted(q) == [('hal/comments.cpp', 1), ('hal/comments.cpp', 3),
                             ('hal/comments.cpp', 4), ('hal/comments.cpp', 8),
                             ('hal/comments.cpp', 9)],
               'every commented-out or quoted directive is counted as prose, '
               'by line', sorted(q))
        # The same claims, one at a time, so a partial regression cannot hide
        # inside a list comparison.
        expect(not any(a == '_cmt_lhs' for a, _, _ in got),
               'a // commented-out pragma is prose', got)
        expect(not any(a == '_blk_lhs' for a, _, _ in got),
               'a /* */ block comment carries across lines and its directive '
               'is prose', got)
        expect(not any(a == '_str_lhs' for a, _, _ in got),
               'a whole pragma quoted inside a string literal is prose', got)
        expect(not any(a == '_swallowed_lhs' for a, _, _ in got),
               'a // comment continued by a backslash swallows the next line',
               got)
        expect(any(a == '_live_lhs' for a, _, _ in got),
               'a "/*" inside a string literal does not open a comment', got)
        expect(('hal/comments.cpp', 9) in q
               and any(a == '_both_lhs' for a, _, _ in got),
               'one line can carry a live directive and a quoted one, and '
               'both are reported', (got, q))

    # ----------------------------------------------------------------- 18
    # THE THUMB BIT. hal/method_faces.cpp:803 aliases _func_020527e9 onto
    # _func_020527e8: one function, thumb, size 0x16 in config/arm9. The low
    # bit says which instruction set to enter and is not part of the address,
    # so an unmasked comparison fails the build on a correct line.
    expect(rom_address('_func_020527e9') == rom_address('_func_020527e8')
           == 0x020527e8,
           'bit 0 is masked out of an address read from a name',
           (rom_address('_func_020527e9'), rom_address('_func_020527e8')))
    expect(rom_address('?data_02082214@@3PAUS4@@A')
           == rom_address('_data_02082214') == 0x02082214,
           'both decorations of one data name give one address',
           rom_address('?data_02082214@@3PAUS4@@A'))
    expect(rom_address('_data_ov075_0211c968') == 0x0211c968,
           'an overlay-qualified name gives its address',
           rom_address('_data_ov075_0211c968'))
    expect(rom_address('__ZTV14daObjC1_Trap_c') is None
           and rom_address('?SetDefault@Heap@@QAEHXZ') is None,
           'a name with no embedded address gives None, and is never '
           'compared', rom_address('__ZTV14daObjC1_Trap_c'))
    thumb_row = [('_func_020527e9', '_func_020527e8', 'hal/method_faces.cpp',
                  803)]
    expect(address_disagreements(thumb_row) == [],
           'the thumb pair is NOT an address disagreement',
           address_disagreements(thumb_row))
    real_row = [('_func_020527e8', '_func_02052800', 'hal/x.cpp', 1)]
    expect(len(address_disagreements(real_row)) == 1,
           'two different addresses ARE a disagreement, so the mask has not '
           'blinded the check', address_disagreements(real_row))
    # A whole-address difference in the low nibble, not just bit 0: the mask
    # clears exactly one bit and nothing else.
    near_row = [('_func_020527e8', '_func_020527ea', 'hal/x.cpp', 1)]
    expect(len(address_disagreements(near_row)) == 1,
           'the mask clears bit 0 only, not the low nibble',
           address_disagreements(near_row))
    mixed_row = [('_data_ov075_0211c968', '_data_0211c968', 'hal/x.cpp', 1)]
    expect(address_disagreements(mixed_row) == [],
           'an overlay-qualified spelling and a flat one at the same address '
           'agree', address_disagreements(mixed_row))
    # 18b. And the limit of that, pinned so nobody reads a pass here as an
    #      identity claim. TWO OVERLAYS at one address are different objects
    #      and this check cannot tell: 103 rows in the tree cross a module
    #      qualifier that way, hal/actor_classes_bowserpuzzle.cpp:204 among
    #      them, and every one of them passes BY CONSTRUCTION. The day this
    #      check is asked to judge overlay identity it needs the overlay
    #      layout, not a regex.
    cross_row = [('_data_ov075_0211c800', '_data_ov064_0211c800',
                  'hal/actor_classes_bowserpuzzle.cpp', 204)]
    expect(address_disagreements(cross_row) == [],
           'two DIFFERENT overlays at one address pass: this check reads the '
           'address, never the module', address_disagreements(cross_row))

    # ----------------------------------------------------------------- 19
    # A COMMENTED-OUT PRAGMA THAT DUPLICATES A LIVE ONE. The koopa_chuckya
    # shape, and the reason closing the comment gap moved the tree's live
    # count by -2 while the FIRED count moved by only -1. evaluate() returns
    # one row per DECLARATION, so the phantom rode the real declaration's
    # verdict and read as fired rather than as the harmless "unused" the file
    # used to claim. A phantom that inflates a green number is worse than one
    # that inflates a scanned total, so this arm exists on its own.
    dup_publics = {'_dup_lhs': '0001:00000700', '_dup_rhs': '0001:00000700'}
    real_only = [('_dup_lhs', '_dup_rhs', 'hal/real.cpp', 3)]
    with_phantom = real_only + [('_dup_lhs', '_dup_rhs', 'hal/prose.cpp', 9)]
    expect(evaluate(real_only, dup_publics)[1] == 1,
           'the real declaration fires once', evaluate(real_only, dup_publics))
    expect(evaluate(with_phantom, dup_publics)[1] == 2,
           'a duplicate declaration of a firing pair fires AGAIN, which is '
           'how a commented-out pragma inflated the fired count',
           evaluate(with_phantom, dup_publics))
    with tempfile.TemporaryDirectory() as td:
        port = os.path.join(td, 'port')
        os.makedirs(os.path.join(port, 'hal'))
        with open(os.path.join(port, 'hal', 'real.cpp'), 'w') as f:
            f.write('#pragma comment(linker, '
                    '"/alternatename:_dup_lhs=_dup_rhs")\n')
        with open(os.path.join(port, 'hal', 'prose.cpp'), 'w') as f:
            f.write('/* if the link ever needs it, uncomment: */\n')
            f.write('/* #pragma comment(linker, '
                    '"/alternatename:_dup_lhs=_dup_rhs") */\n')
        d, q = collect_directives(port)
        expect(d == [('_dup_lhs', '_dup_rhs', 'hal/real.cpp', 1)],
               'only the live declaration of a duplicated pair is scanned', d)
        expect(q == [('hal/prose.cpp', 2)],
               'the commented-out duplicate is prose, and is named', q)
        expect(evaluate(d, dup_publics)[1] == 1,
               'so the pair fires exactly once', evaluate(d, dup_publics))

    print("selftest %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    default_map = os.path.join(os.path.dirname(PORT_DIR), 'build', 'port',
                               'walk_window.map')
    ap.add_argument('--map', default=default_map,
                    help='linked MSVC map file (default: %(default)s)')
    ap.add_argument('--port', default=PORT_DIR,
                    help='port tree to scan for directives (default: %(default)s)')
    ap.add_argument('--update', action='store_true',
                    help='rewrite the baseline PAIRS block from the live '
                         'defeated set (reviewed changes only)')
    ap.add_argument('--selftest', action='store_true',
                    help='run the scoping fixture; needs no map and no build')
    args = ap.parse_args()

    if args.selftest:
        return selftest()

    if not os.path.isfile(args.map):
        print('alternatename_guard: FAIL -- map not found: %s' % args.map)
        print('  (the guard runs post-link; a missing map means no link happened)')
        return 1
    if os.path.getsize(args.map) == 0:
        print('alternatename_guard: FAIL -- map is EMPTY: %s' % args.map)
        print('  (a failed link truncates the map to zero bytes; rebuild first)')
        return 1

    unregistered = check_registration(args.port)
    if unregistered:
        print('alternatename_guard: FAIL -- the linker is fed a directive '
              'source this guard does not read.')
        print('An /alternatename arriving that way is invisible to the map '
              'check below, which is the')
        print('hole the old port/**.txt sweep was flailing at. Register it, '
              'or move it into a TU.')
        for line in unregistered:
            print('  %s' % line)
        return 1

    directives, quoted = collect_directives(args.port)

    # Name-level, so it needs no map: when both sides carry a ROM address, the
    # ROM has already said whether they are the same object. Bit 0 is masked
    # (thumb) inside rom_address, without which this fails on the one correct
    # row in the tree, hal/method_faces.cpp:803.
    crossed = address_disagreements(directives)
    if crossed:
        print('alternatename_guard: FAIL -- %d alias(es) whose two sides name '
              'DIFFERENT ROM addresses.' % len(crossed))
        print('An /alternatename renames a symbol; it cannot make two ROM '
              'objects into one. Bit 0 is')
        print('already masked, so a thumb spelling (_func_020527e9 for '
              '_func_020527e8) is not this.')
        for lhs, rhs, rel, ln, la, ra in crossed:
            print('  port/%s:%d' % (rel, ln))
            print('    /alternatename:%s=%s' % (lhs, rhs))
            print('    LHS names 0x%08x, RHS names 0x%08x' % (la, ra))
        return 1

    publics = parse_map_publics(args.map)
    if not publics:
        print('alternatename_guard: FAIL -- no publics parsed from %s'
              % args.map)
        print('  (map exists but has no publics rows; the link is suspect)')
        return 1

    defeated, fired = evaluate(directives, publics)

    if args.update:
        lines = sorted(set('%s=%s' % (lhs, rhs)
                           for lhs, rhs, _, _, _, _ in defeated))
        try:
            old = open(baseline_path(), 'r').read()
        except OSError:
            old = ''
        head = [l for l in old.splitlines() if l.startswith('#')]
        with open(baseline_path(), 'w') as f:
            for l in head:
                f.write(l + '\n')
            for l in lines:
                f.write(l + '\n')
        print('alternatename_guard: baseline rewritten, %d pair(s)'
              % len(lines))
        return 0

    baseline = load_baseline()
    # Per PAIR, not per declaration: defeated carries one row per declaring
    # TU, and an alias declared in two TUs is two rows for one pair (today 23
    # rows, 22 pairs, _port_last_frame being the double). The baseline is a
    # set of pairs, so the reported figure has to be too.
    live = {'%s=%s' % (lhs, rhs) for lhs, rhs, _, _, _, _ in defeated}
    stale = sorted(baseline - live)
    if stale:
        print('alternatename_guard: note -- %d baseline pair(s) no longer '
              'defeated (baseline can be tightened with --update):'
              % len(stale))
        for pair in stale:
            print('  %s' % pair)
    defeated = [d for d in defeated
                if '%s=%s' % (d[0], d[1]) not in baseline]

    if defeated:
        print('alternatename_guard: FAIL -- %d NEW defeated alias(es), not in'
              % len(defeated))
        print('the baseline: each LHS below is DEFINED at its own address, so')
        print('the /alternatename is inert and references bind to the')
        print('definition, not the intended RHS (the R1/R2 arrival shape).')
        for lhs, rhs, rel, ln, la, ra in defeated:
            print('  %s:%d' % (rel, ln))
            print('    /alternatename:%s=%s' % (lhs, rhs))
            print('    LHS at %s, RHS %s' % (la, ra if ra else 'NOT IN MAP'))
        print('Fix (the wave-5 R1/R2 recipe): delete the dead alternatename;')
        print('if the old routing is still needed, compile the referencing TUs')
        print('with a per-source -DLHS=RHS (see the R1/R2 blocks in')
        print('port/CMakeLists.txt).')
        return 1

    print('alternatename_guard: OK -- %d directive(s) scanned, %d fired, '
          '%d baseline-known, 0 new defeats (%d publics)'
          % (len(directives), fired, len(live), len(publics)))
    print('  sources: %d registered directive file(s) plus every pragma in '
          '%s under port/ and in the %d file(s) those #include, continuation '
          'lines included'
          % (len(REGISTERED_DIRECTIVE_FILES), '/'.join(SRC_EXTS),
             len(included_inputs(args.port))))
    print('  (comments stripped and #pragma anchored, so a commented-out '
          'pragma is prose; #if is still not evaluated)')
    if quoted:
        print('  %d directive(s) quoted in prose (doc files and source '
              'comments), NOT linker inputs, not measured:' % len(quoted))
        for rel, ln in quoted:
            print('    port/%s:%d' % (rel, ln))
    return 0


if __name__ == '__main__':
    sys.exit(main())
