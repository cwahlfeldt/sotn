// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo0.h"

// PC only: the PSX build takes this table from the data segment

void EntityBreakable(Entity* self);
void EntityExplosion(Entity* self);
void EntityPrizeDrop(Entity* self);
void EntityDamageDisplay(Entity* self);
void EntityRedDoor(Entity* self);
void EntityIntenseExplosion(Entity* self);
void EntitySoulStealOrb(Entity* self);
void EntityRoomForeground(Entity* self);
void EntityStageNamePopup(Entity* self);
void EntityEquipItemDrop(Entity* self);
void EntityRelicOrb(Entity* self);
void EntityPersistentItemDrop(Entity* self);
void EntityEnemyBlood(Entity* self);
void EntityMessageBox(Entity* self);
void EntityDummy(Entity* self);
void EntityBackgroundBlock(Entity* self);
void EntityLockCamera(Entity* self);
void EntityUnkId13(Entity* self);
void EntityExplosionVariants(Entity* self);
void EntityGreyPuff(Entity* self);
void EntitySkelerang(Entity* self);
void EntitySkelerangBoomerang(Entity* self);
void EntitySkelerangUnknown(Entity* self);
void func_us_801B3D8C(Entity* self);
void func_us_801B3F30(Entity* self);
void func_us_801B4148(Entity* self);
void func_us_801B41A4(Entity* self);
void func_us_801B4210(Entity* self);
void Entity3DBackgroundHouse(Entity* self);
void Entity3DHouseSpawner(Entity* self);
void func_us_801B4DA4(Entity* self);
void EntitySpikes(Entity* self);
void EntitySpikesParts(Entity* self);
void EntitySpikesDust(Entity* self);
void func_us_801ABFE0(Entity* self);
void func_us_801AC894(Entity* self);
void func_us_801AC54C(Entity* self);
void EntityMarionette(Entity* self);
void EntityBigRedFireball(Entity* self);
void EntityPrisoner(Entity* self);
void func_us_801B001C(Entity* self);
void func_us_801AD338(Entity* self);
void EntityOlroxAfterImage(Entity* self);
void func_us_801AFAF4(Entity* self);
void func_us_801AF8C0(Entity* self);
void func_us_801AF604(Entity* self);
void func_us_801AF31C(Entity* self);
void func_us_801AE858(Entity* self);
void func_us_801B0930(Entity* self);
void func_us_801B365C(Entity* self);
void func_us_801B5470(Entity* self);
void func_us_801B551C(Entity* self);
void func_us_801B619C(Entity* self);
void func_us_801B6CA4(Entity* self);
void func_us_801B76E4(Entity* self);
void EntityLaserAttack(Entity* self);
void EntityFireballAttack(Entity* self);
void func_us_801B7BAC(Entity* self);
void func_us_801B7CC8(Entity* self);
void EntityRealOlroxDrool(Entity* self);
void func_us_801BA4AC(Entity* self);
void EntityLifeUpSpawn(Entity* self);
void EntityOwlKnight(Entity* self);
void EntityOwl(Entity* self);
void EntityOwlKnightSword(Entity* self);
void EntityOwlTarget(Entity* self);
void EntityBloodyZombie(Entity* self);
void EntityBloodDrips(Entity* self);
void EntityBloodSplatter(Entity* self);
void func_us_801B72E8(Entity* self);
void EntitySpikesDamage(Entity* self);
void EntityBossDoors(Entity* self);
void EntitySealedDoor(Entity* self);
void EntityChair(Entity* self);

PfnEntityUpdate EntityUpdates[] = {
    EntityBreakable,
    EntityExplosion,
    EntityPrizeDrop,
    EntityDamageDisplay,
    EntityRedDoor,
    EntityIntenseExplosion,
    EntitySoulStealOrb,
    EntityRoomForeground,
    EntityStageNamePopup,
    EntityEquipItemDrop,
    EntityRelicOrb,
    EntityPersistentItemDrop,
    EntityEnemyBlood,
    EntityMessageBox,
    EntityDummy,
    EntityDummy,
    EntityBackgroundBlock,
    EntityLockCamera,
    EntityUnkId13,
    EntityExplosionVariants,
    EntityGreyPuff,
    EntitySkelerang,
    EntitySkelerangBoomerang,
    EntitySkelerangUnknown,
    func_us_801B3D8C,
    func_us_801B3F30,
    func_us_801B4148,
    func_us_801B41A4,
    func_us_801B4210,
    Entity3DBackgroundHouse,
    Entity3DHouseSpawner,
    func_us_801B4DA4,
    EntitySpikes,
    EntitySpikesParts,
    EntitySpikesDust,
    func_us_801ABFE0,
    func_us_801AC894,
    func_us_801AC54C,
    EntityMarionette,
    EntityBigRedFireball,
    EntityPrisoner,
    func_us_801B001C,
    func_us_801AD338,
    EntityOlroxAfterImage,
    func_us_801AFAF4,
    func_us_801AF8C0,
    func_us_801AF604,
    func_us_801AF31C,
    func_us_801AE858,
    func_us_801B0930,
    func_us_801B365C,
    func_us_801B5470,
    func_us_801B551C,
    func_us_801B619C,
    func_us_801B6CA4,
    func_us_801B76E4,
    EntityLaserAttack,
    EntityFireballAttack,
    func_us_801B7BAC,
    func_us_801B7CC8,
    EntityRealOlroxDrool,
    func_us_801BA4AC,
    EntityLifeUpSpawn,
    EntityOwlKnight,
    EntityOwl,
    EntityOwlKnightSword,
    EntityOwlTarget,
    EntityBloodyZombie,
    EntityBloodDrips,
    EntityBloodSplatter,
    func_us_801B72E8,
    EntitySpikesDamage,
    EntityBossDoors,
    EntitySealedDoor,
    EntityChair,
};
