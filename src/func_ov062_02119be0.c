typedef struct Vec3 {
    int x, y, z;
} Vec3;

extern int Vec3_Dist(Vec3* a, Vec3* b);
extern short Vec3_HorzAngle(Vec3* a, Vec3* b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* player, void* actor, int flag);
extern int _ZN6Player12GetTalkStateEv(void* player);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(void* player, void* actor, unsigned int msg, Vec3* pos, unsigned int a, unsigned int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int idx, int speed, unsigned int flags);
extern void _ZN5Sound22StopLoadedMusic_Layer2Ev(void);
extern void func_0201277c(unsigned int id);
extern int _Z14ApproachLinearRsss(short* val, short target, short step);
extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(void* actor, signed char* flag, unsigned int id, Vec3* pos, unsigned int arg);

extern void* data_ov062_0211e03c[];
extern void* data_ov062_0211e034[];
extern signed char data_0209f2f8;

void func_ov062_02119be0(char* self)
{
    Vec3 playerPos;
    Vec3 msgPos;
    Vec3 starPos;
    unsigned int msg;
    unsigned short t;
    unsigned short* tp;
    int x;
    int y;
    int z;

    tp = (unsigned short*)(self + 0x100);
    t = *tp;
    if (t != 0) {
        t--;
        *tp = t;
    }

    switch (*(unsigned char*)(self + 0x390)) {
    case 0:
        if (*(unsigned short*)(self + 0x100) != 0)
            return;
        {
            Vec3* pp = (Vec3*)(((int)*(char**)(self + 0x398) + 0x5c));
            playerPos.x = pp->x;
            playerPos.y = pp->y;
            playerPos.z = pp->z;
        }
        if (Vec3_Dist((Vec3*)(self + 0x5c), &playerPos) >= 0x190000)
            return;
        if (_ZN6Player9StartTalkER9ActorBaseb(*(void**)(self + 0x398), self, 1) == 0)
            return;
        *(short*)(self + 0x3a8) = Vec3_HorzAngle((Vec3*)(self + 0x5c), &playerPos);
        (*(unsigned char*)(((int)self + 0x390)))++;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, data_ov062_0211e03c[1], 0, 0x1000, 0);
        if (*(unsigned char*)(self + 0x3b4) == 0) {
            _ZN5Sound22StopLoadedMusic_Layer2Ev();
            func_0201277c(0x4d);
            *(unsigned char*)(self + 0x3b5) = 0;
            *(unsigned char*)(self + 0x3b4) = 1;
        }
        *(unsigned char*)(self + 0x3b3) = 0;
        return;
    case 1:
        if (_Z14ApproachLinearRsss((short*)(self + 0x94), *(short*)(self + 0x3a8), 0x800) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        *(short*)(self + 0x8e) = *(short*)(self + 0x94);
        return;
    case 2:
        if (*(int*)(*(char**)(self + 0x398) + 8) == 0) {
            if (*(unsigned char*)(self + 0x3af) == 0) {
                msg = 0x14d;
            } else if (*(unsigned char*)(self + 0x3ac) != 0) {
                *(unsigned char*)(self + 0x3af) = 0;
                msg = 0x14c;
            } else if (data_0209f2f8 == 0x18) {
                msg = 0xc5;
            } else {
                msg = 0x92;
            }
        } else {
            *(unsigned char*)(self + 0x3af) = 0;
            *(unsigned char*)(self + 0x3b3) = 1;
            msg = 0x9f;
        }
        x = _ZN6Player12GetTalkStateEv(*(void**)(self + 0x398));
        if (x != 0)
            return;
        x = *(int*)(self + 0x5c);
        z = *(int*)(self + 0x64);
        y = *(int*)(self + 0x60) + 0xc8000;
        msgPos.x = x;
        msgPos.y = y;
        msgPos.z = z;
        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(*(void**)(self + 0x398), self, msg, &msgPos, 0, 0) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        return;
    case 3:
        if (_ZN6Player12GetTalkStateEv(*(void**)(self + 0x398)) != 0xFFFFFFFF)
            return;
        (*(unsigned char*)(((int)self + 0x390)))++;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, data_ov062_0211e034[1], 0, 0x1000, 0);
        *(unsigned short*)(self + 0x100) = 0x3c;
        if (*(unsigned char*)(self + 0x3af) != 0) {
            starPos.x = *(int*)(self + 0x5c);
            starPos.y = *(int*)(self + 0x60);
            starPos.z = *(int*)(self + 0x64);
            starPos.y += 0x64000;
            _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(self, (signed char*)(self + 0x3b0), *(unsigned char*)(self + 0x3b1), &starPos, 4);
            return;
        }
        if (*(unsigned char*)(self + 0x3b3) == 1) {
            *(unsigned char*)(self + 0x3b3) = 0;
            *(unsigned char*)(self + 0x390) = 0;
        }
        return;
    case 4:
        {
            Vec3* pp = (Vec3*)(((int)*(char**)(self + 0x398) + 0x5c));
            playerPos.x = pp->x;
            playerPos.y = pp->y;
            playerPos.z = pp->z;
        }
        if (Vec3_Dist((Vec3*)(self + 0x5c), &playerPos) >= 0x190000)
            return;
        if (_ZN6Player9StartTalkER9ActorBaseb(*(void**)(self + 0x398), self, 0) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        return;
    case 5:
        if (_ZN6Player12GetTalkStateEv(*(void**)(self + 0x398)) != 0)
            return;
        *(short*)(self + 0x3a8) = Vec3_HorzAngle((Vec3*)(self + 0x5c), (Vec3*)(*(char**)(self + 0x398) + 0x5c));
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, data_ov062_0211e03c[1], 0, 0x1000, 0);
        (*(unsigned char*)(((int)self + 0x390)))++;
        return;
    case 6:
        if (_Z14ApproachLinearRsss((short*)(self + 0x94), *(short*)(self + 0x3a8), 0x800) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        *(short*)(self + 0x8e) = *(short*)(self + 0x94);
        return;
    case 7:
        {
            char* p = *(char**)(self + 0x398);
            if (*(int*)(p + 8) == 0)
                msg = 0xa0;
            else
                msg = 0xa1;
            x = _ZN6Player12GetTalkStateEv(p);
            if (x != 0)
                return;
        }
        x = *(int*)(self + 0x5c);
        z = *(int*)(self + 0x64);
        y = *(int*)(self + 0x60) + 0xc8000;
        msgPos.x = x;
        msgPos.y = y;
        msgPos.z = z;
        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(*(void**)(self + 0x398), self, msg, &msgPos, 0, 0) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        return;
    case 8:
        if (_ZN6Player12GetTalkStateEv(*(void**)(self + 0x398)) != 0xFFFFFFFF)
            return;
        *(unsigned char*)(self + 0x390) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, data_ov062_0211e034[1], 0, 0x1000, 0);
        return;
    }
}
