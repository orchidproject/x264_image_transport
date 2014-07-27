x264_image_transport
====================

This software is derived from the ROS packages made available by IntRoLab, Universite De Sherbrooke, Quebec, Canada.

Specifically, it provides a catkinized image transport plugin for recent versions of ROS, using x264 encoding.

Installation
============
Under current releases of ubuntu, system installed ffmpeg libraries are too old to support this software. To get around this, do the following (tested under ROS Indigo and Ubuntu 14.04 Trusty):

1. Rename/remove any previously build ffmpeg libraries installed in your home directory under ~/ffmpeg_build

2. Rebuild ffmpeg with shared libraries enabled and install them under ~/ffmpeg_build - this can be done using the included build_ffmpeg.sh script

3. Ensure that the built libraries are on path by adding

  "export LD_LIBRARY_PATH=$HOME/ffmpeg_build/lib:$LD_LIBRARY_PATH"
  
  to the bottom of your .bashrc file.

4. build the project using catkin_make

5. test by running: roslaunch x264_image_transport test_x264.launch

Contributors
============
Alexander Botev

Luke Teacy

License
=======
Copyright © 2014 University of Southampton. All Rights Reserved.

Redistribution and use of this software in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY LUKE TEACY "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
