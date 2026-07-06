//cpp
// NONMATCHING: regperm: the pooled base data_020a0c80 colors into r1 (`ldr r1;ldr r0,[r1,r4,lsl#2]`) but the ROM uses r2. Occupying r1 (e.g. passing the dead `a` param) forces r2 but changes a call arg; no zero-cost lever found. (div=2)
/* _ZN16MeshColliderBase6EnableEP5Actor at 0x02039184 (arm9), size 0x6c
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc */
typedef int s32;

extern void func_020395fc(void* c);
extern void func_020393fc(void* p, int v);
extern void* data_020a0c80[];

struct Actor;

struct MeshColliderBase {
    int Enable(Actor* a);
};

int MeshColliderBase::Enable(Actor* a)
{
    s32 i = 0;
    for (;;) {
        if (data_020a0c80[i] == 0) {
            func_020395fc(this);
            func_020393fc(this, i);
            data_020a0c80[i] = this;
            return 1;
        }
        i++;
        if (i >= 0x18) break;
    }
    return 0;
}