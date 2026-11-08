-- DESCRIPTION: A global script that controls game & player logic. Do not assign to an object.
local N = require "scriptbank\\perlin_noise"

local max    = math.max
local min    = math.min
local random = math.random

math.randomseed(os.time())
random(); random(); random()

local gameplayercontrol = {}

g_gunandmeleemouseheld = 0
g_meleekeynotfree = 0
g_meleeclicknotfree = 0		   
g_lmbheld = 0
g_lmbheldtime = 0
g_plrkeyRheld = 0
g_jamadjust = 0
g_forcealtswap = 0
g_keyboardpress = 0
g_mousewheeltimer = 0			 
g_mousewheellast = 0
g_suspendplayercontrols = 0
g_playercontrolcooldownmode = 0
g_specialPBRDebugView = 0
g_FootFallTimer = 0
g_FootFallLeftOrRight = 0
g_FootFallWalkOrRun = 0
g_gameplayercontrol_rotate45 = 0
g_gameplayercontrol_lasthmdDeltaY=0
g_gameplayercontrol_lasthmdDeltaP=0
g_gameplayercontrol_lasthmdDeltaR=0
g_gameplayercontrol_lastTimeMoved=123
g_gameplayercontrol_powerjump=0
g_swimimpulsetimer = 0
g_swimeffects = 0
g_underwatersoundon = 0
g_VRswitchweapondelay = 0
g_vrthumbx = 0
g_vrthumby = 0
g_gravitymode = 0
g_gravitymoderefresh = 1
g_gravityhold = 0

function gameplayercontrol.main()
 -- grab thumb state just once
 g_vrthumbx = 0
 g_vrthumby = 0
 if GetHeadTracker() == 1 then
  if GetGamePlayerStateMotionController() == 1 and GetGamePlayerStateMotionControllerType() == 2 then -- OPENXR
	g_vrthumbx = CombatControllerThumbstickX()
	g_vrthumby = CombatControllerThumbstickY()
  end
 end
 -- do functions   
 gameplayercontrol.jetpack()
 gameplayercontrol.weaponfire()
 gameplayercontrol.weaponselectzoom()
 gameplayercontrol.lookmove()
 gameplayercontrol.control()
 gameplayercontrol.combatmusic()							
 if ( g_Scancode == 87 and IsTestGame()) then g_specialPBRDebugView = 1 end
 if ( g_specialPBRDebugView == 1 ) then gameplayercontrol.debug() end
 -- General VR thumb interaction delay so not too quick
 if GetHeadTracker() == 1 and g_VRswitchweapondelay == 1 then
  if math.abs(g_vrthumbx) < 0.5 and math.abs(g_vrthumby) < 0.5 then
   g_VRswitchweapondelay = 0
  end
 end
end
   
function gameplayercontrol.jetpack()
 
   -- Jet Pack Controls
   if ( GetGamePlayerControlJetpackMode() ~= 0 ) then 
      if ( GetGamePlayerControlJetpackMode() == 2 ) then 
         if ( (bit32.band(g_MouseClickControl,2)) ~= 0 and GetGamePlayerControlJetpackFuel()>0 and GetCameraPositionY(0)<3000.0 ) then 
            -- if jetpack starting for first time
            if ( GetGamePlayerControlJetpackParticleEmitterIndex() == 0 ) then 
               -- start jet emitter
               tResult = ParticlesGetFreeEmitter()
               if ( tResult>0 ) then 
                  SetGamePlayerControlJetpackParticleEmitterIndex(tResult)
                  ParticlesAddEmitter(tResult,1/64.0,1,-20,-20,-20,20,20,20,5,10,90,100,-0.1,-0.9,-0.1,0.1,-0.1,0.1,-0.1,0.1,1000,2000,40,75,0,0,25)
               end
            end
            -- apply player upward thrust
            SetGamePlayerControlJetpackThrust(GetGamePlayerControlJetpackThrust()+(0.75*GetTimeElapsed()))
            if ( GetGamePlayerControlJetpackThrust()>1.0 ) then 
               SetGamePlayerControlJetpackThrust(1.0)
            end
            -- play thrust sound with volume control
            if ( RawSoundExist(GetGamePlayerControlSoundStartIndex()+18) == 1 ) then 
               if ( RawSoundPlaying(GetGamePlayerControlSoundStartIndex()+18) == 0 ) then 
                  LoopRawSound ( GetGamePlayerControlSoundStartIndex()+18 )
               end
            end
            if ( RawSoundExist(GetGamePlayerControlSoundStartIndex()+18) == 1 ) then 
               SetRawSoundVolume ( GetGamePlayerControlSoundStartIndex()+18, 70+(30*GetGamePlayerControlJetpackThrust()) )
            end
            -- deduct fuel
            SetGamePlayerControlJetpackFuel(GetGamePlayerControlJetpackFuel() - (0.1*GetTimeElapsed()))
            if ( GetGamePlayerControlJetpackFuel() < 0 ) then SetGamePlayerControlJetpackFuel(0.0) end
         end
         if ( g_PlrKeyJ == 1 ) then 
            SetGamePlayerControlJetpackMode(3)
         end
      end
      if ( GetGamePlayerControlJetpackMode() == 1 ) then 
         if ( g_PlrKeyJ == 0 ) then 
            SetGamePlayerControlJetpackMode(2)
            if ( GetGamePlayerControlJetpackHidden() == 0 ) then 
               ChangePlayerWeaponID(0)
            end
         end
      end
      if ( GetGamePlayerControlJetpackMode() == 3 ) then 
         if ( g_PlrKeyJ == 0 ) then 
            SetGamePlayerControlJetpackMode(0)
         end
      end
   else
      if ( g_PlrKeyJ == 1 and GetGamePlayerControlJetpackCollected() == 1 ) then 
         SetGamePlayerControlJetpackMode(1)
      end
   end
   if ( GetGamePlayerControlJetpackThrust()>0 ) then 
      -- reduce thrust using inertia
      SetGamePlayerControlJetpackThrust(GetGamePlayerControlJetpackThrust()-(GetTimeElapsed()*0.05))
      if ( GetGamePlayerControlJetpackThrust() <= 0 ) then 
         -- jet pack switches off
         if ( RawSoundExist(GetGamePlayerControlSoundStartIndex()+18) == 1 ) then StopRawSound ( GetGamePlayerControlSoundStartIndex()+18 ) end
         SetGamePlayerControlJetpackThrust(0.0)
         -- stop particle emitter
         if ( GetGamePlayerControlJetpackParticleEmitterIndex()>0 ) then 
            ParticlesDeleteEmitter(GetGamePlayerControlJetpackParticleEmitterIndex())
            SetGamePlayerControlJetpackParticleEmitterIndex(0)
         end
      end
      -- thrust noise volume control
      if ( RawSoundExist(GetGamePlayerControlSoundStartIndex()+18) == 1 ) then 
         SetRawSoundVolume ( GetGamePlayerControlSoundStartIndex()+18, 70+(30*GetGamePlayerControlJetpackThrust()) )
      end
   end

end

function gameplayercontrol.weaponfire()
   -- carrying an entity (pickuppable.lua script) then return
   if PU_GetEntityCarried ~= nil and PU_GetEntityCarried() ~= nil then return end

   -- Mouse based control
   SetGamePlayerStateFiringMode(0)
   if ( g_PlayerHealth>0 or GetGamePlayerControlStartStrength() == 0 ) then 
      -- Melee control
      if ( g_gunandmeleemouseheld == 1 ) then 
         if ( g_MouseClick ~= 1 or GetWeaponAmmo(GetGamePlayerStateWeaponAmmoIndex()+GetGamePlayerStateAmmoOffset())>0 ) then g_gunandmeleemouseheld = 0 end
      end
      -- Melee with a key press
      if ( GetGamePlayerStateGunMeleeKey()>0 ) then 
         if ( GetKeyState(GetGamePlayerStateGunMeleeKey()) == 1 and g_meleekeynotfree == 0 and GetGamePlayerStateIsMelee() == 0 and GetGamePlayerStateBlockingAction() == 0) then 
            SetGamePlayerStateIsMelee(1) 
            g_meleekeynotfree = 1 
         end
         if ( g_meleekeynotfree == 1 and GetKeyState(GetGamePlayerStateGunMeleeKey()) == 0 ) then g_meleekeynotfree = 0 end
      else
         g_meleekeynotfree=0
      end
      -- Melee or Block with a right mouse click
	  if ( GetFireModeSettingsBlockWithRightClick() == 1 ) then
		  if ( g_MouseClick == 2 and g_meleeclicknotfree == 0 and GetGamePlayerStateBlockingAction() == 0 ) then 
			 SetGamePlayerStateBlockingAction(1) 
			 g_meleeclicknotfree = 1
		  end
	  else
		  if ( GetFireModeSettingsMeleeWithRightClick() == 1 ) then
		     if ( g_MouseClick == 2 and g_meleeclicknotfree == 0 and GetGamePlayerStateIsMelee() == 0 ) then 
			   SetGamePlayerStateIsMelee(1) 
			   g_meleeclicknotfree = 1 
		     end
		  else
		     g_meleeclicknotfree = 0
		  end
	  end
	  if ( g_meleeclicknotfree == 1 and g_MouseClick ~= 2 ) then g_meleeclicknotfree = 0 end
	  -- handle melee																								  
      if ( GetWeaponAmmo(GetGamePlayerStateWeaponAmmoIndex()+GetGamePlayerStateAmmoOffset())>0 or GetFireModeSettingsReloadQty() == 0 ) then 
         if ( (bit32.band(g_MouseClickControl,1)) == 1 and GetGamePlayerStateBlockingAction() == 0 and GetGamePlayerStateIsMelee() == 0 ) then SetGamePlayerStateFiringMode(1) end
         if ( GetFireModeSettingsIsEmpty()>0 ) then 
            if ( GetGamePlayerStateGunMode() ~= 7031 and GetGamePlayerStateGunMode() ~= 7041 ) then 
               if ( GetGamePlayerStateGunMode()<123 or GetGamePlayerStateGunMode()>124 ) then 
                  if ( GetGamePlayerStateGunMode()<700 or GetGamePlayerStateGunMode()>707 ) then SetFireModeSettingsIsEmpty(0) end
               end
            end
         end
      else
         if ( GetFireModeSettingsIsEmpty() == 0 and GetFireModeSettingsHasEmpty() == 1 ) then SetFireModeSettingsIsEmpty(1) end
         if ( (bit32.band(g_MouseClickControl,1)) == 1 and g_gunandmeleemouseheld == 0 and GetGamePlayerStateBlockingAction() == 0 and GetGamePlayerStateIsMelee() == 0 ) then SetGamePlayerStateGunShootNoAmmo(1) end
      end

      -- Firing the weapon if mouseclick pressed
      if ( (bit32.band(g_MouseClickControl,1)) == 1 ) then 
         SetGamePlayerStateFiringMode(1)
      end

      -- VR trigger can fire weapon
      if GetHeadTracker() == 1 then
         if GetGamePlayerStateMotionController() == 1 and GetGamePlayerStateMotionControllerType() == 2 then -- OPENXR
            if CombatControllerTrigger() > 0.5 then
               SetGamePlayerStateFiringMode(1)
            end
         else
            if g_KeyPressE == 1 then
               SetGamePlayerStateFiringMode(1)
            end
         end
      end
      
      -- Hold down fire button value (can be interrupted when weapon runs out or is reloading)
      holdDownMouseClickButton = (bit32.band(g_MouseClickControl,1))
      holdDownMouseClickButton2 = (bit32.band(g_MouseClickControl,2))
      if ( GetGamePlayerStateGunMode() <= 8 or GetGamePlayerStateGunMode() == 9999 ) then
         holdDownMouseClickButton = 0
         holdDownMouseClickButton2 = 0
      end

      -- Gun jamming Timer
      if ( holdDownMouseClickButton == 1 ) then 
         if ( g_lmbheld == 0 ) then 
            if ( GetFireModeSettingsJammed() == 1 ) then 
               -- if tried to fire, but jammed, dry fire
               if ( GetGamePlayerStateAlternate() == 0 ) then 
                  tsndid = GetGamePlayerStateGunSound(3)
               else
                  tsndid = GetGamePlayerStateGunAltSound(3)
               end
               if ( tsndid > 0 ) then 
                  if ( RawSoundExist(tsndid) == 1 ) then 
                     if ( RawSoundPlaying(tsndid) == 0 ) then 
                        PlayRawSound(tsndid)
                     end
                  end
               end
            end
            g_jamadjust = 0
            g_lmbheld = 1
            g_lmbheldtime = Timer()
         end
      end

      -- Track mouse held times (for lmb/rmb and jam adjust control) then
      if ( holdDownMouseClickButton2 == 2 ) then 
         if ( g_rmbheld == 0 ) then 
            g_rmbheld = 1
            g_rmbheldtime = Timer()
         end
      end
      if ( holdDownMouseClickButton == 0 ) then 
         g_lmbheld = 0
         g_lmbheldtime = 0
         g_jamadjust = 0
      end
      if ( holdDownMouseClickButton2 == 0 ) then 
         g_rmbheld = 0
         g_rmbheldtime = 0
      end

      -- Weapon jammed
      if ( GetFireModeSettingsJamChance() ~= 0 ) then 
         -- amount of minimum delay before jamming can occour
         if ( GetFireModeSettingsMinTimer() ~= 0 ) then 
            tmintimer=GetFireModeSettingsMinTimer()
         else
            tmintimer=500
         end
         -- increase jam change as barrels overheat
         if ( g_lmbheld ~= 0 ) then 
            if ( math.floor((Timer()-g_lmbheldtime)/100)>(math.floor((Timer()-g_lmbheldtime)/100)) ) then 
               g_jamadjust=g_jamadjust+5
            end
         end
         if ( (GetGamePlayerStateGunMode() == 105 or GetGamePlayerStateGunMode() == 106) and (Timer()-GetFireModeSettingsAddTimer())>(tmintimer-g_jamadjust) ) then 
            -- semi-auto fire
            SetFireModeSettingsShotsFired(GetFireModeSettingsShotsFired()+1)
            SetFireModeSettingsCoolTimer(Timer())
            SetFireModeSettingsAddTimer(Timer())
         end
         if ( GetGamePlayerStateGunMode() == 104 and (Timer()-GetFireModeSettingsAddTimer())>100 ) then 
            -- full-auto fire
            SetFireModeSettingsShotsFired(GetFireModeSettingsShotsFired()+1)
            SetFireModeSettingsCoolTimer(Timer())
            SetFireModeSettingsAddTimer(Timer())
         end
         if ( GetGamePlayerStateFiringMode() == 1 and GetFireModeSettingsJammed() == 0 ) then 
            tchance=99999
            if ( Timer()>g_lmbheldtime+(GetFireModeSettingsOverheatAfter()*1000) ) then 
               -- beyond overheat range
               if ( Timer()>GetFireModeSettingsJamChanceTime() ) then 
                  SetFireModeSettingsJamChanceTime(Timer()+100)
                  math.randomseed ( Timer() )
                  tjamtemp=100-GetFireModeSettingsJamChance()
                  if ( tjamtemp<0 ) then tjamtemp = 0 end
                  if ( tjamtemp == 0 or math.random(0,tjamtemp) == 0 ) then tchance = 0 end
               end
            end
            if ( GetFireModeSettingsShotsFired()>tchance and GetWeaponAmmo(GetGamePlayerStateWeaponAmmoIndex())>1 ) then 
               SetFireModeSettingsJammed(1)
               -- ensure weapon jams affect both modes if sharing ammo
               if ( GetGamePlayerStateModeShareMags() == 1 ) then 
                  ---g.firemodes[GetGamePlayerStateGunID()][0].settings.jammed=1
                  ---g.firemodes[GetGamePlayerStateGunID()][1].settings.jammed=1
                  SetFireModeSettingsJammed(1)
               end
               if ( GetGamePlayerStateAlternate() == 0 ) then 
                  tsndid=GetGamePlayerStateGunSound(3)
               else
                  tsndid=GetGamePlayerStateGunAltSound(3)
               end
               if ( tsndid>0 ) then 
                  if ( RawSoundExist(tsndid) ) then 
                     PlayRawSound ( tsndid )
                  end
               end
            end
         end
         if ( GetFireModeSettingsJammed() == 1 ) then 
            SetGamePlayerStateFiringMode(0)
            SetFireModeSettingsShotsFired(0)
         end
         -- jam cool-down control
         if ( GetFireModeSettingsCoolDown() ~= 0 ) then 
            ttempcool=Timer()-GetFireModeSettingsCoolTimer()
            if ( ttempcool>GetFireModeSettingsCoolDown() ) then 
               SetFireModeSettingsShotsFired(0)
            end
         end
      end

      -- Underwater weapons fire
      if ( GetFireModeSettingsNoSubmergedFire() == 1 and GetGamePlayerStateUnderwater() == 1 ) then 
         SetGamePlayerStateFiringMode(0)
      end
      if ( (bit32.band(g_MouseClickControl,2)) == 2 and GetFireModeSettingsActionBlockStart() ~= 0 and GetGamePlayerStateBlockingAction() == 0 and GetGamePlayerStateIsMelee() == 0 ) then 
         SetGamePlayerStateBlockingAction(1)  
      end

      -- Trigger Zoom (no Zoom in When Reloading or firing in simple zoom or with gun empty or when running) then
      tttriggerironsight=0
      if ( GetGamePlayerControlThirdpersonEnabled() == 0 ) then
      if ( GetGamePlayerStateRightMouseHold()>0 ) then 
         -- mode to allow toggle with RMB instead of holding it down
         if ( (bit32.band(g_MouseClickControl,2)) == 2 and GetGamePlayerStateRightMouseHold() == 1 ) then SetGamePlayerStateRightMouseHold(2) end
         if ( (bit32.band(g_MouseClickControl,2)) == 0 and GetGamePlayerStateRightMouseHold() == 2 ) then SetGamePlayerStateRightMouseHold(3) end
         if ( (bit32.band(g_MouseClickControl,2)) == 2 and GetGamePlayerStateRightMouseHold() == 3 ) then SetGamePlayerStateRightMouseHold(4) end
         if ( (bit32.band(g_MouseClickControl,2)) == 0 and GetGamePlayerStateRightMouseHold() == 4 ) then SetGamePlayerStateRightMouseHold(1) end
         if ( GetGamePlayerStateRightMouseHold() >= 2 and GetGamePlayerStateRightMouseHold() <= 4 ) then tttriggerironsight = 1 end
      else
         if ( (bit32.band(g_MouseClickControl,2)) == 2  ) then tttriggerironsight = 1 end
      end
      end
      if ( GetGamePlayerStateXBOX() == 1 ) then 
         if ( GetGamePlayerStateXBOXControllerType() == 2 ) then 
            -- Logitech F310
         else
            if ( JoystickZ() > 200  ) then
               tttriggerironsight = 1
            end
         end
      end
      if ( GetGamePlayerControlJetpackMode()>0 ) then tttriggerironsight = 0 end

      -- can now zoom even if have no bullets (only if not in VR)
      if GetHeadTracker() == 0 then
         if ( tttriggerironsight == 1 and (GetGamePlayerStateGunMode() <= 100 or GetFireModeSettingsSimpleZoom() == 0) and (GetFireModeSettingsForceZoomOut() == 0 or GetGamePlayerStateGunMode() <= 100) and GetGamePlayerControlIsRunning() == 0 and GetGamePlayerStateIsMelee() == 0 ) then 
            -- Modified for Simple Zoom
            if ( GetGamePlayerStateGunZoomMode() == 0 and (GetFireModeSettingsZoomMode() ~= 0 or GetFireModeSettingsSimpleZoom() ~= 0) ) then 
               SetGamePlayerStateGunZoomMode(1)  
               SetGamePlayerStateGunZoomMag(1.0)
               if ( GetFireModeSettingsSimpleZoomAnim() ~= 0 and GetFireModeSettingsSimpleZoom() ~= 0  ) then SetGamePlayerStateGunMode(2001) end
               if ( GetGamePlayerStateAlternate() == 0 ) then 
                  tsndid = GetGamePlayerStateGunSound(0)
               else 
                  tsndid = GetGamePlayerStateGunAltSound(0)
               end
               if ( tsndid > 0 ) then
                  if ( RawSoundPlaying ( tsndid ) == 1 ) then
                     PlayRawSound(tsndid,GetCameraPositionX(0),GetCameraPositionY(0),GetCameraPositionZ(0))
                  end
               end
            end
         else
            -- only UNZOOM when we have finished other gun actions
            if ( GetGamePlayerStateGunMode() ~= 106 ) then 
               if ( GetGamePlayerStateGunZoomMode() == 9 and (tttriggerironsight == 0 or GetGamePlayerControlIsRunning() ~= 0) ) then 
                  SetGamePlayerStateGunZoomMode(11)
                  if ( GetFireModeSettingsSimpleZoomAnim() ~= 0 and GetFireModeSettingsSimpleZoom() ~= 0  ) then SetGamePlayerStateGunMode(2003) end
               end
            end
            if ( GetGamePlayerStateGunZoomMode() == 20 ) then SetGamePlayerStateGunZoomMode(0) end
         end
      end

      -- Reload can be from key or from VR thumb down
      local tRealReloadKey = g_PlrKeyR
      if GetHeadTracker() == 1 and g_VRswitchweapondelay == 0 then
         if GetGamePlayerStateMotionController() == 1 and GetGamePlayerStateMotionControllerType() == 2 then -- OPENXR
			if g_vrthumby < -0.5 then tRealReloadKey = 1 g_VRswitchweapondelay = 1 end
         end
      end      
      
      -- Reload system
      if ( g_plrkeyRheld == 1 and tRealReloadKey == 0 ) then g_plrkeyRheld = 0 end
      ttpool=GetFireModeSettingsPoolIndex()
      if ( ttpool == 0  ) then 
         tammo = GetWeaponClipAmmo(GetGamePlayerStateWeaponAmmoIndex()+GetGamePlayerStateAmmoOffset())
      else 
         tammo = GetWeaponPoolAmmo(ttpool)
      end
      if ( tammo == 0 or GetFireModeSettingsReloadQty() == 0 ) then 
         if ( (tRealReloadKey) == 1 and g_plrkeyRheld == 0 ) then 
            SetGamePlayerStateGunReloadNoAmmo(1)   
            g_plrkeyRheld=1
            if ( GetFireModeSettingsJammed() == 1 ) then 
               SetGamePlayerStateFiringMode(2)
            end
         end
      else
         if ( (tRealReloadKey) == 1 and GetGamePlayerStateIsMelee() ~= 2 ) then 
            SetGamePlayerStateFiringMode(2)
            SetGamePlayerStatePlrReloading(1)
            if ( GetGamePlayerStateIsMelee() == 1 ) then SetGamePlayerStateIsMelee(0) end
         end
      end
   end

   -- ensure weapon unjam if jammed
   if ( GetGamePlayerStateGunZoomMode() ~= 0 ) then tRealReloadKey = 0 end
   if ( GetFireModeSettingsJammed() == 1 and (tRealReloadKey) == 1 and GetGamePlayerStateIsMelee() ~= 2 and GetFireModeSettingsSimpleZoomAnim() == 0 and GetFireModeSettingsSimpleZoom() == 0 ) then 
      SetGamePlayerStateFiringMode(2)
   end

