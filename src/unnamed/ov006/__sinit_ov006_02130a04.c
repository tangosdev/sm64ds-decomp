/* ov006 static initializer at 0x02130a04: an empty body (bx lr) with a
 * .ctor entry at 0x02133598, between d_s_mg_curling2.cpp's and
 * d_mg3d_esp_anim_set.cpp's.
 *
 * Not folded: its owner is d_s_mg_d3d_base.cpp. mwcc emits exactly this for a
 * non-array global whose class has an empty inline constructor, but that TU's
 * .bss (0x02141a40..0x02141e54) holds only five pointers and the 0x400 OAM
 * shadow, and nothing shows which of them, if any, was such an object.
 */
void __sinit_ov006_02130a04(void) {}
