// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo2.h"
#include <cutscene.h>

extern Dialogue g_Dialogue;
extern const char* actor_names[];

#include "../../st/cutscene_unk1.h"

#include "../../st/set_cutscene_script.h"

#include "../../st/cutscene_unk3.h"

#include "../../st/cutscene_unk4.h"

#include "../../st/cutscene_actor_name.h"

#include "../../st/set_cutscene_events.h"

#include "../../st/cutscene_events.h"

#include "../../st/cutscene_skip.h"

#include "../../st/cutscene_scale_avatar.h"

INCLUDE_RODATA("boss/bo2/nonmatchings/e_cutscene_dialogue", D_us_801A1F6C);

INCLUDE_RODATA("boss/bo2/nonmatchings/e_cutscene_dialogue", D_us_801A1F78);

extern u8 D_us_80180B7C[];
extern u8 D_us_80180B80[];
extern u16 D_us_80180B84[];
extern u16 D_us_80180B88[];
extern u16 D_us_80180B8C[];
extern s16 D_us_80180B90[];
extern s32 D_us_80180B78;
extern s32 D_us_801B68F0;
extern u8 D_us_80181CCC[];
extern u8 D_us_80181DC5[];
extern u32 g_CutsceneFlags;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/e_cutscene_dialogue", EntityCutsceneDialogue);
#else

