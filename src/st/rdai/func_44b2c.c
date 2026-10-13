// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/func_44b2c", func_us_801C4B2C);
#else
// The Black Panther's eight slices: moves the joint between each slice and
// the next toward the middle of the pair, so the stack bends smoothly
void func_us_801C4B2C(void) {
    Primitive* prim;
    Primitive* next;
    s32 x, y;
    s32 i;

    prim = g_CurrentEntity->ext.blackPanther.prim;
    for (i = 0; i < 7; i++) {
        next = prim->next;
        x = (next->x2 + prim->x0) / 2;
        y = (next->y2 + prim->y0) / 2;
        prim->x2 = next->x0 = (prim->x2 - x) / 8 + x;
        prim->y2 = next->y0 = (prim->y2 - y) / 8 + y;
        x = (next->x3 + prim->x1) / 2;
        y = (next->y3 + prim->y1) / 2;
        prim->x3 = next->x1 = (prim->x3 - x) / 8 + x;
        prim->y3 = next->y1 = (prim->y3 - y) / 8 + y;
        prim = next;
    }
}
#endif
