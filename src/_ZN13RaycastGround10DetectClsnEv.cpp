//cpp
struct Vec3 { int x, y, z; };
struct ClsnObj { char pad[0x5c]; int vec[3]; char pad2[0x48]; int flags; int hb4; int hb8; };
struct ClsnActor { virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3(); virtual int v4(); virtual int v5(); virtual int v6(void*); };
extern "C" void func_02037464(void*);
extern "C" ClsnObj* func_020393b4(ClsnActor*);
extern "C" int func_020393ac(ClsnActor*);
extern "C" int func_0203939c(ClsnActor*);
extern "C" int func_0203938c(ClsnActor*);
extern "C" int func_02035354(void*, ClsnObj*);
extern "C" void func_02037fec(char*, int, int, int, ClsnActor*);
extern "C" int Vec3_HorzDist(Vec3*, Vec3*);
extern "C" ClsnActor* data_020a0c80[];
class RaycastGround { public: char pad[0x38]; Vec3 f38; int DetectClsn(); };

int RaycastGround::DetectClsn()
{
    int ret = 0;
    func_02037464(this);
    ClsnActor* o = data_020a0c80[0];
    if (o != 0 && func_02035354(this, func_020393b4(o)) == 0 && o->v6(this) != 0) {
        func_02037fec((char*)this + 0x10, 0, func_020393ac(o), (int)func_020393b4(o), o);
        ret = 1;
    }
    int flag = 1;
    int one = 1;
    Vec3 pos;
    for (int i = 1; i < 24; i++) {
        ClsnActor* obj = data_020a0c80[i];
        if (obj == 0) continue;
        ClsnObj* p = func_020393b4(obj);
        if (func_02035354(this, p) != 0) continue;
        if (p != 0 && ((p->flags & 2) ? flag : 0) != 0) {
            int* pv = (int*)(((long long)(int)&p->vec[0]) & 0xFFFFFFFFFFFFFFFFLL);
            pos.x = pv[0];
            pos.y = pv[1];
            pos.z = pv[2];
            int thr = func_0203939c(obj);
            if (thr == -0x1000) {
                int h8 = p->hb8;
                pos.y = pos.y + p->hb4;
                thr = h8 << 3;
            } else {
                pos.y = pos.y + func_0203938c(obj);
            }
            Vec3* selfpos = (Vec3*)(((long long)(int)&this->f38) & 0xFFFFFFFFFFFFFFFFLL);
            if (selfpos->y < pos.y - thr) continue;
            if (Vec3_HorzDist(selfpos, &pos) > thr) continue;
        }
        if (obj->v6(this) != 0) {
            func_02037fec((char*)this + 0x10, i, func_020393ac(obj), (int)func_020393b4(obj), obj);
            ret = one;
        }
    }
    return ret;
}
