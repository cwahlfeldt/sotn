// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo6.h"
#include <cutscene.h>

extern Dialogue g_Dialogue;
extern const char* actor_names[];

#define CUTSCENE_UNK1_NEXT_X (DISP_STAGE_W + 2)
#include "../../st/cutscene_unk1.h"

#include "../../st/set_cutscene_script.h"

#define CUTSCENE_UNK3_RECT_X DISP_STAGE_W
#include "../../st/cutscene_unk3.h"

#define CUTSCENE_UNK4_TPAGE 0x14
#include "../../st/cutscene_unk4.h"

#include "../../st/cutscene_actor_name.h"

#include "../../st/set_cutscene_events.h"

#define CUTSCENE_TILEMAP_SCROLL
#include "../../st/cutscene_events.h"

#include "../../st/cutscene_skip.h"

#include "../../st/cutscene_scale_avatar.h"

extern u16 PrizeDrops[];
extern u8 D_us_80180614[];
extern u16 D_us_80180618[];
extern u16 D_us_80180620[];
extern u16 D_us_80180624[];
extern s16 D_us_8018062C[];
extern u8 D_us_8019A0C8;
extern u8 D_us_8019A0CC;
extern u8 D_us_8019A0D0[];
extern u8 D_us_8019A2E1[];
extern u8 D_us_8019A2FB[];
extern s32 g_IsCutsceneDone;
extern u32 g_CutsceneFlags;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo6/nonmatchings/cutscene", func_us_801A7DC0);
#else

