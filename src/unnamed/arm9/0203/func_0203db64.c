#include "types.h"
#include "nitro/os/thread.h"
#include "private/player_input_record.h"
extern void CpuCopy8(void *dst, void *src, int n);
extern void func_0205a588(void *dst, int c, int n);
extern void func_02042778(void);
extern void func_0203df40(void);
extern int func_02040a94(void);
extern void func_02040724(void);
extern void func_0203d854(void);
extern int func_0203d81c(void);
extern void func_0203d918(void);
extern int func_0203d8fc(void);
extern void OS_GetOwnerInfo(void *dst);
extern void OS_GetMacAddress(void *p);

/* A player's 0x16-byte profile as the link exchanges it (func_0203ea5c sends
   them a chunk at a time): the owner's favourite colour and nickname.
   data_020a10fc and data_020a10a4 are each a table of four (0x58 bytes,
   cleared below); func_0203d8a0 hands out player i's from data_020a10fc in
   link mode 3 and from data_020a10a4 otherwise. Off a download-play child
   this fills in entry 0 of both from the owner settings; data_020a10fe and
   data_020a10a6 are those entries' nickName fields. */
typedef struct {
    u8 favoriteColor : 4;
    u8 hi : 4;
    u8 nickNameLength;
} Profile;

extern u8 data_020a0ef0;
extern u8 data_020a1064[];
extern u8 data_020a0fec[];
extern u8 data_020a0fb8[];
extern u8 data_020a0fa0[];
extern Profile data_020a10fc;
extern Profile data_020a10a4;
extern u8 data_020a0f08;
extern u16 data_020a0f1c;
extern u16 data_020a0f30;
extern u16 data_020a0f28;
extern u8 data_020a0f10;
extern u8 data_02099e18;
extern u8 data_020a0f04;
extern u8 data_020a10fe[];
extern u8 data_020a10a6[];
extern u8 data_020a0f00;

/* The owner settings OS_GetOwnerInfo copies out of the firmware's user info at
   0x27ffc80: NitroSDK's OSOwnerInfo, which OS_GetOwnerInfo fills in (the
   language there is the one func_0201fec8 switches on). This is the older
   SDK's 0x50-byte layout: nickName and comment have no terminator slot. */
typedef struct {
    u8 language;        /* 0x00 */
    u8 favoriteColor;   /* 0x01 */
    u8 birthday[2];     /* 0x02 -- month, day */
    u16 nickName[10];   /* 0x04 */
    u16 nickNameLength; /* 0x18 */
    u16 comment[26];    /* 0x1a */
    u16 commentLength;  /* 0x4e */
} OwnerInfo;

/* Start the link (func_0201fec8 passes per-language data): keep the 0x40 and
   0x1c bytes it is given, clear the input records, profiles and link state.
   A download-play child (the boot type at 0x27ffc40 is 2, NitroSDK's
   MB_IsMultiBootChild; see OSi_ReloadRomData.cpp) takes mode 2 and runs the
   link step (func_0203df40) until each of four handshake waits is met, 0x4b0
   frames for the first and 0x258 for the rest; a wait that runs out flags
   data_020a0f1c bit 0, shuts the link down and parks the thread. Otherwise
   this console fills in its own profile from the owner settings. */
void func_0203db64(void *arg0, void *arg1) {
    OwnerInfo buf;
    int b;

    data_020a0ef0 = 0;
    CpuCopy8(arg0, data_020a1064, 0x40);
    CpuCopy8(arg1, data_020a0fec, 0x1c);
    func_0205a588(&data_020a1040, 0, 0x24);
    func_0205a588(data_020a0fb8, 0, 0x18);
    func_0205a588(data_020a0fa0, 0, 0x18);
    func_0205a588(&data_020a10fc, 0, 0x58);
    func_0205a588(&data_020a10a4, 0, 0x58);
    func_0205a588(data_020a1154, 0, 0x90);
    data_020a0f08 = 0;
    data_020a0f1c = 0;
    data_020a0f30 = 0;
    data_020a0f28 = 0;
    b = 1;
    data_02099e18 = 1;
    data_020a0f10 = 0;
    data_020a0f04 = 0;
    if (*(u16 *)0x27ffc40 != 2) {
        b = 0;
    }
    if (b) {
        int count;

        data_020a0f04 = 2;
        count = 0x4b0;
        while (!(func_02040a94() == 2 && data_02099e18 == data_020a0f08 && data_020a0f08 != 0) &&
               count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        count = 0x258;
        data_020a1040.flags |= 1;
        while ((data_020a1040.flags & 1) && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        func_0203d854();
        count = 0x258;
        while (func_0203d81c() == 0 && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        func_0203d918();
        count = 0x258;
        while (func_0203d8fc() == 0 && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }
    } else {
        OS_GetOwnerInfo(&buf);
        func_0205a588(&data_020a10fc, 0, 0x16);
        data_020a10fc.favoriteColor = buf.favoriteColor;
        data_020a10fc.nickNameLength = buf.nickNameLength;
        data_020a10a4.favoriteColor = data_020a10fc.favoriteColor;
        data_020a10fc.hi = 0;
        data_020a10a4.hi = data_020a10fc.hi;
        data_020a10a4.nickNameLength = data_020a10fc.nickNameLength;
        CpuCopy8(buf.nickName, data_020a10fe, buf.nickNameLength * 2);
        CpuCopy8(buf.nickName, data_020a10a6, buf.nickNameLength * 2);
        data_020a0f00 = buf.language;
        OS_GetMacAddress(data_020a0fb8);
        OS_GetMacAddress(data_020a0fa0);
    }
    data_020a0ef0 = 1;
}
