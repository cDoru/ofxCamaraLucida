/*
 * Camara Lucida
 * www.camara-lucida.com.ar
 *
 * Copyright (C) 2011  Christian Parsons
 * www.chparsons.com.ar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "cmlDepthmap.h"
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnLog.h>

namespace cml 
{
  class Depthmap_openni : public Depthmap
  {
    public:

      Depthmap_openni(){};
      ~Depthmap_openni(){};

      void update( 
          //uint16_t *mm_depth_pix,
          xn::DepthGenerator& depth_gen )
      {
        if ( mesh == NULL )
          return;

        const XnDepthPixel* _map = depth_gen.GetDepthMap(); 
        uint16_t *mm_depth_pix = (uint16_t*)_map;

        int len = mesh->length();

        XnPoint3D _pts2d[ len ];

        for ( int i = 0; i < len; i++ )
        {
          int xdepth, ydepth, idepth;

          mesh->to_depth( i, 
              &xdepth, &ydepth, &idepth );

          _pts2d[i].X = xdepth;
          _pts2d[i].Y = ydepth;
          _pts2d[i].Z = mm_depth_pix[ idepth ];
        }

        XnPoint3D _pts3d[ len ];

        depth_gen.ConvertProjectiveToRealWorld( 
            len, _pts2d, _pts3d ); 

        for ( int i = 0; i < len; i++ )
        {
          XnVector3D p3d = _pts3d[i];
          XnVector3D p2d = _pts2d[i];

          // mm to mts
          int xdepth = (int)p2d.X;
          int ydepth = (int)p2d.Y;
          float zmts = p3d.Z * 0.001;

          zmts = CLAMP((zmts==0.?5.:zmts),0.,5.);

          float x, y;

          depth->unproject( 
              xdepth, ydepth, zmts, &x, &y );  

          mesh->set_vertex( i, x, y, zmts );
 
        }

        mesh->update();
      };
  };
};


