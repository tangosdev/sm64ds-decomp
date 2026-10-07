//cpp
/* Sound -- the game's public sound API and the state block behind it.
 * include/Sound.h models the namespace; the ROM linker run
 * 0x02011654..0x02013548 carries the whole module: the 16-bucket sound-object
 * registry at data_0209b53c (find/alloc/free/sweep helpers), the voice-group
 * loaders, the banked Play wrappers, the music-layer state machine, and the
 * init and per-frame update entry points func_020133bc/func_020132d8.
 *
 * Deferred codegen emits definitions in reverse source order, so the file
 * opens with the tail helpers and closes on the registry internals.
 *
 * Sound::Play and Sound::Play2D are declared void by their definitions but
 * some callers harvest r0 -- call sites that consume a result keep the
 * extern "C" mangled spelling, which is not type-checked across the seam.
 * PlaySub and ChangeMusicVolume carry `5Fix12IiE' in their mangled names,
 * which no type in this tree produces; they keep their literal symbol names.
 */
#include "types.h"
#include "Sound.h"

extern "C" {

/* Foreign callees. */
extern int func_02051f1c(void *a, u32 b, int c);
extern int func_02051e60(int a, int b, int c, int d, int e, int f);
extern int func_02051074(int a);
extern int func_020511d4(int a, int b, int c, int d, int e);
extern int func_0203d974(void);
extern int func_020510a4(int a, int b);
extern int func_0205117c(int a);
extern int func_0203d7d4(int a);
extern int func_02048a1c(int *a0, int kind, int id);
extern int func_02048a18(int x, int y);
extern void func_02048af4(void *p, int b, int c);
extern void func_02048d80(void *obj, int *p);
extern void func_02048c30(void *a, int b, int c);
extern void func_0204f7fc(void *c, int a, int b);
extern void func_0204f82c(void *a, int b, int c);
extern void func_0204f7cc(void *a, int b, int c);
extern void func_020485f0(int a0, int a1);
extern void func_02048528(int a0, int a1);
extern void func_0204f9c4(int a, int b);
extern void func_0204f958(int a, int b);
extern void func_0204fa2c(int *g, int z);
extern void func_0204f94c(void *p);
extern void func_0204f8cc(int a, int b, int c);
extern void func_0204f070(void);
extern void *func_0205130c(u32 addr, u32 size);
extern void func_0201a9fc(char *c);
extern void func_02050f34(void *self, int a1, int a2, int a3);
extern int func_02052008(int arg);
extern void func_020506fc(int arg);
extern void func_02049cd8(void *a, int b, int mode);
extern void func_02048f34(void *owner);
extern int func_0201a9ec(int *c);
extern int func_02051918(unsigned int id, int arg);
extern void func_0201a96c(int *c, int s);
extern int func_02051fb4(void *g, unsigned int x);
extern void func_02049764(void);
extern void func_0204921c(int *g, int v);
extern void func_020494cc(void *p, int s);
extern void func_020490b0(void *p);
extern void func_0204f03c(void);
extern void *func_02050cdc(int a, int idx);
extern int func_02048720(const Vector3 *v, unsigned int a, unsigned int b);
extern void func_02048908(int x, const Vector3 *v);
extern void func_02048e54(void);
extern void func_02048e74(void);
extern void func_02048e94(void);
extern void func_02049c48(int self);
extern void func_02048ed4(void);
extern void func_02048ef4(void);
extern void func_02048f04(void);
extern void func_02048f14(void);
extern void func_02048f24(void);
extern void func_0204efe0(void);
extern void func_0204effc(void);
extern void *_ZN6Memory8AllocateEj(u32 size);
extern void _ZN5Sound13Func_02048eb4Ev(void);
extern void _ZN5Sound13Func_02048ec4Ev(void);
extern void _ZN5Sound13Func_02048ee4Ev(void);
/* Callers that consume r0 from the void-declared Sound::Play2D keep the
 * mangled spelling; the extern "C" name is not type-checked across the seam.
 * Members reach the same symbol by casting Play2D/Play's address instead. */
extern unsigned int _ZN5Sound6Play2DEjj(unsigned int a, unsigned int b);

/* In-TU functions, declared once so callers ahead of their definitions link. */
extern void func_02013524(int fileRef, int vol, int mode);
extern int func_020134d8(void *a, void *b);
extern void func_020133bc(void);
extern int func_02013078(void);
extern void func_02012d64(void);
extern void func_02012860(void *a, int flags, int c, int d, int e);
extern void func_020127ec(int a0, int a1, int a2, int a3, int a4, int a5);
extern int func_020126e8(int a);
extern void func_020123c8(int a0, int type, int code, int arg);
extern int func_02011dcc(int *g, int x);
extern void func_02011c54(void);
extern void func_02011b7c(void);
extern void func_02011b38(void *c, int a1, int a2, int a3, short a5);
extern void func_02011af8(void *c, int a1, int a2, int a3, int s0, short s1);
extern void func_02011ac4(void *c, int a1, int a2, int a3, int s0, int s1, short s2);
extern int func_02011aa0(void *c);
extern void func_02011a28(char *c);
extern void func_020119c8(void *base);
extern void *func_02011934(char *table, int id);
extern int func_0201186c(char *thiz, int a1, int a2, short a3);
extern int func_0201179c(char *thiz, int a1, int a2, int a3, short s1);
extern int func_020116c4(char *c, int a1, int a2, int a3, int s0, short s1);
extern void func_02011698(char *table, char *n);
extern int func_02011654(char *table, char *n);
extern int Player_PlaySoundEffect(int arg0, int arg1, int arg2);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);

