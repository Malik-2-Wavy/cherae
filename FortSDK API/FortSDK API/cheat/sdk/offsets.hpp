#pragma once
#include <cstdint>

//Cracky

namespace Offsets {
	uint64_t
		UWorld = 0x17AA2698,
		GameState = 0x1C0,
		AcknowledgedPawn = 0x350,

		PlayerArray = 0x2C0,
		OwningGameInstance = 0x238,
		LocalPlayers = 0x38,
		PlayerController = 0x30,
		PlayerState = 0x2B0,
		RootComponent = 0x1B0,
		PawnPrivate = 0x320,
		Mesh = 0x328,

		BoneArray = 0x5E8,
		ComponentToWorld = 0x1E0,

		PlayerName = 0xB08,
		CurWeapon = 0xAA8,
		WeaponData = 0x568,
	    

	    CameraLocation = 0x168,
		CameraRotation = 0x178;
};