end

function gameplayercontrol.weaponselectzoom()

   -- Weapon Select can be from keys 1,2,3,4,5,6,7,8,9,0 or in VR right joystick left/right stick (see below)
   tselkeystate = 0
   if ( tselkeystate == 0 ) then 
        for ti = 2, 11, 1 do
         if ( GetKeyState(ti)==1 ) then 
            tselkeystate = ti 
            break
         end
      end
   end
   
   -- VR can alt swap if push upward
   if GetHeadTracker() == 1 and g_VRswitchweapondelay == 0 then
      if GetGamePlayerStateMotionController() == 1 and GetGamePlayerStateMotionControllerType() == 2 then -- OPENXR
         if g_vrthumby > 0.5 then g_forcealtswap = 1 g_VRswitchweapondelay = 1 end
      end
   end      

   -- Keyboard
   taltswapkeycalled = 0
   if ( GetGamePlayerStateGunAltSwapKey1()>-1 and GetKeyState(GetGamePlayerStateGunAltSwapKey1())==1 and (GetGamePlayerStateGunZoomMode() == 0 or (GetGamePlayerStateGunZoomMode()>=8 and GetGamePlayerStateGunZoomMode()<=10)) or g_forcealtswap == 1 ) then 
      taltswapkeycalled = 1 
      g_forcealtswap = 0 
   end
   
   --if ( tselkeystate>0 and GetGamePlayerStateIsMelee() == 0 and (GetGamePlayerStateGunZoomMode() == 0 or (GetGamePlayerStateGunZoomMode()>=8 and GetGamePlayerStateGunZoomMode()<=10)) or taltswapkeycalled == 1 ) then 
   if ( tselkeystate>0 and (GetGamePlayerStateGunZoomMode() == 0 or (GetGamePlayerStateGunZoomMode()>=8 and GetGamePlayerStateGunZoomMode()<=10)) or taltswapkeycalled == 1 ) then 
      if ( taltswapkeycalled == 1 and GetGamePlayerStateGunAltSwapKey2()>-1 and GetKeyState(GetGamePlayerStateGunAltSwapKey2())==1 or taltswapkeycalled == 1 and GetGamePlayerStateGunAltSwapKey2() == -1 or taltswapkeycalled == 0 ) then 
         if ( g_keyboardpress == 0 ) then 
            -- Change weapon - and prevent plr selecting gun if flagged
            if ( (g_PlayerHealth>0 or GetGamePlayerControlStartStrength() == 0) ) then 
               -- if in zoom mode, switch out at same time
			   if ( GetGamePlayerStateGunZoomMode() == 10 ) then SetGamePlayerStateGunZoomMode(11) end
               -- Weapon select - Allow moving and switching
               twepsel=tselkeystate-1
               -- Cannot keypress until release
               g_keyboardpress=tselkeystate
               if ( tselkeystate == 11 ) then twepsel = -1 end
               if ( taltswapkeycalled == 1 ) then 
                  g_keyboardpress = -2   
                  twepsel = -2 
               end
               -- Actual weapon change
               if ( taltswapkeycalled == 0 ) then 
                  if ( twepsel>0 ) then 
				     local currentlyholding = GetGamePlayerStateGunID()
					 local newonechosen = GetWeaponSlotGot(twepsel)
				     if currentlyholding == newonechosen then
					   twepsel = 0
				     else
                       SetGamePlayerStateWeaponKeySelection(twepsel)
                       if ( GetGamePlayerStateWeaponKeySelection()>0 ) then 
                        if ( GetWeaponSlotNoSelect(GetGamePlayerStateWeaponKeySelection()) == 0 ) then 
						   SetGamePlayerStateWeaponIndex(newonechosen)
						   if ( GetGamePlayerStateWeaponIndex()>0 ) then SetGamePlayerStateCommandNewWeapon(GetGamePlayerStateWeaponIndex()) end
                        end
                       end
					 end
                  end
                  -- ensure can only put weapon away when in idle mode
                  if ( twepsel == -1 ) then 
                     if ( GetGamePlayerStateGunMode()<31 or GetGamePlayerStateGunMode()>35 ) then 
                        if ( GetGamePlayerStateGunID() ~= 0 ) then 
                           SetGamePlayerStateGunMode(31)  
                           SetGamePlayerStateGunSelectionAfterHide(0)
                        end
                     end
                  end
               end
               if ( taltswapkeycalled == 1 and GetWeaponSlotNoSelect(GetGamePlayerStateWeaponKeySelection()) == 0 ) then 
                  if ( GetGamePlayerStateGunID() ~= 0 ) then 
                     if ( (GetGamePlayerStateAlternateIsFlak() == 1 or GetGamePlayerStateAlternateIsRay() == 1) and GetGamePlayerStateGunMode() <= 100 ) then 
                        -- reset gunburst variable so burst doesn't get confused when switching gunmodes
                        if ( GetGamePlayerStateAlternate() == 1 ) then 
                           SetGamePlayerStateGunMode(2009) 
                           --SetGamePlayerStateAlternate(0) now set inside gunmode 2009 'after' zoom out
                           SetGamePlayerStateGunBurst(0)
                        else 
                           SetGamePlayerStateGunMode(2007) 
                           --SetGamePlayerStateAlternate(1) now set inside gunmode 2009 'after' zoom out
                           SetGamePlayerStateGunBurst(0) 
                        end
                     end
                  end
               end
            end
            taltswapkeycalled=0
         end
      end
   end
   if ( g_keyboardpress>0 ) then 
      if ( GetKeyState(g_keyboardpress) == 0 ) then g_keyboardpress = 0 end
   end
   if ( g_keyboardpress == -2 and taltswapkeycalled == 0 ) then g_keyboardpress = 0 end

   --  Mouse Wheel scrolls weapons
   ttmz = 0
   if g_mousewheeltimer == 0 or Timer() > g_mousewheeltimer+500 then
    ttmz = g_MouseWheel - g_mousewheellast
   end
   g_mousewheellast = g_MouseWheel
   g_mousewheellast = g_MouseWheel

   -- Controller only if ready for gun swap
   if ( GetGamePlayerStateXBOX() == 1 ) then 
      if ( GetGamePlayerStateGunMode() >= 5 and GetGamePlayerStateGunMode()<31 ) then 
         if ( JoystickHatAngle(0) == 9000  ) then ttmz = 10 end
         if ( JoystickHatAngle(0) == 27000  ) then ttmz = -10 end
         if ( JoystickFireXL(4) == 1  ) then ttmz = -10 end
         if ( JoystickFireXL(5) == 1  ) then ttmz = 10 end
      end
   end
   
   -- Weapon Select can be from VR right joystick left/right stick (see below)
   if GetHeadTracker() == 1 and g_VRswitchweapondelay == 0 then
      if GetGamePlayerStateMotionController() == 1 and GetGamePlayerStateMotionControllerType() == 2 then -- OPENXR
         if g_vrthumbx < -0.5 then ttmz = -10 g_VRswitchweapondelay = 1 end
         if g_vrthumbx > 0.5 then ttmz = 10 g_VRswitchweapondelay = 1 end
      end
   end      

   -- Change weapon using scroll wheel
   if ( GetGamePlayerStateIsMelee() == 0 and (GetGamePlayerStateGunZoomMode() == 0 or GetGamePlayerStateGunZoomMode() == 10) ) then 
      if ( ttmz ~= 0 ) then 
         -- and prevent player changing guns with joystick
         ttokay=0  
         tttries=10
         ttweaponindex=0
         while ( ttokay == 0 and tttries>0 ) do
            if ( ttmz<0 ) then SetGamePlayerStateWeaponKeySelection(GetGamePlayerStateWeaponKeySelection()-1) end
            if ( ttmz>0 ) then SetGamePlayerStateWeaponKeySelection(GetGamePlayerStateWeaponKeySelection()+1) end
            if ( GetGamePlayerStateWeaponKeySelection()>10 ) then SetGamePlayerStateWeaponKeySelection(1) end
            if ( GetGamePlayerStateWeaponKeySelection()<1 ) then SetGamePlayerStateWeaponKeySelection(10) end
            if ( GetWeaponSlotNoSelect(GetGamePlayerStateWeaponKeySelection()) == 0 ) then 
               ttweaponindex = GetWeaponSlotGot(GetGamePlayerStateWeaponKeySelection()) 
            end
            if ( ttweaponindex>0 ) then ttokay = 1 end
            tttries = tttries - 1
         end
         if ( ttokay == 1 ) then 
            -- change up or down
            SetGamePlayerStateWeaponIndex(GetWeaponSlotGot(GetGamePlayerStateWeaponKeySelection()))
            if ( GetGamePlayerStateWeaponIndex()>0 ) then SetGamePlayerStateCommandNewWeapon(GetGamePlayerStateWeaponIndex()) end
            if ( GetGamePlayerStateGunZoomMode() == 10 ) then SetGamePlayerStateGunZoomMode(11) end
         end
         g_mousewheeltimer = Timer()									
      end
   else
      -- zoom magnification wheel
      if ( ttmz ~= 0 ) then 
         ttmz_f=ttmz  
         SetGamePlayerStateGunZoomMag(GetGamePlayerStateGunZoomMag()+(ttmz_f/100.0))
         if ( GetGamePlayerStateGunZoomMag()<1.0 ) then SetGamePlayerStateGunZoomMag(1.0) end
         if ( GetGamePlayerStateGunZoomMag()>10.0 ) then SetGamePlayerStateGunZoomMag(10.0) end
         SetGamePlayerStatePlrZoomInChange(1)
      end
   end

   -- Gun Zoom Control
   if ( GetGamePlayerStateGunZoomMode()>0 ) then 
      if ( GetGamePlayerStateGunZoomMode() >= 1 and GetGamePlayerStateGunZoomMode() <= 8 ) then SetGamePlayerStateGunZoomMode(GetGamePlayerStateGunZoomMode()+1) end
      if ( GetGamePlayerStateGunZoomMode() >= 11 and GetGamePlayerStateGunZoomMode() <= 19 ) then SetGamePlayerStateGunZoomMode(GetGamePlayerStateGunZoomMode()+1) end
      if ( GetGamePlayerStateGunZoomMode()<10 ) then 
         SetGamePlayerStatePlrZoomIn(GetGamePlayerStateGunZoomMode())  
         SetGamePlayerStatePlrZoomInChange(GetGamePlayerStatePlrZoomInChange()+1)
      end
      if ( GetGamePlayerStateGunZoomMode() == 10 ) then 
         -- in full zoom
      end
      if ( GetGamePlayerStateGunZoomMode()>10 ) then 
         SetGamePlayerStatePlrZoomIn(10-(GetGamePlayerStateGunZoomMode()-10))
         SetGamePlayerStatePlrZoomInChange(1)
      end
   end

   -- Handle optical effect of zoom
   if ( GetGamePlayerStatePlrZoomInChange() == 1 ) then 
      if ( GetGamePlayerStatePlrZoomIn()>1.0 ) then 
         if ( GetFireModeSettingsSimpleZoom() ~= 0 ) then 
            trealfovdegree=GetGamePlayerStateCameraFovZoomed()*((GetGamePlayerStatePlrZoomIn()*GetFireModeSettingsSimpleZoom()))
            SetGamePlayerStateRealFov(GetGamePlayerStateCameraFov()-trealfovdegree)
            if ( GetGamePlayerStateRealFov()<7 ) then SetGamePlayerStateRealFov(7) end
            SetCameraFOV ( 0, GetGamePlayerStateRealFov() )
            SetCameraFOV ( 2, GetGamePlayerStateRealFov() )
         else
            trealfovdegree=GetGamePlayerStateCameraFovZoomed()*((GetGamePlayerStatePlrZoomIn()*7)-GetGamePlayerStateGunZoomMag())
            SetGamePlayerStateRealFov(GetGamePlayerStateCameraFov()-trealfovdegree)
            if ( GetGamePlayerStateRealFov()<15 ) then SetGamePlayerStateRealFov(15) end
            SetCameraFOV ( 0, GetGamePlayerStateRealFov() )
            SetCameraFOV ( 2, GetGamePlayerStateRealFov() )
         end
      else
         trealfovdegree=GetGamePlayerStateCameraFovZoomed()
         SetGamePlayerStateRealFov(GetGamePlayerStateCameraFov()+trealfovdegree)
         if ( GetGamePlayerStateRealFov()<15 ) then SetGamePlayerStateRealFov(15) end
         SetCameraFOV ( 0, GetGamePlayerStateRealFov() )
         SetCameraFOV ( 2, GetGamePlayerStateRealFov() )
      end
   end
   SetGamePlayerStatePlrZoomInChange(0)

