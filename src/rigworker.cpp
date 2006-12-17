/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "rigworker.h"


/** The constructor does some preliminary calculations to set
 *  internal variables.
 */
CRigWorker::CRigWorker(const CRigDef &d) : CRigDef(d)
{
}


/** Creates the rig.
 *
 * @return CPanelGroup
 */
CPanelGroup CRigWorker::makeRig() const
{
    unsigned int i = 0, j = 0;
    CPoint3d p1(0, 0, 0), p2(0, 0, 0);
    CVector3d v1(1, 0, 0), vm(0, 1, 0);
    CPanel mast1, mast2; // half mast section
    //unsigned int npl = mast1.left.nbpoints();   // number of right/left points
    unsigned int npb = mast1.bottom.nbpoints(); // number of bottom/top points
    
    CPanelGroup rig;
   
    /** add test mast */
    // TODO take into account mast rake and round
    real cord = MCord / 2;
    vm = CVector3d(MRakeM, MHeight, 0);
    p1 = CPoint3d (foreJ + cord , 0 , 0); // mast centre base point
    p1 = p1 + CVector3d(-(MRakeM * foreI / MHeight), 0, 0); // offset by rake 
    p2 = p1 + vm;
    
    for ( j = 0 ; j < npb ; j++ )
    {
        v1 = CVector3d(cos(PI * real(j) /(npb-1)), 0, sin(PI * real(j) /(npb-1) ) );
        v1.z() = v1.z() * MWidth/MCord;
        mast1.bottom.point[j] = p1 + cord * v1;
        mast1.top.point[j] = p2 + cord *v1;
        mast2.bottom.point[j] = p1 - cord *v1;
        mast2.top.point[j] = p2 - cord * v1;
    }
    mast1.left.fill(mast1.bottom.point[0],mast1.top.point[0]); 
    mast1.right.fill(mast1.bottom.point[npb-1],mast1.top.point[npb-1]); 
    rig.panel.push_back(mast1);
    
    mast2.left.fill(mast2.bottom.point[0],mast2.top.point[0]); 
    mast2.right.fill(mast2.bottom.point[npb-1],mast2.top.point[npb-1]); 
    rig.panel.push_back(mast2);
    
    /** add test spreader */
    cord = MCord / 6;
    for ( i = 1; i <= SPNB ; i++)
    { 
        p2 = p1 + vm.unit() * SPH[i];
        for ( j = 0 ; j < npb ; j++ )
        {
            v1 = CVector3d(cos(PI * real(j) /(npb-1)), sin(PI * real(j) /(npb-1) ), 0 );
            v1.y() = v1.y() / 2;
            mast1.bottom.point[j] = p2 + cord * v1;
            mast1.bottom.point[j] = mast1.bottom.point[j] + CVector3d(0 , 0 , -SPW[i]); 
            mast1.top.point[j] = mast1.bottom.point[j] + CVector3d(0 , 0 , 2*SPW[i]); 
        }
        mast1.left.fill(mast1.bottom.point[0],mast1.top.point[0]); 
        mast1.right.fill(mast1.bottom.point[npb-1],mast1.top.point[npb-1]); 
        rig.panel.push_back(mast1);
        // make second half by rotation
        mast1 = mast1.rotate(p2 , CMatrix::rot3d(2, PI) );
        rig.panel.push_back(mast1);
    }
     
    return rig;
}


