typedef short s16;
typedef struct { int x, y, z; } Vector3;

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void* self, Vector3* v, int power);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(Vector3* in, void* m, Vector3* out);
extern void AddVec3(Vector3* a, Vector3* b, Vector3* out);
extern void _ZN5Actor13LandingDustAtER7Vector3b(void* self, Vector3* v, int b);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, Vector3* pos);
extern int _Z14ApproachLinearRsss(s16* dst, s16 target, s16 step);
extern int Vec3_HorzDist(Vector3* a, Vector3* b);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void func_ov071_021223c8(void* c, int i);

extern void* data_020a0e68;

void func_ov071_021221bc(void* self) {
    char* c = (char*)self;
    Vector3 vin, vout;
    Vector3 pp;
    Vector3 din, dout;
    Vector3 eq;
    Vector3 ld;

    if (*(s16*)(c + 0x8c) != 0) {
        _Z14ApproachLinearRsss((s16*)(c + 0x326), -0x7d0, 0xc8);
        if (_Z14ApproachLinearRsss((s16*)(c + 0x8c), 0, -*(s16*)(c + 0x326)) == 0)
            return;
        eq.x = *(int*)(c + 0x5c);
        eq.y = *(int*)(c + 0x60);
        eq.z = *(int*)(c + 0x64);
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(self, &eq, 0x5dc000);

        vin.x = 0; vin.y = 0; vin.z = -0xc8000;
        vout.x = 0; vout.y = 0; vout.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
        MulVec3Mat4x3(&vin, &data_020a0e68, &vout);
        AddVec3(&vout, (Vector3*)(c + 0x5c), &vout);
        ld.x = vout.x; ld.y = vout.y; ld.z = vout.z;
        _ZN5Actor13LandingDustAtER7Vector3b(self, &ld, 1);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x5a, (Vector3*)(c + 0x74));
    } else {
        Vector3* plp;
        void* pl = _ZN5Actor13ClosestPlayerEv(self);
        if (pl == 0)
            return;
        plp = (Vector3*)(((int)pl + 0x5c) & 0xFFFFFFFFFFFFFFFFll);
        pp.x = plp->x;
        pp.y = plp->y;
        pp.z = plp->z;
        din.x = 0; din.y = 0x64000; din.z = -0xc8000;
        dout.x = 0; dout.y = 0; dout.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
        MulVec3Mat4x3(&din, &data_020a0e68, &dout);
        AddVec3(&dout, (Vector3*)(c + 0x5c), &dout);
        if (Vec3_HorzDist(&dout, &pp) >= 0x12c000)
            return;
        if (DecIfAbove0_Short((unsigned short*)(c + 0x324)) != 0)
            return;
        func_ov071_021223c8(self, 1);
    }
}
