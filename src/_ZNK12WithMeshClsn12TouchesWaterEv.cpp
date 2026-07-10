//cpp
extern "C" int func_02037e78(const void* p);

struct WithMeshClsn
{
    char pad[0x34];
    int TouchesWater() const;
};

int WithMeshClsn::TouchesWater() const
{
    return func_02037e78((const char*)this + 0x34);
}
