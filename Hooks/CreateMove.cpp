#include "main.h"

#include "../Hacks/aimbot.h"
#include "../Hacks/antiaim.h"
#include "../Hacks/autoshoot.h"
#include "../Hacks/bhop.h"
#include "../Hacks/clantag.h"
#include "../Hacks/spammer.h"
#include "../Hacks/fakewalk.h"
#include "../Backtrack.hpp"
#include "../moonwalk.hpp"
#include "../Hacks/triggerbot.hpp"
#include "../noduckcooldown.hpp"
#include "../Hacks/EnginePrediction.h"
#include "../Hacks/fakelag.hpp"
#include "../Hacks/legit.hpp"
#include "../Hacks/logshots.hpp"
#include "../Hacks/esp.h"
#include "../Hacks/grenadeprediction.hpp"
#include "../Hacks/autoblock.hpp"
#include "../Hacks/autoknife.hpp"
#include "../Hacks/airstuck.hpp"

Vector tpangles;




string GetLocalName()
{
    player_info_t localInfo;
    pEngine->GetPlayerInfo(pEngine->GetLocalPlayer(), &localInfo);
    return localInfo.name;
}

void RecoilControl(C_BaseEntity* local, CUserCmd* cmd)
{
    if(!vars.misc.norecoil && !vars.aimbot.rcs)
        return;
    
    Vector punch = local->GetPunchAngles() * 2.f;
    
    if(vars.misc.norecoil)
    {
        if(cmd->buttons & IN_ATTACK)
        {
            
            cmd->viewangles.x -= punch.x;
            cmd->viewangles.y -= punch.y;
        }
        return;
    }
    
    if(vars.aimbot.rcs)
    {
        if(cmd->buttons & IN_ATTACK)
        {
            cmd->viewangles.x -= punch.x * (2.f / 100.f * (vars.aimbot.rcsf / 2));
            cmd->viewangles.y -= punch.y * (2.f / 100.f * (vars.aimbot.rcsf / 2));
        }
        return;
    }
    
}


void hacks(CUserCmd* cmd, C_BaseEntity* local, C_BaseCombatWeapon* weapon, Vector& vOldAngles, float& flForwardmove, float& flSidemove,  bool& sendPacket, CCSGOAnimState* animState, C_BasePlayer* player)
{
    
    DoAutoStrafe(cmd, local);
    DoBhop(cmd, local);
    
    antiResolverFlip(cmd, local);
    turbojizzer(cmd, local);
    backjizzer(cmd, local);
    lby_spin(cmd, local);
    tank(cmd, local);
    resolverfucker(cmd, local);
    DoAntiAimFake(cmd, local, weapon);
    DoLBYBreaker(local, cmd, animState, weapon);
    DoAntiaim(cmd, local, weapon, sendPacket, animState);
    doManual(cmd, local, weapon);
    DuckCool(cmd);
    StartPrediction(cmd);
    Airstuck::CreateMove(cmd);
    DoAim(cmd, local, weapon, flForwardmove, flSidemove, player);
    DoTrigger(cmd, weapon);
    backtracking->legitBackTrack(cmd, local);
    RecoilControl(local, cmd);
    grenade_prediction::get().Tick(cmd->buttons);
    ContinuousPistols(cmd, weapon);
    AutoCock(cmd, weapon);
    DoLegitAim(cmd, local, weapon, flForwardmove, flSidemove);
    Hitchance(local, weapon);
    LogShots::CreateMove(cmd);
    EndPrediction();
    CirlceStrafe(local, cmd, vOldAngles);
    Moonwalk(cmd);
    Autoblock::CreateMove(cmd);
    AutoKnife::CreateMove(cmd);
    DoSpammer();
    
    if(draw->m_szChangedValue[3].length() > 0 && vars.misc.clantag) // Clantag Changer
        SetClanTag(draw->m_szChangedValue[3].c_str(), "Xanax");
    //movement->FakeLag(cmd, sendPacket);
    
}
    
bool bOnce = false;
bool SendPacket = true;

bool CreateMove::sendPacket = true;
Vector CreateMove::lastTickViewAngles = Vector(0, 0, 0);

bool hkCreateMove(void* thisptr, float flSampleInput, CUserCmd* cmd)
{
    
    misc->clan_tag();
    
    createmoveVMT->GetOriginalMethod<tCreateMove>(25)(thisptr, flSampleInput, cmd);
    
    
    if(!cmd->command_number)
        return false;
    
    if(!bSendPacket)
        return false;
    
    auto* local = pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    
    if(!local)
        return false;
    
    if(local->GetLifeState() != 0)
        return false;
    
    if(local->GetHealth() < 1)
        return false;
    
    if(local->GetDormant())
        return false;
    
    if(!bOnce)
    {
        if(draw->m_szCurrentString[2].empty())
        {
            draw->m_szCurrentString[2] = GetLocalName();
        }
        bOnce = true;
    }
    
    C_BaseCombatWeapon* weapon = GetActiveWeapon(local);
    C_BasePlayer* player = (C_BasePlayer*) pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    
    if(!weapon)
        return false;
    
    SendPacket = true;
    *bSendPacket = SendPacket;
    
    Vector vOldAngles = cmd->viewangles;
    Vector viewforward, viewright, viewup, aimforward, aimright, aimup;
    Vector qAimAngles;
    CCSGOAnimState* animState = local->GetAnimState();
    
    
    float forward = cmd->forwardmove;
    float sidemove = cmd->sidemove;
    float upmove = cmd->upmove;
    if (vars.visuals.inverseragdoll)
    {
        if (!weapon->IsKnife())//!weapon->isKnife
        {
            pEngine->ExecuteClientCmd("cl_righthand 1");
        
        }else{
        pEngine->ExecuteClientCmd("cl_righthand 0");
        }
    }
    
    if(pEngine->IsInGame() && pEngine->IsConnected())
    {
        hacks(cmd, local, weapon, vOldAngles, forward, sidemove, *bSendPacket, animState, player);
        
        if (vars.visuals.antiafkkick && cmd->command_number % 2){
            cmd->buttons |= 1 << 26;
        }
        
        
        FixMovement(vOldAngles, cmd);
        ClampMoves(forward, sidemove, upmove);
        
        if(local->GetAlive()){
            *bSendPacket = true;
        }
        
        if(*bSendPacket)
            tpangles = cmd->viewangles;
        
       
        
            cmd->viewangles.ClampAngles();
    
    }
    
    
    
    if(cmd && cmd->command_number)
    {
        matrix3x4_t fakelagmatrix[128];
        uintptr_t rbp;
        asm volatile("mov %%rbp, %0" : "=r" (rbp));
        bool *sendPacket = ((*(bool **)rbp) - 0x18);
        CreateMove::sendPacket =  true;
        
        FakeLag(cmd);
        Fakewalk(cmd, local);
        
        
        *sendPacket = CreateMove::sendPacket;

        
        if(CreateMove::sendPacket){
            CreateMove::lastTickViewAngles = cmd->viewangles;
        }
    }
    
    
    
    
    return false;
    
}
