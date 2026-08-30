// @symbol func_ov074_021201f0
/* recovered: Goomboss (daKuriKing_c) state-0 tick -- the intro cutscene.
 *
 * Frames the boss for the opening conversation: pins the camera to a fixed
 * look-at/eye pair derived from the boss position, turns the closest player
 * around to face him (angle + 0x7fff), snaps the boss's own facing to
 * f_5f0 * 0x4500 + f_5f4, then runs the talk handshake. Player::StartTalk
 * and Player::ShowMessage each gate the rest of the tick, so a refused talk
 * simply retries next frame; on success the music layer flips once (guarded
 * by the f_60c latch) and the state advances to 1.
 *
 * Shape notes: the angle temp is a Vector3_16 STRUCT COPY, not three field
 * assignments -- mwccarm scalarizes the field form away and drops the
 * memory home the ROM keeps at sp+8. msgpos[0] is filled through a base
 * pointer (the ov073 idiom) while msgpos[1] uses direct offsets; that
 * difference is in the ROM, not an accident. msgpos[0] is dead, kept alive
 * only because &msgpos[1] homes the whole array.
 *
 * Matched byte-for-byte with mwccarm 2004/b56 (ov074); linkcheck VERIFIED.
 */
#include "common.h"

extern char* _ZN8dActor_c13ClosestPlayerEv(void* actor);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, struct Vector3* v);
extern int _ZN6Player9StartTalkER7fBase_cb(void* self, void* actor, int b);
extern void _ZN7Message11PrepareTalkEv(void);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int id);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* self, void* actor, unsigned int msg, const struct Vector3* pos, unsigned int a, unsigned int b);
extern void func_02012694(int a, void* p);
extern void func_ov074_021203e4(void* c, int i);
extern void* data_0209f318;

void func_ov074_021201f0(char* c) {
    struct Vector3_16 ang;
    struct Vector3 look, pos;
    struct Vector3 msgpos[2];
    char* player;
    void* cam;
    short msg;

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);

    look.x = *(int*)(c + 0x5c);
    look.y = *(int*)(c + 0x60);
    look.z = *(int*)(c + 0x64);
    pos.x = *(int*)(c + 0x5c);
    pos.y = *(int*)(c + 0x60);
    pos.z = *(int*)(c + 0x64);
    look.x += 0x130000;
    look.y = 0x1cd000;
    look.z -= 0x2a0000;
    pos.x -= 0x2f0000;
    pos.y += 0x20000;
    pos.z += 0x254000;
    _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
    _ZN6Camera6SetPosERK7Vector3(cam, &pos);

    ang = *(struct Vector3_16*)(c + 0x8c);
    ang.y += 0x7fff;
    *(short*)(player + 0x8c) = ang.x;
    *(short*)(player + 0x8e) = ang.y;
    *(short*)(player + 0x90) = ang.z;

    {
        int pv = (int)(player + 0x5c);
        msgpos[0].x = *(int*)pv;
        msgpos[0].y = *(int*)(pv + 4);
        msgpos[0].z = *(int*)(pv + 8);
    }
    msgpos[1].x = *(int*)(c + 0x5c);
    msgpos[1].y = *(int*)(c + 0x60);
    msgpos[1].z = *(int*)(c + 0x64);
    msgpos[1].y += 0x78000;

    *(short*)(c + 0x8e) = *(int*)(c + 0x5f0) * 0x4500 + *(short*)(c + 0x5f4);
    *(char**)(c + 0x5c8) = player;
    if (_ZN6Player9StartTalkER7fBase_cb(player, c, 1) == 0) return;
    _ZN7Message11PrepareTalkEv();
    if (*(unsigned char*)(c + 0x60c) == 0) {
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
        *(unsigned char*)(c + 0x60c) = 1;
    }
    msg = (unsigned short)(*(int*)(player + 8) + 0xd3);
    if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, c, msg, &msgpos[1], 0, 2) == 0) return;
    func_02012694(0x15a, c + 0x74);
    func_ov074_021203e4(c, 1);
}
