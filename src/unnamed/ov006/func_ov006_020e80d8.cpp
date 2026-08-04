//cpp
struct SharedFilePtr { void Release(); };
struct TextureSequence { ~TextureSequence(); };
struct ModelAnim { ~ModelAnim(); };
extern "C" void* func_ov006_020e7a50(char* c);

extern SharedFilePtr data_ov006_02141e54;
extern SharedFilePtr data_ov006_02141e5c;
extern SharedFilePtr data_ov006_02141e7c;
extern SharedFilePtr data_ov006_02141e8c;
extern SharedFilePtr data_ov006_02141e84;
extern SharedFilePtr data_ov006_02141e64;

extern "C" char* func_ov006_020e80d8(char* c) {
    data_ov006_02141e54.Release();
    data_ov006_02141e5c.Release();
    data_ov006_02141e7c.Release();
    data_ov006_02141e8c.Release();
    data_ov006_02141e84.Release();
    data_ov006_02141e64.Release();
    func_ov006_020e7a50(c + 0x84);
    ((TextureSequence*)(c + 0x70))->~TextureSequence();
    ((ModelAnim*)(c + 0xc))->~ModelAnim();
    return c;
}
