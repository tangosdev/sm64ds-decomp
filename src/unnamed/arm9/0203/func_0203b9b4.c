/* Seed a RandomIntInternal state word (its neighbour, the LCG
   seed * 0x19660d + 0x3c6ef35f). Every caller: daFish_c and dScMgCoin_c
   seed their mSeed with 1, as __sinit_02074dc4 and __sinit_02074e44 seed
   data_0209d4b8 and data_0209e650; d_a_ply_methods.cpp and func_020196cc seed
   data_ov002_0210e160 and data_0209d4b8 with 1234; and d_s_stage_methods.cpp
   reseeds data_0209e650 from the input record's frame counter
   (func_0203dad4). */
void func_0203b9b4(int *p, int v)
{
    p[0] = v;
}
