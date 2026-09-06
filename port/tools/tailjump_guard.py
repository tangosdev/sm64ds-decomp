"""Build-time guard: the frames that carry an ARM argument through on the host
must still be TAIL JUMPS, and the frames that do not must still be CALLS.

THE BUG THIS EXISTS FOR HAS NOT HAPPENED YET, WHICH IS THE POINT.

port/ov007_seat.txt section 5d screened sixteen arity candidates in the ov007
slice and found something larger than the sixteen: roughly fifty rows in that
slice are correct TODAY only because MSVC chose to compile a forwarder as a
tail jump. The exact integer is method-dependent -- 54 by the screening lane's
parse, 51 by its review's tokenizer -- and nothing turns on which is right.
What matters is the shape:

    a `jmp` reuses the caller's own cdecl argument frame, so an argument the
    forwarder never names is still sitting where its target reads it, which
    reproduces the ROM's `bx ip` and `bl`-with-live-registers exactly
    a `push ebp / mov ebp, esp` builds a NEW frame, and the target reads a
    stack slot nobody filled

Nothing in the tree asks for the first one. It is an optimiser decision. Build
at /Od, turn off /Ob inlining, or add ONE STATEMENT to any of those forwarders,
and MSVC emits a real prologue and every affected row breaks in the same build
-- as a scatter of unrelated-looking faults in unrelated-looking actors, with
no single change to point at. Section 5d called it "the largest unguarded
assumption the ov007 slice rests on" and section 8 recorded that nothing
guarded it. This is the guard.

WHY LINK TIME, AND WHY NO DISASSEMBLER. The question is not "what does this
function do", it is "did the control transfer out of this frame stay a jump".
That is one byte of opcode and four bytes of displacement, and the answer is in
the linked image at an address the map already publishes. So the check is a
byte scan of a frame's own span for

    E8 <rel32>   a near CALL whose target resolves to the callee
    E9 <rel32>   a near JMP  whose target resolves to the callee

and an assertion about which of the two is there. No instruction decoder, no
dumpbin, nothing that can desynchronise on the data islands MSVC leaves between
functions -- section 5d hit exactly that, where a linear `dumpbin /disasm` walk
swallowed func_ov007_020add3c's `push ebp` into a bogus six-byte instruction
and the function appeared not to exist. Reading FROM the map address cannot
desynchronise because the map address is a boundary by construction, and
reading only for two five-byte forms cannot desynchronise at all.

AND THERE IS NO PORT CI. port/ is built by nothing in the repo's CI, the byte
gate never touches it, and this class is invisible to every guard that does
run: linkage.py counts objects, alternatename_guard.py reads directives,
gxband_guard.py reads layout, ptr_audit.py counts unhosted pointers. A frame
that stopped tail-jumping still links, still exports the same symbol, still
passes all four. It shows up as a fault in a scene nobody was working on.

WHAT IS DECLARED, AND WHERE THE NUMBERS COME FROM

THE UNIT IS THE FRAME, NOT THE ARITY ROW, and that is a deliberate improvement
on the source material rather than a shortcut. Section 5d has to quote its
count two ways ("49 veneer rows by this lane's parse, 46 by the review's
tokenizer") because a ROW is a (declaring TU, callee) pair and counting those
means tokenizing C declarations. A FRAME is a linker symbol. The 22 veneer
frames cover every one of those 46-to-49 rows, whichever tokenizer you believe,
because the property being asserted belongs to the frame and not to the row:
if the veneer tail-jumps, every caller of it rides through, and if it does not,
none of them do. So the guard is exact where the seat file has to say "roughly
fifty", and it is exact by asking a smaller question rather than a vaguer one.

  CLASS A   7 frames, HAND-DECLARED from section 5d's "CLASS A, REAL SEAM"
            listing, one `cite` per row. These are the real seams: the frame
            emits a real call and the callee reads a parameter that call did
            not push. Asserted in the NEGATIVE direction -- must call, must not
            jump -- because the classification is what a later lane will read
            when it picks the next seat, and a Class A frame that quietly
            became a tail jump would have stopped being a seam without anyone
            noticing that 5d's list went stale.
  CLASS C   5 frames, HAND-DECLARED from section 5d's "CLASS C, THE RIDE IS
            PRESERVED BY A TAIL JUMP" listing. Must jump, must not call.
  VENEER   22 frames, DERIVED at run time, not listed here. The ROM shape is
            three words

                E59FC000   ldr ip, [pc]
                E12FFF1C   bx  ip
                <target>

            and the guard scans overlay 7's image for it on every run, resolves
            both ends against config/arm9/overlays/ov007/symbols.txt and
            config/arm9/symbols.txt, and asserts a tail jump for each. The
            count 22 is DECLARED as a tripwire, the gxband_guard `members`
            pattern: if the overlay or the config yields a different number the
            guard REFUSES rather than quietly asserting a different set than
            the one written down. Deriving these is what keeps the guard honest
            about a set nobody wants to hand-maintain, and it is what would
            catch the same idiom appearing in a frame 5d never saw.

  Two frames are in both the hand-declared Class C list and the derived veneer
  set (func_ov007_020bdeb0 and func_ov007_020c05f8). That overlap is not tidied
  away: the guard cross-checks that the hand row and the derived row name the
  SAME callee and refuses if they disagree, which is a free consistency check
  between a human classification and a ROM scan.

  AND 34 FRAMES IS NOT 34 BYTE SPANS. ICF folds 17 of them onto one address in
  every hosting map. See the ICF entry under WHAT THIS GUARD DOES NOT PROVE
  for why that costs the guard nothing.

  THE COUNT WAS 32 UNTIL run link60 lane RF1 ADDED TWO. Those two are the arm9
  heap-teardown veneers, and they are the first rows here that were declared
  BECAUSE a fix started depending on them rather than because a screening lane
  found them already load-bearing: replacing the HeapAllocator::Remove
  /alternatename with a receiver-bridging face makes the allocator reach the
  face through those two jmps. A dependency somebody just created is the one
  most worth pinning on the day it is created.

THE LIST IS LIVE, AND A ROW RETIRES WITHOUT A HAND EDIT

func_ov007_020add3c is Class A row 1 and it is the live scene-1 fault. It is
also being SEATED right now by another lane, by displacement: the matched TU
leaves the port's ov007 slice and a host copy that passes the argument stands
in its place, the shape run link60 lane RT1 used on func_ov007_020be980. The
day that lands, the frame this guard classified is not the frame in the binary,
and a guard that kept asserting over it would be asserting about a body that no
longer ships.

So every row carries `tu`, THE SOURCE FILE THAT MUST HAVE PRODUCED THE FRAME'S
OBJECT, and the guard reads back whether it did. Two facts, both read and
neither remembered, the RT1 predicate pattern:

    fact 1   the map's Lib:Object column for the frame symbol -- which OBJECT
             in this binary defines it
    fact 2   build.ninja's rule for that object in that target -- which SOURCE
             the build compiled to produce it

If the source is the declared `tu`, the row is ASSERTED. If it is anything
else, the row RETIRES, named and counted on the guard's own output line, with
the object and the source that displaced it printed. No hand edit, no stale
assertion, and putting the matched TU back re-arms the row in the same build.

IT IS TWO FACTS AND NOT ONE FOR A MEASURED REASON, not for symmetry.
port/tools/objsrc_check.py's banner records it: an MSVC /MAP names an object by
its BASENAME ONLY, and this tree has had eighteen cases where a host copy in
port/unmatched/ and a matched TU in src/ carried the same basename, so the map
alone could not say which was in the binary. The map answers "which object",
build.ninja answers "from which source", and only the pair answers the question
the row is asking.

AND IT IS BETTER THAN THE PREDICATE IT COPIES, in one specific way worth
stating because the CMake predicates are the precedent. port/CMakeLists.txt's
RT1 and PC2 markers read a regex over a src/ file plus a CMake variable set by
the cut a few hundred lines up. Both are inputs to the build: they describe
what the build was ASKED to do. This guard reads the linked map and the
generated build.ninja, which are what the build DID. A configure that silently
failed to apply the cut would satisfy the CMake predicate and fail this one.

THE FOUR VERDICTS PER ROW PER MAP, so nothing falls between them:

  ASSERTED   the frame is in this map, its object came from the declared TU,
             and the required form is there and the forbidden one is not
  BROKEN     same, and the form is wrong. Fails the build.
  RETIRED    the frame is in this map and its object came from somewhere else.
             Reported by name, never silent, and not a failure.
  SKIPPED    the frame is not in this map at all. That target does not link it.

A row that is SKIPPED in EVERY map REFUSES: a declared frame that exists
nowhere means the classification is stale, and a guard that checked nothing
must never read green. That is closure.map_defined's rule and gxband_guard's,
inherited here for the same reason.

WHOLE-CLASS FLIPS ARE REPORTED AS ONE THING. If the tail-jump frames break one
at a time somebody changed a forwarder. If they ALL break in one build somebody
changed the optimisation level, and thirty separate failure blocks would bury
that. When every hosted tail-jump frame in a map has become a call, the guard
says so first, in one line, and names /Od, /Ob0 and /OPT:ICF before it lists
anything.

WHAT THIS GUARD DOES NOT PROVE

  * That a tail jump is CORRECT. It proves the shape survived. Whether the
    argument riding through it is the right argument is section 5d's reading of
    the ROM, and this guard takes that as given -- it is a regression guard on
    a measured property, not a re-derivation of the classification.
  * That the ~50 rows are exactly 50. It asserts 34 distinct frames, 32 of
    them 5d's and two added by lane RF1. The mapping from frames to arity rows
    is 5d's and is method-dependent; the frames are not.
  * Anything about a rel8 jump. `EB <rel8>` is not scanned, and it cannot be
    the transfer being asked about: x86 COFF has no 8-bit relocation, so a
    control transfer to a separately-linked function symbol is always rel32,
    and an `EB` inside a frame is an intra-function branch the compiler
    resolved itself. Scanning for it would also cost far more than it bought --
    one byte of displacement matches by accident roughly once per 65,536
    positions, which over a 700-byte Class A frame is a false failure about one
    time in a hundred.
  * That an E8 or E9 the scan finds is really an instruction. Both forms are
    five bytes and are matched only when the four displacement bytes resolve
    EXACTLY to the callee's own address, so a false hit needs five specific
    bytes at one position; over the whole declared set that is a probability
    around 1e-8. It is not zero. It has never fired, and the measured hit
    counts are printed under --report so a reviewer can see the separation
    rather than take it on faith.
  * Anything about a function that ICF folded onto another one's address, and
    THIS CUTS BOTH WAYS -- read the frame side before quoting the assertion
    count. /OPT:ICF is on. On the CALLEE side it means "resolves to the callee"
    really means "resolves to the address the callee shares", so a transfer to
    a different function folded onto the same bytes reads as a transfer to the
    callee. On the FRAME side it means the 34 declared frames are not 34
    distinct byte spans: 17 of them fold onto ONE address in every hosting map,
    so a map reads fewer spans than rows and the headline assertion count is a
    per-map row count over those spans, not that many independent readings.
    WHAT THAT DOES NOT COST is the thing the guard is for, and the reason is
    the folding rule itself: ICF folds only IDENTICAL bodies. A veneer that
    grows one statement stops being identical to its seventeen twins and
    unfolds into a span of its own, which is exactly the case this guard
    exists to catch. Folding is a property of the healthy state; the regression
    breaks it before the guard has to see through it.
    A SECOND NAME AT AN ADDRESS IS NOT ALWAYS A FOLD, and the guard does not
    tell the two apart: a fired /alternatename and MSVC's decorated C++
    spelling of the same function both publish another name at one address,
    the same reason gxband_guard's arm C excuses equal addresses. The fold
    count printed for a callee is therefore an upper bound on how many
    distinct functions share those bytes, not a measurement of it.
  * Anything about overlays other than 7. The pooled-load-then-branch idiom is
    a mwccarm idiom and 5d says plainly there is no reason it stops at ov007.
    No other overlay is scanned, by this guard or by anything else.

USAGE

    python port/tools/tailjump_guard.py --build-dir build/port
    python port/tools/tailjump_guard.py --map build/port/walk_window.map
    python port/tools/tailjump_guard.py --build-dir build/port --report
    python port/tools/tailjump_guard.py --selftest

Exit 0 when every declared frame in every map that hosts it has the form its
class requires; 1 with the frame, the callee, the form found, the form wanted
and the remedy otherwise; 2 when the guard cannot answer the question.
"""

