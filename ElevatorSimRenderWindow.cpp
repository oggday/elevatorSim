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

#include "ElevatorSim.hpp"
#include "SimulationState.hpp"
#include "ElevatorSimRenderWindow.hpp"
#include "cTimeManager.hpp"
#include "cKeyManager.hpp"
#include "Logger.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/names.h>
#include <GL/glut.h>

#include <ctime>
#include <sstream>

namespace elevatorSim {

const int ElevatorSimRenderWindow::LEFT_MARGIN = 120;
const int ElevatorSimRenderWindow::RIGHT_MARGIN = 8;
const int ElevatorSimRenderWindow::TOP_MARGIN = 28;
const int ElevatorSimRenderWindow::BOTTOM_MARGIN = 8;

const GLfloat ElevatorSimRenderWindow::light1_ambient[4] =
{ 0.4f, 0.4f, 0.4f, 1.0f };
const GLfloat ElevatorSimRenderWindow::light1_diffuse[4] =
{ 0.15f, 0.15f, 0.15f, 1.0f };
const GLfloat ElevatorSimRenderWindow::light1_specular[4] =
{ 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat ElevatorSimRenderWindow::light1_position[4] =
{ 1.f, 8.f, 2.0f, 0.0f };
const GLfloat ElevatorSimRenderWindow::light1_direction[4] =
{ 0.0f, 0.0f, 0.0f, 0.0f };

void ElevatorSimRenderWindow::timerCB(void* userdata) {
   ElevatorSimRenderWindow* myWindow = (ElevatorSimRenderWindow*)userdata;
   myWindow->redraw();
   Fl::repeat_timeout(cTimeManager::redrawInterval, timerCB, userdata);
}

int ElevatorSimRenderWindow::handle(int event) {
   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS << "RenderWin: event: " << fl_eventnames[event] << std::endl;
      LOG_INFO( Logger::SUB_FLTK, sstreamToBuffer( dbgSS ) );
   }

   if( event == FL_PUSH ) {
      rayCasting( Fl::event_x(), Fl::event_y());
      return true;
   } else {
      return Fl_Gl_Window::handle(event);
   }
}

void ElevatorSimRenderWindow::glInit() {
   /* if GlInit is called while valid() returns true, drop a breakpoint */
   assert(!valid());

   glEnable(GL_TEXTURE_2D);

   glShadeModel(GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glDepthMask(GL_TRUE);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);

   glLightfv(GL_LIGHT0, GL_AMBIENT, light1_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT0, GL_POSITION, light1_position);
   glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light1_direction);
}

void ElevatorSimRenderWindow::setViewport() {
   glViewport(0, 0, w(), h());

   float ratio = (float)w() / (float)h();

   setPerspective(45.0, 1.0*ratio, 1.0, 200.0);
}

void ElevatorSimRenderWindow::setPerspective(
         GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar) {
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan(fovy * MY_PI / 360.0);
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void ElevatorSimRenderWindow::drawFPS(int fps, int totalFrames) {
   std::stringstream renderStringStream(
            std::stringstream::in | std::stringstream::out);
   const int MAX_DIAG_STR_LEN = 256;
   char stringBuf[MAX_DIAG_STR_LEN];

   glColor3f(0.0f, 1.f, 0.f); /* because green text is sexy text */

   renderStringStream
   << "FPS: " << fps << std::endl;

   renderStringStream
   << "Total Frame: " << totalFrames   << std::endl;

   renderStringStream.getline(stringBuf, MAX_DIAG_STR_LEN);
   drawText(stringBuf, 10.f, 10.f);

   renderStringStream.getline(stringBuf, MAX_DIAG_STR_LEN);
   drawText(stringBuf, 10.f, 20.f);
}

void ElevatorSimRenderWindow::drawText(const char * const str,
         float x, float y) {
   /* enabling prolog */
   glDisable(GL_DEPTH_TEST);
   glPushAttrib(GL_LIGHTING_BIT);
   glDisable(GL_LIGHTING);

   /* projection push */
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();

   /* modelview push */
   glLoadIdentity();
   gluOrtho2D(0, w(), 0, h());
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();

   glLoadIdentity();
   glRasterPos2f(x, y);

   /* loop through the string and draw each character */
   for (const char *c = str; *c != '\0'; c++) {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
   }

   /* modelview pop */
   glPopMatrix();

   /* projection pop */
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();

   /* disabling prolog */
   glMatrixMode(GL_MODELVIEW);
   glPopAttrib();
   glEnable(GL_DEPTH_TEST);
}

ElevatorSimRenderWindow::ElevatorSimRenderWindow(
         int X, int Y, int W, int H, const char* Label) :
                              Fl_Gl_Window(X, Y, W, H, Label) {

   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS << "constructring sim render window @ " << this << std::endl;
      LOG_INFO( Logger::SUB_MEMORY, sstreamToBuffer(dbgSS) );
   }

   spin = 0.0;
   m_bRenderFPS = true;

   GLWindow_width = W;
   GLWindow_height = H;

   Fl::add_timeout(cTimeManager::redrawInterval, timerCB, (void*)this);
   take_focus();
}

