//
// GPUParticles system from AGK Header
//

#pragma once

#ifndef NOTFORMAINENGINE
#include "../../../WickedEngineDX12/WickedEngine/wiGraphicsDevice.h"
#include "../../../WickedEngineDX12/WickedEngine/wiScene.h"
#endif

namespace GPUParticles
{

// Public functions

int gpup_init();
int gpup_loadEffect( const char* fl, float x, float y, float z, float s ); // Load an Effect and set Global Position and Scale
int gpup_loadEffectLow( const char* fl, float x, float y, float z, float s, float ps ); // This quarters the particle count of the effect. 
int gpup_loadEffectHigh( const char* fl, float x, float y, float z, float s, float ps ); // This quadruples the particle count of the effect. 
void gpup_emitterActive( int ID, int active ); // Activate or deactivate the emitter of a specific effect. 
void gpup_setGlobalPosition( int ID, float x, float y, float z ); // Can be used to move an effect including all emitted particles and the turbulence
void gpup_setGlobalPivot( int ID, float x, float y, float z ); // Set a new global position for the effect without moving already existing particles
void gpup_setGlobalScale( int ID, float s ); // Change the global scale of an effect
void gpup_setParticleScale( int ID, float s ); // This will only scale each particle sprite and not have an effect on the overall size and flow of the effect.
void gpup_setGlobalRotation( int ID, float x, float y, float z ); // does not work as far as I can see!
void gpup_getEmitterSpeedAngleAdjustment(int ID, float* x, float* y, float* z);
void gpup_setEmitterSpeedAngleAdjustment(int ID, float x, float y, float z);
void gpup_setLocalPosition( int ID, float x, float y, float z );
void gpup_resetLocalPosition(int ID);
void gpup_setSubPosition( int ID, int pos, float x, float y, float z ); // Subemitter 0 is the overall global position of the effect.
int gpup_addSubPosition( int ID, float x, float y, float z ); // Add a sub-emitter
int gpup_deleteSubPosition( int ID, int pos );
int gpup_getSubPositionCount( int ID ); // query the number of current sub-emitters
void gpup_emitterBurstLoopAutoMode(int ID, int iAuto); // Control if burst repeats
void gpup_emitterBurstMode(int ID, int iMode); // Set burst mode on/off
void gpup_emitterFire( int ID ); // Fire a burst emitter
void gpup_setBlendmode( int ID, int usel );
void gpup_setAutoFire( int ID, int act ); // Toggle the auto fire option for burst emitters
void gpup_floorReflection(int ID, int active, float height);
void gpup_restoreFloorReflection(int ID, int active, float height);
void gpup_sphereReflection( int ID, int active, float x, float y, float z, float radius ) ;
void gpup_setBounciness( int ID, float value ); // Change the bounciness of the particles of an effect with reflections
void gpup_setLine( int ID, float x1, float y1, float z1, float x2, float y2, float z2 ); // Sets the end points of line and beam emitters
void gpup_setLineLocal( int ID, float x1, float y1, float z1, float x2, float y2, float z2 ); // Sets the end points of line and beam emitters
void gpup_setLineBeams( int ID, int beamcount, float beamoffset, float beamcurl ); // Adds more than one beam to a line emitter (up to 10) and sets the offset between the duplicated beams
void gpup_setEffectColor( int ID, int r, int g, int b ); // Set a global Tint for an effect. This helps to integrate alpha and clip blended effects into your scene. 
void gpup_setEffectOpacity(int ID, float a); // Set global opacity for effect.
void gpup_setSmoothBlending( int ID, int act ); // Activate or deactivate smooth blending of spritesheet animations
int gpup_deleteEffect( int ID );
void gpup_deleteAllEffects(void);
void gpup_setEffectVisible( int ID, int visible );
int gpup_getEffectVisible ( int ID );
float gpup_getParticleSpeed(int ID);
float gpup_getParticleOpacity(int ID);
float gpup_getParticleSize(int ID);
float gpup_getBounciness(int ID);
float gpup_getFloorReflectionHeight(int ID);
void gpup_getEffectColor(int ID, float* r, float* g, float* b);
float gpup_getEffectLifespan( int ID );
void gpup_setEffectLifespan(int ID, float value);
void gpup_setEffectAnimationSpeed(int ID, float speed);
void gpup_setBilinear( int ID, int active );

#ifndef NOTFORMAINENGINE
void gpup_update( float frameTime, wiGraphics::CommandList cmd ); // must not be called from a render pass
extern "C" void gpup_draw( const wiScene::CameraComponent& camera, wiGraphics::CommandList cmd ); // must be called from a render pass
extern "C" void gpup_draw_init(const wiScene::CameraComponent & camera, wiGraphics::CommandList cmd); // must be called from a render pass
extern "C" void gpup_draw_bydistance(const wiScene::CameraComponent & camera, wiGraphics::CommandList cmd, float fDistanceFromCamera); // must be called from a render pass
#endif

}