// The conversation with Richter in the Castle Keep: before the fight
// (params 0), when he is defeated (params 1) and after it (params 2 and 3,
// which differ by D_us_8019A0CC)
void func_us_801A7DC0(Entity* self) {
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
        if (g_IsCutsceneDone && !g_SkipCutscene) {
            if (g_GameClearFlag) {
                CutsceneSkip(self);
            } else {
                switch (self->params) {
                case 0:
                    if (g_Settings.D_8003CB04 & 0x8000) {
                        CutsceneSkip(self);
                    }
                    break;
                case 2:
                    if (g_Settings.D_8003CB04 & 0x10000) {
                        CutsceneSkip(self);
                    }
                    break;
                case 3:
                    if (g_Settings.D_8003CB04 & 0x20000) {
                        CutsceneSkip(self);
                    }
                    break;
                }
            }
        }
        if (self->step && g_Dialogue.hasEvents) {
            RunCutsceneEvents();
        }
    }
    switch (self->step) {
    case 0:
        script = NULL;
        switch (self->params) {
        case 0:
            if (g_DemoMode != Demo_None || g_CastleFlags[0x95] ||
                g_PlayableCharacter != PLAYER_ALUCARD) {
                DestroyEntity(self);
                return;
            }
            script = D_us_8019A0D0;
            break;
        case 1:
            script = D_us_8019A2E1;
            break;
        case 2:
        case 3:
            if (g_DemoMode != Demo_None || g_CastleFlags[0x95]) {
                DestroyEntity(self);
                return;
            }
            script = D_us_8019A2FB;
            break;
        }
        if (script && (SetCutsceneScript(script) & 0xFF)) {
            // the switch variables the script reads
            if (self->params == 0) {
                D_us_8019A0C8 = g_CastleFlags[0xD8] ? 1 : 0;
                D_us_8019A0CC =
                    g_api.CheckEquipmentItemCount(0x22, 1) ? 1 : 0;
            } else if (self->params >= 2) {
                D_us_8019A0CC = self->params - 2;
            }
            g_CutsceneHasControl = 1;
            self->flags |= FLAG_HAS_PRIMS | FLAG_UNK_2000;
            self->step++;
            g_CutsceneFlags = 0;
            g_IsCutsceneDone = 0;
            g_SkipCutscene = 0;
            self->primIndex = g_Dialogue.primIndex[2];
            self->flags |= FLAG_UNK_10000;
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
                // splat named the u table PrizeDrops
                x = ((u8*)PrizeDrops)[nextChar2 & 1];
                y = D_us_80180614[nextChar2 & 1];
                prim->clut = D_us_80180618[i];
                prim->tpage = 0x94;
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
                g_Dialogue.clutIndex = D_us_80180624[i];
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
                // the switch variable is a byte
                g_Dialogue.scriptCur += *CS_PTR(ptr) << 2;

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
                    LoadTPage((u_long*)ptr, 1, 0, D_us_80180620[nextChar2],
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
                g_IsCutsceneDone = 0;
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
            g_IsCutsceneDone = 1;
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
                prim->x2 = D_us_8018062C[j];
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
        switch (self->params) {
        case 0:
            g_Settings.D_8003CB04 |= 0x8000;
            break;
        case 2:
            g_Settings.D_8003CB04 |= 0x10000;
            break;
        case 3:
            g_CastleFlags[0x95] = 1;
            g_Settings.D_8003CB04 |= 0x20000;
            break;
        }
        DestroyEntity(self);
        g_CutsceneHasControl = 0;
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
const char* actor_names[3] = { _S("Alucard"), _S("Richter"), _S("Shaft") };
u8 D_us_80180614[4] = { 0, 0, 0, 0 };
u16 D_us_80180618[4] = { 0x260, 0x268, 0x270, 0x218 };
u16 D_us_80180620[2] = { 0x100, 0x120 };
u16 D_us_80180624[4] = { 0x1A1, 0x1A1, 0x1A1, 0x1A1 };
s16 D_us_8018062C[0x48] = {
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
u8 D_us_8019A0C8 = 0;
u8 D_us_8019A0CC = 0;
u8 D_us_8019A0D0[0x211] = {
    0xC,
    0x9A,
    0xA4,
    0x4C,
    0xC7,
    0x10,
    0,
    7,
    0x3C,
    0x1C,
    0x13,
    0x9C,
    0xC2,
    0x2A,
    0xA8,
    0,
    5,
    1,
    0,
    0xB,
    9,
    0x3D,
    0xD3,
    0xA,
    2,
    2,
    0x49,
    0x27,
    0x76,
    0x65,
    0x20,
    0x62,
    0x65,
    0x65,
    0x6E,
    0x20,
    0x77,
    0x61,
    0x69,
    0x74,
    0x69,
    0x6E,
    0x67,
    1,
    3,
    8,
    0x66,
    0x6F,
    0x72,
    0x20,
    0x79,
    0x6F,
    0x75,
    0x2E,
    3,
    0x30,
    6,
    0x13,
    0x9B,
    0xB5,
    0x52,
    0x28,
    1,
    5,
    0,
    0x81,
    0xB,
    0xE,
    0x9A,
    0xA0,
    0xC,
    0xC8,
    0x9A,
    0xA1,
    0x12,
    0x20,
    0x9A,
    0xA1,
    0x18,
    0x85,
    9,
    0x3D,
    0xD4,
    0xA,
    2,
    3,
    0x53,
    0x6F,
    0x20,
    0x69,
    0x74,
    0x20,
    0x69,
    0x73,
    0x20,
    0x79,
    0x6F,
    0x75,
    0x2E,
    0x2E,
    0x2E,
    1,
    3,
    0x10,
    2,
    2,
    0x42,
    0x75,
    0x74,
    0x20,
    0x77,
    0x68,
    0x79,
    0x20,
    0x69,
    0x73,
    0x20,
    0x61,
    0x20,
    0x42,
    0x65,
    0x6C,
    0x6D,
    0x6F,
    0x6E,
    0x74,
    1,
    3,
    4,
    0x70,
    0x6C,
    0x61,
    0x6E,
    0x6E,
    0x69,
    0x6E,
    0x67,
    0x20,
    0x74,
    0x68,
    0x65,
    1,
    3,
    4,
    0x72,
    0x65,
    0x73,
    0x75,
    0x72,
    0x72,
    0x65,
    0x63,
    0x74,
    0x69,
    0x6F,
    0x6E,
    0x20,
    0x6F,
    0x66,
    1,
    3,
    4,
    0x43,
    0x6F,
    0x75,
    0x6E,
    0x74,
    0x20,
    0x44,
    0x72,
    0x61,
    0x63,
    0x75,
    0x6C,
    0x61,
    0x3F,
    0xF,
    0x9A,
    0xA1,
    0x1D,
    0xDC,
    9,
    0x3D,
    0xD6,
    0xA,
    2,
    3,
    0x41,
    0x6E,
    0x73,
    0x77,
    0x65,
    0x72,
    0x20,
    0x6D,
    0x65,
    0x21,
    1,
    3,
    0x10,
    2,
    2,
    0x57,
    0x68,
    0x79,
    0x20,
    0x69,
    0x73,
    0x20,
    0x61,
    0x20,
    0x42,
    0x65,
    0x6C,
    0x6D,
    0x6F,
    0x6E,
    0x74,
    1,
    3,
    4,
    0x70,
    0x6C,
    0x61,
    0x6E,
    0x6E,
    0x69,
    0x6E,
    0x67,
    0x20,
    0x74,
    0x68,
    0x65,
    1,
    3,
    8,
    0x72,
    0x65,
    0x73,
    0x75,
    0x72,
    0x72,
    0x65,
    0x63,
    0x74,
    0x69,
    0x6F,
    0x6E,
    0x20,
    0x6F,
    0x66,
    1,
    3,
    4,
    0x43,
    0x6F,
    0x75,
    0x6E,
    0x74,
    0x20,
    0x44,
    0x72,
    0x61,
    0x63,
    0x75,
    0x6C,
    0x61,
    0x3F,
    3,
    0x30,
    6,
    5,
    1,
    0,
    0xB,
    9,
    0x3D,
    0xD7,
    0xA,
    2,
    3,
    0x43,
    0x6F,
    0x75,
    0x6E,
    0x74,
    0x20,
    0x44,
    0x72,
    0x61,
    0x63,
    0x75,
    0x6C,
    0x61,
    0x20,
    0x72,
    0x69,
    0x73,
    0x65,
    0x73,
    1,
    3,
    4,
    0x62,
    0x75,
    0x74,
    0x20,
    0x6F,
    0x6E,
    0x63,
    0x65,
    0x20,
    0x65,
    0x76,
    0x65,
    0x72,
    0x79,
    1,
    3,
    4,
    0x63,
    0x65,
    0x6E,
    0x74,
    0x75,
    0x72,
    0x79,
    0x20,
    3,
    0x28,
    0x61,
    0x6E,
    0x64,
    0x20,
    0x6D,
    0x79,
    0x20,
    0x72,
    0x6F,
    0x6C,
    0x65,
    1,
    3,
    4,
    0x69,
    0x73,
    0x20,
    0x6F,
    0x76,
    0x65,
    0x72,
    0x2E,
    0x20,
    2,
    2,
    3,
    0x1C,
    0x49,
    0x66,
    0x20,
    0x49,
    0x20,
    0x63,
    0x61,
    0x6E,
    1,
    3,
    4,
    0x72,
    0x65,
    0x73,
    0x75,
    0x72,
    0x72,
    0x65,
    0x63,
    0x74,
    0x20,
    0x68,
    0x69,
    0x6D,
    0x2C,
    1,
    3,
    0x14,
    0x74,
    0x68,
    0x65,
    0x6E,
    0x20,
    0x74,
    0x68,
    0x65,
    0x20,
    0x62,
    0x61,
    0x74,
    0x74,
    0x6C,
    0x65,
    0x20,
    0x77,
    0x69,
    0x6C,
    0x6C,
    1,
    3,
    4,
    0x6C,
    0x61,
    0x73,
    0x74,
    0x20,
    0x66,
    0x6F,
    0x72,
    0x20,
    0x65,
    0x74,
    0x65,
    0x72,
    0x6E,
    0x69,
    0x74,
    0x79,
    0x21,
    3,
    0x30,
    6,
    0xE,
    0x9A,
    0xA0,
    0xC,
    0xCC,
    0x9A,
    0xA2,
    0x2D,
    0xD9,
    0x9A,
    0xA2,
    0x28,
    0x8A,
    5,
    0,
    0x81,
    2,
    4,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    1,
    3,
    8,
    0xB,
    9,
    0x3D,
    0xD8,
    0xA,
    2,
    2,
    0x49,
    0x66,
    0x20,
    0x74,
    0x68,
    0x6F,
    0x73,
    0x65,
    0x20,
    0x61,
    0x72,
    0x65,
    0x20,
    0x79,
    0x6F,
    0x75,
    0x72,
    1,
    3,
    4,
    0x74,
    0x72,
    0x75,
    0x65,
    0x20,
    0x66,
    0x65,
    0x65,
    0x6C,
    0x69,
    0x6E,
    0x67,
    0x73,
    0x20,
    0x74,
    0x68,
    0x65,
    0x6E,
    1,
    3,
    4,
    2,
    3,
    0x73,
    0x6F,
    0x20,
    0x62,
    0x65,
    0x20,
    0x69,
    0x74,
    0x2E,
    3,
    0x30,
    6,
    8,
    0xB,
    0x11,
    1,
    0x14,
    0x33,
    0x36,
    0,
};
u8 D_us_8019A2E1[0x1A] = {
    0xC,
    0x9A,
    0xA4,
    0x4E,
    0xE8,
    9,
    9,
    0x91,
    0xB,
    9,
    0x3E,
    0xE1,
    0xA,
    3,
    0x80,
    3,
    0x80,
    0x11,
    2,
    0xB,
    0x11,
    3,
    0x14,
    0x33,
    0x36,
    0,
};
u8 D_us_8019A2FB[0x1F9] = {
    0xE,
    0x9A,
    0xA0,
    0xC,
    0xCC,
    0x9A,
    0xA3,
    0x30,
    8,
    0x9A,
    0xA4,
    0x43,
    0x32,
    0xC,
    0x9A,
    0xA4,
    0x4D,
    0xDD,
    9,
    9,
    0x91,
    3,
    0x20,
    0xB,
    7,
    0x3C,
    0x1C,
    0x13,
    0x9B,
    0xB5,
    0x52,
    0x28,
    1,
    5,
    0,
    0x81,
    0xB,
    9,
    0x3E,
    0xEA,
    0xA,
    2,
    3,
    0x49,
    0x74,
    0x27,
    0x73,
    0x20,
    0x6F,
    0x76,
    0x65,
    0x72,
    0x2E,
    0x2E,
    0x2E,
    0x42,
    0x65,
    0x6C,
    0x6D,
    0x6F,
    0x6E,
    0x74,
    0x2E,
    3,
    0x30,
    6,
    0x13,
    0x9C,
    0xC2,
    0x2A,
    0xA8,
    0,
    5,
    1,
    0,
    0xB,
    9,
    0x3E,
    0xEB,
    0xA,
    2,
    3,
    0x53,
    0x6F,
    0x20,
    3,
    0x10,
    0x74,
    0x68,
    0x65,
    0x20,
    0x77,
    0x61,
    0x72,
    0x20,
    0x62,
    0x65,
    0x74,
    0x77,
    0x65,
    0x65,
    0x6E,
    1,
    3,
    4,
    2,
    4,
    0x68,
    0x75,
    0x6D,
    0x61,
    0x6E,
    0x73,
    0x20,
    0x61,
    0x6E,
    0x64,
    0x20,
    0x76,
    0x61,
    0x6D,
    0x70,
    0x69,
    0x72,
    0x65,
    0x73,
    1,
    3,
    4,
    0x66,
    0x69,
    0x6E,
    0x61,
    0x6C,
    0x6C,
    0x79,
    0x20,
    0x65,
    0x6E,
    0x64,
    0x73,
    0x20,
    0x68,
    0x65,
    0x72,
    0x65,
    0x2E,
    0x2E,
    0x2E,
    3,
    0x30,
    6,
    5,
    0,
    0x81,
    0xB,
    2,
    4,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    0x2E,
    3,
    0x38,
    6,
    5,
    1,
    0,
    0xB,
    9,
    0x3E,
    0xEC,
    0xA,
    2,
    2,
    0x57,
    0x68,
    0x61,
    0x74,
    0x20,
    0x6E,
    0x65,
    0x65,
    0x64,
    0x20,
    0x66,
    0x6F,
    0x72,
    0x20,
    0x74,
    0x68,
    0x65,
    1,
    3,
    4,
    0x73,
    0x68,
    0x65,
    0x70,
    0x68,
    0x65,
    0x72,
    0x64,
    0x20,
    3,
    0x20,
    0x77,
    0x68,
    0x65,
    0x6E,
    0x20,
    0x74,
    0x68,
    0x65,
    1,
    3,
    4,
    0x77,
    0x6F,
    0x6C,
    0x76,
    0x65,
    0x73,
    0x20,
    0x68,
    0x61,
    0x76,
    0x65,
    0x20,
    0x61,
    0x6C,
    0x6C,
    1,
    3,
    4,
    0x67,
    0x6F,
    0x6E,
    0x65,
    0x2E,
    0x2E,
    0x2E,
    1,
    3,
    0x1C,
    0x4D,
    0x79,
    0x20,
    0x74,
    0x69,
    0x6D,
    0x65,
    0x20,
    0x6F,
    0x6E,
    0x20,
    0x74,
    0x68,
    0x69,
    0x73,
    1,
    3,
    4,
    0x77,
    0x6F,
    0x72,
    0x6C,
    0x64,
    0x20,
    3,
    0x24,
    2,
    3,
    0x68,
    0x61,
    0x73,
    0x20,
    0x63,
    0x6F,
    0x6D,
    0x65,
    0x20,
    3,
    0x20,
    0x74,
    0x6F,
    1,
    3,
    4,
    0x61,
    0x6E,
    0x20,
    0x65,
    0x6E,
    0x64,
    0x2E,
    0x2E,
    0x2E,
    3,
    0x30,
    6,
    8,
    0x11,
    8,
    0,
    0xC,
    0x9A,
    0xA4,
    0x4D,
    0xD2,
    9,
    9,
    0x91,
    3,
    0x20,
    0xB,
    7,
    0x3C,
    0x1C,
    0x13,
    0x9D,
    0xD0,
    2,
    0x28,
    0,
    5,
    2,
    0,
    0xB,
    9,
    0x3E,
    0xEF,
    0xA,
    2,
    4,
    0x59,
    0x6F,
    0x75,
    0x27,
    0x76,
    0x65,
    0x20,
    0x64,
    0x65,
    0x66,
    0x65,
    0x61,
    0x74,
    0x65,
    0x64,
    0x20,
    0x6D,
    0x65,
    0x2E,
    1,
    3,
    0x18,
    0x42,
    0x75,
    0x74,
    0x20,
    0x61,
    0x6C,
    0x6C,
    0x20,
    0x69,
    0x73,
    0x20,
    2,
    3,
    0x6E,
    0x6F,
    0x74,
    0x20,
    0x79,
    0x65,
    0x74,
    1,
    3,
    4,
    0x6C,
    0x6F,
    0x73,
    0x74,
    0x2E,
    1,
    3,
    0x28,
    2,
    3,
    0x54,
    0x68,
    0x65,
    0x20,
    0x72,
    0x65,
    0x73,
    0x75,
    0x72,
    0x72,
    0x65,
    0x63,
    0x74,
    0x69,
    0x6F,
    0x6E,
    0x20,
    0x6F,
    0x66,
    1,
    3,
    4,
    0x43,
    0x6F,
    0x75,
    0x6E,
    0x74,
    0x20,
    0x44,
    0x72,
    0x61,
    0x63,
    0x75,
    0x6C,
    0x61,
    0x20,
    2,
    2,
    0x69,
    0x73,
    0x20,
    0x61,
    0x74,
    1,
    3,
    4,
    0x68,
    0x61,
    0x6E,
    0x64,
    0x21,
    3,
    0x30,
    6,
    8,
    3,
    0x60,
    0x11,
    5,
    3,
    0x20,
    0x11,
    6,
    0,
    0,
    0,
    0,
    0x89,
    0x18,
    8,
    0x80,
    8,
    0x80,
    0xFF,
    0xFF,
    0,
    0,
    0,
    0x89,
    0x19,
    8,
    0x80,
    8,
    0x80,
    0xFF,
    0xFF,
    0,
    0,
    0,
    0x89,
    0x1A,
    8,
    0x80,
    8,
    0x80,
    0xFF,
    0xFF,
    0,
    0,
    0,
    0x89,
    0x1B,
    8,
    0x80,
    8,
    0x80,
    0xFF,
    0xFF,
    0,
};
u32 g_CutsceneFlags;
Dialogue g_Dialogue;
s32 g_IsCutsceneDone;
s32 g_SkipCutscene;
// end pc_import_data.py
// clang-format on
#endif
