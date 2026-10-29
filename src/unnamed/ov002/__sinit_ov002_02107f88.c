/* Static initializer for daSoundObj_c's starter table data_ov002_0211110c,
 * which Behavior indexes by spawn param and calls through as a
 * pointer-to-member. Each entry is copied from an 8-byte {function, 0}
 * constant in .data:
 *
 *   0 Sound::PlaySecretSound       (0x0210c040)
 *   1 Sound::PlaySmallSecretSound  (0x0210c010)
 *   2 func_0200f7f0                (0x0210c020)
 *   3 func_ov002_020f9468          (0x0210c038)
 *   4 func_ov002_020f93a8          (0x0210c030)
 *   5 func_ov002_020f9468          (0x0210c018)
 *   6 func_ov002_020f9468          (0x0210c028)
 *
 * Not folded into d_a_sound_obj.cpp, its owner: the first three targets
 * are arm9 Sound:: free functions, not members, so no pointer-to-member
 * type can name the table for mwcc to emit it. */

/* A non-virtual pointer-to-member: the function and a zero this-adjust. */
struct SoundObjStarter {
    void *func;
    int thisAdjust;
};

extern struct SoundObjStarter data_ov002_0210c040;
extern struct SoundObjStarter data_ov002_0210c010;
extern struct SoundObjStarter data_ov002_0210c020;
extern struct SoundObjStarter data_ov002_0210c038;
extern struct SoundObjStarter data_ov002_0210c030;
extern struct SoundObjStarter data_ov002_0210c018;
extern struct SoundObjStarter data_ov002_0210c028;
extern struct SoundObjStarter data_ov002_0211110c[7];

void __sinit_ov002_02107f88(void) {
    data_ov002_0211110c[0] = data_ov002_0210c040;
    data_ov002_0211110c[1] = data_ov002_0210c010;
    data_ov002_0211110c[2] = data_ov002_0210c020;
    data_ov002_0211110c[3] = data_ov002_0210c038;
    data_ov002_0211110c[4] = data_ov002_0210c030;
    data_ov002_0211110c[5] = data_ov002_0210c018;
    data_ov002_0211110c[6] = data_ov002_0210c028;
}