end

function gameplayercontrol.lookmove()

   -- determine if any head tracker in use
   ttvrheadtrackermode = 0
   if GetHeadTracker() == 1 then
     ttvrheadtrackermode = 1
    if g_InKey == "p" then
     ResetHeadTracker()
    end
   end
   
   -- completely skip use of mousemovexy so LUA mouse system can use it for its own pointer
   if ( GetGamePlayerStateLuaActiveMouse() ~= 1 or GetGamePlayerStateMotionController() == 1 ) then
      -- No player control if dead, but use up mousemoves to prevent sudden move on respawn or if in multiplayer and respawning
      if ( g_PlayerHealth == 0 or GetGamePlayerStatePlrHasFocus() == 0 or (GetGamePlayerStateGameRunAsMultiplayer() == 1 and GetGamePlayerStateSteamWorksRespawnLeft() ~= 0) ) then 
         tcleardeltas = MouseMoveX() + MouseMoveY()
      end
      -- Tab Mode 2, low FPS screen and construction kit menus require mouse
      if ( GetGamePlayerStateTabMode()<2 and GetGamePlayerStateLowFpsWarning() ~= 1 ) then 
         -- Manipulate camera angle using mouselook
         if ( GetGamePlayerStateMouseInvert() == 1 ) then 
            tttmousemovey = MouseMoveY()*-1 
         else 
            tttmousemovey = MouseMoveY()
         end
         --if ttvrheadtrackermode == 1 then --allow mouselook up/down in VR
         --   tttmousemovey = 0
         --end         
         if ( GetGamePlayerStateSlowMotion() == 1 and GetGamePlayerStateSmoothCameraKeys() == 1 ) then 
            SetGamePlayerStateCamMouseMoveX(GetGamePlayerStateCamMouseMoveX()+(MouseMoveX()/10.0))
            SetGamePlayerStateCamMouseMoveY(GetGamePlayerStateCamMouseMoveY()+(tttmousemovey/10.0))
            SetGamePlayerStateCamMouseMoveX(GetGamePlayerStateCamMouseMoveX()*0.94)
            SetGamePlayerStateCamMouseMoveY(GetGamePlayerStateCamMouseMoveY()*0.94)
         else
            SetGamePlayerStateCamMouseMoveX(MouseMoveX())
            SetGamePlayerStateCamMouseMoveY(tttmousemovey)
         end
         if ( GetGamePlayerStatePlrHasFocus() == 1 ) then 
            PositionMouse ( GetDesktopWidth()/2,GetDesktopHeight()/2 )
         end
      else
         tnull=MouseMoveX() 
         tnull=MouseMoveY()
         SetGamePlayerStateCamMouseMoveX(0)  
         SetGamePlayerStateCamMouseMoveY(0)
      end
      if ( GetGamePlayerStateXBOX() == 1 ) then 
         if ( GetGamePlayerStateXBOXControllerType() == 2 ) then 
            -- Logitech F310 (with deadzones)
            ttjoyrotx=(JoystickZ()+0.0)/1000.0
            --if ttvrheadtrackermode == 1 then  --allow mouselook up/down in VR
            --   ttjoyroty=0
            --else
               ttjoyroty=((JoystickTwistZ()+0.0)-32768.0)/32768.0
            --end
         else
            ttjoyrotx=((JoystickTwistX()+0.0)-32768.0)/32768.0
            ttjoyroty=((JoystickTwistY()+0.0)-32768.0)/32768.0
         end
         if ( ttjoyrotx>-0.3 and ttjoyrotx<0.3 ) then ttjoyrotx = 0 end
         if ( ttjoyroty>-0.3 and ttjoyroty<0.3 ) then ttjoyroty = 0 end
         SetGamePlayerStateCamMouseMoveX(ttjoyrotx*6.0)
         SetGamePlayerStateCamMouseMoveY(ttjoyroty*6.0)
      end
      if ( GetGamePlayerStateMotionController() == 1 ) then
         -- Speed of turning decided by whether moving forward or static
         -- Get Motion Controller input to control player turning
         if ( GetGamePlayerStateMotionControllerType() == 2 ) then -- OPENXR
            if g_PlrKeyW == 1 or g_PlrKeyS == 1 then
               -- normal smooth way
               if ( MotionControllerThumbstickX() < -0.5 ) then
                  SetGamePlayerStateCamMouseMoveX(GetGamePlayerStateCamMouseMoveX()-4.0)         
               end
               if ( MotionControllerThumbstickX() > 0.5 ) then
                  SetGamePlayerStateCamMouseMoveX(GetGamePlayerStateCamMouseMoveX()+4.0)         
               end
            else
               -- new method rotate 45 degrees left/right on trigger
               if ( g_gameplayercontrol_rotate45 == 0 ) then
                  if ( MotionControllerThumbstickX() < -0.5 ) then
                     g_gameplayercontrol_rotate45 = -44
                  end
                  if ( MotionControllerThumbstickX() > 0.5 ) then
                     g_gameplayercontrol_rotate45 = 44
                  end
               end         
            end
         end
      end
      
      -- Handle slow turning mode of motion controller
      if ( g_gameplayercontrol_rotate45 < 0 ) then
         g_gameplayercontrol_rotate45 = g_gameplayercontrol_rotate45 + 2
         if ( g_gameplayercontrol_rotate45 <= -22 ) then
            SetGamePlayerStateCamMouseMoveX(GetGamePlayerStateCamMouseMoveX()-32.8)--16.4)         
         end
      end
      if ( g_gameplayercontrol_rotate45 > 0 ) then
         g_gameplayercontrol_rotate45 = g_gameplayercontrol_rotate45 - 2
         if ( g_gameplayercontrol_rotate45 >= 22 ) then
            SetGamePlayerStateCamMouseMoveX(GetGamePlayerStateCamMouseMoveX()+32.8)--16.4)         
         end
      end

      -- Modify rotation speed by zoom amount
      ttturnspeedmodifier=1.0
      if ( GetGamePlayerStateGunID()>0 ) then 
         ttturnspeedmodifier=GetFireModeSettingsPlrTurnSpeedMod()
         if ( GetGamePlayerStatePlrZoomIn()>1 ) then 
            -- use gunspec settings to control relative speed of zoom mouselook
            ttturnspeedmodifier=ttturnspeedmodifier*GetFireModeSettingsZoomTurnSpeed()
         end
      end
      ttRotationDivider=8.0/ttturnspeedmodifier
      SetGamePlayerStateCamAngleX(GetGamePlayerStateCamAngleX(0)+(GetGamePlayerStateCamMouseMoveY()/ttRotationDivider))
      if ttvrheadtrackermode == 1 then
         SetGamePlayerStateCamAngleY(GetGamePlayerStateCamAngleY()+(GetGamePlayerStateCamMouseMoveX()/ttRotationDivider))
      else
         SetGamePlayerStateCamAngleY(GetGamePlayerControlFinalCameraAngley()+(GetGamePlayerStateCamMouseMoveX()/ttRotationDivider))
      end
   end

   -- Handle player weapon recoil
   if ( GetGamePlayerControlThirdpersonEnabled() == 0 ) then 
      SetGamePlayerStateGunRecoilAngleY(CurveValue(GetGamePlayerStateGunRecoilY(),GetGamePlayerStateGunRecoilAngleY(),2.0))
      SetGamePlayerStateGunRecoilY(CurveValue(0.0,GetGamePlayerStateGunRecoilY(),1.5))
      SetGamePlayerStateGunRecoilAngleX(CurveValue(GetGamePlayerStateGunRecoilX(),GetGamePlayerStateGunRecoilAngleX(),2.0))
      SetGamePlayerStateGunRecoilX(CurveValue(0.0,GetGamePlayerStateGunRecoilX(),1.5))
      SetGamePlayerStateGunRecoilCorrectY(CurveValue(0.0,GetGamePlayerStateGunRecoilCorrectY(),3.0))
      SetGamePlayerStateGunRecoilCorrectX(CurveValue(0.0,GetGamePlayerStateGunRecoilCorrectX(),3.0))
      SetGamePlayerStateGunRecoilCorrectAngleY(CurveValue(GetGamePlayerStateGunRecoilCorrectY(),GetGamePlayerStateGunRecoilCorrectAngleY(),4.0))
      SetGamePlayerStateGunRecoilCorrectAngleX(CurveValue(GetGamePlayerStateGunRecoilCorrectX(),GetGamePlayerStateGunRecoilCorrectAngleX(),4.0))
      SetGamePlayerStateCamAngleX(GetGamePlayerStateCamAngleX()-GetGamePlayerStateGunRecoilAngleY()+GetGamePlayerStateGunRecoilCorrectAngleY())
      SetGamePlayerStateCamAngleY(GetGamePlayerStateCamAngleY()-GetGamePlayerStateGunRecoilAngleX()+GetGamePlayerStateGunRecoilCorrectAngleX())
   end

   -- Cap look up/down angle so cannot wrap around
   if ( GetGamePlayerStateCamAngleX()<-80 ) then SetGamePlayerStateCamAngleX(-80) end
   if ( GetGamePlayerStateCamAngleX()>85 ) then SetGamePlayerStateCamAngleX(85) end

   -- Handle player ducking
   if ( g_PlrKeyC == 1 ) then 
      SetGamePlayerStatePlayerDucking(1)
   else
      if ( GetGamePlayerStatePlayerDucking() == 1 ) then SetGamePlayerStatePlayerDucking(2) end
      if ( GetCharacterControllerDucking() == 0 and GetGamePlayerControlInWaterState() < 2) then 
         SetGamePlayerStatePlayerDucking(0)
      end
   end

   --  third person can freeze player controls if MID-anim (such as jump) then
   g_suspendplayercontrols=0
   if ( GetGamePlayerControlThirdpersonEnabled() == 1 ) then 
      -- if in middle of playcsi event, suspend controls until finished
      SetGamePlayerStateCharAnimIndex(GetGamePlayerControlThirdpersonCharacterIndex())
      if ( GetCharAnimStatePlayCsi(GetGamePlayerStateCharAnimIndex()) > 0 ) then 
         if ( GetCharSeqTrigger(GetCharAnimStatePlayCsi(GetGamePlayerStateCharAnimIndex())) == 0 ) then 
            g_suspendplayercontrols=1
         end
      end
   end
   
   -- Rotate player camera
   if ( g_PlayerHealth>0 and g_suspendplayercontrols == 0 ) then 
      -- when camera rotation overridden, prevent rotation
      if ( GetCameraOverride() ~= 2 and GetCameraOverride() ~= 3 ) then
         -- detect if HMD in use, and if so, show HMD view with camera zero
         if ttvrheadtrackermode == 1 then
            hmdDeltaMotionY = GetHeadTrackerYaw()
            hmdDeltaMotionP = GetHeadTrackerPitch()
            hmdDeltaMotionR = GetHeadTrackerRoll()
            if g_gameplayercontrol_lastTimeMoved ~= 123 then
               hmdShiftThisCycle = math.abs(hmdDeltaMotionY - g_gameplayercontrol_lasthmdDeltaY)
               hmdShiftThisCycle = hmdShiftThisCycle + math.abs(hmdDeltaMotionP - g_gameplayercontrol_lasthmdDeltaP)
               hmdShiftThisCycle = hmdShiftThisCycle + math.abs(hmdDeltaMotionR - g_gameplayercontrol_lasthmdDeltaR)
               if hmdShiftThisCycle > 0.05 then
                  if g_gameplayercontrol_lastTimeMoved == 0 then
                     PromptDuration ( "Switching to HMD view", 5000 ) 
                  end
                  g_gameplayercontrol_lastTimeMoved = Timer()
               end
            end
            g_gameplayercontrol_lasthmdDeltaY = hmdDeltaMotionY
            g_gameplayercontrol_lasthmdDeltaP = hmdDeltaMotionP
            g_gameplayercontrol_lasthmdDeltaR = hmdDeltaMotionR
         else   
            g_gameplayercontrol_lastTimeMoved = 0
         end
		 
         if ttvrheadtrackermode == 1 and 
		    g_gameplayercontrol_lastTimeMoved ~= 0 and 
			Timer() < g_gameplayercontrol_lastTimeMoved + 10000 then 
            -- set camera accoding to HMD direction
            local tHMDDirectionOnXZ = math.deg(math.atan2(GetHeadTrackerNormalX(),GetHeadTrackerNormalZ()))
            SetGamePlayerControlCx(0)
            SetGamePlayerControlCy(tHMDDirectionOnXZ)
            SetGamePlayerControlCz(0)
            SetCameraAngle ( 0, 0, GetGamePlayerStateCamAngleY(), 0 )
         else
            -- camera zero showing keyboard/mouse view
            g_gameplayercontrol_lastTimeMoved = 0
            SetCameraAngle ( 0, GetGamePlayerStateCamAngleX(), GetGamePlayerStateCamAngleY(), 0 )
            SetGamePlayerControlCx(GetCameraAngleX(0))
            SetGamePlayerControlCy(GetCameraAngleY(0))
            SetGamePlayerControlCz(GetCameraAngleZ(0))
         end
      end
	  
      -- when camera overridden, do not allow movement
      if ( GetCameraOverride() ~= 1 and GetCameraOverride() ~= 3 ) then
	     if GetCameraOverride() == 4 then
			-- Gradually slows base speed down (caused by FreezePlayer with value > 1)
			local currentbasespeed = GetGamePlayerControlBasespeed()
			currentbasespeed = currentbasespeed * 0.99
			SetGamePlayerControlBasespeed(currentbasespeed)
			SetGamePlayerControlIsRunning(0)
		 else
			 if ( GetGamePlayerStateEditModeActive() ~= 0 ) then 
				if ( GetGamePlayerStatePlrKeyShift() == 1 or GetGamePlayerStatePlrKeyShift2() == 1 ) then 
				   SetGamePlayerControlBasespeed(4.0)
				else
				   if ( GetGamePlayerStatePlrKeyControl() == 1 ) then 
					  SetGamePlayerControlBasespeed(0.25)
				   else
					  SetGamePlayerControlBasespeed(1.5)
				   end
				end
				SetGamePlayerControlIsRunning(0)
			 else
				if ( GetGamePlayerStatePlayerDucking() == 1 ) then 
				   SetGamePlayerControlBasespeed(0.5)
				   SetGamePlayerControlIsRunning(0)
				else
				   if ( GetGamePlayerControlCanRun() == 1 and (GetGamePlayerStatePlrKeyShift() == 1 or GetGamePlayerStatePlrKeyShift2() == 1) ) then 
				   
					  -- detect extra user defined global for MYSTAMINAMAXIMUM (moving)
					  gameplayerspeed = require "scriptbank\\gameplayerspeed"
					  local baserunningspeed = gameplayerspeed.movingspeed(2.0)
					  SetGamePlayerControlBasespeed(baserunningspeed)
					  
					  if ( GetGamePlayerControlThirdpersonEnabled() == 1 and GetGamePlayerControlThirdpersonCameraFollow() == 1 ) then 
						 -- WASD run speed
					  else
						 if ( g_PlrKeyS == 1 ) then 
							SetGamePlayerControlBasespeed(1.0)
						 else
							if ( g_PlrKeyW  ==  1 ) then 
							   if ( g_PlrKeyA == 1 or g_PlrKeyD == 1 ) then 
								  SetGamePlayerControlBasespeed(baserunningspeed*0.75) -- 2 TO 1.5
							   end
							else
							   if ( g_PlrKeyA == 1 or g_PlrKeyD == 1 ) then 
								  SetGamePlayerControlBasespeed(baserunningspeed*0.75) -- 2 TO 1.5
							   end
							end
						 end
					  end
					  SetGamePlayerControlIsRunning(1)
				   else
					  SetGamePlayerControlBasespeed(1.0)
					  SetGamePlayerControlIsRunning(0)	  
					  -- detect extra user defined global for MYSTAMINAMAXIMUM (recover)
					  gameplayerspeed = require "scriptbank\\gameplayerspeed"
					  gameplayerspeed.recover()		 
				   end
				end
			 end
		 end
         SetGamePlayerControlMaxspeed(GetGamePlayerControlTopspeed()*GetGamePlayerControlBasespeed())
         SetGamePlayerControlMovement(0)
         if ( g_PlrKeyA == 1 ) then 
            SetGamePlayerControlMovement(1)
            SetGamePlayerControlMovey(GetGamePlayerControlCy()-90)
            if ( g_PlrKeyW == 1 ) then SetGamePlayerControlMovey(GetGamePlayerControlCy()-45)
            elseif ( g_PlrKeyS == 1 ) then SetGamePlayerControlMovey(GetGamePlayerControlCy()-45-90) end
         else
            if ( g_PlrKeyD == 1 ) then 
               SetGamePlayerControlMovement(1)
               SetGamePlayerControlMovey(GetGamePlayerControlCy()+90)
               if ( g_PlrKeyW == 1 ) then SetGamePlayerControlMovey(GetGamePlayerControlCy()+45)
               elseif ( g_PlrKeyS == 1 ) then SetGamePlayerControlMovey(GetGamePlayerControlCy()+45+90) end
            else
               if ( g_PlrKeyW == 1 and g_PlrKeyS==0 ) then 
                  SetGamePlayerControlMovement(1)  
                  SetGamePlayerControlMovey(GetGamePlayerControlCy())
               elseif ( g_PlrKeyS == 1 and g_PlrKeyW == 0 ) then 
                  SetGamePlayerControlMovement(1)  
                  SetGamePlayerControlMovey(GetGamePlayerControlCy()+180) 
               end
            end
         end
      else
         -- camera overridden players cannot move themselves
         SetGamePlayerControlMovement(0)
      end
   else
      --  dead players cannot move themselves
      SetGamePlayerControlMovement(0)
   end

