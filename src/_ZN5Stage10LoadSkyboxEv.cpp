//cpp
/* Stage::LoadSkybox() at 0x0202b0fc, 0x68 bytes.
 *
 * Asks the level record which skybox to use, and if there is one, allocates a
 * Model, constructs it and loads the file for that ID.
 *
 * This file used to declare its own `struct Stage { char pad[0x9bc]; Model* }`
 * and its own `struct Model { char data[0x50]; }` -- two shadow types that
 * between them encoded the entire layout claim this slice moves into
 * include/Stage.h. It is the reason `skyboxModel` is typed there: the store here
 * and the virtual destroy in CleanupResources are the two ends of the evidence.
 *
 * The `new` is spelled as the raw _Znwj call rather than as `new Model` because
 * the ROM calls operator new and then guards the constructor on a null return,
 * which the language's own `new` expression does not emit.
 */
#include "Stage.h"
#include "Model.h"

extern "C" {
extern void *_Znwj(u32 size);
extern void *_ZN5ModelC1Ev(void *thiz);
extern void _ZN5Model14LoadAndSetFileEtii(void *thiz, u16 fileID, s32 a, s32 b);
extern u16 data_02075620[];
}

void Stage::LoadSkybox()
{
    u32 skyboxID = GetSkyboxID();
    if (!skyboxID) return;
    Model *model = (Model *)_Znwj(0x50);
    if (model != 0) {
        model = (Model *)_ZN5ModelC1Ev(model);
    }
    skyboxModel = model;
    u16 fileID = data_02075620[skyboxID - 1];
    _ZN5Model14LoadAndSetFileEtii(skyboxModel, fileID, 0, 2);
}
