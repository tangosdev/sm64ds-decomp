// GATE 32: the faces whose class IS in include/, in a TU that includes nothing
// else.
//
// The split is forced rather than tidy: hal/bob_enemy_bridges.cpp has already
// spelled shadow Actor, Enemy, Player, ModelBase and ShadowModel, so including
// include/Player.h there would redefine every one of them. See the note at the
// foot of that file.
#include "Player.h"

extern "C" {
/* Both are reached from ov084 by their Itanium C names while their definitions
   are real methods: Goomba's death branch asks whether the player is riding a
   shell, and its collect path registers the coin against the egg count. */
int _ZN6Player9IsOnShellEv(void *self)
{ return ((Player *)self)->Player::IsOnShell(); }
void _ZN6Player20RegisterEggCoinCountEjbb(void *self, unsigned n, int b2, int b3)
{ ((Player *)self)->Player::RegisterEggCoinCount(n, b2 != 0, b3 != 0); }
/* Player::Unk_020ca150 (ov002 0x020ca150), the state change the CHAIN_CHOMP's
   lunge asks for when it catches the player. Same shape: an Itanium C-named
   reference onto a definition that is a real method. */
int _ZN6Player12Unk_020ca150Eh(void *self, unsigned char a)
{ return ((Player *)self)->Player::Unk_020ca150(a); }
/* Player::Unk_020c4f40 (ov002 0x020c4f40), the state KOOPA_THE_QUICK puts the
   player into when he starts the race. */
int _ZN6Player12Unk_020c4f40Et(void *self, unsigned short a)
{ return ((Player *)self)->Player::Unk_020c4f40(a); }
}

/* Actor::GetSubtraction (arm9 0x0200f8d4), the absolute angle difference two
   of the koopa's states run. Its definition is a real method; his own TUs
   spell it by the Itanium C name. */
#include "dActor_c.h"
extern "C" int _ZN8dActor_c14GetSubtractionEss(void *self, short a, short b)
{ return ((dActor_c *)self)->dActor_c::GetSubtraction(a, b); }

/* KING_BOB_OMB's four. Model::Render (arm9 0x02016d20), BlendModelAnim's
   Advance and SetAnim (0x02016468 and 0x020163e0) and Player::Unk_020c6a10
   (ov002 0x020c6a10) are all real methods in src/ while his own TUs reach
   them by their Itanium C names. */
#include "Model.h"
#include "BlendModelAnim.h"
extern "C" {
/* Model::Render is faced in hal/actor_faces_bob.cpp, which gate 33 reached
   first through the CAP. Both gates wrote the same face and the two
   collided at link; this one keeps the reference only. */
void _ZN14BlendModelAnim7AdvanceEv(void *self)
{ ((BlendModelAnim *)self)->BlendModelAnim::Advance(); }
int _ZN6Player12Unk_020c6a10Ej(void *self, unsigned a)
{ return ((Player *)self)->Player::Unk_020c6a10(a); }
}

/* CommonModel::DoSetFile (arm9 0x02016144), which is the whole of that class's
   ROM vtable past its two destructors, and the two Sound layer-3 methods the
   king's boss music starts and stops. Same shape as the rest of this file. */
#include "CommonModel.h"
extern "C" int _ZN11CommonModel9DoSetFileEPcii(void *self, char *f, int a,
                                               int b)
{ return ((CommonModel *)self)->CommonModel::DoSetFile(f, a, b); }


/* Model::HideMaterial (arm9 0x02016a58), which KOOPA_THE_QUICK's Render calls
   through its own shadow while the definition is a real method. */
#include "Model.h"
extern "C" void _ZN5Model12HideMaterialEii(void *self, int boneID, int listIdx)
{ ((Model *)self)->Model::HideMaterial(boneID, listIdx); }
/* Model::ShowMaterial (arm9, the HideMaterial sibling), gate 192:
   MrBlizzard's own Render (src/_ZN10MrBlizzard6RenderEv.cpp) calls it
   through decl_Model.h's flat declaration while the matched definition
   (src/_ZN5Model12ShowMaterialEii.cpp, gate 182) is a real method -- the
   same seam HideMaterial already needed. */
extern "C" void _ZN5Model12ShowMaterialEii(void *self, int boneID, int listIdx)
{ ((Model *)self)->Model::ShowMaterial(boneID, listIdx); }
