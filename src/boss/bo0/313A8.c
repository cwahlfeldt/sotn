// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo0.h"
#include <scratchpad.h>

// Olrox's second form: a skeleton that walks on two legs, made of the entity
// itself and the ten slots after it (see ET_OlroxBeast)

// pointers on PC, so these need prototypes
Primitive* FindFirstUnkPrim2(Primitive* prim, u8 index);
Primitive* UnkRecursivePrimFunc2(
    Primitive* srcPrim, s32 iterations, Primitive* dstPrim, u8* dataPtr);
u8 AnimateEntity(u8 frames[], Entity* entity);
void MakeExplosions(void);

extern EInit D_us_80180708;
extern EInit D_us_80180714;
extern s32 D_us_80181190;
extern s16 D_us_80181194[4][7]; // neck pose: head offset, texture, parts
extern s16 D_us_801811CC[4][4]; // laser: neck angles, head angle
extern s16 D_us_801811EC[4][4]; // fireballs: duration, range, speed, delay
extern u8 D_us_8018120C[];
extern u8 D_us_80181220[];
extern u8 D_us_80181234[];
extern s16 D_us_80181268[8];
extern s16 D_us_80181278[8];
extern s16 D_us_80181288[8];
extern s16 D_us_80181298[8];
extern s16 D_us_801812A8[8];
extern s16 D_us_801812B8[8];
extern s16* D_us_801812D0[2];
extern s16 D_us_801812D8[6];
extern s16* sprites_bo0_5[];
extern s32 D_us_801CE56C; // base draw priority of the skeleton
extern s32 D_us_801CE5B0; // set while the first form transforms

#define BEAST(e) ((e)->ext.olroxBeast)
#define HEAD (g_CurrentEntity + 3)
#define TAIL (g_CurrentEntity + 4)
#define MOUTH (g_CurrentEntity + 7)

static s16 AbsX(Entity* self) {
    s16 x = g_Tilemap.scrollX.i.hi + self->posX.i.hi;
    if (self->facingLeft) {
        x = 0x200 - x;
    }
    return x;
}

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B13A8);
#else
// Sets a quad from the entity's frame in sprites_bo0_5
void func_us_801B13A8(Primitive* prim) {
    Entity* self = g_CurrentEntity;
    s16* part;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s32 uOffset;
    s32 vOffset;

    uOffset = (self->unk5A & 1) * 0x7F;
    vOffset = ((self->unk5A & 2) >> 1) * 0x7F;
    part = sprites_bo0_5[self->animCurFrame] + 2;
    if (self->facingLeft) {
        x = self->posX.i.hi - part[0];
    } else {
        x = self->posX.i.hi + part[0];
    }
    y = self->posY.i.hi + part[1];
    width = part[2];
    height = part[3];
    if (self->facingLeft) {
        prim->x0 = prim->x2 = x - width + 1;
        prim->x1 = prim->x3 = x + 1;
    } else {
        prim->x0 = prim->x2 = x;
        prim->x1 = prim->x3 = x + width;
    }
    prim->y0 = prim->y1 = y;
    prim->y2 = prim->y3 = y + height;
    prim->clut = part[4];
    if (part[5] & 1) {
        uOffset += 0x7F;
    }
    if (part[5] & 2) {
        vOffset += 0x7F;
    }
    if (self->facingLeft) {
        prim->u1 = prim->u3 = (u8)part[6] + uOffset;
        prim->v0 = prim->v1 = (u8)part[7] + vOffset;
        prim->u0 = prim->u2 = (u8)part[8] + uOffset;
    } else {
        prim->u0 = prim->u2 = (u8)part[6] + uOffset;
        prim->v0 = prim->v1 = (u8)part[7] + vOffset;
        prim->u1 = prim->u3 = (u8)part[8] + uOffset;
    }
    prim->v2 = prim->v3 = (u8)part[9] + vOffset;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1590);
#else
static void func_us_801B1590(u8 step) {
    OlroxLeg* leg = BEAST(g_CurrentEntity).legs[0];

    g_CurrentEntity->step = step;
    g_CurrentEntity->step_s = 0;
    leg->step = 0;
    leg->other->step = 0;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B15BC);
#else
// moves a value toward a target by step; true once there
s32 func_us_801B15BC(s16* value, s16 target, s16 step) {
    s16 current = *value;

    if (abs(current - target) < step) {
        *value = target;
        return 1;
    }
    if (target < current) {
        *value = current - step;
    }
    if (*value < target) {
        *value += step;
    }
    return 0;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B163C);
#else
// eases a servo toward a target at up to maxSpeed; true once there
s32 func_us_801B163C(OlroxServo* servo, s16 target, s16 maxSpeed) {
    s16 accel;
    s16 speed;
    s16 diff;

    accel = maxSpeed / 16;
    if (!accel) {
        accel = 1;
    }
    diff = target - servo->value;
    if (diff > 0) {
        if (servo->speed >= maxSpeed) {
            speed = servo->speed - accel;
        } else {
            speed = servo->speed + accel;
        }
    } else if (-maxSpeed < servo->speed) {
        speed = servo->speed - accel;
    } else {
        speed = servo->speed + accel;
    }
    if (abs(diff) >= abs(speed)) {
        servo->value += speed;
        servo->speed = speed;
        return 0;
    }
    servo->value = target;
    servo->speed = diff;
    return 1;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B171C);
#else
// bends a leg toward two angles at a constant rate; true once both there
s32 func_us_801B171C(OlroxLeg* leg, s16 upper, s16 lower, s16 step) {
    OlroxServo* servo;
    s32 moving;
    s32 i;
    s16 target;
    s16 diff;

    moving = 0;
    servo = &leg->upper;
    target = upper;
    for (i = 0; i < 2; i++) {
        diff = target - servo->value;
        if (abs(diff) < step) {
            servo->value = target;
            servo->speed = diff;
        } else {
            if (diff > 0) {
                servo->value += step;
                servo->speed = step;
            } else {
                servo->value -= step;
                servo->speed = -step;
            }
            moving |= 1;
        }
        servo = &leg->lower;
        target = lower;
    }
    return moving ^ 1;
}
#endif

