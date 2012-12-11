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

#ifndef _ELEVATOR_H
#define _ELEVATOR_H

#include "ElevatorSim.hpp"
#include "Location.hpp"
#include "Person.hpp"
#include "ISimulationTerminal.hpp"
#include "IPersonCarrier.hpp"
#include "cRenderObjs.hpp"
#include "Building.hpp"
#include "SimulationState.hpp"

#include <vector>
#include <set>
#include <utility>

namespace elevatorSim {

class Location;
class Building;
class Person;
class Floor;
struct SimulationTerminal;
class IPersonCarrier;

class Elevator :
         public Location, public ISimulationTerminal, public IPersonCarrier {

   /* friends */
   friend class Building;

   /* private static constants */

   /* private static methods */

   /* private const instance members */
   const int maxVel;
   const int maxAccel;
   const int maxOccupants;
   const int accelTimeInterval;
   const int stoppingDistance;

   /* private instance members */
   int currentVel;
   int currentAccel;
   int closeDoorTimer;

   bool peopleOffAnimation;
   bool peopleOnAnimation;

   int peopleOffCounter;
   int peopleOnCounter;
   float peopleOnOffPosX;

   float doorOpenAngle;

   std::vector<int> scheduledFloors;
   std::vector<std::pair<int, int>> scheduledAccels;

   /* private methods */

   /* constructor */
   Elevator(
            int _yVal,
            const int _maxVel = DEFAULT_MAX_VEL,
            const int _maxAccel = DEFAULT_MAX_ACCEL,
            const int _maxOccupants = DEFAULT_MAX_OCCUPANTS);

   void scheduleAccelsToFloor( const int srcFloor, const int destfloor );

public:

   /* public static constants */
   static const int DEFAULT_MAX_VEL;
   static const int DEFAULT_MAX_ACCEL;
   static const int DEFAULT_MAX_OCCUPANTS;
   static const int DEFAULT_MAX_ANI_COUNTER;
   static const int DEFAULT_DOOR_CLOSE_DELAY;

   /* public instance members */

   /* destructor */
   ~Elevator();

   /* public methods */
   bool canStopAtNextFloor();
   void goToFloor(int floor);
   int getCurrentFloor();

   bool isStopped() const {
      return (currentVel == 0);
   };

   inline bool isFull() const {
      return (maxOccupants <= numPeopleContained()?true:false);
   }

   /* fancy animation turn on function */
   inline void peopleGetOnAnimationOn() {
      peopleOnAnimation = true;
      peopleOnCounter = DEFAULT_MAX_ANI_COUNTER;
   }

   inline void peopleGetOffAnimationOn() {
      peopleOffAnimation = true;
      peopleOffCounter = 0;
   }

   inline void peopleGetOnOffAnimationOff() {
      peopleOnAnimation = false;
      peopleOffAnimation = false;
   }

   enum PERSON_CARRIER_TYPE getCarrierType() const {
      return IPersonCarrier::ELEVATOR_CARRIER; 
   }

   /* inherited from SimulationTerminal */
   void init();
   void render();
   void update();

   /* public methods inherited fomr IPersonCarrier */
   void updateTuple() {
      if(pythonRepr != NULL) {
         freeTuple();
      }

      pythonRepr = peopleToTuple();
   }

   void freeTuple() {
      Py_CLEAR(pythonRepr);
   }
};

} /* namespace elevatorSim */

#endif /* _ELEVATOR_H */
