// SPDX-License-Identifier: AGPL-3.0-or-later
// GTE library calls that psyz does not implement yet.
#include <game.h>

// Multiplies m0 by m1 into the constant rotation matrix; m0 is left untouched
// and the current translation is kept.
MATRIX* SetMulMatrix(MATRIX* m0, MATRIX* m1) {
    MATRIX r = *m0;
    MulMatrix(&r, m1);
    SetRotMatrix(&r);
    return m0;
}

// PsyQ's CompMatrix(m0, m1, m2): m2 = m0 * m1 and t2 = m0 * t1 + t0, leaving
// m0 in the GTE rotation and translation registers. psyz declares it with
// other parameter types, hence the casts.
MATRIX* CompMatrix(SVECTOR* rot, VECTOR* trans, MATRIX* out) {
    MATRIX* m0 = (MATRIX*)rot;
    MATRIX* m1 = (MATRIX*)trans;
    MATRIX r;
    s64 v;
    s32 t[3];
    int i, j, k;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            v = 0;
            for (k = 0; k < 3; k++) {
                v += (s64)m0->m[i][k] * m1->m[k][j];
            }
            v >>= 12;
            r.m[i][j] = v < -0x8000 ? -0x8000 : v > 0x7FFF ? 0x7FFF : v;
        }
        v = 0;
        for (k = 0; k < 3; k++) {
            v += (s64)m0->m[i][k] * m1->t[k];
        }
        t[i] = (s32)(v >> 12) + m0->t[i];
    }
    SetRotMatrix(m0);
    SetTransMatrix(m0);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            out->m[i][j] = r.m[i][j];
        }
        out->t[i] = t[i];
    }
    return out;
}

unsigned int Psyz_GteDataRead(unsigned idx);
unsigned int Psyz_GteCtrlRead(unsigned idx);

// PsyQ macros psyz has no wrapper for: store IR0 and the FLAG register
void gte_stdp(void* p) { *(s32*)p = (s32)Psyz_GteDataRead(8); }
#ifndef gte_stflg // newer psyz has it
void gte_stflg(void* p) { *(u32*)p = Psyz_GteCtrlRead(31); }
#endif
