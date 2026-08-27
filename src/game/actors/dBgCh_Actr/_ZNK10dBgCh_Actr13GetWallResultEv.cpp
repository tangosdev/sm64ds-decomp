//cpp
extern "C" int func_020378dc(const void* p);

struct dBgCh_Actr
{
    char pad[0x20];
    int GetWallResult() const;
};

int dBgCh_Actr::GetWallResult() const
{
    return func_020378dc((const char*)this + 0x20);
}
