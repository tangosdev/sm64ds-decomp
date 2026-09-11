# PR #2474: hosted fader calling convention

The repaired filesystem boot let the full battery reach level 3, where the
installed fader crashed. `src/func_02018efc.c` calls vtable slot 2 with a cdecl
receiver on the stack. The installed `HalFaderWipe::AdvanceFade` expected its
receiver in ECX. The debugger measured ECX as 0x332dcf86 while the real installed
object pointer was on the stack. This is **PORT-FADER-01**.

Only that host member now declares `__cdecl`. Its raw slot caller and qualified
host calls therefore agree. The other slots keep their existing calling
conventions, and the seven objects remain 0x60 bytes each. No ROM source or shared
class header changes.

**PORT-FADER-02** concerns the diagnostic index helper. It subtracted pointers
from different objects, including the separate color fader. It now compares
integer addresses against the seven-element array and accepts only aligned
object starts. Null, foreign, misaligned and one-past addresses report -1.

`python port/tools/test_fader_wipes_abi.py -v` passes three native x86 tests.
The fixture compiles the actual HalFaderWipe class and index-function text from
the source file, plus the complete actual C caller. All seven objects pass raw
and qualified calls, driven and undriven paths, stack-balance checks and calls
through the other virtual slots. It also checks a null installed pointer, an
independent cdecl table and index boundaries. Removing only the cdecl annotation
in a separate fixture reproduces failure. Interpolator and blend callees are
controlled fixture implementations; these tests prove the caller/receiver and
field contracts, not ROM arithmetic or full rendering.

The full native battery provides the separate real-runtime result: the build,
all 20 smoke programs and levels 0 through 21 pass. Level 22 reaches a different
access violation in ModelComponents::UpdateBones from Chuckya::Behavior, with
an invalid animation pointer. That failure remains unresolved. Later levels,
scenes, shipping runtime, multiplayer and rewind are not accepted here.

Local evidence: `build/fader-actual-tests.log`,
`build/fader-abi-probe/results.json`, `build/fader-full-battery.log`,
`build/level3-debug/result.json` and `build/level22-debug/result.json` in the
producer worktree. Source base and actual PR target remain b99310a8 and
31f31604 respectively; target composition and public publication are separate.
The prior PORT2474-01 and PORT2474-02 findings retain their dispositions.

Independent review found **PORT-FADER-03**: the qualified-call comment still
presented a pre-repair `__thiscall` disassembly as current. The comment now dates
that observation before the 2026-09-10 `__cdecl` repair and explains the remaining
reason for qualification: it selects this class's int completion-result body
even for the separate color object whose installed slot returns void. This is
a comment-only successor; the source statements and native test inputs are
unchanged. The reviewer additionally compiled all seven current virtual calls
with poisoned ECX and balanced-stack checks, and checked all 673 byte addresses
around the wipe array. The producer reran its three native tests after this
correction. The earlier full-battery result remains pinned to db3b5efb; its
level-22 failure is being repaired in a separate owned Chuckya task.
