//
//  grenadeprediction.cpp
//  vHook
//
//  Created by Timothy Dillan on 19/8/19.
//  Copyright © 2019 ViKiNG. All rights reserved.
//

#include "grenadeprediction.hpp"

/*#include <sstream>

//bool Settings::GrenadePrediction::enabled = false;
//colorVar Settings::GrenadePrediction::color = ImColor( 255, 79, 56, 255 );

std::vector<Vector> grenadePath;
int grenadeType = 0;
float GrenadePrediction::cameraHeight = 100.0f;

static void TraceHull( Vector& src, Vector& end, trace_t& tr ) {
    if ( !vars.misc.grenadepred )
        return;
    
    C_BasePlayer* pLocal = ( C_BasePlayer* ) pEntList->GetClientEntity( pEngine->GetLocalPlayer() );
    if ( !pLocal || !pLocal->GetAlive() )
        return;
    
    Ray_t ray;
    ray.Init( src, end, Vector( -2.0f, -2.0f, -2.0f ), Vector( 2.0f, 2.0f, 2.0f ) );
    
    CTraceFilter filter;
    filter.pSkip = pLocal;
    
    pEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &tr );
}

static void Setup( Vector& vecSrc, Vector& vecThrow, Vector viewangles ) {
    if ( !vars.misc.grenadepred )
        return;
    
    C_BasePlayer* localplayer = ( C_BasePlayer* ) pEntList->GetClientEntity( pEngine->GetLocalPlayer() );
    if ( !localplayer || !localplayer->GetAlive() )
        return;
    
    C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) pEntList->GetClientEntityFromHandle(
                                                                                                     localplayer->GetActiveWeapon() );
    if ( !activeWeapon || (CSWeaponType)activeWeapon->GetCSWpnData()->m_WeaponType != CSWeaponType::WEAPONTYPE_GRENADE )
        return;
    
    C_BaseCSGrenade* grenade = ( C_BaseCSGrenade* ) activeWeapon;
    
    Vector angThrow = viewangles;
    float pitch = angThrow.x;
    
    if ( pitch <= 90.0f ) {
        if ( pitch < -90.0f ) {
            pitch += 360.0f;
        }
    } else {
        pitch -= 360.0f;
    }
    float a = pitch - ( 90.0f - fabs( pitch ) ) * 10.0f / 90.0f;
    angThrow.x = a;
    
    // Gets ThrowVelocity from weapon files
    // Clamped to [15,750]
    float flVel = 750.0f * 0.9f;
    
    float b = grenade->GetThrowStrength();
    
    // Clamped to [0,1]
    b = b * 0.7f;
    b = b + 0.3f;
    flVel *= b;
    
    Vector vForward, vRight, vUp;
    
    AngleVectors2( angThrow, &vForward, &vRight, &vUp );
    
    vecSrc = localplayer->GetEyePosition();
    float off = ( grenade->GetThrowStrength() * 12.0f ) - 12.0f;
    vecSrc.z += off;
    
    // Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
    trace_t tr;
    Vector vecDest = vecSrc;
    vecDest += vForward * 22.0f; //vecDest.MultAdd(vForward, 22.0f);
    
    TraceHull( vecSrc, vecDest, tr );
    
    // After the hull trace it moves 6 units back along vForward
    // vecSrc = tr.endpos - vForward * 6
    Vector vecBack = vForward;
    vecBack *= 6.0f;
    vecSrc = tr.endpos;
    vecSrc -= vecBack;
    
    // Finally calculate velocity
    vecThrow = localplayer->GetVelocity();
    vecThrow *= 1.25f;
    vecThrow += vForward * flVel; //    vecThrow.MultAdd(vForward, flVel);
}


static bool CheckDetonate( const Vector& vecThrow, const trace_t& tr, int tick, float interval ) {
    if ( grenadeType == 0 )
        return false;
    
    switch ( grenadeType ) {
        case 45: // WEAPON_SMOKEGRENADE = 45,
        case 47: // WEAPON_DECOY = 47,
            // Velocity must be <0.1, this is only checked every 0.2s
            if ( vecThrow.Length2D() < 0.1f ) {
                int det_tick_mod = static_cast<int>(0.2f / interval);
                return !( tick % det_tick_mod );
            }
            return false;
        case 46: // WEAPON_MOLOTOV = 46,
        case 48: // WEAPON_INCGRENADE = 48,
            // Detonate when hitting the floor
            if ( tr.fraction != 1.0f && tr.plane.normal.z > 0.7f )
                return true;
            // OR we've been flying for too long
        case 43: // WEAPON_FLASHBANG = 43,
        case 44: // WEAPON_HEGRENADE = 44,
            // Pure timer based, detonate at 1.5s, checked every 0.2s
            return static_cast<float>(tick) * interval > 1.5f && !( tick % static_cast<int>(0.2f / interval) );
        default:
            assert( false );
            return false;
    }
}

static void AddGravityMove( Vector& move, Vector& vel, float frametime, bool onground ) {
    if ( !vars.misc.grenadepred)
        return;
    
    Vector basevel( 0.0f, 0.0f, 0.0f );
    
    move.x = ( vel.x + basevel.x ) * frametime;
    move.y = ( vel.y + basevel.y ) * frametime;
    
    if ( onground ) {
        move.z = ( vel.z + basevel.z ) * frametime;
    } else {
        // Game calls GetActualGravity( this );
        float gravity = 800.0f * 0.4f;
        
        float newZ = vel.z - ( gravity * frametime );
        move.z = ( ( vel.z + newZ ) / 2.0f + basevel.z ) * frametime;
        
        vel.z = newZ;
    }
}

static void PushEntity( Vector& src, const Vector& move, trace_t& tr ) {
    if ( !vars.misc.grenadepred )
        return;
    
    Vector vecAbsEnd = src;
    vecAbsEnd += move;
    
    // Trace through world
    TraceHull( src, vecAbsEnd, tr );
}

static int PhysicsClipVelocity( const Vector& in, const Vector& normal, Vector& out, float overbounce ) {
    static const float STOP_EPSILON = 0.1f;
    
    float backoff;
    float change;
    float angle;
    int i, blocked;
    
    blocked = 0;
    
    angle = normal[2];
    
    if ( angle > 0 ) {
        blocked |= 1;        // floor
    }
    if ( !angle ) {
        blocked |= 2;        // step
    }
    
    backoff = in.Dot( normal ) * overbounce;
    
    for ( i = 0; i < 3; i++ ) {
        change = normal[i] * backoff;
        out[i] = in[i] - change;
        if ( out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON ) {
            out[i] = 0;
        }
    }
    
    return blocked;
}

static void ResolveFlyCollisionCustom( trace_t& tr, Vector& vecVelocity, float interval ) {
    if ( !vars.misc.grenadepred )
        return;
    
    // Calculate elasticity
    float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
    float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
    float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
    if ( flTotalElasticity > 0.9f ) flTotalElasticity = 0.9f;
    if ( flTotalElasticity < 0.0f ) flTotalElasticity = 0.0f;
    
    // Calculate bounce
    Vector vecAbsVelocity;
    PhysicsClipVelocity( vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f );
    vecAbsVelocity *= flTotalElasticity;
    
    // Stop completely once we move too slow
    float flSpeedSqr = vecAbsVelocity.LengthSqr();
    static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
    if ( flSpeedSqr < flMinSpeedSqr ) {
        //vecAbsVelocity.Zero();
        vecAbsVelocity.x = 0.0f;
        vecAbsVelocity.y = 0.0f;
        vecAbsVelocity.z = 0.0f;
    }
    
    // Stop if on ground
    if ( tr.plane.normal.z > 0.7f ) {
        vecVelocity = vecAbsVelocity;
        vecAbsVelocity *= ( ( 1.0f - tr.fraction ) * interval ); //vecAbsVelocity.Mult((1.0f - tr.fraction) * interval);
        PushEntity( tr.endpos, vecAbsVelocity, tr );
    } else {
        vecVelocity = vecAbsVelocity;
    }
}

static int Step( Vector& vecSrc, Vector& vecThrow, int tick, float interval ) {
    // Apply gravity
    Vector move;
    AddGravityMove( move, vecThrow, interval, false );
    
    // Push entity
    trace_t tr;
    PushEntity( vecSrc, move, tr );
    
    int result = 0;
    // Check ending conditions
    if ( CheckDetonate( vecThrow, tr, tick, interval ) ) {
        result |= 1;
    }
    
    // Resolve collisions
    if ( tr.fraction != 1.0f ) {
        result |= 2; // Collision!
        ResolveFlyCollisionCustom( tr, vecThrow, interval );
    }
    
    // Set new position
    vecSrc = tr.endpos;
    
    return result;
}

static void Simulate( CViewSetup* setup ) {
    if ( !vars.misc.grenadepred )
        return;
    
    Vector vecSrc, vecThrow;
    Setup( vecSrc, vecThrow, setup->angles );
    
    float interval = pGlobals->interval_per_tick;
    
    // Log positions 20 times per sec
    int logstep = static_cast<int>(0.05f / interval);
    int logtimer = 0;
    
    grenadePath.clear();
    for ( unsigned int i = 0; i < grenadePath.max_size() - 1; ++i ) {
        if ( !logtimer )
            grenadePath.push_back( vecSrc );
        
        int s = Step( vecSrc, vecThrow, i, interval );
        if ( ( s & 1 ) ) break;
        
        // Reset the log timer every logstep OR we bounced
        if ( ( s & 2 ) || logtimer >= logstep ) logtimer = 0;
        else ++logtimer;
    }
    grenadePath.push_back( vecSrc );
}

void GrenadePrediction::OverrideView( CViewSetup* pSetup ) {
    if ( !vars.misc.grenadepred )
        return;
    
    C_BasePlayer* pLocal = ( C_BasePlayer* ) pEntList->GetClientEntity( pEngine->GetLocalPlayer() );
    if ( !pLocal || !pLocal->GetAlive() )
        return;
    
    C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) pEntList->GetClientEntityFromHandle(
                                                                                                     pLocal->GetActiveWeapon() );
    if ( !activeWeapon || (CSWeaponType)activeWeapon->GetCSWpnData()->m_WeaponType != CSWeaponType::WEAPONTYPE_GRENADE )
        return;
    
    C_BaseCSGrenade* grenade = ( C_BaseCSGrenade* ) activeWeapon;
    
    if ( grenade->GetPinPulled() ) {
        ItemDefinitionIndex itemDefinitionIndex = *activeWeapon->GetItemDefinitionIndex();
        
        grenadeType = ( int ) itemDefinitionIndex;
        Simulate( pSetup );
    } else {
        grenadeType = 0;
    }
}

void GrenadePrediction::Paint( ) {
    if ( !vars.misc.grenadepred )
        return;
    
    C_BasePlayer* pLocal = ( C_BasePlayer* ) pEntList->GetClientEntity( pEngine->GetLocalPlayer() );
    if ( !pLocal || !pLocal->GetAlive() )
        return;
    
    C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) pEntList->GetClientEntityFromHandle(
                                                                                                     pLocal->GetActiveWeapon() );
    if ( !activeWeapon || (CSWeaponType)activeWeapon->GetCSWpnData()->m_WeaponType != CSWeaponType::WEAPONTYPE_GRENADE )
        return;
    
    C_BaseCSGrenade* grenade = ( C_BaseCSGrenade* ) activeWeapon;
    
    if ( ( grenadeType ) && ( grenadePath.size() > 1 ) && ( grenade->GetPinPulled() ) ) {
        Vector nadeStart, nadeEnd;
        
        Vector prev = grenadePath[0];
        
        for ( auto it = grenadePath.begin(), end = grenadePath.end(); it != end; ++it ) {
            if ( WorldToScreen( prev, &nadeStart ) && WorldToScreen( *it, &nadeEnd ) ) {
                draw->drawline( ( int ) nadeStart.x, ( int ) nadeStart.y,
                              ( int ) nadeEnd.x, ( int ) nadeEnd.y,
                               Color::Red() );
            }
            prev = *it;
        }
        
        if ( ESP::WorldToScreen( prev, &nadeEnd ) ) {
            draw->drawline( ( int ) nadeStart.x, ( int ) nadeStart.y,
                          ( int ) nadeEnd.x, ( int ) nadeEnd.y,
                           Color::Red() );
        }
    }
}

void GrenadePrediction::RenderView( void* thisptr, CViewSetup& setup, CViewSetup& hudViewSetup, unsigned int nClearFlags,
                                   int whatToDraw ) {
    static ITexture* nadeTexture = NULL;
    static std::string textureName = ( "GrenadeView" );
    static IMaterial* nadeViewMat = NULL;
    static long lastCalled = 0;
    
    if ( !vars.misc.grenadepred )
        return;
    
    if ( GetEpochTime() - lastCalled > 2000 ) { // Hack for it breaking on map change. might be false triggered by getting 0.5FPS.
        GetLocalClient( -1 )->m_nDeltaTick = -1;
        nadeTexture = NULL;
        nadeViewMat = NULL;
    }
    lastCalled = GetEpochTime();
    
    C_BasePlayer* localPlayer = ( C_BasePlayer* ) pEntList->GetClientEntity( pEngine->GetLocalPlayer() );
    if ( !localPlayer || !localPlayer->GetAlive() )
        return;
    
    C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) pEntList->GetClientEntityFromHandle(
                                                                                                     localPlayer->GetActiveWeapon() );
    
    if ( !activeWeapon || (CSWeaponType)activeWeapon->GetCSWpnData()->m_WeaponType != CSWeaponType::WEAPONTYPE_GRENADE )
        return;
    
    C_BaseCSGrenade* grenade = ( C_BaseCSGrenade* ) activeWeapon;
    
    if ( !grenade->GetPinPulled() ){
        cameraHeight = 100.0f;
        return;
    }
    
    if ( !grenadeType || grenadePath.empty() )
        return;
    
    /* Setup our Camera settings for Rendering 
    CViewSetup nadeView = setup;
    nadeView.origin = grenadePath[grenadePath.size() - 1];
    nadeView.origin.z += cameraHeight;
    nadeView.x = nadeView.oldX = 0;
    nadeView.y = nadeView.oldY = 0;
    nadeView.width = nadeView.unscaledWidth = 320;
    nadeView.height = nadeView.unscaledHeight = 240;
    nadeView.angles = Vector( 89.0f, setup.angles.y, setup.angles.z );
    nadeView.fov = 100.0f;
    nadeView.m_flAspectRatio = float( nadeView.width ) / float( nadeView.height );
    
    if ( !nadeTexture ) {
        material->forceBeginRenderTargetAllocation();
        nadeTexture = material->createFullFrameRenderTarget( textureName.c_str(), nadeView.width, nadeView.height );
        //material->forceEndRenderTargetAllocation(); // Causes texture handles to go HAM. scrambles textures.
        if ( !nadeTexture ) {
            pCvar->ConsoleDPrintf(( "Could not Create Rear View Texture! (%s)\n" ), textureName.c_str() );
            return;
        }
    }
    
    IMatRenderContext* renderCtx = material->GetRenderContext();
    if ( !renderCtx ) {
        pCvar->ConsoleDPrintf(( "Could not acquire render context\n" ) );
        return;
    }
    
    renderCtx->PushRenderTargetAndViewport();
    renderCtx->SetRenderTarget( nadeTexture );
    /* Render to our texture 
    viewRenderVMT->GetOriginalMethod<RenderViewFn>( 6 )( thisptr, nadeView, hudViewSetup,
                                                        VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_FULL_TARGET |
                                                        VIEW_CLEAR_OBEY_STENCIL | VIEW_CLEAR_STENCIL,
                                                        0 );
    if ( !nadeViewMat ) {
        std::stringstream materialData;
        materialData << "\"" << "UnlitGeneric" << "\"\n" <<
        "{\n" <<
        "\t\"$basetexture\" \"" << textureName.c_str() << "\"\n"
        "}\n" << std::flush;
        
        std::string materialName = ( "KisakDoesNotSuck" );
        KeyValues* keyValues = new KeyValues( ( "UnlitGeneric" ) );
        
        InitKeyValues( keyValues, ( "UnlitGeneric" ) );
        LoadFromBuffer( keyValues, materialName.c_str(), materialData.str().c_str(), nullptr, NULL, nullptr );
        
        nadeViewMat = material->CreateMaterial( materialName.c_str(), keyValues );
        if ( !nadeViewMat ) {
            cvar->ConsoleDPrintf( XORSTR( "Could not Create Grenade View Material! (%s)\n" ), materialName.c_str() );
            renderCtx->Release();
            return;
        }
        
        nadeViewMat->AlphaModulate( Settings::GrenadePrediction::color.Color().Value.w );
    }
    renderCtx->PopRenderTargetAndViewport();
    
    /* Make a Request to draw this later ( mainly for layering purposes )
    RenderView::RenderRequest request;
    request.destX = 100;
    request.destY = 275;
    request.width = nadeView.width;
    request.height = nadeView.height;
    request.mat = nadeViewMat;
    request.tex = nadeTexture;
    /* Push this RenderRequest to the Queue, it will be dealt with later.
    RenderView::renderQueue.push( request );
    renderCtx->Release();
}

/* Don't jump with Scroll-Wheel if grenade predicting
void GrenadePrediction::CreateMove( CUserCmd *cmd ) {
    if( !Settings::GrenadePrediction::enabled )
        return;
    
    C_BasePlayer* localPlayer = ( C_BasePlayer* ) entityList->GetClientEntity( engine->GetLocalPlayer() );
    if ( !localPlayer || !localPlayer->GetAlive() )
        return;
    
    C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) entityList->GetClientEntityFromHandle( localPlayer->GetActiveWeapon() );
    if ( !activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_GRENADE )
        return;
    
    C_BaseCSGrenade* grenade = ( C_BaseCSGrenade* ) activeWeapon;
    if ( grenade->GetPinPulled() && !inputSystem->IsButtonDown(KEY_SPACE) ) // HACK
        cmd->buttons &= ~IN_JUMP;
}
*/
