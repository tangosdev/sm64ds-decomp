//cpp
/* _ZN5Stage16CleanupResourcesEv @ 0x0202c9a8 (arm9, size 0x264)
 * Releases the level file handles, tears down fader/mesh-collider/message
 * state and unloads the level overlays/archive. Returns 1.
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned int u32;

extern "C" {
extern void *data_020756f0[];   /* 12 SharedFilePtr* released on cleanup */
extern u8 data_0209f2d8;
extern int data_0209d4a8;
extern char *data_0209f340;     /* -> struct, count byte at +0x14 */
extern int data_0209f320;
extern int data_0209f314;
extern void *data_0209f324;
extern int data_0209d4b0;
extern s8 data_0209f2f8;
extern int data_0209f394[];
extern int data_0209f318;
extern u8 data_0209f2c4;
extern u8 data_0209f2e0;
extern u8 data_0209f2d4;
extern u8 data_0209f244;
extern u8 data_0209f2b0;
extern u8 data_0209f20c;

extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void _ZN5Sound21ResetPlayerVoiceGroupEv(void);
extern void EndKuppaScript(void);
extern void _ZN6Memory16operator_delete2EPv(void *);
extern void _ZN5Scene20SetAndStopColorFaderEv(void);
extern void func_02073244(void *, int, int, void (*)(void *));
extern void _ZN9FaderWipeD1Ev(void *);
extern void CleanCommonModelDataArr(void);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN5Stage18ResetMeshCollidersEv(void);
extern void func_01ffb0c8(void *);
extern void Deallocate(void);
extern void _Z19UnloadLevelOverlaysi(int);
extern void UnloadArchive(int);
extern void _ZN7Message15ResetAllGlobalsEv(void);
extern void _ZN5Sound6Player19SetPlayableSeqCountEii(int, int);

static inline void DestroyVirt(void **o)
{
    if (o)
        ((void (*)(void *))((void **)o[0])[1])((void *)o);
}

int _ZN5Stage16CleanupResourcesEv(char *thiz)
{
    u32 i;

    for (i = 0; i < 12; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_020756f0[i]);

    {
        int atEnd = (data_0209f2d8 == 2);
        if (!atEnd)
            _ZN5Sound21ResetPlayerVoiceGroupEv();
    }
    EndKuppaScript();
    data_0209d4a8 = 0;

    {
        void **o = *(void ***)(thiz + 0x9bc);
        if (o)
            DestroyVirt(o);
    }

    {
        char *e = thiz + 0x8bc;
        int j;
        for (j = 0; j < *(u8 *)(data_0209f340 + 0x14); j++, e += 0xc) {
            void **o = *(void ***)e;
            if (o)
                DestroyVirt(o);
            {
                char *p = *(char **)(e + 8);
                while (p) {
                    char *next = *(char **)(p + 0xc);
                    _ZN6Memory16operator_delete2EPv(p);
                    p = next;
                }
            }
        }
    }

    data_0209f320 = 0;
    data_0209f314 = 0;
    _ZN5Scene20SetAndStopColorFaderEv();
    data_0209d4b0 = 0;
    func_02073244(data_0209f324, 0x60, 8, _ZN9FaderWipeD1Ev);
    data_0209f324 = 0;
    CleanCommonModelDataArr();
    _ZN16MeshColliderBase7DisableEv(thiz + 0x91c);
    _ZN5Stage18ResetMeshCollidersEv();
    func_01ffb0c8(thiz + 0x91c);
    Deallocate();
    _Z19UnloadLevelOverlaysi(data_0209f2f8);
    if (data_0209f2f8 == 1)
        UnloadArchive(7);

    {
        int k;
        for (k = 0; k < 4; k++)
            data_0209f394[k] = 0;
    }
    data_0209f318 = 0;
    data_0209f2c4 = 0;
    _ZN7Message15ResetAllGlobalsEv();

    {
        u8 mode = data_0209f2d8;
        int music = 0;
        data_0209f2e0 = 0;
        data_0209f2d4 = 0;
        data_0209f244 = 0;
        data_0209f2b0 = 0;
        data_0209f20c = 0;
        if (mode == 1)
            music = 1;
        if (music) {
            _ZN5Sound6Player19SetPlayableSeqCountEii(2, 2);
            _ZN5Sound6Player19SetPlayableSeqCountEii(3, 1);
        }
    }
    return 1;
}
}
