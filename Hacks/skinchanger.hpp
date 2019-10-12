#include "../main.h"

class cSkin{
public:
    cSkin(){}
    cSkin(int paintkit, int seed, int itemDefinition, int stattrack, int EntityQuality, char* szName, float flWear) :
    Paintkit(paintkit), Seed(seed), ItemDefinition(itemDefinition), StatTrack(stattrack), EntityQuality(EntityQuality), name(szName), Wear(flWear){}
    
    int Paintkit = 0;
    int Seed = 0;
    int ItemDefinition = 0;
    int StatTrack = -1;
    int EntityQuality = 0;
    char* name = nullptr;
    float Wear = 0.0001f;
};

class cSkinchanger {
public:
    static unordered_map<int, cSkin> Skins;
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
    
    unordered_map<size_t, string> EntityQuality = {
        {0, "Normal"}, // Normal
        {1, "Genuine"}, // Genuine
        {2, "Vintage"}, // Vintage
        {3, "Unusual"}, // Star
        {5, "Community"}, // Commuinity
        {6, "Developer"}, // Valve
        {7, "Self-Made"}, // Protoype
        {8, "Customized"}, // Customized
        {9, "Strange"}, // Stattrak™
        {10, "Completed"}, // Completed
        {12, "Tournament"} // Souvenir
    };
    void FireEventClientSide(IGameEvent* event);
    void FindModels();
    void ForceSkins();
    void FrameStageNotify(ClientFrameStage_t stage);
    void Init();
    void ApplyCustomGloves();
    bool glovesUpdated = false;
    
    C_BaseEntity* pLocalPlayer = nullptr;
    
    bool bInit = false;
};

extern cSkinchanger* skinchanger;