/* Module state -- all .bss, referenced here, defined by the link. */
extern signed char data_0208e420;
extern signed char data_0208e424;
extern signed char data_0208e428;
extern signed char data_0208e42c;
extern int data_0208e430;
extern int data_0208e434;
extern int data_0208e438;
extern int data_0208e43c;
extern int data_0208e440;
extern int data_0208e444;
extern signed char data_0209b470;
extern unsigned char data_0209b474;
extern unsigned char data_0209b478;
extern unsigned char data_0209b47c;
extern unsigned char data_0209b480;
extern int data_0209b484;
extern int data_0209b488;
extern int data_0209b48c;
extern int data_0209b490;
extern int data_0209b494;
extern int data_0209b498;
extern int data_0209b49c;
extern int data_0209b4a0[];
extern int data_0209b4a4[];
extern int data_0209b4a8;
extern int data_0209b4ac;
extern int data_0209b4b0[];
extern char data_0209b4b4[];
extern char data_0209b53c[];
extern int data_0209baa0;
extern char data_0209d574[];
extern unsigned char data_0209f250;
extern signed char data_0209f2f8;
extern void *data_0209f394[];
extern signed char data_02092120;
extern unsigned char data_02075250[];
extern unsigned char data_02075258[];
extern char data_0208e498[];

}

/* ApproachLinear(int&, int, int) is _Z14ApproachLinearRiii -- a C++ symbol,
 * so it is declared as one rather than by its mangled name. */
void ApproachLinear(int &val, int target, int step);

typedef struct {
    int playerID;
    int maxSeq;
} SoundPlayerDefault;

extern "C" SoundPlayerDefault data_0208e448[10]; /* 0x0208e448 */

/* The sound-object registry: a counter/cursor header, a fixed 0x40-entry pool
 * of 0x14-byte nodes at +0x8, and 16 hash-bucket heads at +0x508 chaining the
 * live entries by id&0xf. */
typedef struct SndRegEntry {
    int field_0;                 /* unique id; 0 marks a free slot */
    unsigned char pad4[2];
    unsigned char field_6;       /* in-use flag checked by the sweeps */
    unsigned char pad7[0xd];
} SndRegEntry;

typedef struct SndRegTable {
    int counter;                 /* 0x00 - handed out as the unique id */
    unsigned char field_4;       /* 0x04 - round-robin cursor into entries */
    unsigned char pad5[3];
    SndRegEntry entries[0x40];   /* 0x08 */
    void *buckets[16];           /* 0x508 - chain heads by id&0xf */
} SndRegTable;

namespace Sound {

// @symbol _ZN5Sound22LoadAndSetMusic_Layer1Ei
void LoadAndSetMusic_Layer1(int j)
{
    data_0208e438 = -1;
    data_0208e434 = -1;
    data_0209b48c = 0;
    if (j < 0) {
        func_0204fa2c(data_0209b4a0, 0);
    } else {
        if (!(data_0208e430 >= 0 && data_0209b4ac >= 0 && data_0208e43c == j))
            func_02011dcc(data_0209b4a0, j);
        func_02013524((int)data_0209b4a0, 0x7f, 0);
        func_0204fa2c(data_0209b4b0, 0);
    }
    data_0209b4ac = j;
    data_0208e43c = j;
    func_02011b7c();
}

}

// @symbol func_02013078
extern "C" int func_02013078(void)
{
    char *obj = (char *)data_0209f394[data_0209f250];
    int state;
    int flag;
    int result;

    if (obj == 0)
        return -1;
    flag = (*(unsigned char *)(obj + 0x706) != 0);
    state = data_0209f2f8;
    result = -1;
    if (state == 8) {
        result = data_0208e434;
        if (result < 0) {
            if (flag)
                result = 0;
            else
                result = 1;
        }
    } else if (state == 9) {
        result = flag ? 1 : 2;
    } else if (state == 0x12) {
        if (data_02092120 == 0)
            result = 1;
        else
            result = flag ? 0 : 2;
    } else if (state == 0xc) {
        if (data_02092120 == 0)
            data_0209b48c = Sound_PlayIfNotActive(data_0209b48c, 3, 0x18d, 0);
        result = (data_02092120 == 0) ? 4 : 3;
    } else if (state == 0xd) {
        result = (*(int *)(obj + 0x60) < (int)0xffe3e000) ? 5 : 6;
    } else if (state == 0x15) {
        result = (*(int *)(obj + 0x60) < 0) ? 5 : 6;
    }
    if (result >= 0 && result != data_0208e438) {
        data_0208e438 = result;
        return result;
    }
    return -1;
}

