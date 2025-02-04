//
//  antiaim.hpp
//  vHook
//
#pragma once

#include "main.h"

extern Vector atTargets;

enum VIEW_ANTIAIM_PITCH {
    Down        = 1,
    Up          = 2,
};

namespace AntiAim{
   float GetMaxxDelta( CCSGOAnimState *animState );
    
}


namespace fake{
    //
}

enum VIEW_ANTIAIM_YAW {
    Backwards       = 1,
    Jitter          = 2,
    FakeStatic      = 3,
    FJitter         = 4,
    SlowSpin        = 5,
    FastSpin        = 6,
    RandomBackJitter= 7,
    BackJitter      = 8,
    LowerYaw        = 9,
    SidewaysLeft    = 10,
    SidewaysRight   = 11,
    LBYBreaker      = 12,
    Desync1      = 13
};

enum VIEW_ANTIIAIM_FYAW {
    FakeSpin        = 1,
    FakeLBYHook     = 2,
    FakeTwoStep     = 3,
    FakeLowerBody135= 4,
    FakeInverseRotation = 5,
    Desync      = 6,
    FakeLBY         = 7,
    FakeSideLBY     = 8
    
};

enum VIEW_ANTIIAIM_MYAW
{
    LowerBody       = 1,
    Rotate          = 2
};

enum VIEW_ANTIIAIM_AYAW
{
    LowerBod        = 1,
    Rotatee         = 2
};

void antiResolverFlip(CUserCmd* cmd, C_BaseEntity* local);
void DoAntAimFake(CCSGOAnimState* animState, Vector &angle);
void turbojizzer(CUserCmd* cmd, C_BaseEntity* local);
void backjizzer(CUserCmd* cmd, C_BaseEntity* local);
void lby_spin(CUserCmd* cmd, C_BaseEntity* local);
void tank(CUserCmd* cmd, C_BaseEntity* local);
void resolverfucker(CUserCmd* cmd, C_BaseEntity* local);
void DoAntiaim(CUserCmd* cmd, C_BaseEntity* local, C_BaseCombatWeapon* weapon, bool& bPacket, CCSGOAnimState* animState);
void LegitAA(CUserCmd *pCmd, C_BaseEntity* local);
void doManual(CUserCmd* cmd, C_BaseEntity* local, C_BaseCombatWeapon* weapon);
//float last_fake;
//void GetBestHeadAngle(CUserCmd* cmd, C_BasePlayer* local);
//void DoOffsets(CUserCmd* cmd, C_BaseEntity* local, C_BaseCombatWeapon* weapon, bool& bPacket);
void AngleVectors3(const Vector &angles, Vector& forward);
void AngleVectors3(const Vector &angles, Vector& forward, Vector& right, Vector& up);
void DoAntiAimFake(CUserCmd* cmd, C_BaseEntity* local, C_BaseCombatWeapon* weapon);
//Vector GFakeAngle;
//Vector GRealAngle;

void DoLBYBreaker(C_BaseEntity* local, CUserCmd* cmd, CCSGOAnimState* animState, C_BaseCombatWeapon* weapon);








