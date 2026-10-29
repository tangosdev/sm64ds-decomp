struct Node5b1a4;

/* NitroSDK SND_CommandInit. SNDCommand is the SDK's command record: the
   pool is the 256 records at data_020a6760 (sCommandArray), threaded onto the free list
   data_020a6484; func_0205b554 is SNDi_InitSharedWork. The first command
   sent is id 0x19 (SND_COMMAND_SHARED_WORK) with the shared work block as
   arg[0]. */
typedef struct SNDCommand {
    struct SNDCommand *next; /* 0x00 */
    int id;                  /* 0x04 */
    unsigned int arg[4];     /* 0x08 */
} SNDCommand;

/* The other files declare these as int / void *; reach them through casts
   rather than retyping the externs. */
extern int data_020a6760;
extern void *data_020a6484;
#define COMMAND_ARRAY ((SNDCommand *)&data_020a6760)
extern unsigned char data_020a7760[];
extern int data_020a648c;
extern int data_020a6490;
extern int data_020a64a0;
extern int data_020a6498;
extern int data_020a649c;
extern int data_020a6488;
extern void *data_020a7f48;
extern void *data_020a6494;
extern int data_020a64a4;
extern int data_020a64e0;
extern void *data_020a7fc0;

void func_0205ae64(void);
void func_0205b554(char *p);
void *func_0205b1d8(int arg);
void func_0205b1a4(struct Node5b1a4 *node);
void func_0205b070(int arg);

void func_0205b358(void)
{
    int i;
    SNDCommand *p;

    func_0205ae64();

    data_020a6484 = COMMAND_ARRAY;
    for (i = 0; i < 0xff; i++)
        COMMAND_ARRAY[i].next = &COMMAND_ARRAY[i + 1];

    /* COMMAND_ARRAY[255].next (the free list's tail): the ROM reaches it
       through the data_020a7760 label 0x1000 into the pool. */
    *(int *)(data_020a7760 + 0x7e8) = 0;
    data_020a6494 = &data_020a7f48;
    data_020a648c = 0;
    data_020a6490 = 0;
    data_020a64a0 = 0;
    data_020a6498 = 0;
    data_020a649c = 0;
    data_020a64a4 = 1;
    data_020a6488 = 0;
    data_020a7fc0 = &data_020a64e0;

    func_0205b554((char *)&data_020a64e0);

    p = (SNDCommand *)func_0205b1d8(1);
    if (p == 0)
        return;

    p->id = 0x19;
    p->arg[0] = (unsigned int)data_020a7fc0;
    func_0205b1a4((struct Node5b1a4 *)p);
    func_0205b070(1);
}