namespace Sound {

// @symbol _ZN5Sound22StopLoadedMusic_Layer1Ej
void StopLoadedMusic_Layer1(unsigned int arg)
{
    data_0209b4ac = ~0u;
    func_0204fa2c(data_0209b4a0, arg);
}

// @symbol _ZN5Sound10PauseMusicEv
void PauseMusic(void)
{
    int state = data_0209f2f8;
    if (state == 2 || state == 5 || state == 4 || state == 0x32) {
        if (data_0208e420 < 0) {
            data_0208e420 = data_0208e42c;
            data_0208e424 = data_0209b470;
            data_0208e42c = 0x40;
            data_0209b470 = 0x40;
            data_0209b494 = 0xc999;
        }
    } else {
        func_0204f958(0, 1);
        func_0204f958(1, 1);
    }
    func_02011c54();
    func_0204f9c4(0xd, 1);
}

// @symbol _ZN5Sound12UnpauseMusicEv
void UnpauseMusic(void)
{
    signed char s = data_0209f2f8;
    if (s == 2 || s == 5 || s == 4 || s == 0x32) {
        signed char lr = data_0208e420;
        if (lr < 0) return;
        data_0208e42c = lr;
        signed char ip = data_0208e424;
        data_0209b470 = ip;
        data_0208e420 = -1;
        return;
    }
    func_0204f958(0, 0);
    func_0204f958(1, 0);
}

}

// @symbol func_02012e78
extern "C" void func_02012e78(void)
{
    if (data_0208e420 >= 0) return;
    data_0208e420 = data_0208e42c;
    data_0208e424 = data_0209b470;
    data_0208e42c = 0x40;
    data_0209b470 = 0x40;
    data_0209b494 = 0xc999;
}

// @symbol func_02012e1c
extern "C" void func_02012e1c(void)
{
    signed char v = data_0208e420;
    if (v < 0) return;
    data_0208e42c = v;
    data_0209b470 = data_0208e424;
    data_0208e420 = -1;
}

// @symbol func_02012dd0
extern "C" void func_02012dd0(void *c)
{
    func_0204f9c4(0, (int)c);
    func_0204f9c4(1, (int)c);
    func_0204f9c4(0xe, (int)c);
    func_0204f9c4(0x15, (int)c);
    func_0204f9c4(0x1f, (int)c);
}

// @symbol func_02012dbc
extern "C" void func_02012dbc(int a)
{
    func_0204f9c4(0xe, a);
}

// @symbol func_02012d64
extern "C" void func_02012d64(void)
{
    ApproachLinear(data_0209b490,
                   (s32)data_0208e42c << 12,
                   data_0209b494);
    func_02013524((int)data_0209b4a0,
                  data_0209b490 >> 12,
                  0);
}

// @symbol _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE
extern "C" int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b)
{
    data_0208e42c = a;
    data_0209b494 = b;
    return data_0209b490 == (data_0208e42c << 12);
}

// @symbol _ZN5Sound7PlaySubEjjj5Fix12IiEb
extern "C" bool _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e)
{
    int x;
    if (c == 0) {
        if (e) goto end;
        x = data_0208e430;
        if (x >= 0 && x != (int)a) goto ret1;
        if (data_0209b49c != 0) goto end;
        if (data_0208e42c != (int)b) goto end;
        if (data_0209b490 != (int)(b << 12)) goto end;
ret1:
        return true;
    }
    x = data_0208e430;
    if (x >= 0) {
        if (!e) goto ld4;
    }
    if (x == (int)a) goto end;
    data_0208e430 = a;
    if (a != 0x35 && a != 0x36) {
        if (a != 0x2e) data_0209b49c = c & 0xff;
    }
    func_02011dcc(data_0209b4b0, a);
    goto end;
ld4:
    if (x != (int)a) return false;
end:
    data_0208e42c = (signed char)b;
    data_0209b470 = (signed char)c;
    data_0209b494 = d;
    if (data_0209b490 != (int)(b << 12)) goto ret0;
    if (data_0209b49c == (int)(c << 12)) return true;
ret0:
    return false;
}

