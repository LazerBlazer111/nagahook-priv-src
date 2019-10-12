//
//  skinconfigchanger.cpp
//  vHook
//
//  Created by Timothy Dillan on 12/10/19.
//  Copyright © 2019 ViKiNG. All rights reserved.
//

#include "../Variables/Config.h"
#include "../Variables/skins.h"



int GloveCT = skin.gloveCT;
int GloveT = skin.gloveT;
unordered_map<int, SkinClass>Skins = unordered_map<int, SkinClass>( {

    make_pair(GLOVE_CT_SIDE, SkinClass(skin.CTgloveID, -1, GloveCT, -1, nullptr, 0.0001f)), //
    make_pair(GLOVE_T_SIDE, SkinClass(skin.TgloveID, -1, GloveT, -1, nullptr, 0.0001f)), //
});
static unordered_map<int, const char*> ModelList;


unordered_map<int, string> GloveToModelMatrix = unordered_map<int, string>({
    {GLOVE_STUDDED_BLOODHOUND, "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"},
    {GLOVE_HYDRA, "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"},
    {GLOVE_SPORTY, "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"},
    {GLOVE_SLICK, "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"},
    {GLOVE_LEATHER_WRAP, "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"},
    {GLOVE_MOTORCYCLE, "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"},
    {GLOVE_SPECIALIST, "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"}
});

