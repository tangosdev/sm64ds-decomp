struct OSThread;

/* NitroSDK OSMutex, the fields set here. This build's thread queue is a
 * u16 (OSThread's joinQueue in func_02058200.cpp), and func_02058b08 and
 * func_02058a94 (OS_LockMutex / OS_UnlockMutex) read the owner at +0x04 and
 * the lock count at +0x08. */
typedef struct OSMutex {
    unsigned short queue;    /* 0x00 -- threads waiting for the mutex */
    struct OSThread *thread; /* 0x04 -- the owner, 0 while free */
    int count;               /* 0x08 -- times the owner has locked it */
} OSMutex;

/* NitroSDK OS_InitMutex(mutex). The parameter keeps its char * spelling:
 * func_0205fde8's declaration is checked against it. */
void func_02058b9c(char *p)
{
    OSMutex *mutex = (OSMutex *)p;
    mutex->queue = 0;
    mutex->thread = 0;
    mutex->count = 0;
}
