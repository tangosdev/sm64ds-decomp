//cpp
#include "Model.h"

extern "C" int func_02017060(BMD_File *f);

struct Thing {
    virtual void v0();
    virtual void v1();
    virtual int v2(BMD_File *f, int c, int d);
};

extern "C" int func_02016ff4(Thing *self, BMD_File *file, int c, int d)
{
    int ret;
    if (!file) return 0;
    Model::UpdateFileOffsets(*file);
    ret = self->v2(file, c, d);
    if (ret) func_02017060(file);
    return ret;
}
