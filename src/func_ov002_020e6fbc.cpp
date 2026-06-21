//cpp
struct Actor;
struct SoundObj { int f4; };
struct Actor { SoundObj *SpawnSoundObj(unsigned int); };
extern "C" int func_ov002_020ea3a4(void *);

extern "C" void func_ov002_020e6fbc(Actor *self, int arg) {
    if (*(int *)((char *)self + 0x430) != 0) return;
    SoundObj *s = self->SpawnSoundObj(4);
    if (s == 0) return;
    *(int *)((char *)self + 0x430) = *(int *)((char *)s + 4);
    int r = func_ov002_020ea3a4(self);
    *(int *)((char *)s + 0xd4) = r;
    *(int *)((char *)s + 0xd8) = arg;
}