end

function gameplayercontrol.footfall()
   -- player footfalls
   SetGamePlayerControlFootfallType(-1)
   if ( GetGamePlayerControlGravityActive() == 1 ) then 
      if ( GetGamePlayerStatePlayerDucking() == 0 ) then 
         SetGamePlayerStatePlayerY(GetPlrObjectPositionY())
      else
         SetGamePlayerStatePlayerY(GetPlrObjectPositionY()+9)
      end
      if ( GetGamePlayerStatePlayerY()>GetGamePlayerStateWaterlineY()+36 or GetGamePlayerStateNoWater() ~= 0 ) then 
         -- takes physcs hit material as Floor (  material ) then
         if ( GetGamePlayerControlPlrHitFloorMaterial()>0 ) then 
            SetGamePlayerControlFootfallType(GetGamePlayerControlPlrHitFloorMaterial()-1)
         else
            SetGamePlayerControlFootfallType(0)
         end
      else
         if ( GetGamePlayerStatePlayerY()>GetGamePlayerStateWaterlineY()-20 and GetGamePlayerControlInWaterState() ~= 2) then 
            -- Footfall water wading sound
            SetGamePlayerControlFootfallType(17)
            -- And also trigger ripples in water
            SetGamePlayerControlRippleCount(GetGamePlayerControlRippleCount()+GetTimeElapsed())
            if ( GetGamePlayerControlMovement() ~= 0  ) then SetGamePlayerControlRippleCount(GetGamePlayerControlRippleCount()+(GetTimeElapsed()*4)) end
            if ( GetGamePlayerControlRippleCount()>20.0 ) then 
               SetGamePlayerControlRippleCount(0.0)
               TriggerWaterRipple(GetPlrObjectPositionX(),GetGamePlayerStateWaterlineY()+0.5,GetPlrObjectPositionZ())
            end
         else
            SetGamePlayerControlFootfallType(18)
         end
      end
   end

   -- Manage trigger of footfall sound effects
   if ( GetGamePlayerControlFootfallType() ~= -1 ) then 
      if ( GetGamePlayerControlMovement() == 0 or GetGamePlayerControlJumpMode() == 1 ) then 
         -- no footfalls here
      else
         tttriggerfootsound=0
         if ( GetGamePlayerControlThirdpersonEnabled() == 1 ) then 
            -- third person uses protagonist animation to trigger footfalling
            tttobjframe=0.0
            tttobj=GetEntityElementObj(GetGamePlayerControlThirdpersonCharactere())
            if ( tttobj>0 ) then 
               if ( GetObjectExist(tttobj) == 1 ) then 
                  tttobjframe=GetObjectFrame(tttobj)
               end
            end
            -- anim based footfall/groundsound triggers (hard coded) then
            for twalktypes = 1, 13, 1 do
               if ( twalktypes == 1 ) then ttleftframe = 306 ttrightframe = 323 end
               if ( twalktypes == 2 ) then ttleftframe = 349 ttrightframe = 369 end
               if ( twalktypes == 3 ) then ttleftframe = 386 ttrightframe = 401 end
               if ( twalktypes == 4 ) then ttleftframe = 429 ttrightframe = 442 end
               if ( twalktypes == 5 ) then ttleftframe = 470 ttrightframe = 479 end
               if ( twalktypes == 6 ) then ttleftframe = 528 ttrightframe = 538 end
               if ( twalktypes == 7 ) then ttleftframe = 555 ttrightframe = 569 end
               if ( twalktypes == 8 ) then ttleftframe = 601 ttrightframe = 615 end
               if ( twalktypes == 9 ) then ttleftframe = 641 ttrightframe = 654 end
               if ( twalktypes == 10 ) then ttleftframe = 674 ttrightframe = 684 end
               if ( twalktypes == 11 ) then ttleftframe = 705 ttrightframe = 716 end
               if ( twalktypes == 12 ) then ttleftframe = 756 ttrightframe = 759 end
               if ( twalktypes == 13 ) then ttleftframe = 817 ttrightframe = 819 end
               if ( tttobjframe >= ttleftframe and tttobjframe <= ttrightframe+1 ) then 
                  if ( GetGamePlayerControlFootfallCount() == 0 and tttobjframe>ttleftframe and tttobjframe<ttrightframe ) then 
                     -- left foot
                     tttriggerfootsound=1
                     SetGamePlayerControlFootfallCount(1)
                  end
                  if ( GetGamePlayerControlFootfallCount() == 1 and tttobjframe>ttrightframe ) then 
                     -- right foot
                     tttriggerfootsound=1
                     SetGamePlayerControlFootfallCount(0)
                  end
               end
            end
         else
            -- FPS triggers footfall based on wobble progress
            if ( GetGamePlayerControlPlrHitFloorMaterial() ~= 0 ) then 
               if GetGamePlayerControlWobble() > 0.0 then 
                  ttFootfallPaceMultiplier = 1.0/(GetGamePlayerControlFootfallPace()/3.0)
                  ttWeaponMoveSpeedMod = GetFireModeSettingsPlrMoveSpeedMod()
                  if ttWeaponMoveSpeedMod == 0.0 then ttWeaponMoveSpeedMod = 1.0 end
                  if ttWeaponMoveSpeedMod < 0.4 then ttWeaponMoveSpeedMod = 0.4 end
                  ttAddWobbleStep = GetGamePlayerControlWobbleSpeed()*GetElapsedTime()*GetGamePlayerControlBasespeed()*GetGamePlayerControlSpeedRatio()*ttFootfallPaceMultiplier*ttWeaponMoveSpeedMod
                  g_FootFallTimer = g_FootFallTimer + ttAddWobbleStep
                  if g_FootFallTimer > 315 and GetGamePlayerControlFootfallCount() == 0 then
                     SetGamePlayerControlFootfallCount(1)
                     tttriggerfootsound=1
                  end
                  if g_FootFallTimer < 315 and GetGamePlayerControlFootfallCount() == 1 then 
                     SetGamePlayerControlFootfallCount(0)
                  end
                  if g_FootFallTimer >= 360 then
                     g_FootFallTimer = g_FootFallTimer - 360
                  end
               end
            end
         end
         if ( tttriggerfootsound == 1 ) then 
            -- play footfall sound effect at specified position
            g_FootFallLeftOrRight = 1 - g_FootFallLeftOrRight
            local footFallWalkOrRun = 0
            if ( GetGamePlayerControlIsRunning() == 1 ) then footFallWalkOrRun = 1 end
            SetGamePlayerControlLastFootfallSound( PlayFootfallSound ( GetGamePlayerControlFootfallType(), GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0), GetGamePlayerControlLastFootfallSound(), g_FootFallLeftOrRight, footFallWalkOrRun ) )            
            -- and if not crouching, will make a noise for AI to detect
            if ( GetGamePlayerStatePlayerDucking() == 0 ) then
               local tradiusofsound = 250
               if GetGamePlayerControlIsRunning() == 1 then tradiusofsound = 500 end
               MakeAISound(GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0),tradiusofsound,1,-1)
            end
         end
      end
   end
end