// The conversation with Richter before the fight (params 0) and after it
// (params 1)
void EntityCutsceneDialogue(Entity* self) {
    const int PrimCount = 72;
    Primitive* prim;
    RECT rect;
    s32 primIndex;
    s32 i;
    s16 j;
    s16 x;
    s16 y;
    u16 nextChar;
    s32 nextChar2;
    u_long ptr;
    u8* script;

    if (self->step) {
        if (D_us_801B68F0 && !g_SkipCutscene) {
            if (g_GameClearFlag) {
                CutsceneSkip(self);
            } else if (self->params ? (g_Settings.D_8003CB04 & 0x4000)
                                    : (g_Settings.D_8003CB04 & 0x10)) {
                CutsceneSkip(self);
            }
        }
        if (self->step && g_Dialogue.hasEvents) {
            RunCutsceneEvents();
        }
    }
    switch (self->step) {
    case 0:
        if (self->params) {
            if (g_CastleFlags[RICHTER_CS_AFTER_M_AND_W] ||
                g_PlayableCharacter != PLAYER_ALUCARD ||
                g_DemoMode != Demo_None) {
                DestroyEntity(self);
                return;
            }
            script = D_us_80181DC5;
        } else {
            if (g_CastleFlags[RICHTER_CS_AFTER_M_AND_W] || D_us_80180B78 ||
                g_PlayableCharacter != PLAYER_ALUCARD ||
                g_DemoMode != Demo_None) {
                DestroyEntity(self);
                return;
            }
            script = D_us_80181CCC;
        }
        if (SetCutsceneScript(script) & 0xFF) {
            g_CutsceneFlags = 0;
            D_us_801B68F0 = 0;
            g_SkipCutscene = 0;
            g_CutsceneHasControl = 1;
            self->flags |= FLAG_HAS_PRIMS | FLAG_UNK_2000;
            self->step++;
            self->primIndex = g_Dialogue.primIndex[2];
        }
        break;

    case 1:
        // this is a huge While-loop! Don't miss it!
        while (1) {
            if ((g_Dialogue.nextCharTimer != 0) && !g_SkipCutscene) {
                g_Dialogue.nextCharTimer--;
                return;
            }

            nextChar = *g_Dialogue.scriptCur++;
            switch (nextChar) {
            case 0:
                self->step = 7;
                return;

            case 1:
                if (g_SkipCutscene) {
                    continue;
                }
                g_Dialogue.nextCharX = g_Dialogue.nextLineX;
                if (!(g_Dialogue.unk12 & 1)) {
                    g_Dialogue.nextLineY += 12;
                }
                g_Dialogue.nextCharY++;
                if (g_Dialogue.nextCharY >= 5) {
                    g_Dialogue.nextCharY = 0;
                }
                CutsceneUnk4();
                if (!(g_Dialogue.unk12 & 1)) {
                    if (g_Dialogue.nextCharY >= 4) {
                        g_Dialogue.unk12 |= 1;
                        g_Dialogue.portraitAnimTimer = 0;
                        self->step_s = 0;
                        self->step++;
                        return;
                    }
                    continue;
                }
                g_Dialogue.portraitAnimTimer = 0;
                self->step_s = 0;
                self->step++;
                return;

            case 2:
                g_Dialogue.unk17 = *g_Dialogue.scriptCur++;
                continue;

            case 3:
                g_Dialogue.nextCharTimer = *g_Dialogue.scriptCur++;
                if (g_SkipCutscene) {
                    continue;
                }
                return;

            case 4:
                if (g_SkipCutscene) {
                    continue;
                }
                prim = g_Dialogue.prim[0];
                for (i = 0; i < 5; i++) {
                    prim->drawMode = DRAW_HIDE;
                    prim = prim->next;
                }
                return;

            case 5:
                if (g_SkipCutscene) {
                    g_Dialogue.scriptCur += 2;
                    continue;
                }
                i = *g_Dialogue.scriptCur++;
                nextChar2 = *g_Dialogue.scriptCur++;
                prim = g_Dialogue.prim[5];
                x = D_us_80180B7C[nextChar2 & 1];
                y = D_us_80180B80[nextChar2 & 1];
                prim->clut = D_us_80180B84[i];
                prim->tpage = 0x90;
                if (nextChar2 & 0x80) {
                    prim->u0 = prim->u2 = x + 0x2F;
                    prim->u1 = prim->u3 = x;
                } else {
                    prim->u0 = prim->u2 = x;
                    prim->u1 = prim->u3 = x + 0x2F;
                }
                prim->v0 = prim->v1 = y;
                prim->v2 = prim->v3 = y + 0x48;
                prim->x0 = prim->x1 = prim->x2 = prim->x3 =
                    g_Dialogue.startX - 0x1E;
                prim->y0 = prim->y1 = prim->y2 = prim->y3 =
                    g_Dialogue.startY + 0x24;
                g_Dialogue.clutIndex = D_us_80180B8C[i];
                CutsceneUnk1();
                CutsceneUnk4();
                prim->priority = PRIORITY_DIALOGUE;
                prim->drawMode = DRAW_DEFAULT;
                DrawCutsceneActorName(i & 0xFFFF, self);
                g_Dialogue.portraitAnimTimer = 6;
                self->step = 3;
                return;

            case 6:
                if (g_SkipCutscene) {
                    continue;
                }
                for (prim = g_Dialogue.prim[0], i = 0; i < 5; i++) {
                    prim->drawMode = DRAW_HIDE;
                    prim = prim->next;
                }
                g_api.FreePrimitives(g_Dialogue.primIndex[1]);
                g_Dialogue.primIndex[1] = -1;
                g_Dialogue.portraitAnimTimer = 6;
                self->step = 4;
                return;

            case 7:
                if (g_SkipCutscene) {
                    g_Dialogue.scriptCur++;
                    g_Dialogue.scriptCur++;
                    continue;
                }
                g_Dialogue.startX = *g_Dialogue.scriptCur++;
                g_Dialogue.startY = *g_Dialogue.scriptCur++;
                prim = g_Dialogue.prim[5];
                prim = prim->next;
                prim->y0 = prim->y1 = g_Dialogue.startY;
                prim->y2 = prim->y3 = g_Dialogue.startY + 0x48;
                prim = prim->next;
                prim->y0 = g_Dialogue.startY - 1;
                prim->u0 = 0xF6;
                g_Dialogue.portraitAnimTimer = 0x18;
                self->step = 5;
                self->step_s = 0;
                return;

            case 8:
                if (g_SkipCutscene) {
                    continue;
                }
                g_Dialogue.portraitAnimTimer = 0x18;
                self->step = 6;
                return;

            case 9:
                if (g_SkipCutscene) {
                    g_Dialogue.scriptCur++;
                    g_Dialogue.scriptCur++;
                    continue;
                }
                nextChar = *g_Dialogue.scriptCur++;
                nextChar <<= 4;
                nextChar |= *g_Dialogue.scriptCur++;
                g_api.PlaySfx(nextChar);
                continue;

            case 10:
                if (g_SkipCutscene) {
                    continue;
                }
                if (g_api.func_80131F68() != false) {
                    continue;
                }
                *g_Dialogue.scriptCur--;
                return;

            case 11:
                if (g_SkipCutscene) {
                    continue;
                }
                if (g_api.func_80131F68() != true) {
                    continue;
                }
                *g_Dialogue.scriptCur--;
                return;

            case 12:
                ptr = *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                SetCutsceneEvents((u8*)ptr);
                continue;

            case 13:
                continue;

            case 14:
                ptr = *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                // unlike the other overlays, this reads the table without
                // adding 0x100000
                g_Dialogue.scriptCur += *CS_NEXT(ptr) << 2;

                ptr = *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur;
                g_Dialogue.scriptCur = CS_PTR(ptr);
                continue;

            case 15:
                ptr = *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur++;
                ptr <<= 4;
                ptr |= *g_Dialogue.scriptCur;
                g_Dialogue.scriptCur = CS_PTR(ptr);
                continue;

            case 16:
                if (!((g_CutsceneFlags >> *g_Dialogue.scriptCur) & 1)) {
                    g_Dialogue.scriptCur--;
                    return;
                }
                g_CutsceneFlags &= ~(1 << *g_Dialogue.scriptCur);
                *g_Dialogue.scriptCur++;
                continue;

            case 17:
                g_CutsceneFlags |= 1 << *g_Dialogue.scriptCur++;
                continue;

            case 18:
                g_Dialogue.hasEvents = 0;
                continue;

            case 19:
                if (g_SkipCutscene) {
                    g_Dialogue.scriptCur += 5;
                } else {
                    ptr = *g_Dialogue.scriptCur++;
                    ptr <<= 4;
                    ptr |= *g_Dialogue.scriptCur++;
                    ptr <<= 4;
                    ptr |= *g_Dialogue.scriptCur++;
                    ptr <<= 4;
                    ptr |= *g_Dialogue.scriptCur++;
                    ptr = (uintptr_t)CS_PTR(ptr);
                    nextChar2 = g_Dialogue.scriptCur++[0];
                    LoadTPage((u_long*)ptr, 1, 0, D_us_80180B88[nextChar2],
                              0x100, 0x30, 0x48);
                }
                continue;

            case 20:
                nextChar = *g_Dialogue.scriptCur++;
                nextChar <<= 4;
                nextChar |= *g_Dialogue.scriptCur++;
                g_api.PlaySfx(nextChar);
                continue;

            case 21:
                g_CutsceneFlags = 0;
                g_SkipCutscene = 0;
                D_us_801B68F0 = 0;
                continue;

            case 22:
                g_CutsceneFlags &= ~(1 << *g_Dialogue.scriptCur++);
                continue;

            case 23:
                return;

            case 24:
                if (!((g_CutsceneFlags >> *g_Dialogue.scriptCur) & 1)) {
                    *g_Dialogue.scriptCur--;
                    return;
                }
                *g_Dialogue.scriptCur++;
                continue;
            }

            if (g_SkipCutscene) {
                continue;
            }
            g_Dialogue.nextCharTimer = g_Dialogue.unk17;
            break;
        }

        if (nextChar == ' ') {
            g_Dialogue.nextCharX += 2;
            return;
        }
        rect.x = ((nextChar & 0xF) * 2) + 0x380;
        rect.y = ((nextChar & 0xF0) >> 1) + 0xF0;
        rect.w = 2;
        rect.h = 8;
        y = ((g_Dialogue.nextCharY * 0xC) + 0x180);
        x = g_Dialogue.nextCharX;
        MoveImage(&rect, x, y);
        g_Dialogue.nextCharX += 2;
        break;

    case 2:
        ScaleCutsceneAvatar(2);
        if (g_Dialogue.portraitAnimTimer >= 6) {
            self->step -= 1;
            return;
        }
        break;

    case 3:
        prim = g_Dialogue.prim[5];
        prim->x0 = prim->x2 -= 4;
        prim->x1 = prim->x3 += 4;
        prim->y0 = prim->y1 -= 6;
        prim->y2 = prim->y3 += 6;
        if (--g_Dialogue.portraitAnimTimer == 0) {
            self->step = 1;
            for (prim = &g_PrimBuf[g_Dialogue.primIndex[1]]; prim != NULL;
                 prim = prim->next) {
                prim->drawMode = DRAW_DEFAULT;
            }
        }
        break;

    case 4:
        prim = g_Dialogue.prim[5];
        prim->x0 = prim->x2 += 4;
        prim->x1 = prim->x3 -= 4;
        prim->y0 = prim->y1 += 6;
        prim->y2 = prim->y3 -= 6;
        if (prim->x1 >= (g_Dialogue.startX - 2)) {
            prim->x1 = prim->x3 = g_Dialogue.startX - 3;
        }
        if (--g_Dialogue.portraitAnimTimer == 0) {
            self->step = 1;
            return;
        }
        break;

    case 5:
        switch (self->step_s) {
        case 0:
            D_us_801B68F0 = 1;
            primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, PrimCount);
            if (primIndex == -1) {
                DestroyEntity(self);
                return;
            }
            g_Dialogue.primIndex[0] = primIndex;
            for (prim = &g_PrimBuf[primIndex], j = 0; prim != NULL;
                 prim = prim->next) {
                prim->r0 = prim->r1 = 0x7F;
                prim->b0 = prim->b1 = prim->g0 = prim->g1 = 0;
                prim->x0 = prim->x1 = 0xF7;
                prim->y0 = prim->y1 = g_Dialogue.startY + j;
                prim->priority = PRIORITY_DIALOGUE;
                prim->drawMode = DRAW_DEFAULT;
                prim->x2 = D_us_80180B90[j];
                prim->x3 = 0xF70;
                j++;
            }
            self->step_s++;
            break;

        case 1:
            j = false;
            prim = &g_PrimBuf[g_Dialogue.primIndex[0]];
            while (prim != NULL) {
                prim->x3 -= prim->x2;
                prim->x2 += 2;
                prim->x0 = prim->x3 / 16;
                if (prim->x0 < 5) {
                    prim->x0 = 4;
                } else {
                    j = true;
                }
                prim = prim->next;
            }
            if (!j) {
                g_api.FreePrimitives(g_Dialogue.primIndex[0]);
                g_Dialogue.primIndex[0] = -1;
                prim = g_Dialogue.prim[5];
                prim = prim->next;
                prim->drawMode = DRAW_TPAGE | DRAW_TRANSP;
                prim = prim->next;
                prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE | DRAW_TRANSP;
                self->step_s++;
            }
            break;

        case 2:
            j = false;
            prim = g_Dialogue.prim[5];
            prim = prim->next;
            if (prim->r0 < 0x10) {
                prim->r0 = prim->r1 = prim->r2 = prim->r3 = 0;
            } else {
                prim->r0 = prim->r1 = prim->r2 = prim->r3 -= 0x10;
                j = true;
            }
            if (prim->b0 >= 0x7F) {
                prim->b0 = prim->b1 = 0x7F;
            } else {
                prim->b0 = prim->b1 += 8;
                j = true;
            }
            prim = prim->next;
            if (prim->r0 < 0x10) {
                prim->r0 = prim->g0 = prim->b0 = 0x10;
            } else {
                prim->r0 = prim->g0 = prim->b0 -= 0xF;
                j = true;
            }
            if (!j) {
                self->step = 1;
            }
        }
        break;

    case 6:
        prim = g_Dialogue.prim[5];
        prim = prim->next;
        if (--g_Dialogue.portraitAnimTimer >= 0xC) {
            prim = prim->next;
            prim->u0 -= 20;
            if (g_Dialogue.portraitAnimTimer & 1) {
                prim->u0--;
            }
        } else {
            if (g_Dialogue.portraitAnimTimer == 0) {
                self->step = 1;
                prim->drawMode = DRAW_HIDE;
            } else {
                prim->y2 = (prim->y3 -= 6);
            }
            prim = prim->next;
            prim->drawMode = DRAW_HIDE;
        }
        break;

    case 7:
        if (self->params) {
            g_CastleFlags[RICHTER_CS_AFTER_M_AND_W] = 1;
            g_CastleFlags[0x85] = 1;
            g_CastleFlags[0x62] = 1;
            g_Settings.D_8003CB04 |= 0x4000;
            g_Settings.D_8003CB04 |= 0x4100;
            g_Settings.D_8003CB04 |= 0x4300;
        } else {
            D_us_80180B78 = 1;
            stopMusicFlag = 0;
            currentMusicId = MU_FESTIVAL_OF_SERVANTS;
            g_Settings.D_8003CB04 |= 0x10;
        }
        DestroyEntity(self);
        g_CutsceneHasControl = 0;
        break;
    }
}
#endif