namespace Sound {

// @symbol _ZN5Sound22LoadAndSetMusic_Layer3Ej
void LoadAndSetMusic_Layer3(unsigned int musicId)
{
    data_0208e444 = musicId;
    func_02011dcc(data_0209b4a0, musicId);
}

// @symbol _ZN5Sound22StopLoadedMusic_Layer3Ev
void StopLoadedMusic_Layer3(void)
{
    int a = data_0209b4ac;
    if (a < 0) {
        if (data_0208e444 < 0) return;
    }
    data_0208e444 = -1;
    int b = data_0208e43c;
    if (a == b) {
        int e440 = data_0208e440;
        if (e440 >= 0) { func_02011dcc(data_0209b4a0, e440); return; }
    }
    if (a < 0 || b < 0)
        func_0204fa2c(data_0209b4a0, 0);
    else
        func_02011dcc(data_0209b4a0, a);
}

// @symbol _ZN5Sound8SetMusicEjj
void SetMusic(unsigned int j1, unsigned int j2)
{
    if (j1 != data_0209f250) return;
    if (data_0209b4ac < 0) {
        if (data_0208e43c >= 0) return;
    }
    int e = data_0208e444;
    data_0209b4ac = j2;
    if (e >= 0) return;
    func_02011dcc(data_0209b4a0, j2);
}

// @symbol _ZN5Sound8EndMusicEjj
void EndMusic(unsigned int j1, unsigned int j2)
{
    if (data_0209b4ac < 0) return;
    if (j1 != data_0209f250) return;
    int e43c = data_0208e43c;
    int e444 = data_0208e444;
    data_0209b4ac = e43c;
    if (e444 >= 0) return;
    int e440 = data_0208e440;
    if (e440 >= 0) { func_02011dcc(data_0209b4a0, e440); return; }
    if (e43c >= 0) { func_02011dcc(data_0209b4a0, e43c); return; }
    func_0204fa2c(data_0209b4a0, 0);
}

// @symbol _ZN5Sound22LoadAndSetMusic_Layer2Ej
void LoadAndSetMusic_Layer2(unsigned int j)
{
    int a = data_0209b4ac;
    int b = data_0208e43c;
    data_0208e440 = j;
    if (a != b) return;
    func_02011dcc(data_0209b4a0, j);
}

// @symbol _ZN5Sound22StopLoadedMusic_Layer2Ev
void StopLoadedMusic_Layer2(void)
{
    int a = data_0209b4ac;
    if (a < 0) {
        if (data_0208e440 < 0) return;
    }
    data_0208e440 = -1;
    if (a < 0) { func_0204fa2c(data_0209b4a0, 0); return; }
    func_02011dcc(data_0209b4a0, a);
}

}

// @symbol func_02012860
extern "C" void func_02012860(void *a, int flags, int c, int d, int e)
{
    if (flags & 1) func_0204f82c(a, 0xffff, c);
    if (flags & 2) func_0204f7fc(a, 0xffff, d);
    if (flags & 4) func_0204f7cc(a, 0xffff, e - 0x40);
}

namespace Sound {

// @symbol _ZN5Sound6Play2DEjj
void Play2D(unsigned int j1, unsigned int j2)
{
    Player_PlaySoundEffect((int)data_0209b4a4, j1, j2);
}

}

// @symbol func_020127ec
extern "C" void func_020127ec(int a0, int a1, int a2, int a3, int a4, int a5)
{
    Player_PlaySoundEffect((int)data_0209b4a4, a0, a1);
    func_02012860(data_0209b4a4, a2, a3, a4, a5);
}

// @symbol func_020127a4
extern "C" void func_020127a4(int r0, int r1, int r2, int r3)
{
    Player_PlaySoundEffect((int)data_0209b4a4, r0, r1);
    func_0204f7cc(data_0209b4a4, r2, r3);
}

// @symbol func_02012790
extern "C" unsigned int func_02012790(unsigned int a)
{
    return _ZN5Sound6Play2DEjj(2, a);
}

// @symbol func_0201277c
extern "C" unsigned int func_0201277c(unsigned int a)
{
    return _ZN5Sound6Play2DEjj(3, a);
}

namespace Sound {

// @symbol _ZN5Sound12PlayBank3_2DEj
unsigned int PlayBank3_2D(unsigned int a)
{
    return ((unsigned int (*)(unsigned int, unsigned int))Play2D)(3, a);
}

// @symbol _ZN5Sound12PlayBank2_2DEj
unsigned int PlayBank2_2D(unsigned int a)
{
    return ((unsigned int (*)(unsigned int, unsigned int))Play2D)(2, a);
}

}

// @symbol func_02012718
extern "C" void func_02012718(int a, int b)
{
    func_020127ec(2, a, 4, 0, 0, func_020126e8(b));
}

// @symbol func_020126e8
extern "C" int func_020126e8(int a)
{
    int r = a * 0x4c / 0x100000 + 0x1a;
    if (r < 0x1a) return 0x1a;
    if (r > 0x66) return 0x66;
    return r;
}

// @symbol func_020126ac
extern "C" void func_020126ac(int a0, int a1, int a2, int a3, int s0)
{
    func_020127ec(2, a0, a1, a2, a3, s0);
}

// @symbol func_02012694
extern "C" void func_02012694(unsigned int id, const Vector3 *v)
{
    Sound::Play(3, id, *v);
}

// @symbol func_0201267c
extern "C" void func_0201267c(unsigned int id, const Vector3 *v)
{
    Sound::Play(3, id, *v);
}

namespace Sound {

// @symbol _ZN5Sound9PlayBank3EjRK7Vector3
void PlayBank3(u32 id, const Vector3 &pos)
{
    Play(3, id, pos);
}

// @symbol _ZN5Sound9PlayBank0EjRK7Vector3
void PlayBank0(u32 id, const Vector3 &pos)
{
    Play(0, id, pos);
}

// @symbol _ZN5Sound4PlayEjjRK7Vector3
void Play(unsigned int j1, unsigned int j2, const Vector3 &v)
{
    char *s = (char *)func_02050cdc(j1, j2);
    int t = *(unsigned char *)(s + 5);
    if (t == 9 || t == 2) {
        int r = func_02048720(&v, j1, j2);
        if (r == 0)
            return;
        Player_PlaySoundEffect(r, j1, j2);
        func_02048908(r, &v);
        return;
    }
    if (func_02048a1c((int *)&v, j1, j2) == 0)
        return;
    Player_PlaySoundEffect((int)data_0209b4a4, j1, j2);
    func_02048d80(data_0209b4a4, (int *)&v);
}

}

