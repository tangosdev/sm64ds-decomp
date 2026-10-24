/* storage 0x02383770 runtime 0x037FB608 linked 0xB8 wram ARM.
 * NitroSDK SND_SetupChannelPcm: program a playback channel. Stores the
 * original volume, applies surround decay when enabled, then writes the
 * channel's CNT/TMR/PNT/LEN/SAD registers. */

extern int fn_02383450(int volume, int pan);
extern signed char unk_38070B8[];
extern int dword_38070B4;

#define reg_SNDoff(off)     ((char *)((off) + 0x4000000))
#define reg_SOUNDoffCNT(off) (*(volatile unsigned int *)(reg_SNDoff(off) + 0x400))
#define reg_SOUNDoffSAD(off) (*(volatile unsigned int *)(reg_SNDoff(off) + 0x404))
#define reg_SOUNDoffTMR(off) (*(volatile unsigned short *)(reg_SNDoff(off) + 0x408))
#define reg_SOUNDoffPNT(off) (*(volatile unsigned short *)(reg_SNDoff(off) + 0x40A))
#define reg_SOUNDoffLEN(off) (*(volatile unsigned int *)(reg_SNDoff(off) + 0x40C))

int fn_02383770(int chnIdx, int data, int format, int loop, int loopStart,
                int loopLength, int volume, int volumeDiv, int timer, int pan) {
    unsigned int off = chnIdx * 0x10;
    unk_38070B8[chnIdx] = (signed char)volume;
    if (dword_38070B4 > 0 && ((1 << chnIdx) & 0xFFF5) != 0)
        volume = fn_02383450(volume, pan);
    reg_SOUNDoffCNT(off) =
        (format << 29) | (loop << 27) | (pan << 16) | (volumeDiv << 8) | volume;
    reg_SOUNDoffTMR(off) = (unsigned short)(0x10000 - timer);
    reg_SOUNDoffPNT(off) = (unsigned short)loopStart;
    reg_SOUNDoffLEN(off) = loopLength;
    reg_SOUNDoffSAD(off) = data;
    return loopLength;
}
