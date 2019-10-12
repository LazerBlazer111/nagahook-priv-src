#include "skinchanger.hpp"

int KnifeCT = WEAPON_KNIFE_M9_BAYONET;
int KnifeT = WEAPON_KNIFE_KARAMBIT;
int GloveCT = GLOVE_MOTORCYCLE;
int GloveT = GLOVE_MOTORCYCLE;

unordered_map<int, cSkin> cSkinchanger::Skins = unordered_map<int, cSkin>( {
    // Gloves
    make_pair(GLOVE_CT_SIDE, cSkin(10049, -1, GloveCT, -1, 3, nullptr, 0.0001f)), //
    make_pair(GLOVE_T_SIDE, cSkin(10049, -1, GloveT, -1, 3, nullptr, 0.0001f)), //
    // Pistols

});

unordered_map<int, const char*> cSkinchanger::ModelList;

cSkinchanger* skinchanger = new cSkinchanger;

void cSkinchanger::FrameStageNotify(ClientFrameStage_t stage) {
    if(stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START){
        pLocalPlayer = (C_BaseEntity*)(pEntList->GetClientEntity(pEngine->GetLocalPlayer()));
        
        if(pLocalPlayer && pLocalPlayer->GetHealth() > 0){
            if(!bInit){
                Init();
                bInit = true;
            }
            ForceSkins();
        }
    }
}

