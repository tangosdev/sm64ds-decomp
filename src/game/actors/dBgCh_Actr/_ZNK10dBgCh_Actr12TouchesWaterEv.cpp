//cpp
extern "C" int SurfaceInfo_TestFlag0x20(const void* p);

struct dBgCh_Actr
{
    char pad[0x34];
    int TouchesWater() const;
};

int dBgCh_Actr::TouchesWater() const
{
    return SurfaceInfo_TestFlag0x20((const char*)this + 0x34);
}
