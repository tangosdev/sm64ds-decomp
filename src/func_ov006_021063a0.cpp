//cpp
typedef unsigned char u8;
typedef unsigned int u32;

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))&0xFFFFFFFFFFFFFFFFLL))
#define RND ((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff)

struct O
{
  virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
  virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
  virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
  virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
  virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
  virtual void v32(); virtual void v33(); virtual void v34();
  virtual int m8c();
};

extern "C" {

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u8 data_ov006_0213dd64[];
extern u8 data_ov006_0213dd74[];
extern u8 data_ov006_0213dd94[];
extern u8 data_ov006_0213de28[];
extern u8 data_ov006_0213de44[];
extern u8 data_ov006_0213de60[];

#pragma opt_common_subs off
void func_ov006_021063a0(char *c)
{
  int t = *(int *)(c + 0xbc);
  if (((O *)c)->m8c() != 0)
  {
    int nv;
    if (t >= 0xf)
    {
      t = (RND * 0xf) >> 15;
      if (t == *(int *)(c + 0x4de4))
      {
        t += ((RND * 0xe) >> 15) + 1;
        if (t >= 0xf) t -= 0xf;
      }
    }
    if (data_ov006_0213dd64[t] != 0 || *(int *)(c + 0x4cb0) == 0)
    {
      if (*(int *)(c + 0x4cb0) == 0) *(int *)AT(c, 0x4cb0) += 1;
      nv = (RND * 8) >> 15;
      if (nv == *(int *)(c + 0x4cb4))
      {
        nv = (nv + (((RND * 7) >> 15) + 1)) & 7;
      }
      *(int *)(c + 0x4cb4) = nv;
    }
    *(int *)(c + 0x4cbc) = data_ov006_0213dd94[t];
    { int q = *(int *)(c + 0x4cbc); *(int *)(c + 0x4cb8) = q * q; }
    *(int *)(c + 0x4cc0) = data_ov006_0213dd74[t];
    *(int *)(c + 0x4de4) = t;
  }
  else
  {
    int nv;
    int k;
    u8 *p;
    while (t >= 0x19) t -= 0x19;
    if (data_ov006_0213de28[t] != 0 || *(int *)(c + 0x4cb0) == 0)
    {
      if (*(int *)(c + 0x4cb0) == 0) *(int *)AT(c, 0x4cb0) += 1;
      nv = (RND * 4) >> 15;
      k = data_ov006_0213de60[t];
      p = &data_ov006_0213de60[t];
      if (k == 5)
      {
        nv = ((RND * 5) >> 15) + 4;
      }
      if (nv == *(int *)(c + 0x4cb4))
      {
        if (*p == 4)
        {
          nv = (nv + (((RND * 3) >> 15) + 1)) & 3;
        }
        else
        {
          nv = nv + (((RND * 4) >> 15) + 1);
          if (nv >= 9) nv -= 5;
        }
      }
      *(int *)(c + 0x4cb4) = nv;
    }
    *(int *)(c + 0x4cbc) = data_ov006_0213de60[t];
    { int q = *(int *)(c + 0x4cbc); *(int *)(c + 0x4cb8) = q * q; }
    *(int *)(c + 0x4cc0) = data_ov006_0213de44[t];
  }
}

}
