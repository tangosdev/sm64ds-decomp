//cpp
// @symbol _ZN14BlendModelAnim9DoSetFileEPcii
#include "BlendModelAnim.h"
extern "C" int func_020165c4(BlendModelAnim *self, char *file);

int BlendModelAnim::DoSetFile(char *file, int a, int b)
{
    char *f = file;
    int r = Model::DoSetFile(file, a, b);
    if (r == 0)
        return 0;
    return func_020165c4(this, f);
}
