//cpp
/* func_ov006_020c87d0 at 0x020c87d0 (ov006), size 0x16c
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef struct { int w[2]; } SharedFilePtr;
typedef unsigned short u16;

extern "C" {

extern SharedFilePtr data_ov006_02140450;
extern SharedFilePtr data_ov006_02140460;
extern SharedFilePtr data_ov006_02140468;
extern SharedFilePtr data_ov006_02140458;
extern SharedFilePtr data_ov006_02140438;
extern SharedFilePtr data_ov006_02140440;
extern SharedFilePtr data_ov006_02140448;

extern void* data_ov006_02140430;
extern void* data_ov006_0214040c;
extern void* data_ov006_0214041c;
extern void* data_ov006_02140424;
extern void* data_ov006_02140408;
extern void* data_ov006_0214042c;

extern char* data_ov006_02141a40;
extern void* ROOT_ACTOR_BASE;

extern int func_020179b4(SharedFilePtr* f, void* model, int a);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void func_ov006_020bfec0(char* p, void* q, short* s);
extern void func_02016a14(void* self, int a);
extern void func_02016a04(void* self, int a);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* file, int a, int b, unsigned c);
extern void func_ov006_020c8658(void* c);

int func_ov006_020c87d0(char* c)
{
    int t;

    if (func_020179b4(&data_ov006_02140450, c + 0x4c, 1) == 0)
        return 0;

    data_ov006_02140430 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140460);
    data_ov006_0214040c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140468);
    data_ov006_0214041c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140458);
    data_ov006_02140424 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140438);
    data_ov006_02140408 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140440);
    data_ov006_0214042c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140448);

    *(int*)(c + 0x44) = 0;
    if (data_ov006_02141a40 != 0)
        func_ov006_020bfec0(data_ov006_02141a40, c + 0x14, (short*)(c + 0x36));

    t = *(u16*)((char*)ROOT_ACTOR_BASE + 0xc) == 0x175;
    if (t != false) {
        func_02016a14(c + 0x4c, 0x7fff);
        func_02016a04(c + 0x4c, 0x210);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0x4c, data_ov006_02140430, 0x40000000, 0x800, 0);

    func_ov006_020c8658(c);
    return 1;
}

}
