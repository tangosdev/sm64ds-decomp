//cpp
extern "C" int func_020378dc(const void* p);

struct WithMeshClsn
{
    char pad[0x20];
    int GetWallResult() const;
};

int WithMeshClsn::GetWallResult() const
{
    return func_020378dc((const char*)this + 0x20);
}
