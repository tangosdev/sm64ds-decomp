extern void func_02072168(void *a, void *b, int d);
extern void func_02071790(void *a, void *b, int x);

/* File-local views of CodeWarrior's exception runtime, with the names
 * func_02072168.c gives them. CatchInfo's last three fields and the
 * ExSpecification fields follow the runtime's own CatchInfo and
 * ex_specification records, not a reader in this image (medium). */
typedef struct ThrowContext {
    int mThrowType; /* 0x00 -- type string */
    int mLocation;  /* 0x04 -- thrown object */
    int mDtor;      /* 0x08 -- thrown object's destructor */
    char pad_0c[0xc];
    char *mFP; /* 0x18 -- base for frame-relative operands */
} ThrowContext;

typedef struct ActionIterator {
    int mFuncStart; /* 0x00 -- start of the function the actions cover */
} ActionIterator;

/* The catch record in the handler's frame. func_02071ccc fills the first
 * five words for a catch; mStackTop is the runtime's stacktop. */
typedef struct CatchInfo {
    int mLocation;    /* 0x00 */
    int mTypeInfo;    /* 0x04 */
    int mDtor;        /* 0x08 */
    int mSublocation; /* 0x0c */
    int mPointerCopy; /* 0x10 */
    int mStackTop;    /* 0x14 */
} CatchInfo;

/* An exception-specification action as func_02071d3c decodes it (case 15):
 * the allowed-type count and list func_02071f88 checks, the unexpected
 * handler's offset from the function start, and the frame offset of its
 * CatchInfo. */
typedef struct ExSpecification {
    unsigned int mSpecCount; /* 0x00 */
    int mPCOffset;           /* 0x04 */
    int mCatchInfoRef;       /* 0x08 */
    unsigned char *mSpecs;   /* 0x0c */
} ExSpecification;

/* The thrown type broke an exception specification (CodeWarrior's
 * unexpected-handler path, medium): unwind to the specification's action
 * (func_02072168), hand the exception to the handler's CatchInfo and jump to
 * the handler (func_02071790). The parameters keep their spellings:
 * func_02071d3c's declaration is checked against them. */
void func_02071f1c(char *a, void *b, char *c, int d)
{
    ThrowContext *context = (ThrowContext *)a;
    ExSpecification *spec = (ExSpecification *)c;
    CatchInfo *catchInfo;
    int off;

    func_02072168(a, b, d);

    off = spec->mCatchInfoRef;
    catchInfo = (CatchInfo *)(context->mFP + off);
    catchInfo->mLocation = context->mLocation;
    catchInfo->mTypeInfo = context->mThrowType;
    catchInfo->mDtor = context->mDtor;
    catchInfo->mStackTop = d;

    func_02071790(a, b, ((ActionIterator *)b)->mFuncStart + spec->mPCOffset);
}