function gameplayercontrol.thirdperson()
    -- ensure camerafocus is restored each cycle
    SetGamePlayerControlFinalCameraAnglex(GetGamePlayerControlFinalCameraAnglex()+GetGamePlayerControlThirdpersonCameraFocus())
    -- take over control of a character in the game
    ttpersone=GetGamePlayerControlThirdpersonCharactere()  
    ttpersonobj=0
    if ( ttpersone>0 ) then 
       ttpersonobj=GetEntityElementObj(ttpersone)
       if ( GetObjectExist(ttpersonobj) == 0 ) then ttpersonobj = 0 end
    end
    if ( ttpersonobj == 0 ) then 
       return 
    end
    -- person coordinate
    ttpx=GetPlrObjectPositionX()
    ttpy=GetPlrObjectPositionY() + 35
    ttpz=GetPlrObjectPositionZ()

    -- stop control if in ragdoll (dying) then
    if ( GetEntityElementRagdollified(ttpersone) ~= 0 ) then 
       return
    end
      
    -- control animation of third person character
    SetGamePlayerStateCharAnimIndex(GetGamePlayerControlThirdpersonCharacterIndex())
    CopyCharAnimState(GetGamePlayerStateCharAnimIndex(),0)
    SetCharAnimStateE(0,ttpersone)
    if ( GetGamePlayerControlMovement() ~= 0 ) then 
       if ( GetGamePlayerControlThirdpersonEnabled() == 1 and GetGamePlayerControlThirdpersonCameraFollow() == 1 and GetGamePlayerControlCamRightMouseMode() == 0 ) then 
          -- anim based on WASD run mode
          if ( GetGamePlayerControlIsRunning() == 1 ) then 
             SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),5)
          else
             SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),1)
          end
       else
          -- anim based on camera
          if ( GetGamePlayerControlIsRunning() == 1 ) then 
             if ( GetGamePlayerControlMovey() == GetGamePlayerControlCy()-45 or GetGamePlayerControlMovey() == GetGamePlayerControlCy()-90 or GetGamePlayerControlMovey() == GetGamePlayerControlCy()-45-90 ) then 
                SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),7)
             else
                if ( GetGamePlayerControlMovey() == GetGamePlayerControlCy()+45 or GetGamePlayerControlMovey() == GetGamePlayerControlCy()+90 or GetGamePlayerControlMovey() == GetGamePlayerControlCy()+45+90 ) then 
                   SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),8)
                else
                   if ( GetGamePlayerControlMovey() == GetGamePlayerControlCy()+180 ) then 
                      SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),2)
                   else
                      SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),5)
                   end
                end
             end
          else
             if ( GetGamePlayerControlMovey() == GetGamePlayerControlCy()-45 or GetGamePlayerControlMovey() == GetGamePlayerControlCy()-90 or GetGamePlayerControlMovey() == GetGamePlayerControlCy()-45-90 ) then 
                SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),3)
             else
                if ( GetGamePlayerControlMovey() == GetGamePlayerControlCy()+45 or GetGamePlayerControlMovey() == GetGamePlayerControlCy()+90 or GetGamePlayerControlMovey() == GetGamePlayerControlCy()+45+90 ) then 
                   SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),4)
                else
                   if ( GetGamePlayerControlMovey() == GetGamePlayerControlCy()+180 ) then 
                      SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),2)
                   else
                      SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),1)
                   end
                end
             end
          end
       end
    else
       SetCharAnimControlsMoving(GetGamePlayerStateCharAnimIndex(),0)
    end

    -- character Animation Speed
    if ( GetObjectSpeed(GetCharAnimStateObj(0))<0 ) then
       tpolarity = -1 
    else 
       tpolarity = 1
    end
    ttfinalspeed=GetEntityElementSpeedModulator(GetCharAnimStateE(0))*GetCharAnimStateAnimationSpeed(0)*tpolarity*2.5*GetTimeElapsed()
    SetObjectSpeed ( GetCharAnimStateObj(0),ttfinalspeed )

    -- run character animation system
    RunCharLoop()

    -- ensure 'jump' animation is intercepted so anim frame controlled on decent
    fStartFrame=0
    fHoldFrame=0
    fResumeFrame=0
    fFinishFrame=0
    ttentid=GetEntityElementBankIndex(GetCharAnimStateOriginalE(0))
    tq = GetEntityProfileStartOfAIAnim(ttentid) + GetCsiStoodVault(1)
    fStartFrame = GetEntityAnimStart(ttentid,tq)
    fFinishFrame = GetEntityAnimFinish(ttentid,tq)
    if ( GetEntityProfileJumpHold(ttentid) > 0 ) then
       fHoldFrame = GetEntityProfileJumpHold(ttentid)
    else
       fHoldFrame = GetEntityAnimFinish(ttentid,tq) - 10
    end
    if ( GetEntityProfileJumpResume(ttentid) > 0 ) then
       fResumeFrame = GetEntityProfileJumpResume(ttentid)
    else
       fResumeFrame = GetEntityAnimFinish(ttentid,tq)
    end
    if ( GetObjectFrame(GetCharAnimStateObj(0)) >= fStartFrame and GetObjectFrame(GetCharAnimStateObj(0)) <= fFinishFrame ) then 
       -- until we hit terra firma, hold the last 'air' frame
       ttframeposyadjustindex=math.floor(GetObjectFrame(GetCharAnimStateObj(0)))
       ttearlyoutframe = fHoldFrame
       if ( GetGamePlayerControlJumpMode() == 1 and ttframeposyadjustindex>ttearlyoutframe ) then 
          SetObjectFrame ( GetCharAnimStateObj(0),ttearlyoutframe+1.0 )
       end
    end

    -- restore entry in character array
    SetCharAnimStateE(0,0)
    CopyCharAnimState(0,GetGamePlayerStateCharAnimIndex())

    -- transfer first person camera awareness to character
    tttargetx=GetPlrObjectPositionX()
    tttargety=GetPlrObjectPositionY()-35.0
    tttargetz=GetPlrObjectPositionZ()
    PositionObject ( ttpersonobj,tttargetx,tttargety,tttargetz )

    -- in follow mode, only rotate protagonist if moving OR shooting
    ttokay=0
    if ( GetGamePlayerControlThirdpersonCameraFollow() == 0 ) then ttokay = 1 end
    if ( GetGamePlayerControlThirdpersonCameraFollow() == 1 ) then 
       ttokay=2
       if ( GetGamePlayerControlCamRightMouseMode() == 1 ) then ttokay = 1 end
       if ( GetGamePlayerControlThirdpersonShotFired() == 1 ) then 
          SetGamePlayerControlThirdpersonShotFired(0) 
          ttokay = 1 
       end
    end
    if ( ttokay == 1 ) then 
       RotateObject (  ttpersonobj,0,GetCameraAngleY(0),0 )
    end
    if ( ttokay == 2 and (g_PlrKeyW == 1 or g_PlrKeyA == 1 or g_PlrKeyS == 1 or g_PlrKeyD == 1) ) then 
       if ( g_PlrKeyW == 1 ) then 
          if ( g_PlrKeyA == 1 ) then 
             ttadjusta=-45.0
          else
             if ( g_PlrKeyD == 1 ) then 
                ttadjusta=45.0
             else
                ttadjusta=0.0
             end
          end
       else
          if ( g_PlrKeyS == 1 ) then 
             if ( g_PlrKeyA == 1 ) then 
                ttadjusta=225.0
             else
                if ( g_PlrKeyD == 1 ) then 
                   ttadjusta=135.0
                else
                   ttadjusta=180.0
                end
             end
          else
             if ( g_PlrKeyA == 1 ) then ttadjusta = -90.0 end
             if ( g_PlrKeyD == 1 ) then ttadjusta = 90.0 end
          end
       end
       ttfinalrotspeed=2.0/GetTimeElapsed()
       ttfinalobjroty=CurveAngle(GetCameraAngleY(0)+ttadjusta,GetObjectAngleY(ttpersonobj),ttfinalrotspeed)
       RotateObject ( ttpersonobj,0,ttfinalobjroty,0 )
    end

    -- cap vertical angle
    ttcapverticalangle=GetGamePlayerControlFinalCameraAnglex()
    if ( ttcapverticalangle>90 ) then ttcapverticalangle = 90 end
    if ( ttcapverticalangle<-12 ) then ttcapverticalangle = -12 end

    --  retract system when collision moves out way
    if ( GetGamePlayerControlCamCollisionSmooth() == 2 ) then 
       if ( GetGamePlayerControlCamCurrentDistance()<GetGamePlayerControlThirdpersonCameraDistance() ) then 
          ttretractcamspeed=GetGamePlayerControlThirdpersonCameraSpeed()*GetTimeElapsed()
          SetGamePlayerControlCamCurrentDistance(GetGamePlayerControlCamCurrentDistance()+ttretractcamspeed)
          if ( GetGamePlayerControlCamCurrentDistance()>GetGamePlayerControlThirdpersonCameraDistance() ) then 
             SetGamePlayerControlCamCurrentDistance(GetGamePlayerControlThirdpersonCameraDistance())
          end
       end
    end
    ttusecamdistance=GetGamePlayerControlCamCurrentDistance()
    if ( GetGamePlayerControlCamDoFullRayCheck() == 2 ) then 
       SetGamePlayerControlCamDoFullRayCheck(0)
    end
    if ( GetGamePlayerControlCamDoFullRayCheck() == 1 ) then 
       ttusecamdistance=GetGamePlayerControlThirdpersonCameraDistance()
       SetGamePlayerControlCamDoFullRayCheck(2)
    end

    --  work out camera distance and height based on vertical angle (GTA) then
    if ( GetGamePlayerControlThirdpersonCameraLocked() == 1 ) then 
       ttusecamdist=GetGamePlayerControlThirdpersonCameraDistance()
       ttusecamheight=GetGamePlayerControlThirdpersonCameraHeight()
    else
       ttusecamdist=ttusecamdistance
       ttusecamdist=(ttusecamdist*((102.0-(ttcapverticalangle+12.0))/102.0))
       ttusecamheight=(GetGamePlayerControlThirdpersonCameraHeight()*2)-5.0
       ttusecamheight=25.0+(ttusecamheight*((ttcapverticalangle+12.0)/102.0))
    end

    -- the ideal final position of the camera fully extended
    if ( GetGamePlayerControlThirdpersonCameraFollow() == 1 ) then 
       tdaa=WrapValue(180-(0-GetCameraAngleY(0)))
    else
       tdaa=WrapValue(180-(0-GetObjectAngleY(ttpersonobj)))
    end
    ttadjx=(math.sin(math.rad(tdaa))*ttusecamdist)
    tffdcx=tttargetx+ttadjx
    ttadjy=ttusecamheight
    tffdcy=tttargety+ttadjy
    ttadjz=(math.cos(math.rad(tdaa))*ttusecamdist)
    tffdcz=tttargetz+ttadjz
    tffidealdist=math.sqrt(math.abs(tffdcx*tffdcx)+math.abs(tffdcy*tffdcy)+math.abs(tffdcz*tffdcz))

    -- if fixed view, get X angle before collision adjustment
    if ( GetCameraOverride() ~= 1 and GetCameraOverride() ~= 3 ) then
       if ( GetGamePlayerControlThirdpersonCameraLocked() == 1 ) then 
          PositionCamera ( 0,tffdcx,tffdcy,tffdcz )
          PointCamera ( 0,tttargetx,tttargety,tttargetz )
          ttcapverticalangle=GetCameraAngleX(0)
       end
    end

    -- special mode which ignores camera collision (for isometric style games) then
    bIgnoreCameraCollision = false
    if ( GetGamePlayerControlThirdpersonCameraHeight() > GetGamePlayerControlThirdpersonCameraDistance()/2.0 ) then
       bIgnoreCameraCollision = true
    end

    -- check if this places camera in collision
    tthitvalue=0
    if ( bIgnoreCameraCollision==false ) then
       if ( RayTerrain(ttpx,ttpy,ttpz,tffdcx,tffdcy,tffdcz) == 1 ) then tthitvalue = -1 end
       if ( RayTerrain(ttpx,ttpy,ttpz,tffdcx-10,tffdcy,tffdcz-10) == 1 ) then tthitvalue = -1 end
       if ( RayTerrain(ttpx,ttpy,ttpz,tffdcx-10,tffdcy,tffdcz+10) == 1 ) then tthitvalue = -1 end
       if ( RayTerrain(ttpx,ttpy,ttpz,tffdcx+10,tffdcy,tffdcz-10) == 1 ) then tthitvalue = -1 end
       if ( RayTerrain(ttpx,ttpy,ttpz,tffdcx+10,tffdcy,tffdcz+10) == 1 ) then tthitvalue = -1 end
       if ( tthitvalue == 0 ) then 
          tthitvalue=IntersectAll(ttpx,ttpy,ttpz,tffdcx,tffdcy,tffdcz,ttpersonobj)
       end
    end
    if ( tthitvalue ~= 0 ) then 
       if ( tthitvalue == -1 ) then 
          -- terrain hit
          tthitvaluex=GetRayCollisionX()
          tthitvaluey=GetRayCollisionY()
          tthitvaluez=GetRayCollisionZ()
       else
          -- entity hit
          tthitvaluex=GetIntersectCollisionX()
          tthitvaluey=GetIntersectCollisionY()
          tthitvaluez=GetIntersectCollisionZ()
       end

       -- work out new camera position
       SetGamePlayerControlCamCollisionSmooth(0)
       ttddx=ttpx-tthitvaluex
       ttddy=ttpy-tthitvaluey
       ttddz=ttpz-tthitvaluez
       ttdiff=math.sqrt(math.abs(ttddx*ttddx)+math.abs(ttddy*ttddy)+math.abs(ttddz*ttddz))
       if ( ttdiff>12.0 ) then 
          ttddx=ttddx/ttdiff
          ttddy=ttddy/ttdiff
          ttddz=ttddz/ttdiff
          tffdcx=tthitvaluex+(ttddx*5.0)
          tffdcy=tthitvaluey+(ttddy*1.0)
          tffdcz=tthitvaluez+(ttddz*5.0)
          SetGamePlayerControlLastGoodcx(tffdcx)
          SetGamePlayerControlLastGoodcy(tffdcy)
          SetGamePlayerControlLastGoodcz(tffdcz)
          SetGamePlayerControlCamCurrentDistance(ttdiff)
          SetGamePlayerControlCamDoFullRayCheck(1)
       else
          tffdcx=GetGamePlayerControlLastGoodcx()
          tffdcy=GetGamePlayerControlLastGoodcy()
          tffdcz=GetGamePlayerControlLastGoodcz()
       end
    else
       -- this one ensures the full-ray check does not flick the camera to 'full-retract distance'
       if ( GetGamePlayerControlCamDoFullRayCheck() == 2 ) then 
          tffdcx=GetGamePlayerControlLastGoodcx()
          tffdcy=GetGamePlayerControlLastGoodcy()
          tffdcz=GetGamePlayerControlLastGoodcz()
       end
    end

    -- ideal range camera position
    tdcx=tffdcx  
    tdcy=tffdcy  
    tdcz=tffdcz

    -- smooth to destination from current real camera position
    if ( GetGamePlayerControlCamCollisionSmooth() == 0 ) then 
       SetGamePlayerControlCamCollisionSmooth(1)
       ttcx=tdcx
       ttcy=tdcy
       ttcz=tdcz
    else
       if ( GetGamePlayerControlCamCollisionSmooth() == 1 ) then SetGamePlayerControlCamCollisionSmooth(2) end
       ttcx=CurveValue(tdcx,GetCameraPositionX(0),2.0)
       ttcy=CurveValue(tdcy,GetCameraPositionY(0),2.0)
       ttcz=CurveValue(tdcz,GetCameraPositionZ(0),2.0)
    end
    if ( GetCameraOverride() ~= 1 and GetCameraOverride() ~= 3 ) then
       PositionCamera ( 0,ttcx,ttcy,ttcz )
    end

    --  if over shoulder active, use it
    if ( GetGamePlayerControlThirdpersonCameraShoulder() ~= 0 ) then 
       if ( GetCameraOverride() ~= 2 and GetCameraOverride() ~= 3 ) then
          SetCameraAngle ( 0,0,GetGamePlayerControlFinalCameraAngley()+90,0 )
          MoveCamera ( 0,GetGamePlayerControlThirdpersonCameraShoulder() )
       end
    end

    -- adjust horizon angle
    ttcapverticalangle=ttcapverticalangle-GetGamePlayerControlThirdpersonCameraFocus()
	
    -- final camera rotation
    if ( GetCameraOverride() ~= 2 and GetCameraOverride() ~= 3 ) then
       SetCameraAngle ( 0,ttcapverticalangle,GetGamePlayerControlFinalCameraAngley(),GetGamePlayerControlFinalCameraAnglez() )
    end
