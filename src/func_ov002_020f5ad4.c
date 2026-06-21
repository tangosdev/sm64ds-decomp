/* func_ov002_020f5ad4 at 0x020f5ad4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_ov002_020f5a6c(unsigned char* p);
extern void func_ov002_020f396c(char* p);
extern void func_ov002_020f30d4(char* p);
extern void func_ov002_020f2ea0(char* p);
extern void func_ov002_020f2a68(char* p);
extern void func_ov002_020f2984(char* p);

void func_ov002_020f5ad4(char* self)
{
    *(int*)(self + 0x1f0) = 0;
    *(int*)(self + 0x1f4) = 0;
    *(int*)(self + 0x1f8) = 0;
    *(unsigned char*)(self + 0x514) = 0;
    func_ov002_020f5a6c((unsigned char*)self);
    func_ov002_020f396c(self);
    func_ov002_020f30d4(self);
    func_ov002_020f2ea0(self);
    func_ov002_020f2a68(self);
    func_ov002_020f2984(self);
}