void cSkinchanger::FindModels() {
    ModelList[pModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl")] = KnifeToModelMatrix[KnifeCT].c_str();
    ModelList[pModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl")] = KnifeToModelMatrix[KnifeT].c_str();
    ModelList[pModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl")] = GloveToModelMatrix[GloveCT].c_str();
    ModelList[pModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl")] = GloveToModelMatrix[GloveT].c_str();
}

void cSkinchanger::ForceSkins() {
    player_info_t player_info;
    
    if(pEngine->GetPlayerInfo(pLocalPlayer->GetId(), &player_info)){
        
        int* pWeapons = pLocalPlayer->GetWeapons();
        
        C_BaseViewModel* LocalPlayerViewModel = (C_BaseViewModel*)pEntList->GetClientEntityFromHandle(pLocalPlayer->GetViewModel());
        
        C_BaseAttributableItem* WeaponViewModel = nullptr;
        
        if(LocalPlayerViewModel)
            WeaponViewModel = (C_BaseAttributableItem*)pEntList->GetClientEntityFromHandle(LocalPlayerViewModel->GetWeapon());
        
        C_BaseCombatWeapon* localWeapon = (C_BaseCombatWeapon*)pEntList->GetClientEntityFromHandle(pLocalPlayer->GetActiveWeapon());
        
        if(pWeapons){
            for(int i = 0; pWeapons[i]; i++){
                C_BaseAttributableItem* attributableItem = (C_BaseAttributableItem*)pEntList->GetClientEntityFromHandle(pWeapons[i]);
                if(attributableItem) {
                    short* Definition = attributableItem->GetItemDefinitionIndex();
                    
                    unordered_map<int, cSkin>::iterator SkinIter = (*Definition == WEAPON_KNIFE ? (*Definition == WEAPON_KNIFE ? Skins.find(WEAPON_KNIFE) : Skins.find(WEAPON_KNIFE_T)) : Skins.find(*Definition));
                    
                    if(SkinIter != Skins.end()) {
                        if(*attributableItem->GetOriginalOwnerXuidLow() == player_info.xuidlow && *attributableItem->GetOriginalOwnerXuidHigh() == player_info.xuidhigh){
                            
                            int* model_index = attributableItem->GetModelIndex();
                            
                            unordered_map<int, const char*>::iterator model_iter = ModelList.find(*model_index);
                            
                            if(model_iter != ModelList.end()){
                                *model_index = pModelInfo->GetModelIndex(model_iter->second);
                            }
                            
                            cSkin skin = move(SkinIter->second);
                            
                            if(KnifeCT && (*Definition == WEAPON_KNIFE))
                                *attributableItem->GetItemDefinitionIndex() = KnifeCT;
                            else if(KnifeT && (*Definition == WEAPON_KNIFE_T))
                                *attributableItem->GetItemDefinitionIndex() = KnifeT;
                            
                            if(skin.name) {
                                sprintf(attributableItem->GetCustomName(), "%s", skin.name);
                            }
                            
                            *attributableItem->GetItemIDHigh() = -1;
                            *attributableItem->GetFallbackPaintKit() = skin.Paintkit;
                            *attributableItem->GetFallbackStatTrak() = skin.StatTrack;
                            *attributableItem->GetEntityQuality() = skin.EntityQuality;
                            *attributableItem->GetFallbackSeed() = skin.Seed;
                            *attributableItem->GetFallbackWear() = skin.Wear;
                            *attributableItem->GetAccountID() = player_info.xuidlow;
                            
                            ApplyCustomGloves();
                            Init();
                        }
                    }
                    
                    if (WeaponViewModel && WeaponViewModel == attributableItem) {
                        int* model_index = ((C_BaseEntity*)LocalPlayerViewModel)->GetModelIndex();
                        
                        unordered_map<int, const char*>::iterator model_iter = ModelList.find(*model_index);
                        
                        if (model_iter != ModelList.end()) {
                            *model_index = pModelInfo->GetModelIndex(model_iter->second);
                        }
                    }
                }
            }
            
            if(LocalPlayerViewModel && localWeapon) {
                
                int* model_index = ((C_BaseEntity*)LocalPlayerViewModel)->GetModelIndex();
                
                unordered_map<int, const char*>::iterator model_iter = ModelList.find(*((C_BaseEntity*)localWeapon)->GetModelIndex());
                
                if(model_iter != ModelList.end()){
                    *model_index = pModelInfo->GetModelIndex(model_iter->second);
                }
            }
        }
    }
}

void cSkinchanger::ApplyCustomGloves() {
    
    C_BaseEntity* pLocal = (C_BaseEntity*)pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    
    if (!pEntList->GetClientEntityFromHandle((void*)pLocal->GetWearables())) {
        for (ClientClass* pClass = pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
            if (pClass->m_ClassID != (int)EClassIds::CEconWearable)
                continue;
            
            int entry = (pEntList->GetHighestEntityIndex() + 1);
            int serial = RandomInt(0x0, 0xFFF);
            
            pClass->m_pCreateFn(entry, serial);
            pLocal->GetWearables()[0] = entry | serial << 16;
            
            glovesUpdated = true;
            
            break;
        }
    }
    
    player_info_t LocalPlayerInfo;
    pEngine->GetPlayerInfo(pEngine->GetLocalPlayer(), &LocalPlayerInfo);
    
    C_BaseAttributableItem* glove = (C_BaseAttributableItem*)pEntList->GetClientEntity(pLocal->GetWearables()[0] & 0xFFF);
    
    if (!glove)
        return;
    
    int* glove_index = glove->GetModelIndex();
    unordered_map<int, const char*>::iterator glove_iter = ModelList.find(*glove_index);
    unordered_map<int, cSkin>::iterator Iter = (pLocal->GetTeam() == TEAM_COUNTER_TERRORIST ? Skins.find(GLOVE_CT_SIDE) : Skins.find(GLOVE_T_SIDE));
    cSkin gloveskin = move(Iter->second);
    
    if(glove_iter != ModelList.end())
        *glove_index = pModelInfo->GetModelIndex(glove_iter->second);
    
    if(GloveCT && pLocal->GetTeam() == TEAM_COUNTER_TERRORIST)
        *glove->GetItemDefinitionIndex() = GloveCT;
    if(GloveT && pLocal->GetTeam() == TEAM_TERRORIST)
        *glove->GetItemDefinitionIndex() = GloveT;
    
    *glove->GetItemIDHigh() = -1;
    *glove->GetFallbackPaintKit() = gloveskin.Paintkit;
    *glove->GetFallbackWear() = gloveskin.Wear;
    *glove->GetAccountID() = LocalPlayerInfo.xuidlow;
    
    if(glovesUpdated) {
        glove->GetNetworkable()->PreDataUpdate(DATA_UPDATE_CREATED);
        glovesUpdated = false;
    }
}

void cSkinchanger::Init() {
    ModelList.clear();
    FindModels();
}

void cSkinchanger::FireEventClientSide(IGameEvent *event) {
   if (!vars.visuals.skinc)
        return;
    
    if (!pEngine->IsInGame())
        return;
    
    if (!event || strcmp(event->GetName(), "player_death") != 0)
        return;
    
    if (!event->GetInt("attacker") || pEngine->GetPlayerForUserID(event->GetInt("attacker")) != pEngine->GetLocalPlayer())
        return;
    
    std::string weapon = event->GetString("weapon");
}

// Fix Animations (I don't know if any of the new knives have a new animation so I can't update them)
inline const int RandomSequence(int low, int high) {
    return (rand() % (high - low + 1) + low);
}
