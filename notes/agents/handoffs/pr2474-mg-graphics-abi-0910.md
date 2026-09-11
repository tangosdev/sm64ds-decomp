# PR #2474: minigame graphics forwarder ABI

PORT-MG-GRAPHICS-01 was exposed by the full native battery at scene 361,
SCENE_MG_CUP. The actual func_ov004_020ae03c forwarder called scene slot 25
with a cdecl stack receiver. Its installed mb_v25 face expected ECX instead;
the debugger observed ECX=3 and the real func_ov004_020ae128 then read 3+0x4628.
The graphics block's outer cdecl entry was already correct.

The four sibling forwarders now use hostgen's existing VIRTUAL_CALL adaptation
for their inner scene calls. They keep slots 25, 24, 22 and 23, respectively,
the original null path and return contract. CMake routes only those four source
inputs through the generator. Their complete cartridge sources are pinned before
any rewrite, just like the independently accepted Chuckya input. The matched
sources, installed tables and class headers are unchanged.

Four tests pass in `python port/tools/test_mg_graphics_abi.py -v`. The native
fixture compiles all four complete forwarders, the actual mb_v25 face and its
real func_ov004_020ae128 callee, plus the real outer func_02019100 dispatcher.
It checks 128 calls and null paths across two alternating receiver objects,
32 outer dispatches, 64 real slot-25 calls, exact slot/return behavior and stack
balance. Other scene callbacks are controlled fixtures. The raw-source control
fails on the same cases; the original slot-24 build happened to retain ECX,
while slots 25/22/23 did not. Inactive/comment decoys and changed source refuse
before output creation; LF and CRLF sources emit identical results. The accepted
Chuckya suite also passes all five tests after this shared-generator change.

The accepted A checker at a242aa57 reports 9,953 references and no failures.
Its graph gains exactly four generated inputs and four requests: 642 selected
sources, 224 requests and the same 12,157 named data owners. Fresh Ninja queries
for walk_window, walk_window_hires and smoke_player prove all four generated
objects are linked once per target, their functions are defined, and the raw
objects are excluded. All named owners remain in the 85 actual data objects;
those objects are byte-identical to their pre-build snapshots. This separates
bounded static inference from the complete actual native build.

The new full battery passes the build, all 20 smoke programs, all 51 mounted
level rows and 18 scene rows: 0, 1, 4, 5, 6, 8 and 360 through 371. Both inherited
class skips remain explicit: TTC_MOVING_BEAM at level 27 and GOOMBOSS at level 45,
each because its required cartridge body is unmatched. The earlier Chuckya
battery also had both skips; its shorter handoff named only Goomboss. Scene 372,
SCENE_MG_JUMP, then fails in ModelComponents::Render with access violation
0xc0000005. That separate failure is retained. Remaining scenes, default boot,
shipping runtime and multiplayer/rewind acceptance are not claimed here.

Evidence in the producer worktree: build/mg-graphics-tests.log,
build/mg-graphics-chuckya-regression.log, build/mg-forwarder-probe/results.json,
build/scene361-debug/result.json, build/mg-graphics-port-refcheck.json,
build/mg-graphics-graph-delta.json, build/mg-graphics-native-input-proof.json and
build/mg-graphics-full-battery.log. The prior scene-361 failure remains in
build/chuckya-full-battery.log at a3c1e551; its accepted guard successor is
2862747c. This task inherits that accepted input and retains PORT2474-01/02 and
PORT-CHUCKYA-01/02. Source base b99310a8 remains distinct from actual PR target
31f31604; target composition and public publication are separate work.
