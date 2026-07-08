extern int ACTIVE_MESH_COLLIDERS[];
void _ZN5Stage18ResetMeshCollidersEv(void){
    int i;
    for(i=0;i<0x18;i++) ACTIVE_MESH_COLLIDERS[i]=0;
}
