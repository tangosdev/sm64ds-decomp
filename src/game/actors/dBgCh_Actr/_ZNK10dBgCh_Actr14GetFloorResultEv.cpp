//cpp
extern "C" int func_02037938(const void* p);

struct dBgCh_Actr
{
    char pad[0x20];
    int GetFloorResult() const;
};

int dBgCh_Actr::GetFloorResult() const
{
    return func_02037938((const char*)this + 0x20);
}