ElevatorSimRenderWindow::~ElevatorSimRenderWindow() {
   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS << "destroying sim render window @ " << this << std::endl;
      LOG_INFO( Logger::SUB_MEMORY, sstreamToBuffer(dbgSS) );
   }
}

void ElevatorSimRenderWindow::draw() {
   SimulationState& simState = SimulationState::acquire();

   if(!valid()) {
      /* init, this code only gets executed the first time draw() is called */

      simState.lockBASM(); /* CRITICAL SECTION START */
      simState.initRenderObjs();
      simState.unlockBASM(); /* CRITICAL SECTION STOP */

      glInit();
      setViewport();
   }

   /* draw */
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   /* TODO: make these constants somewhere */
   gluPerspective(45.0f, (GLfloat)w()/(GLfloat)h(), 0.1f, 500.0f);

   simState.lockBASM(); /* CRITICAL SECTION START */
   simState.getCameraManager().render();

   glMatrixMode(GL_MODELVIEW);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   simState.getBuilding().render();

   int curFPS = simState.getTimeManager().getFPS();
   int curTotalFrames = simState.getTimeManager().getTotalFrames();

   SimulationState::StateKind currentSimState = simState.getStateUnsafe();
   simState.unlockBASM(); /* CRITICAL SECTION STOP */

   if(currentSimState == SimulationState::SIMULATION_RUNNING &&
            m_bRenderFPS) {
      drawFPS(curFPS, curTotalFrames);
   }

   GLenum err = glGetError();
   if ( err != GL_NO_ERROR ) {
      std::stringstream dbgSS;
      dbgSS << "GLGETERROR= " << (int) err << std::endl;
      LOG_ERROR( Logger::SUB_RENDER, sstreamToBuffer(dbgSS) );
   }
}

void ElevatorSimRenderWindow::rayCasting(int x, int y) {
   SimulationState& simState = SimulationState::acquire();

   float fovX = (GLWindow_width/GLWindow_height) * 45.f;

   float mx = (float)((x - GLWindow_width * 0.5) *
            (1.0 / GLWindow_width) * fovX * 0.5);
   float my = (float)((y - GLWindow_height * 0.5) *
            (1.0 / GLWindow_width) * fovX * 0.5);
   Vec3f dx = simState.getCameraManager().getRight() * mx;
   Vec3f dy = simState.getCameraManager().GetCameraUp() * my;

   Vec3f dir = simState.getCameraManager().GetCameraLookAt() + (dx + dy) * 2.0;
   dir.Normalize();

   const int eachFloorHeight = simState.getBuilding().gfxEachFloorHeight;
   std::vector<Elevator*> & elevators = simState.getBuilding().getElevators();

   std::for_each(
            elevators.begin(),
            elevators.end(),
            [this, &eachFloorHeight] ( const Elevator* thisElev ) {
      float pos = 1.0f + thisElev->getYVal() /
               Floor::YVALS_PER_FLOOR * eachFloorHeight;

      (void) pos;
   });
}

} /* namespace elevatorSim */
