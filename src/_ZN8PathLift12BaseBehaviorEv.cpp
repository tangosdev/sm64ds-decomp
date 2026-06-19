//cpp
struct PathLift;
typedef void (PathLift::*PLFn)();
struct Entry { char pad[8]; PLFn fn; char tail[4]; };
extern "C" Entry data_ov002_0210af2c[];
struct PathLift { char pad[0x450]; };
extern "C" void _ZN8PathLift12BaseBehaviorEv(PathLift *c) {
    int i = *(int*)((char*)c + 0x44c);
    Entry *e = &data_ov002_0210af2c[i];
    (c->*(e->fn))();
    *((unsigned char*)c + 0x42a) = 0;
}