end
	
-- Camera Shake stuff
local shakeAddTrauma = 0

function GamePlayerControlAddShakeTrauma( num )
	num = num or 25
	if num >= 100 then 
		shakeAddTrauma = 1
	else
		shakeAddTrauma = min( 1, shakeAddTrauma + num / 100 )
	end
end

local shakeSetTrauma = 0

function GamePlayerControlSetShakeTrauma( num )
	num = num or 0
	if num >= 100 then 
		shakeSetTrauma = 1
	else
		shakeSetTrauma = num / 100
	end
end

local shakeAddPeriod = 50

function GamePlayerControlAddShakePeriod( num )
	num = num or 50
	if num == 0 then 
		num = 50 
	end
	shakeAddPeriod = num
end

local shakeSetPeriod = 50

function GamePlayerControlSetShakePeriod( num )
	num = num or 50
	if num == 0 then 
		num = 50 
	end
	shakeSetPeriod = num
end

local traumaFade = 0.02
local lastFade   = nil

function GamePlayerControlAddShakeFade( num, temp )
	num = num or 2
	if num > 10 then num = 10 end
	if temp then
		lastFade = lastFade or traumaFade
	else
		lastFade = nil
	end
	traumaFade = num / 100
end

local lastShakeTime = 0
local addSeed1, addSeed2, addSeed3 = random() * 1000, random() * 1000, random() * 1000
local setSeed1, setSeed2, setSeed3 = random() * 1000, random() * 1000, random() * 1000
local maxPitch = 15  -- Maximum allowed camera angle shake in degrees
local maxYaw   = 15  -- Maximum allowed camera angle shake in degrees
local maxRoll  = 10  -- Maximum allowed camera angle shake in degrees

local lastTime    = nil
local timeDiff    = 0

local function AddShake( timeNow )
	local shakex, shakey, shakez = 0, 0, 0	
	if shakeAddTrauma > 0 or shakeSetTrauma > 0 then   
	    -- periodically calculate new seeds to keep the randomness
		if timeNow - lastShakeTime > 5000 then
			addSeed1 = random() * 1000
			addSeed2 = random() * 1000
			addSeed3 = random() * 1000
			setSeed1 = random() * 1000
			setSeed2 = random() * 1000
			setSeed3 = random() * 1000
		end
		if shakeSetTrauma > 0 then
			-- Set trauma doesn't fade
			local shake = shakeSetTrauma^2
			local t = timeNow / shakeSetPeriod
			shakex = maxPitch * shake * N.PNoise( t, setSeed1 )
			shakey = maxYaw   * shake * N.PNoise( t, setSeed2 )
			shakez = maxRoll  * shake * N.PNoise( t, setSeed3 )
		end	
		if shakeAddTrauma > 0 then
			-- Add trauma fade over time
			shakeAddTrauma = max( shakeAddTrauma - traumaFade * timeDiff, 0 )
		
			local shake = shakeAddTrauma^2
			local t = timeNow / shakeAddPeriod
			shakex = shakex + maxPitch * shake * N.PNoise( t, addSeed1 )
			shakey = shakey + maxYaw   * shake * N.PNoise( t, addSeed2 )
			shakez = shakez + maxRoll  * shake * N.PNoise( t, addSeed3 )
		else 
			if lastFade ~= nil then traumaFade = lastFade end
		end 
		lastShakeTime = timeNow
	end
	SetGamePlayerStateShakeX( shakex )
	SetGamePlayerStateShakeY( shakey )
	SetGamePlayerStateShakeZ( shakez )
end

-- plays only while the head is under, and stops wherever the player stops being under
function gameplayercontrol.underwatersound(on)
	local underwaterSound = GetGamePlayerControlSoundStartIndex()+34
	if (RawSoundExist(underwaterSound) == 1) then
		if (on == 1) then
			if (RawSoundPlaying(underwaterSound) == 0) then
				LoopRawSound(underwaterSound)
			end
			g_underwatersoundon = 1
		elseif (g_underwatersoundon == 1) then
			StopRawSound(underwaterSound)
			g_underwatersoundon = 0
		end
	end
end

-- the ambient track and exit sound come back on any way out of the water
function gameplayercontrol.leftunderwater()
	if ( g_prevunderwater == 1 ) then
		StartAmbientMusicTrack()
		local exitSound = GetGamePlayerControlSoundStartIndex()+14
		if ( RawSoundExist ( exitSound ) == 1 ) then
			if ( RawSoundPlaying ( exitSound ) == 0 ) then
				PlayRawSound ( exitSound )
			end
		end
		g_prevunderwater = 0
	end
end