import argparse
import os
import re
import shutil
import struct
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# THE MAP READER IS gxband_guard's, IMPORTED RATHER THAN COPIED. Its parse_map
# carries the refusals this guard needs verbatim -- missing, zero-byte,
# row-less, trailer-less, no load-address header -- which are closure's, and
# its find_maps carries the orphan rule (a .map with no .exe beside it is a
# stale artifact from a target that no longer exists). Copying them would give
# this tree a second reader to keep in step with the first, and the failure
# that produces is a guard reading a truncated map and reporting green, which
# is the failure both files exist against. The coupling is deliberate and it is
# loud: if that reader changes shape, this guard's selftest fails.
from gxband_guard import Refused, parse_map, find_maps  # noqa: E402


# ---------------------------------------------------------------------------
# THE DECLARED FRAMES.
#
# Class A and Class C are HAND-DECLARED, because they are a human
# classification of ROM and host listings and there is no file to derive them
# from. Every row cites the section it came from. The veneer set is DERIVED --
# see VENEER below -- because it can be, and a set of 22 that nobody has to
# maintain by hand is worth more than a list.
#
# frame    the caller symbol: the frame that DROPS the argument. Named as the
#          DS/config spelling; the map's leading-underscore C decoration is
#          tried too, gxband_guard.MapFile.lookup's rule.
# callee   the symbol the transfer must reach.
# form     'jump' -- must contain an E9 to the callee and NO E8 to it
#          'call' -- must contain an E8 to the callee and NO E9 to it
# tu       the repo-relative source that must have produced the frame's object.
#          THIS IS THE RETIREMENT MECHANISM, not documentation: a row whose
#          frame is compiled from anything else retires itself. See the banner.
# cite     the seat-file section and heading the row was read out of.
# note     what breaks if the form flips, in this row's own terms.
# ---------------------------------------------------------------------------

SEAT = 'port/ov007_seat.txt'

CLASS_A = tuple(
    dict(row, form='call', cls='A',
         cite='%s section 5d, CLASS A, REAL SEAM' % SEAT)
    for row in (
        {'frame': 'func_ov007_020add3c', 'callee': 'func_ov007_020ae558',
         'tu': 'src/func_ov007_020add3c.c',
         'note':
             'THE LIVE SCENE-1 FAULT, and the row being seated by another '
             'lane as this one was written. The caller pushes zero arguments '
             'and the callee reads [ebp+8], which is the caller own saved '
             'esi. If this frame ever reads as a tail jump the seam has '
             'silently changed character and 5d Class A row 1 is stale.'},
        {'frame': 'func_ov007_020b3360', 'callee': 'func_ov007_020b3c54',
         'tu': 'src/func_ov007_020b3360.c',
         'note':
             'b3360 pushes nothing and b3c54 reads its SECOND parameter on '
             'the default arm of a 24-way jump table. The damage is a STORE '
             'of garbage into the object rather than a fault.'},
        {'frame': 'func_ov007_020b3c54', 'callee': 'func_ov007_020b3d30',
         'tu': 'src/func_ov007_020b3c54.c',
         'note':
             'b3c54 pushes one argument and b3d30 reads two. The second is '
             'used as an array subscript in seven places and the first of '
             'those is a double dereference. 5d names this the next fault '
             'after ae558 is seated.'},
        {'frame': 'func_ov007_020b4b5c', 'callee': 'func_ov007_020c43bc',
         'tu': 'src/func_ov007_020b4b5c.c',
         'note':
             'Three pushes before the call and only one of them is an '
             'argument; c43bc reads two. 5d quotes this listing in full '
             'because an automated push counter gets it wrong.'},
        {'frame': 'func_ov007_020bcdb8', 'callee': 'func_ov007_020c99d8',
         'tu': 'src/func_ov007_020bcdb8.cpp',
         'note':
             'A RETURN VALUE RIDING: on ARM the previous call leaves its '
             'result in r0 and c99d8 reads r0. On the host the value is in '
             'EAX and c99d8 reads the stack, and the frame pushes nothing.'},
        {'frame': 'func_ov007_020cc600', 'callee': 'func_ov007_020cc0e4',
         'tu': 'src/func_ov007_020cc600.c',
         'note':
             'The same return-value shape. cc600 is reached from no lifecycle '
             'slot 5d could trace; its only reference is a data slot at '
             '0x021032a4.'},
        {'frame': 'func_ov007_020b6eb4', 'callee': 'func_ov007_020bd648',
         'tu': 'src/func_ov007_020b6eb4.c',
         'note':
             'ONE OF TWO CALLERS IS CORRECT, which is why the row names the '
             'frame and not the callee: src/func_ov007_020b7138.c declares '
             'bd648 with one parameter and pushes one. This frame declares it '
             '(void) and pushes none.'},
    ))

CLASS_C = tuple(
    dict(row, form='jump', cls='C',
         cite='%s section 5d, CLASS C, THE RIDE IS PRESERVED BY A TAIL JUMP'
              % SEAT)
    for row in (
        {'frame': 'func_ov007_020bdeb0', 'callee': 'func_ov007_020be098',
         'tu': 'src/func_ov007_020bdeb0.c',
         'note': 'Also found by the veneer scan; the two derivations agree.'},
        {'frame': 'func_ov007_020c05f8',
         'callee': '_ZN6Player17St_EndingFly_MainEv',
         'tu': 'src/func_ov007_020c05f8.c',
         'note':
             'Also found by the veneer scan, and one of the seventeen MSVC '
             'folded onto a single address. Its target takes void *self and '
             'hands it to a function pointer, so the whole chain is tail '
             'jumps and the argument never moves.'},
        {'frame': 'func_0204f76c', 'callee': 'func_0205ac5c',
         'tu': 'src/func_0204f76c.c',
         'note':
             'THE arm9 TRIO, and 5d calls this one the instructive case. By '
             'declaration counting it drops two arguments across two frames. '
             'What MSVC emitted overwrites its OWN argument slot with p[0x3c] '
             'and then tail-jumps, so [ebp+8] is p[0x3c] and [ebp+0xc] and '
             '[ebp+0x10] are still the original caller second and third, '
             'exactly as r1 and r2 still hold them on ARM.'},
        {'frame': 'func_0204f79c', 'callee': 'func_0205ac84',
         'tu': 'src/func_0204f79c.c',
         'note': 'The arm9 trio, second of three.'},
        {'frame': 'func_0204f73c', 'callee': 'func_0205ad54',
         'tu': 'src/func_0204f73c.c',
         'note': 'The arm9 trio, third of three.'},
        # THE HEAP-TEARDOWN PAIR, added by run link60 lane RF1 when the
        # receiver-bridging face for HeapAllocator::Remove replaced the
        # /alternatename that used to hide the question. These two are not
        # from section 5d: they are arm9, and 5d only screened ov007. The ROM
        # shape is the same three words the VENEER set scans overlay 7 for --
        #     0204e3b4  ldr ip,[pc]; bx ip; .word 0x0204df38
        #     0204ebb8  ldr ip,[pc]; bx ip; .word 0x0204df38
        # -- so r0 rides both frames untouched, and on the host the jmp is
        # what makes the caller's pushed allocator still be at [esp+4] when
        # the face reads it. Verified in this lane's own build: both objects
        # disassemble to exactly `jmp __ZN13HeapAllocator6RemoveEv` and
        # nothing else.
        {'frame': '_ZN13HeapAllocator7DestroyEv',
         'callee': '_ZN13HeapAllocator6RemoveEv',
         'tu': 'src/_ZN13HeapAllocator7DestroyEv.cpp',
         'note':
             'The ExpandingHeap teardown leg. src/_ZN13ExpandingHeap8VDestroy'
             'Ev.c:23 pushes self->allocator into this frame and this frame '
             'names no parameter at all, so the allocator reaches '
             'HeapAllocator::Remove ONLY through the jmp. If it becomes a '
             'call, Remove unlinks a stack word from the nested-allocator '
             'list on every expanding-heap teardown.'},
        {'frame': 'func_0204ebb8',
         'callee': '_ZN13HeapAllocator6RemoveEv',
         'tu': 'src/func_0204ebb8.c',
         'note':
             'The SolidHeap teardown leg, same shape and same consequence; '
             'its pusher is src/_ZN9SolidHeap8VDestroyEv.c:23.'},
        # CURLING'S COLLISION VENEER, added by run link60 lane CUR2 when
        # func_ov006_020e20bc stopped being a return-0 face and became a
        # transcribed body in port/unmatched/MgCurling_Collide_020e20bc.cpp.
        # Also not from section 5d: it is ov006, and 5d only screened ov007.
        # The ROM shape is the same three words the VENEER set scans overlay 7
        # for --
        #     020e285c  ldr ip,[pc]; bx ip; .word 0x020e20bc
        # -- so r0 and r1 ride the frame untouched. The row is REQUIRED and not
        # decoration: unmatched/MgCurling_StateDispatch.cpp dispatches this
        # address from a one-argument slot and passes (c, a), the src TU
        # declares itself (void) and names neither, and the only thing that
        # puts them where 020e20bc reads them is the jmp reusing the
        # dispatcher's frame. If it becomes a call, the collision body gets a
        # garbage `this` and a garbage shell index on every stopped shell,
        # every frame.
        {'frame': 'func_ov006_020e285c',
         'callee': 'func_ov006_020e20bc',
         'tu': 'src/func_ov006_020e285c.c',
         'note':
             'The curling collision veneer. Its callee is the face in '
             'hal/scene_mg_faces.cpp, which forwards to the transcription; '
             'the arguments ride the whole chain because each link is one '
             'call.'},
    ))

# THE VENEER SET, DERIVED. Not a list: the three-word ROM shape below is
# scanned for in overlay 7's image on every run and both ends are resolved
# against config. `count` is the tripwire, the gxband_guard `members` pattern.
VENEER = {
    'module': 'ov007',
    'image': ('extracted', 'overlays', 'overlay_0007.bin'),
    'base': 0x020ad660,
    'count': 22,
    # ldr ip, [pc] ; bx ip -- and the word after them is the target.
    'words': (0xE59FC000, 0xE12FFF1C),
    'cite': '%s section 5d, THE VENEER CLASS' % SEAT,
}

# The maps that hosted at least one declared frame when this guard was
# measured. gxband_guard's `hosts` floor, and the same reason: a slice quietly
# leaving a target is a coverage regression that reads as a smaller green,
# because a map that does not host a frame is correctly SKIPPED. Widening this
# is a measurement anyone can take; narrowing it needs the reason written down.
HOSTS = ('smoke_player.map', 'walk_window.map', 'walk_window_hires.map')

# config symbol tables consulted when resolving a veneer's two ends. ov007
# first: a veneer's target is usually inside the overlay, and two of the 22
# reach arm9 (Deallocate and LoadFile).
SYMBOL_TABLES = (
    ('ov007', ('config', 'arm9', 'overlays', 'ov007', 'symbols.txt')),
    ('arm9', ('config', 'arm9', 'symbols.txt')),
)

CONFIG_ROW = re.compile(r"^(\S+)\s+kind:\S+\s+addr:0x([0-9a-fA-F]+)")

# `build CMakeFiles\<target>.dir\<path>.obj: <rule> C$:\...\file.cpp || ...`
# objsrc_check.py's rule, verbatim, for its reason: ninja escapes a colon in a
# path as `$:` and CMake writes Windows separators.
BUILD_RULE = re.compile(
    r"^build CMakeFiles\\(\w+)\.dir\\([^:]+?\.obj): \S+ (\S+)", re.MULTILINE)

# `build <name>.exe: CXX_EXECUTABLE_LINKER__<target>_<cfg> <obj> <obj> ...`
# The link line is read because an executable's objects no longer all live in
# its own CMakeFiles directory: the tier-independent slice is compiled once
# into an OBJECT library and linked into three executables, so walk_window.exe
# carries objects out of CMakeFiles\port_slice_shared.dir. Only the link edge
# says which objects are actually in which binary.
LINK_RULE = re.compile(
    r"^build ([^:\n]+?\.exe): CXX_EXECUTABLE_LINKER__\S+ ([^\n|]*)",
    re.MULTILINE)

