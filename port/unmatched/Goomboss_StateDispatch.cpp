/* HOST COPIES of src/func_ov074_021203e4.cpp and src/func_ov074_0212042c.cpp
 * -- run rel0215 wave 2, lane w2-ov074. Goomboss's NINE-STATE
 * pointer-to-member machine, the MrBlizzard/BabyPenguin/HootTheOwl/PushBlock/
 * Ukiki/Spiny disease one more time.
 *
 * The matched sources spell it:
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct Entry { PMF pmf[2]; };
 *     extern Entry data_ov074_021230f8[];
 *     struct C { char pad[0x5cc]; int idx; };
 *     void func_ov074_021203e4(C *c, int i)
 *     { c->idx = i; int j = c->idx; (c->*data_ov074_021230f8[j].pmf[0])(); }
 *     void func_ov074_0212042c(C *c)
 *     { int j = c->idx; (c->*data_ov074_021230f8[j].pmf[1])(); }
 *
 * mwcc's pointer-to-member is an 8-byte {fn, delta} record, so Entry is 16
 * bytes and the ROM's table is EIGHTEEN records = NINE cells of {enter, tick}.
 * MSVC's representation for a pointer-to-member of an INCOMPLETE class is the
 * generalised 16-byte one and it dispatches through a thunk, so the matched TU
 * compiled by MSVC reads the wrong words and calls through the wrong ABI. Both
 * sites are host-copied; both matched sources stay byte-locked in src/ and are
 * dropped from port/slice_ov074.txt.
 *
 * WHERE THE TABLE COMES FROM. src/__sinit_ov074_02122978.c copies eighteen
 * mounted .data records into data_ov074_021230f8[0..17]; index 2k is state k's
 * ENTER half and 2k+1 its TICK half. A mounted record's fn word is a RAW DS
 * ADDRESS -- ovdata's pointer pass only rebases pointers into other mounted
 * DATA, and these point at CODE -- so hal/actor_classes_ov074.cpp's
 * port_ov074_states_seat() rewrites all eighteen with their host bodies BEFORE
 * the sinit runs, validating each against the ROM's own word first. One of the
 * eighteen (state 0's tick, ROM 0x021201f0) has no matched body anywhere in
 * the tree and is seated with a loud face.
 *
 * THE 0x90 PIN MATTERS HERE. dsd sizes data_ov074_021230f8 0x4c -- nine and a
 * half of the eighteen records -- because three arm9 spawn-table slots that
 * belong to ov073 and ov070 cite addresses inside it. port/ov074_syms.txt pins
 * it to 0x90 and leaves those three phantom splits out. Unpinned, states 5
 * through 8 would dispatch through whatever the linker put after the array.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class.
 */

extern "C" {

struct PortOv074Pmf { unsigned int fn; int delta; };
typedef void (*PortOv074StateFn)(void *);

extern PortOv074Pmf data_ov074_021230f8[];

/* ENTER: sets the state index at +0x5cc, then dispatches cell[idx].pmf[0]. The
   ROM reads the field back after the store, which is why the matched source
   does too; kept here so a debugger sees the same sequence. */
// PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class.
void func_ov074_021203e4(void *cv, int i)
{
    char *c = (char *)cv;
    *(int *)(c + 0x5cc) = i;
    int j = *(int *)(c + 0x5cc);
    ((PortOv074StateFn)(size_t)data_ov074_021230f8[2 * j].fn)(cv);
}

/* TICK: dispatches cell[idx].pmf[1]. Called every frame from
   Goomboss::Behavior. */
// PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class.
void func_ov074_0212042c(void *cv)
{
    char *c = (char *)cv;
    int j = *(int *)(c + 0x5cc);
    ((PortOv074StateFn)(size_t)data_ov074_021230f8[2 * j + 1].fn)(cv);
}

}  /* extern "C" */
