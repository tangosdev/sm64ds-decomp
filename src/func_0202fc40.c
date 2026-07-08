extern void func_0202ed14(char *o);
extern void *_ZTV5Fader;
extern void *_ZTV15FaderBrightness;
extern void *_ZTV10FaderColor;
extern void *data_020926f0;

void *func_0202fc40(char *self) {
    *(void**)self = &_ZTV5Fader;
    *(void**)self = &_ZTV15FaderBrightness;
    *(int*)(self + 4) = 0;
    *(int*)(self + 8) = 0;
    *(void**)self = &_ZTV10FaderColor;
    *(short*)(self + 0xc) = 0;
    *(void**)self = &data_020926f0;
    func_0202ed14(self);
    return self;
}
