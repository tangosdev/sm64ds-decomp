/* storage 0x02386D08 runtime 0x037FEBA0 size 0x428 + 5 pool words, wram ARM.
 * SND_CommandProc: drains sMsgQueue, walks each command's linked list and
 * dispatches on cmd.id. From NitroSDK arm7 SND_command.c (pokediamond),
 * older SM64DS revision: 30 commands, llNext-first SNDCommand, PCM/PSG/
 * NOISE at ids 11-13, CAPTURE/ALARM at 14-15, INVALIDATE_* at 27-29. */

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;
typedef short s16;
typedef int BOOL;
typedef void *OSMessage;
typedef unsigned long long OSTick;

struct SNDCommand {
    struct SNDCommand *llNext;
    u32 id;
    u32 arg[4];
};

struct OSMessageQueue { int _x[8]; };
struct SNDSharedWork { u32 finishedCommandTag; };

extern struct OSMessageQueue sMsgQueue;              /* 0x03808744 */
extern struct SNDSharedWork *SNDi_SharedWork;        /* 0x038075C0 */
extern unsigned char SNDi_Work[];                    /* 0x038075C4 */

extern int fn_0238155C(struct OSMessageQueue *, OSMessage *, s32);   /* OS_ReceiveMessage */
extern void fn_023862C0(int, const void *, u32, void *);             /* SND_StartSeq */
extern void fn_02386260(int);                                        /* SND_StopSeq */
extern void fn_02386204(int, BOOL);                                  /* SND_PauseSeq */
extern void fn_02385FC0(int, u32, u32, int);                         /* SNDi_SetPlayerParam */
extern void fn_02385F2C(int, u32, u32, u32, u8);                     /* SNDi_SetTrackParam */
extern void fn_02386170(int, u32, BOOL);                             /* SND_SetTrackMute */
extern void fn_02386100(int, u32, u32);                              /* SND_SetTrackAllocatableChannel */
extern void fn_02386814(int, int, s16);                              /* SND_SetPlayerLocalVariable */
extern void fn_023867F8(int, s16);                                   /* SND_SetPlayerGlobalVariable */
extern int fn_02386ABC(u32, u32, u32, int);                          /* StartTimer */
extern int fn_023869F8(u32, u32, u32, int);                          /* StopTimer */
extern void fn_02383770(int, void *, int, int, int, int, int, int, int, int); /* SND_SetupChannelPcm */
extern int fn_023836E0(int, int, int, int, int, int);                /* SND_SetupChannelPsg */
extern int fn_02383658(int, int, int, int, int);                     /* SND_SetupChannelNoise */
extern void fn_02383CA8(int, int, void *, int, int, int, int);       /* SND_SetupCapture */
extern int fn_02386970(s32, OSTick, OSTick, u32);                    /* SND_SetupAlarm */
extern void fn_02386C54(u32, int);                                   /* SetChannelTimer */
extern void fn_02386BFC(u32, int, int);                              /* SetChannelVolume */
extern void fn_02386BB4(u32, int);                                   /* SetChannelPan */
extern void fn_023834BC(int);                                        /* SNDi_SetSurroundDecay */
extern void fn_02383364(int);                                        /* SND_SetMasterVolume */
extern void fn_0238332C(int, int, int, int);                         /* SND_SetOutputSelector */
extern void fn_02384200(u32, u32);                                   /* SND_LockChannel */
extern void fn_023841C4(u32, u32);                                   /* SND_UnlockChannel */
extern void fn_023842F8(u32, u32);                                   /* SND_StopUnlockedChannel */
extern void fn_02382CC8(void *, void *, u32);                        /* driver-info copy */
extern void fn_02386068(void *, void *);                             /* SND_InvalidateSeq */
extern void fn_0238600C(void *, void *);                             /* SND_InvalidateBank */
extern void fn_02384148(void *, void *);                             /* SND_InvalidateWave */

