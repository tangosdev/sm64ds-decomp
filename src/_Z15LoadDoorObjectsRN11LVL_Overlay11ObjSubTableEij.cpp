//cpp
// _Z15LoadDoorObjectsRN11LVL_Overlay11ObjSubTableEij at 0x020fe4f0
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };

struct DoorEntry {
    short x;
    short y;
    short z;
    short angle;
    unsigned short field8;
    unsigned short fielda;
};

class LVL_Overlay {
public:
    struct ObjSubTable {
        unsigned char b0;
        unsigned char count;
        DoorEntry* entries;
    };
};

extern unsigned short data_ov002_0210cb88[];
extern unsigned char data_ov002_0210cb70[];

extern "C" int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    unsigned int id, unsigned int param, const Vector3& pos,
    const Vector3_16* rot, int a, int b);

void LoadDoorObjects(LVL_Overlay::ObjSubTable& sub, int a, unsigned int b) {
    DoorEntry* e = sub.entries;
    for (int i = 0; i < sub.count; i++) {
        Vector3 pos;
        int z = e->z << 12;
        int y = e->y << 12;
        int x = e->x << 12;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        Vector3_16 rot;
        short rx = (e->field8 >> 8) & 0xf;
        short ry = e->angle;
        short rz = (e->field8 >> 12) & 0xf;
        rot.x = rx;
        rot.y = ry;
        rot.z = rz;
        int idx = e->fielda & 0x1f;
        unsigned int id = data_ov002_0210cb88[idx];
        unsigned int param = e->field8 | (data_ov002_0210cb70[idx] << 16);
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(id, param, pos, &rot, -1, -1);
        e++;
    }
}
