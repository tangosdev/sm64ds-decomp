struct N { char p0[0xc]; unsigned short h; char p1[0x390-0xe]; struct N *next; };
struct N *func_ov096_021357b4(struct N *c){
    struct N *p = c->next;
    if(p==0) return c;
    while(p){
        unsigned r2 = (p->h != 0xf0) ? 1u : 0u;
        if(!r2) return p;
        p = p->next;
    }
    return 0;
}
