//cpp
// @symbol _ZN14UnknownVsEntry13InitResourcesEv
#include "decl_common.h"
#include "UnknownVsEntry.h"
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* bca, int a, int fx, unsigned int f);
extern void func_ov075_021152d4(char* self);
}

extern char data_ov075_0211d3fc;
extern char data_ov075_0211d3bc;
extern char data_ov075_0211d3e4;
extern char data_ov075_0211d404;
extern char data_ov075_0211d3c4;
extern char data_ov075_0211d414;
extern char data_ov075_0211d394;
extern char data_ov075_0211d3cc;
extern char data_ov075_0211d39c;
extern char data_ov075_0211d3d4;
extern char data_ov075_0211d3a4;
extern char data_ov075_0211d3ec;
extern char data_ov075_0211d384;
extern char data_ov075_0211d424;
extern char data_ov075_0211d42c;
extern char data_ov075_0211d41c;
extern char data_ov075_0211d3ac;
extern char data_ov075_0211d3b4;
extern char data_ov075_0211d3f4;
extern char data_ov075_0211d38c;
extern char data_ov075_0211d3dc;
extern char data_ov075_0211d40c;
extern char data_020a0e68;

struct M48 { int w[12]; };

int UnknownVsEntry::InitResources()
{
    int i; int kind; UnknownVsPlayer* player;

    InitialiseVramGlobals();
    Model::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3fc);
    if (param1 != 1) {
        Model::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3bc);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3e4);
    }
    Model::LoadFile(*(SharedFilePtr*)&data_ov075_0211d404);
    Model::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3c4);
    Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d414);

    if (param1 != 1) {
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d394);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3cc);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d39c);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3d4);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3a4);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3ec);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d384);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d424);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d42c);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d41c);
    } else {
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3ac);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3b4);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3f4);
        Animation::LoadFile(*(SharedFilePtr*)&data_ov075_0211d38c);
        TextureSequence::LoadFile(*(SharedFilePtr*)&data_ov075_0211d3dc);
    }

    TextureSequence::LoadFile(*(SharedFilePtr*)&data_ov075_0211d40c);

    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    ShadowModel::CleanAll();

    mModel.SetFile(*(BMD_File**)(&data_ov075_0211d3fc + 4), 1, -1);

    func_0203c178(&data_020a0e68, 0x7d000, 0x7d000, 0x7d000);
    /* 0x888 is +0x1c inside the Model at 0x86c -- its mat4x3. The cartridge's own
       ~UnknownVsEntry proves the extent; see tools/dtor_members.py. */
    *(struct M48*)((char*)&mModel.mat4x3) = *(struct M48*)&data_020a0e68;

    if (param1 != 1) {
        mModelAnim.SetFile(*(BMD_File**)(&data_ov075_0211d3bc + 4), 1, -1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0x8bc, *(void**)(&data_ov075_0211d3e4 + 4), 0, 0x1000, 0);
    }

    func_ov075_0211b458(((char*)this) + 0xe80, (int*)&data_ov075_0211c654, 0);
    mParticles.Initialise();

    player = mPlayers;
    i = 0;
    do {
        kind = param1;
        int r = func_ov075_02115290(((char*)this), i);
        if (!func_ov075_02114ddc((char*)player, kind, i, r))
            return 0;
        i++;
        player++;
    } while (i < 4);

    data_ov075_0211d380 = -1;
    mAnimActive = 0;

    if (param1 == 2) {
        int v = func_0203da9c();
        func_ov075_02115098(((char*)this), v);
    }

    mCamTargetX = 0;
    mCamPosX = mCamTargetX;
    mCamTargetY = 0x14000;
    mCamPosY = mCamTargetY;
    mCamPosZ = 0x50000;
    mCamTargetZ = -0x8000;

    func_ov075_021152d4(((char*)this));

    mSuspended = 0;
    mState = 0;
    mFocusedPlayer = 0;
    mPlayerCount = data_0209fc50;
    if (mPlayerCount < 1)
        mPlayerCount = 1;

    return 1;
}
