/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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
    CPoint3d p0, p1, p2;
    CVector3d v1(1, 0, 0), vm(0, 1, 0);
    CPanel mast1, mast2; // half mast section
    unsigned int i = 0, j = 0;
    //unsigned int npl = mast1.left.size();   // number of right/left points
    unsigned int npb = mast1.bottom.size(); // number of bottom/top points
    real h = 0, cord;
    
    CPanelGroup rig;
    rig.type = RIG; // used for color scheme in saildispgl
    rig.title = rigID;
   
    // add mast //
    cord = MCord / 2;
    vm = CVector3d(MRakeM, MHeight, 0); //straight mast cord vector 
    
    // base section of mast
    h = 0;
    p0 = mastCenter ( h );
    for ( j = 0 ; j < npb ; j++ )
        {
            v1 = CVector3d(cos(PI * real(j) /(npb-1)), 0, sin(PI * real(j) /(npb-1) ) );
            v1.z() = v1.z() * MWidth / MCord;
            mast1.top[j] = p0 + cord * v1;
            mast2.top[j] = p0 - cord *v1;
        }
    
    // all other sections of mast
    for ( i = 1; i <= 20; i++ )
    {
        // top of current section
        h = real(i) * MHeight / 20; 
        p2 = mastCenter ( h );
        
        for ( j = 0 ; j < npb ; j++ )
        {
            v1 = CVector3d(cos(PI * real(j) /(npb-1)), 0, sin(PI * real(j) /(npb-1) ) );
            v1.z() = v1.z() * MWidth / MCord;
            mast1.bottom[j] = mast1.top[j];
            mast2.bottom[j] = mast2.top[j];
            mast1.top[j] = p2 + cord *v1;
            mast2.top[j] = p2 - cord * v1;
        }
        mast1.left.fill(mast1.bottom[0],mast1.top[0]); 
        mast1.right.fill(mast1.bottom[npb-1],mast1.top[npb-1]); 
    
        mast2.left.fill(mast2.bottom[0],mast2.top[0]); 
        mast2.right.fill(mast2.bottom[npb-1],mast2.top[npb-1]); 
        // add mast panel to rig
        rig.push_back(mast1);
        rig.push_back(mast2);
    }
    
    // add spreaders //
    cord = MCord / 6;
    for ( i = 1; i <= SPNB; i++)
    { 
        p2 = mastCenter ( SPH[i] );
        //printf ("P2 x= %f, y= %f \n", p2.x(), p2.y());
        for ( j = 0 ; j < npb ; j++ )
        {
            v1 = CVector3d(cos(PI * real(j) /(npb-1)), sin(PI * real(j) /(npb-1) ), 0 );
            v1.y() = v1.y() / 2;
            mast1.bottom[j] = p2 + cord * v1;
            mast1.bottom[j] = mast1.bottom[j] + CVector3d(0 , 0 , -SPW[i]); 
            mast1.top[j] = mast1.bottom[j] + CVector3d(0 , 0 , 2*SPW[i]); 
        }
        mast1.left.fill(mast1.bottom[0],mast1.top[0]); 
        mast1.right.fill(mast1.bottom[npb-1],mast1.top[npb-1]); 
        
        rig.push_back(mast1);
        
        // make symetrical by rotation
        mast1 = mast1.rotate(p2 , CMatrix::rot3d(2, PI) );
        
        rig.push_back(mast1);
    }
    
    // add shrouds //
    cord = MCord /12;  // half width of shroud
    p1 = p0;  // mast base centre
    
    if (SPNB >= 1)    // add inner shroud if at least one spreader
    {
        for ( i = 1; i <= SPNB; i++ ) 
        {
            p2 = mastCenter( SPH[i] );
            // make +Z shroud
            mast1.bottom[0] = p1 + cord * CVector3d (1,0,0) + CVector3d(0,0,SPW[i-1]);
            mast1.top[0] = p1 - cord * CVector3d(1,0,0) + CVector3d(0,0,SPW[i-1]); 
            mast1.bottom[npb-1] = p2 + cord * CVector3d (1,0,0) + CVector3d(0,0,MWidth/2);
            mast1.top[npb-1] = p2 - cord * CVector3d(1,0,0) + CVector3d(0,0,MWidth/2); 

            mast1.bottom.fill(mast1.bottom[0],mast1.bottom[npb-1]); 
            mast1.top.fill(mast1.top[0],mast1.top[npb-1]); 
            mast1.left.fill(mast1.bottom[0],mast1.top[0]); 
            mast1.right.fill(mast1.bottom[npb-1],mast1.top[npb-1]); 
            // make -Z symetrical 
            mast2.bottom[0] = p1 + cord * CVector3d (1,0,0) - CVector3d(0,0,SPW[i-1]);
            mast2.top[0] = p1 - cord * CVector3d(1,0,0) - CVector3d(0,0,SPW[i-1]); 
            mast2.bottom[npb-1] = p2 + cord * CVector3d (1,0,0) - CVector3d(0,0,MWidth/2);
            mast2.top[npb-1] = p2 - cord * CVector3d(1,0,0) - CVector3d(0,0,MWidth/2);

            mast2.bottom.fill(mast2.bottom[0],mast2.bottom[npb-1]);
            mast2.top.fill(mast2.top[0],mast2.top[npb-1]); 
            mast2.left.fill(mast2.bottom[0],mast2.top[0]); 
            mast2.right.fill(mast2.bottom[npb-1],mast2.top[npb-1]);
            // add inner shrouds to rig
            rig.push_back(mast1);
            rig.push_back(mast2);
            
            // add outer shroud to tip of spreaders
            // make +Z shroud
            mast1.bottom[0] = p1 + cord * CVector3d (1,0,0) + CVector3d(0,0,SPW[i-1]);
            mast1.top[0] = p1 - cord * CVector3d(1,0,0) + CVector3d(0,0,SPW[i-1]); 
            mast1.bottom[npb-1] = p2 + cord * CVector3d (1,0,0) + CVector3d(0,0,SPW[i]);
            mast1.top[npb-1] = p2 - cord * CVector3d(1,0,0) + CVector3d(0,0,SPW[i]); 

            mast1.bottom.fill(mast1.bottom[0],mast1.bottom[npb-1]); 
            mast1.top.fill(mast1.top[0],mast1.top[npb-1]); 
            mast1.left.fill(mast1.bottom[0],mast1.top[0]); 
            mast1.right.fill(mast1.bottom[npb-1],mast1.top[npb-1]); 
            // make -Z symetrical
            mast2.bottom[0] = p1 + cord * CVector3d (1,0,0) - CVector3d(0,0,SPW[i-1]);
            mast2.top[0] = p1 - cord * CVector3d(1,0,0) - CVector3d(0,0,SPW[i-1]); 
            mast2.bottom[npb-1] = p2 + cord * CVector3d (1,0,0) - CVector3d(0,0,SPW[i]);
            mast2.top[npb-1] = p2 - cord * CVector3d(1,0,0) - CVector3d(0,0,SPW[i]); 

            mast2.bottom.fill(mast2.bottom[0],mast2.bottom[npb-1]); 
            mast2.top.fill(mast2.top[0],mast2.top[npb-1]); 
            mast2.left.fill(mast2.bottom[0],mast2.top[0]); 
            mast2.right.fill(mast2.bottom[npb-1],mast2.top[npb-1]); 
            // add outer shroud to rig
            rig.push_back(mast1);
            rig.push_back(mast2);
        
            p1 = p2;
        }
    }
    // add cap shroud bit which is also the only shroud if n==0
        p2 = mastCenter( CSH );
        // make +Z
        mast1.bottom[0] = p1 + cord * CVector3d (1,0,0) + CVector3d(0,0,SPW[i-1]);
        mast1.top[0] = p1 - cord * CVector3d(1,0,0) + CVector3d(0,0,SPW[i-1]); 
        mast1.bottom[npb-1] = p2 + cord * CVector3d (1,0,0) + CVector3d(0,0,MWidth/2);
        mast1.top[npb-1] = p2 - cord * CVector3d(1,0,0) + CVector3d(0,0,MWidth/2); 

        mast1.bottom.fill(mast1.bottom[0],mast1.bottom[npb-1]); 
        mast1.top.fill(mast1.top[0],mast1.top[npb-1]); 
        mast1.left.fill(mast1.bottom[0],mast1.top[0]); 
        mast1.right.fill(mast1.bottom[npb-1],mast1.top[npb-1]); 
        // make -Z symetrical 
        mast2.bottom[0] = p1 + cord * CVector3d (1,0,0) - CVector3d(0,0,SPW[i-1]);
        mast2.top[0] = p1 - cord * CVector3d(1,0,0) - CVector3d(0,0,SPW[i-1]); 
        mast2.bottom[npb-1] = p2 + cord * CVector3d (1,0,0) - CVector3d(0,0,MWidth/2);
        mast2.top[npb-1] = p2 - cord * CVector3d(1,0,0) - CVector3d(0,0,MWidth/2); 

        mast2.bottom.fill(mast2.bottom[0],mast2.bottom[npb-1]); 
        mast2.top.fill(mast2.top[0],mast2.top[npb-1]); 
        mast2.left.fill(mast2.bottom[0],mast2.top[0]); 
        mast2.right.fill(mast2.bottom[npb-1],mast2.top[npb-1]); 
        // add cap shroud to rig
        rig.push_back(mast1);
        rig.push_back(mast2);
    //    
    return rig;
}

/** Routine for computing the 3D point at center of mast 
 *   at a given height 
 *
 * @author Robert Laine
 */
CPoint3d CRigWorker::mastCenter( const real &HM ) const 
{
    CPoint3d p0, p1;
    CVector3d vm;
    real cord = MCord / 2;
    
    //straight mast cord vector 
    vm = CVector3d(MRakeM, MHeight, 0);  
    // upright mast base centre point
    p0 = CPoint3d (foreJ + cord , 0 , 0); 
    // base centre point offset by rake 
    p0 = p0 + CVector3d(-(MRakeM * foreI / MHeight), 0, 0); 
    
    real h = HM / MHeight;
    if ( h < 0 ) h = 0;
    if ( h > 1 ) h = 1;
    
    // round
    real round = MRnd * RoundP( h, MRndPos ); 
        // printf ("i= %d, MRnd = %f \n", i, round);

    // straight mast section center point
    p1 = p0 + HM * vm.unit();

    // displace center point of section by round
    p1 = p1 + CMatrix::rot3d( 2, PI/2 ) * vm.unit() * round;
    //
    return p1;
}
