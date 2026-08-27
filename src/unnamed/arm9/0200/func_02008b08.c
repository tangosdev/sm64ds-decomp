/* func_02008b08 @ 0x02008b08, size 0x44, ARM.
 * Camera member function: reads a per-mode target value from a global s16 table
 * (indexed by the global current-mode byte, stride 0x18, first field s16) and
 * eases the camera's s16 field at 0x19c toward that target via _Z15ApproachLinear2Rsss.
 * The step rate is 0xa0 when the target is nonzero, else 0x400.
 */

typedef short s16;

/* Per-mode camera setting entry; 0x18-byte stride, first field is the s16 read. */
struct CameraModeEntry {
    s16 field_0x0;
    char pad[0x16];
};

extern unsigned char data_020a0e40;                  /* 0x020a0e40 : current camera mode (u8) */
extern struct CameraModeEntry data_0209f4a0[]; /* 0x0209f4a0 : table, stride 0x18 */
extern int _Z15ApproachLinear2Rsss(s16 *counter, s16 dest, s16 step);

struct Camera {
    char pad0[0x19c];
    s16 field_0x19c; /* 0x19c : s16 eased value */
};

int func_02008b08(struct Camera *cam) {
    s16 dest = data_0209f4a0[data_020a0e40].field_0x0;
    return _Z15ApproachLinear2Rsss(&cam->field_0x19c, dest, dest != 0 ? 0xa0 : 0x400);
}