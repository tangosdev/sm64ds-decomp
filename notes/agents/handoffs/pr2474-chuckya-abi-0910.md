# PR #2474: Chuckya host model-call convention

PORT-CHUCKYA-01 is the next failure exposed by the full native battery. At
level 22, the complete `Chuckya::Behavior` source called ModelAnim slot 3 with
a cdecl stack receiver. The host slot expects its object in ECX. The debugger
measured ECX as 0x00020008 at the real ModelAnim update; the later access through
0x3f3f3f3f was a consequence of that invalid receiver.

The existing hostgen VIRTUAL_CALL mechanism now adapts that one expression to
the port's fastcall receiver/dummy-EDX convention. The slot, object, surrounding
Behavior control flow and cartridge source remain unchanged. Generation refuses
a missing, altered or repeated original expression. CMake replaces only this
raw PMFB7 input with its generated host copy for walk_window, walk_window_hires
and smoke_player.

`python port/tools/test_chuckya_model_abi.py -v` passes three native x86 tests.
They compile the complete actual Behavior and ModelAnim::UpdateVerts source.
The selected Behavior path checks receiver, ModelComponents address, animation
file/frame, vertex call, return and stack balance; its invincibility early return
still makes no model call. Other game callees are controlled fixtures. Compiling
the unchanged raw Behavior fails the receiver/control expectations, and three
source-drift cases refuse before producing an output directory. This is focused
host ABI proof, not proof of all game branches or ROM arithmetic.

The accepted port-owner checker at a242aa57 passes 9,949 references on this
cumulative tree. Its selected graph gains only the Chuckya generated input and
request: 638 selected sources, 220 generator requests, with all 12,157 named data
owners retained. After the actual native build, Ninja's direct linker inputs
for all three executables contain exactly the generated Chuckya object and no
raw Chuckya object. All named owners occur in the 85 actual data objects, whose
bytes are unchanged from the pre-build snapshot. Static inference still excludes
unsupported dynamic selections; the native build separately compiles the actual
graph.

The complete battery passed its native build, all 20 smoke programs and all 51
mounted level rows. Level 45 retains the battery's explicit Goomboss skip because
its required cartridge body is unmatched. Scenes 0, 1, 4, 5, 6, 8 and 360 passed;
scene 361 (SCENE_MG_CUP) then failed with access violation 0xc0000005. That new
failure is not waived. Later scenes, default boot, shipping runtime and the
multiplayer/rewind proofs were not reached by this run.

Evidence in the producer worktree: build/chuckya-actual-tests.log,
build/chuckya-abi-probe/results.json, build/chuckya-port-refcheck.json,
build/chuckya-graph-delta.json, build/chuckya-native-input-proof.json and
build/chuckya-full-battery.log. The source base is b99310a8; the actual PR target
is 31f31604. Final target composition and public publication remain separate.
Inherited PORT2474-01/02 retain their accepted dispositions. The fader comment
successor ec9908da is included; its independent acceptance remains separate.

Independent review found **PORT-CHUCKYA-02**: counting an exact raw-call string
also counted comments and inactive preprocessor branches. The generator now
pins the complete reviewed input before any rewrite, with only line-ending
normalization. It refuses changed or missing active calls even when an old call
is preserved as inactive text. Unrecognized harmless context changes also
require review; this bounded gate does not model the C++ preprocessor. The
single-expression count remains an additional check after the input pin.
Five tests cover the three independent inactive-decoy fixtures, previous drift
cases, an unreviewed context edit and LF/CRLF/CR input equivalence, alongside the
native positive and old-ABI negative controls. The generated Chuckya source is
byte-identical to a3c1e551's output; its full-battery failure at scene 361 remains
pinned to that earlier input and is being diagnosed separately.