function gameplayercontrol.control()

	local timeNow = g_Time
	if lastTime == nil then 
		lastTime = timeNow
		timeDiff = 1
	else
		timeDiff = ( timeNow - lastTime ) / 16
		-- this represents a minimum frame rate allowance of 6 FPS, if we get that low
		-- then all bets are off!
		if timeDiff > 10 then timeDiff = 1 end
		lastTime = timeNow
	end
	
   -- if movement state changed, reset footfall to get first foot in frame
   if ( GetGamePlayerControlThirdpersonEnabled() == 1 ) then 
      if ( GetGamePlayerControlLastMovement() ~= GetGamePlayerControlMovement() ) then 
         SetGamePlayerControlFootfallCount(0)
      end
   end
   SetGamePlayerControlLastMovement(GetGamePlayerControlMovement())

   --  Control speed and walk wobble
   if ( GetGamePlayerControlMovement()>0 ) then 
      ttokay=0
      if ( GetGamePlayerControlGravityActive() == 1 ) then 
         if ( GetGamePlayerControlPlrHitFloorMaterial() ~= 0 ) then ttokay = 1 end
         if ( GetGamePlayerControlUnderwater() == 1 and GetGamePlayerStateNoWater() == 0  ) then ttokay = 1 end
         if ( GetGamePlayerControlJumpMode() == 1 and GetGamePlayerControlJumpModeCanAffectVelocityCountdown()>0 ) then ttokay = 1 end
      else
         ttokay=1
      end
      if ( ttokay == 1 ) then 
         -- increase plr speed using accel ratio
         SetGamePlayerControlSpeed(GetGamePlayerControlSpeed()+(GetGamePlayerControlAccel()*GetTimeElapsed()*0.05))
         if ( GetGamePlayerControlSpeed()>GetGamePlayerControlMaxspeed()*GetGamePlayerControlSpeedRatio() ) then 
            SetGamePlayerControlSpeed(GetGamePlayerControlMaxspeed()*GetGamePlayerControlSpeedRatio())
         end
      else
         -- with no Floor (  under player, reduce any speed for downhill and fall behavior ) then
         ttdeductspeed=(GetGamePlayerControlMaxspeed()*GetGamePlayerControlSpeedRatio())*GetTimeElapsed()*0.01
         SetGamePlayerControlSpeed(GetGamePlayerControlSpeed()-ttdeductspeed)
         if ( GetGamePlayerControlSpeed()<0.0 ) then SetGamePlayerControlSpeed(0.0) end
      end
      if ( GetGamePlayerControlGravityActive() == 1 and GetGamePlayerControlJumpMode() ~= 1 ) then 
         -- on ground
         ttWeaponMoveSpeedMod = 1.0
         if ( GetGamePlayerStateGunID() > 0 ) then
          ttWeaponMoveSpeedMod = GetFireModeSettingsPlrMoveSpeedMod()
          if ttWeaponMoveSpeedMod < 0.4 then ttWeaponMoveSpeedMod = 0.4 end
         end
         SetGamePlayerControlWobble(WrapValue(GetGamePlayerControlWobble()+(GetGamePlayerControlWobbleSpeed()*GetElapsedTime()*GetGamePlayerControlBasespeed()*GetGamePlayerControlSpeedRatio()*ttWeaponMoveSpeedMod)))
      else
         -- in air
         SetGamePlayerControlWobble(CurveValue(0,GetGamePlayerControlWobble(),3.0))
      end
   else
      ttdeductspeed=(GetGamePlayerControlMaxspeed()*GetGamePlayerControlSpeedRatio())*GetTimeElapsed()*0.5
      SetGamePlayerControlSpeed(GetGamePlayerControlSpeed()-ttdeductspeed)
      if ( GetGamePlayerControlSpeed()<0.0 ) then SetGamePlayerControlSpeed(0.0) end
      ttamounttostep = 25.0*GetTimeElapsed()
      if ( GetGamePlayerControlWobble() < 180 ) then
         ttnewtotal = GetGamePlayerControlWobble() - ttamounttostep
         if ttnewtotal < 0 then ttnewtotal = 0 end
         SetGamePlayerControlWobble(ttnewtotal)
      else
         ttnewtotal = GetGamePlayerControlWobble() + ttamounttostep
         if ttnewtotal >= 360 then ttnewtotal = 0 end
         SetGamePlayerControlWobble(ttnewtotal)
      end
   end

   -- Handle player jumping
   local tcausetojump = 0
   if g_PlrKeySPACE == 1 then tcausetojump = 1 end
   if g_gameplayercontrol_powerjump > 0 then tcausetojump = 1 end
   tplayerjumpnow=0.0
   if ( GetGamePlayerControlGravityActive() == 1 and g_suspendplayercontrols == 0 ) then 
      ttokay=0
      if ( GetGamePlayerControlThirdpersonEnabled() == 1 ) then 
         ttcharacterindex=GetGamePlayerControlThirdpersonCharacterIndex()
         entid = GetEntityElementBankIndex(GetCharAnimStateOriginalE(ttcharacterindex))
         if ( tcausetojump == 1 and GetGamePlayerControlJumpMode() == 0 and GetEntityProfileJumpModifier(entid) > 0.0 ) then 
            ttplaycsi=GetCharAnimStatePlayCsi(ttcharacterindex)
            if ( ttplaycsi >= GetCsiStoodVault(1) and ttplaycsi <= GetCsiStoodVault(1)+2 ) then 
               --  still in jump animation action
            else
               ttokay=1
            end
         end
         ttjumpmodifier = GetEntityProfileJumpModifier(entid)
      else
         if ( tcausetojump == 1 and GetGamePlayerControlJumpMode() == 0  ) then ttokay = 1 end
         ttjumpmodifier = 1.0
      end
      if ( ttokay == 1 ) then 
         -- player can only jump if a certain height above the waterline (i.e wading in shallows, not swimming) then
         if ( GetGamePlayerStateNoWater() ~= 0 or GetCameraPositionY(0) > GetGamePlayerStateWaterlineY() + 20 ) then 
            tplayerjumpnow=GetGamePlayerControlJumpmax()*ttjumpmodifier
			if g_gameplayercontrol_powerjump > 0 then
			 tplayerjumpnow = tplayerjumpnow * (g_gameplayercontrol_powerjump/100.0)
			 g_gameplayercontrol_powerjump = 0
			end
            if ( GetGamePlayerStateGunID()>0 ) then tplayerjumpnow = tplayerjumpnow*GetFireModeSettingsPlrJumpSpeedMod() end
			
			-- detect extra user defined global for MYSTAMINAMAXIMUM (drain)
			gameplayerspeed = require "scriptbank\\gameplayerspeed"
			gameplayerspeed.drain(tplayerjumpnow/10)
            
			SetGamePlayerControlJumpMode(1)
            if ( GetGamePlayerControlThirdpersonEnabled() == 1 ) then 
               entid = GetEntityElementBankIndex(GetGamePlayerControlThirdpersonCharactere())
               tq = GetEntityProfileStartOfAIAnim(entid) + GetCsiStoodVault(1)
               if ( GetEntityAnimStart(entid,tq) > 0 ) then
                  -- only if have VAULT animation, so we use the jump animation at all
                  SetCharAnimControlsLeaping(ttcharacterindex,1)
               end
               SetGamePlayerControlJumpModeCanAffectVelocityCountdown(7.0)
            else
               if ( GetGamePlayerControlMovement() == 0 ) then 
                  --  only if static can have the 'burst forward' feature to get onto crates
                  SetGamePlayerControlJumpModeCanAffectVelocityCountdown(7.0)
               else
                  --  otherwise we can make HUGE running jumps with this
                  SetGamePlayerControlJumpModeCanAffectVelocityCountdown(0)
               end
            end
         end
      end
   end
   if ( GetGamePlayerControlJumpModeCanAffectVelocityCountdown()>0 ) then 
      SetGamePlayerControlJumpModeCanAffectVelocityCountdown(GetGamePlayerControlJumpModeCanAffectVelocityCountdown()-GetTimeElapsed())
      if ( GetGamePlayerControlJumpModeCanAffectVelocityCountdown()<0 ) then 
         SetGamePlayerControlJumpModeCanAffectVelocityCountdown(0)
         if ( GetGamePlayerControlThirdpersonEnabled() == 1 ) then 
            ttcharacterindex=GetGamePlayerControlThirdpersonCharacterIndex()
            SetCharAnimControlsLeaping(ttcharacterindex,0)
         end
      end
   end
   if ( GetGamePlayerControlJumpMode() == 1 ) then 
      if ( GetGamePlayerControlJumpModeCanAffectVelocityCountdown() == 0 ) then 
         if ( GetPlrObjectAngleX() == 1.0 ) then 
            SetGamePlayerControlJumpMode(2)
            SetGamePlayerControlJumpModeCanAffectVelocityCountdown(0)
         end
      end
   end
   if ( GetGamePlayerControlJumpMode() == 2 ) then 
      if ( tcausetojump == 0 ) then SetGamePlayerControlJumpMode(0) end
   end

   --  Handle player flash light control
   if ( g_PlrKeyF == 1 and GetGamePlayerStateFlashlightKeyEnabled() == 1 ) then
      if ( GetGamePlayerStateFlashlightControl() == 0.0 ) then SetGamePlayerStateFlashlightControl(0.9) end
      if ( GetGamePlayerStateFlashlightControl() == 1.0 ) then SetGamePlayerStateFlashlightControl(0.1) end
   else
      if ( GetGamePlayerStateFlashlightControl() >= 0.89 ) then SetGamePlayerStateFlashlightControl(1.0) end
      if ( GetGamePlayerStateFlashlightControl() <= 0.11 ) then SetGamePlayerStateFlashlightControl(0.0) end
   end

   -- Pass movement state to player array
   SetGamePlayerStateMoving(0)
   if ( GetGamePlayerControlMovement() == 1 ) then 
      SetGamePlayerStateMoving(1)
      if ( GetGamePlayerStatePlayerDucking() ~= 0 ) then 
         SetGamePlayerStateMoving(2)
      end
   end

   -- Prevent player leaving terrain area
   local allowplayertoleaveterraineditablearea = 0
   if allowplayertoleaveterraineditablearea == 0 then
    local mapsizeminx = GetTerrainEditableArea(0) + 50
    local mapsizeminz = GetTerrainEditableArea(1) + 50
    local mapsizemaxx = GetTerrainEditableArea(2) - 50
    local mapsizemaxz = GetTerrainEditableArea(3) - 50
    local leavingmap = 0
    if ( GetPlrObjectPositionX() < mapsizeminx ) then leavingmap=1 SetGamePlayerControlPushangle(90.0) SetGamePlayerControlPushforce(1.0) end
    if ( GetPlrObjectPositionX() > mapsizemaxx ) then leavingmap=1 SetGamePlayerControlPushangle(270.0) SetGamePlayerControlPushforce(1.0) end
    if ( GetPlrObjectPositionZ() < mapsizeminz ) then leavingmap=1 SetGamePlayerControlPushangle(0.0) SetGamePlayerControlPushforce(1.0) end
    if ( GetPlrObjectPositionZ() > mapsizemaxz ) then leavingmap=1 SetGamePlayerControlPushangle(180.0) SetGamePlayerControlPushforce(1.0) end
    if leavingmap == 1 then
     Prompt ( "You cannot leave the area of play" )
    end
   end
   
   -- Reduce any player push force over time
   local pushforcestrength = 10 
   local finalpushforce = GetGamePlayerControlPushforce() * pushforcestrength
   if ( GetGamePlayerControlPushforce()>0 ) then 
      --SetGamePlayerControlPushforce(GetGamePlayerControlPushforce()-(GetTimeElapsed()/2.0))
	  --new system is much smoother at fading out effect of force applied to player
	  local amounttoreduce = GetGamePlayerControlPushforce()
	  amounttoreduce = amounttoreduce * (GetTimeElapsed())
      SetGamePlayerControlPushforce(GetGamePlayerControlPushforce()-amounttoreduce)
      if ( GetGamePlayerControlPushforce()<0.01 ) then 
         SetGamePlayerControlPushforce(0.0)
      end
   end

   -- Get current player ground height stood on (MAX for now)
   SetGamePlayerStateTerrainHeight(0.0)

   -- Control player using Physics character controller
   ttgotonewheight=0
   SetGamePlayerStateJetpackVerticalMove(0.0)
   if ( GetGamePlayerControlGravityActive() == 0 ) then 
      -- in air
      if ( GetGamePlayerControlLockAtHeight() == 1 ) then 
         SetGamePlayerStateJetpackVerticalMove(0)
      else
         SetGamePlayerStateJetpackVerticalMove(WrapValue(GetCameraAngleX(0)))
         if ( g_PlrKeyS == 1 ) then SetGamePlayerStateJetpackVerticalMove(GetGamePlayerStateJetpackVerticalMove()-360.0) end
         if ( g_PlrKeyA == 1 ) then SetGamePlayerStateJetpackVerticalMove(0) end
         if ( g_PlrKeyD == 1 ) then SetGamePlayerStateJetpackVerticalMove(0) end
      end
      -- extra height control with mouse wheel
      if ( GetGamePlayerControlControlHeight()>0 or GetGamePlayerControlControlHeightCooldown()>0 ) then 
         SetGamePlayerControlStoreMovey(GetGamePlayerControlMovey())
         if ( GetGamePlayerControlControlHeight() == 3 ) then 
            if ( GetGamePlayerStateTerrainID()>0 ) then 
               ttgotonewheight=1000.0+GetGamePlayerStateTerrainHeight()
               SetGamePlayerControlControlHeight(0)
            end
         else
            if ( GetGamePlayerControlControlHeightCooldown() > 0.0 ) then
               if ( g_playercontrolcooldownmode == 1 ) then 
                  SetGamePlayerControlMovey(GetGamePlayerControlMovey()+180)
                  SetGamePlayerStateJetpackVerticalMove(-270)
               else
                  SetGamePlayerStateJetpackVerticalMove(90)
               end
            else
               if ( GetGamePlayerControlControlHeight() == 1 ) then 
                  -- move up
                  SetGamePlayerControlMovey(GetGamePlayerControlMovey()+180)
                  SetGamePlayerStateJetpackVerticalMove(-270)
               else
                  -- move down
                  SetGamePlayerStateJetpackVerticalMove(90)
               end
               g_playercontrolcooldownmode = GetGamePlayerControlControlHeight()
            end
         end
         if ( GetGamePlayerControlControlHeightCooldown() > 0 ) then
            SetGamePlayerControlControlHeightCooldown(GetGamePlayerControlControlHeightCooldown() - 1.0)
            SetGamePlayerControlSpeed(0)
         else
            SetGamePlayerControlSpeed(GetGamePlayerControlMaxspeed()*GetGamePlayerControlSpeedRatio())
         end
      end
   else
      -- regular gravity
      if ( GetGamePlayerControlJumpMode() == 0 ) then 
         SetGamePlayerStateJetpackVerticalMove(0.0) -- see below
      end
      -- add any JETPACK device effect
      if ( GetGamePlayerControlJetpackThrust()>0 ) then 
         SetGamePlayerControlWobble(0.0)
         ttgeardirection=WrapValue(GetCameraAngleX(0))
         if ( ttgeardirection>180 ) then ttgeardirection = ttgeardirection-360.0 end
         ttgeardirection=(ttgeardirection/90.0)*2.0
         if ( g_PlrKeyW == 1 ) then ttgeardirection = ttgeardirection+1.0 end
         if ( g_PlrKeyS == 1 ) then ttgeardirection = ttgeardirection-1.0 end
         ttnewspeedneed=GetGamePlayerControlMaxspeed()*ttgeardirection*GetGamePlayerControlJetpackThrust()
         if ( GetGamePlayerControlSpeed()<ttnewspeedneed ) then 
            SetGamePlayerControlSpeed(ttnewspeedneed)
         end
         SetGamePlayerControlMovey(CurveAngle(GetCameraAngleY(0),GetGamePlayerControlMovey(),5.0))
      end
   end
   
   -- handle slope climb prevention
   if ( ttgotonewheight == 0 ) then 
      -- Player direction and speed
      ttfinalplrmovey=GetGamePlayerControlMovey()
      ttfinalplrspeed=GetGamePlayerControlSpeed()*2
      -- If not in air due to jetpack thurst
      if ( GetGamePlayerControlJetpackThrust() == 0 ) then 
         -- Player speed affected by gun speed modifier and zoom mode modifier
         if ( GetGamePlayerStateGunID()>0 ) then 
            if ( GetGamePlayerStateGameRunAsMultiplayer() == 0 and GetGamePlayerStateEnablePlrSpeedMods() == 1 ) then 
               -- only for single player action - MP play is too muddy!
               if ( GetFireModeSettingsIsEmpty() == 1 ) then 
                  ttfinalplrspeed=ttfinalplrspeed*GetFireModeSettingsPlrEmptySpeedMod()
               else
                  ttfinalplrspeed=ttfinalplrspeed*GetFireModeSettingsPlrMoveSpeedMod()
               end
               if ( GetGamePlayerStatePlrZoomIn()>1 ) then 
                  ttfinalplrspeed=ttfinalplrspeed*GetFireModeSettingsZoomWalkSpeed()
               end
               if ( (GetGamePlayerStateGunMode() >= 121 and GetGamePlayerStateGunMode() <= 126) or (GetGamePlayerStateGunMode()>= 700 and GetGamePlayerStateGunMode() <= 707) or (GetGamePlayerStateGunMode() >= 7000 and GetGamePlayerStateGunMode()<= 7099) ) then 
                  ttfinalplrspeed=ttfinalplrspeed*GetFireModeSettingsPlrReloadSpeedMod()
               end
            end
         end
         -- Determine slope angle of plr direction
         SetPlayerSlopeAngle()
      end                            

      -- Move player using Bullet character controller
      if ( GetGamePlayerStateRiftMode()>0 ) then ttfinalplrmovey = ttfinalplrmovey + 0 end
      ControlDynamicCharacterController ( ttfinalplrmovey,GetGamePlayerStateJetpackVerticalMove(),ttfinalplrspeed,tplayerjumpnow,GetGamePlayerStatePlayerDucking(),GetGamePlayerControlPushangle(),finalpushforce,GetGamePlayerControlJetpackThrust() )
      if GetDynamicCharacterControllerDidJump() == 1 then
         ttsnd = GetGamePlayerControlSoundStartIndex()+6
         if ( RawSoundExist(ttsnd) == 1 ) then
            PlayRawSound(ttsnd)
         end
      end
      if ( GetGamePlayerControlGravityActive() == 0 ) then 
         if ( GetGamePlayerControlControlHeight()>0 ) then 
            SetGamePlayerControlMovey(GetGamePlayerControlStoreMovey())
         end
      end

      -- If player under waterline, set playercontrol.underwater flag
      SetGamePlayerStatePlayerY(GetPlrObjectPositionY())
      if ( GetGamePlayerStatePlayerDucking() ~= 0 ) then 
         ttsubtleeyeadjustment=10.0
      else
         ttsubtleeyeadjustment=30.0
      end
      if ( (GetGamePlayerStatePlayerY()+ttsubtleeyeadjustment)<GetGamePlayerStateWaterlineY() and GetGamePlayerStateNoWater() == 0 ) then 
         SetGamePlayerControlUnderwater(1)
         if ( g_PlayerUnderwaterMode == 1 ) then
            ChangePlayerWeaponID(0)
         end
      else
         SetGamePlayerControlUnderwater(0)
      end

      -- detect a floor under player
      ttplrhitfloormaterial=GetCharacterHitFloor()
	  
      -- Act on any responses from character controller
      if ( ttplrhitfloormaterial>0 ) then 
         SetGamePlayerControlPlrHitFloorMaterial(ttplrhitfloormaterial)
         ttplrfell=GetCharacterFallDistance()
         if ( GetGamePlayerStateImmunity() == 0 ) then 
            if ( ttplrfell>0 and (g_PlayerUnderwaterMode == 0 or GetGamePlayerStatePlayerY()>GetGamePlayerStateWaterlineY()) ) then 
               -- for a small landing, make a sound
               if ( ttplrfell>75 ) then 
                  ttsnd=GetGamePlayerControlSoundStartIndex()+5
                  if ( RawSoundExist(ttsnd)==1 ) then
                     PlayRawSound(ttsnd)
                  end
               end
                 -- when player lands on Floor, it returns how much 'air' the player traversed
               if ( GetGamePlayerControlHurtFall()>0 ) then 
                  if ( ttplrfell>GetGamePlayerControlHurtFall() ) then 
                     ttdamage=(ttplrfell-GetGamePlayerControlHurtFall())*GetGamePlayerControlFallDamageModifier()  
                     HurtPlayer(-1,ttdamage)
                  end
               end
            end
         end
      else
         SetGamePlayerControlPlrHitFloorMaterial(0)
      end
      
   else
      -- shift player to new height position ('M' key) then
      SetFreezePosition(GetPlrObjectPositionX(),ttgotonewheight,GetPlrObjectPositionZ())
      SetFreezeAngle(0,GetCameraAngleY(0),0)
      TransportToFreezePosition()
      if ( GetCameraOverride() ~= 2 and GetCameraOverride() ~= 3 ) then
         SetCameraAngle ( 0, 90, GetCameraAngleY(0), GetCameraAngleZ(0) )
      end
   end

   -- Place camera at player object position (stood or ducking) then
   if ( GetGamePlayerStatePlayerDucking() ~= 0 ) then 
      ttsubtleeyeadjustment=10.0
   else
      ttsubtleeyeadjustment=30.0
   end
   SetGamePlayerStateAdjustBasedOnWobbleY((math.cos(math.rad(GetGamePlayerControlWobble()))*GetGamePlayerControlWobbleHeight()))
   if ( GetGamePlayerStateSlowMotion() == 1 and GetGamePlayerStateSmoothCameraKeys() == 1 ) then 
      SetGamePlayerStateFinalCamX(CurveValue(GetPlrObjectPositionX(),GetGamePlayerStateFinalCamX(),10.0))
      SetGamePlayerStateFinalCamY(CurveValue(ttsubtleeyeadjustment+GetPlrObjectPositionY()-GetGamePlayerStateAdjustBasedOnWobbleY(),GetGamePlayerStateFinalCamY(),10.0))
      SetGamePlayerStateFinalCamZ(CurveValue(GetPlrObjectPositionZ(),GetGamePlayerStateFinalCamZ(),10.0))
   else
      ttleanangle=GetCameraAngleY(0)+GetGamePlayerControlLeanoverAngle()
      ttleanoverx=NewXValue(0,ttleanangle,GetGamePlayerControlLeanover())
      ttleanoverz=NewZValue(0,ttleanangle,GetGamePlayerControlLeanover())
      SetGamePlayerStateFinalCamX(GetPlrObjectPositionX()+ttleanoverx)
      SetGamePlayerStateFinalCamY(ttsubtleeyeadjustment+GetPlrObjectPositionY()-GetGamePlayerStateAdjustBasedOnWobbleY())
      SetGamePlayerStateFinalCamZ(GetPlrObjectPositionZ()+ttleanoverz)
   end
   
   -- record present camera angles
   SetGamePlayerControlFinalCameraAnglex(GetCameraAngleX(0))
   SetGamePlayerControlFinalCameraAngley(GetCameraAngleY(0))
   SetGamePlayerControlFinalCameraAnglez(GetCameraAngleZ(0))
   
   -- New: apply a shake angle to camera rotation
   if GetGamePlayerControlCameraShake() > 0 then
    GamePlayerControlAddShakeTrauma( GetGamePlayerControlCameraShake() )
    SetGamePlayerControlCameraShake( 0 )
   end
   AddShake( timeNow )
	
   if ( GetGamePlayerControlThirdpersonEnabled() == 1 ) then 
      gameplayercontrol.thirdperson()
   else
      -- update camera position
      if ( GetCameraOverride() ~= 1 and GetCameraOverride() ~= 3 ) then
         PositionCamera ( 0, GetGamePlayerStateFinalCamX(),GetGamePlayerStateFinalCamY(),GetGamePlayerStateFinalCamZ() )
      end
      -- Apply flinch (for when get hit)
      if ( GetCameraOverride() ~= 2 and GetCameraOverride() ~= 3 ) then
         SetGamePlayerControlFlinchx(CurveValue(0,GetGamePlayerControlFlinchx(),20))
         SetGamePlayerControlFlinchy(CurveValue(0,GetGamePlayerControlFlinchy(),20))
         SetGamePlayerControlFlinchz(CurveValue(0,GetGamePlayerControlFlinchz(),20))
         SetGamePlayerControlFlinchCurrentx(CurveValue(GetGamePlayerControlFlinchx(),GetGamePlayerControlFlinchCurrentx(),10.0))
         SetGamePlayerControlFlinchCurrenty(CurveValue(GetGamePlayerControlFlinchy(),GetGamePlayerControlFlinchCurrenty(),10.0))
         SetGamePlayerControlFlinchCurrentz(CurveValue(GetGamePlayerControlFlinchz(),GetGamePlayerControlFlinchCurrentz(),10.0))
         SetCameraAngle ( 0, GetGamePlayerControlFinalCameraAnglex() + GetGamePlayerStateShakeX() + GetGamePlayerControlFlinchCurrentx(),
		                     GetGamePlayerControlFinalCameraAngley() + GetGamePlayerStateShakeY() + GetGamePlayerControlFlinchCurrenty(),
							 GetGamePlayerControlFinalCameraAnglez() + GetGamePlayerStateShakeZ() + GetGamePlayerControlFlinchCurrentz() + (GetGamePlayerControlLeanover()/-10.0) )
	  end
   end

   -- player footfall handling
   gameplayercontrol.footfall()

   -- drowning
   if ( g_PlayerHealth == 0 or GetGamePlayerStateEditModeActive() ~= 0 or GetGamePlayerStateNoWater() == 1 ) then 
      ResetUnderwaterState()
      gameplayercontrol.underwatersound(0)
      ---return
   end
	
  -- Water states 0 out of water, 1 in water, on surface, 2 under water, 3 under water, drowning, 4 dead underwater
	if ( GetGamePlayerStateNoWater() == 0 and GetWaterEnabled() == 1) then 
		local swimStrokeLength = 1000
		local swimForce = 0
		local camX = GetCameraPositionX(0)
		local camY = GetCameraPositionY(0)
		local camZ = GetCameraPositionZ(0)
		-- Swim direction based on camera angles.
		local angX = GetCameraAngleX(0)
		local angY = GetCameraAngleY(0)
		local angZ = GetCameraAngleZ(0)
		local localDirectionX = 0
		local localDirectionY = 0
		local localDirectionZ = 1
				
		limitVerticalMovement = 0		
	
		-- If player is already swimming, prevent them from reaching a height where gravity pushes them down, provided there is no land or static objects beneath them.
		if(GetGamePlayerControlInWaterState() >= 2) then
			g_gravitymode = 1
			g_gravitymoderefresh = 1
			SetGamePlayerStatePlayerDucking(2)
			if(camY > GetGamePlayerStateWaterlineY() + 19) then
				if(IntersectStatic(camX, camY, camZ, camX, camY - 100, camZ, 1000) == 0) then
					limitVerticalMovement = 1
				end
			else
				-- Check for shallow enough water to stop swimming.
				if(GetGamePlayerStateWaterlineY() - GetGroundHeight(camX,camZ) < 47) then
					SetGamePlayerControlInWaterState(1)
					SetGamePlayerControlDrownTimestamp(0)
					SetUnderwaterOff()
					g_gravitymode = 0
					g_gravitymoderefresh = 1
				end
			end	
		end

		-- Alter speed and direction of swimming based on input.
		local speed = GetGamePlayerControlSwimSpeed()
		if(g_PlrKeyW == 1) then
		 swimForce = speed
		elseif(g_PlrKeyS == 1) then
		 swimForce = speed * 0.66
		 localDirectionZ = -1
		end
  
		if(g_PlrKeyA == 1) then
		 swimForce = speed * 0.66
		 localDirectionX = -1
		elseif(g_PlrKeyD == 1) then
		 swimForce = speed * 0.66
		 localDirectionX = 1
		end
	 
		-- check for in water
		if ( camY <= GetGamePlayerStateWaterlineY() + 20 ) then 
			-- just entered water
			if ( GetGamePlayerControlInWaterState() == 0 ) then 
				ttsnd = GetGamePlayerControlSoundStartIndex()+13
				if ( RawSoundExist ( ttsnd ) == 1 ) then
					if ( g_PlayerHealth > 0 ) then
						PlayRawSound ( ttsnd )
					end
				end
				SetGamePlayerControlInWaterState(1)
			end
		 
			-- Timer that goes from 0 to 1
			local timerSpeed = 0.04
			if(GetGamePlayerStatePlrKeyShift() == 1) then 
				timerSpeed = 0.055
			end
			g_swimimpulsetimer = g_swimimpulsetimer + GetTimeElapsed() * timerSpeed
			if(g_swimimpulsetimer >= 1) then
				g_swimimpulsetimer = 0
			end
			
			if(swimForce == 0 ) then
				g_swimimpulsetimer = -0.001
			end
				
			
			local waterThreshold = GetGamePlayerStateWaterlineY()
			if(GetGamePlayerControlInWaterState() >= 2) then
				waterThreshold = waterThreshold + 20
			end
		 
			-- going under water
			if ( camY <= waterThreshold) then 
				-- head goes under water
				if ( GetGamePlayerControlInWaterState() < 2 ) then 
					SetGamePlayerControlInWaterState(2)
					SetUnderwaterOn()
					g_gravitymode = 1
					g_gravitymoderefresh = 1
					if( g_PlayerUnderwaterMode == 1 ) then
						-- added delay before drowning damage starts
						SetGamePlayerControlDrownTimestamp(Timer() + 8000)
					end
					-- player weapon is unequipped when swimming, so we need to ensure that the weapon zoom is also stopped
					local zoomMode = GetGamePlayerStateGunZoomMode()
					if ( zoomMode == 9 or zoomMode == 10) then 
						SetGamePlayerStateGunZoomMode(11) 
					end
				end
				-- check for drowning
				if ( GetGamePlayerControlDrownTimestamp() == 0 ) then 
					if( g_PlayerUnderwaterMode == 1 ) then
						SetGamePlayerControlDrownTimestamp(Timer()+8000)
					else
						SetGamePlayerControlDrownTimestamp(Timer()+4000)
					end
				end
				if ( camY <= waterThreshold - 20) then
					if ( g_PlayerHealth > 0 ) then
						gameplayercontrol.underwatersound(1)
					end
					g_prevunderwater = 1
					StopAmbientMusicTrack()
					-- if g_PlayerUnderwaterMode == 1 lose 1 health per second until dead , else 50 health 
					if ( Timer() > GetGamePlayerControlDrownTimestamp() ) then 
						-- if there was no start marker, reset player (cannot kill, as no start marker) then. Indicated by crazy health and no lives
						if ( g_PlayerLives == 0 and g_PlayerHealth == 99999 ) then 
							-- start death sequence for player
							SetGamePlayerControlDeadTime(Timer()+2000)
							g_PlayerHealth=0
						else
							-- Gulp in water for plr damage
							if( g_PlayerUnderwaterMode == 1 ) then
								SetGamePlayerControlDrownTimestamp(Timer()+8000)
								DrownPlayer(-1,125)
							else
								SetGamePlayerControlDrownTimestamp(Timer()+4000)
								DrownPlayer(-1,550)
							end
						end
						-- if player died
						if ( g_PlayerHealth == 0 ) then 
							SetGamePlayerControlInWaterState(4)
						else
							SetGamePlayerControlInWaterState(3)
						end
						--Play drown sound
						ttsnd = GetGamePlayerControlSoundStartIndex()+11
						if ( RawSoundExist ( ttsnd ) == 1 ) then
							PlayRawSound ( ttsnd )
						end
					end
					
					-- Play underwater swimming sounds.
					if(g_swimimpulsetimer == 0) then
						local swimSound = GetGamePlayerControlSoundStartIndex()+39+math.random(0,3)
						if ( RawSoundExist ( swimSound ) == 1 ) then
							PlayRawSound ( swimSound )
						end
					end
				else 
					-- if we were drowning, gasp for air
					if ( GetGamePlayerControlInWaterState() == 3 ) then 
						ttsnd = GetGamePlayerControlSoundStartIndex()+12
						
						if ( RawSoundExist ( ttsnd ) == 1 ) then
							if(RawSoundPlaying(ttsnd) == 0) then
								PlayRawSound ( ttsnd )
							end
						end
					end
					-- Reset drown timer.
					SetGamePlayerControlDrownTimestamp(Timer()+8000)
					
					-- End underwater ambient sounds.
					gameplayercontrol.underwatersound(0)
					
					-- play exit underwater sound
					gameplayercontrol.leftunderwater()
					
					if(g_swimimpulsetimer == 0) then
						local swimSound = GetGamePlayerControlSoundStartIndex()+35+math.random(0,3)
						if ( RawSoundExist ( swimSound ) == 1 ) then
							PlayRawSound ( swimSound )
						end
					end
					
				end
				
				if(camY > GetGamePlayerStateWaterlineY() and swimForce > 0) then
					
				g_swimeffects = g_swimeffects + 1
				if(g_swimeffects == 3) then
					TriggerWaterRippleSize(GetPlrObjectPositionX(),GetGamePlayerStateWaterlineY()+1,GetPlrObjectPositionZ(), 40, 40)
					g_swimeffects = 0
				end
				end
				-- Alter the swim force to replicate swimming acceleration x^2(1-x)
				local multiplier = g_swimimpulsetimer * g_swimimpulsetimer * (1 - g_swimimpulsetimer)+0.12
				if(GetGamePlayerStatePlrKeyShift() == 1) then 
					multiplier = 0.2
				end
				
				-- Apply swimming movement to the player (overrides existing move direction - localDirection is transformed by angles to produce a new look direction)
				LimitSwimmingVerticalMovement(limitVerticalMovement)
				SetCharacterDirectionOverride(angX,angY,angZ, localDirectionX, localDirectionY, localDirectionZ, swimForce * multiplier)
				
			else
				-- head comes out of water
				gameplayercontrol.underwatersound(0)
				gameplayercontrol.leftunderwater()
				if ( GetGamePlayerControlInWaterState() > 1 ) then 
					SetGamePlayerControlInWaterState(1)
					SetGamePlayerControlDrownTimestamp(0)
					SetUnderwaterOff()
					g_gravitymode = 0
					g_gravitymoderefresh = 1
				end
			end
		else
			-- climb out of water
			gameplayercontrol.underwatersound(0)
			if ( GetGamePlayerControlInWaterState() > 0 ) then 
				ttsnd = GetGamePlayerControlSoundStartIndex()+14
				if ( RawSoundExist ( ttsnd ) == 1 ) then
					PlayRawSound ( ttsnd )
				end
				SetGamePlayerControlInWaterState(0)
				SetUnderwaterOff()
				g_gravitymode = 0
				g_gravitymoderefresh = 1
			end
			gameplayercontrol.leftunderwater()
		end
	else
      gameplayercontrol.underwatersound(0)
      gameplayercontrol.leftunderwater()
      SetGamePlayerControlInWaterState(0)
      SetUnderwaterOff()
	  g_gravitymode = 0
	  g_gravitymoderefresh = 1
	end
	
	if g_gravitymoderefresh == 1 then
		if g_gravitymode == 0 then 
			SetWorldGravity(0, -20, 0, 0)
		else
			SetWorldGravity(0, -0.5, 0, 150.0)
		end
		g_gravitymoderefresh = 0
	end
	if g_gravityhold > 0 then
		SetWorldGravity(0, 0, 0, 0)
		g_gravityhold = g_gravityhold - 1
		if g_gravityhold <= 0 then
			g_gravityhold = 0
			g_gravitymoderefresh = 1
		end
	end
	
