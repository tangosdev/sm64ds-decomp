/* func_ov006_0211d75c at 0x0211d75c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */

extern void RenderOamMainScreen(void*, int, int, int, int);
extern void* data_ov006_0213a5f4;
void func_ov006_0211d75c(char* c){
  if(*(unsigned char*)(c+0x4000+0xbc9)==0) return;
  RenderOamMainScreen(data_ov006_0213a5f4,
                      *(int*)(c+0x4000+0xbc0)>>0xc,
                      *(int*)(c+0x4000+0xbc4)>>0xc,
                      -1, -1);
}
