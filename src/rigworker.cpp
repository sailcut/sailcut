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
#include "sailcalc.h"


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
    CPoint3d p0, p2;
    CVector3d v1(1, 0, 0), vm(0, 1, 0);
    CPanel mast1, mast2; // half mast section
    unsigned int i = 0, j = 0;
    //unsigned int npl = mast1.left.nbpoints();   // number of right/left points
    unsigned int npb = mast1.bottom.nbpoints(); // number of bottom/top points
    real h = 0, cord, round;
    
    CPanelGroup rig;
   
    /** add test mast */
    cord = MCord / 2;
    vm = CVector3d(MRakeM, MHeight, 0); //straight mast cord vector 
    p0 = CPoint3d (foreJ + cord , 0 , 0); // mast centre base point
    p0 = p0 + CVector3d(-(MRakeM * foreI / MHeight), 0, 0); // base centre offset by rake 
    
    for ( j = 0 ; j < npb ; j++ )
        {
            v1 = CVector3d(cos(PI * real(j) /(npb-1)), 0, sin(PI * real(j) /(npb-1) ) );
            v1.z() = v1.z() * MWidth/MCord;
            mast1.top.point[j] = p0 + cord * v1;
            mast2.top.point[j] = p0 - cord *v1;
        }
    
    for ( i = 1; i <= 10; i++ )
    {
        h = i * MHeight / 10; // top of section
        round = MRnd * RoundP(h/MHeight , MRndPos); // round
        // printf ("i= %d, MRnd = %f \n", i, round);
        // mast section center
        p2 = p0 + h * vm.unit();
        // displace center by round
        p2 = p2 + CMatrix::rot3d(2, PI/2) * vm.unit() * round;

        for ( j = 0 ; j < npb ; j++ )
        {
            v1 = CVector3d(cos(PI * real(j) /(npb-1)), 0, sin(PI * real(j) /(npb-1) ) );
            v1.z() = v1.z() * MWidth/MCord;
            mast1.bottom.point[j] = mast1.top.point[j];
            mast2.bottom.point[j] = mast2.top.point[j];
            mast1.top.point[j] = p2 + cord *v1;
            mast2.top.point[j] = p2 - cord * v1;
        }
        mast1.left.fill(mast1.bottom.point[0],mast1.top.point[0]); 
        mast1.right.fill(mast1.bottom.point[npb-1],mast1.top.point[npb-1]); 
        rig.panel.push_back(mast1);
    
        mast2.left.fill(mast2.bottom.point[0],mast2.top.point[0]); 
        mast2.right.fill(mast2.bottom.point[npb-1],mast2.top.point[npb-1]); 
        rig.panel.push_back(mast2);
    }
    /** add test spreader */
    cord = MCord / 6;
    for ( i = 1; i <= SPNB ; i++)
    { 
        p2 = p0 + vm.unit() * SPH[i];
        round = MRnd * RoundP(SPH[i]/MHeight , MRndPos); // round
        p2 = p2 + CMatrix::rot3d(2, PI/2) * vm.unit() * round;
        //printf ("P2 x= %f, y= %f \n", p2.x(), p2.y());
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


