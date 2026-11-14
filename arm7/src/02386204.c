/* storage 0x02386204 runtime 0x037FE09C linked 0x5C wram ARM.
 * Store value in track byte 1. When it is nonzero, refresh all 16 channels.
 * Pool: unk_3807B04. */

extern unsigned char unk_3807B04[];
extern char *fn_023859CC(char *entry, int channel);
extern void fn_02385AB0(char *channel, char *entry, int arg);

void fn_02386204(int index, int value) {
    unsigned char *entry = unk_3807B04 + 36 * index;
    entry[1] = value;
    if (value != 0) {
        int channel;
        for (channel = 0; channel < 16; channel++) {
            char *p = fn_023859CC((char *)entry, channel);
            if (p != 0)
                fn_02385AB0(p, (char *)entry, 0x7F);
        }
    }
}
