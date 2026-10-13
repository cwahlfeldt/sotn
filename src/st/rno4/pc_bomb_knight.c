// SPDX-License-Identifier: AGPL-3.0-or-later
// PC only: RNZ1's Bomb Knight and Rock Knight, which RNO4 shares, with
// RNO4's entity ids, EInits and palettes. It stands in for the seven
// func_us_*_from_rnz1 functions in unk_58A30.c.
#include "rno4.h"

#define RNZ1_H // RNO4's header stands in for RNZ1's

#define EntityBombKnight func_us_801BBE58_from_rnz1
#define EntityThrownBomb func_us_801BC650_from_rnz1
#define EntityBombFuseSparks func_us_801BCA5C_from_rnz1
#define EntityThrownRock func_us_801BCB9C_from_rnz1
#define EntityRockPiece func_us_801BCD80_from_rnz1
#define EntityKnightDeathParts func_us_801BCE4C_from_rnz1
#define EntityBombKnightArm func_us_801BCFC8_from_rnz1

#define E_BOMB_KNIGHT E_UNK_48
#define E_THROWN_BOMB E_UNK_49
#define E_DEATH_PARTS E_UNK_4A
#define E_FUSE_SPARKS E_UNK_4B
#define E_THROWN_ROCK E_UNK_4C
#define E_ROCK_PIECE E_UNK_4D
#define E_BOMB_KNIGHT_ARM E_UNK_4E

#define g_EInitBomb D_us_80180BCC
#define g_EInitKnightDebris D_us_80180BD8
#define g_EInitRock D_us_80180BE4

#define BOMB_PAL 0x228
#define BOMB_BLAST_ANIMSET ANIMSET_OVL(6)
#define BOMB_BLAST_PAL 0x22C
#define BOMB_BLAST_UNK5A 0x4E

#include "../rnz1/e_bomb_knight.c"
