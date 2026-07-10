//cpp
struct Vector3 { int x, y, z; };
extern "C" {
int Vec3_HorzDist(const void* a, const void* b);
short Vec3_HorzAngle(const void* a, const void* b);
int _Z11UpdateAngleRssis(short* a, short b, int c, short d);
void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, const Vector3& v, unsigned int e);
void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void* a);
void func_0201267c(int a, void* t);
int _ZNK12WithMeshClsn13JustHitGroundEv(void* self);
void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void* self, const Vector3* v, int fix);
void func_ov072_0211f158(void* c);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* c);
void func_ov072_0211f330(void* c, void* p);
void func_ov072_0211f280(void* c);
extern Vector3 data_ov072_02122b58;
}

extern "C" int func_ov072_0211f65c(unsigned char* thiz)
{
    unsigned char* st;
    switch (thiz[0x3a2]) {
    case 0:
        {
            int d = Vec3_HorzDist(&data_ov072_02122b58, thiz + 0x5c);
            _Z11UpdateAngleRssis((short*)(thiz + 0x8e), Vec3_HorzAngle(thiz + 0x5c, &data_ov072_02122b58), 2, 0x600);
            *(short*)(thiz + 0x94) = *(short*)(thiz + 0x8e);
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(thiz + 0x5c), *(int*)(thiz + 0x60), *(int*)(thiz + 0x64));
            *(unsigned int*)(thiz + 0x39c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
                *(unsigned int*)(thiz + 0x39c), 3, 0x8a, *(const Vector3*)(thiz + 0x74), 0);
            if (d < 0x17c000) {
                void* a = _ZN5Actor15FindWithActorIDEjPS_(0x111, 0);
                *(unsigned char*)((unsigned char*)a + 0x336) = 1;
                func_0201267c(0x114, thiz + 0x74);
                *(int*)(thiz + 0xa8) = 0x1d000;
                *(int*)(thiz + 0x98) = 0xe000;
                st = (unsigned char*)(((int)thiz + 0x3a2) & 0xFFFFFFFFFFFFFFFFLL);
                *st = *st + 1;
            }
        }
        break;
    case 1:
        if (_ZNK12WithMeshClsn13JustHitGroundEv(thiz + 0x180) != 0) {
            Vector3 v;
            v.x = *(int*)(thiz + 0x5c);
            v.y = *(int*)(thiz + 0x60);
            v.z = *(int*)(thiz + 0x64);
            _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(thiz, &v, 0x5dc000);
            *(int*)(thiz + 0x5c) = data_ov072_02122b58.x;
            *(int*)(thiz + 0x60) = data_ov072_02122b58.y;
            *(int*)(thiz + 0x64) = data_ov072_02122b58.z;
            *(short*)(thiz + 0x8c) = 0;
            *(short*)(thiz + 0x8e) = (short)-0x4000;
            *(int*)(thiz + 0x98) = 0;
            st = (unsigned char*)(((int)thiz + 0x3a2) & 0xFFFFFFFFFFFFFFFFLL);
            *st = *st + 1;
        }
        break;
    case 2:
    default:
        break;
    }

    func_ov072_0211f158(thiz);
    _ZN5Actor9UpdatePosEP12CylinderClsn(thiz, thiz + 0x14c);
    func_ov072_0211f330(thiz, thiz + 0x180);
    func_ov072_0211f280(thiz);
    return 1;
}