// @symbol func_0201251c
extern "C" void func_0201251c(int a, int b, int c, int d)
{
    int r4 = func_02048a18(b, d);
    if (func_02048a1c((int *)c, a, r4) == 0) return;
    Player_PlaySoundEffect((int)data_0209b4a4, a, r4);
    func_02048af4(&data_0209b4a4, c, d);
}

// @symbol Sound_PlayIfNotActive
extern "C" int Sound_PlayIfNotActive(int a, int b, int c, int d)
{
    char *o = (char *)func_02011934(data_0209b53c, a);
    if (o) {
        *(unsigned char *)(o + 6) = 1;
        return *(int *)o;
    }
    /* original TU declared func_0201186c's #4 as int (def is short):
       the call site passes d untruncated -- cast through an int-arity
       pointer to reproduce it without the narrowing sequence */
    return ((int (*)(char *, int, int, int))func_0201186c)(data_0209b53c, b, c, d);
}

// @symbol func_02012468
extern "C" int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h)
{
    int dd = (int)(((long long)d));
    int x;
    char *p;

    x = Sound_PlayIfNotActive(a, b, c, h);
    p = (char *)func_02011934(data_0209b53c, x);
    if (p != 0)
        func_02012860(p + 8, dd, e, f, g);
    return x;
}

// @symbol func_020123c8
extern "C" void func_020123c8(int a0, int type, int code, int arg)
{
    if (type == 3 && code >= 0x100 && code <= 0x103) {
        func_020485f0(a0, arg);
        return;
    }
    if (type == 3 && code == 0x18d) {
        func_02048528(a0, arg);
        return;
    }
    if (type == 3 && code == 0x81) {
        func_020485f0(a0, arg);
        return;
    }
    func_02048d80((void *)a0, (int *)arg);
}

namespace Sound {

// @symbol _ZN5Sound8PlayLongEjjjRK7Vector3s
int PlayLong(u32 handle, u32 a, u32 b, const Vector3 &pos, s16 e)
{
    if (func_02048a1c((int *)&pos, a, b) == 0)
        return 0;

    char *slot = (char *)func_02011934(data_0209b53c, handle);
    if (slot) {
        *(char *)(slot + 6) = 1;
        func_020123c8((int)(slot + 8), a, b, (int)&pos);
        return *(int *)slot;
    }

    return func_0201179c(data_0209b53c, a, b, (int)&pos, e);
}

}

// @symbol func_02012310
extern "C" int func_02012310(int handle, int sound, int arg)
{
    return Sound_PlayIfNotActive(handle, 2, sound, arg);
}

// @symbol func_0201226c
extern "C" int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5)
{
    void *p;
    if (func_02048a1c((int *)a3, a1, a2) == 0) return 0;
    p = func_02011934(data_0209b53c, a0);
    if (p != 0) {
        *(unsigned char *)((char *)p + 6) = 1;
        func_02048c30((char *)p + 8, a3, a4);
        return *(int *)p;
    }
    return func_020116c4(data_0209b53c, a1, a2, a3, a4, a5);
}

// @symbol func_02012194
extern "C" int func_02012194(char *c, int a1, int a2, int a3, int s0, int s1, short s2)
{
    char *e;
    if (func_02048a1c((int *)s1, a1, a2) == 0) return 0;
    e = (char *)func_02011934(data_0209b53c, (int)c);
    if (e != 0) {
        char *slot = e + 8;
        e[6] = 1;
        func_02048d80(slot, (int *)s1);
        func_0204f7fc(slot, a3, s0);
        return *(int *)e;
    } else {
        char *n = (char *)func_0201179c(data_0209b53c, a1, a2, s1, s2);
        char *e2 = (char *)func_02011934(data_0209b53c, (int)n);
        if (e2 != 0)
            func_0204f7fc(e2 + 8, a3, s0);
        return (int)n;
    }
}

// @symbol func_02012174
extern "C" unsigned int func_02012174(unsigned int a, unsigned int b)
{
    return _ZN5Sound6Play2DEjj(1, b + data_02075250[a]);
}

namespace Sound {

// @symbol _ZN5Sound13PlayCharVoiceEjjRK7Vector3
unsigned int PlayCharVoice(unsigned int a, unsigned int b, const Vector3 &v)
{
    return ((unsigned int (*)(unsigned int, unsigned int, const Vector3 &))Play)(
        1, b + data_02075250[a], v);
}

}

// @symbol func_02012120
extern "C" void func_02012120(unsigned int p0, unsigned int p1, unsigned int p2,
                              const Vector3 *p3, short p4)
{
    p2 += data_02075250[p1];
    Sound::PlayLong(p0, 1, p2, *p3, p4);
}

