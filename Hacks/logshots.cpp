//
//  logshots.cpp
//  vHook
//
//  Created by Timothy Dillan on 20/8/19.
//  Copyright © 2019 ViKiNG. All rights reserved.
//

#include "logshots.hpp"
//bool Settings::LogShots::enabled = false;

std::deque<Shots> LogShots::shots;
std::deque<LoggedEvent> LogShots::eventList;

std::array<int, 64> LogShots::missedShots;

std::string HitgroupToString(int hitgroup)
{
    switch((HitGroups) hitgroup)
    {
        case HitGroups::HITGROUP_GENERIC:
            return ("Generic");
        case HitGroups::HITGROUP_CHEST:
            return ("Body");
        case HitGroups::HITGROUP_HEAD:
            return ("Head");
        case HitGroups::HITGROUP_LEFTARM:
            return ("Left Arm");
        case HitGroups::HITGROUP_LEFTLEG:
            return ("Left Leg");
        case HitGroups::HITGROUP_RIGHTARM:
            return ("Right Arm");
        case HitGroups::HITGROUP_RIGHTLEG:
            return ("Right Leg");
        case HitGroups::HITGROUP_STOMACH:
            return ("Stomach");
        case HitGroups::HITGROUP_GEAR:
            return ("Gear");
    }
}

void LogShots::Paint()
{
    int offset = 18;
    int i = 0;
    
    
    if (!pEngine->IsInGame())
        return;
    
    if (!vars.misc.logshots)
        return;
    

}

void LogShots::FireGameEvent(IGameEvent *event)
{
    if (!event)
        return;
    
    C_BasePlayer* localplayer = (C_BasePlayer*) pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if (!localplayer)
        return;
    
    if (!vars.misc.logshots)
        return;
    
    if (!strcmp(event->GetName(), ("player_hurt")))
    {
        int attacker = pEngine->GetPlayerForUserID(event->GetInt(("attacker")));
        int victim = pEngine->GetPlayerForUserID(event->GetInt(("userid")));
        std::string hitgroup = HitgroupToString(event->GetInt(("hitgroup")));
        std::string damage = event->GetString(("dmg_health"));
        std::string health = event->GetString(("health"));
        
        if (attacker != pEngine->GetLocalPlayer())
            return;
        
        if (victim == pEngine->GetLocalPlayer())
            return;
        
        auto ent = pEntList->GetClientEntity(pEngine->GetPlayerForUserID(victim));
        player_info_t info;
        pEngine->GetPlayerInfo(victim, &info);
        
        std::string str;
        str += std::string(("Hit ")) + info.name + (" in the ") + hitgroup + (" for ") + damage + (" dmg (") + health + (" health remaining.)\n");
        
       // cvar->ConsoleColorPrintf(ColorRGBA(39, 106, 219, 255), ("[SpaceHook] "));
        //cvar->ConsoleDPrintf(str.c_str());
        eventList.push_back(LoggedEvent(str, pGlobals->curtime + 5.f));
        
        missedShots[victim - 1] = 0;
        
        if (!shots.empty())
        {
            shots.front().hit = true;
            shots.front().processed = true;
            shots.front().networked = true;
        }
    }
    if (!strcmp(event->GetName(), ("bullet_impact")))
    {
        int userid = pEngine->GetPlayerForUserID(event->GetInt(("userid")));
        
        if (userid != pEngine->GetLocalPlayer())
            return;
        
        if (!shots.empty())
        {
            shots.front().impacts.push_back(Vector(event->GetFloat(("x")), event->GetFloat(("y")), event->GetFloat(("z"))));
            shots.front().networked = true;
        }
    }
}

void LogShots::CreateMove(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if (!localplayer)
        return;
    
    if (!vars.misc.logshots)
        return;
    
    if (shots.empty()) return;
    auto shot = shots.front();
    if (shot.processed || shot.hit || abs(shot.time - pGlobals->curtime) > 1.f)
    {
        shots.erase(shots.begin());
        return;
    }
    if (!shot.networked) return;
    
    Vector impact;
    float bestLength = 0.f;
    
    while (!shot.impacts.empty())
    {
        if ((shot.src - shot.impacts.front()).Length() > bestLength)
        {
            impact = shot.impacts.front();
            bestLength = (shot.src - shot.impacts.front()).Length();
        }
        shot.impacts.erase(shot.impacts.begin());
    }
    
    if (vars.aimbot.yresolve != 6)
    {
        return;
    }
    
    Ray_t ray;
    trace_t tr;
    CTraceFilterEntitiesOnly filter;
    
    ray.Init(shot.src, impact);
    filter.pSkip = localplayer;
    pEngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
    
    if (tr.m_pEnt == shot.ent) // resolver error
    {
        std::string str;
        switch ( (int) (pGlobals->curtime * 100) % 5 )
        {
            case 0:
                str = ("Missed shot due to bad config\n");
                break;
            case 1:
                str = ("Missed shot due to the cheat refusing to look at gay people\n");
                break;
            case 2:
                str = ("Missed shot due to Earth's curvature\n");
                break;
            case 3:
                str = ("Missed shot due to pasted resolver\n");
                break;
            case 4:
                str = ("Missed shot due to [insert excuses here]\n");
                break;
        }
        
        eventList.push_back({str, pGlobals->curtime + 5.f});
        //cvar->ConsoleColorPrintf(ColorRGBA(39, 106, 219, 255), ("[SpaceHook] "));
        //cvar->ConsoleDPrintf(str.c_str());
        
        missedShots[shot.ent->GetIndex() - 1]++;
    } else { // spread
        eventList.push_back({"Missed shot due to spread", pGlobals->curtime + 5.f});
        //cvar->ConsoleColorPrintf(ColorRGBA(39, 106, 219, 255), ("[SpaceHook] "));
        //cvar->ConsoleDPrintf(("Missed shot due to spread\n"));
    }
    
    shot.processed = true;
    shots.erase(shots.begin());
}
