//cpp
extern "C" int SurfaceInfo_TestFlag0x20(const void* p);

struct WithMeshClsn
{
    char pad[0x34];
    int TouchesWater() const;
};

int WithMeshClsn::TouchesWater() const
{
    return SurfaceInfo_TestFlag0x20((const char*)this + 0x34);
}
