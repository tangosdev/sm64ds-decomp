/* HOST FACE for func_0203dae4, THE COMMS SLOT INDEX, KEY HALF. Run mg12, lane
 * TITLE. Everything in the header of port/unmatched/Comms_SlotIndex_0203dabc.cpp
 * applies here word for word; only the field differs.
 *
 *     include/decl_common.h:1332     extern int func_0203dae4(void);
 *     src/func_0203dae4.c:3          int func_0203dae4(int idx)
 *
 * The one linked caller is src/func_ov007_020cc2cc.c:31-33, which calls
 * func_0203da9c() and then this with nothing, riding the index through r0.
 * src/func_0203bb60.c and src/func_0203bc7c.c, which do pass an index, are in
 * no slice.
 *
 * data_020a1162 is data_020a1154 + 0x0e, the key word of comms record 0. The
 * host hosts both as grouped sections at ROM spacing and
 * hal_camera_check_layout asserts the spacing at bring-up on BOTH the level
 * path and the scene path (port/hal/level_boot.cpp's port_a2_seat_body, which
 * port_scene_a2_seat calls), so the stride this indexes with is checked rather
 * than assumed.
 */

extern "C" {

extern int data_020a0f10[];

struct Ov007CommsKeys { unsigned short h; char _rest[0x24 - 2]; };
extern Ov007CommsKeys data_020a1162[];

int func_0203dae4(void);

int func_0203dae4(void)
{
    return data_020a1162[data_020a0f10[0]].h;
}

}
