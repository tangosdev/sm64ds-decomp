/* storage 0x02386170 runtime 0x037FE008 linked 0x94 wram ARM.
 * Per-track mute: for each set mask bit, fetch the channel entry, refresh it
 * when the mode is nonzero, then write bit 2 of its flags byte.
 * Pool: unk_3807B04. */

extern unsigned char unk_3807B04[];
extern char *fn_023859CC(char *entry, int channel); /* local extern: callee owned by another job, no header */
extern void fn_02385AB0(char *channel, char *entry, int arg); /* local extern: callee owned by another job, no header */

typedef struct {
    unsigned char active : 1;
    unsigned char : 1;
    unsigned char mute : 1;
    unsigned char : 5;
} TrackFlags;

void fn_02386170(int player, unsigned int trackMask, int muteMode) {
    unsigned char *ply = unk_3807B04 + 36 * player;
    TrackFlags *trk;
    int i;
    for (i = 0; i < 16 && trackMask != 0; i++, trackMask >>= 1) {
        if (trackMask & 1) {
            trk = (TrackFlags *)fn_023859CC((char *)ply, i);
            if (trk != 0) {
                if (muteMode != 0)
                    fn_02385AB0((char *)trk, (char *)ply, 0x7F);
                trk->mute = muteMode;
            }
        }
    }
}
