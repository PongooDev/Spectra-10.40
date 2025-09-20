#pragma once
#include "framework.h"

#include "FortInventory.h"

namespace BuildingActor {
	void (*OnDamageServerOG)(ABuildingActor* This, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);
	void OnDamageServer(ABuildingActor* This, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext)
	{
		if (!This || !InstigatedBy || !InstigatedBy->IsA(AFortPlayerControllerAthena::StaticClass()) || !DamageCauser->IsA(AFortWeapon::StaticClass()) || !((AFortWeapon*)DamageCauser)->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) || This->bPlayerPlaced) {
			return OnDamageServerOG(This, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
		}

		ABuildingSMActor* BuildingSMActor = (ABuildingSMActor*)This;
		AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)InstigatedBy;
		if (!PC->Pawn) {
			return OnDamageServerOG(This, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
		}
		AFortPlayerPawnAthena* Pawn = (AFortPlayerPawnAthena*)PC->Pawn;

		int MaterialCount = (Damage / (UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(6, 12)));

		PC->ClientReportDamagedResourceBuilding(BuildingSMActor, BuildingSMActor->ResourceType, MaterialCount, BuildingSMActor->bDestroyed, (Damage == 100.f));

		UFortResourceItemDefinition* ResourceItemDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingSMActor->ResourceType);
		if (!ResourceItemDefinition) {
			return OnDamageServerOG(This, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
		}

		AFortPickup* Pickup = SpawnPickup(ResourceItemDefinition, MaterialCount, 0, PC->K2_GetActorLocation(), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, PC->MyFortPawn);
		Pawn->ServerHandlePickup(Pickup, 0.3f, FVector(), true);

		return OnDamageServerOG(This, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
	}

	void HookAll() {
		MH_CreateHook((LPVOID)(ImageBase + 0x1CC36A0), OnDamageServer, (LPVOID*)&OnDamageServerOG);

		Log("BuildingActor Hooked!");
	}
}