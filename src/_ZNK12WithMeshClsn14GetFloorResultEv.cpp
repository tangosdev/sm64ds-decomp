//cpp
extern "C" int func_02037938(const void* p);

struct WithMeshClsn
{
    char pad[0x20];
    int GetFloorResult() const;
};

int WithMeshClsn::GetFloorResult() const
{
    return func_02037938((const char*)this + 0x20);
}