CALL, JMP = 0xE8, 0xE9

REMEDY_JUMP = (
    'the frame must forward with a TAIL JUMP. Three things take that away and '
    'they are worth ruling out in this order:\n'
    '     1. A STATEMENT WAS ADDED to the forwarder. A body that is a single '
    'call gets tail-jumped; a body that is a call plus anything\n'
    '        else gets a real prologue. If this is a one-line change to the '
    'named TU, that is the change.\n'
    '     2. THE OPTIMISATION LEVEL MOVED. /Od, /Ob0, or dropping /O2 on this '
    'TU turns every forwarder in the build into a real\n'
    '        frame at once. Check port/CMakeLists.txt for a '
    'set_source_files_properties COMPILE_OPTIONS or COMPILE_FLAGS on it.\n'
    '     3. THE RIDE-THROUGH IS REAL AND MUST BE SEATED. If the frame has to '
    'grow a body, the argument stops riding and the row\n'
    '        needs the RT1/PC2 displacement shape instead: cut the matched TU '
    'from the port slice, stand a host copy in\n'
    '        port/unmatched/ that names and forwards every argument, and this '
    'row RETIRES itself in the same build.\n'
    '     Read %s section 5d for what rides through this frame and why it is '
    'correct today.' % SEAT)

REMEDY_CALL = (
    'this frame is a classified SEAM and must still emit a real call. It '
    'reading as a tail jump means one of two things:\n'
    '     1. THE SEAT LANDED AND THE ROW WAS NOT RETIRED. If the frame was '
    'displaced to a host copy, the `tu` in this file is stale;\n'
    '        the retirement is automatic when `tu` names the source the build '
    'really compiled, so fix the row rather than deleting it.\n'
    '     2. MSVC FOLDED OR TAIL-CALLED THE FRAME. Then %s section 5d Class A '
    'is stale for this row: the seam it describes is\n'
    '        no longer the shape in the binary, and the next lane picking a '
    'seat would derive from a listing that no longer holds.\n'
    '     Either way the fix is to re-read the frame and amend 5d, not to '
    'delete the row.' % SEAT)


REMEDY_TU = (
    'correct the row\'s `tu` in this file to the source build.ninja really '
    'compiles for that frame, printed above. Two ways to land here:\n'
    '     1. A TYPO, which is the case this check exists for. The row is not '
    'asserting anything until the tu is right.\n'
    '     2. THE MATCHED TU MOVED OR CHANGED EXTENSION, a .c promoted to .cpp '
    'being the common one in this tree. Same fix,\n'
    '        and the commit should say which measurement moved it.\n'
    '     If instead the frame was DISPLACED to a host copy, this is not the '
    'message you would be reading: a host copy lives\n'
    '     outside src/ and the row retires on its own. That is the difference '
    'this check is drawing.')


def default_root():
    """The checkout this script lives in: <root>/port/tools/<this>."""
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


# ---------------------------------------------------------------------------
# config and ROM side: the derived veneer set
# ---------------------------------------------------------------------------

def config_symbols(root):
    """{addr: name} over every consulted config table, ov007 winning ties.

    Ties are real: an overlay symbol and an arm9 symbol never share an address,
    but a table can carry two names for one address. First spelling wins and
    the order of SYMBOL_TABLES decides, which is stated rather than sorted so a
    reader knows which name a message will use.
    """
    out = {}
    for _mod, rel in SYMBOL_TABLES:
        path = os.path.join(root, *rel)
        if not os.path.isfile(path):
            raise Refused(
                "no config symbols at %s -- a veneer's two ends are resolved "
                "against that file and cannot be guessed" % path)
        n = 0
        with open(path, encoding='utf-8', errors='replace') as f:
            for line in f:
                m = CONFIG_ROW.match(line)
                if m:
                    n += 1
                    out.setdefault(int(m.group(2), 16), m.group(1))
        if not n:
            raise Refused("%s parsed to zero symbol rows" % path)
    return out


def find_src(root, sym):
    """The src/ TU that would define a symbol, or None.

    linkage.matched_index's convention: src/<sym>.c, else src/<sym>.cpp. Used
    only for the DERIVED rows -- the hand-declared ones carry their `tu`
    written down, because a hand row's whole value is that a human wrote the
    pairing and a guess would quietly replace it.
    """
    for ext in ('.c', '.cpp'):
        rel = 'src/' + sym + ext
        if os.path.isfile(os.path.join(root, rel)):
            return rel
    return None


def derive_veneers(root, spec=VENEER):
    """The veneer rows, scanned out of the overlay image and resolved.

    A veneer is three words at a symbol boundary: the pooled load, the branch
    through it, and the target. Both ends must be config symbols -- a shape at
    an address config does not name is not a frame the linker can be asked
    about, and the guard refuses rather than skipping it, because a veneer that
    stopped being a symbol is exactly the kind of drift the count is for.
    """
    path = os.path.join(root, *spec['image'])
    if not os.path.isfile(path):
        raise Refused(
            "no overlay image at %s -- the veneer set is DERIVED from the ROM "
            "shape on every run and there is no hand list to fall back on. "
            "Every port tree needs extracted/ anyway; the binaries abort "
            "without the NitroFS emissions." % path)
    img = open(path, 'rb').read()
    if len(img) < 12:
        raise Refused("%s is %d bytes, not an overlay image" % (path, len(img)))
    syms = config_symbols(root)
    w0, w1 = spec['words']
    base = spec['base']
    if syms.get(base) is None:
        raise Refused(
            "overlay image base 0x%08x is not a config symbol, so the image "
            "is not loaded where this guard thinks it is. Re-derive the base "
            "before trusting any address it would have printed." % base)
    rows, unnamed = [], []
    for off in range(0, len(img) - 11, 4):
        a, b = struct.unpack_from('<II', img, off)
        if a != w0 or b != w1:
            continue
        target = struct.unpack_from('<I', img, off + 8)[0]
        va = base + off
        fname, cname = syms.get(va), syms.get(target)
        if fname is None or cname is None:
            unnamed.append((va, target, fname, cname))
            continue
        rows.append({
            'frame': fname, 'callee': cname, 'form': 'jump', 'cls': 'V',
            'tu': find_src(root, fname), 'cite': spec['cite'],
            'ds': va, 'ds_callee': target,
            'note':
                'A pooled-load-then-branch veneer at DS 0x%08x. Its own C '
                'signature and its extern of the target both say (void), so '
                'no arity sweep can see it; what disagrees is the veneer arity '
                'and its CALLERS. It is correct only while it tail-jumps.'
                % va,
        })
    total = len(rows) + len(unnamed)
    if unnamed:
        raise Refused(
            "%d of the %d veneer shapes in %s do not resolve to config "
            "symbols at BOTH ends: %s. A veneer whose frame or target is "
            "unnamed cannot be asked about in the map, and dropping it "
            "silently would shrink the asserted set without saying so."
            % (len(unnamed), total, os.path.join(*spec['image']),
               ', '.join("0x%08x->0x%08x" % (v, t) for v, t, _f, _c
                         in unnamed[:6])))
    if total != spec['count']:
        raise Refused(
            "the veneer shape occurs %d times in %s and this file declares "
            "%d. The overlay or the config moved under the declaration. "
            "Re-read the set and say in the commit which measurement changed "
            "it before trusting any assertion this guard would have made."
            % (total, os.path.join(*spec['image']), spec['count']))
    return tuple(rows)


def declared_rows(root, hand=None, spec=VENEER):
    """Every row: hand-declared plus derived, with the overlap cross-checked.

    A frame in both lists must name the same callee. That is a free agreement
    check between a human reading of a host listing and a scan of the ROM, and
    a disagreement means one of the two is wrong -- which is a refusal, not a
    row to pick between.

    `hand` and `spec` are injection points for the selftest and nothing else.
    They exist because the review found this refusal was hand-proven and
    fixture-less: deleting the disagreement check passed the whole battery,
    which makes it a comment rather than a check.
    """
    veneers = derive_veneers(root, spec)
    hand = (CLASS_A + CLASS_C) if hand is None else tuple(hand)
    by_frame = {}
    for row in hand:
        if row['frame'] in by_frame:
            raise Refused("frame %s is declared twice by hand"
                          % row['frame'])
        by_frame[row['frame']] = row
    out = list(hand)
    overlap = 0
    for row in veneers:
        prior = by_frame.get(row['frame'])
        if prior is None:
            out.append(row)
            continue
        overlap += 1
        if prior['callee'] != row['callee']:
            raise Refused(
                "frame %s is hand-declared (%s) calling %s and the ROM veneer "
                "scan makes it jump to %s. One of the two is wrong and the "
                "guard will not pick."
                % (row['frame'], prior['cite'], prior['callee'],
                   row['callee']))
        if prior['form'] != row['form']:
            raise Refused(
                "frame %s is hand-declared form %r and derives as %r"
                % (row['frame'], prior['form'], row['form']))
    return tuple(out), len(veneers), overlap


# ---------------------------------------------------------------------------
# build side: which source produced which object
# ---------------------------------------------------------------------------

def object_sources(build_dir):
    """{(target, object basename): set(source path)} out of build.ninja.

    objsrc_check.py's read, widened from its two hardcoded targets to every
    target in the file, because this guard sweeps every map rather than
    walk_window's alone. Keeping the target in the key is what makes that safe:
    the tree builds twenty-odd harnesses out of overlapping source sets and a
    basename read across all of them collides constantly.

    WHAT "TARGET" MEANS HERE IS THE BINARY, NOT THE CMAKE DIRECTORY. It used to
    be both, because every source was compiled once per executable and so every
    object in walk_window.map came out of CMakeFiles/walk_window.dir. That is
    no longer true: the tier-independent slice is compiled ONCE into an OBJECT
    library and linked into three executables, so walk_window.map names objects
    from CMakeFiles/port_slice_shared.dir. Looking those up under the map's own
    target name found nothing and skipped the row, which this guard correctly
    refuses to read as a pass. So the mapping is now built from the LINK EDGE:
    every compile rule indexed by its FULL object path, then each executable's
    own input list resolved through it. The per-binary basename key is
    unchanged, and identifying the object by path rather than by the directory
    it was assumed to live in is strictly more precise than the old read.
    """
    ninja = os.path.join(build_dir, 'build.ninja')
    if not os.path.isfile(ninja):
        raise Refused(
            "no build.ninja at %s. The map says which OBJECT defines a frame "
            "and only build.ninja says which SOURCE produced it, and without "
            "the pair a displaced row cannot be told from an asserted one -- "
            "an MSVC map names an object by its basename alone." % ninja)
    with open(ninja, errors='replace') as f:
        text = f.read()

    # every compile rule, keyed by the object's full path
    by_path = {}
    for cmake_dir, obj, src in BUILD_RULE.findall(text):
        full = ('CMakeFiles\\%s.dir\\%s' % (cmake_dir, obj)).lower()
        src = src.replace('$:', ':').replace('\\', '/')
        by_path.setdefault(full, set()).add(src)
    if not by_path:
        raise Refused("%s parsed to zero compile rules" % ninja)

    # every executable, resolved through its own link inputs
    out = {}
    linked = 0
    for exe, inputs in LINK_RULE.findall(text):
        target = os.path.splitext(os.path.basename(exe))[0]
        for tok in inputs.split():
            if not tok.lower().endswith('.obj'):
                continue
            srcs = by_path.get(tok.replace('$:', ':').lower())
            if not srcs:
                continue
            out.setdefault((target, tok.split('\\')[-1]), set()).update(srcs)
            linked += 1
    if not linked:
        # No link edge parsed: fall back to the directory read rather than
        # refuse, so a generator whose link lines this cannot parse degrades to
        # the old behaviour instead of blocking a build.
        for cmake_dir, obj, src in BUILD_RULE.findall(text):
            base = obj.split('\\')[-1]
            src = src.replace('$:', ':').replace('\\', '/')
            out.setdefault((cmake_dir, base), set()).add(src)
    return out


