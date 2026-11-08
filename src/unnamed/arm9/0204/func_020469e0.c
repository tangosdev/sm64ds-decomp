/* The word at +0x38 of a model file. TrimModelFile reallocates the file
   down to that size once it is set up (SetLoadedFile), so it is the part
   the file keeps. */
int func_020469e0(int *p)
{
    return p[14];
}