#ifdef VERSION_PC
// bss
s32 g_SkipCutscene;
Dialogue g_Dialogue;
u32 g_CutsceneFlags;
s32 D_us_801B68F0;
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
const char* actor_names[2] = { _S("Alucard"), _S("Richter") };
s32 D_us_80180B78 = 0;
u8 D_us_80180B7C[4] = { 0, 0x40, 0, 0 };
u8 D_us_80180B80[4] = { 0, 0, 0, 0 };
u16 D_us_80180B84[2] = { 0x230, 0x238 };
u16 D_us_80180B88[2] = { 0, 0x20 };
u16 D_us_80180B8C[2] = { 0x1A1, 0x1A1 };
s16 D_us_80180B90[0x48] = {
    8,
    0x13,
    0x11,
    0x31,
    0x4F,
    0x26,
    0x36,
    0x1D,
    0x1B,
    0x33,
    0x2C,
    0x21,
    0x19,
    0xA,
    0x33,
    0x1F,
    0x48,
    0x2F,
    0x13,
    0x19,
    0x4D,
    0x4B,
    0x17,
    0x1D,
    0x12,
    2,
    0x1B,
    0x2A,
    0x50,
    0x45,
    0x32,
    0xD,
    0x2A,
    0x4D,
    6,
    0x27,
    7,
    0x48,
    0x2F,
    0x1B,
    0x36,
    0x22,
    0x39,
    0x14,
    0x39,
    0x1D,
    0xA,
    0x35,
    0x10,
    0x1B,
    0x3D,
    0x17,
    0x2E,
    0xB,
    0x49,
    0x42,
    0x3D,
    0x2A,
    1,
    0xC,
    0x1B,
    0x34,
    0x41,
    0x35,
    8,
    0xE,
    0x4D,
    0x11,
    0x34,
    0x41,
    0x29,
    0x48,
};
// end pc_import_data.py
// clang-format on
#endif
