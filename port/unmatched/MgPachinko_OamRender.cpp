// PORT_HOST_ABI. src/func_ov006_020fa7b8.cpp, verbatim except for ONE TOKEN.
// dScMgPachinko_c, actor id 0x170, scene 368. Run mg5, lane PCH.
//
// ---- THE DEFECT, AND IT IS A DEFINITION WHERE A DECLARATION WAS MEANT -----
//
// The src TU opens
//
//     extern "C" {
//     int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, ...);
//     void* data_ov006_0213ac24;
//     }
//
// The function line is a declaration. THE DATA LINE IS NOT. C has tentative
// definitions and C++ does not, so at namespace scope `void* x;` is a
// DEFINITION of a zero-initialised pointer, and `extern "C"` changes the
// LINKAGE of the name rather than making it a declaration. The file is a .cpp
// and the port compiles it as C++, so the port gets a second definition of a
// symbol the ov006 mount already defines:
//
//     ov006_syms.c.obj : error LNK2005: _data_ov006_0213ac24 already defined
//                        in func_ov006_020fa7b8.cpp.obj
//
// THE LINK ERROR IS THE LUCKY OUTCOME. build/port/host-src/ov006_syms.c:1285
// emits the cell as `u8 data_ov006_0213ac24[4] = { 40,172,19,2 }` -- the ROM
// word 0x0213ac28 -- and line 9618 REBASES it at mount time
// (`*(unsigned int *)(data_ov006_0213ac24 + 0) = (unsigned int)(data_ov006_0213ac28 + 0)`),
// so the cell holds a live host pointer to the OamAttr block the Render call
// below passes to OAM::Render. A build where the src TU's zero won instead
// would have handed OAM::Render a null attribute pointer with nothing to
// diagnose it.
//
// ONE TOKEN CHANGES: `void* data_ov006_0213ac24;` becomes
// `extern void* data_ov006_0213ac24;`. Every other character of the body is
// the src's, including the loop bound, the stride, all four offsets and all
// ten arguments. This is the treatment port/mg_fanout_costs.txt section 6a
// describes for the two decl_common.h conflicts and
// port/unmatched/MgBase_DeclConflict.cpp implements: the src TU is matched and
// read-only, so the port hosts a corrected copy and touches neither src/ nor
// include/.
//
// THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN. The src line wants `extern`, and
// whether that still builds byte-identically under mwccarm 1.2/sp2p3 is a
// byte-gated-tree question this lane does not answer. Note that the TU's own
// banner says "Matched byte-for-byte", which it is -- mwcc reads the .cpp with
// the same tentative-definition tolerance C has, and no ROM byte moves either
// way. The byte gate has never had an opinion about this, which is why it
// survived to be found by a host link.
//
// THE BODY IS LIVE ON THIS CLASS'S PATH: it is the tenth and last call of
// vtable slot 9 (func_ov006_020fedc4, dScMgPachinko_c::Render), so a boot that
// renders reaches it.

extern "C" {

int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void *, int, int, int,
                                                int, int, void *, int, int);

/* THE ONE TOKEN. The mount owns this cell and has already rebased it. */
extern void *data_ov006_0213ac24;

// PORT_HOST_ABI: src's `void* data_ov006_0213ac24;` in a .cpp is a tentative definition C++ treats as a real one, colliding with the mount (LNK2005); host copy respells it extern
void func_ov006_020fa7b8(char *thiz)
{
    for (int i = 0; i < 3; i++) {
        if (*(unsigned char *)(thiz + 0x4000 + 0xe6d) != 0) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
                false, data_ov006_0213ac24,
                *(int *)(thiz + 0x4000 + 0xe58) >> 12,
                *(int *)(thiz + 0x4000 + 0xe5c) >> 12,
                -1, -1, 0x1000, (void *)0x1000,
                *(unsigned short *)(thiz + 0x4e00 + 0x68), -1);
        }
        thiz += 0x18;
    }
}

}  /* extern "C" */
