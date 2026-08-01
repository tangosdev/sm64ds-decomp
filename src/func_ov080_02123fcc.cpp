//cpp
/* func_ov080_02123fcc at 0x02123fcc (ov080), size 0xbc
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
struct Actor;
extern "C" Actor* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, Actor* after);

extern "C" void func_ov080_02123fcc(char* thiz)
{
    char* c = thiz;
    {
        int* p150 = (int*)(((int)c + 0x150));
        int* pb0 = (int*)(((int)c + 0xb0));
        *p150 = *p150 | 1;
        *pb0 = *pb0 & ~0x10000000;
    }
    if (*(unsigned char*)(c + 0x180) != 0) {
        Actor* a = 0;
        while (1) {
            a = _ZN5Actor15FindWithActorIDEjPS_(0x136, a);
            if (a == 0) break;
            if (a != (Actor*)c) {
                if (*(unsigned char*)(c + 0x182) == *(unsigned char*)((char*)a + 0x182)) {
                    *(int*)(c + *(unsigned char*)(c + 0x183) * 4 + 0x16c) = *(int*)((char*)a + 4);
                    (*(unsigned char*)(c + 0x183))++;
                    if (*(unsigned char*)(c + 0x183) == 4) break;
                }
            }
        }
        *(int*)(c + 0x17c) = 1;
        return;
    }
    *(int*)(c + 0x17c) = 1;
}