void FindModels() {
    ModelList[pModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl")] = GloveToModelMatrix[GloveCT].c_str();
    ModelList[pModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl")] = GloveToModelMatrix[GloveT].c_str();
}


bool glovesUpdated = false;

void ApplyCustomGloves() {
    
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
    unordered_map<int, SkinClass>::iterator Iter = (pLocal->GetTeam() == TEAM_COUNTER_TERRORIST ? Skins.find(GLOVE_CT_SIDE) : Skins.find(GLOVE_T_SIDE));
    SkinClass gloveskin = move(Iter->second);
    
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

void InitNigga() {
    ModelList.clear();
    FindModels();
}

void applySkins() {
    
    int localplayer_index = pEngine->GetLocalPlayer();
    C_BaseEntity* local = (C_BaseEntity*)pEntList->GetClientEntity(localplayer_index);
    
    if (!local || local->GetHealth() < 1)
        return;
    
    
    player_info_t localplayer_info;
    pEngine->GetPlayerInfo(localplayer_index, &localplayer_info);
    
    int* pWeapons = local->GetWeapons();
    
    if(pWeapons) {
        for(int i = 0; pWeapons[i]; i++){
            C_BaseAttributableItem* weapon = (C_BaseAttributableItem*)pEntList->GetClientEntityFromHandle(pWeapons[i]);
            
            if (!weapon)
                continue;
            
            short* item_definition_index = weapon->GetItemDefinitionIndex();
            
            if (*item_definition_index == 0)
                continue;
            
            const char* weaponConfigName = ItemDefIndex.at(*item_definition_index).entity_name;
            
            if (config->HasWeaponConfiguration(weaponConfigName)) {
                
                const SkinClass& weapon_config = config->GetWeaponConfiguration(weaponConfigName);
                
                if (weapon_config.ItemDefinition != -1) {
                    
                    if (ItemDefIndex.find(weapon_config.ItemDefinition) != ItemDefIndex.end()) {
                        
                        *reinterpret_cast<int*>(weapon->GetModelIndex()) = pModelInfo->GetModelIndex(ItemDefIndex.at(weapon_config.ItemDefinition).model);
                        
                        if (ItemDefIndex.find(*item_definition_index) != ItemDefIndex.end()) {
                            
                            //                 const Item_t& original_item = ItemDefIndex.at(*item_definition_index);
                            //             const Item_t& replacement_item = ItemDefIndex.at(weapon_config.ItemDefinition);
                            *item_definition_index = weapon_config.ItemDefinition;
                        }
                    }
                }
                
                
                
                *weapon->GetEntityQuality() = 4;
                *weapon->GetFallbackWear() = 0.000000000001f;
                *weapon->GetFallbackStatTrak() = -1;
                *weapon->GetItemIDHigh() = -1;
                *weapon->GetAccountID() = localplayer_info.xuidlow;
                
                
                
                
                if (weapon_config.Paintkit != -1){
                    *weapon->GetFallbackPaintKit() = weapon_config.Paintkit;
                }
                
                if (weapon_config.Seed != -1){
                    *weapon->GetFallbackSeed() = weapon_config.Seed;
                }
                
                ApplyCustomGloves();
                InitNigga();
                
            }
        }
        
        C_BaseViewModel* viewmodel = (C_BaseViewModel*)pEntList->GetClientEntityFromHandle(local->GetViewModel());
        
        if (!viewmodel)
            return;
        
        CBaseHandle viewmodel_weapon_handle = viewmodel->GetWeapon();
        
        if (viewmodel_weapon_handle == INVALID_EHANDLE_INDEX)
            return;
        
        C_BaseAttributableItem* viewmodel_weapon = (C_BaseAttributableItem*)pEntList->GetClientEntityFromHandle(viewmodel_weapon_handle);
        
        if (!viewmodel_weapon)
            return;
        
        
        if (ItemDefIndex.find(*viewmodel_weapon->GetItemDefinitionIndex()) != ItemDefIndex.end()) {
            
            const Item_t& override_weapon = ItemDefIndex.at(*viewmodel_weapon->GetItemDefinitionIndex());
            *viewmodel->GetModelIndex() = pModelInfo->GetModelIndex(override_weapon.model);
            
        }
    }
    return;
}

void MakeSkinChanger(ClientFrameStage_t stage) {
    
    if(stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START){
        
        C_BaseEntity* local = (C_BaseEntity*)pEntList->GetClientEntity(pEngine->GetLocalPlayer());
        
        if(local && local->GetHealth() > 0)
            applySkins();
        
    }
}

void FireEventClientSide(IGameEvent *event) {
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

void HSequenceProxyFn(const CRecvProxyData *pDataConst, void *pStruct, void *pOut) {
    CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);
    C_BaseViewModel* pViewModel = (C_BaseViewModel*)pStruct;
    
    if(!pViewModel)
        return g_pSequence(pDataConst, pStruct, pOut);
    
    C_BaseEntity* pOwner = (C_BaseEntity*)pEntList->GetClientEntityFromHandle(pViewModel->GetOwner());
    
    if (pViewModel && pOwner) {
        if (pOwner->GetIndex() == pEngine->GetLocalPlayer()) {
            
            const model_t* knife_model = pModelInfo->GetModel(*pViewModel->GetModelIndex());
            const char* model_filename = pModelInfo->GetModelName(knife_model);
            
            int m_nSequence = (int)pData->m_Value.m_Int;
            
            if (!strcmp(model_filename, "models/weapons/v_knife_butterfly.mdl")) {
                switch (m_nSequence) {
                    case SEQUENCE_DEFAULT_DRAW:
                        m_nSequence = RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
                        break;
                    case SEQUENCE_DEFAULT_LOOKAT01:
                        m_nSequence = RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
                        break;
                    default:
                        m_nSequence++;
                }
            } else if (!strcmp(model_filename, "models/weapons/v_knife_falchion_advanced.mdl")) {
                switch (m_nSequence) {
                    case SEQUENCE_DEFAULT_IDLE2:
                        m_nSequence = SEQUENCE_FALCHION_IDLE1;
                        break;
                    case SEQUENCE_DEFAULT_HEAVY_MISS1:
                        m_nSequence = RandomSequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
                        break;
                    case SEQUENCE_DEFAULT_LOOKAT01:
                        m_nSequence = RandomSequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
                        break;
                    case SEQUENCE_DEFAULT_DRAW:
                    case SEQUENCE_DEFAULT_IDLE1:
                        break;
                    default:
                        m_nSequence--;
                }
            } else if (!strcmp(model_filename, "models/weapons/v_knife_push.mdl")) {
                switch (m_nSequence) {
                    case SEQUENCE_DEFAULT_IDLE2:
                        m_nSequence = SEQUENCE_DAGGERS_IDLE1;
                        break;
                    case SEQUENCE_DEFAULT_LIGHT_MISS1:
                    case SEQUENCE_DEFAULT_LIGHT_MISS2:
                        m_nSequence = RandomSequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
                        break;
                    case SEQUENCE_DEFAULT_HEAVY_MISS1:
                        m_nSequence = RandomSequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
                        break;
                    case SEQUENCE_DEFAULT_HEAVY_HIT1:
                    case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
                    case SEQUENCE_DEFAULT_LOOKAT01:
                        m_nSequence += 3;
                        break;
                    case SEQUENCE_DEFAULT_DRAW:
                    case SEQUENCE_DEFAULT_IDLE1:
                        break;
                    default:
                        m_nSequence += 2;
                }
            } else if (!strcmp(model_filename, "models/weapons/v_knife_survival_bowie.mdl")) {
                
                switch (m_nSequence) {
                    case SEQUENCE_DEFAULT_DRAW:
                    case SEQUENCE_DEFAULT_IDLE1:
                        break;
                    case SEQUENCE_DEFAULT_IDLE2:
                        m_nSequence = SEQUENCE_BOWIE_IDLE1;
                        break;
                    default:
                        m_nSequence--;
                }
            } else if (!strcmp(model_filename, "models/weapons/v_knife_ursus.mdl")) {
                switch (m_nSequence) {
                    case SEQUENCE_DEFAULT_DRAW:
                        m_nSequence = RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
                        break;
                    case SEQUENCE_DEFAULT_LOOKAT01:
                        m_nSequence = RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
                        break;
                    default:
                        m_nSequence++;
                }
            } else if (!strcmp(model_filename, "models/weapons/v_knife_stiletto.mdl")) {
                switch (m_nSequence){
                    case SEQUENCE_DEFAULT_LOOKAT01:
                        m_nSequence = RandomSequence(12, 13);
                        break;
                }
            } else if(!strcmp(model_filename, "models/weapons/v_knife_widowmaker.mdl")) {
                switch (m_nSequence) {
                    case SEQUENCE_DEFAULT_LOOKAT01:
                        m_nSequence = RandomSequence(SEQUENCE_TALON_LOOKAT1, SEQUENCE_TALON_LOOKAT2);
                        break;
                }
            }
            pData->m_Value.m_Int = m_nSequence;
        }
    }
    return g_pSequence(pData, pStruct, pOut);
}
