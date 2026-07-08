extern int func_02053e34(void);
extern int func_02053e1c(void);
extern int VRAM_Tex_End;
extern int VRAM_Tex_Size;
extern int VRAM_Tex4x4_TexelData_Start;
extern int VRAM_Tex4x4_IndexData_Start;
extern int VRAM_Tex4x4_MaxSize;
extern int VRAM_Pal_End;
extern int VRAM_Pal_Start;

void InitialiseVramGlobals(void)
{
    int r = func_02053e34();
    VRAM_Tex_Size = 0;
    VRAM_Tex4x4_TexelData_Start = 0;
    VRAM_Tex4x4_IndexData_Start = 0x20000;
    VRAM_Tex4x4_MaxSize = 0x20000;
    VRAM_Tex_End = r;
    VRAM_Pal_End = func_02053e1c();
    VRAM_Pal_Start = 0;
}
