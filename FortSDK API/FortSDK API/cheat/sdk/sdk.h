#pragma once
#include "../../util/util.h"
#include "../esp/esp.h"
#include "offsets.hpp"
#include "sdk.h"
#include <math.h>

//Cracky

namespace cache {
	inline uintptr_t LocalPawn;
	inline uintptr_t Mesh;
	inline uintptr_t PlayerState ;
	inline uintptr_t RootComponent;
	inline uintptr_t LocalPlayers;
	inline uintptr_t PlayerController;
	inline uintptr_t Gworld;
	inline uintptr_t GameInstance;
	inline uintptr_t GameState;
	inline uintptr_t PlayerArray;
	inline int PlayerArraySize;
}
inline Camera vCamera;

FTransform GetBoneIndex(uint64_t mesh, int index) {
	uint64_t bonearray = read<uint64_t>(mesh + Offsets::BoneArray);
	if (!bonearray) bonearray = read<uint64_t>(mesh + Offsets::BoneArray + 0x10);
	return read<FTransform>(bonearray + (index * 0x60));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id) {
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(mesh + Offsets::ComponentToWorld);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

struct CamewaDescwipsion {
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
	char Useless[0x18];
};

Camera GetViewAngles() {
	CamewaDescwipsion camera;
	auto locationPointer = read<uintptr_t>(cache::Gworld + Offsets::CameraLocation);
	auto rotationPointer = read<uintptr_t>(cache::Gworld + Offsets::CameraRotation);

	struct Rotation {
		double a;
		char pad_0008[24];
		double b;
		char pad_0028[424];
		double c;
	};
	Rotation rotation;
	rotation = read<Rotation>(rotationPointer);

	camera.Location = read<Vector3>(locationPointer);
	camera.Rotation.x = asin(rotation.c) * (180.0 / M_PI);
	camera.Rotation.y = ((atan2(rotation.a * -1, rotation.b) * (180.0 / M_PI)) * -1) * -1;
	camera.FieldOfView = read<float>(cache::PlayerController + 0x3AC) * 90.f;

	return { camera.Location, camera.Rotation, camera.FieldOfView };
}

Vector2 ProjectWorldToScreen(Vector3 WorldLocation) {
	vCamera = GetViewAngles();

	D3DMATRIX tempMatrix = Matrix(vCamera.Rotation, Vector3(0,0,0));

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector2((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z);
}

class utility_class
{
public:
	bool IsEnemyVisible(uintptr_t Mesh_MJ)
	{
		auto Seconds = read<double>(cache::Gworld + 0x188);
		auto LastRenderTime = read<float>(Mesh_MJ + 0x32C);
		return Seconds - LastRenderTime <= 0.06f;
	}

	bool IsEnemyInScreen(Vector2 screen_location)
	{
		if (screen_location.x > 0 && screen_location.x < Width && screen_location.y > 0 && screen_location.y < Height) return true;
		else return false;
	}
}; inline utility_class utility;

class player_info {
public:
	std::string getPlayerName(uintptr_t playerstate)
	{
		__int64 FString = read<__int64>(playerstate + Offsets::PlayerName);
		int Length = read<int>(FString + 16);
		uintptr_t FText = read<__int64>(FString + 8);

		__int64 v6 = Length;
		if (!v6) return std::string("BOT");

		wchar_t* NameBuffer = new wchar_t[Length];
		requests::RPhysical((PVOID)FText, NameBuffer, Length * sizeof(wchar_t));

		char v21;
		int v22;
		int i;
		int v25;
		WORD* v23;

		v21 = v6 - 1;
		if (!(DWORD)v6)
			v21 = 0;
		v22 = 0;
		v23 = (WORD*)NameBuffer;
		for (i = (v21) & 3; ; *v23++ += i & 7)
		{
			v25 = v6 - 1;
			if (!(DWORD)v6)
				v25 = 0;
			if (v22 >= v25)
				break;
			i += 3;
			++v22;
		}

		std::wstring wbuffer{ NameBuffer };

		return std::string(wbuffer.begin(), wbuffer.end());
	}

	std::string getWeaponName(uintptr_t pawn_private)
	{
		std::string weapon_name;

		uint64_t current_weapon = read<uint64_t>(pawn_private + Offsets::CurWeapon);
		uint64_t weapon_data = read<uint64_t>(current_weapon + Offsets::WeaponData);
		uint64_t item_name = read<uint64_t>(weapon_data + 0x40);

		uint64_t FData = read<uint64_t>(item_name + 0x20);
		int FLength = read<int>(item_name + 0x28);

		if (FLength > 0 && FLength < 50)
		{
			wchar_t* WeaponBuffer = new wchar_t[FLength];
			requests::RPhysical((void*)FData, (PVOID)WeaponBuffer, FLength * sizeof(wchar_t));
			std::wstring wstr_buf(WeaponBuffer);
			weapon_name.append(std::string(wstr_buf.begin(), wstr_buf.end()));

			delete[] WeaponBuffer;
		}

		return weapon_name;
	}

}; inline player_info PINFO;