def source_of(objsrc, target, obj, root):
    """(repo-relative source, prose) for an object in a target, or (None, why).

    The prose is what gets printed when a row retires or cannot be judged, so
    it names the state rather than just failing to find one.
    """
    base = obj.split(':')[-1]
    srcs = objsrc.get((target, base))
    if not srcs:
        return None, ("no build.ninja rule for %s in target %s" % (base,
                                                                   target))
    if len(srcs) > 1:
        return None, ("%s in target %s has %d compile rules (%s)"
                      % (base, target, len(srcs), ', '.join(sorted(srcs))))
    src = next(iter(srcs))
    disp = _slashed(src)
    rootn = _slashed(root)
    if disp.lower().startswith(rootn.lower() + '/'):
        return disp[len(rootn) + 1:], ''
    return disp, ''


def is_decomp_source(rel):
    """Is this path the decomp's own code, rather than a host copy?

    THIS IS WHAT SEPARATES A SEAT LANDING FROM A TYPO, and it exists because
    the review found the hole: a `tu` mistyped to ANOTHER REAL src/ FILE
    disarmed its row at exit 0. The row was named on the green line, so it was
    not invisible, but it read as a retirement -- which is a legitimate,
    expected state -- rather than as the misdeclaration it is.

    A DISPLACEMENT MOVES A FRAME OUT OF THE DECOMP. That is the whole shape:
    the RT1/PC2 seat cuts the matched TU from the port slice and stands a host
    copy under port/unmatched/ in its place. A typo does not move anything --
    the build is still compiling decomp source for that frame, just not the
    file the declaration names. So the two are told apart by asking where the
    source the build REALLY used lives, which is a fact and not a convention.

      src/                     the matched TU
      build/.../host-src/      hostgen's rewrite of the matched TU. Still the
                               matched code, transformed for the host by a
                               reviewable tool, and objsrc_check.py's banner
                               records that counting it as linked is CORRECT.
      anything else            a host copy, which is what a seat produces

    WHY NOT THE COUNT PIN THE REVIEW SUGGESTED. Pinning the expected assertion
    count, or asserting zero retirements on a clean tree, closes the same hole
    and this lane was briefed against both: either one makes the ae558 seat
    fail the build on the day it merges, until somebody hand-edits this file to
    accept a retirement that was the entire point of the mechanism. This asks
    the question the count was standing in for, and it stays quiet for a real
    seat while failing a typo.
    """
    p = rel.replace('\\', '/').lower()
    return (p.startswith('src/') or p.startswith('host-src/')
            or '/host-src/' in p)


def _slashed(path):
    """An absolute path with forward slashes, CASE PRESERVED.

    NOT os.path.normcase, which on Windows lower-cases AND flips separators
    back to backslashes. Both halves of that bite: a comparison written against
    forward slashes silently never matches, so every row reads as displaced and
    the guard asserts nothing; and a lower-cased path printed in a retirement
    message names a file that is not spelled that way on disk, which is the
    second bug on top of the first. Comparison lower-cases at the point of
    comparison; display never does.
    """
    return os.path.abspath(path).replace('\\', '/')


# ---------------------------------------------------------------------------
# image side: the bytes at a map address, with no disassembler
# ---------------------------------------------------------------------------

class Image(object):
    """A linked PE32, read by virtual address.

    Only the section table is parsed, which is a header walk and not a decode:
    e_lfanew, the COFF header's section count and optional-header size, the
    optional header's ImageBase, then one 40-byte row per section. Everything
    this guard reads out of the file is bytes at an address the map published.
    """

    def __init__(self, path, data, base, sections):
        self.path = path
        self.data = data
        self.base = base
        self.sections = sections     # [(name, va, vsize, raw_off, raw_size)]

    def read(self, va, n):
        """Up to n bytes at a virtual address, CLAMPED to the section.

        None means the address is in no section at all, which is a map and an
        image that disagree about the layout and must never be measured. A
        SHORT read is not that: a frame that runs to the end of its section
        has fewer bytes after it than its span claims, and scanning what is
        really there is the right answer rather than a refusal. The span is a
        bound derived from the next symbol, not a measurement of the
        function's length, so treating a short read as a fault would fail a
        build over the last symbol in .text being last.
        """
        rva = va - self.base
        for _name, sva, vsize, roff, rsize in self.sections:
            if not (sva <= rva < sva + vsize):
                continue
            off = roff + (rva - sva)
            avail = min(rsize - (rva - sva), n)
            if avail <= 0:
                return b''
            return self.data[off:off + avail]
        return None

    def section_of(self, va):
        rva = va - self.base
        for name, sva, vsize, _roff, _rsize in self.sections:
            if sva <= rva < sva + vsize:
                return name
        return None


def parse_image(path):
    """Image for an MSVC-linked PE32, or Refused.

    The refusals are the map reader's, in the same spirit: a truncated or
    absent executable must never be measured, because measuring the wreckage
    and reporting green is the failure this whole family of guards exists
    against.
    """
    if not os.path.isfile(path):
        raise Refused("no executable at %s -- the link did not run" % path)
    data = open(path, 'rb').read()
    if len(data) < 0x40:
        raise Refused("%s is %d bytes, not a linked executable"
                      % (path, len(data)))
    if data[:2] != b'MZ':
        raise Refused("%s has no MZ header -- that is not a PE image" % path)
    e_lfanew = struct.unpack_from('<I', data, 0x3c)[0]
    if e_lfanew + 24 > len(data) or data[e_lfanew:e_lfanew + 4] != b'PE\0\0':
        raise Refused("%s has no PE signature at e_lfanew (0x%x) -- a "
                      "truncated or non-PE file" % (path, e_lfanew))
    coff = e_lfanew + 4
    nsect = struct.unpack_from('<H', data, coff + 2)[0]
    optsize = struct.unpack_from('<H', data, coff + 16)[0]
    opt = coff + 20
    magic = struct.unpack_from('<H', data, opt)[0]
    if magic != 0x10b:
        raise Refused(
            "%s is not PE32 (optional header magic 0x%04x). The port links "
            "32-bit MSVC and every address in this guard is a 32-bit one; a "
            "PE32+ image means the toolchain moved and the guard must be "
            "re-derived rather than reinterpreted." % (path, magic))
    base = struct.unpack_from('<I', data, opt + 28)[0]
    sect0 = opt + optsize
    sections = []
    for i in range(nsect):
        row = sect0 + 40 * i
        if row + 40 > len(data):
            raise Refused("%s section table runs past end of file -- "
                          "TRUNCATED. Relink." % path)
        name = data[row:row + 8].rstrip(b'\0').decode('ascii', 'replace')
        vsize, va, rsize, roff = struct.unpack_from('<IIII', data, row + 8)
        sections.append((name, va, vsize or rsize, roff, rsize))
    if not sections:
        raise Refused("%s has no sections" % path)
    return Image(path, data, base, sections)


def scan_frame(img, start, end, callee_va):
    """([call offsets], [jmp offsets]) inside [start, end) reaching callee_va.

    The whole decode. A five-byte form at offset i transfers to
    (i + 5) + rel32, so a hit is an exact address comparison and nothing is
    interpreted. See the banner for why rel8 is not scanned.
    """
    blob = img.read(start, max(end - start, 0))
    if blob is None:
        return None, None
    calls, jmps = [], []
    for i in range(0, len(blob) - 4):
        op = blob[i]
        if op != CALL and op != JMP:
            continue
        rel = struct.unpack_from('<i', blob, i + 1)[0]
        if (start + i + 5 + rel) & 0xffffffff == callee_va:
            (calls if op == CALL else jmps).append(start + i)
    return calls, jmps


# ---------------------------------------------------------------------------
# the check
# ---------------------------------------------------------------------------

class Boundaries(object):
    """Frame spans and address fold counts for one map.

    A frame ends at the next STRICTLY GREATER symbol address, which matters
    here rather than being a detail: /OPT:ICF is on and seventeen of the 22
    veneers are folded onto ONE address in this build, so "the next symbol" is
    very often the same address, and a span computed from it would be zero
    bytes and every one of those rows would read as a missing tail jump.
    """

    def __init__(self, mp):
        self.names = {}
        per_sect = {}
        for name, va, sect, _o, _obj, _st in mp.rows:
            self.names.setdefault(va, []).append(name)
            per_sect.setdefault(sect, set()).add(va)
        self.vas = dict((s, sorted(v)) for s, v in per_sect.items())
        self.index = dict(
            (s, dict((va, i) for i, va in enumerate(lst)))
            for s, lst in self.vas.items())

    def span(self, va, sect):
        """[start, end) for a frame, bounded by ITS OWN image section.

        Scoping to the section is not tidiness. Measured on this tree's
        walk_window.map, the last symbol in each image section is 96 to 4096
        bytes short of the first symbol in the next one, so a span taken from
        a globally sorted symbol list would hand the last function in .text a
        2336-byte window running into .rdata. It cannot produce a false green
        -- over-scanning only adds bytes -- but it can produce a false
        failure, and a guard that blocks a build over the last symbol being
        last is a guard people route around.
        """
        lst = self.vas.get(sect)
        i = self.index.get(sect, {}).get(va)
        if lst is None or i is None:
            return None
        if i + 1 < len(lst):
            return (va, lst[i + 1])
        # Last symbol in its section: nothing after it to bound it, so the
        # bound is the section's own end, which Image.read clamps to. 4096 is
        # an upper limit on how much of that tail to look at, not a claim
        # about the function's length.
        return (va, va + 4096)

    def folded(self, va):
        return len(self.names.get(va, ()))


def check_map(root, rows, mp, objsrc, out, report=False):
    """(results, fails) for every declared row against one map.

    results is [(row, verdict, detail)] with verdict in asserted / broken /
    retired / skipped.
    """
    target = os.path.splitext(os.path.basename(mp.path))[0]
    exe = mp.path[:-4] + '.exe'
    img = parse_image(exe)
    if img.base != mp.base:
        raise Refused(
            "%s says the preferred load address is 0x%08x and %s carries "
            "ImageBase 0x%08x. The map and the image disagree about where "
            "every address in both of them is, so nothing read off either can "
            "be trusted. Relink." % (mp.path, mp.base, exe, img.base))
    bounds = Boundaries(mp)
    results, fails, unbacked = [], [], []
    for row in rows:
        frame = mp.lookup(row['frame'])
        if frame is None:
            results.append((row, 'skipped', 'not defined in this map'))
            continue
        fspell, fva, fsect, _foff, fobj = frame
        callee = mp.lookup(row['callee'])
        if callee is None:
            fails.append((row, fva, None,
                          "CALLEE MISSING: %s is defined in this map and its "
                          "callee %s is not.\n"
                          "     The frame cannot transfer to a symbol that is "
                          "not in the binary, so either the callee was "
                          "renamed or\n     /OPT:REF removed it while a caller "
                          "survived. Nothing about the form can be measured."
                          % (row['frame'], row['callee'])))
            results.append((row, 'broken', 'callee not in map'))
            continue
        cspell, cva = callee[0], callee[1]

        # FACT 2, and the row's own retirement. Read before the form is
        # measured, because a displaced frame's form says nothing about the
        # classification and printing a failure over it would be noise.
        src, why = source_of(objsrc, target, fobj, root)
        if row['tu'] is None:
            # A DERIVED ROW WITH NO src/ TU TO NAME. It still gets its form
            # asserted; what it does not get is the displacement retirement,
            # because there is no declared source to compare the build's
            # against. Zero rows today -- all 22 veneers have a matched TU --
            # and the count is carried onto the green line by the caller so
            # this can never become a quiet hole. It costs a loud failure
            # rather than a clean retirement if such a frame is ever
            # displaced, never a false green.
            unbacked.append(row['frame'])
        elif src is None:
            results.append((row, 'skipped', why))
            continue
        elif src.lower() != row['tu'].replace('\\', '/').lower():
            if is_decomp_source(src):
                # NOT A SEAT LANDING. The build is still compiling decomp
                # source for this frame, so nothing was displaced and the
                # declaration is simply wrong. Retiring here is how a mistyped
                # tu disarms a row behind a green.
                fails.append((row, fva, None,
                              "MISDECLARED ROW: %s is declared as compiled "
                              "from %s and the build compiled it from %s.\n"
                              "     Both are the decomp's own source, so "
                              "nothing was displaced and this is a typo in "
                              "the declaration, not a seat\n     landing. A "
                              "row whose tu names the wrong real file retires "
                              "itself and stops asserting anything, which is "
                              "a\n     green that was never earned. Fix the "
                              "tu; do not delete the row."
                              % (row['frame'], row['tu'], src)))
                results.append((row, 'broken', 'misdeclared tu'))
            else:
                results.append((row, 'retired',
                                '%s is compiled from %s (object %s), not %s'
                                % (row['frame'], src, fobj.split(':')[-1],
                                   row['tu'])))
            continue

        span = bounds.span(fva, fsect)
        calls, jmps = scan_frame(img, span[0], span[1], cva)
        if calls is None:
            fails.append((row, fva, span,
                          "FRAME NOT READABLE: %s is at %08x and those %d "
                          "bytes are not mapped in %s.\n"
                          "     The map and the image disagree about the "
                          "layout, which is a stale artifact rather than a "
                          "form regression."
                          % (row['frame'], fva, span[1] - span[0],
                             os.path.basename(exe))))
            results.append((row, 'broken', 'frame not readable'))
            continue
        want_jump = row['form'] == 'jump'
        ok = (bool(jmps) and not calls) if want_jump else \
             (bool(calls) and not jmps)
        detail = ('%d call, %d jmp in %08x..%08x'
                  % (len(calls), len(jmps), span[0], span[1]))
        if ok:
            results.append((row, 'asserted', detail))
            if report:
                out.write("  %-4s %-34s -> %-34s %-5s %s%s\n"
                          % (row['cls'], row['frame'], row['callee'],
                             row['form'], detail,
                             '  (folded x%d)' % bounds.folded(cva)
                             if bounds.folded(cva) > 1 else ''))
            continue
        results.append((row, 'broken', detail))
        fails.append((row, fva, span, _form_failure(
            row, fspell, fva, span, cspell, cva, calls, jmps, fobj, src,
            bounds)))
    return results, fails, unbacked