end

function gameplayercontrol.debug()

 -- use to view PBR texture layers
 PBRDebugView = 0
 if g_InKey == "1" then PBRDebugView = 1 end
 if g_InKey == "2" then PBRDebugView = 2 end
 if g_InKey == "3" then PBRDebugView = 3 end
 if g_InKey == "4" then PBRDebugView = 4 end
 if g_InKey == "5" then PBRDebugView = 5 end
 if g_InKey == "6" then PBRDebugView = 6 end
 if g_InKey == "7" then PBRDebugView = 7 end 
 if g_InKey == "8" then PBRDebugView = 8 end 
 if g_InKey == "9" then PBRDebugView = 9 end 
 SetShaderVariable(0,"ShaderVariables",PBRDebugView,0,0,0)
 if PBRDebugView > 0 then
  if PBRDebugView == 1 then Prompt ( "Albedo" ) end
  if PBRDebugView == 2 then Prompt ( "Normal" ) end
  if PBRDebugView == 3 then Prompt ( "Metalness" ) end
  if PBRDebugView == 4 then Prompt ( "Gloss" ) end
  if PBRDebugView == 5 then Prompt ( "AO" ) end
  if PBRDebugView == 6 then Prompt ( "Ambience Contrib" ) end
  if PBRDebugView == 7 then Prompt ( "Light Contrib" ) end
  if PBRDebugView == 8 then Prompt ( "Env Contrib" ) end
  if PBRDebugView == 9 then Prompt ( "Shadows" ) end
 end
 
end

function gameplayercontrol.combatmusic()
 -- handle combat music track control (fades out after no combat)
 if g_CombatMusicMode == nil then
   g_CombatMusicVolume = 0.2
   g_CombatMusicMode = 2
   g_CombatMusicLatest = 0
 end
 if g_PlayerHealth <= 0 then 
  StopCombatMusicTrack()
  g_CombatMusicMode = 0 
 end
 if g_CombatMusicMode == 0 then
  if GetCombatMusicTrackPlaying() == 1 then
   -- detected combat music started
   StopAmbientMusicTrack()
   SetAmbientMusicTrackVolume(0)
   SetCombatMusicTrackVolume(100)
   g_CombatMusicAmbience = 0
   g_CombatMusicLatest = Timer()
   g_CombatMusicVolume = 100.0
   g_CombatMusicMode = 1
  end 
 else
  if g_CombatMusicMode == 1 then
   -- wait for combat to finish
   local delayBeforeFadeOut = g_CombatMusicModeDuration
   if delayBeforeFadeOut == nil then delayBeforeFadeOut = 3000 end
   if Timer() > g_CombatMusicLatest + delayBeforeFadeOut then
	 g_CombatMusicLatest = 0
     g_CombatMusicMode = 2
   end
  else
   if g_CombatMusicMode == 2 then
    -- fade out combat music
	g_CombatMusicVolume = g_CombatMusicVolume - 0.2
	SetCombatMusicTrackVolume(g_CombatMusicVolume)
	if g_CombatMusicVolume <= 50 then
	 -- and stop combat music
	 SetCombatMusicTrackVolume(0)
	 StartAmbientMusicTrack()
	 SetAmbientMusicTrackVolume(50)
	 g_CombatMusicAmbience = 50
	 g_CombatMusicMode = 3
    end   
   else
    if g_CombatMusicMode == 3 then
     -- fade in ambient music
	 g_CombatMusicAmbience = g_CombatMusicAmbience + 0.2
	 if g_CombatMusicAmbience >= 100 then
      StopCombatMusicTrack()
	  SetCombatMusicTrackVolume(100)
	  g_CombatMusicAmbience = 100
	  g_CombatMusicMode = 0
	 end
	 SetAmbientMusicTrackVolume(g_CombatMusicAmbience)
    end
   end
  end
 end
end						
				
return gameplayercontrol