namespace Sound {

// @symbol _ZN5Sound19LoadGroupAndSetBankEii
void LoadGroupAndSetBank(int a, int b)
{
    if (func_0203d974() != 0) {
        if (a != 0x2f) return;
        func_0203d7d4(func_020134d8((void *)a, (void *)data_0209b498));
        data_0208e428 = b;
        data_0209b47c = a;
        return;
    }
    if (a == 0) {
        func_020134d8((void *)a, (void *)data_0209b498);
        data_0209b4a8 = func_0205117c(data_0209b498);
    } else if (data_0209b47c != a) {
        func_020510a4(data_0209b498, data_0209b4a8);
        func_020134d8((void *)a, (void *)data_0209b498);
        data_0209b484 = func_0205117c(data_0209b498);
    }
    data_0208e428 = b;
    data_0209b47c = a;
    data_0209b478 = 0;
}

// @symbol _ZN5Sound16LoadInitialGroupEi
void LoadInitialGroup(int group)
{
    LoadGroupAndSetBank(group, 0x20);
}

// @symbol _ZN5Sound21UnsetPlayerVoiceGroupEv
void UnsetPlayerVoiceGroup(void)
{
    data_0209b478 = 0;
}

}

// @symbol func_02011f7c
extern "C" void func_02011f7c(void *self)
{
    if (!func_0203d974()) {
        func_020510a4(data_0209b498, data_0209b484);
        func_020134d8(self, (void *)data_0209b498);
        data_0209b488 = func_0205117c(data_0209b498);
    }
    data_0209b478 = (unsigned char)(int)self;
}

namespace Sound {

// @symbol _ZN5Sound21ResetPlayerVoiceGroupEv
void ResetPlayerVoiceGroup(void)
{
    if (func_0203d974() == 0) {
        func_020510a4(data_0209b498, data_0209b484);
    }
    data_0209b478 = 0;
}

}

// @symbol func_02011ee4
extern "C" int func_02011ee4(void *c)
{
    int x = func_0203d974();
    if (x != 0) return x;
    func_020510a4(data_0209b498, data_0209b488);
    return func_020134d8(c, (void *)data_0209b498);
}

// @symbol func_02011e94
extern "C" void func_02011e94(int *out)
{
    int r = func_02051074(data_0209b498);
    func_020511d4(data_0209b498, r, 0, 0, 0);
    if (out != 0) *out = r;
}

// @symbol Player_PlaySoundEffect
extern "C" int Player_PlaySoundEffect(int arg0, int arg1, int arg2)
{
    if (data_0209b480 == 0) return 1;
    if (arg1 != 3)
        return func_02051f1c((void *)arg0, arg1, arg2);
    return func_02051e60(arg0, -1, data_0208e428, -1, arg1, arg2);
}

// @symbol func_02011dcc
extern "C" int func_02011dcc(int *g, int x)
{
    int r4 = func_02051fb4(g, x);
    if (r4 != 0 && g == data_0209b4a0) {
        func_02049764();
        func_02013078();
        if (data_0208e438 >= 0) {
            func_0204921c(g, data_0208e438);
        }
    }
    return r4;
}

// @symbol func_02011dc0
extern "C" void func_02011dc0(void)
{
    func_0204effc();
}

// @symbol func_02011db4
extern "C" void func_02011db4(void)
{
    func_0204efe0();
}

// @symbol func_02011d5c
extern "C" void func_02011d5c(int self)
{
    if (data_0209b480 == 0) return;
    switch (self) {
    case 1: func_02048e94(); break;
    case 2: func_02048e74(); break;
    default: func_02048e54(); break;
    }
    func_02049c48(self);
}

// @symbol func_02011d50
extern "C" void func_02011d50(void)
{
    func_02048f24();
}

// @symbol func_02011d44
extern "C" void func_02011d44(void)
{
    func_02048f14();
}

// @symbol func_02011d38
extern "C" void func_02011d38(void)
{
    func_02048f04();
}

// @symbol func_02011d2c
extern "C" void func_02011d2c(void)
{
    func_02048ef4();
}

// @symbol func_02011d20
extern "C" void func_02011d20(void)
{
    _ZN5Sound13Func_02048ee4Ev();
}

// @symbol func_02011d14
extern "C" void func_02011d14(void)
{
    func_02048ed4();
}

// @symbol func_02011d08
extern "C" void func_02011d08(void)
{
    _ZN5Sound13Func_02048ec4Ev();
}

// @symbol func_02011cfc
extern "C" void func_02011cfc(void)
{
    _ZN5Sound13Func_02048eb4Ev();
}

// @symbol DisableSoundPlayersForCredits
extern "C" void DisableSoundPlayersForCredits(void)
{
    u32 i;
    s32 zero;
    zero = 0;
    for (i = 0; i < 10; i++)
        Sound::Player::SetPlayableSeqCount(data_0208e448[i].playerID, zero);
}

// @symbol func_02011c8c
extern "C" void func_02011c8c(void)
{
    u32 i;
    for (i = 0; i < 10; i++)
        Sound::Player::SetPlayableSeqCount(data_0208e448[i].playerID, data_0208e448[i].maxSeq);
}

