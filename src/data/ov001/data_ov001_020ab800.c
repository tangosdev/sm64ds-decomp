/*
 * OAM sprite templates and their index tables for the title screen.
 *
 * Every table here is a static member of OAM (OamAttr.h): a single sprite is one
 * OamAttr whose address the callers hand to OAM::Render, a list runs until the
 * 0xffff terminator in attr3, and the entries between are arrays of OamAttr
 * pointers indexed by character, digit or controller mode. MM_* are the minimap
 * cap icons, LIFE_ICONS the health pips, NUMBERS the timer/coin digits, and
 * CONTROLLER_MODE_TEXTS the three configuration diagrams. The identifier of each
 * object is the ROM's mangled static-member name, because OAM.h declares only
 * part of the set and include/ is out of scope for this pass.
 */
#include "types.h"
#include "OamAttr.h"

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
struct OamAttr _ZN3OAM13MM_MARIO_ICONE = { 0x00f0, 0x81f0, 0xa180, 0xffff };
#pragma section obj end
