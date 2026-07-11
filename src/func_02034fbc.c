typedef unsigned int u32;
extern int LoadArchive(int idx);
extern void *func_0201a458(void);
extern void *_ZN4Heap10SetDefaultEv(void *h);
extern void LoadTextNarcs(void);
extern void func_0201a5f8(int t);
extern void LoadOverlay(int id);
extern void *func_0201a3e4(void);

extern u32 data_020a0c60;
extern void *data_020a0c5c;
extern int overlay_64;
extern int overlay_66;

void func_02034fbc(void)
{
    void *h;

    LoadArchive(0);
    h = func_0201a458();
    if (!(data_020a0c60 & 1)) {
        data_020a0c5c = _ZN4Heap10SetDefaultEv(h);
        data_020a0c60 |= 1;
    }
    LoadTextNarcs();
    LoadArchive(1);
    _ZN4Heap10SetDefaultEv(data_020a0c5c);
    func_0201a5f8(6);
    LoadOverlay((int)&overlay_64);
    LoadOverlay((int)&overlay_66);
    data_020a0c5c = _ZN4Heap10SetDefaultEv(func_0201a3e4());
    LoadArchive(7);
    _ZN4Heap10SetDefaultEv(data_020a0c5c);
}
