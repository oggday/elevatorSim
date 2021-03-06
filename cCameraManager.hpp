/*
 * Copyright (c) 2012, Joseph Max DeLiso
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the FreeBSD Project.
 */

#ifndef __CCAMERAMANAGER_H__
#define __CCAMERAMANAGER_H__

#include "ElevatorSim.hpp"
#include "cKeyManager.hpp"
#include "cTimeManager.hpp"
#include "MVectors.hpp"
#include "SimulationState.hpp"
#include "ISimulationTerminal.hpp"

namespace elevatorSim {

class cCameraManager : public ISimulationTerminal {
   friend class SimulationState;

   Vec3f m_vecCamPos;
   Vec3f m_vecCamLookAt;
   Vec3f m_vecCamUp;

   Vec3f m_vecZoomPos;
   Vec3f m_vecZoomLook;

   float m_fPitchAngle;
   float m_fYawAngle;
   float m_fRollAngle;

   bool m_bZoomInEnabled;

   cCameraManager();

public:

   void init();
   void update();
   void render();

   void updateTuple() {
      /* TODO */
   }

   void freeTuple() {
      /* TODO */
   }

   void yaw(float fAngle);
   void pitch(float fAngle);
   void roll(float fAngle);

   Vec3f getRight();

   inline Vec3f GetCameraPos() {
      return m_vecCamPos;
   }

   inline Vec3f GetCameraLookAt() {
      return m_vecCamLookAt;
   }

   inline Vec3f GetCameraUp() {
      return m_vecCamUp;
   }

   bool isZoomIn()   {  return m_bZoomInEnabled;   }
   void UpdateZoomVectors(Vec3f pos, Vec3f look);
   void ActivateZoomin();
   void DeactivateZoomin();
}; /* cCameraManager */

} /* namespace elevatorSim */

#endif /* __CCAMERAMANAGER_H__ */
