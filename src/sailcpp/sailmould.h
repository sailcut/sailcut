/*
 * Copyright (C) 1993-2018 Robert & Jeremy Laine
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SAILMOULD_H
#define SAILMOULD_H

#include <geocpp/geocpp.h>
#include <vector>

using namespace std;

/** Class used to work on sail profiles.
 *
 * @ingroup SailCpp
 * @see CSailMould
 */
class CProfile
{
public:
    CProfile( real rDepth=0.05, real rLeech=0.02, real rLuff=1 );

    real z( real dX ) const;
    real slope( real dX ) const;
    real camber( real dX ) const;

    /** Accessor for the depth */
    real getDepth() const
    {
        return depth;
    }
    /** Accessor for the leech */
    real getLeech() const
    {
        return kleech;
    }
    /** Accessor for the luff */
    real getLuff() const
    {
        return kluff;
    }
    /** Accessor for xmax */
    real getMaxPos() const
    {
        return xmax;
    }
    /** Accessor for zmax */
    real getMaxDepth() const
    {
        return zmax;
    }

    /** Set the depth */
    void setDepth( real ndepth )
    {
        depth = ndepth;
        calcMax();
    }
    /** Set the leech */
    void setLeech( real nkleech )
    {
        kleech = nkleech;
        calcMax();
    }
    /** Set the luff */
    void setLuff( real nkluff )
    {
        kluff = nkluff;
        calcMax();
    }

protected:
    void calcMax();

    // member variables
    /** depth */
    real depth;
    /** leech coefficient */
    real kleech;
    /** luff coefficient */
    real kluff;
    /** x location of maximum z */
    real xmax;
    /** maximum depth z */
    real zmax;
};


/** Class used to store the profile for a sail.
 *
 * see CFormMould, CProfile
 */
class CSailMould
{
public:
    CSailMould();

    CProfile interpol( const real h ) const;

    /** the mould's profiles ( top, middle, bottom ) */
    //  CProfile profile[3];
    vector<CProfile> profile;

    /** vertical position of profile[1] in percent */
    int vertpos;
};

#endif
