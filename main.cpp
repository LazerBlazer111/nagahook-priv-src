#include "main.h"


namespace Global
{
    CUserCmd*     cmd;
    C_BasePlayer* local;
    C_BaseCombatWeapon* localWeapon;
    float fake_angle;
    float real_angle;
    Vector fake_ang1e;
    Vector real_ang1e;
     //Vector GFakeAngle;
     //Vector GRealAngle;
}

namespace AntiAem
{
    Vector GFakeAngle;
    Vector GRealAngle;
    Vector GRealAngle1;
    Vector realangle;
    Vector fakeangle;
}
int __attribute__((constructor)) Init()
{

    InitializeInterfaces();
    
    InitializeVMTs();
    
    InitializeHooks();
    
    NetVarManager::DumpNetvars();
    
    Offsets::getOffsets();
    
    UpdateResolver();
    
    PrintInfo();
    
    return 0;
}

void __attribute__((constructor)) DeInit() {
    Unhook();
    return 0;
}
