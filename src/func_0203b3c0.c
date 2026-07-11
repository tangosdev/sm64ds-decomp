struct Node { int f0; int f4; struct Node* f8; struct Node* fc; };

int func_0203b3c0(int* list, struct Node* n)
{
    if(n == 0) goto fail;
    if(n->f4 != 0) return 0;
    if(n->f8 != 0){
        n->f8->fc = n->fc;
    }else{
        if(n->f0 != 0){
            *(int*)(n->f0 + 4) = (int)n->fc;
        }else{
            *list = 0;
        }
    }
    if(n->fc != 0){
        n->fc->f8 = n->f8;
    }
    n->f8 = 0;
    n->fc = 0;
    n->f0 = 0;
    goto success;
fail:
    return 0;
success:
    return 1;
}
