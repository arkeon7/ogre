/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2013 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreRoot.h"

#include "OgreRPIEGLSupport.h"
#include "OgreRPIEGLWindow.h"
#include "OgreGLUtil.h"

#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#include "bcm_host.h"

namespace Ogre {
  GLNativeSupport* getGLSupport(int profile)
  {
    return new RPIEGLSupport(profile);
  }
    
  RPIEGLSupport::RPIEGLSupport(int profile) : EGLSupport(CONTEXT_ES) //profile ?
  {
    bcm_host_init();

    //get frame buffer size
    /*
    int fh;
    struct fb_var_screeninfo var;
    fh = open("/dev/fb0", O_RDONLY);
    ioctl(fh, FBIOGET_VSCREENINFO, &var);
    mCurrentMode.first.first = var.xres;
    mCurrentMode.first.second = var.yres;
    */

    mNativeDisplay = EGL_DEFAULT_DISPLAY;
    mGLDisplay = getGLDisplay();

    mCurrentMode.width = 1280;
    mCurrentMode.height = 800;
    mCurrentMode.refreshRate = 0;
    mOriginalMode = mCurrentMode;
    mVideoModes.push_back(mCurrentMode);
  }

  ConfigOptionMap RPIEGLSupport::getConfigOptions()
  {
      ConfigOptionMap mOptions = EGLSupport::getConfigOptions();
      ConfigOption optOrientation;
      optOrientation.name = "Orientation";
      optOrientation.immutable = false;
      optOrientation.possibleValues.push_back("Landscape");
      optOrientation.possibleValues.push_back("Portrait");
      optOrientation.currentValue = optOrientation.possibleValues[0];
      
      mOptions[optOrientation.name] = optOrientation;
      return mOptions;
  }
  
  RPIEGLSupport::~RPIEGLSupport()
  {
    bcm_host_deinit();
  }

  void RPIEGLSupport::switchMode(uint& width, uint& height, short& frequency)
  {
  }

  RenderWindow* RPIEGLSupport::newWindow(const String &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList *miscParams)
  {
    RPIEGLWindow* window = new RPIEGLWindow(this);
    window->create(name, width, height, fullScreen, miscParams);

    return window;
  }
}
