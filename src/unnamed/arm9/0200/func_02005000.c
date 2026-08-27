#include "types.h"
extern s32 ApproachAngle(s16 *target, s16 from, s16 start, s16 speed, s16 max);
extern void func_02009e70(void *self);
typedef struct CameraDef CameraDef;
extern CameraDef data_020874cc; /* at 0x020874cc */
extern CameraDef data_020874f4; /* at 0x020874f4 */
typedef struct Camera Camera;
struct Camera {
    char _pad0[0x13c];
    CameraDef *defaultCamDef; /* 0x13c */
    char _pad1[0x184 - 0x140];
    s16  field_184;           /* 0x184 */
};
void func_02005000(Camera *self)
{
    if (self->defaultCamDef == &data_020874cc)
    {
        if (!ApproachAngle(&self->field_184, 0, 8, 0x2000, 0x400))
            self->defaultCamDef = &data_020874f4;
    }
    func_02009e70(self);
}
