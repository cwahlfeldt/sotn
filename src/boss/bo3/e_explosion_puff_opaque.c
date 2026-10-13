// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo3.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/e_explosion_puff_opaque", CreateExplosionPuff);
#else
// As in e_explosion_puff_opaque.h, but BO3 has no puff entity: it spawns
// E_DUMMY_28
void CreateExplosionPuff(void) {
    Entity* puff;
    s32 rand3 = Random() & 3;
    s16 initAngle = ((Random() & 0xF) << 8) - ROT(180);
    s32 i;

    for (i = 0; i < 6; i++) {
        puff = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (puff != NULL) {
            CreateEntityFromEntity(E_DUMMY_28, g_CurrentEntity, puff);
            puff->params = 2;
            puff->ext.opaquePuff.speed = 6 - i;
            puff->ext.opaquePuff.angle = initAngle;
            puff->ext.opaquePuff.puffStyle = rand3;
        }
    }
}
#endif