// @symbol func_02011c54
extern "C" void func_02011c54(void)
{
    unsigned int i;
    unsigned char *p = data_02075258;
    for (i = 0; i < 11; i++) {
        func_0204f9c4(*p, 1);
        p = p + 1;
    }
}

// @symbol func_02011c24
extern "C" void func_02011c24(void)
{
    func_0204f958(0, 1);
    func_0204f958(1, 1);
    func_02011c54();
}

// @symbol func_02011b7c
extern "C" void func_02011b7c(void)
{
    int v = data_0208e430;
    data_0209b490 = 0x7f000;
    data_0208e42c = 0x7f;
    data_0209b494 = 0x7f000;
    if (v >= 0) {
        data_0208e430 = -1;
        func_0204fa2c(data_0209b4b0, 0);
    }
    data_0208e444 = -1;
    data_0208e440 = -1;
    data_0209b49c = 0;
    data_0209b470 = 0;
    data_0208e420 = -1;
}

// @symbol func_02011b38
extern "C" void func_02011b38(void *c, int a1, int a2, int a3, short a5)
{
    *(int *)c = a1;
    *(short *)((char *)c + 4) = a5;
    *(char *)((char *)c + 6) = 1;
    *(int *)((char *)c + 0xc) = 0;
    *(int *)((char *)c + 0x10) = 0;
    Player_PlaySoundEffect((int)((char *)c + 8), a2, a3);
}

// @symbol func_02011af8
extern "C" void func_02011af8(void *c, int a1, int a2, int a3, int s0, short s1)
{
    func_02011b38(c, a1, a2, a3, s1);
    func_020123c8((int)((char *)c + 8), a2, a3, s0);
}

// @symbol func_02011ac4
extern "C" void func_02011ac4(void *c, int a1, int a2, int a3, int s0, int s1, short s2)
{
    func_02011b38(c, a1, a2, a3, s2);
    func_02048c30((char *)c + 8, s0, s1);
}

// @symbol func_02011aa0
extern "C" int func_02011aa0(void *c)
{
    func_0204fa2c((int *)((char *)c + 8), *(short *)((char *)c + 4));
    *(int *)c = 0;
    return 0;
}

// @symbol func_02011a5c
extern "C" void func_02011a5c(void *vc)
{
    SndRegTable *c = (SndRegTable *)vc;
    int i;
    SndRegEntry *p = c->entries;
    do {
        p->field_0 = 0;
        p++;
    } while (p != c->entries + 0x40);
    c->counter = 0;
    c->field_4 = 0;
    for (i = 0; i < 16; i++) c->buckets[i] = 0;
}

// @symbol func_02011a28
extern "C" void func_02011a28(char *c)
{
    char *p = c + 8;
    int i;
    for (i = 0; i < 0x40; i++) {
        func_0204f94c(p + 8);
        p += 0x14;
    }
}

// @symbol func_020119c8
extern "C" void func_020119c8(void *vbase)
{
    SndRegTable *base = (SndRegTable *)vbase;
    SndRegEntry *e = base->entries;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (e->field_0) {
            if (e->field_6 == 1) {
                e->field_6 = 0;
            } else {
                func_02011654((char *)base, (char *)e);
                base->field_4 = (unsigned char)i;
            }
        }
        e++;
    }
}

// @symbol func_02011974
extern "C" void func_02011974(void *vbase)
{
    SndRegTable *base = (SndRegTable *)vbase;
    SndRegEntry *e = base->entries;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (e->field_0) {
            e->field_6 = 0;
            func_02011654((char *)base, (char *)e);
            base->field_4 = (unsigned char)i;
        }
        e++;
    }
}

// @symbol func_02011934
extern "C" void *func_02011934(char *table, int id)
{
    if (id == 0) return 0;
    int idx = id & 0xf;
    char *base = table + (idx << 2);
    char *node = *(char **)(base + 0x508);
    while (node) {
        if (id == *(int *)node) return node;
        node = *(char **)(node + 0x10);
    }
    return 0;
}

// @symbol func_0201186c
extern "C" int func_0201186c(char *thiz, int a1, int a2, short a3)
{
    int i = 0;
    do {
        if (*(int *)(thiz + *(u8 *)(thiz + 4) * 0x14 + 8) == 0) {
            *(int *)thiz += 1;
            if (*(int *)thiz == 0) {
                *(int *)thiz += 1;
            }
            func_02011b38(thiz + 8 + *(u8 *)(thiz + 4) * 0x14, *(int *)thiz, a1, a2, a3);
            func_02011698(thiz, thiz + 8 + *(u8 *)(thiz + 4) * 0x14);
            return *(int *)thiz;
        }
        *(u8 *)(thiz + 4) = (*(u8 *)(thiz + 4) + 1) % 0x40;
    } while (++i < 0x40);
    return 0;
}

// @symbol func_0201179c
extern "C" int func_0201179c(char *thiz, int a1, int a2, int a3, short s1)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        if (*(int *)(thiz + *(u8 *)(thiz + 4) * 0x14 + 8) == 0) {
            *(int *)thiz += 1;
            if (*(int *)thiz == 0) *(int *)thiz += 1;
            func_02011af8(thiz + 8 + *(u8 *)(thiz + 4) * 0x14, *(int *)thiz, a1, a2, a3, s1);
            func_02011698(thiz, thiz + 8 + *(u8 *)(thiz + 4) * 0x14);
            return *(int *)thiz;
        }
        *(u8 *)(thiz + 4) = (*(u8 *)(thiz + 4) + 1) % 0x40;
    }
    return 0;
}