void fn_02386D08(void) {
    struct SNDCommand cmd;
    struct SNDCommand *cmd_ptr;
    OSMessage msg;

    while (fn_0238155C(&sMsgQueue, &msg, 0)) {
        cmd_ptr = (struct SNDCommand *)msg;

        while (cmd_ptr) {
            cmd = *cmd_ptr;

            switch (cmd.id) {
            case 0:
                fn_023862C0((int)cmd.arg[0], (const void *)cmd.arg[1],
                            cmd.arg[2], (void *)cmd.arg[3]);
                break;
            case 1:
                fn_02386260((int)cmd.arg[0]);
                break;
            case 2:
                fn_02386204((int)cmd.arg[0], (BOOL)cmd.arg[1]);
                break;
            case 3:
                fn_02385FC0((int)cmd.arg[0], cmd.arg[1], cmd.arg[2], (int)cmd.arg[3]);
                break;
            case 4:
                fn_02385F2C((int)cmd.arg[0] & 0xFFFFFF,
                            cmd.arg[1], cmd.arg[2], cmd.arg[3],
                            (u8)(cmd.arg[0] >> 24));
                break;
            case 5:
                fn_02386170((int)cmd.arg[0], cmd.arg[1], (BOOL)cmd.arg[2]);
                break;
            case 6:
                fn_02386100((int)cmd.arg[0], cmd.arg[1], cmd.arg[2]);
                break;
            case 7:
                fn_02386814((int)cmd.arg[0], (int)cmd.arg[1], (s16)cmd.arg[2]);
                break;
            case 8:
                fn_023867F8((int)cmd.arg[0], (s16)cmd.arg[1]);
                break;
            case 9:
                fn_02386ABC(cmd.arg[0], cmd.arg[1], cmd.arg[2], (int)cmd.arg[3]);
                break;
            case 10:
                fn_023869F8(cmd.arg[0], cmd.arg[1], cmd.arg[2], (int)cmd.arg[3]);
                break;
            case 14:
                fn_02383CA8((int)(cmd.arg[2] >> 31u) & 1,
                            (int)(cmd.arg[2] >> 30u) & 1,
                            (void *)cmd.arg[0], (int)cmd.arg[1],
                            (int)(cmd.arg[2] >> 29u) & 1,
                            (int)(cmd.arg[2] >> 28u) & 1,
                            (int)(cmd.arg[2] >> 27u) & 1);
                break;
            case 15:
                fn_02386970((s32)cmd.arg[0], (OSTick)cmd.arg[1],
                            (OSTick)cmd.arg[2], (u32)cmd.arg[3]);
                break;
            case 16:
                fn_02386C54(cmd.arg[0], (int)cmd.arg[1]);
                break;
            case 17:
                fn_02386BFC(cmd.arg[0], (int)cmd.arg[1], (int)cmd.arg[2]);
                break;
            case 18:
                fn_02386BB4(cmd.arg[0], (int)cmd.arg[1]);
                break;
            case 11:
                fn_02383770((int)cmd.arg[0] & 0xFFFF,
                            (void *)(cmd.arg[1] & 0x7FFFFFFu),
                            (int)(cmd.arg[3] >> 24u) & 0x3,
                            (int)(cmd.arg[3] >> 26u) & 0x3,
                            (int)cmd.arg[3] & 0xFFFF,
                            (int)cmd.arg[2] & 0x3FFFFF,
                            (int)(cmd.arg[2] >> 24u) & 0x7F,
                            (int)(cmd.arg[2] >> 22u) & 0x3,
                            (int)(cmd.arg[0] >> 16u) & 0xFFFF,
                            (int)(cmd.arg[3] >> 16u) & 0x7F);
                break;
            case 12:
                fn_023836E0((int)cmd.arg[0], (int)cmd.arg[3],
                            (int)cmd.arg[1] & 0x7F, (int)(cmd.arg[1] >> 8u) & 0x3,
                            (int)(cmd.arg[2] >> 8u) & 0xFFFF, (int)cmd.arg[2] & 0x7F);
                break;
            case 13:
                fn_02383658((int)cmd.arg[0],
                            (int)cmd.arg[1] & 0x7F, (int)(cmd.arg[1] >> 8u) & 0x3,
                            (int)(cmd.arg[2] >> 8u) & 0xFFFF, (int)cmd.arg[2] & 0x7F);
                break;
            case 19:
                fn_023834BC((int)cmd.arg[0]);
                break;
            case 20:
                fn_02383364((int)cmd.arg[0]);
                break;
            case 21:
                fn_0238332C((int)cmd.arg[0], (int)cmd.arg[1],
                            (int)cmd.arg[2], (int)cmd.arg[3]);
                break;
            case 22:
                fn_02384200(cmd.arg[0], cmd.arg[1]);
                break;
            case 23:
                fn_023841C4(cmd.arg[0], cmd.arg[1]);
                break;
            case 24:
                fn_023842F8(cmd.arg[0], cmd.arg[1]);
                break;
            case 27:
                fn_02386068((void *)cmd.arg[0], (void *)cmd.arg[1]);
                break;
            case 28:
                fn_0238600C((void *)cmd.arg[0], (void *)cmd.arg[1]);
                break;
            case 29:
                fn_02384148((void *)cmd.arg[0], (void *)cmd.arg[1]);
                break;
            case 25:
                SNDi_SharedWork = (struct SNDSharedWork *)cmd.arg[0];
                break;
            case 26:
                fn_02382CC8(SNDi_Work, (void *)cmd.arg[0], 0x1180);
                break;
            }

            cmd_ptr = cmd.llNext;
        }

        SNDi_SharedWork->finishedCommandTag++;
    }
}