// Each of these plays one leg of a move, keyed on leg->step

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B17BC);
#else
static void func_us_801B17BC(OlroxLeg* leg) {
    switch (leg->step) {
    case 0:
        if (func_us_801B171C(leg, -0x400, 0x200, 0x18)) {
            leg->step++;
        }
        break;
    case 1:
        if (func_us_801B171C(leg, -0x400, -0x40, 0x20)) {
            leg->step++;
        }
        break;
    case 2:
        if (func_us_801B171C(leg, -0x300, -0x40, 0x20)) {
            leg->step++;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1864);
#else
static void func_us_801B1864(OlroxLeg* leg) {
    switch (leg->step) {
    case 0:
        if (func_us_801B171C(leg, -0x40, 0x40, 0x1C)) {
            leg->step++;
        }
        if (BEAST(g_CurrentEntity).neck.value >= -0xFF) {
            BEAST(g_CurrentEntity).neck.value -= 0xC;
        }
        break;
    case 1:
        if (func_us_801B171C(leg, -0x100, 0x300, 0x18)) {
            leg->step++;
        }
        if (BEAST(g_CurrentEntity).neck.value < 0x100) {
            BEAST(g_CurrentEntity).neck.value += 8;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1950);
#else
static void func_us_801B1950(OlroxLeg* leg) {
    switch (leg->step) {
    case 0:
        if (func_us_801B171C(leg, -0x200, 0x300, 0x18)) {
            leg->step++;
        }
        break;
    case 1:
        if (func_us_801B171C(leg, 0, 0x280, 0x1C)) {
            leg->step++;
        }
        break;
    case 2:
        if (func_us_801B171C(leg, 0x80, 0x180, 0x18)) {
            leg->step++;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B19FC);
#else
static void func_us_801B19FC(OlroxLeg* leg) {
    switch (leg->step) {
    case 0:
        if (func_us_801B171C(leg, -0x80, 0x80, 0x1C)) {
            leg->step++;
        }
        if (BEAST(g_CurrentEntity).neck.value >= -0xFF) {
            BEAST(g_CurrentEntity).neck.value -= 0x10;
        }
        break;
    case 1:
        if (func_us_801B171C(leg, -0x100, 0, 0x18)) {
            leg->step++;
        }
        break;
    case 2:
        if (func_us_801B171C(leg, -0x280, -0xC0, 0x14)) {
            leg->step++;
        }
        if (BEAST(g_CurrentEntity).neck.value < 0x100) {
            BEAST(g_CurrentEntity).neck.value += 0x10;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1B30);
#else
// crouch before a jump
static s32 func_us_801B1B30(OlroxLeg* leg) {
    OlroxLeg* other = leg->other;
    s32 done;

    switch (leg->step) {
    case 0:
        done = func_us_801B171C(leg, -0x180, 0, 0x20);
        if (done + func_us_801B171C(other, 0x80, 0x280, 0x20) == 2) {
            leg->step++;
        }
        return 0;
    case 1:
        done = func_us_801B171C(leg, -0x200, -0x100, 0x40);
        func_us_801B171C(other, -0x100, 0x280, 0x40);
        if (done) {
            leg->step++;
        }
        return 0;
    case 2:
        done = func_us_801B171C(leg, -0x280, -0x200, 0x60);
        func_us_801B171C(other, -0x180, 0x300, 0x40);
        return done ? 1 : 0;
    }
    return 0;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1C60);
#else
static s32 func_us_801B1C60(OlroxLeg* leg) {
    OlroxLeg* other = leg->other;
    s32 done;

    func_us_801B163C(&BEAST(g_CurrentEntity).neck, -0x100, 0xC);
    done = func_us_801B171C(leg, -0x1C0, 0x280, 0x10);
    return done + func_us_801B171C(other, -0x380, 0x140, 0x10) == 2;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1CE0);
#else
static s32 func_us_801B1CE0(OlroxLeg* leg) {
    OlroxLeg* other = leg->other;
    s32 done;

    done = func_us_801B163C(&BEAST(g_CurrentEntity).neck, 0x180, 0x40);
    switch (leg->step) {
    case 0:
        func_us_801B171C(leg, -0x60, 0x340, 0x18);
        if (func_us_801B171C(other, -0x500, 0x80, 0x20)) {
            leg->step++;
        }
        return 0;
    case 1:
        done += func_us_801B171C(leg, -0x40, 0x340, 0x18);
        return done + func_us_801B171C(other, -0x3C0, 0xE0, 0x20) == 3;
    }
    return 0;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1DDC);
#else
static s32 func_us_801B1DDC(OlroxLeg* leg) {
    OlroxLeg* other = leg->other;
    s32 done;

    func_us_801B163C(&BEAST(g_CurrentEntity).neck, 0, 0xC);
    done = func_us_801B171C(leg, -0x2C0, 0x2C0, 0x28);
    return done + func_us_801B171C(other, -0x100, 0x3C0, 0x38) == 2;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1E5C);
#else
static s32 func_us_801B1E5C(OlroxLeg* leg) {
    OlroxLeg* other = leg->other;
    s32 done;

    func_us_801B163C(&BEAST(g_CurrentEntity).neck, 0, 0xC);
    done = func_us_801B171C(leg, -0x40, 0x40, 0x60);
    return done + func_us_801B171C(other, -0x200, 0x280, 0x50) == 2;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1EDC);
#else
static s32 func_us_801B1EDC(OlroxLeg* leg) {
    OlroxLeg* other = leg->other;
    s32 done;

    func_us_801B163C(&BEAST(g_CurrentEntity).neck, 0, 0x10);
    done = func_us_801B171C(leg, -0x200, 0x200, 0x60);
    return done + func_us_801B171C(other, -0x200, 0x180, 0x40) == 2;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B1F5C);
#else
static s32 func_us_801B1F5C(OlroxLeg* leg) {
    OlroxLeg* other = leg->other;
    s32 done;

    switch (leg->step) {
    case 0:
        done = func_us_801B171C(leg, -0x80, 0x80, 0x20);
        if (done + func_us_801B171C(other, 0, 0x180, 0xC) == 2) {
            leg->step++;
        }
        return 0;
    case 1:
        done = func_us_801B171C(leg, -0x300, 0, 0x28);
        return done + func_us_801B171C(other, 0x80, 0x100, 0xC) == 2;
    }
    return 0;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B2044);
#else
static void func_us_801B2044(OlroxLeg* leg) {
    switch (leg->step) {
    case 0:
        if (func_us_801B171C(leg, -0x400, 0x200, 8)) {
            leg->step++;
        }
        break;
    case 1:
        if (func_us_801B171C(leg, -0x400, -0x40, 8)) {
            leg->step++;
        }
        break;
    case 2:
        if (func_us_801B171C(leg, -0x300, -0x40, 0xC)) {
            leg->step++;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B20F4);
#else
static void func_us_801B20F4(OlroxLeg* leg) {
    switch (leg->step) {
    case 0:
        if (func_us_801B171C(leg, -0x40, 0x40, 8)) {
            leg->step++;
        }
        break;
    case 1:
        if (func_us_801B171C(leg, -0x100, 0x380, 0x14)) {
            leg->step++;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B2178);
#else
// how far the foot sank into the floor, which then holds it
static s16 func_us_801B2178(OlroxLeg* leg) {
    s16 y = g_Tilemap.scrollY.i.hi + leg->footY.i.hi;
    s16 push = 0;

    if (y >= 0x1CC) {
        push = 0x1CC - y;
        leg->onGround = 1;
    } else {
        leg->onGround = 0;
    }
    FntPrint("hit_kind %x\n", leg->onGround);
    return push;
}
#endif

static s16 LegAngle(s16 angle) {
    if (g_CurrentEntity->facingLeft) {
        return 0xC00 - angle;
    }
    return angle + 0xC00;
}

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B21F0);
#else
// Places the body over the planted foot and the other leg under the hip
static void func_us_801B21F0(void) {
    Entity* self = g_CurrentEntity;
    OlroxLeg* leg;
    OlroxLeg* other;
    s32 footX;
    s32 footY;
    s32 kneeX;
    s32 kneeY;
    s32 hipX;
    s32 hipY;
    s16 angle;

    BEAST(self).legs[0]->footX.val = (self + 1)->posX.val;
    BEAST(self).legs[0]->footY.val = (self + 1)->posY.val;
    BEAST(self).legs[1]->footX.val = (self + 2)->posX.val;
    BEAST(self).legs[1]->footY.val = (self + 2)->posY.val;
    if (!BEAST(self).plantedLeg) {
        leg = BEAST(self).legs[0];
    } else {
        leg = BEAST(self).legs[1];
    }

    footX = leg->footX.val;
    footY = leg->footY.val;
    angle = LegAngle(leg->lower.value);
    kneeX = footX - rcos(angle) * 0x2A0;
    kneeY = footY + rsin(angle) * 0x2A0;
    leg->kneeX.val = kneeX;
    leg->kneeY.val = kneeY;
    angle = LegAngle(leg->upper.value);
    hipX = kneeX - rcos(angle) * 0x1C0;
    hipY = kneeY + rsin(angle) * 0x1C0;
    leg->reach = footX - hipX;
    if (!self->facingLeft) {
        leg->reach = -(footX - hipX);
    }
    self->posX.val = hipX;
    self->posY.val = hipY - FIX(4);

    other = leg->other;
    angle = LegAngle(other->upper.value);
    kneeX = hipX + rcos(angle) * 0x1C0;
    kneeY = hipY - rsin(angle) * 0x1C0;
    other->kneeX.val = kneeX;
    other->kneeY.val = kneeY;
    angle = LegAngle(other->lower.value);
    footX = kneeX + rcos(angle) * 0x2A0;
    footY = kneeY - rsin(angle) * 0x2A0;
    other->footX.val = footX;
    other->footY.val = footY;
    other->reach = hipX - footX;
    if (self->facingLeft) {
        other->reach = -(hipX - footX);
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B24CC);
#else
// In the air: both legs hang from the hip
static void func_us_801B24CC(void) {
    Entity* self = g_CurrentEntity;
    OlroxLeg* leg;
    s32 hipX;
    s32 hipY;
    s32 kneeX;
    s32 kneeY;
    s32 footX;
    s32 i;
    s16 angle;

    BEAST(self).legs[0]->footX.val = (self + 1)->posX.val;
    BEAST(self).legs[0]->footY.val = (self + 1)->posY.val;
    BEAST(self).legs[1]->footX.val = (self + 2)->posX.val;
    BEAST(self).legs[1]->footY.val = (self + 2)->posY.val;
    leg = BEAST(self).legs[0];
    for (i = 0; i < 2; i++) {
        hipX = self->posX.val;
        hipY = self->posY.val;
        angle = LegAngle(leg->upper.value);
        kneeX = hipX + rcos(angle) * 0x1C0;
        kneeY = hipY - rsin(angle) * 0x1C0;
        leg->kneeX.val = kneeX;
        leg->kneeY.val = kneeY;
        angle = LegAngle(leg->lower.value);
        footX = kneeX + rcos(angle) * 0x2A0;
        leg->footX.val = footX;
        leg->footY.val = kneeY - rsin(angle) * 0x2A0;
        leg->reach = footX - hipX;
        if (self->facingLeft) {
            leg->reach = -(footX - hipX);
        }
        leg = leg->other;
    }
}
#endif

#define COS_OFS(angle, len) ((rcos(angle) * (len)) >> 12)
#define SIN_OFS(angle, len) ((rsin(angle) * (len)) >> 12)

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B2690);
#else
// Draws a leg: thigh, shin and a claw that bends on the floor
static Primitive* func_us_801B2690(OlroxLeg* leg, Primitive* prim, u8 back) {
    Entity* self = g_CurrentEntity;
    s16 floor;
    u16 clut;
    u8 priority;
    s16 footX;
    s16 footY;
    s16 kneeX;
    s16 kneeY;
    s16 hipX;
    s16 hipY;
    s16 upper;
    s16 lower;
    s16 mid;
    s16 bend;
    s16 thickness;
    s16 hipX0;
    s16 hipY0;
    s16 hipX1;
    s16 hipY1;
    s16 kneeX0;
    s16 kneeY0;
    s16 kneeX1;
    s16 kneeY1;
    s16 ankleX0;
    s16 ankleY0;
    s16 ankleX1;
    s16 ankleY1;
    s16 heelX;
    s16 heelY;
    s16 toeX;
    s16 toeY;
    s16 tipX;
    s16 tipY;
    s16 clawX0;
    s16 clawY0;
    s16 clawX1;
    s16 clawY1;
    s16 joinX;
    s16 joinY;
    s16 endX;
    s16 endY;
    s16 angle;
    s16 dy;
    s16 side;

    if (!D_us_801CE5B0) {
        floor = g_Tilemap.scrollY.i.hi;
    } else {
        floor = 0xE0;
    }
    clut = back ? 0x20B : 0x218;
    priority = back * 2;

    footX = leg->footX.i.hi;
    footY = leg->footY.i.hi;
    kneeX = leg->kneeX.i.hi;
    kneeY = leg->kneeY.i.hi;
    hipX = self->posX.i.hi;
    hipY = self->posY.i.hi + 4;
    if (self->facingLeft) {
        upper = -leg->upper.value;
        lower = -leg->lower.value;
    } else {
        upper = leg->upper.value + 0x800;
        lower = leg->lower.value + 0x800;
    }
    mid = upper + (lower - upper) / 2;

    // the thigh's hip end
    hipX1 = hipX + COS_OFS(upper, 12);
    hipY1 = hipY + ((-(rsin(upper) * 12)) >> 12);
    hipX0 = hipX - COS_OFS(upper, 12);
    hipY0 = hipY - ((-(rsin(upper) * 12)) >> 12);

    // the knee, wider as it bends
    if (!self->facingLeft) {
        bend = upper - (lower - 0x800);
    } else {
        bend = leg->upper.value - (leg->lower.value - 0x800);
    }
    if (bend > 0x400) {
        bend = 0x400;
    }
    thickness = COS_OFS(bend, 24) + 12;
    kneeX0 = kneeX + COS_OFS(mid, 12);
    kneeY0 = kneeY + ((-(rsin(mid) * 12)) >> 12);
    kneeX1 = kneeX - ((thickness * rcos(mid)) >> 12);
    kneeY1 = kneeY - ((-(thickness * rsin(mid))) >> 12);

    // the ankle
    ankleX0 = footX + COS_OFS(lower, 6);
    ankleY0 = footY + ((-(rsin(lower) * 6)) >> 12);
    ankleX1 = footX + ((-(rcos(lower) * 6)) >> 12);
    ankleY1 = footY + SIN_OFS(lower, 6);

    prim->tpage = 0x14;
    prim->u0 = 0x4C;
    prim->u2 = 0x5C;
    prim->v0 = 0x72;
    prim->u1 = prim->u3 = 0x6C;
    prim->v1 = 0x64;
    prim->v2 = prim->v3 = 0x80;
    prim->clut = clut;
    prim->x0 = kneeX0;
    prim->x1 = hipX1;
    prim->x2 = kneeX1;
    prim->x3 = hipX0;
    prim->y0 = kneeY0;
    prim->y1 = hipY1;
    prim->y2 = kneeY1;
    prim->y3 = hipY0;
    prim->drawMode = DRAW_UNK02;
    prim->priority = priority + D_us_801CE56C;
    prim = prim->next;

    prim->tpage = 0x14;
    prim->u0 = 0x74;
    prim->u1 = 0x68;
    prim->u3 = 0x76;
    prim->v0 = 0x4E;
    prim->v1 = 0x30;
    prim->v2 = 0x46;
    prim->u2 = 0x80;
    prim->v3 = 0x20;
    prim->clut = clut;
    prim->x0 = ankleX0;
    prim->x1 = kneeX0;
    prim->x2 = ankleX1;
    prim->x3 = kneeX1;
    prim->y0 = ankleY0;
    prim->y1 = kneeY0;
    prim->y2 = ankleY1;
    prim->y3 = kneeY1;
    prim->drawMode = DRAW_UNK02;
    prim->priority = priority + D_us_801CE56C;
    prim = prim->next;

    // the claw: flattened when it reaches the floor
    angle = leg->lower.value;
    if (self->facingLeft) {
        angle = 0x800 - angle;
    }
    heelX = footX + COS_OFS(angle, 8);
    heelY = footY + ((-(rsin(angle) * 8)) >> 12);
    side = self->facingLeft ? 0x400 : -0x400;
    toeX = heelX + COS_OFS(angle + side, 4);
    toeY = heelY + ((-(rsin(angle + side) * 4)) >> 12);
    if ((s16)(toeY + floor) >= 0x1CE) {
        toeY = 0x1D0 - floor;
        angle = self->facingLeft ? 0x800 : 0;
    }
    tipY = toeY - SIN_OFS(angle - 0x800, 16);
    if ((s16)(tipY + floor) >= 0x1D0) {
        dy = toeY + floor - 0x1D0;
        if (!self->facingLeft) {
            tipX = toeX - SquareRoot0(0x100 - dy * dy);
        } else {
            tipX = toeX + SquareRoot0(0x100 - dy * dy);
        }
        angle = ratan2(-dy, toeX - tipX);
        tipY = 0x1D0 - floor;
        side = self->facingLeft ? -0x400 : 0x400;
        clawX1 = toeX + COS_OFS(angle + side, 16);
        clawY1 = toeY - SIN_OFS(angle + side, 16);
    } else {
        tipX = toeX + COS_OFS(angle - 0x800, 16);
        side = self->facingLeft ? -0x400 : 0x400;
        clawX1 = toeX + COS_OFS(angle + side, 16);
        clawY1 = toeY - SIN_OFS(angle + side, 16);
    }
    if (!self->facingLeft) {
        mid = angle / 2 + 0x400;
    } else {
        mid = angle / 2;
    }
    joinX = tipX + COS_OFS(mid, 16);
    joinY = tipY + ((-(rsin(mid) * 16)) >> 12);

    lower = angle + 0x800;
    clawY0 = tipY - ((rsin(lower) * 24) >> 12);
    if ((s16)(clawY0 + floor) >= 0x1D0) {
        dy = tipY + floor - 0x1D0;
        if (!self->facingLeft) {
            clawX0 = tipX - SquareRoot0(0x240 - dy * dy);
        } else {
            clawX0 = tipX + SquareRoot0(0x240 - dy * dy);
        }
        angle = ratan2(-dy, tipX - clawX0);
        clawY0 = 0x1D0 - floor;
    } else {
        clawX0 = tipX + ((rcos(lower) * 24) >> 12);
    }
    side = self->facingLeft ? -0x400 : 0x400;
    endX = clawX0 + COS_OFS(angle + side, 16);
    endY = clawY0 + ((-(rsin(angle + side) * 16)) >> 12);

    prim->tpage = 0x14;
    prim->u0 = 0xD8;
    prim->u1 = 0xF0;
    prim->u2 = 0xD8;
    prim->u3 = 0xF0;
    prim->v0 = prim->v1 = 0x50;
    prim->v2 = prim->v3 = 0x5E;
    prim->clut = clut;
    prim->x0 = endX;
    prim->x1 = joinX;
    prim->x2 = clawX0;
    prim->x3 = tipX;
    prim->y0 = endY;
    prim->y1 = joinY;
    prim->y2 = clawY0;
    prim->y3 = tipY;
    prim->drawMode = DRAW_UNK02;
    prim->priority = priority + D_us_801CE56C + 1;
    prim = prim->next;

    prim->tpage = 0x14;
    prim->clut = clut;
    prim->u0 = 0xF0;
    prim->u1 = 0xFF;
    prim->u2 = 0xF0;
    prim->u3 = 0xFF;
    prim->v0 = prim->v1 = 0x50;
    prim->v2 = prim->v3 = 0x5E;
    prim->x0 = joinX;
    prim->x1 = clawX1;
    prim->x2 = tipX;
    prim->x3 = toeX;
    prim->y0 = joinY;
    prim->y1 = clawY1;
    prim->y2 = tipY;
    prim->y3 = toeY;
    prim->drawMode = DRAW_UNK02;
    prim->priority = priority + D_us_801CE56C + 1;
    return prim->next;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B30AC);
#else
// Draws the neck and places the head (+3) and the mouth (+7) at its end
static Primitive* func_us_801B30AC(Primitive* prim) {
    Entity* self = g_CurrentEntity;
    s16* pose;
    s16 baseX;
    s16 baseY;
    s16 halfAngle;
    s16 angle;
    s16 dir;
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;
    s16 q1x;
    s16 q1y;
    s16 r1x;
    s16 r1y;
    s16 q2x;
    s16 q2y;
    s16 r2x;
    s16 r2y;
    s16 rootX0;
    s16 rootX1;
    s16 neck;
    Entity* part;

    pose = D_us_80181194[BEAST(self).headPose];
    if (!self->facingLeft) {
        baseX = self->posX.i.hi + pose[0];
    } else {
        baseX = self->posX.i.hi - pose[0];
    }
    baseY = self->posY.i.hi;
    if (!self->facingLeft) {
        rootX0 = baseX - 0x20;
        rootX1 = baseX + 8;
    } else {
        rootX0 = baseX + 0x20;
        rootX1 = baseX - 8;
    }

    // first segment, bent half as much as the head
    halfAngle = BEAST(self).neck.value / 2;
    if (!self->facingLeft) {
        angle = halfAngle;
        dir = halfAngle + 0x400;
    } else {
        angle = 0x800 - halfAngle;
        dir = angle - 0x400;
    }
    x1 = baseX + COS_OFS(dir, 16);
    y1 = baseY + ((-(rsin(dir) * 16)) >> 12);
    q1x = x1 + COS_OFS(angle, 8);
    q1y = y1 + ((-(rsin(angle) * 8)) >> 12);
    r1x = x1 + ((-(rcos(angle) * 32)) >> 12);
    r1y = y1 + ((rsin(angle) * 32) >> 12);

    // second segment
    neck = BEAST(self).neck.value;
    if (!self->facingLeft) {
        angle = neck;
        dir = neck + 0x400;
    } else {
        angle = 0x800 - neck;
        dir = angle - 0x400;
    }
    x2 = x1 + COS_OFS(dir, 32);
    y2 = y1 + ((-(rsin(dir) * 32)) >> 12);
    q2x = x2 + COS_OFS(angle, 8);
    q2y = y2 + ((-(rsin(angle) * 8)) >> 12);
    r2x = x2 + ((-(rcos(angle) * 32)) >> 12);
    r2y = y2 + ((rsin(angle) * 32) >> 12);

    prim->tpage = 0x14;
    prim->clut = 0x20B;
    prim->u1 = prim->u3 = (u8)pose[1];
    prim->u0 = prim->u2 = (u8)pose[1] + 0x28;
    prim->x0 = q2x;
    prim->x1 = r2x;
    prim->x2 = q1x;
    prim->x3 = r1x;
    prim->y0 = q2y;
    prim->y1 = r2y;
    prim->y2 = q1y;
    prim->y3 = r1y;
    prim->v0 = prim->v1 = (u8)pose[2];
    prim->v2 = prim->v3 = (u8)pose[2] + 0x20;
    prim->drawMode = DRAW_UNK02;
    prim->priority = D_us_801CE56C - 1;
    prim = prim->next;

    prim->tpage = 0x14;
    prim->clut = 0x20B;
    prim->u1 = prim->u3 = (u8)pose[1];
    prim->u0 = prim->u2 = (u8)pose[1] + 0x28;
    prim->v0 = prim->v1 = (u8)pose[2] + 0x20;
    prim->v2 = prim->v3 = (u8)pose[2] + 0x30;
    prim->x0 = q1x;
    prim->x1 = r1x;
    prim->x2 = rootX1;
    prim->x3 = rootX0;
    prim->y0 = q1y;
    prim->y1 = r1y;
    prim->y2 = baseY;
    prim->y3 = baseY;
    prim->drawMode = DRAW_UNK02;
    prim->priority = D_us_801CE56C - 1;
    prim = prim->next;

    part = self + 3;
    if (!self->facingLeft) {
        part->posX.i.hi = r2x + ((pose[3] * rcos(neck + pose[4])) >> 12);
    } else {
        part->posX.i.hi = r2x - ((pose[3] * rcos(neck + pose[4])) >> 12);
    }
    part->posY.i.hi = r2y + ((-(pose[3] * rsin(neck + pose[4]))) >> 12);
    if (BEAST(self).headPose == 2) {
        part->zPriority = D_us_801CE56C;
    } else {
        part->zPriority = D_us_801CE56C - 2;
    }
    part = self + 7;
    if (!self->facingLeft) {
        part->posX.i.hi = r2x + ((pose[5] * rcos(neck + pose[6])) >> 12);
    } else {
        part->posX.i.hi = r2x - ((pose[5] * rcos(neck + pose[6])) >> 12);
    }
    part->posY.i.hi = r2y + ((-(pose[5] * rsin(neck + pose[6]))) >> 12);
    return prim;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B365C);
#else
static OlroxLeg s_OlroxLegs[2];

static OlroxLeg* PlantedLeg(Entity* self) {
    if (!BEAST(self).plantedLeg) {
        return BEAST(self).legs[0];
    }
    return BEAST(self).legs[1];
}

// Olrox's second form
void func_us_801B365C(Entity* self) {
    static const OlroxLeg noLeg;
    Primitive* prim;
    Entity* entity;
    OlroxLeg* leg;
    OlroxLeg* other;
    OlroxLeg* far;
    s32 primIndex;
    s32 i;
    s32 n;
    s32 done;
    s16 absX;
    s16 dx;
    s16 dy;
    s16 push;
    s16 x;
    s16 y;

    FntPrint("step %x\n", self->step);
    FntPrint("step_s %x\n", self->step_s);
    FntPrint("abs_x %x\n", AbsX(self));
    if (!BEAST(self).dying && (self->flags & FLAG_DEAD)) {
        self->hitboxState = 0;
        if (self->step < 16) {
            func_us_801B1590(16);
        }
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180708);
        self->hitboxWidth = 8;
        self->hitboxHeight = 0x10;
        self->hitboxOffY = -8;
        D_us_801CE56C = 0xA0;
        self->animCurFrame = 0xB;
        self->zPriority = D_us_801CE56C;
        CreateEntityFromEntity(E_UNK_34, self, self + 1);
        CreateEntityFromEntity(E_UNK_34, self, self + 2);
        CreateEntityFromEntity(E_UNK_35, self, self + 3);
        CreateEntityFromEntity(E_UNK_36, self, self + 4);
        CreateEntityFromEntity(E_UNK_37, self, self + 7);
        CreateEntityFromEntity(E_UNK_38, self, self + 9);
        (self + 9)->params = 0;
        CreateEntityFromEntity(E_UNK_38, self, self + 8);
        (self + 8)->params = 1;
        primIndex = g_api.func_800EDB58(PRIM_GT4, 0x11);
        if (primIndex == -1) {
            self->step = 0;
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->flags |= FLAG_HAS_PRIMS;
        // PSX keeps the legs in these two primitives
        s_OlroxLegs[0] = s_OlroxLegs[1] = noLeg;
        prim->type = PRIM_TILE;
        prim->drawMode = DRAW_HIDE;
        BEAST(self).legs[0] = &s_OlroxLegs[0];
        (self + 1)->ext.olroxLimb.leg = BEAST(self).legs[0];
        prim = prim->next;
        prim->type = PRIM_TILE;
        prim->drawMode = DRAW_HIDE;
        BEAST(self).legs[1] = &s_OlroxLegs[1];
        (self + 2)->ext.olroxLimb.leg = BEAST(self).legs[1];
        prim = prim->next;
        BEAST(self).prim = prim;
        for (i = 0; i < 14; i++) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        BEAST(self).spritePrim = prim;
        prim->drawMode = DRAW_HIDE;
        CreateEntityFromEntity(E_UNK_3B, self, self + 5);
        (self + 5)->ext.olroxLimb.leg = BEAST(self).legs[0];
        CreateEntityFromEntity(E_UNK_3B, self, self + 6);
        (self + 6)->ext.olroxLimb.leg = BEAST(self).legs[1];
        // hits on any part count against the head
        entity = self;
        for (i = 0; i < 7; i++, entity++) {
            entity->nextPart = entity + 1;
            entity->parent = self + 3;
        }
        entity->nextPart = self;
        entity->parent = self + 3;
        (self + 3)->parent = NULL;
        BEAST(self).legs[0]->other = BEAST(self).legs[1];
        BEAST(self).legs[0]->upper.value = -0x200;
        BEAST(self).legs[0]->lower.value = 0x300;
        BEAST(self).legs[1]->other = BEAST(self).legs[0];
        BEAST(self).legs[1]->upper.value = -0x380;
        BEAST(self).legs[1]->lower.value = 0x100;
        BEAST(self).plantedLeg = 1;
        self->step = 1;
        // fallthrough

    case 1:
        // rise from what is left of the first form
        switch (self->step_s) {
        case 0:
            D_us_801CE56C = 0xD0;
            self->zPriority = D_us_801CE56C;
            func_us_801B21F0();
            self->step_s++;
            break;
        case 1:
            D_us_801CE56C = 0xA0;
            (self + 2)->posX.i.hi = -g_Tilemap.scrollX.i.hi;
            self->zPriority = D_us_801CE56C;
            (self + 2)->posY.i.hi = -0x20 - g_Tilemap.scrollY.i.hi;
            func_us_801B21F0();
            self->step_s++;
            break;
        case 2:
            if (!D_us_801CE5B0) {
                entity = self;
                for (i = 0; i < 8; i++, entity++) {
                    entity->flags |= FLAG_POS_CAMERA_LOCKED;
                }
                self->facingLeft = self->params;
                (self + 2)->posX.i.hi = self->posX.i.hi;
                (self + 2)->posY.i.hi = self->posY.i.hi;
                func_us_801B21F0();
                BEAST(self).timer = 0x20;
                func_us_801B1590(12);
            }
            break;
        }
        break;

    case 2:
        if (!--BEAST(self).timer) {
            func_us_801B1590(10);
        }
        func_us_801B21F0();
        break;

    case 10:
        // pick the next move
        func_us_801B21F0();
        absX = AbsX(self);
        dx = self->posX.i.hi - PLAYER.posX.i.hi;
        dy = self->posY.i.hi - PLAYER.posY.i.hi;
        if (BEAST(self).approach) {
            BEAST(self).approach = 0;
            if (abs(dx) < 0x80) {
                if (absX < 0x180) {
                    func_us_801B1590(4);
                } else {
                    func_us_801B1590(3);
                }
            } else if (self->facingLeft == (dx < 1)) {
                if (absX > 0xD0) {
                    func_us_801B1590(3);
                } else {
                    func_us_801B1590(4);
                }
            } else if (absX < 0x100) {
                func_us_801B1590(17);
            } else {
                func_us_801B1590(13);
            }
        } else {
            BEAST(self).approach = 1;
            if (self->facingLeft != (dx < 1)) {
                func_us_801B1590(13);
            } else if (abs(dx) < 0x60) {
                if (dy > 0) {
                    func_us_801B1590(18);
                } else if (Random() & 1) {
                    func_us_801B1590(6);
                } else {
                    func_us_801B1590(8);
                }
            } else if (dy > 0x40 || (Random() & 1)) {
                func_us_801B1590(9);
            } else {
                func_us_801B1590(8);
            }
        }
        break;

    case 3:
        // walk forward two steps
        if (!self->step_s) {
            if (AbsX(self) < 0xC0) {
                func_us_801B1590(10);
                break;
            }
            (self + 7)->step = 2;
            (self + 4)->step = 3;
            (self + 3)->step = 1;
            BEAST(self).stepCount = 0;
            self->step_s++;
        }
        if (AbsX(self) < 0x180 && ((self + 3)->hitFlags & 3) >= 2) {
            func_us_801B1590(14);
        }
        leg = PlantedLeg(self);
        func_us_801B1864(leg);
        other = leg->other;
        func_us_801B17BC(other);
        func_us_801B21F0();
        push = func_us_801B2178(other);
        far = other->other;
        if (other->onGround && far->reach < other->reach) {
            g_api.PlaySfx(0x647);
            BEAST(self).stepCount++;
            other->step = 0;
            other->footY.i.hi += push;
            far->step = 0;
            if (AbsX(self) >= 0xC0 && BEAST(self).stepCount < 2) {
                BEAST(self).plantedLeg ^= 1;
            } else {
                func_us_801B1590(10);
            }
        }
        break;

    case 4:
        // walk back two steps
        if (!self->step_s) {
            if (AbsX(self) > 0x180) {
                func_us_801B1590(10);
                break;
            }
            (self + 7)->step = 2;
            (self + 4)->step = 3;
            (self + 3)->step = 1;
            BEAST(self).stepCount = 0;
            self->step_s++;
        }
        if (AbsX(self) < 0x180 && ((self + 3)->hitFlags & 3) >= 2) {
            func_us_801B1590(14);
        }
        leg = PlantedLeg(self);
        func_us_801B19FC(leg);
        other = leg->other;
        func_us_801B1950(other);
        func_us_801B21F0();
        push = func_us_801B2178(other);
        far = other->other;
        if (other->onGround && other->reach < far->reach) {
            g_api.PlaySfx(0x647);
            BEAST(self).stepCount++;
            other->step = 0;
            other->footY.i.hi += push;
            far->step = 0;
            if (AbsX(self) <= 0x180 && BEAST(self).stepCount < 2) {
                BEAST(self).plantedLeg ^= 1;
            } else {
                func_us_801B1590(10);
            }
        }
        break;

    case 17:
        // jump over the player
        leg = PlantedLeg(self);
        far = leg->other;
        switch (self->step_s) {
        case 0:
            leg = BEAST(self).legs[0];
            far = leg->other;
            BEAST(self).plantedLeg = far->reach >= leg->reach;
            func_us_801B21F0();
            self->step_s++;
            // fallthrough
        case 1:
            func_us_801B21F0();
            if (func_us_801B1B30(leg)) {
                leg->step = 0;
                far->step = 0;
                self->velocityX = FIX(4);
                if (self->facingLeft) {
                    self->velocityX = FIX(-4);
                }
                self->velocityY = FIX(-3);
                (self + 4)->step = 8;
                self->step_s++;
            }
            break;
        case 2:
            self->posX.val += self->velocityX;
            self->posY.val += self->velocityY;
            self->velocityY += 0x2800;
            func_us_801B171C(far, -0x300, -0x180, 0x28);
            func_us_801B171C(leg, -0x280, 0x180, 0x28);
            func_us_801B24CC();
            push = func_us_801B2178(leg);
            if (leg->onGround) {
                g_api.PlaySfx(0x63D);
                leg->footY.i.hi += push;
                func_us_801B21F0();
                (self + 4)->step = 3;
                self->step_s++;
                func_us_801B1590(3);
            }
            break;
        }
        break;

    case 6:
        // bite
        leg = PlantedLeg(self);
        far = leg->other;
        func_us_801B21F0();
        if ((self + 7)->ext.olroxMouth.hurt) {
            func_us_801B1590(10);
            break;
        }
        switch (self->step_s) {
        case 0:
            leg = BEAST(self).legs[0];
            (self + 7)->step = 3;
            BEAST(self).headPose = 2;
            (self + 3)->step = 6;
            self->step_s++;
            BEAST(self).plantedLeg = leg->other->reach < leg->reach;
            break;
        case 1:
            if (func_us_801B1C60(leg) && (self + 7)->step == 1) {
                (self + 7)->step = 4;
                leg->step = 0;
                far->step = 0;
                self->step_s++;
            }
            break;
        case 2:
            if (func_us_801B1CE0(leg) && (self + 7)->step == 1) {
                (self + 7)->step = 4;
                leg->step = 0;
                far->step = 0;
                self->step_s++;
            }
            break;
        case 3:
            (self + 7)->step = 2;
            (self + 3)->step = 1;
            BEAST(self).headPose = 0;
            func_us_801B1590(10);
            break;
        }
        break;

    case 18:
        // pounce
        leg = PlantedLeg(self);
        far = leg->other;
        entity = self + 7;
        switch (self->step_s) {
        case 0:
            leg = BEAST(self).legs[0];
            BEAST(self).plantedLeg = leg->other->reach >= leg->reach;
            func_us_801B21F0();
            entity->step = 11;
            BEAST(self).headPose = 2;
            (self + 3)->step = 10;
            self->step_s++;
            break;
        case 1:
            func_us_801B21F0();
            if (entity->ext.olroxMouth.hurt) {
                func_us_801B1590(10);
                break;
            }
            if (func_us_801B1DDC(leg) && entity->step == 1) {
                entity->step = 12;
                (self + 4)->step = 6;
                leg->step = 0;
                far->step = 0;
                self->step_s++;
            }
            break;
        case 2:
            func_us_801B21F0();
            if (entity->ext.olroxMouth.hurt) {
                func_us_801B1590(10);
                break;
            }
            if (func_us_801B1E5C(leg)) {
                BEAST(self).headPose = 3;
                leg->step = 0;
                far->step = 0;
                self->velocityX = FIX(-1);
                if (self->facingLeft) {
                    self->velocityX = FIX(1);
                }
                absX = AbsX(self);
                if (absX < 0x100) {
                    self->velocityX /= 2;
                }
                if (absX < 0xC0) {
                    self->velocityX = 0;
                }
                self->velocityY = FIX(-6);
                self->step_s++;
            }
            break;
        case 3:
            self->posX.val += self->velocityX;
            self->posY.val += self->velocityY;
            self->velocityY += 0x2800;
            func_us_801B24CC();
            func_us_801B163C(&BEAST(self).neck, -0x100, 0x30);
            func_us_801B171C(leg, -0x80, 0x100, 0x10);
            func_us_801B171C(far, -0x280, 0x280, 0x10);
            if (self->velocityY > FIX(-1)) {
                (self + 4)->step = 6;
                BEAST(self).plantedLeg ^= 1;
                self->step_s++;
            }
            if ((self + 3)->rotate < 0x80) {
                (self + 3)->rotate += 8;
            }
            break;
        case 4:
            self->posX.val += self->velocityX;
            self->posY.val += self->velocityY;
            self->velocityY += 0x2800;
            func_us_801B163C(&BEAST(self).neck, 0x100, 0x20);
            func_us_801B171C(leg, -0x180, -0x100, 0x18);
            func_us_801B171C(far, -0x340, 0x2C0, 0x18);
            entity->step = 13;
            func_us_801B24CC();
            push = func_us_801B2178(leg);
            if (leg->onGround) {
                g_api.ShakeCamera(1);
                g_api.PlaySfx(0x63D);
                entity->step = 14;
                leg->footY.i.hi += push;
                BEAST(self).headPose = 0;
                (self + 4)->step = 3;
                self->step_s++;
            }
            if ((self + 3)->rotate >= -0x1FF) {
                (self + 3)->rotate -= 0x40;
            }
            break;
        case 5:
            if (func_us_801B1EDC(leg)) {
                leg->step = 0;
                far->step = 0;
                self->step_s++;
            }
            func_us_801B21F0();
            if ((self + 3)->rotate > 0) {
                (self + 3)->rotate -= 0x100;
            } else {
                (self + 3)->rotate = 0;
            }
            break;
        case 6:
            entity->step = 2;
            BEAST(self).headPose = 0;
            (self + 3)->step = 1;
            func_us_801B1590(10);
            break;
        }
        break;

    case 8:
        // lasers, fewer as the head loses health
        func_us_801B21F0();
        switch (self->step_s) {
        case 0:
            (self + 7)->step = 6;
            n = (self + 3)->hitPoints /
                (g_api.enemyDefs[56].hitPoints / 4 + 1);
            (self + 3)->step = 2;
            BEAST(self).laserIndex = 0;
            BEAST(self).laserActive = 0;
            self->step_s++;
            (self + 3)->ext.olroxHead.laserIndex = BEAST(self).laserIndex;
            BEAST(self).laserCount = 3 - n;
            break;
        case 1:
            func_us_801B163C(&BEAST(self).neck,
                             D_us_801811CC[BEAST(self).laserIndex][0], 0x10);
            if (BEAST(self).laserActive) {
                self->step_s++;
            }
            break;
        case 2:
            func_us_801B163C(&BEAST(self).neck,
                             D_us_801811CC[BEAST(self).laserIndex][1], 0x10);
            BEAST(self).headPose = 0;
            if (BEAST(self).laserActive) {
                break;
            }
            BEAST(self).laserIndex++;
            if ((BEAST(self).laserIndex & 1) && AbsX(self) < 0xF8) {
                BEAST(self).laserIndex = 8;
            }
            if (BEAST(self).laserCount < BEAST(self).laserIndex) {
                self->step_s++;
                break;
            }
            (self + 3)->step = 2;
            (self + 3)->ext.olroxLaser.unkA9 = 0;
            (self + 3)->ext.olroxHead.laserIndex = BEAST(self).laserIndex;
            self->step_s--;
            break;
        case 3:
            (self + 3)->step = 1;
            (self + 7)->step = 2;
            func_us_801B1590(10);
            break;
        }
        break;

    case 9:
        // fireballs, more of them as the head loses health
        func_us_801B21F0();
        switch (self->step_s) {
        case 0:
            BEAST(self).fireballsActive = 0;
            n = (self + 3)->hitPoints /
                (g_api.enemyDefs[56].hitPoints / 4 + 1);
            (self + 7)->step = 6;
            (self + 3)->step = 4;
            (self + 3)->step_s = 0;
            BEAST(self).fireballPower = 3 - n;
            self->step_s++;
            (self + 3)->ext.olroxLaser.scaleIndex = BEAST(self).fireballPower;
            break;
        case 1:
            if (BEAST(self).fireballsActive) {
                self->step_s++;
            }
            break;
        case 2:
            if (!BEAST(self).fireballsActive) {
                self->step_s++;
            }
            break;
        case 3:
            (self + 3)->step = 1;
            (self + 7)->step = 2;
            func_us_801B1590(10);
            break;
        }
        break;

    case 12:
        // roar
        leg = PlantedLeg(self);
        far = leg->other;
        func_us_801B21F0();
        switch (self->step_s) {
        case 0:
            (self + 7)->step = 7;
            self->step_s++;
            break;
        case 1:
            func_us_801B1F5C(leg);
            func_us_801B21F0();
            push = func_us_801B2178(far);
            if (far->onGround) {
                g_api.PlaySfx(0x647);
                far->footY.i.hi += push;
                leg->step = 0;
                far->step = 0;
                (self + 3)->step = 5;
                BEAST(self).roaring = 1;
                self->step_s++;
            }
            break;
        case 2:
            func_us_801B163C(&BEAST(self).neck, -0x100, 0x10);
            if (!BEAST(self).roaring) {
                self->step_s++;
            }
            break;
        case 3:
            (self + 3)->step = 1;
            (self + 7)->step = 2;
            func_us_801B1590(10);
            break;
        }
        break;

    case 13:
        // turn around
        leg = PlantedLeg(self);
        far = leg->other;
        func_us_801B21F0();
        switch (self->step_s) {
        case 0:
            (self + 7)->step = 8;
            (self + 4)->step = 4;
            self->step_s++;
            break;
        case 1:
            done = func_us_801B163C(&BEAST(self).neck, -0x100, 0x30);
            done += func_us_801B171C(leg, -0x40, 0x40, 0x20);
            if (done + func_us_801B171C(far, -0x100, 0x100, 0x20) == 3) {
                if (self->facingLeft) {
                    leg->footX.i.hi -= 0x30;
                } else {
                    leg->footX.i.hi += 0x30;
                }
                func_us_801B21F0();
                self->facingLeft ^= 1;
                self->step_s++;
            }
            break;
        case 2:
            (self + 7)->step = 2;
            (self + 4)->step = 3;
            func_us_801B1590(3);
            break;
        }
        break;

    case 14:
        // back off after being hit
        if (!self->step_s) {
            if (AbsX(self) > 0x180) {
                func_us_801B1590(10);
                break;
            }
            (self + 7)->step = 15;
            (self + 4)->step = 9;
            (self + 3)->step = 7;
            BEAST(self).stepCount = 0;
            self->step_s++;
        }
        leg = PlantedLeg(self);
        func_us_801B19FC(leg);
        other = leg->other;
        func_us_801B1950(other);
        func_us_801B21F0();
        push = func_us_801B2178(other);
        far = other->other;
        if (other->onGround && other->reach < far->reach) {
            g_api.PlaySfx(0x647);
            BEAST(self).stepCount++;
            other->step = 0;
            other->footY.i.hi += push;
            far->step = 0;
            if (AbsX(self) > 0x180 || BEAST(self).stepCount >= 2) {
                BEAST(self).stepCount = 0;
                func_us_801B1590(10);
                (self + 4)->step = 3;
            } else {
                BEAST(self).plantedLeg ^= 1;
            }
        }
        break;

    case 16:
        // defeated
        func_us_801B21F0();
        leg = PlantedLeg(self);
        switch (self->step_s) {
        case 0:
            entity = self + 1;
            for (i = 0; i < 9; i++, entity++) {
                entity->flags |= FLAG_DEAD;
            }
            g_api.PlaySfx(0x92);
            g_api.PlaySfx(0x819);
            (self + 3)->step = 5;
            (self + 3)->step_s = 0;
            (self + 4)->step_s = 0;
            (self + 4)->step = 9;
            currentMusicId = 0x317;
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_OLROX_DEFEAT, TIMEATTACK_SET_RECORD);
            BEAST(self).dying = 0;
            BEAST(self).timer = 0x60;
            g_api.PlaySfx(0x7C6);
            self->step_s++;
            break;
        case 1:
            func_us_801B1F5C(leg);
            func_us_801B21F0();
            far = leg->other;
            push = func_us_801B2178(far);
            if (far->onGround) {
                far->footY.i.hi += push;
                leg->step = 0;
                far->step = 0;
                self->step_s++;
            }
            // fallthrough
        case 2:
            // explosions all over the body, the neck shaking
            if (!(BEAST(self).timer & 1)) {
                if (!(BEAST(self).timer & 7)) {
                    g_api.PlaySfx(0x655);
                }
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    prim = BEAST(self).prim;
                    n = Random() & 0xF;
                    // PSX can step one past the list and read low memory
                    for (i = 0; i < n && prim->next != NULL; i++) {
                        prim = prim->next;
                    }
                    y = prim->y0;
                    x = prim->x0;
                    if (Random() & 1) {
                        CreateEntityFromCurrentEntity(E_LASER_EXPLOSION, entity);
                        entity->params = 1;
                    } else {
                        CreateEntityFromCurrentEntity(
                            E_INTENSE_EXPLOSION, entity);
                    }
                    entity->posX.i.hi = x;
                    entity->posY.i.hi = y;
                    entity->params |= (D_us_801CE56C + 0x10) << 8;
                }
                x = BEAST(self).neck.value + 0x18;
            } else {
                x = BEAST(self).neck.value - 0x18;
            }
            func_us_801B15BC(&BEAST(self).neck.value, x, 0x18);
            if (!--BEAST(self).timer) {
                self->step_s++;
            }
            break;
        case 3:
            entity = self + 10;
            (self + 3)->step = 8;
            (self + 4)->step = 5;
            (self + 7)->step = 10;
            (self + 3)->step_s = 0;
            (self + 4)->step_s = 0;
            (self + 7)->step_s = 0;
            (self + 9)->step_s = 0;
            (self + 9)->step = 3;
            (self + 8)->step_s = 0;
            (self + 8)->step = 3;
            DestroyEntity(entity);
            CreateEntityFromEntity(E_UNK_3C, self, entity);
            prim = BEAST(self).spritePrim;
            entity->facingLeft = self->facingLeft;
            func_us_801B13A8(prim);
            g_api.func_800EB758(self->posX.i.hi, self->posY.i.hi, self,
                                self->drawFlags, (POLY_GT4*)prim,
                                self->facingLeft);
            prim->tpage = 0x14;
            prim->clut = 0x20B;
            prim->drawMode = DRAW_UNK02;
            prim->priority = self->zPriority;
            self->step_s++;
            break;
        case 4:
            self->palette = 0x219;
            self->unk5A = 0x54;
            BEAST(self).spritePrim->drawMode = DRAW_HIDE;
            BEAST(self).dying = 1;
            g_api.PlaySfx(0x817);
            self->step_s++;
            break;
        case 5:
            self->palette = 0x219;
            self->unk5A = 0x54;
            if (!(g_Timer & 0x1F)) {
                g_api.PlaySfx(0x817);
            }
            if (BEAST(self).dying == 2) {
                if (AbsX(self) > 0x80) {
                    (self + 4)->step = 3;
                    BEAST(self).stepCount = 0;
                    self->step_s++;
                } else {
                    self->step_s = 7;
                }
            }
            break;
        case 6:
            // limp back
            func_us_801B15BC(&(self + 3)->rotate, -0x100, 6);
            (self + 3)->animCurFrame = 1;
            func_us_801B163C(&BEAST(self).neck, 0x180, 8);
            func_us_801B20F4(leg);
            other = leg->other;
            func_us_801B2044(other);
            func_us_801B21F0();
            push = func_us_801B2178(other);
            far = other->other;
            if (other->onGround && far->reach < other->reach) {
                g_api.PlaySfx(0x647);
                BEAST(self).stepCount++;
                other->step = 0;
                other->footY.i.hi += push;
                far->step = 0;
                if (BEAST(self).stepCount >= 2) {
                    (self + 4)->step_s = 0;
                    (self + 4)->step = 5;
                    self->step_s++;
                } else {
                    BEAST(self).plantedLeg ^= 1;
                }
            }
            break;
        case 7:
            prim = BEAST(self).spritePrim;
            BEAST(self).dying = 3;
            (self + 10)->posX.i.hi = self->posX.i.hi;
            (self + 10)->posY.i.hi = self->posY.i.hi;
            func_us_801B13A8(prim);
            g_api.func_800EB758(self->posX.i.hi, self->posY.i.hi, self,
                                self->drawFlags, (POLY_GT4*)prim,
                                self->facingLeft);
            prim->tpage = 0x15;
            prim->clut = 0x219;
            prim->drawMode = DRAW_UNK02;
            prim->priority = self->zPriority;
            self->step_s++;
            break;
        case 8:
            entity = self;
            for (i = 0; i < 10; i++, entity++) {
                DestroyEntity(entity);
            }
            return;
        }
        break;
    }

    prim = func_us_801B2690(BEAST(self).legs[0], BEAST(self).prim, 0);
    prim = func_us_801B2690(BEAST(self).legs[0]->other, prim, 1);
    func_us_801B30AC(prim);
    if (D_us_801CE5B0) {
        for (prim = BEAST(self).prim; prim != NULL; prim = prim->next) {
            prim->drawMode &= ~DRAW_UNK02;
        }
    }
    if (self->palette & 0x8000) {
        for (prim = BEAST(self).prim; prim != NULL; prim = prim->next) {
            prim->clut = self->palette & 0xFFF;
        }
    }
    if (BEAST(self).dying) {
        for (prim = BEAST(self).prim; prim != NULL; prim = prim->next) {
            prim->tpage = 0x15;
            prim->clut = 0x219;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B5470);
#else
// A foot, one of the skeleton's hitboxes
void func_us_801B5470(Entity* self) {
    OlroxLeg* leg;

    self->facingLeft = (self - 1)->facingLeft;
    if (self->flags & FLAG_DEAD) {
        self->hitboxState = 0;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180708);
        self->hitboxWidth = 0xC;
        self->hitboxHeight = 4;
        self->hitboxOffX = -0xC;
        self->animCurFrame = 0;
        break;
    case 1:
        leg = self->ext.olroxLimb.leg;
        self->posX.val = leg->footX.val;
        self->posY.val = leg->footY.val;
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B551C);
#else
// The head: aims and fires the lasers and fireballs
void func_us_801B551C(Entity* self) {
    Entity* beast = self - 3;
    Entity* entity;
    Primitive* prim;
    s16* pattern;
    s32 primIndex;
    s32 done;
    s16 dx;
    s16 aim;

    self->facingLeft = beast->facingLeft;
    self->palette = beast->palette;
    if (self->flags & FLAG_DEAD) {
        self->hitboxState = 0;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180714);
        self->hitboxWidth = 0xA;
        self->hitboxHeight = 0xA;
        self->animCurFrame = 1;
        self->drawFlags |= ENTITY_ROTATE;
        self->rotate = -0x100;
        self->zPriority = D_us_801CE56C - 2;
        break;

    case 1:
        AnimateEntity(D_us_8018120C, self);
        func_us_801B15BC(&self->rotate, -0x80, 8);
        self->step_s = 0;
        break;

    case 7:
        AnimateEntity(D_us_80181220, self);
        func_us_801B15BC(&self->rotate, 0xC0, 8);
        if (!self->poseTimer && !(self->pose & 3)) {
            g_api.PlaySfx(0x819);
        }
        self->step_s = 0;
        break;

    case 6:
        self->animCurFrame = 1;
        func_us_801B15BC(&self->rotate, -0x100, 0x20);
        break;

    case 5:
        // roar, the head shaking
        if (func_us_801B15BC(&self->rotate, 0x180, 0x10)) {
            if (!AnimateEntity(D_us_80181234, self)) {
                BEAST(beast).roaring = 0;
                self->step = 1;
            }
            if (self->pose == 2 && !self->poseTimer) {
                g_api.PlaySfx(0x819);
            }
            if (self->pose >= 3) {
                if (self->poseTimer & 2) {
                    self->posX.i.hi++;
                } else {
                    self->posX.i.hi--;
                }
            }
        } else {
            self->pose = 0;
            self->poseTimer = 0;
        }
        break;

    case 2:
        // aim a laser
        self->animCurFrame = 2;
        if (func_us_801B15BC(&self->rotate,
                             D_us_801811CC[self->ext.olroxHead.laserIndex][2],
                             0x10)) {
            self->ext.olroxLaser.unkA9 = 1;
            self->step = 3;
            self->step_s = 0;
        }
        break;

    case 3:
        // fire it
        switch (self->step_s) {
        case 0:
            entity =
                AllocEntity(&g_Entities[64], &g_Entities[TOTAL_ENTITY_COUNT]);
            if (entity == NULL) {
                // as on PSX: the laser takes the skeleton's own slot
                entity = beast;
                BEAST(beast).laserActive = 1;
                self->ext.olroxLaser.unkA9 = 0;
                self->step_s++;
            }
            CreateEntityFromEntity(E_LASER_ATTACK, self, entity);
            entity->ext.olroxLaser.next = self;
            entity->params = self->ext.olroxHead.laserIndex & 1;
            entity->facingLeft = self->facingLeft;
            BEAST(beast).laserActive = 1;
            self->animCurFrame = 3;
            self->step_s++;
            break;
        case 1:
            if (!self->ext.olroxLaser.unkA9) {
                BEAST(beast).laserActive = 0;
            }
            break;
        }
        break;

    case 4:
        // aim at the player
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 4;
            dx = self->posX.i.hi - PLAYER.posX.i.hi;
            if (self->facingLeft) {
                dx = -dx;
            }
            aim = -ratan2(-(s16)(self->posY.i.hi - PLAYER.posY.i.hi), dx) +
                  0x100;
            if (aim > 0x380) {
                aim = 0x380;
            }
            if (aim < -0x180) {
                aim = 0x180; // sic
            }
            self->ext.olroxHead.aim = aim;
            self->step_s++;
            // fallthrough
        case 1:
            aim = self->ext.olroxHead.aim;
            done = func_us_801B15BC(&self->rotate, aim, 0x20);
            if (done + func_us_801B163C(&BEAST(beast).neck,
                                        -(aim + 0x180) / 2, 0x20) ==
                2) {
                BEAST(beast).fireballsActive = 1;
                self->step_s = 0;
                self->step = 9;
            }
            break;
        }
        break;

    case 9:
        // sweep fireballs around the aim
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 4;
            pattern = D_us_801811EC[self->ext.olroxLaser.scaleIndex];
            self->ext.olroxHead.duration = pattern[0];
            self->ext.olroxHead.sweepDown = 0;
            self->step_s++;
            self->ext.olroxHead.fireTimer = pattern[3];
            // fallthrough
        case 1:
            pattern = D_us_801811EC[self->ext.olroxLaser.scaleIndex];
            if (!--self->ext.olroxHead.fireTimer) {
                self->ext.olroxHead.fireTimer = pattern[3];
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_FIREBALL_ATTACK, self, entity);
                    entity->ext.olroxLaser.next = self;
                    entity->ext.olroxLaser.scaleIndex =
                        self->ext.olroxLaser.scaleIndex;
                    entity->facingLeft = self->facingLeft;
                }
            }
            if (self->ext.olroxHead.sweepDown) {
                self->rotate += pattern[2];
            } else {
                self->rotate -= pattern[2];
            }
            if (self->rotate < self->ext.olroxHead.aim - pattern[1]) {
                self->ext.olroxHead.sweepDown = 1;
            }
            if (self->ext.olroxHead.aim + pattern[1] < self->rotate) {
                self->ext.olroxHead.sweepDown = 0;
            }
            if (self->rotate < -0x180) {
                self->ext.olroxHead.sweepDown = 1;
            }
            if (self->rotate > 0x380) {
                self->ext.olroxHead.sweepDown = 0;
            }
            BEAST(beast).neck.value = -(self->rotate + 0x180) / 2;
            if (!--self->ext.olroxHead.duration) {
                self->step_s = 0;
                self->step = 1;
                BEAST(beast).fireballsActive = 0;
            }
            break;
        }
        break;

    case 10:
        self->animCurFrame = 1;
        break;

    case 8:
        // defeated: becomes a sprite of the dissolving body
        switch (self->step_s) {
        case 0:
            primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
            if (primIndex != -1) {
                prim = &g_PrimBuf[primIndex];
                self->primIndex = primIndex;
                self->ext.olroxHead.prim = prim;
                self->flags |= FLAG_HAS_PRIMS;
                func_us_801B13A8(prim);
                g_api.func_800EB758(self->posX.i.hi, self->posY.i.hi, self,
                                    self->drawFlags, (POLY_GT4*)prim,
                                    self->facingLeft);
                prim->tpage = 0x14;
                prim->clut += 0x20B;
                prim->drawMode = DRAW_UNK02;
                prim->priority = self->zPriority;
            }
            self->step_s++;
            break;
        case 1:
            self->palette = 0x219;
            self->unk5A = 0x54;
            if (BEAST(beast).dying) {
                self->ext.olroxHead.prim->drawMode = DRAW_HIDE;
                self->step_s++;
            }
            break;
        case 2:
            self->palette = PAL_FLAG(0x219);
            self->unk5A = 0x54;
            if (BEAST(beast).dying == 3) {
                prim = self->ext.olroxHead.prim;
                self->animCurFrame = 3;
                func_us_801B13A8(prim);
                g_api.func_800EB758(self->posX.i.hi, self->posY.i.hi, self,
                                    self->drawFlags, (POLY_GT4*)prim,
                                    self->facingLeft);
                prim->tpage = 0x15;
                prim->clut = 0x219;
                prim->drawMode = DRAW_UNK02;
                prim->priority = self->zPriority;
                self->step_s++;
            }
            break;
        }
        break;
    }

    self->hitboxOffX = -((rcos(self->rotate) * 12) >> 12);
    self->hitboxOffY = -((rsin(self->rotate) * 12) >> 12);
    if (!D_us_801CE5B0 && self->step < 8 && !(Random() & 0x3F)) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_OLROX_DROOL, self, entity);
            entity->ext.olroxDrool.entity = self;
            entity->params = (Random() & 7) + 0x10;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B5D6C);
#else
// moves each tail segment toward its target by step; true once all there
static s32 func_us_801B5D6C(OlroxServo* segments, s16* targets, s16 step) {
    s32 done = 1;
    s32 i;
    s16 value;

    for (i = 0; i < 8; i++, segments++, targets++) {
        value = segments->value;
        if (abs(value - *targets) < step) {
            segments->value = *targets;
        } else {
            done = 0;
            if (*targets < value) {
                segments->value = value - step;
            }
            if (segments->value < *targets) {
                segments->value += step;
            }
        }
    }
    return done;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B5E08);
#else
// eases each tail segment toward its target; true once all there
static s32 func_us_801B5E08(OlroxServo* segments, s16* targets, s16 speed) {
    s32 done = 1;
    s32 i;

    for (i = 0; i < 8; i++, segments++, targets++) {
        if (!func_us_801B163C(segments, *targets, speed)) {
            done = 0;
        }
    }
    return done;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B5E8C);
#else
// Draws the tail, thinner toward its tip
static void func_us_801B5E8C(void) {
    Entity* self = g_CurrentEntity;
    OlroxServo* segment;
    Primitive* prim;
    s32 i;
    s16 x;
    s16 y;
    s16 prevAngle;
    s16 angle;
    s16 dir;
    s16 top;
    s16 bottom;
    s16 scale;
    s16 len;
    s16 topX = 0;
    s16 topY = 0;
    s16 bottomX = 0;
    s16 bottomY = 0;
    s16 prevTopX;
    s16 prevTopY;
    s16 prevBottomX;
    s16 prevBottomY;

    x = self->posX.i.hi;
    y = self->posY.i.hi;
    segment = self->ext.olroxTail.segments;
    prim = self->ext.olroxTail.prim;
    prevAngle = -0x180;
    for (i = 0; i < 9; i++, segment++) {
        angle = segment->value;
        dir = (prevAngle + angle) / 2 + 0x400;
        top = ((rcos((s16)(dir - angle)) * 6) >> 12) + 8;
        bottom = 8 - ((rcos((s16)(dir - angle)) * 6) >> 12);
        prevTopX = topX;
        prevTopY = topY;
        prevBottomX = bottomX;
        prevBottomY = bottomY;
        if (!i) {
            bottom = 0xC;
            dir = 0x260;
        }
        scale = self->ext.olroxTail.length / 2 + 0x800;
        top = (top * scale) >> 12;
        bottom = (bottom * scale) >> 12;
        if (self->facingLeft) {
            dir = 0x800 - dir;
        }
        topX = x + ((top * rcos(dir)) >> 12);
        topY = y - ((top * rsin(dir)) >> 12);
        bottomX = x - ((bottom * rcos(dir)) >> 12);
        bottomY = y + ((bottom * rsin(dir)) >> 12);
        len = (self->ext.olroxTail.length * 11) >> 12;
        if (!self->facingLeft) {
            x += (len * rcos(segment->value)) >> 12;
        } else {
            x -= (len * rcos(segment->value)) >> 12;
        }
        prevAngle = segment->value;
        y -= (len * rsin(segment->value)) >> 12;
        if (i) {
            prim->tpage = 0x14;
            prim->clut = 0x20B;
            prim->x0 = prevTopX;
            prim->x2 = prevBottomX;
            prim->y0 = prevTopY;
            prim->y2 = prevBottomY;
            prim->x3 = bottomX;
            prim->x1 = topX;
            prim->y3 = bottomY;
            prim->y1 = topY;
            prim = prim->next;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B619C);
#else
// The tail
void func_us_801B619C(Entity* self) {
    Entity* beast = self - 4;
    Primitive* prim;
    s32 primIndex;
    s32 i;
    u8 u;

    self->facingLeft = beast->facingLeft;
    self->palette = beast->palette;
    if (!D_us_801CE5B0) {
        self->posX.val = beast->posX.val;
        self->posY.val = beast->posY.val;
        if (!self->facingLeft) {
            self->posX.i.hi += 4;
        } else {
            self->posX.i.hi -= 4;
        }
    }
    if (self->flags & FLAG_DEAD) {
        self->hitboxState = 0;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180708);
        self->hitboxState = 0;
        self->animCurFrame = 0;
        self->flags |= FLAG_POS_CAMERA_LOCKED;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 8);
        if (primIndex == -1) {
            self->step = 0;
            break;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.olroxTail.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        for (i = 0, u = 0x80; i < 8; i++, u += 0xB) {
            prim->u0 = prim->u2 = u;
            prim->tpage = 0x14;
            prim->clut = 0x20B;
            prim->u1 = prim->u3 = u + 0xB;
            prim->v0 = prim->v1 = 0x50;
            prim->v2 = prim->v3 = 0x60;
            prim->priority = 0xA1;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        }
        for (i = 0; i < 8; i++) {
            self->ext.olroxTail.segments[i].value = -0x180;
        }
        self->ext.olroxTail.length = 0;
        // fallthrough
    case 1:
        if (!D_us_801CE5B0) {
            self->step++;
        }
        break;
    case 2:
        // grow
        if (self->ext.olroxTail.length < 0x1000) {
            self->ext.olroxTail.length += 0x200;
        } else {
            self->ext.olroxTail.length = 0x1000;
            self->step++;
        }
        break;
    case 3:
        if (func_us_801B5E08(self->ext.olroxTail.segments,
                             D_us_801812D0[self->ext.olroxTail.pose], 0x20)) {
            self->ext.olroxTail.pose ^= 1;
        }
        break;
    case 4:
        func_us_801B5E08(self->ext.olroxTail.segments, D_us_80181288, 0x20);
        break;
    case 6:
        func_us_801B5E08(self->ext.olroxTail.segments, D_us_80181298, 0x60);
        break;
    case 7:
        func_us_801B5E08(self->ext.olroxTail.segments, D_us_801812A8, 0x80);
        break;
    case 8:
        func_us_801B5E08(self->ext.olroxTail.segments, D_us_801812B8, 0x60);
        break;
    case 9:
        if (func_us_801B5D6C(self->ext.olroxTail.segments,
                             D_us_801812D0[self->ext.olroxTail.pose], 0x40)) {
            self->ext.olroxTail.pose ^= 1;
        }
        break;
    }

    func_us_801B5E8C();
    if (self->palette & 0x8000) {
        for (prim = self->ext.olroxTail.prim; prim != NULL;
             prim = prim->next) {
            prim->clut = self->palette & 0xFFF;
        }
    }
    if (BEAST(beast).dying) {
        for (prim = self->ext.olroxTail.prim; prim != NULL;
             prim = prim->next) {
            prim->tpage = 0x15;
            prim->clut = 0x219;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B6520);
#else
// Eases the four jaw angles; each argument packs a target (low half) and a
// speed (high half), as PSX passes them. True once all four are there.
static s32 func_us_801B6520(u32 lower0, u32 lower1, u32 upper0, u32 upper1) {
    ET_OlroxMouth* mouth = &g_CurrentEntity->ext.olroxMouth;
    s32 done;

    done = func_us_801B163C(
        &mouth->jaws[1][0], (s16)upper0, (s16)(upper0 >> 16));
    done += func_us_801B163C(
        &mouth->jaws[1][1], (s16)upper1, (s16)(upper1 >> 16));
    done += func_us_801B163C(
        &mouth->jaws[0][0], (s16)lower0, (s16)(lower0 >> 16));
    done += func_us_801B163C(
        &mouth->jaws[0][1], (s16)lower1, (s16)(lower1 >> 16));
    return done == 4;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B65C0);
#else
// Draws one jaw and puts its fang (mouth +1 or +2) at the tip
static Primitive* func_us_801B65C0(
    s16 x, s16 y, OlroxServo* jaw, Primitive* prim, u8 upper) {
    Entity* self = g_CurrentEntity;
    Entity* fang;
    u8* open;
    u16 clut;
    u8 priority;
    s16 angle;
    s16 len;
    s16 baseX;
    s16 baseY;
    s16 jointX;
    s16 jointY;
    s16 ax0;
    s16 ay0;
    s16 ax1;
    s16 ay1;
    s16 bx0;
    s16 by0;
    s16 bx1;
    s16 by1;
    s16 cx0;
    s16 cy0;
    s16 cx1;
    s16 cy1;
    s16 tipX;
    s16 tipY;

    if (upper) {
        clut = 0x20B;
        open = &self->ext.olroxMouth.open[1];
    } else {
        clut = 0x218;
        open = &self->ext.olroxMouth.open[0];
    }
    priority = upper * 6;

    angle = jaw[0].value;
    if (self->facingLeft) {
        angle = 0x800 - angle;
    }
    ax1 = x + ((-(rcos(angle) * 8)) >> 12);
    ax0 = x + ((rcos(angle) * 8) >> 12);
    ay1 = y + ((rsin(angle) * 8) >> 12);
    ay0 = y + ((-(rsin(angle) * 8)) >> 12);

    len = (*open + 0x100) / 8;
    if (!self->facingLeft) {
        baseX = x + ((len * rcos(jaw[0].value - 0x400)) >> 12);
    } else {
        baseX = x - ((len * rcos(jaw[0].value - 0x400)) >> 12);
    }
    baseY = y + ((-len * rsin(jaw[0].value - 0x400)) >> 12);

    angle = jaw[1].value;
    if (self->facingLeft) {
        angle = 0x800 - angle;
    }
    bx0 = baseX + ((-(rcos(angle) * 6)) >> 12);
    by0 = baseY + ((rsin(angle) * 6) >> 12);
    bx1 = baseX + ((rcos(angle) * 6) >> 12);
    by1 = baseY + ((-(rsin(angle) * 6)) >> 12);

    prim->tpage = 0x14;
    prim->u0 = prim->u1 = 0xC8;
    prim->u2 = 0xD4;
    prim->u3 = 0xDA;
    prim->v0 = 0x1E;
    prim->v1 = 4;
    prim->v2 = 0x20;
    prim->v3 = 4;
    prim->clut = clut;
    prim->x0 = bx0;
    prim->x1 = ax1;
    prim->x2 = bx1;
    prim->x3 = ax0;
    prim->y0 = by0;
    prim->y1 = ay1;
    prim->y2 = by1;
    prim->y3 = ay0;
    prim->drawMode = DRAW_UNK02;
    prim->priority = priority + D_us_801CE56C - 3;
    prim = prim->next;

    if (!self->facingLeft) {
        jointX = baseX + ((rcos(jaw[1].value + 0x400) * 6) >> 12);
    } else {
        jointX = baseX - ((rcos(jaw[1].value + 0x400) * 6) >> 12);
    }
    jointY = baseY + ((-(rsin(jaw[1].value + 0x400) * 6)) >> 12);
    angle = jaw[1].value;
    if (self->facingLeft) {
        angle = 0x800 - angle;
    }
    bx0 = jointX + ((-(rcos(angle) * 6)) >> 12);
    by0 = jointY + ((rsin(angle) * 6) >> 12);
    bx1 = jointX + ((rcos(angle) * 6) >> 12);
    by1 = jointY + ((-(rsin(angle) * 6)) >> 12);

    len = ((*open + 0x100) * 24) / 256;
    if (!self->facingLeft) {
        tipX = jointX + ((len * rcos(jaw[1].value - 0x400)) >> 12);
    } else {
        tipX = jointX - ((len * rcos(jaw[1].value - 0x400)) >> 12);
    }
    tipY = jointY + ((-len * rsin(jaw[1].value - 0x400)) >> 12);
    cx0 = tipX + ((-(rcos(angle) * 6)) >> 12);
    cy0 = tipY + ((rsin(angle) * 6) >> 12);
    cx1 = tipX + ((rcos(angle) * 6) >> 12);
    cy1 = tipY + ((-(rsin(angle) * 6)) >> 12);

    if (upper) {
        fang = self + 2;
        open = &self->ext.olroxMouth.fangFrame[1];
    } else {
        fang = self + 1;
        open = &self->ext.olroxMouth.fangFrame[0];
    }
    fang->posX.i.hi = tipX;
    fang->posY.i.hi = tipY;
    fang->animCurFrame = *open;
    if (self->ext.olroxMouth.bite) {
        fang->rotate = -(jaw[1].value + 0x300);
    } else {
        fang->rotate = 0;
    }

    prim->tpage = 0x14;
    prim->clut = clut;
    prim->u0 = 0x88;
    prim->u1 = 0x9C;
    prim->u2 = 0x8C;
    prim->u3 = 0xA2;
    prim->v0 = 0x74;
    prim->v1 = 0x6D;
    prim->v2 = 0x80;
    prim->v3 = 0x77;
    prim->x0 = cx0;
    prim->x1 = bx0;
    prim->x2 = cx1;
    prim->x3 = bx1;
    prim->y0 = cy0;
    prim->y1 = by0;
    prim->y2 = cy1;
    prim->y3 = by1;
    prim->drawMode = DRAW_UNK02;
    prim->priority = priority + D_us_801CE56C - 3;
    return prim->next;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B6CA4);
#else
// The mouth: two jaws, each with a fang the player can break
void func_us_801B6CA4(Entity* self) {
    Entity* beast = self - 7;
    ET_OlroxMouth* mouth = &self->ext.olroxMouth;
    Primitive* prim;
    s32 primIndex;
    s32 done;
    s16 speed;
    u32 lower0;
    u32 upper0;

    self->facingLeft = beast->facingLeft;
    self->palette = beast->palette;
    if (!BEAST(beast).dying) {
        mouth->hurt = 0;
        if ((self + 1)->flags & FLAG_DEAD) {
            mouth->hurt = 2;
        }
        if ((self + 2)->flags & FLAG_DEAD) {
            mouth->hurt |= 1;
        }
        if (mouth->hurt && self->step != 10) {
            self->step = 9;
        }
    }
    if (self->flags & FLAG_DEAD) {
        self->hitboxState = 0;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180708);
        self->hitboxWidth = 0xC;
        self->hitboxHeight = 8;
        self->hitboxOffX = 0;
        self->hitboxOffY = 8;
        self->animCurFrame = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 6);
        if (primIndex == -1) {
            self->step = 0;
            break;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        mouth->prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        for (; prim != NULL; prim = prim->next) {
            prim->drawMode = DRAW_HIDE;
        }
        mouth->jaws[1][0].value = 0x180;
        mouth->jaws[1][1].value = -0x180;
        mouth->jaws[0][0].value = 0x100;
        mouth->jaws[0][1].value = -0x80;
        mouth->fangFrame[1] = 0x1D;
        mouth->fangFrame[0] = 0x21;
        mouth->bite = 0;
        // fallthrough
    case 1:
        self->step_s = 0;
        (self + 2)->attack = g_api.enemyDefs[55].attack;
        break;

    case 2:
        // breathe
        mouth->bite = 0;
        switch (self->step_s) {
        case 0:
            mouth->fangFrame[1] = 0x1D;
            mouth->fangFrame[0] = 0x21;
            if (func_us_801B6520(0x100100, 0x10FD00, 0x10FFC0, 0x10FE00)) {
                self->step_s = 1;
            }
            break;
        case 1:
            if (func_us_801B6520(0x100000, 0x10FD80, 0x100080, 0x10FD00)) {
                self->step_s = 0;
            }
            break;
        }
        break;

    case 3:
        mouth->fangFrame[1] = 0x1F;
        mouth->fangFrame[0] = 0x20;
        mouth->bite = 0;
        if (func_us_801B6520(0x100100, 0x10FD80, 0x100100, 0x10FD00)) {
            self->step = 1;
        }
        break;

    case 4:
        // bite
        mouth->fangFrame[1] = 0x1F;
        mouth->fangFrame[0] = 0x20;
        mouth->bite = 0;
        done = func_us_801B6520(0x600200, 0x60FD00, 0x60FDC0, 0x60FDC0);
        if (mouth->jaws[1][0].value < 0x80) {
            BEAST(beast).headPose = 0;
        }
        if (mouth->jaws[1][0].value < -0x80) {
            BEAST(beast).headPose = 3;
        }
        (self + 2)->attack = g_api.enemyDefs[57].attack;
        if (done) {
            if ((s8)mouth->open[1] < 0x60) {
                mouth->open[1] += 0x10;
            } else {
                self->step = 5;
                (self + 2)->attack = g_api.enemyDefs[55].attack;
                PlaySfxPositional(0x7C8);
            }
        }
        break;

    case 5:
        mouth->bite = 0;
        if ((s8)mouth->open[1] > 0) {
            mouth->open[1] -= 0xC;
        } else {
            mouth->open[1] = 0;
            self->step = 1;
        }
        break;

    case 11:
        mouth->bite = 1;
        mouth->fangFrame[1] = 0x1F;
        mouth->fangFrame[0] = 0x20;
        if (func_us_801B6520(0x20FE00, 0x20FC00, 0x180280, 0x18FE00)) {
            self->step = 1;
        }
        break;

    case 12:
        mouth->bite = 1;
        mouth->fangFrame[1] = 0x1D;
        mouth->fangFrame[0] = 0x21;
        if ((s8)mouth->open[1] < 0x40) {
            mouth->open[1] += 8;
        }
        done = func_us_801B6520(0x400180, 0x40FE00, 0x60FB00, 0x78F880);
        (self + 2)->attack = g_api.enemyDefs[58].attack;
        if (done) {
            self->step = 1;
        }
        break;

    case 13:
        mouth->bite = 1;
        mouth->open[1] -= 0xC;
        if (mouth->open[1] & 0x80) {
            mouth->open[1] = 0;
        }
        mouth->fangFrame[1] = 0x1F;
        mouth->fangFrame[0] = 0x21;
        if (func_us_801B6520(0x400000, 0x40FF80, 0x400100, 0x50FE00)) {
            self->step = 1;
        }
        break;

    case 14:
        mouth->bite = 1;
        mouth->fangFrame[1] = 0x1F;
        mouth->fangFrame[0] = 0x20;
        mouth->open[1] -= 0xC;
        if (mouth->open[1] & 0x80) {
            mouth->open[1] = 0;
        }
        if (func_us_801B6520(0x200100, 0x20FD00, 0x20FF80, 0x20FE00)) {
            self->step = 1;
        }
        break;

    case 6:
        mouth->fangFrame[1] = 0x1F;
        mouth->fangFrame[0] = 0x20;
        mouth->bite = 0;
        if (func_us_801B6520(0x200000, 0x20FC80, 0x200080, 0x20FD80)) {
            self->step = 1;
        }
        break;

    case 7:
        mouth->bite = 1;
        mouth->fangFrame[1] = 0x1D;
        mouth->fangFrame[0] = 0x20;
        if (func_us_801B6520(0x100000, 0x10FE00, 0x100100, 0x10FF00)) {
            self->step = 1;
        }
        break;

    case 8:
        mouth->bite = 1;
        mouth->fangFrame[1] = 0x1F;
        mouth->fangFrame[0] = 0x21;
        if (func_us_801B6520(0x200000, 0x20FEC0, 0x200180, 0x20FF00)) {
            self->step = 1;
        }
        break;

    case 9:
        // a fang is broken: the jaw it was on hangs open
        mouth->bite = 1;
        mouth->fangFrame[0] = 0x20;
        speed = mouth->hurt == 2 ? 0x18 : 0x10;
        lower0 = (mouth->hurt == 2 ? 0 : 0x200) | (speed << 16);
        mouth->fangFrame[1] = 0x1D;
        func_us_801B6520(lower0, (u16)-0x100 | (speed << 16),
                         (mouth->hurt == 1 ? 0 : 0x200) |
                             ((mouth->hurt == 1 ? 0x18 : 0x10) << 16),
                         (u16)-0x100 |
                             ((mouth->hurt == 1 ? 0x18 : 0x10) << 16));
        if (!mouth->hurt) {
            self->step = 1;
        }
        break;

    case 15:
        mouth->bite = 1;
        mouth->fangFrame[1] = 0x1D;
        mouth->fangFrame[0] = 0x20;
        if (func_us_801B6520(0x200000, 0x20FE00, 0x200080, 0x20FF00)) {
            self->step = 1;
        }
        break;

    case 10:
        // defeated
        switch (self->step_s) {
        case 0:
            if (BEAST(beast).dying == 2) {
                self->step_s++;
            }
            break;
        case 1:
            mouth->bite = 1;
            mouth->fangFrame[1] = 0x1D;
            mouth->fangFrame[0] = 0x21;
            if (func_us_801B6520(0x10FF00, 0x10FC00, 0x10FE00, 0x10FD00)) {
                self->step_s++;
            }
            if (BEAST(beast).dying == 3) {
                self->step_s = 3;
            }
            break;
        case 2:
            mouth->bite = 1;
            mouth->fangFrame[1] = 0x1D;
            mouth->fangFrame[0] = 0x21;
            func_us_801B6520(0xA0100, 0xAFE00, 0x60000, 0x6FF00);
            if (BEAST(beast).dying == 3) {
                self->step_s = 3;
            }
            break;
        }
        break;
    }

    prim = func_us_801B65C0(self->posX.i.hi, self->posY.i.hi, mouth->jaws[1],
                            mouth->prim, 1);
    func_us_801B65C0(
        self->posX.i.hi, self->posY.i.hi, mouth->jaws[0], prim, 0);
    if (self->palette & 0x8000) {
        for (prim = mouth->prim; prim != NULL; prim = prim->next) {
            prim->clut = self->palette & 0xFFF;
        }
    }
    if (BEAST(beast).dying) {
        for (prim = mouth->prim; prim != NULL; prim = prim->next) {
            prim->tpage = 0x15;
            prim->clut = 0x219;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B76E4);
#else
// A fang (params 1: upper jaw). Breaking one makes the mouth flinch; it
// grows back.
void func_us_801B76E4(Entity* self) {
    Entity* beast;
    Primitive* prim;
    s32 primIndex;

    beast = self->params ? self - 8 : self - 9;
    self->facingLeft = beast->facingLeft;
    if (beast->palette & 0x8000) {
        self->palette = beast->palette;
    } else if (!(self->flags & 0xF)) {
        self->palette = 0x20B;
    }
    if (self->flags & FLAG_DEAD) {
        self->hitboxState = 0;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180708);
        self->hitPoints = 0x80;
        self->hitboxWidth = 0xC;
        self->hitboxHeight = 6;
        self->parent = NULL;
        self->drawFlags |= ENTITY_ROTATE;
        if (self->params) {
            self->animCurFrame = 0x20;
            self->zPriority = D_us_801CE56C - 4;
        } else {
            self->animCurFrame = 0x1F;
            self->zPriority = D_us_801CE56C + 2;
        }
        break;

    case 1:
        if (self->flags & FLAG_DEAD) {
            self->hitboxState = 0;
            self->step = 2;
            self->step_s = 0;
        }
        break;

    case 2:
        switch (self->step_s) {
        case 0:
            MakeExplosions();
            self->ext.olroxFang.timer = 0x100;
            self->animCurFrame = 0;
            self->scaleX = self->scaleY = 0;
            self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY;
            self->step_s++;
            self->flags &= ~0xFF;
            break;
        case 1:
            if (!--self->ext.olroxFang.timer) {
                self->step_s++;
            }
            break;
        case 2:
            // grow back
            self->scaleX = self->scaleY = self->scaleX + 2;
            if (self->scaleX > 0x100) {
                self->hitPoints = 0x80;
                self->drawFlags = ENTITY_ROTATE;
                self->hitboxState = 3;
                self->palette = 0x20B;
                self->step = 1;
                self->hitEffect = 0;
                self->step_s = 0;
                self->flags &= ~FLAG_DEAD;
            }
            break;
        }
        break;

    case 3:
        // defeated
        self->drawFlags = ENTITY_ROTATE;
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
            if (primIndex == -1) {
                self->step_s = 2;
                break;
            }
            prim = &g_PrimBuf[primIndex];
            self->primIndex = primIndex;
            self->ext.olroxFang.prim = prim;
            self->flags |= FLAG_HAS_PRIMS;
            func_us_801B13A8(prim);
            g_api.func_800EB758(self->posX.i.hi, self->posY.i.hi, self,
                                self->drawFlags, (POLY_GT4*)prim,
                                self->facingLeft);
            prim->tpage = 0x14;
            prim->clut += 0x20B;
            prim->drawMode = DRAW_UNK02;
            prim->priority = self->zPriority;
            self->step_s++;
            break;
        case 1:
            self->palette = 0x219;
            self->unk5A = 0x54;
            if (BEAST(beast).dying) {
                self->ext.olroxFang.prim->drawMode = DRAW_HIDE;
                self->step_s++;
            }
            break;
        case 2:
            self->palette = PAL_FLAG(0x219);
            self->unk5A = 0x54;
            if (BEAST(beast).dying == 3) {
                prim = self->ext.olroxFang.prim;
                func_us_801B13A8(prim);
                g_api.func_800EB758(self->posX.i.hi, self->posY.i.hi, self,
                                    self->drawFlags, (POLY_GT4*)prim,
                                    self->facingLeft);
                prim->tpage = 0x15;
                prim->clut = 0x219;
                prim->drawMode = DRAW_UNK02;
                prim->priority = self->zPriority;
                self->step_s++;
            }
            break;
        }
        break;
    }

    if (self->params) {
        beast = self - 8;
        self->zPriority = D_us_801CE56C - 4;
    } else {
        beast = self - 9;
        self->zPriority = D_us_801CE56C + 2;
    }
    if (beast->flags & FLAG_DEAD) {
        self->hitboxState = 0;
    }
    self->hitboxOffX = -((rcos(self->rotate - 0x100) * 10) >> 12);
    self->hitboxOffY = -((rsin(self->rotate - 0x100) * 10) >> 12);
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B7BAC);
#else
// A knee, one of the skeleton's hitboxes
void func_us_801B7BAC(Entity* self) {
    OlroxLeg* leg;

    if (self->flags & FLAG_DEAD) {
        self->hitboxState = 0;
    }
    if (!self->step) {
        InitializeEntity(D_us_80180708);
        self->hitboxOffY = 4;
        self->hitboxWidth = 4;
        self->animCurFrame = 0;
        self->hitboxHeight = 0x10;
    }
    leg = self->ext.olroxLimb.leg;
    self->posX.i.hi = leg->kneeX.i.hi;
    self->posY.i.hi = leg->kneeY.i.hi;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B7C44);
#else
// the first run of count hidden primitives
static Primitive* func_us_801B7C44(Primitive* prim, u8 count) {
    Primitive* start;
    s32 i;

    while (prim != NULL) {
        if (prim->drawMode & DRAW_HIDE) {
            start = prim;
            for (i = 1; i < count; i++) {
                prim = prim->next;
                if (prim == NULL) {
                    return NULL;
                }
                if (!(prim->drawMode & DRAW_HIDE)) {
                    break;
                }
            }
            if (i == count) {
                return start;
            }
        }
        prim = prim->next;
    }
    return NULL;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/313A8", func_us_801B7CC8);
#else
static Primitive* CopyPrims(Primitive* dst, Primitive* src, u16 clut) {
    Primitive* next;

    for (; src != NULL; src = src->next) {
        next = dst->next;
        *dst = *src;
        dst->next = next;
        dst->clut = clut;
        dst = dst->next;
    }
    return dst;
}

// The skeleton shatters, then a flash leaves the Heart Max Up
void func_us_801B7CC8(Entity* self) {
    Entity* beast = self - 10;
    ET_OlroxDeath* death = &self->ext.olroxDeath;
    Entity* entity;
    Primitive* prim;
    Primitive* dst;
    Primitive* next;
    s32 primIndex;
    s32 i;
    s32 count;
    s16 x;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180708);
        self->zPriority = D_us_801CE56C + 8;
        self->animCurFrame = 0;
        self->flags |= FLAG_POS_CAMERA_LOCKED;
        primIndex = g_api.func_800EDB58(PRIM_GT4, 0x108);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        dst = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        death->prim = dst;
        self->flags |= FLAG_HAS_PRIMS;
        // copy what every part draws
        entity = beast;
        for (i = 0; i < 10; i++, entity++) {
            for (prim = entity->ext.prim; prim != NULL; prim = prim->next) {
                next = dst->next;
                *dst = *prim;
                dst->next = next;
                dst->clut = 0x20B;
                dst->r0 = dst->r1 = dst->r2 = dst->r3 = 0x80;
                dst->priority += 8;
                dst = dst->next;
            }
        }
        death->spare = dst;
        for (; dst != NULL; dst = dst->next) {
            dst->drawMode = DRAW_HIDE;
        }
        break;

    case 1:
        // shatter
        count = 0;
        for (prim = death->prim; prim != death->spare; prim = prim->next) {
            if (prim->drawMode & DRAW_HIDE) {
                continue;
            }
            count++;
            if (!(Random() & 0xF)) {
                dst = func_us_801B7C44(death->spare, 4);
                if (dst != NULL) {
                    prim->r0 = 0xB0;
                    prim->r1 = 0x98;
                    prim->r2 = 0xA0;
                    prim->r3 = 0x88;
                    UnkRecursivePrimFunc2(prim, 1, dst, (u8*)SP(0));
                    prim->drawMode = DRAW_HIDE;
                }
            }
        }
        for (prim = death->spare; prim != NULL; prim = prim->next) {
            if (prim->drawMode & DRAW_HIDE) {
                continue;
            }
            count++;
            if (prim->r0 > 0x80) {
                prim->r0--;
                if (prim->r0 == 0x80) {
                    dst = func_us_801B7C44(death->spare, 4);
                    if (dst != NULL) {
                        prim->r0 = 0x20;
                        prim->r1 = 0x14;
                        prim->r2 = 0x18;
                        prim->r3 = 0x10;
                        UnkRecursivePrimFunc2(prim, 1, dst, (u8*)SP(0));
                        prim->drawMode = DRAW_HIDE;
                        prim->p3 = 0;
                    } else {
                        prim->r0 += (Random() & 0x1F) + 2;
                    }
                }
            } else {
                prim->r0--;
                if (!prim->r0) {
                    prim->p3 = 0;
                    prim->drawMode = DRAW_HIDE;
                } else if (prim->r0 < 0x10) {
                    prim->y0 -= (prim->r0 >> 1) - 8;
                    prim->y1 -= (prim->r0 >> 1) - 8;
                    prim->y2 -= (prim->r0 >> 1) - 8;
                    prim->y3 -= (prim->r0 >> 1) - 8;
                }
            }
        }
        if (!count) {
            BEAST(beast).dying = 2;
            self->step++;
        }
        break;

    case 2:
        // the skeleton limps back, then turns into a sprite to dissolve
        if (BEAST(beast).dying != 3) {
            break;
        }
        prim = death->prim;
        death->flash = prim;
        prim->type = PRIM_G4;
        x = self->posX.i.hi;
        prim->r0 = prim->g0 = prim->b0 = 0;
        prim->r2 = prim->g2 = prim->b2 = 0xF0;
        prim->y2 = prim->y3 = 0xF0;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r2);
        prim->y0 = prim->y1 = 0;
        prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                         DRAW_UNK02 | DRAW_TRANSP;
        prim->x0 = prim->x1 = prim->x2 = prim->x3 = x;
        prim->priority = D_us_801CE56C + 0x10;
        dst = prim->next;
        death->prim = dst;
        for (i = 0; i < 6; i++) {
            dst = CopyPrims(dst, (beast + D_us_801812D8[i])->ext.prim, 0x219);
        }
        death->spare = dst;
        for (; dst != NULL; dst = dst->next) {
            dst->drawMode = DRAW_HIDE;
        }
        death->count = 0x28;
        self->step++;
        break;

    case 3:
        // a flash widens over the screen
        prim = death->flash;
        prim->y2 = prim->y3 = 0xF0;
        prim->y0 = prim->y1 = 0;
        prim->x0 = prim->x2 = prim->x0 - 6;
        prim->x1 = prim->x3 = prim->x1 + 6;
        if ((prim->x0 & 0xF000) && (prim->x3 & 0xF00)) {
            prim->x0 = prim->x2 = 0;
            prim->x1 = prim->x3 = 0x100;
            prim->drawMode &= ~DRAW_UNK02;
            death->timer = 1;
            self->step++;
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_3E, self, entity);
                entity->facingLeft = self->facingLeft;
                death->next = entity;
                return;
            }
        }
        break;

    case 4:
        // break the sprite up, one piece every four frames
        if (!--death->timer) {
            g_api.PlaySfx(0x60D);
            prim = death->prim;
            death->timer = 4;
            for (i = 0; i < death->count; i++) {
                prim = prim->next;
            }
            dst = FindFirstUnkPrim2(death->spare, 4);
            if (dst != NULL) {
                prim->p3 = 1;
                prim->r0 = 0x24;
                prim->r1 = 0x28;
                prim->r2 = 0x2C;
                prim->r3 = 0x21;
                UnkRecursivePrimFunc2(prim, 1, dst, (u8*)SP(0));
                prim->drawMode |= DRAW_HIDE;
                if (--death->count < 0) {
                    g_api.PlaySfx(0x636);
                    self->step++;
                }
            }
        }
        // fallthrough
    case 5:
        death->flash->drawMode = (g_Timer & 1) ? 0x71 : DRAW_HIDE;
        count = death->count;
        death->frame++;
        for (prim = death->spare; prim != NULL; prim = prim->next) {
            if (prim->drawMode & DRAW_HIDE) {
                continue;
            }
            prim->r0--;
            count++;
            switch ((prim->r0 & 0xF0) >> 4) {
            default:
                if (death->frame & 1) {
                    prim->x0++;
                    prim->x1++;
                    prim->x2++;
                    prim->x3++;
                } else {
                    prim->x0--;
                    prim->x1--;
                    prim->x2--;
                    prim->x3--;
                }
                break;
            case 1:
                if (prim->r0 == 0x10) {
                    dst = FindFirstUnkPrim2(death->spare, 4);
                    if (dst != NULL) {
                        prim->p3 = 3;
                        prim->r0 = 0xD;
                        prim->r1 = 6;
                        prim->r2 = 0xE;
                        prim->r3 = 0x14;
                        UnkRecursivePrimFunc2(prim, 1, dst, (u8*)SP(0));
                        prim->drawMode = DRAW_HIDE;
                        prim->p3 = 0;
                    } else {
                        prim->r0 += Random() & 0xF;
                    }
                }
                prim->y0--;
                prim->y1--;
                prim->y2--;
                prim->y3--;
                break;
            case 0:
                prim->y0 += (prim->r0 >> 1) - 8;
                prim->y1 += (prim->r0 >> 1) - 8;
                prim->y2 += (prim->r0 >> 1) - 8;
                prim->y3 += (prim->r0 >> 1) - 8;
                if (!prim->r0) {
                    prim->drawMode = DRAW_HIDE;
                    prim->p3 = 0;
                }
                break;
            }
        }
        if (count < 0) {
            BEAST(beast).dying = 2;
            death->next->step++;
            self->step++;
        }
        break;

    case 6:
        // the flash closes
        prim = death->flash;
        prim->drawMode = 0x71;
        prim->x0 += 2;
        prim->x1 -= 2;
        prim->y0 = prim->y1 = 0;
        prim->y2 = prim->y3 = 0xF0;
        prim->x2 = prim->x0;
        prim->x3 = prim->x1;
        if (prim->x1 < prim->x0) {
            prim->drawMode = DRAW_HIDE;
            death->timer = 1;
            self->step++;
        }
        break;

    case 7:
        entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_LIFE_UP_SPAWN, self, entity);
            entity->params = 0;
            D_us_80181190 = 1;
            stopMusicFlag = 1;
            entity->posY.i.hi -= 0x28;
            currentMusicId = 0x317;
            self->step++;
        }
        break;

    case 8:
        if (!g_api.func_80131F68()) {
            stopMusicFlag = 0;
            g_api.PlaySfx(currentMusicId);
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
EInit D_us_80180708 = { 0x8007, 1, 0x50, 0x20B, 0x37 };
EInit D_us_80180714 = { 0x8007, 1, 0x50, 0x20B, 0x38 };
s16 D_us_80181194[4][7] = {
    { 2, 0, 0x50, 0x10, -0x360, 0xE, -0xE0 },
    { 2, 0x28, 0x50, 0x10, -0x360, 0xE, -0xE0 },
    { -1, 0x40, 0x20, 0x10, -0x360, 0x13, -0xB0 },
    { -2, 0xD7, 0, 0xE, -0x2C0, 0x12, -0x160 },
};
s16 D_us_801811CC[4][4] = {
    { 0x40, 0x180, -0x200, 0 },
    { 0x100, -0xC0, -0x30, 0 },
    { 0x80, 0x200, -0x180, 0 },
    { 0xC0, -0x80, -0x10, 0 },
};
s16 D_us_801811EC[4][4] = {
    { 0x80, 0x140, 4, 0x14 },
    { 0x100, 0x1C0, 8, 0x10 },
    { 0x180, 0x200, 0xC, 0xC },
    { 0x200, 0x300, 0x1C, 4 },
};
u8 D_us_8018120C[0x14] = { 0x30, 1, 0xC, 1, 0xC, 2, 0xC, 1, 0xC, 2, 0xC, 1, 0xC, 1, 0xC, 1, 0, 0, 0, 0 };
u8 D_us_80181220[0x14] = { 8, 1, 8, 2, 8, 3, 8, 2, 8, 4, 8, 5, 8, 6, 8, 3, 0, 0, 0, 0 };
u8 D_us_80181234[0x14] = { 8, 1, 8, 2, 8, 3, 8, 4, 8, 3, 8, 5, 8, 3, 8, 2, 0xFF, 0, 0, 0 };
s16 D_us_80181288[8] = { -0x1A0, -0x200, -0x280, -0x2C0, -0x300, -0x340, -0x380, -0x400 };
s16 D_us_80181298[8] = { -0x180, -0x1C0, -0x240, -0x2C0, -0x300, -0x320, -0x300, -0x200 };
s16 D_us_801812A8[8] = { -0x80, 0, 0x80, 0x100, 0x200, 0x240, 0x200, 0x180 };
s16 D_us_801812B8[8] = { -0x180, -0x280, -0x380, -0x400, -0x480, -0x500, -0x580, -0x600 };
s16* D_us_801812D0[2] = { D_us_80181268, D_us_80181278 };
s16 D_us_801812D8[6] = { 0, 3, 7, 8, 9, 4 };
s32 D_us_801CE56C;
s16 D_us_80181268[8] = { -0x340, -0x220, -0x1C0, -0x80, 0x180, 0x240, 0x40, -0x140 };
s16 D_us_80181278[8] = { -0x100, 0x1C0, 0x300, 0x180, 0, -0x200, -0x400, -0x480 };
// end pc_import_data.py
// clang-format on
#endif
