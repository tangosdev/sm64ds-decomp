/* RETIRED HOST COPY of dScMgSmartball_c vtable slot 18,
 * _ZN16dScMgSmartball_c13OnYoshiTryEatEi (ov006 0x02118a8c). Run mg5 lane SMB
 * wrote it; run linkfull wave 27 lane P1 retired it. This file now defines
 * nothing: it stays an empty translation unit because port/CMakeLists.txt's
 * SCENE_SMB_SOURCES list, which enrolls it, has another owner.
 *
 * WHY IT EXISTED. The matched source declared func_ov006_02115b0c with no
 * parameters and called it with none, while ARM rides the object through r0
 * into it (0x02118a94 `bl 0x2115b0c` with nothing writing r0 first, and the
 * callee's first instruction is `mov r8, r0`). Under MSVC that call would run
 * the board teardown's seven destructor loops off an uninitialised pointer, so
 * this file carried the src body with the object passed.
 *
 * WHY IT IS GONE. src/_ZN16dScMgSmartball_c13OnYoshiTryEatEi.cpp is now the
 * member dScMgSmartball_c::OnYoshiTryEat(int), an override of dScMgBase_c's
 * slot 18, and its first statement is func_ov006_02115b0c((char *)this): the
 * declaration takes the object and the call passes it, which is everything
 * this copy changed. The two G2x::SetBlendAlpha calls are the same five
 * arguments and the same two register addresses. The TU is 2004/b56 strict
 * MATCHING and linkcheck VERIFIED blind 0 (runs/linkfull/out/P1/), and it is
 * on port/slice_w28_p1.txt.
 *
 * The flat name the scene layer's slot-18 thunk calls (hal/scene_mg.cpp's
 * smb_reset) is now a REVERSE face in port/faces_sync.txt onto
 * ?OnYoshiTryEat@dScMgSmartball_c@@UAEXH@Z, the shape the class's six other
 * seated overrides already use; the row used to run the other way (a forward
 * face defining the member over this file's flat body).
 */