// @symbol func_020116c4
extern "C" int func_020116c4(char *c, int a1, int a2, int a3, int s0, short s1)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        if (*(int *)(c + (u8)c[4] * 0x14 + 8) == 0) {
            *(int *)c = *(int *)c + 1;
            if (*(int *)c == 0) *(int *)c = *(int *)c + 1;
            func_02011ac4(c + 8 + (u8)c[4] * 0x14, *(int *)c, a1, a2, a3, s0, s1);
            func_02011698(c, c + 8 + (u8)c[4] * 0x14);
            return *(int *)c;
        }
        c[4] = (u8)(((u8)c[4] + 1) % 0x40);
    }
    return 0;
}

// @symbol func_02011698
extern "C" void func_02011698(char *table, char *n)
{
    int idx = (*(int *)n) & 0xf;
    char *base = table + (idx << 2);
    char *head = *(char **)(base + 0x508);
    if (head == 0) {
        *(char **)(base + 0x508) = n;
    } else {
        *(char **)(head + 0xc) = n;
        *(char **)(n + 0x10) = *(char **)(base + 0x508);
        *(char **)(base + 0x508) = n;
    }
}

// @symbol func_02011654
extern "C" int func_02011654(char *table, char *n)
{
    void *prev = *(void **)(n + 0xc);
    void *next = *(void **)(n + 0x10);
    if (prev == 0) {
        int idx = (*(int *)n) & 0xf;
        *(void **)(table + (idx << 2) + 0x508) = next;
    } else {
        *(void **)((char *)prev + 0x10) = next;
    }
    if (next) {
        *(void **)((char *)next + 0xc) = prev;
    }
    return func_02011aa0(n);
}

#pragma defer_codegen off

// @symbol func_020132d8
extern "C" void func_020132d8(void)
{
    if (data_0209b480 == 0) return;
    func_02012d64();
    if (data_0208e430 >= 0) {
        ApproachLinear(data_0209b49c, data_0209b470 << 0xc, data_0209b494);
        func_02013524((int)data_0209b4b0, data_0209b49c >> 0xc, 0);
        if (data_0209b49c == 0) {
            data_0208e430 = -1;
            func_0204fa2c(data_0209b4b0, 0);
        }
    }
    int s = func_02013078();
    if (s >= 0) {
        func_020494cc(&data_0209b4a0, s);
    }
    func_020490b0(&data_0209b4a0);
    func_0204f03c();
    func_020119c8(&data_0209b53c);
}

// @symbol func_020133bc
#pragma opt_propagation off
extern "C" void func_020133bc(void)
{
    char *anim;
    func_0204f070();
    data_0209b498 = (int)func_0205130c((u32)_ZN6Memory8AllocateEj(0x100000), 0x100000);
    anim = data_0208e498;
    func_0201a9fc(data_0209d574);
    func_02050f34(&data_0209b4b4, (int)anim, data_0209b498, 0);
    func_0201a9fc(data_0209d574);
    func_02052008(func_0203d974() == 0 ? data_0209b498 : 0);
    if (func_0203d974() == 0) {
        func_0201a9fc(data_0209d574);
        func_020134d8((void *)1, (void *)data_0209b498);
        func_0201a9fc(data_0209d574);
    }
    func_0204f94c(&data_0209b4a0);
    func_0204f94c(&data_0209b4b0);
    func_0204f94c(&data_0209b4a4);
    func_02011a28(data_0209b53c);
    func_02048f34(&data_0209b4b4);
    func_020506fc(2);
    func_02049cd8(&data_0209baa0, 0x1000, 0);
    data_0209b480 = 1;
}
/* opt_propagation is file-global last-wins under deferred codegen in mwccarm
 * 2004/b56, and the deferred queue's codegen binds when the next function
 * parses. The defer_codegen brackets emit func_020132d8 and func_020133bc at
 * parse position -- func_020132d8 first, flushing the queued functions under
 * on -- so func_020133bc alone sees off and keeps `anim` in r4 like the ROM. */
#pragma opt_propagation on
#pragma defer_codegen on

// @symbol func_02013524
extern "C" void func_02013524(int a, int b, int c)
{
    if (data_0209b474 != 0) {
        c = b = 0;
    }
    func_0204f8cc(a, b, c);
}

// @symbol func_020134d8
extern "C" int func_020134d8(void *a, void *b)
{
    int s = func_0201a9ec((int *)data_0209d574);
    int r = func_02051918((u32)a, (int)b);
    func_0201a9fc(data_0209d574);
    func_0201a96c((int *)data_0209d574, s);
    return r;
}

// @symbol func_020134c8
extern "C" void func_020134c8(void)
{
    /* arity split (banked): caller passes 0, def is (void) */
    ((void (*)(int))func_020133bc)(0);
}
