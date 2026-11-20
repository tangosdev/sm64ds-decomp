/* Empty: the per-particle texture bind SPLManager_DoDraw uses when the
   definition does not animate its texture. The name is pret's SPL's, for
   the same role (SPLManager_DrawParticles' `hasTexAnim ? SPLUtil_SetTexture
   : SPLUtil_SetTexture_Stub`). */
void SPLUtil_SetTexture_Stub(void *texture) {}
