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

extern SharedFilePtr WATER_RING_MODEL_PTR;
extern SharedFilePtr RED_NUMBER_MODEL_PTR;
extern signed char LEVEL_ID;
extern unsigned char STAR_ID;
extern int data_ov064_0211c934;

extern "C" int func_ov064_0211992c(char* c)
{
    Model::LoadFile(WATER_RING_MODEL_PTR);
    Model::LoadFile(RED_NUMBER_MODEL_PTR);
    *(int*)(c + 0x314) = (*(unsigned int*)(c + 8) >> 0xc) & 0xf;
    *(int*)(c + 0x318) = *(unsigned int*)(c + 8) & 1;
    if ((*(unsigned int*)(c + 8) & 0xf) > 1) *(int*)(c + 0x318) = 0;
    if (LEVEL_ID == 8 && (STAR_ID == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0)) {
        return 0;
    }
    ((MovingCylinderClsn*)(c + 0x110))->Init((Actor*)c, 0xc8000, 0x190000, 0x800004, 0);
    func_ov064_0211982c(c, &data_ov064_0211c934);
    return 1;
}