def _form_failure(row, fspell, fva, span, cspell, cva, calls, jmps, fobj, src,
                  bounds):
    want_jump = row['form'] == 'jump'
    got = []
    if calls:
        got.append("%d E8 call%s (%s)"
                   % (len(calls), '' if len(calls) == 1 else 's',
                      ', '.join('%08x' % a for a in calls[:4])))
    if jmps:
        got.append("%d E9 jmp%s (%s)"
                   % (len(jmps), '' if len(jmps) == 1 else 's',
                      ', '.join('%08x' % a for a in jmps[:4])))
    if not got:
        got.append("NEITHER -- no five-byte form in the frame resolves to the "
                   "callee at all")
    return (
        "%s: %s -> %s\n"
        "     frame   %08x..%08x (%d bytes), symbol %s, object %s%s\n"
        "     callee  %08x, symbol %s%s\n"
        "     wanted  an %s rel32 to the callee and NO %s rel32 to it\n"
        "     found   %s"
        % ("TAIL JUMP LOST" if want_jump else "SEAM IS NO LONGER A CALL",
           row['frame'], row['callee'],
           span[0], span[1], span[1] - span[0], fspell, fobj.split(':')[-1],
           '' if src is None else ' from %s' % src,
           cva, cspell,
           '' if bounds.folded(cva) < 2 else
           ' (ICF folded: %d names share that address)' % bounds.folded(cva),
           'E9' if want_jump else 'E8', 'E8' if want_jump else 'E9',
           '; '.join(got)))


def run(root, maps, build_dir=None, out=sys.stdout, report=False,
        sweep=False, rows=None):
    """0 when every declared frame has the form its class requires."""
    try:
        if rows is None:
            rows, nveneer, overlap = declared_rows(root)
        else:
            nveneer = sum(1 for r in rows if r['cls'] == 'V')
            overlap = 0
        objsrc = object_sources(build_dir) if build_dir else {}
    except Refused as e:
        out.write("tailjump_guard: REFUSED -- %s\n" % e)
        return 2
    if not maps:
        out.write("tailjump_guard: REFUSED -- no maps to check. A guard with "
                  "nothing to read is not a green.\n")
        return 2

    parsed = []
    try:
        for path in maps:
            parsed.append(parse_map(path))
    except Refused as e:
        out.write("tailjump_guard: REFUSED -- %s\n" % e)
        return 2

    per_map = []
    try:
        for mp in parsed:
            if report:
                out.write("\n%s\n" % os.path.basename(mp.path))
            per_map.append((mp,) + check_map(root, rows, mp, objsrc, out,
                                             report=report))
    except Refused as e:
        out.write("tailjump_guard: REFUSED -- %s\n" % e)
        return 2

    # A row that exists in NO map is a stale classification, not a pass.
    seen = {}
    for _mp, results, _f, _u in per_map:
        for row, verdict, _d in results:
            seen.setdefault(row['frame'], set()).add(verdict)
    nowhere = [r for r in rows
               if seen.get(r['frame'], {'skipped'}) == {'skipped'}]
    if nowhere:
        out.write(
            "\ntailjump_guard: REFUSED -- %d declared frame(s) are in NONE of "
            "the %d map(s) read:\n" % (len(nowhere), len(parsed)))
        for r in nowhere[:12]:
            out.write("   * %-34s (%s)\n" % (r['frame'], r['cite']))
        out.write(
            "  A frame nothing links is a classification that has gone stale, "
            "and a guard that checked nothing must never\n"
            "  read green. Either the symbol was renamed, or /OPT:REF removed "
            "it, or the slice stopped carrying it -- all three\n"
            "  are changes that must be read and written down, not skipped.\n")
        return 2

    broken = 0
    retired = {}
    asserted = 0
    no_tu = set()
    hosted_names = set()
    for mp, results, fails, unbacked in per_map:
        no_tu.update(unbacked)
        counts = {}
        for row, verdict, detail in results:
            counts[verdict] = counts.get(verdict, 0) + 1
            if verdict == 'asserted':
                asserted += 1
            elif verdict == 'retired':
                retired.setdefault(row['frame'], detail)
        if counts.get('asserted') or counts.get('broken'):
            hosted_names.add(os.path.basename(mp.path))
        # THE WHOLE-CLASS FLIP, said before anything else. Thirty separate
        # failure blocks bury the one fact that explains all thirty.
        jump_rows = [(r, v) for r, v, _d in results if r['form'] == 'jump'
                     and v in ('asserted', 'broken')]
        flipped = [r for r, v in jump_rows if v == 'broken']
        if jump_rows and len(flipped) == len(jump_rows) and len(jump_rows) > 3:
            out.write(
                "\ntailjump_guard: THE WHOLE TAIL-JUMP CLASS FLIPPED in %s -- "
                "all %d hosted forwarders became real calls.\n"
                "  That is not %d separate regressions. One change does this: "
                "the optimisation level. Check /Od, /Ob0, a\n"
                "  dropped /O2, or /OPT:ICF being turned off, on the whole "
                "build or on these TUs via set_source_files_properties,\n"
                "  BEFORE reading a single row below. %s section 5d predicted "
                "exactly this shape.\n"
                % (os.path.basename(mp.path), len(flipped), len(flipped),
                   SEAT))
        for row, fva, span, text in fails:
            broken += 1
            out.write("\ntailjump_guard: %s\n" % text)
            out.write("     class   %s (%s)\n" % (row['cls'], row['cite']))
            out.write("     why     %s\n" % row['note'])
            out.write("     map     %s\n" % mp.path)
            if text.startswith('MISDECLARED ROW'):
                remedy = REMEDY_TU
            elif row['form'] == 'jump':
                remedy = REMEDY_JUMP
            else:
                remedy = REMEDY_CALL
            out.write("  REMEDY: %s\n" % remedy)

    # RETIREMENT IS A MECHANISM, NOT AN EXIT. One row retiring is the AE1 seat
    # landing. EVERY row retiring is a guard that asserted nothing and would
    # print a green saying so in small letters, which is the failure this whole
    # family of tools is written against.
    if rows and not asserted and not broken:
        out.write(
            "\ntailjump_guard: REFUSED -- all %d declared frame(s) RETIRED and "
            "none was asserted, so this guard checked nothing.\n"
            % len(rows))
        for frame, detail in sorted(retired.items())[:8]:
            out.write("   * %s\n" % detail)
        out.write(
            "  A whole set retiring at once is not %d seats landing. It is "
            "the `tu` paths in this file having gone stale\n"
            "  against the build -- read one of the lines above and check it "
            "against build.ninja before changing anything else.\n"
            % len(retired))
        return 2

    read_names = set(os.path.basename(mp.path) for mp in parsed)
    lost = [n for n in HOSTS if n in read_names and n not in hosted_names]
    absent = [n for n in HOSTS if n not in read_names]
    floors = 0
    if lost or (sweep and absent):
        out.write(
            "\ntailjump_guard: COVERAGE FLOOR BROKEN -- the declared frames "
            "are hosted by %d of the %d floor map(s).\n"
            % (len(hosted_names & set(HOSTS)), len(HOSTS)))
        for n in lost:
            out.write("   * %s was read and hosts none of them\n" % n)
        for n in (absent if sweep else []):
            out.write("   * %s is not in the build at all\n" % n)
        out.write(
            "  Either the ov007 slice left that target on purpose, in which "
            "case narrow HOSTS in this file and say in the\n"
            "  commit which measurement says so, or a refactor dropped it and "
            "that target now ships the whole class unchecked.\n")
        floors += 1

    if broken or floors:
        parts = []
        if broken:
            parts.append("%d frame/map pair(s) BROKEN" % broken)
        if floors:
            parts.append("%d coverage floor(s) BROKEN" % floors)
        out.write("\ntailjump_guard: %s.\n" % '; '.join(parts))
        return 1
    line = ("tailjump_guard: %d frames (%d A, %d C, %d veneer derived, %d "
            "overlap), %d assertions over %d map(s)"
            % (len(rows), sum(1 for r in rows if r['cls'] == 'A'),
               sum(1 for r in rows if r['cls'] == 'C'), nveneer, overlap,
               asserted, len(parsed)))
    if no_tu:
        # NEVER SILENT EITHER. These frames get their form asserted but
        # not the displacement retirement, because no source is declared
        # for them to be compared against. Zero on this tree.
        line += ("; %d frame(s) with NO declared tu, form asserted but NOT "
                 "displacement-checked: %s"
                 % (len(no_tu), ', '.join(sorted(no_tu))))
    if retired:
        # NEVER SILENT. A retirement is the mechanism working, and it is also
        # exactly what a wrong `tu` looks like, so it is on the green line by
        # name rather than in a verbose mode nobody passes.
        line += "; RETIRED %d: %s" % (
            len(retired), '; '.join("%s (%s)" % (k, v)
                                    for k, v in sorted(retired.items())))
    out.write(line + " -- forms OK\n")
    return 0


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

IMAGE_BASE = 0x00400000
TEXT_RVA = 0x1000
TEXT_OFF = 0x400


def _fabmap(path, publics, base_line=True):
    """A fabricated real-shaped map. publics: [(name, sect, off, va, obj)]."""
    text = MAP_HEAD if base_line else " walk_window\n\n"
    text += " 0001:00000000 00010000H .text                   CODE\n"
    text += MAP_PUBHDR
    for name, idx, off, va, obj in publics:
        text += " %04x:%08x       %-30s %08x     %s\n" % (idx, off, name, va,
                                                          obj)
    text += MAP_TRAILER
    with open(path, 'w', encoding='utf-8') as f:
        f.write(text)
    return path


def _fabexe(path, code, base=IMAGE_BASE):
    """A fabricated PE32 with one .text section holding `code` at TEXT_RVA."""
    e_lfanew = 0x80
    optsize = 0xe0
    dos = bytearray(e_lfanew)
    dos[0:2] = b'MZ'
    struct.pack_into('<I', dos, 0x3c, e_lfanew)
    coff = struct.pack('<HHIIIHH', 0x14c, 1, 0, 0, 0, optsize, 0x0102)
    opt = bytearray(optsize)
    struct.pack_into('<H', opt, 0, 0x10b)
    struct.pack_into('<I', opt, 28, base)
    raw = (len(code) + 0x1ff) & ~0x1ff
    sect = struct.pack('<8sIIIIIIHHI', b'.text', len(code), TEXT_RVA, raw,
                       TEXT_OFF, 0, 0, 0, 0, 0x60000020)
    blob = bytearray(dos + b'PE\0\0' + coff + bytes(opt) + sect)
    blob += b'\0' * (TEXT_OFF - len(blob))
    blob += code + b'\0' * (raw - len(code))
    with open(path, 'wb') as f:
        f.write(blob)
    return path


