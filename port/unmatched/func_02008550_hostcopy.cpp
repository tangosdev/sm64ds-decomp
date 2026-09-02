/* HOST COPY of src/func_02008550.cpp -- the camera half of the kuppa script's
 * command dispatch, with the pointer-to-member call replaced by a plain one.
 *
 * The matched source is
 *
 *     extern PMF data_0209b138[39];              // int (Obj::*)(u8*,int,int)
 *     ... data_0209b138[0] = data_02086c80; ...  // 39 of these, once
 *     return (obj->*data_0209b138[msg[6]])(msg + 7, a2, a3);
 *
 * and it cannot be taken verbatim for two reasons that stack.
 *
 * The first is the one hal/camera_states.cpp already answers for the state
 * machine in the same address run: the 39 right-hand sides are mwcc
 * pointer-to-member pairs, {code address, this-delta}, and MSVC's PMF for a
 * complete single-inheritance class is a 4-byte __thiscall address instead --
 * a different shape reached by a different calling convention, while the
 * handlers are C-linkage cdecl functions that take the camera as an ordinary
 * first argument.
 *
 * The second is what made this worth finding at all. Until now those 39
 * symbols came out of port/tools/romdata.py as the ROM's own bytes, so the
 * copy above loaded 39 DS code addresses into data_0209b138 and the dispatch
 * jumped straight into unmapped host memory. Same shape as the billboard
 * tables that killed a play session (hal/particle_vtable.cpp), found by
 * port/tools/ptr_audit.py. They are hosted with real host function pointers in
 * hal/ptr_tables.cpp now, keeping the ROM's two-word record layout, and this
 * copy reads word 0 out of each record exactly where the matched source reads
 * the pair.
 *
 * Everything else is the matched source: the same one-shot guard on bit 0 of
 * data_0209b004, the same 39 records in the same slots, the same command byte
 * at msg[6] and the same (msg + 7, beg, end) arguments.
 *
 * This replaces src/func_02008550.cpp in every target that links the camera,
 * which is the same filter list that already swaps func_0200cae4 and
 * func_0200ca50 for their host copies.
 */
extern "C" {

typedef int (*CamScriptFn)(void *cam, unsigned char *arg, int beg, int end);

extern int data_0209b004;
/* arm9 bss 0x0209b138, 0x138 bytes -- 39 records on the DS, 39 host function
   pointers here. hal/auto_bss.cpp sizes it from the config either way. */
extern CamScriptFn data_0209b138[39];

/* the 39 records, hal/ptr_tables.cpp, in this function's own slot order */
extern void *const *const port_cam_script_records[39];

// PORT_HOST_ABI: the 39 dispatch entries are mwcc pointer-to-member pairs holding DS code addresses MSVC cannot represent; read as host function pointers from the hosted records instead
int func_02008550(void *obj, unsigned char *msg, int a2, int a3)
{
    if ((data_0209b004 & 1) == 0) {
        data_0209b004 |= 1;
        for (int i = 0; i < 39; ++i)
            data_0209b138[i] = (CamScriptFn)port_cam_script_records[i][0];
    }
    return data_0209b138[msg[6]](obj, msg + 7, a2, a3);
}

}  /* extern "C" */
