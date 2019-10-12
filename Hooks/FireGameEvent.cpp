#include "../main.h"
#include "../Hacks/skinconfigchanger.hpp"

bool hkFireEventClientSide(void* thisptr, IGameEvent* event){
    
    FireEventClientSide(event);

    return game_event_vmt->GetOriginalMethod<tFireGameEvent>(11)(thisptr, event);
}