def _code(size, planted):
    """A .text blob of `size` bytes with five-byte forms planted in it.

    planted: [(offset into .text, opcode, target VA)]. Everything else is 0xCC,
    which is what MSVC pads with and is a byte the scan must never mistake for
    a transfer.
    """
    buf = bytearray(b'\xcc' * size)
    for off, op, target in planted:
        va = IMAGE_BASE + TEXT_RVA + off
        buf[off] = op
        struct.pack_into('<i', buf, off + 1, target - (va + 5))
    return bytes(buf)


class _Out(object):
    def __init__(self):
        self.buf = []

    def write(self, s):
        self.buf.append(s)

    def text(self):
        return ''.join(self.buf)


def _fabninja(build_dir, rules):
    """A fabricated build.ninja. rules: [(target, objpath, source)]."""
    lines = ["ninja_required_version = 1.5", ""]
    for target, obj, src in rules:
        lines.append(
            "build CMakeFiles\\%s.dir\\%s: C_COMPILER__%s_unscanned_Release %s "
            "|| cmake_object_order_depends_target_%s"
            % (target, obj.replace('/', '\\'), target,
               src.replace(':', '$:').replace('/', '\\'), target))
    os.makedirs(build_dir, exist_ok=True)
    path = os.path.join(build_dir, 'build.ninja')
    with open(path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')
    return path


def selftest():
    """Every arm proven by a mutation that only that arm can catch.

    THE BOTH-SIDES DISCIPLINE IS THE WHOLE POINT of this battery and it is the
    thing a one-directional guard gets wrong. A guard that only checked "is
    there a jump" would pass a frame that jumps AND calls, and would say
    nothing at all about the seven Class A frames. So every form arm appears
    twice, once in each direction: a frame that SHOULD tail-jump fails when it
    calls, and a frame that should call fails when it jumps.
    """
    passed = failed = 0
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
            log.append("  FAIL %-46s %s" % (name, '; '.join(why)))
            log.append("       ---- what it was judging ----")
            for line in out.rstrip().splitlines():
                log.append("       %s" % line)
        else:
            passed += 1
            log.append("  ok   %s" % name)

    tmp = tempfile.mkdtemp(prefix='tailjump_selftest_')
    try:
        bd = os.path.join(tmp, 'build')
        os.makedirs(bd, exist_ok=True)

        # --- the fixture ---------------------------------------------------
        # Two frames and two callees in a fabricated .text. jf is a forwarder
        # that must tail-jump; cf is a seam frame that must call. Both point at
        # their own callee so a mutation of one cannot be caught by the other.
        JF, JC, CF, CC_ = 0x100, 0x200, 0x300, 0x400
        NEXT = 0x500
        VA = lambda off: IMAGE_BASE + TEXT_RVA + off      # noqa: E731

        def pubs(extra=()):
            rows = [('_veneer_frame', 1, JF, VA(JF), 'veneer_frame.c.obj'),
                    ('_veneer_target', 1, JC, VA(JC), 'veneer_target.c.obj'),
                    ('_seam_frame', 1, CF, VA(CF), 'seam_frame.c.obj'),
                    ('_seam_callee', 1, CC_, VA(CC_), 'seam_callee.c.obj'),
                    ('_after', 1, NEXT, VA(NEXT), 'after.c.obj')]
            return rows + list(extra)

        JROW = {'frame': 'veneer_frame', 'callee': 'veneer_target',
                'form': 'jump', 'cls': 'V', 'tu': 'src/veneer_frame.c',
                'cite': 'fixture', 'note': 'fixture forwarder'}
        CROW = {'frame': 'seam_frame', 'callee': 'seam_callee',
                'form': 'call', 'cls': 'A', 'tu': 'src/seam_frame.c',
                'cite': 'fixture', 'note': 'fixture seam'}
        BOTH = (JROW, CROW)

        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])

        def build(name, planted, publics=None, rules=None):
            """(map path) for a fabricated map+exe pair called <name>."""
            code = _code(0x600, planted)
            _fabexe(os.path.join(bd, name + '.exe'), code)
            return _fabmap(os.path.join(bd, name + '.map'),
                           pubs() if publics is None else publics)

        def go(maps, rows=BOTH, report=False, sweep=False, root=tmp,
               build_dir=bd):
            o = _Out()
            rc = run(root, maps, build_dir=build_dir, out=o, report=report,
                     sweep=sweep, rows=list(rows))
            return rc, o.text()

        HEALTHY = [(JF, JMP, VA(JC)), (CF, CALL, VA(CC_))]

        # 1. HEALTHY: the forwarder jumps, the seam calls -------------------
        m = build('walk_window', HEALTHY)
        rc, out = go([m])
        case('healthy: forwarder jumps, seam calls', 0, rc, out,
             wants=['forms OK', '2 assertions'])

        # 2. BOTH SIDES. The two mutations the review named, each proven to
        #    fail and to name its own row and nothing else.
        # 2a. the forwarder grew a body: the tail jump became a call.
        m = build('walk_window', [(JF, CALL, VA(JC)), (CF, CALL, VA(CC_))])
        rc, out = go([m])
        case('a forwarder that CALLS fails', 1, rc, out,
             wants=['TAIL JUMP LOST', 'veneer_frame -> veneer_target',
                    '1 E8 call', 'REMEDY'],
             unwanted=['seam_frame'])

        # 2b. the other side: a seam frame that became a tail jump.
        m = build('walk_window', [(JF, JMP, VA(JC)), (CF, JMP, VA(CC_))])
        rc, out = go([m])
        case('a seam frame that JUMPS fails', 1, rc, out,
             wants=['SEAM IS NO LONGER A CALL', 'seam_frame -> seam_callee',
                    '1 E9 jmp'],
             unwanted=['veneer_frame ->'])

        # 2c. NEITHER form present. A frame whose transfer vanished entirely
        #     (inlined, folded away, callee replaced) must not read as green
        #     just because the forbidden form is also absent.
        m = build('walk_window', [(CF, CALL, VA(CC_))])
        rc, out = go([m])
        case('a forwarder with NO transfer fails', 1, rc, out,
             wants=['TAIL JUMP LOST', 'NEITHER'])

        # 2d. BOTH forms present. The one a "is there a jump" check passes.
        #     MSVC does emit a call and a jump to the same callee in one frame
        #     -- a conditional forward and a tail call -- and for a veneer that
        #     means the ride is gone on one path.
        m = build('walk_window',
                  [(JF, JMP, VA(JC)), (JF + 0x20, CALL, VA(JC)),
                   (CF, CALL, VA(CC_))])
        rc, out = go([m])
        case('a forwarder that jumps AND calls fails', 1, rc, out,
             wants=['TAIL JUMP LOST', '1 E8 call', '1 E9 jmp'])

        # 3. THE DISPLACEMENT RETIREMENT, which is the AE1 mechanism ---------
        #    Same broken bytes as 2a. The row must go quiet because the frame
        #    is no longer compiled from the declared TU -- and must say so.
        _fabninja(bd, [
            ('walk_window', 'unmatched/Host_veneer_frame.cpp.obj',
             tmp + '/port/unmatched/Host_veneer_frame.cpp'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])
        displaced = [('_veneer_frame', 1, JF, VA(JF),
                      'Host_veneer_frame.cpp.obj'),
                     ('_veneer_target', 1, JC, VA(JC), 'veneer_target.c.obj'),
                     ('_seam_frame', 1, CF, VA(CF), 'seam_frame.c.obj'),
                     ('_seam_callee', 1, CC_, VA(CC_), 'seam_callee.c.obj'),
                     ('_after', 1, NEXT, VA(NEXT), 'after.c.obj')]
        code = _code(0x600, [(JF, CALL, VA(JC)), (CF, CALL, VA(CC_))])
        _fabexe(os.path.join(bd, 'walk_window.exe'), code)
        m = _fabmap(os.path.join(bd, 'walk_window.map'), displaced)
        rc, out = go([m])
        case('a displaced frame RETIRES instead of failing', 0, rc, out,
             wants=['forms OK', 'RETIRED 1', 'veneer_frame',
                    'port/unmatched/Host_veneer_frame.cpp'],
             unwanted=['TAIL JUMP LOST'])

        # 3b. AND THE RETIREMENT IS NOT A HOLE. Put the matched TU back and
        #     the row re-arms in the same run, unasked. This is the half that
        #     makes it a guard rather than an escape hatch.
        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])
        m = _fabmap(os.path.join(bd, 'walk_window.map'), pubs())
        rc, out = go([m])
        case('restoring the matched TU re-arms the row', 1, rc, out,
             wants=['TAIL JUMP LOST', 'veneer_frame'])

        # 3c. THE BASENAME HOLE, which is why the check reads build.ninja and
        #     not the map alone. A host copy named after the matched TU
        #     produces an IDENTICAL object basename, so the map cannot tell
        #     them apart -- objsrc_check.py's eighteen. The source path can.
        _fabninja(bd, [
            ('walk_window', 'unmatched/veneer_frame.c.obj',
             tmp + '/port/unmatched/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])
        code = _code(0x600, [(JF, CALL, VA(JC)), (CF, CALL, VA(CC_))])
        _fabexe(os.path.join(bd, 'walk_window.exe'), code)
        m = _fabmap(os.path.join(bd, 'walk_window.map'), pubs())
        rc, out = go([m])
        case('a same-basename host copy still retires', 0, rc, out,
             wants=['RETIRED 1', 'port/unmatched/veneer_frame.c'],
             unwanted=['TAIL JUMP LOST'])

        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])

        # 3d. A DERIVED ROW WITH NO src/ TU. Its form is still asserted and
        #     the fact that it got no displacement check is on the green line.
        #     Zero such rows on this tree, which is exactly why the arm is
        #     here: an untested branch that only fires on a tree nobody has
        #     yet is the branch that will be wrong when it does.
        _fabexe(os.path.join(bd, 'walk_window.exe'), _code(0x600, HEALTHY))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), pubs())
        rc, out = go([m], rows=(dict(JROW, tu=None), CROW))
        case('a row with no declared tu is asserted and named', 0, rc, out,
             wants=['forms OK', 'NO declared tu', 'veneer_frame',
                    'NOT displacement-checked'])
        #     ...and its form is still judged, in both directions
        _fabexe(os.path.join(bd, 'walk_window.exe'),
                _code(0x600, [(JF, CALL, VA(JC)), (CF, CALL, VA(CC_))]))
        rc, out = go([m], rows=(dict(JROW, tu=None), CROW))
        case('a row with no declared tu still fails on form', 1, rc, out,
             wants=['TAIL JUMP LOST', 'veneer_frame'])
        _fabexe(os.path.join(bd, 'walk_window.exe'), _code(0x600, HEALTHY))

        # 4. THE WHOLE-CLASS FLIP, the /Od shape ----------------------------
        #    Five forwarders, all become calls in one build. The class line
        #    must come out before any row, because thirty row failures bury
        #    the one sentence that explains them.
        od_rows, od_pubs, od_plant, od_rules = [], [], [], []
        for i in range(5):
            foff, coff = 0x100 + i * 0x40, 0x800 + i * 0x40
            od_pubs.append(('_fwd%d' % i, 1, foff, VA(foff),
                            'fwd%d.c.obj' % i))
            od_pubs.append(('_tgt%d' % i, 1, coff, VA(coff),
                            'tgt%d.c.obj' % i))
            od_rows.append({'frame': 'fwd%d' % i, 'callee': 'tgt%d' % i,
                            'form': 'jump', 'cls': 'V',
                            'tu': 'src/fwd%d.c' % i, 'cite': 'fixture',
                            'note': 'fixture forwarder %d' % i})
            od_plant.append((foff, CALL, VA(coff)))
            od_rules.append(('od', 'src/fwd%d.c.obj' % i,
                             tmp + '/src/fwd%d.c' % i))
            od_rules.append(('od', 'src/tgt%d.c.obj' % i,
                             tmp + '/src/tgt%d.c' % i))
        od_pubs.append(('_after', 1, 0xa00, VA(0xa00), 'after.c.obj'))
        odbd = os.path.join(tmp, 'odbuild')
        os.makedirs(odbd, exist_ok=True)
        _fabninja(odbd, od_rules)
        _fabexe(os.path.join(odbd, 'od.exe'), _code(0xc00, od_plant))
        m = _fabmap(os.path.join(odbd, 'od.map'), od_pubs)
        o = _Out()
        rc = run(tmp, [m], build_dir=odbd, out=o, rows=od_rows)
        text = o.text()
        case('a whole-class flip is reported as one thing', 1, rc, text,
             wants=['THE WHOLE TAIL-JUMP CLASS FLIPPED',
                    'all 5 hosted forwarders became real calls', '/Od'])
        case('the class line comes BEFORE the rows', 1, rc, text,
             wants=['CLASS FLIPPED'])
        if 'CLASS FLIPPED' in text and 'TAIL JUMP LOST' in text:
            case('class line precedes the first row failure', 1, rc,
                 text if text.index('CLASS FLIPPED') <
                 text.index('TAIL JUMP LOST') else 'ORDER WRONG',
                 wants=['CLASS FLIPPED'])

        #    ...and ONE of five flipping is NOT a class flip.
        one = list(od_plant)
        for i in range(1, 5):
            one[i] = (0x100 + i * 0x40, JMP, VA(0x800 + i * 0x40))
        _fabexe(os.path.join(odbd, 'od.exe'), _code(0xc00, one))
        o = _Out()
        rc = run(tmp, [m], build_dir=odbd, out=o, rows=od_rows)
        case('one flip of five is NOT a class flip', 1, rc, o.text(),
             wants=['TAIL JUMP LOST', 'fwd0'],
             unwanted=['CLASS FLIPPED'])

        # 5. ICF FOLDING. Seventeen real veneers share one address in this
        #    build, so a folded frame must be measured over the span to the
        #    next DISTINCT address and not over zero bytes.
        folded_pubs = [('_veneer_frame', 1, JF, VA(JF), 'veneer_frame.c.obj'),
                       ('_fold_twin', 1, JF, VA(JF), 'fold_twin.c.obj'),
                       ('_fold_third', 1, JF, VA(JF), 'fold_third.c.obj'),
                       ('_veneer_target', 1, JC, VA(JC), 'veneer_target.c.obj'),
                       ('_seam_frame', 1, CF, VA(CF), 'seam_frame.c.obj'),
                       ('_seam_callee', 1, CC_, VA(CC_), 'seam_callee.c.obj'),
                       ('_after', 1, NEXT, VA(NEXT), 'after.c.obj')]
        _fabexe(os.path.join(bd, 'walk_window.exe'), _code(0x600, HEALTHY))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), folded_pubs)
        rc, out = go([m])
        case('a folded frame is still measured', 0, rc, out,
             wants=['forms OK'])

        #    and a folded CALLEE is reported as ambiguous in a failure
        fold_callee = [('_veneer_frame', 1, JF, VA(JF), 'veneer_frame.c.obj'),
                       ('_veneer_target', 1, JC, VA(JC), 'veneer_target.c.obj'),
                       ('_other_target', 1, JC, VA(JC), 'other.c.obj'),
                       ('_seam_frame', 1, CF, VA(CF), 'seam_frame.c.obj'),
                       ('_seam_callee', 1, CC_, VA(CC_), 'seam_callee.c.obj'),
                       ('_after', 1, NEXT, VA(NEXT), 'after.c.obj')]
        _fabexe(os.path.join(bd, 'walk_window.exe'),
                _code(0x600, [(JF, CALL, VA(JC)), (CF, CALL, VA(CC_))]))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), fold_callee)
        rc, out = go([m])
        case('a folded callee is named as folded in the failure', 1, rc, out,
             wants=['ICF folded: 2 names share that address'])

        # 6. THE PADDING CONTROL. A frame full of 0xCC must produce no hits,
        #    and a rel32 that lands one byte off the callee must not count.
        _fabexe(os.path.join(bd, 'walk_window.exe'),
                _code(0x600, [(JF, JMP, VA(JC) + 1), (CF, CALL, VA(CC_))]))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), pubs())
        rc, out = go([m])
        case('a transfer one byte off the callee does not count', 1, rc, out,
             wants=['TAIL JUMP LOST', 'NEITHER'])

        # 6b. THE LAST SYMBOL IN ITS SECTION. Its span has no successor to
        #     bound it, so the bound is the section end and the read clamps
        #     there. Before the span was section-scoped this frame got a
        #     window running into the next section and read as unmeasurable,
        #     which is a build blocked over a symbol being last.
        TAILF = 0x5f0
        tail_pubs = pubs() + [('_tail_frame', 1, TAILF, VA(TAILF),
                               'tail_frame.c.obj')]
        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
            ('walk_window', 'src/tail_frame.c.obj',
             tmp + '/src/tail_frame.c'),
        ])
        tail_row = {'frame': 'tail_frame', 'callee': 'veneer_target',
                    'form': 'jump', 'cls': 'V', 'tu': 'src/tail_frame.c',
                    'cite': 'fixture', 'note': 'fixture tail-of-section'}
        _fabexe(os.path.join(bd, 'walk_window.exe'),
                _code(0x600, HEALTHY + [(TAILF, JMP, VA(JC))]))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), tail_pubs)
        rc, out = go([m], rows=(tail_row,))
        case('the last symbol in a section is still measured', 0, rc, out,
             wants=['forms OK'], unwanted=['NOT READABLE'])
        #     and it is still judged: same position, wrong form
        _fabexe(os.path.join(bd, 'walk_window.exe'),
                _code(0x600, HEALTHY + [(TAILF, CALL, VA(JC))]))
        rc, out = go([m], rows=(tail_row,))
        case('the last symbol in a section still fails on form', 1, rc, out,
             wants=['TAIL JUMP LOST', 'tail_frame'])

        # 6c. A FRAME AT AN ADDRESS IN NO SECTION. The map and the image
        #     disagree about the layout, which is a stale artifact, and the
        #     one case where an unreadable frame is the right verdict.
        ghost_pubs = pubs() + [('_ghost_frame', 1, 0x900000,
                                IMAGE_BASE + 0x900000, 'ghost.c.obj')]
        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
            ('walk_window', 'src/ghost.c.obj', tmp + '/src/ghost.c'),
        ])
        _fabexe(os.path.join(bd, 'walk_window.exe'), _code(0x600, HEALTHY))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), ghost_pubs)
        rc, out = go([m], rows=({'frame': 'ghost_frame',
                                 'callee': 'veneer_target', 'form': 'jump',
                                 'cls': 'V', 'tu': 'src/ghost.c',
                                 'cite': 'fixture', 'note': 'fixture ghost'},))
        case('a frame at an unmapped address fails as unreadable', 1, rc, out,
             wants=['FRAME NOT READABLE', 'ghost_frame'])

        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])

        # 7. REFUSALS -------------------------------------------------------
        #    a declared frame in NO map
        _fabexe(os.path.join(bd, 'walk_window.exe'), _code(0x600, HEALTHY))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), pubs())
        ghost = dict(JROW, frame='never_linked')
        rc, out = go([m], rows=(ghost, CROW))
        case('a frame in NO map -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'never_linked', 'gone stale'])

        #    ...but a frame missing from ONE map of two is skipped
        _fabexe(os.path.join(bd, 'other.exe'), _code(0x600, HEALTHY))
        m2 = _fabmap(os.path.join(bd, 'other.map'),
                     [('_seam_frame', 1, CF, VA(CF), 'seam_frame.c.obj'),
                      ('_seam_callee', 1, CC_, VA(CC_), 'seam_callee.c.obj'),
                      ('_after', 1, NEXT, VA(NEXT), 'after.c.obj')])
        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
            ('other', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('other', 'src/seam_callee.c.obj', tmp + '/src/seam_callee.c'),
            ('other', 'src/after.c.obj', tmp + '/src/after.c'),
        ])
        rc, out = go([m, m2])
        case('a frame missing from one map of two is skipped', 0, rc, out,
             wants=['forms OK'])

        #    a map whose callee vanished
        nocallee = [('_veneer_frame', 1, JF, VA(JF), 'veneer_frame.c.obj'),
                    ('_seam_frame', 1, CF, VA(CF), 'seam_frame.c.obj'),
                    ('_seam_callee', 1, CC_, VA(CC_), 'seam_callee.c.obj'),
                    ('_after', 1, NEXT, VA(NEXT), 'after.c.obj')]
        m3 = _fabmap(os.path.join(bd, 'walk_window.map'), nocallee)
        rc, out = go([m3])
        case('a frame whose callee vanished fails', 1, rc, out,
             wants=['CALLEE MISSING', 'veneer_target'])

        #    no build.ninja: the second fact is unavailable, so REFUSE rather
        #    than assert over rows that may be displaced
        m = _fabmap(os.path.join(bd, 'walk_window.map'), pubs())
        o = _Out()
        rc = run(tmp, [m], build_dir=os.path.join(tmp, 'nodir'), out=o,
                 rows=list(BOTH))
        case('no build.ninja -> REFUSED', 2, rc, o.text(),
             wants=['REFUSED', 'build.ninja', 'basename alone'])

        #    a truncated map, inherited from the shared reader
        with open(m, 'r', encoding='utf-8') as f:
            head = f.read().split(' entry point at')[0]
        trunc = os.path.join(bd, 'trunc.map')
        with open(trunc, 'w', encoding='utf-8') as f:
            f.write(head)
        shutil.copy(os.path.join(bd, 'walk_window.exe'),
                    os.path.join(bd, 'trunc.exe'))
        rc, out = go([trunc])
        case('a truncated map -> REFUSED (shared reader)', 2, rc, out,
             wants=['REFUSED', 'TRUNCATED'])

        #    a missing exe beside a real map
        noexe = _fabmap(os.path.join(tmp, 'lonely.map'), pubs())
        rc, out = go([noexe])
        case('a map with no exe -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'the link did not run'])

        #    a non-PE file where the exe should be
        with open(os.path.join(bd, 'junk.exe'), 'wb') as f:
            f.write(b'not a pe file at all, just some bytes' * 4)
        junk = _fabmap(os.path.join(bd, 'junk.map'), pubs())
        rc, out = go([junk])
        case('a non-PE exe -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'no MZ header'])

        #    map and image disagreeing about the load address
        _fabexe(os.path.join(bd, 'rebased.exe'), _code(0x600, HEALTHY),
                base=0x10000000)
        reb = _fabmap(os.path.join(bd, 'rebased.map'), pubs())
        rc, out = go([reb])
        case('map/image base disagreement -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'disagree about where'])

        #    no maps at all
        rc, out = go([])
        case('no maps -> REFUSED', 2, rc, out, wants=['REFUSED'])

        # 8. THE COVERAGE FLOOR ---------------------------------------------
        floor_rows = list(BOTH)
        _fabexe(os.path.join(bd, 'walk_window.exe'), _code(0x600, HEALTHY))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), pubs())
        o = _Out()
        rc = run(tmp, [m], build_dir=bd, out=o, rows=floor_rows, sweep=True)
        case('a sweep missing floor maps FAILS', 1, rc, o.text(),
             wants=['COVERAGE FLOOR BROKEN', 'not in the build at all',
                    'smoke_player.map'])
        o = _Out()
        rc = run(tmp, [m], build_dir=bd, out=o, rows=floor_rows, sweep=False)
        case('a partial run does not judge absent floor maps', 0, rc, o.text(),
             wants=['forms OK'], unwanted=['COVERAGE FLOOR'])

        # 9. THE VENEER DERIVATION, on a fabricated overlay ------------------
        #    No ROM bytes: the image is fabricated and the two constants are
        #    ARM instruction encodings, which are the guard's own and not the
        #    game's. What is proven is the scan, the resolution of both ends
        #    and the tripwire.
        vroot = os.path.join(tmp, 'vroot')
        vov = os.path.join(vroot, 'extracted', 'overlays')
        os.makedirs(vov, exist_ok=True)
        os.makedirs(os.path.join(vroot, 'config', 'arm9', 'overlays',
                                 'ov007'), exist_ok=True)
        VB = 0x020ad660
        blob = bytearray(b'\x00' * 0x100)
        spots = (0x00, 0x40, 0x80)
        for k, off in enumerate(spots):
            struct.pack_into('<III', blob, off, 0xE59FC000, 0xE12FFF1C,
                             VB + 0xc0 + 4 * k)
        with open(os.path.join(vov, 'overlay_0007.bin'), 'wb') as f:
            f.write(bytes(blob))
        rows_cfg = [('func_ov007_%08x' % (VB + o), VB + o) for o in spots]
        tgts = [VB + 0xc0 + 4 * k for k in range(3)]
        rows_cfg += [('func_ov007_%08x' % a, a) for a in tgts]
        rows_cfg += [('func_ov007_%08x' % VB, VB)]
        with open(os.path.join(vroot, 'config', 'arm9', 'overlays', 'ov007',
                               'symbols.txt'), 'w', encoding='utf-8') as f:
            for n, a in rows_cfg:
                f.write("%s kind:function(arm,size=0xc) addr:0x%08x\n" % (n, a))
        os.makedirs(os.path.join(vroot, 'config', 'arm9'), exist_ok=True)
        with open(os.path.join(vroot, 'config', 'arm9', 'symbols.txt'), 'w',
                  encoding='utf-8') as f:
            f.write("stub kind:function(arm,size=0x4) addr:0x02004000\n")
        spec = dict(VENEER, count=3)
        got = derive_veneers(vroot, spec)
        case('the veneer scan finds a fabricated set', 0,
             0 if len(got) == 3 else 1,
             "found %d: %s" % (len(got), [r['frame'] for r in got]),
             wants=['found 3'])
        case('every derived row is a jump row', 0,
             0 if all(r['form'] == 'jump' and r['cls'] == 'V' for r in got)
             else 1, 'forms %s' % [r['form'] for r in got],
             wants=["'jump'"])
        try:
            derive_veneers(vroot, dict(VENEER, count=99))
            case('a count tripwire -> REFUSED', 2, 0, 'no refusal')
        except Refused as e:
            case('a count tripwire -> REFUSED', 2, 2, "REFUSED %s" % e,
                 wants=['occurs 3 times', 'declares 99'])
        try:
            derive_veneers(vroot, dict(VENEER,
                                       image=('extracted', 'nope.bin')))
            case('a missing overlay image -> REFUSED', 2, 0, 'no refusal')
        except Refused as e:
            case('a missing overlay image -> REFUSED', 2, 2,
                 "REFUSED %s" % e, wants=['no overlay image'])
        #    an unresolvable end must refuse rather than shrink the set
        with open(os.path.join(vroot, 'config', 'arm9', 'overlays', 'ov007',
                               'symbols.txt'), 'w', encoding='utf-8') as f:
            for n, a in rows_cfg:
                if a == VB + spots[1]:
                    continue
                f.write("%s kind:function(arm,size=0xc) addr:0x%08x\n" % (n, a))
        try:
            derive_veneers(vroot, spec)
            case('an unnamed veneer end -> REFUSED', 2, 0, 'no refusal')
        except Refused as e:
            case('an unnamed veneer end -> REFUSED', 2, 2, "REFUSED %s" % e,
                 wants=['do not resolve to config symbols'])

        # 9b. THE DUAL-SET DISAGREEMENT. Two frames are in both the hand list
        #     and the derived set, and the guard cross-checks that the two
        #     name the same callee. The review found that check was
        #     hand-proven and fixture-less: deleting it passed the battery.
        #     These two arms are the fixture. vroot's fabricated overlay puts
        #     a veneer at VB+0x40 jumping to VB+0xc4; a hand row claiming that
        #     frame CALLS something else is the disagreement.
        with open(os.path.join(vroot, 'config', 'arm9', 'overlays', 'ov007',
                               'symbols.txt'), 'w', encoding='utf-8') as f:
            for n, a in rows_cfg:
                f.write("%s kind:function(arm,size=0xc) addr:0x%08x\n" % (n, a))
        vfr = 'func_ov007_%08x' % (VB + spots[1])
        vcal = 'func_ov007_%08x' % (VB + 0xc0 + 4)
        try:
            declared_rows(vroot, spec=spec,
                          hand=({'frame': vfr, 'callee': 'somewhere_else',
                                 'form': 'jump', 'cls': 'C', 'tu': None,
                                 'cite': 'fixture',
                                 'note': 'fixture disagreement'},))
            case('a dual-set callee disagreement -> REFUSED', 2, 0,
                 'no refusal')
        except Refused as e:
            case('a dual-set callee disagreement -> REFUSED', 2, 2,
                 "REFUSED %s" % e,
                 wants=['is hand-declared', 'somewhere_else',
                        'will not pick'])
        try:
            declared_rows(vroot, hand=({'frame': vfr, 'callee': vcal,
                                        'form': 'call', 'cls': 'A',
                                        'tu': None, 'cite': 'fixture',
                                        'note': 'fixture form clash'},),
                          spec=spec)
            case('a dual-set FORM disagreement -> REFUSED', 2, 0, 'no refusal')
        except Refused as e:
            case('a dual-set FORM disagreement -> REFUSED', 2, 2,
                 "REFUSED %s" % e,
                 wants=["hand-declared form 'call'", "derives as 'jump'"])
        #     ...and the agreeing case must NOT refuse, or the check is just a
        #     ban on overlap rather than a cross-check.
        try:
            got2, nv2, ov2 = declared_rows(
                vroot, hand=({'frame': vfr, 'callee': vcal, 'form': 'jump',
                              'cls': 'C', 'tu': None, 'cite': 'fixture',
                              'note': 'fixture agreement'},), spec=spec)
            case('an AGREEING dual-set row is accepted as overlap', 0,
                 0 if (len(got2) == 3 and ov2 == 1) else 1,
                 'rows %d overlap %d' % (len(got2), ov2),
                 wants=['rows 3 overlap 1'])
        except Refused as e:
            case('an AGREEING dual-set row is accepted as overlap', 0, 2,
                 "REFUSED %s" % e)

        # 9c. THE ALL-ROWS-RETIRED REFUSAL, also hand-proven and fixture-less
        #     until the review said so. A whole set retiring at once is not a
        #     wave of seats landing, it is the tu paths having gone stale, and
        #     a guard that asserted nothing must never print a green.
        _fabninja(bd, [
            ('walk_window', 'unmatched/Host_veneer_frame.cpp.obj',
             tmp + '/port/unmatched/Host_veneer_frame.cpp'),
            ('walk_window', 'unmatched/Host_seam_frame.cpp.obj',
             tmp + '/port/unmatched/Host_seam_frame.cpp'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])
        all_gone = [('_veneer_frame', 1, JF, VA(JF),
                     'Host_veneer_frame.cpp.obj'),
                    ('_veneer_target', 1, JC, VA(JC), 'veneer_target.c.obj'),
                    ('_seam_frame', 1, CF, VA(CF), 'Host_seam_frame.cpp.obj'),
                    ('_seam_callee', 1, CC_, VA(CC_), 'seam_callee.c.obj'),
                    ('_after', 1, NEXT, VA(NEXT), 'after.c.obj')]
        _fabexe(os.path.join(bd, 'walk_window.exe'), _code(0x600, HEALTHY))
        m = _fabmap(os.path.join(bd, 'walk_window.map'), all_gone)
        rc, out = go([m])
        case('every row retiring at once -> REFUSED', 2, rc, out,
             wants=['REFUSED', 'checked nothing', 'gone stale'],
             unwanted=['forms OK'])

        # 9d. F1: A tu MISTYPED TO ANOTHER REAL src FILE. The review's find.
        #     It used to disarm the row at exit 0, named on the green line but
        #     reading as a legitimate retirement. It is a typo, not a seat,
        #     and the build is still compiling decomp source for that frame.
        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])
        m = _fabmap(os.path.join(bd, 'walk_window.map'), pubs())
        typo = dict(JROW, tu='src/veneer_target.c')     # a REAL other src file
        rc, out = go([m], rows=(typo, CROW))
        case('a tu mistyped to another real src file FAILS', 1, rc, out,
             wants=['MISDECLARED ROW', 'src/veneer_target.c',
                    'src/veneer_frame.c', 'not a seat', 'Fix the tu'],
             unwanted=['forms OK'])
        #     ...and the discriminator reads the REAL source, not the declared
        #     one. A tu that names a host copy while the build is still
        #     compiling src/ is a misdeclaration too: the frame did not move,
        #     so there is nothing to retire. Getting this backwards would let
        #     anyone disarm a row by pointing its tu at port/unmatched/.
        rc, out = go([m], rows=(dict(JROW, tu='port/unmatched/Nope.cpp'),
                                CROW))
        case('a tu naming a host copy that is NOT built also fails', 1, rc,
             out, wants=['MISDECLARED ROW', 'port/unmatched/Nope.cpp',
                         'src/veneer_frame.c'],
             unwanted=['RETIRED'])
        #     ...and hostgen's rewrite of a matched TU is NOT a displacement.
        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/build/port/host-src/src/veneer_frame.cpp'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])
        rc, out = go([m])
        case('a hostgen rewrite is misdeclared, never retired', 1, rc, out,
             wants=['MISDECLARED ROW', 'host-src'],
             unwanted=['RETIRED'])
        _fabninja(bd, [
            ('walk_window', 'src/veneer_frame.c.obj',
             tmp + '/src/veneer_frame.c'),
            ('walk_window', 'src/veneer_target.c.obj',
             tmp + '/src/veneer_target.c'),
            ('walk_window', 'src/seam_frame.c.obj', tmp + '/src/seam_frame.c'),
            ('walk_window', 'src/seam_callee.c.obj',
             tmp + '/src/seam_callee.c'),
            ('walk_window', 'src/after.c.obj', tmp + '/src/after.c'),
        ])

        # 10. THE DECLARATION ITSELF, against this tree ---------------------
        #     Not a fixture: the shipped rows must resolve against this
        #     checkout's config and ROM image, or the guard is dead on
        #     arrival. This is the arm that fails the day the overlay, the
        #     config or the seat file's classification moves.
        real_root = default_root()
        try:
            rows, nven, overlap = declared_rows(real_root)
            frames = set(r['frame'] for r in rows)
            ok = (nven == VENEER['count'] and len(rows) == len(frames))
            log.append("  %s declared set resolves: %d frames (%d A, %d C, "
                       "%d veneer, %d overlap)"
                       % ('ok  ' if ok else 'FAIL', len(rows),
                          sum(1 for r in rows if r['cls'] == 'A'),
                          sum(1 for r in rows if r['cls'] == 'C'), nven,
                          overlap))
            passed += 1 if ok else 0
            failed += 0 if ok else 1
            missing = [r['tu'] for r in rows
                       if r['tu'] and not os.path.isfile(
                           os.path.join(real_root, r['tu']))]
            if missing:
                failed += 1
                log.append("  FAIL declared tu files do not exist: %s"
                           % ', '.join(missing))
            else:
                passed += 1
                log.append("  ok   every declared tu exists in this tree")
        except Refused as e:
            failed += 1
            log.append("  FAIL the declared set does not resolve: %s" % e)
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
                    help='repo root holding config/ and extracted/')
    ap.add_argument('--report', action='store_true',
                    help='one line per asserted frame, with the hit counts')
    ap.add_argument('--selftest', action='store_true',
                    help='run the fixture battery and exit')
    args = ap.parse_args()

    if args.selftest:
        return selftest()

    maps = [os.path.normpath(m) for m in args.map]
    build_dir = args.build_dir
    sweep = False
    if args.build_dir:
        try:
            found, orphans = find_maps(args.build_dir)
        except Refused as e:
            print("tailjump_guard: REFUSED -- %s" % e)
            return 2
        maps += found
        sweep = True
        if orphans and args.report:
            print("  %d orphan map(s) ignored (no executable beside them): %s"
                  % (len(orphans), ', '.join(orphans)))
    if not maps:
        build_dir = build_dir or os.path.join(args.root, 'build', 'port')
        maps = [os.path.join(build_dir, 'walk_window.map')]
    if build_dir is None:
        build_dir = os.path.dirname(maps[0])
    return run(args.root, maps, build_dir=build_dir, report=args.report,
               sweep=sweep)


if __name__ == '__main__':
    sys.exit(main())
