//cpp
extern "C" int _ZN6Player7IsStateERNS_5StateE(void* self, void* state);
extern "C" int _ZN6Player7TryGrabER5Actor(void* arg, void* self);
extern "C" int func_ov002_020db5f4(void* self, void* arg);

extern char _ZN6Player14ST_DIZZY_STARSE;
extern char _ZN6Player7ST_HURTE;

extern "C" int func_ov002_020d85fc(char* self, char* arg, int flags){
    if ((flags & 0x1000)
        && *(int*)(arg + 8) == 2
        && (_ZN6Player7IsStateERNS_5StateE(self, &_ZN6Player14ST_DIZZY_STARSE)
            || (_ZN6Player7IsStateERNS_5StateE(self, &_ZN6Player7ST_HURTE)
                && *(unsigned short*)(self + 0x6a4) != 0))
        && _ZN6Player7TryGrabER5Actor(arg, self)) {
        func_ov002_020db5f4(self, arg);
        return 1;
    }
    return 0;
}
