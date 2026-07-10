//cpp
typedef int Fix12i;
struct SharedFilePtr;
struct Actor;
struct PMF;
namespace Model { void LoadFile(SharedFilePtr& f); }
extern "C" int SublevelToLevel(int i);
extern "C" int IsStarCollected(int r0, int r1);
struct MovingCylinderClsn { void Init(Actor* a, Fix12i b, int c, unsigned int d, unsigned int e); };
extern "C" void func_ov064_0211982c(void *c, void *p);

extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
extern int data_ov064_0211c934;

extern "C" int _ZN17BowserPuzzlePiece13InitResourcesEv(char* c)
{
    Model::LoadFile(data_ov002_0210da10);
    Model::LoadFile(data_ov002_0210d9a8);
    *(int*)(c + 0x314) = (*(unsigned int*)(c + 8) >> 0xc) & 0xf;
    *(int*)(c + 0x318) = *(unsigned int*)(c + 8) & 1;
    if ((*(unsigned int*)(c + 8) & 0xf) > 1) *(int*)(c + 0x318) = 0;
    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0)) {
        return 0;
    }
    ((MovingCylinderClsn*)(c + 0x110))->Init((Actor*)c, 0xc8000, 0x190000, 0x800004, 0);
    func_ov064_0211982c(c, &data_ov064_0211c934);
    return 1;
}
