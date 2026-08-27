int func_02007414(void *c)
{
    /* zero some fields */
    *(int *)((char *)c + 0xb0) = 0;
    *(int *)((char *)c + 0xb4) = 0;
    *(int *)((char *)c + 0xb8) = 0;

    /* load pointers/values */
    int *p1 = *(int **)((char *)c + 0x110);   /* r1 = [r0+0x110] */
    int r2 = *(int *)((char *)c + 0x84);      /* r2 = [r0+0x84] */
    int r1 = *(int *)((char *)p1 + 0x60);    /* r1 = [r1+0x60] */

    /* store more zeros */
    *(int *)((char *)c + 0xbc) = 0;

    /* compute and store result */
    r1 = r2 - r1;                              /* r1 = r2 - r1 */
    *(int *)((char *)c + 0xc0) = r1;           /* [r0+0xc0] = r1 */

    *(int *)((char *)c + 0xc4) = 0;
    *(int *)((char *)c + 0x170) = 0;
    *(int *)((char *)c + 0x174) = 0;

    return 1;
}
