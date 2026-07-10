//cpp
typedef int Fix12i;

struct BowserPuzzleManager;
typedef void (BowserPuzzleManager::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov064_0211c904[];

extern "C" void func_ov064_02118ee4(void* c);
extern "C" char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern "C" void func_ov064_02119010(void* c);
extern "C" int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* c, Fix12i a, Fix12i b);
extern "C" void func_ov064_02118fa4(void* c);

struct BowserPuzzleManager {
    unsigned char pad[0x336];
    unsigned char idx;
    int Behavior();
};

int BowserPuzzleManager::Behavior() {
    func_ov064_02118ee4(this);
    (this->*data_ov064_0211c904[idx].pmf)();
    char* cc = (char*)this;
    char* p = 0;
    unsigned int id = *(unsigned int*)(cc + 0x320);
    if (id != 0)
        p = _ZN5Actor10FindWithIDEj(id);
    if (p == 0 || *(unsigned char*)(p + 0xd6) == 0) {
        unsigned short* ctr = (unsigned short*)(int)(((long long)(int)(cc + 0x334)) & 0xFFFFFFFFFFFFFFFF);
        *ctr = *ctr + 1;
    }
    func_ov064_02119010(cc);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(cc, 0, 0) != 0)
        func_ov064_02118fa4(cc);
    return 1;
}
