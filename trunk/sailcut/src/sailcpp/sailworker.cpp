/*
 * Copyright (C) 1993-2009 Robert & Jeremy Laine
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

#include <cmath>

#include "sailworker.h"
#include "sailcalc.h"

#define MAX_PANELS 210

/* To enable debugging */
// #define DEBUG 1


/** 
 *  The constructor does some preliminary calculations to set
 *  internal variables.
 */
CSailWorker::CSailWorker(const CSailDef &s) : CSailDef(s)
{
    /* First compute the coordinates of the corner of the sail */
    CVector3d v, v1;
    real x = 0, headstay = 0;

    /* Then Compute the coordinates of the 4 corners of the sail */
    switch ( sailType )
    {
    case JIB:
        x = tackY * foreJ / foreI;
        headstay = sqrt(foreI * foreI + foreJ * foreJ);
        rake = foreJ * luffL / headstay;
        tack = CPoint3d( x , tackY , 0 );
        //gaffL = 1;
        gaffR = 0;
        gaffRP = 50;   // imposed value for short gaff
        gaffDeg = atan2( foreI , foreJ ) * 180/PI - 10;
        break;
    default:
        tack = CPoint3d( tackX, tackY, 0 );
        break;
    }

    /** VC++ 6 pre-SP3 bug if we pass the result of a square root directly, see : MS knowledgebase 217164). */
    real stupid_hack = sqrt(luffL*luffL - rake*rake);
    head = tack + CVector3d(rake, stupid_hack, 0);

    v = CVector3d(0, gaffL, 0);  // initial vector gaff set on vertical

    peak = head + CMatrix::rot3d( 2 , (-asin(rake / luffL) - gaffDeg * PI / 180) ) * v;

    if ( fabs(peak.y() - head.y()) < 1 )
        peak.y() = head.y() + 1; // to avoid case with gaff horizontal

    /* Compute triangle tack-peak-clew. */
    real aa, b, bb;
    bb = atan2( peak.y() - tack.y() , peak.x() - tack.x() );
    b = CVector3d(peak - tack).norm();
    aa = Atriangle( leechL , b , footL );

    v1 = CVector3d( footL, 0, 0 );  // initial foot vector set on horizontal
    if (sailType == WING)
        clew = tack + v1;
    else
        clew = tack + CMatrix::rot3d( 2 , bb-aa ) * v1;
    /* end of computation of corners of the sail */

    /** Define foot vector of sail edge. */
    footV = CVector3d( clew - tack );
    /** Define gaff vector of sail edge. */
    gaffV = CVector3d( peak - head );
    /** Define fleech vector of sail edge. */
    leechV = CVector3d( peak - clew );
    /** Define luff vector of sail edge. */
    luffV = CVector3d( head - tack );

    /** Define mitre vector bisecting foot-leech angle. */
    mitreV = CVector3d( tack - clew ).unit() + leechV.unit();

    /** Define the unitary vectors perpendicular to foot edge, rotated anti-clockwise. */
    footVP = CMatrix::rot3d(2, PI/2) * footV.unit();
    /** Define the unitary vectors perpendicular to gaff edge, rotated anti-clockwise. */
    gaffVP = CMatrix::rot3d(2, PI/2) * gaffV.unit();
    /** Define the unitary vectors perpendicular to leech edge, rotated anti-clockwise. */
    leechVP = CMatrix::rot3d(2, PI/2) * leechV.unit();
    /** Define the unitary vectors perpendicular to luff edge, rotated anti-clockwise. */
    luffVP = CMatrix::rot3d(2, PI/2) * luffV.unit();

    /** Define useful straight lines of edges and mitre. */
    footLine = CSubSpace3d::line(tack , footV);
    gaffLine = CSubSpace3d::line(head , gaffV);
    leechLine = CSubSpace3d::line(clew , leechV);
    luffLine = CSubSpace3d::line(tack , luffV);
    mitreLine = CSubSpace3d::line(clew , mitreV);

    /** Define point at intersection of mitre and luff. */
    mitreLuffPt = EdgeIntersect( LUFF_EDGE, clew , mitreV );
}


/** 
 *  Make a Sail from its definition.
 */
CPanelGroup CSailWorker::makeSail() const
{
    CPanelGroup flatsail, dispsail;
    return makeSail(flatsail,dispsail);
}


/** 
 *  Make a Sail from its definition.
 *  This is the main routine of all the sail layout work
 *  The output is a 3D sail, its display and development versions.
 */
CPanelGroup CSailWorker::makeSail( CPanelGroup &flatsail , CPanelGroup &dispsail) const
{
    CPanelGroup output;

    switch ( sailType )
    {
    case WING:
        output = LayoutWing(flatsail , dispsail);
        break;
    default:
        switch ( sailCut )
        {
        case CROSS:
            output = Layout0(flatsail , dispsail);
            break;
        case TWIST:
            output = LayoutTwist(flatsail , dispsail);
            break;
        case HORIZONTAL:
            output = Layout0(flatsail , dispsail);
            break;
        case VERTICAL:
            output = LayoutVertical(flatsail , dispsail);
            break;
        case RADIAL:
            output = LayoutRadial(flatsail , dispsail);
            break;
        case MITRE:
            output = LayoutMitre(flatsail , dispsail);
            break;
        case MITRE2:
            output = LayoutMitre2(flatsail , dispsail);
            break;
        default:
            throw layout_error("CSailWorker::makeSail : unknown sail cut layout!");
        }
    }
    // place the labels at the centre of each panel
    output.placeLabels();
    dispsail.placeLabels();
    flatsail.plotLabels();

    // assign the sail names
    output.title = sailID + " (3D)";
    dispsail.title = sailID + " (3D)";
    flatsail.title = sailID + " (flat)";

    return output;
}


/** 
 *  Creates a Cross cut or horizontal cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 *
 * @author Robert Laine alias Sailcuter
 */
CPanelGroup CSailWorker::Layout0( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
    *   located on the straight edge of the sail (no round)
    *   p1[] are on the luff side and p2[] are on the leech side
    */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS);  // 3D sail
    CPanelGroup dev(MAX_PANELS);  // developed sail

    /* create number of panels */
    unsigned int npanel = 1;
    unsigned int npl = lay[0].right.size();   // number of right/left points
    unsigned int npb = lay[0].bottom.size();  // number of bottom/top points

    unsigned int j = 0, k = 0, cnt = 0;
    bool flag = false;  // to check if top of sail is reached

    /* create arrays t1 and t2 of type of intersection of upper seam
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    */
    int t1[MAX_PANELS], t2[MAX_PANELS];

    /* define point ip for intersections */
    CPoint3d ip;

    CVector3d seamV; // seam Vector
    CSubSpace seamL; // seam Line

    if ( sailCut == HORIZONTAL )
        seamV = CVector3d(-1, 0, 0);  // horizontal seam orientation for Horizontal cut
    else  // define seamV as the vector perpendicular to the leech vector (peak-clew)
        seamV = leechVP;  // for classical cross cut

    /* create variables for the development and edge corrections */
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    vector<CVector3d> deviation;
    deviation.resize(npb);
    vector<CVector3d> deviaPrev;
    deviaPrev.resize(npb);

    /* Other edge hem width */
    real luffHemW = hemsW; 
    // real luffInnerHemW, footInnerHemW;

    real CC = 0, x = 0, y = 0;

    /* seam 0 is on the foot of the sail ending at the clew */
    p1[0] = tack; // initialise seam forward end at tack point
    p2[0] = clew; // initialise seam aft end at clew point
    t1[0] = 1;
    t2[0] = 4;    // type=4=leech intersection

    /** Lay the panels starting from the foot, going upward to the peak */
    
    for (npanel = 1 ; npanel < MAX_PANELS -1 ; npanel++)
    {
        real exc = 0; // current excess of width
        real exb = 0; // total correction
        cnt = 0; // counter of iterations

        do  /* Loop for optimising the seam position to fit cloth width */
        {
            cnt++;
            p2[npanel] = p2[npanel-1] + (clothW - seamW - exb) * leechV.unit();
            t2[npanel] = 4; // type2 = 4 = leech intersection for all horizontally cut panels
            seamL = CSubSpace3d::line( p2[npanel] , seamV );

            if ( CVector3d( p2[npanel] - peak ) * leechV > 0 )
            {   // we are above peak, stop this is last panel
                flag = true;
                p2[npanel] = peak;
                // check on which side of the sail the previous point p1 is located
                if ( t1[npanel-1] < 2 )
                { // previous seam on foot
                    p1[npanel] = head;
                    t1[npanel] = 2; // set on luff
                    // left points on foot-tack-luff
                    lay[npanel-1].left.fill(p1[npanel-1], tack, p1[npanel]);
                    for (k = 0 ; k < npl / 2 ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( FOOT_EDGE, lay[npanel-1].left[k] , seamV);
                    for (k = npl / 2 +1 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k] , seamV);
                }
                else if ( t1[npanel-1] == 2 )
                { // left points on luff
                    p1[npanel] = head;
                    t1[npanel] = 2;
                    lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for (k = 0 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
                }
                else
                { // left points on gaff
                    p1[npanel] = p1[npanel-1];
                    t1[npanel] = 3;
                    lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for (k = 0 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], seamV);
                }

                // fill right points on leech
                lay[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
                for (k = 0 ; k < npl ; k++)
                    lay[npanel-1].right[k]=EdgeIntersect( LEECH_EDGE, lay[npanel-1].right[k], seamV);

                // fill bottom points
                lay[npanel-1].bottom.fill(lay[npanel-1].left[0], lay[npanel-1].right[0]);

                // fill top  points
                lay[npanel-1].top.fill(lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1]);

                // move all top points of top panel to gaff curve
                for (k = 1 ; k < npb -1 ; k++)
                    lay[npanel -1].top[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].top[k], CVector3d (head.y()-peak.y(),peak.x()-head.x(),0));
                // end peak panel //
            }
            else   // normal panel below peak //////
            {
                /* find position of luff/seam intersection relative to tack and head */
                if ( seamL.intersect(luffLine).getdim() == 0 )
                    ip = seamL.intersect(luffLine).getp();
                else throw layout_error("CSailWorker::Layout0 -1 : intersection of seam and luff is not a point!");

                if ( CVector3d( ip - tack ) * luffV <= 0 )
                {   // seam intersects foot
                    if ( seamL.intersect(footLine).getdim() == 0 )
                        p1[npanel] = seamL.intersect(footLine).getp();
                    else throw layout_error("CSailWorker::Layout0 -2 : intersection of seam and foot is not a point!");

                    t1[npanel] =1;  // type1=1 = foot type of intersection

                    if ( npanel == 1 )
                    {   // set lower edge to start at same point p1
                        p1[0] = p1[npanel];
                        t1[0] = 1;
                    }
                }
                else if ( CVector3d(ip- head) * luffV > 0 )
                {   // seam intersects gaff
                    if ( seamL.intersect(gaffLine).getdim() == 0 )
                        p1[npanel] = seamL.intersect(gaffLine).getp();
                    else throw layout_error("CSailWorker::Layout0 -3 : intersection of seam and foot is not a point!");

                    t1[npanel] = 3;  // 3 = gaff type of intersection
                }
                else
                {   // seam intersects luff
                    p1[npanel] = ip;
                    t1[npanel] = 2;  // 2 = luff
                    if ( npanel == 1 )
                    {   // force seam 0 to start at the tack
                        p1[0] = tack;
                        t1[0] = 2;
                    }
                }

                /* We now add the intermediate points on all sides of the normal panel */

                /* Below is the code for the left side depending
                *  on t1 for the top side and bottom side
                */
                if ( t1[npanel-1] == 1  &&  t1[npanel] == 1 )
                {   // full foot
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( FOOT_EDGE, lay[npanel-1].left[k], seamV);
                }
                else if ( t1[npanel-1] == 2  &&  t1[npanel] == 2 )
                {   // full luff
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k] , seamV);
                }
                else if ( t1[npanel-1] == 3  &&  t1[npanel] == 3 )
                {   // full gaff
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k] , seamV);
                }
                else if ( (t1[npanel-1] == 1) && (t1[npanel] == 2) )
                {   // foot-tack-luff
                    lay[npanel-1].left.fill(p1[npanel-1], tack, p1[npanel]);
                    for (k = 0 ; k < npl / 2 ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( FOOT_EDGE, lay[npanel-1].left[k], seamV);
                    for (k = npl / 2 +1 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
                }
                else if ( (t1[npanel-1] == 2) && (t1[npanel] == 3) )
                {   // luff-head-gaff
                    lay[npanel-1].left.fill(p1[npanel-1], head, p1[npanel]);
                    for (k = 0 ; k < npl/2 ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
                    for (k = npl / 2 +1 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], seamV);
                } // end IF ELSE for left side

                /* Below is the code for the intermediate points of the right side
                *  which are all on the leech for a crosscut layout.
                */
                // first check if upper point is not below lower point
                if ( CVector3d( p2[npanel] - p2[npanel-1] ) * leechV < 0)
                    p2[npanel] = p2[npanel-1];

                lay[npanel-1].right.fill(p2[npanel-1] , p2[npanel]);
                for (k = 0 ; k < npl ; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LEECH_EDGE, lay[npanel-1].right[k], seamV);

                /* Below is the code for the intermediate points of the top and bottom sides.
                *  The first point is identical to the last point of the left side
                *  The last point is identical to the last point of the right side
                */
                lay[npanel-1].top.fill( lay[npanel-1].left[npl-1] , lay[npanel-1].right[npl-1] );
                lay[npanel-1].bottom.fill( lay[npanel-1].left[0] , lay[npanel-1].right[0] );

                /* Below is the code for the intermediate points of the bottom side of first panel  */
                if ( npanel == 1 )
                { // move bottom side of first panel to foot curve
                    for (k = 1 ; k < npb -1 ; k++)
                    {
                        lay[0].bottom[k] = EdgeIntersect( FOOT_EDGE, lay[0].bottom[k], CVector3d(0,-1,0));
                    }
                }
#ifdef DEBUG
                if ( npanel == 1 )
                {
                    cout << "CSailWorker::Layout0 Crosscut foot after adding curve" << endl;
                    for (k = 0 ; k < npb ; k++)
                        cout << "pt="<< k << " xyz=" << lay[0].bottom[k] << endl;
                }
#endif
            }  /* end else normal panel */

            /** Go over all the points of current panel and calculate their Z */
            lay[npanel-1] = Zpanel(lay[npanel-1]);

#ifdef DEBUG
            if ( npanel == 1 )
            { // move bottom side of first panel to foot curve
                cout << "CSailWorker::Layout0 Crosscut foot after Z " << endl;
                for (k = 0 ; k < npb ; k++)
                    cout << "pt="<< k << " xyz=" << lay[0].bottom[k] << endl;

                cout << "---end Z foot----   DO LOOP=" << cnt << endl;
            }
#endif

            /** Develop the current panel */
            if ( npanel == 1 ) {
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_TOP);
            }
            else {
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for (k = 1; k < npb-1; k ++)
                    dev[npanel-1].bottom[k] = dev[npanel-1].bottom[k] + deviaPrev[k];
            }

            /**  Compute and store the deviation of top edge of
            *   the developed panel and straighten this top edge
            *   except if this is the top panel
            */
            if ( flag == false ) {
                vb= CMatrix::rot3d(2,PI/2) * CVector3d(dev[npanel-1].top[npb-1] - dev[npanel-1].top[0]).unit();
                for (k = 1 ; k < npb -1 ; k ++)
                {
                    vk= CVector3d (dev[npanel-1].top[k] - dev[npanel-1].top[0]);
                    v= vb * -(vk*vb);
                    deviation[k] = v;
                    dev[npanel-1].top[k] = dev[npanel-1].top[k] + deviation[k];
                }
            }

            /** Add the seam and hems allowance */
            if ( npanel == 1 ) {
                dev[npanel-1].add6Hems( hemsW, hemsW, seamW, leechHemW, leechHemW, footHemW );
            }        
            else if ( flag == true ) {
                dev[npanel-1].add6Hems( hemsW, hemsW, hemsW, leechHemW, leechHemW, 0 );
            }
            else {
                if ( t1[npanel-1] == 1 && t1[npanel] == 2 )
                    dev[npanel-1].add6Hems( footHemW, luffHemW, seamW, leechHemW, leechHemW, 0 );
                else 
                    dev[npanel-1].add6Hems( hemsW, hemsW, seamW, leechHemW, leechHemW, 0 );
            }
            /* Check the width of developed panel and store the excess */
            exc = dev[npanel-1].boundingRect().height() - clothW;

            /* Sum previous correction + 80% of current excess of width + 1mm */
            exb += 0.8 * exc + 1;
        }
        while ( exc > 0  &&  cnt < 9 );
        /* loop as long the excess of width is positive AND counter < 9 */

        deviaPrev = deviation;

        /** Reposition the developed panel such that the 
        *  lowest point is Y=0 AND most left point is X=0.
        */
        dev[npanel-1] = dev[npanel-1].reframe(LOW_LEFT);

        /* check if peak has been reached to break off */
        if ( flag == true )
            break;
    }  /** Loop FOR next panel */

    if ( npanel == MAX_PANELS-1 )
        throw layout_error("CSailWorker::Layout0 : MAX_PANELS without reaching head, do increase cloth width ");

    /* Copy the sails for 3D display */
    CPanelGroup sail(npanel);

    for (j = 0 ; j < npanel ; j ++)
        sail[j] = lay[j];


    /** Create the displays version of the developed sail  */
    
    /* Copy the developed sail */
    flatsail = CPanelGroup(npanel);

    for (j = 0 ; j < npanel ; j++)
    {
        flatsail[j] = dev[j];
    }

    /* Re-position the developed panels in a clean stack */
    dispsail = flatsail;
    for (j = 1 ; j < npanel ; j++)
    {
        top = dispsail[j-1].top[0];
        bot = dispsail[j].bottom[0];

        // rotation to align bottom of panel to top of previous panel
        x = dispsail[j-1].top[npb-1].x() - top.x();
        y = dispsail[j-1].top[npb-1].y() - top.y();
        CC = atan2(y , x);
        dispsail[j] = dispsail[j].rotate(bot,CMatrix::rot3d(2,CC));

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = CVector3d ( top - CPoint3d(0,0,0) );
        v.x() = v.x() - bot.x();
        v.y() = v.y() + 2 * seamW +20; // adding offset to separate panels vertically

        dispsail[j] = dispsail[j] + v;
    }

    return sail;
} /* end layout0 = cross cut or horizontal //////////////// */


/** Creates a twist foot cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine alias Sailcuter
 */
CPanelGroup CSailWorker::LayoutTwist( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
     *   located on the straight edge of the sail (no round)
     */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* create number of panels */
    unsigned int npanel = 1;
    unsigned int npl = lay[0].right.size();  // number of right/left points
    unsigned int npb = lay[0].bottom.size(); // number of bottom/top points

    unsigned int j = 0, k = 0, cnt = 0;
    bool flag = false;

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    */
    int t1[MAX_PANELS], t2[MAX_PANELS];

    /* define point ip for intersections */
    CPoint3d ip;

    /* define the seamV as the vector perpendicular to the leech vector (peak-clew)*/
    CVector3d seamV = leechVP;
    CSubSpace seamL;  // seam line
    CVector3d seamVT; // seam vector twisted
    CSubSpace seamLT; // seam line twisted

    /* create variables for the development and edge corrections */
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    vector<CVector3d> deviation;
    deviation.resize(npb);
    vector<CVector3d> deviaPrev;
    deviaPrev.resize(npb);

    /* create variable to monitor excess over cloth width */
    real CC=0, x=0, y=0;

    /* seam 0 is on the foot of the sail ending at the clew */
    p1[0] = tack; // initialised at tack point
    p2[0] = clew;
    t1[0] = 2;
    t2[0] = 4;    // type=4=leech intersection

    /* Other edge hem width */
    real luffHemW = hemsW; 
    // real luffInnerHemW, footInnerHemW;
    
    /** Start laying the panels from foot upward to the peak */
    for (npanel = 1; npanel < MAX_PANELS-1; npanel++)
    {
        real exb = 0; // total correction
        real exc = 0; // current excess of width
        cnt = 0;

        /* Loop for optimising the seam position to fit cloth width */
        do
        {
            cnt++;
            p2[npanel] = p2[npanel-1] + leechV.unit() * (clothW - seamW - exb);
            t2[npanel] = 4; // type2=4=leech intersection for all horizontally cut panels
            seamL = CSubSpace3d::line( p2[npanel] , seamV );

            if ( CVector3d(p2[npanel]-peak) * leechV > 0 )
            {  // we are above peak, stop this is last panel 
                flag=true;
                p2[npanel] = peak;

                // check where previous point p1 is
                if (t1[npanel-1] <= 2)
                { // left points on luff
                    p1[npanel] = head;
                    t1[npanel] = 2;
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0; k < npl; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
                }
                else
                { // left points on gaff
                    p1[npanel] = p1[npanel-1];
                    t1[npanel] = 3;
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0; k < npl; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], seamV);
                }

                // fill right points on leech
                lay[npanel-1].right.fill(p2[npanel-1] , p2[npanel]);
                for (k = 0; k < npl; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LEECH_EDGE, lay[npanel-1].right[k], seamV);

                // fill bottom points
                lay[npanel-1].bottom.fill(lay[npanel-1].left[0], lay[npanel-1].right[0]);

                // fill top  points
                lay[npanel-1].top.fill(lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1]);

                // move all top points to gaff curve
                for (k=1; k < npb-1; k++)
                    lay[npanel-1].top[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].top[k], CVector3d (head.y()-peak.y() , peak.x()-head.x() , 0));

                // end peak panel //////
            }
            else // normal panel
            { 
                /* find nominal position of luff/seam intersection relative to tack and head */
                if ( seamL.intersect(luffLine).getdim() == 0 )
                    ip = seamL.intersect(luffLine).getp();
                else throw layout_error("CSailWorker::LayoutTwist -1 : twist intersection of seam and luff is not a point!");

                if ( CVector3d( (ip - luffV.unit() * (seamW + clothW/5) ) - p1[npanel-1] ) * luffV < 0 )
                {   // seam intersects luff below previous panel luff point + 1/5 clothW
                    p1[npanel] = p1[npanel-1] + luffV.unit() * (seamW + clothW/5);
                    t1[npanel] = 2;  // 2=luff type of intersection
                    seamVT = CVector3d( p1[npanel] - p2[npanel] ).unit();
                    seamLT = CSubSpace3d::line(p2[npanel] , seamVT);
                    ip = seamLT.intersect(luffLine).getp();
#ifdef DEBUG
                    cout << "CSailWorker::LayoutTwist Seam 1 LUFF CORRECTION DO LOOP = " << cnt << endl;
                    cout << " ip = " << ip << endl;
                    cout << "p1[0] " << p1[0] << "  p2[0] " << p2[0] << " type "<< t1[0] << t2[0] << endl;
                    cout << "p1[1] " << p1[1] << "  p2[1] " << p2[1] << " type "<< t1[1] << t2[1] << endl;
                    cout << "seam VT = " << seamVT << endl;
                    cout << "--- " << endl;
#endif
                }
                else if (CVector3d(ip - head) * luffV > 0)
                {   // seam intersects gaff
                    if ( seamL.intersect(gaffLine).getdim() == 0 )
                        p1[npanel] = seamL.intersect(gaffLine).getp();
                    else throw layout_error("CSailWorker::LayoutTwist -2 : intersection of seam and foot is not a point!");

                    t1[npanel] = 3;  // 3=gaff type of intersection
                    seamVT = seamV;
                }
                else
                { // seam intersects luff normally
                    p1[npanel] = ip;
                    t1[npanel] = 2;  // luff
                    seamVT = seamV;
                }

                /* We now add the intermediate points on all sides of the panel */
                
                /* Below is the code for the left side depending
                 *  on t1 for the top side and bottom side
                 */
                if (t1[npanel-1] == 2 && t1[npanel] == 2)
                { // full luff
                    lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);

                    for (k = 0; k < npl; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamVT);
                }
                else if (t1[npanel-1] == 3 && t1[npanel] == 3)
                { // full gaff
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0; k < npl; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], seamV);
                }
                else if ((t1[npanel-1] ==2 ) && (t1[npanel] == 3))
                { // luff-head-gaff
                    lay[npanel-1].left.fill(p1[npanel-1] , head, p1[npanel]);
                    for (k = 0; k < npl/2; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
                    for (k = npl/2 +1; k < npl; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], seamV);
                } // end IF ELSE for left side

                /*  Compute the intermediate points of the right side
                 *  which are all on the leech for a twist cut layout.
                 */
                // first check if upper point is not below lower point
                if ( CVector3d( p2[npanel] - p2[npanel-1] ) * leechV < 0 )
                    p2[npanel] = p2[npanel-1];

                lay[npanel-1].right.fill(p2[npanel-1] , p2[npanel]);
                for (k = 0; k < npl; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LEECH_EDGE, lay[npanel-1].right[k], seamV);

                /*  Compute the intermediate points of the top and bottom sides.
                 *  The first point is identical to the last point of the left side
                 *  The last point is identical to the last point of the right side
                 */
                lay[npanel-1].top.fill(lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1]);
                lay[npanel-1].bottom.fill(lay[npanel-1].left[0], lay[npanel-1].right[0]);
#ifdef DEBUG
                if ( npanel == 1 )
                {
                    cout << "CSailWorker::LaoutTwist foot straight  - LOOP= "<< cnt << endl;
                    for (k = 0 ; k < npb ; k++)
                        cout << "pt="<< k << " Bottom xyz= " << lay[0].bottom[k] << " Top xyz= " << lay[0].top[k] << endl;
                }
#endif

                /* Move the intermediate points of the bottom side of first panel */
                if ( npanel == 1 )
                { // move bottom side of first panel to foot curve
                    for (k = 1 ; k < npb-1 ; k++)
                        lay[0].bottom[k] = EdgeIntersect( FOOT_EDGE,  lay[0].bottom[k] , footVP );
                }

            } /* end else normal panel //////// */

            /* Now we go over all the points and calculate their Z */
            lay[npanel-1] = Zpanel(lay[npanel-1]);

            /* Develop the current panel */
            if ( npanel == 1 )
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_TOP);
            else
            {
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to this bottom edge
                for (k = 1 ; k < npb-1 ; k++)
                    dev[npanel-1].bottom[k] = dev[npanel-1].bottom[k] + deviaPrev[k];
            }

            /*  Compute and store the deviation of top edge of developed panel
             *  and straighten this top edge Except if this is the top panel. */
            if (flag == false)
            {
                vb= CMatrix::rot3d(2,PI/2) * CVector3d(dev[npanel-1].top[npb-1] - dev[npanel-1].top[0]).unit();
                for (k = 1 ; k < npb-1 ; k++)
                {
                    vk= CVector3d (dev[npanel-1].top[k] - dev[npanel-1].top[0]);
                    v= vb * -(vk * vb);
                    deviation[k] = v;
                    dev[npanel-1].top[k] = dev[npanel-1].top[k] + deviation[k];
                }
            }

            /* Add the seam and hems allowance */
            if (npanel == 1)
                dev[npanel-1].add6Hems( luffHemW, luffHemW, seamW, leechHemW, leechHemW, footHemW );
            else if (flag == true)
                dev[npanel-1].add6Hems( hemsW, hemsW, hemsW, leechHemW, leechHemW, 0 );
            else if ((t1[npanel-1] ==2 ) && (t1[npanel] == 2))
                dev[npanel-1].add6Hems( luffHemW, luffHemW, seamW, leechHemW, leechHemW, 0 );
            else if ((t1[npanel-1] ==2 ) && (t1[npanel] == 3))
                dev[npanel-1].add6Hems( luffHemW, hemsW, seamW, leechHemW, leechHemW, 0 );
            else 
                dev[npanel-1].add6Hems( hemsW, hemsW, seamW, leechHemW, leechHemW, 0 );
            
#ifdef DEBUG
            if ( npanel == 1 )
            { // move bottom side of first panel to foot curve
                cout << "CSailWorker::LayoutTwist foot after adding seams " << endl;
                for (k = 0 ; k < npb ; k++)
                    cout << "pt="<< k << " xyz=" << dev[0].bottom[k] << endl;

                cout << "------END LOOP="<< cnt << endl;
            }
#endif

            /* Check the width of developed panel */
            exc = dev[npanel-1].boundingRect().height() - clothW;

            /* Sum previous correction + 80% of current excess of width + 1mm */
            exb += 0.8 * exc + 1;
        }
        while ( exc > 0 && cnt < 9 );
        /* loop as long the excess of width is positive AND counter < 9 */

        deviaPrev = deviation;

        /* Now we reposition the developed panel such that
         *  bottom minimum is Y=0 AND most left point is X=0 */
        dev[npanel-1] = dev[npanel-1].reframe(LOW_LEFT);

        /* check if peak has been reached to break off */
        if ( flag == true )
            break;
    }  /* loop FOR next seam */

    if (npanel == MAX_PANELS-1)
        throw layout_error("CSailWorker::LayoutTwist -5 : MAX_PANELS without reaching head, do increase cloth width ");

    /* Copy the sails for display */
    CPanelGroup sail(npanel);
    for (j = 0; j < npanel; j++)
        sail[j] = lay[j];

    /* Copy the developed sail */
    flatsail = CPanelGroup(npanel);

    for (j = 0; j < npanel; j++)
        flatsail[j] = dev[j];

    /* Prepare the displays version of the developed sail */
    dispsail = flatsail;

    for (j = 1; j < npanel; j++)
    {
        top = dispsail[j-1].top[0];
        bot = dispsail[j].bottom[0];

        // rotation to align bottom of panel to top of previous panel
        x = dispsail[j-1].top[npb-1].x() - top.x();
        y = dispsail[j-1].top[npb-1].y() - top.y();
        CC = atan2(y,x);
        dispsail[j] = dispsail[j].rotate(bot,CMatrix::rot3d(2,CC));

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = top;
        v.x() -= bot.x();
        v.y() += 2 * seamW +20; // adding offset to separate panels vertically

        dispsail[j] = dispsail[j] + v;
    }

    return sail;
} /* end layout twist foot //////////// */


/** Creates a VERTICAL cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine alias Sailcuter
 */
CPanelGroup CSailWorker::LayoutVertical( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
    *   located on the straight edge of the sail (no round) */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* create number of panels */
    unsigned int npanel = 1;
    unsigned int npl = lay[0].right.size();  // number of right/left points
    unsigned int npb = lay[0].bottom.size(); // number of bottom/top points

    unsigned int j=0, k=0, cnt=0;
    bool flag=false;

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    */
    int t1[MAX_PANELS], t2[MAX_PANELS];

    /* define point ip for intersections */
    CPoint3d ip;

    /* define seamV as the vector parrallel to the leech vector (peak-clew)*/
    CVector3d seamV = leechV.unit();
    CSubSpace seamL; // seam Line

    /* create variables for the development and edge corrections */
    CPoint3d pt(0, 0, 0); // test point
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for panel width correction */
    vector<CVector3d> deviation;
    deviation.resize(npb);
    vector<CVector3d> deviaPrev;
    deviaPrev.resize(npb);

    /* Other edge hem width */
    real luffHemW = hemsW; 
    // real luffInnerHemW, footInnerHemW;
    
    /* seam 0 is on the leech of the sail ending at the peak */
    p1[0] = clew; // initialised at tack point
    p2[0] = peak;
    t1[0] = 1;
    t2[0] = 3;    // type=4=leech intersection

    /** Lay the panels parallel to the leech, from the leech toward the tack */
    
    for (npanel = 1; npanel < MAX_PANELS-1; npanel++)
    {
        real exb = 0; // total correction
        real exc = 0; // current excess of width
        cnt = 0;
        //if (npanel==3) flag=true;

        /* Loop for optimising the seam position to fit cloth width */
        do
        {
            cnt++;
            pt = p1[npanel-1] + (clothW -seamW -exb) * leechVP;
            seamL = CSubSpace3d::line(pt, seamV);
            if ( seamL.intersect(footLine).getdim() == 0 )
                p1[npanel] = seamL.intersect(footLine).getp();
            else throw layout_error("CSailWorker::LayoutVertical -1 : intersection of seam and foot is not a point!");

            t1[npanel] = 1; // type1=1= foot intersection vertically cut panels

            if ( p1[npanel].x() <= tack.x() )
            {   // last panel
                p1[npanel] = tack;
                t1[npanel] = 1;
                p2[npanel] = tack;
                t2[npanel] = 2;
                flag = true; // set the FLAG to get out of FOR
            }
            else
            {   // normal panel
                if ( seamL.intersect(gaffLine).getdim() == 0 )
                    p2[npanel] = seamL.intersect(gaffLine).getp();
                else throw layout_error("CSailWorker::LayoutVertical -2 : intersection of seam and gaff is not a point!");

                if ( CVector3d(p2[npanel]-head) * gaffV > 0 )
                    t2[npanel] = 3;
                else
                {
                    if ( seamL.intersect(luffLine).getdim() == 0 )
                        p2[npanel] = seamL.intersect(luffLine).getp();
                    else throw layout_error("LayoutVertical -3 : intersection of seam and luff is not a point!");

                    t2[npanel] = 2;
                }
            }

            // fill right side points
            if ( t2[npanel-1] == 2 && t2[npanel] == 2 )
            {
                // printf ("full luff \n");
                lay[npanel-1].right.fill(p2[npanel-1], p2[npanel]);
                for (k = 0; k < npl; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].right[k], seamV);
            }
            else if ( t2[npanel-1] == 3 && t2[npanel] == 2 )
            {
                // printf ("gaff-head-luff \n");
                lay[npanel-1].right.fill(p2[npanel-1], head, p2[npanel]);

                for (k = 0; k < npl/2; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].right[k], seamV);

                for (k = npl/2+1; k < npl; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].right[k], seamV);
            }
            else
            {
                // printf ("full gaff \n");
                lay[npanel-1].right.fill(p2[npanel-1], p2[npanel]);

                for (k = 0; k < npl; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].right[k], seamV);
            }
            // fill left side points which are all on foot
            lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
            for (k = 0; k < npl; k++)
                lay[npanel-1].left[k] = EdgeIntersect( FOOT_EDGE, lay[npanel-1].left[k], leechV);

            // fill bottom points
            lay[npanel-1].bottom.fill(lay[npanel-1].left[0], lay[npanel-1].right[0]);
            if ( npanel == 1 )
            {
                // bottom is on the leech
                for (k = 0; k < npb; k++)
                    lay[npanel-1].bottom[k] = EdgeIntersect( LEECH_EDGE, lay[npanel-1].bottom[k], leechVP);
            }

            /* fill top side points on seam */
            lay[npanel-1].top.fill(lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1]);
            
            /* Go over all the points and calculate their z */
            lay[npanel-1] = Zpanel(lay[npanel-1]);

            /* Develop the current panel */
            if ( npanel == 1 )
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_TOP);
            else
            {
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for (k = 1; k < npb-1; k ++)
                    dev[npanel-1].bottom[k] = dev[npanel-1].bottom[k] + deviaPrev[k];
            }

            /*  Now we compute the deviation of top edge of developed panel
            *   and straighten this top edge except if this is the top panel */
            if ( flag == false )
            {
                vb = CMatrix::rot3d(2 , PI/2)*CVector3d(dev[npanel-1].top[npb-1] -dev[npanel-1].top[0]).unit();
                for (k = 1; k < npb-1; k ++)
                {
                    vk = CVector3d (dev[npanel-1].top[k] - dev[npanel-1].top[0]);
                    v = vb * -(vk*vb);
                    deviation[k] = v;
                    dev[npanel-1].top[k] = dev[npanel-1].top[k] + deviation[k];
                }
            }

            /* Add the seam and hems allowance */
            if ( npanel == 1 )
                dev[npanel-1].add6Hems( footHemW, footHemW, seamW, hemsW, hemsW, leechHemW );
            else if ( flag == true )
                dev[npanel-1].add6Hems( footHemW, footHemW, luffHemW, luffHemW, luffHemW, 0 );
            else if ( t2[npanel-1] == 3 && t2[npanel] == 3 )
                dev[npanel-1].add6Hems( footHemW, footHemW, seamW, hemsW, hemsW, 0 );
            else if ( t2[npanel-1] == 3 && t2[npanel] == 2 )
                dev[npanel-1].add6Hems( footHemW, footHemW, seamW, luffHemW, hemsW, 0 );
            else 
                dev[npanel-1].add6Hems( footHemW, footHemW, seamW, luffHemW, luffHemW, 0 );

            /* Check the width of developed panel and store excess */
            exc = dev[npanel-1].boundingRect().height() - clothW;

            /* Sum previous correction + 80% of current excess of width + 1mm */
            exb += 0.8 * exc + 1;

        }
        while ( exc > 0 && cnt < 9 );
        /* loop as long the excess of width is positive AND counter < 9 */

        deviaPrev = deviation;

        /* Now we reposition the developed panel such that bottom left is X=0 Y=0 */
        dev[npanel-1] = dev[npanel-1].reframe(LOW_LEFT);

        /* check if peak has been reached to break off */
        if ( flag == true )
            break;

    }  /* Loop FOR next seam */

    if ( npanel == MAX_PANELS-1 )
        throw layout_error("CSailWorker::LayoutVertical -4 : MAX_PANELS without reaching tack, do increase cloth width ");

    /* Copy the sails for display */
    CPanelGroup sail( npanel );
    for (j = 0; j < npanel; j++)
        sail[j] = lay[j];

    /* Copy the developed sail into flatsail */
    flatsail = CPanelGroup( npanel );

    for (j = 0; j < npanel; j++)
        flatsail[j] = dev[j];

    /* Prepare the displays version of the developed sail */
    dispsail = flatsail;

    for (j = 1; j < npanel; j++)
    {
        top = dispsail[j-1].top[0];
        bot = dispsail[j].bottom[0];

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = top;
        v.x() -= bot.x();
        v.y() += 2 * seamW +10; // adding offset to separate panels vertically

        dispsail[j] = dispsail[j] + v;
    }

    return sail;
} /* end layout vertical cut //////// */


/** Creates a WING with horizontal cut layout.
 *  The panels are layed parrallel to the central line of the wing (horizontal cut)
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine alias Sailcuter
 */
CPanelGroup CSailWorker::LayoutWing( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
    *   located on the straight edge of the sail (no round) */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* Create number of panels */
    unsigned int npanel=1, np=1;
    unsigned int npl= lay[0].right.size();   // number of right/left points
    unsigned int npb= lay[0].bottom.size(); // number of bottom/top points

    /* Angle of the half wing from X-Y plane */
    real alfa = (180 - dihedralDeg) * PI/360;

    unsigned int j=0, k=0;
    bool flag=false;  // to check if top of sail is reached

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    */
    int t1[MAX_PANELS], t2[MAX_PANELS];

    /* define point ip for intersections */
    CPoint3d ip;

    /* define seamV as the horizontal vector*/
    CVector3d seamV(-1,0,0);
    CSubSpace seamL;

    /* create variables for the development and edge corrections */
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    vector<CVector3d> deviation;
    deviation.resize(npb);
    vector<CVector3d> deviaPrev;
    deviaPrev.resize(npb);
    
    /* Other edge hem width */
    real luffHemW = hemsW; 
    // real luffInnerHemW, footInnerHemW;

    /* create variable to monitor excess over cloth width */
    real CC=0, x=0, y=0;

    /* seam 0 is on the foot of the sail ending at the clew */
    p1[0] = tack; // initialised at tack point
    p2[0] = clew;
    t1[0] = 1;
    t2[0] = 4;    // type=4=leech intersection

    /** Position the seams starting from the centre of the wing (foot) */
    for (npanel = 1; npanel < MAX_PANELS-1; npanel++)
    {
        p2[npanel] = p2[npanel-1] + (clothW-seamW)/(seamV*leechVP) * leechV.unit();
        t2[npanel] = 4; // type2=4=leech intersection for all horizontally cut panels
        seamL = CSubSpace3d::line(p2[npanel], seamV);

        if (CVector3d(p2[npanel]-peak)*leechV > 0)  // we are above peak, stop last panel
        {
            flag = true;
            p2[npanel] = peak;
            // check on which side of the sail the previous point p1 is located
            if ( t1[npanel-1] < 2 )
            { // previous seam on foot
                p1[npanel] = head;
                t1[npanel] = 2; // set on luff
                // left points on foot-tack-luff
                lay[npanel-1].left.fill(p1[npanel-1], tack, p1[npanel]);
                for (k = 0; k < npl/2; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( FOOT_EDGE, lay[npanel-1].left[k], seamV);
                for (k = npl/2 +1; k < npl; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
            }
            else if (t1[npanel-1] == 2)
            { // left points on luff
                p1[npanel] = head;
                t1[npanel] = 2;
                lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k = 0; k < npl; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
            }
            else
            { // left points on gaff
                p1[npanel] = p1[npanel-1];
                t1[npanel] = 3;
                lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k = 0; k < npl; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], seamV);
            }

            // fill right points on leech
            lay[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
            for (k = 0; k < npl; k++)
                lay[npanel-1].right[k] = EdgeIntersect( LEECH_EDGE, lay[npanel-1].right[k], seamV);

            // fill bottom points
            lay[npanel-1].bottom.fill(lay[npanel-1].left[0], lay[npanel-1].right[0]);

            // fill top  points
            lay[npanel-1].top.fill(lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1]);

            // move all top points of top panel to gaff curve
            for (k = 1; k < npb-1; k++)
                lay[npanel-1].top[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].top[k], CVector3d (head.y()-peak.y(),peak.x()-head.x(),0));
            // end peak panel
        }
        else // normal panel  
        {   
            /* find position of luff/seam intersection relative to tack and head */
            if ( seamL.intersect(luffLine).getdim() == 0 )
                ip = seamL.intersect(luffLine).getp();
            else throw layout_error("CSailWorker::LayoutWing -1 : intersection of seam and luff is not a point!");

            if ( CVector3d( ip - tack ) * luffV <= 0 )
            {   // seam intersects foot
                if ( seamL.intersect(footLine).getdim() == 0 )
                    p1[npanel] = seamL.intersect(footLine).getp();
                else throw layout_error("CSailWorker::LayoutWing -2 : intersection of seam and foot is not a point!");

                t1[npanel] = 1;  // 1=foot type of intersection

                if ( npanel == 1 )
                {   // set lower edge to start at same point p1
                    p1[0] = p1[npanel];
                    t1[0] = 1;
                }
            }
            else if ( CVector3d(ip - head) * luffV > 0 )
            {   // seam intersects gaff
                if ( seamL.intersect(gaffLine).getdim() == 0 )
                    p1[npanel] = seamL.intersect(gaffLine).getp();
                else throw layout_error("CSailWorker::LayoutWing -3 : intersection of seam and foot is not a point!");

                t1[npanel] = 3;  // 3=gaff type of intersection
            }
            else
            {
                // seam intersects luff
                p1[npanel] = ip;
                t1[npanel] = 2;  // luff
                if ( npanel == 1 )
                {
                    // force seam 0 to start at the tack
                    p1[0] = tack;
                    t1[0] = 2;
                }
            }

            /* We now add the intermediate points on all sides of the normal panel  */

            /* Below is the code for the left side depending
            *  on t1 for the top side and bottom side
            */
            if ( t1[npanel-1] == 1 && t1[npanel] == 1 )
            {
                // full foot
                lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k = 0; k < npl; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( FOOT_EDGE, lay[npanel-1].left[k], seamV);
            }
            else if ( t1[npanel-1] == 2 && t1[npanel] == 2 )
            {
                // full luff
                lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k = 0; k < npl; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
            }
            else if ( t1[npanel-1] == 3 && t1[npanel] == 3 )
            {
                // full gaff
                lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k = 0; k < npl; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], seamV);
            }
            else if ( t1[npanel-1] == 1 && t1[npanel] == 2 )
            {
                // foot-tack-luff
                lay[npanel-1].left.fill(p1[npanel-1], tack, p1[npanel]);
                for (k = 0; k < npl/2; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( FOOT_EDGE, lay[npanel-1].left[k], seamV);
                for (k = npl/2 +1; k < npl; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
            }
            else if ( t1[npanel-1] == 2 && t1[npanel] == 3 )
            {
                // luff-head-gaff
                lay[npanel-1].left.fill(p1[npanel-1], head, p1[npanel]);
                for (k = 0; k < npl / 2; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], seamV);
                for (k = npl/2 +1; k < npl; k++)
                    lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], seamV);
            } // end IF ELSE for left side

            /* Below is the code for the intermediate points of the right side
            *  which are all on the leech for a crosscut layout.
            */
            lay[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
            for (k = 0; k < npl; k++)
                lay[npanel-1].right[k] = EdgeIntersect( LEECH_EDGE, lay[npanel-1].right[k], seamV);

            /* Below is the code for the intermediate points of the top and bottom sides.
            *  The first point is identical to the last point of the left side
            *  The last point is identical to the last point of the right side
            */
            lay[npanel-1].top.fill(lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1]);
            lay[npanel-1].bottom.fill(lay[npanel-1].left[0], lay[npanel-1].right[0]);

            /* Below is the code for the intermediate points of the bottom side of first panel  */
            if ( npanel == 1 )
            { // move bottom side of first panel to foot curve
                for (k = 1; k < npb -1; k++)
                    lay[0].bottom[k] = EdgeIntersect( FOOT_EDGE, lay[0].bottom[k], CVector3d(0,-1,0));
            }
        } // end else normal panel ///////////////// */

        /* Go over all the points and calculate their z */
        lay[npanel-1] = Zpanel(lay[npanel-1]);

        /* Rotate the panel by the dihedral angle */
        lay[npanel-1] = lay[npanel-1].rotate(tack , CMatrix::rot3d(0 , alfa) );

        /* If it is the first panel, then cut the foot to tack level */
        if ( npanel == 1 )
        {
            for (k=0; k < npb-1; k++)
                lay[0].bottom[k].y() = tack.y();

            lay[0].left[0].y() = tack.y();
            lay[0].right[0].y() = tack.y();
        }

        /* Develop the current panel */
        if ( npanel == 1 )
        {
            dev[npanel-1] = lay[npanel-1].develop(ALIGN_TOP);
        }
        else
        {
            dev[npanel-1] = lay[npanel-1].develop(ALIGN_BOTTOM);
            // add deviation of previous panel top edge to bottom edge
            for (k = 1; k < npb-1; k ++)
                dev[npanel-1].bottom[k] = dev[npanel-1].bottom[k] + deviaPrev[k];
        }

        /* Now we compute and store the deviation of top edge of
        *   the developed panel and straighten this top edge
        *   except if this is the top panel
        */
        if ( flag == false )
        {
            vb= CMatrix::rot3d(2,PI/2)*CVector3d(dev[npanel-1].top[npb-1] -dev[npanel-1].top[0]).unit();
            for (k = 1; k < npb-1; k ++)
            {
                vk= CVector3d (dev[npanel-1].top[k] - dev[npanel-1].top[0]);
                v= vb * -(vk*vb);
                deviation[k] = v;
                dev[npanel-1].top[k] = dev[npanel-1].top[k] + deviation[k];
            }
        }

        /* Add the seam and hems allowance to wing horizontal layout */
        if ( npanel == 1 )
            dev[npanel-1].add6Hems( luffHemW, luffHemW, seamW, leechHemW, leechHemW, footHemW );
        else if ( flag == true )
            dev[npanel-1].add6Hems( hemsW, hemsW, hemsW, leechHemW, leechHemW, 0 );
        else if ( t1[npanel-1] == 3 && t1[npanel] == 3 )
            dev[npanel-1].add6Hems( hemsW, hemsW, seamW, leechHemW, leechHemW, 0 );
        else if ( t1[npanel-1] == 2 && t1[npanel] == 3 )
            dev[npanel-1].add6Hems( luffHemW, hemsW, seamW, leechHemW, leechHemW, 0 );
        else
            dev[npanel-1].add6Hems( luffHemW, luffHemW, seamW, leechHemW, leechHemW, 0 );
        
        /* Reset the previous panel deviation to the current panel */
        deviaPrev = deviation;

        /* Reposition the developed panel such that
        *  bottom minimum is Y=0 AND most left is X=0
        */
        dev[npanel-1] = dev[npanel-1].reframe(LOW_LEFT);

        /* check if peak has been reached to break off */
        if ( flag == true )
            break;
    } /* loop FOR next seam ///////////// */

    if ( npanel == (MAX_PANELS/2 -1) )
        throw layout_error("CSailWorker::LayoutWing -4 : got to MAX_PANELS without reaching head, do increase cloth width ");

    /** Create the symetrical panels */
    np = npanel;
    for (j = 0; j < np +1; j++)
    {
        npanel++;
        lay[npanel] = lay[j];
        dev[npanel] = dev[j];

        for (k = 0 ; k < npb ; k++)
        {
            lay[npanel].top[k].y() = -lay[npanel].top[k].y();
            lay[npanel].bottom[k].y() = -lay[npanel].bottom[k].y();
        }
        for (k = 0 ; k < npl ; k++)
        {
            lay[npanel].left[k].y() = -lay[npanel].left[k].y();
            lay[npanel].right[k].y() = -lay[npanel].right[k].y();
        }
    }

    /* Copy the sails for display */
    CPanelGroup sail( 2 * npanel +1 );

    for (j = 0; j < npanel; j++)
        sail[j] = lay[j];

    /* Copy the developed sail */
    flatsail = CPanelGroup( 2 * npanel +1);

    for (j = 0; j < npanel; j++)
    {
        flatsail[j] = dev[j];
    }

    /** Prepare the displays version of the developed sail */
    dispsail = flatsail;

    for (j = 1; j < npanel; j++)
    {
        top = dispsail[j-1].top[0];
        bot = dispsail[j].bottom[0];

        // rotation to align bottom of panel to top of previous panel
        x = dispsail[j-1].top[npb-1].x()-top.x();
        y = dispsail[j-1].top[npb-1].y()-top.y();
        CC= atan2(y,x);
        dispsail[j] = dispsail[j].rotate(bot,CMatrix::rot3d(2,CC));

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = CVector3d ( top - CPoint3d(0,0,0) );
        v.x() = v.x() - bot.x();
        v.y() = v.y() + 2*seamW + 25;  // adding offset to separate panels vertically

        dispsail[j] = dispsail[j] + v;
    }

    return sail;
} /* end layoutWing = cross cut or horizontal //////////////////// */



/** Creates a radial cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine alias Sailcuter
 */
CPanelGroup CSailWorker::LayoutRadial( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    unsigned int h = 0, j = 0, k = 0, a = 1, b = 1;
    unsigned int ngLuff = nbLuffGores;  // limit of luff gores
    unsigned int ng1 = 0;   // number of central panels
    //bool flag=false;
    real x = 0, xm = 0, xp = 0, ym = 0;

    CPoint3d pt0, pt1, pt2, pt3, pt4, ptCentre, ptFoot;  // points

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* create number of panels */
    unsigned int npanel = 1;
    unsigned int npl = lay[0].right.size();  // number of right/left points
    unsigned int npb = lay[0].bottom.size();  // number of bottom/top points
    unsigned int nps[10];   // number of panels per sections

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    */
    // int t1[MAX_PANELS], t2[MAX_PANELS];

    /* create variables for the development and edge corrections */
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    vector<CVector3d> deviation;
    deviation.resize(npb);

    /* Other edge hem width */
    // real footHemW = hemsW;
    // real luffHemW = hemsW; 
    // real luffInnerHemW, footInnerHemW;

    /* Create arrays of points at horizontal seams ends (10 maximum) */
    CPoint3d luffH[10];    // point at the luff end of the horizontal seam
    CPoint3d leechH[10];   // point at the leech end of the horizontal seam
    CSubSpace seamH[10];   // corresponding seam lines
    CSubSpace seamL;  // a seam line

    h = 0;
    luffH[h] = tack;
    leechH[h] = clew;
    seamH[h] = footLine;

    for (h = 1; h < nbSections; h++)
    {
        pt1 = tack + luffV * (real(h) / nbSections );
        pt2 = clew + leechV * (real(h) / nbSections );
        luffH[h] = EdgeIntersect( LUFF_EDGE,  pt1, CVector3d( pt1 - pt2 ) );
        leechH[h] = EdgeIntersect( LEECH_EDGE,  pt2, CVector3d( pt2 - pt1) );
        seamH[h] = CSubSpace3d::line(luffH[h], CVector3d( leechH[h] - luffH[h] ));
    }

    h = nbSections; // one more horizontal line than nbSections
    luffH[h] = head;
    leechH[h] = peak;
    seamH[h] = gaffLine;

    /*  Create arrays of points on luff and leech catenaries
    *   Luff and leech catenaries are the lines going from each side
    *     of the middle panel of the gaff to the tack and clew.
    *   They separate the surface of the sail in 3 zones:
    *     the luff zone, the central zone and the leech zone.
    *   The luff and leech zone have the same number of radial
    *     panels from top to bottom while in the central zone
    *     the number of panels increase by one at each section.
    */
    CPoint3d luffCatenary[11] , leechCatenary[11];

    // top point on luff catenary
    pt0 = head + gaffV * ( real(ngLuff) / real(nbGores) );
    luffCatenary[nbSections] = pt0;

    // top point on leech catenary
    pt0 = head + gaffV * ( real(ngLuff+1) / real(nbGores) );
    leechCatenary[nbSections] = pt0; // top of leech catenary

    // other points on both catenaries
    for (h = nbSections-1 ; h > 0 ; h--)
    {
        pt0 = tack + footV*(real(h) / real(nbSections))*(real(ngLuff) / real(nbGores));
        seamL = CSubSpace3d::line(luffCatenary[h+1], CVector3d(pt0 - luffCatenary[h+1]));
        luffCatenary[h] = seamH[h].intersect(seamL).getp();

        pt0 = clew - footV*(real(h) / real(nbSections))*(real(nbGores-ngLuff) / real(nbGores));
        seamL = CSubSpace3d::line(leechCatenary[h+1], CVector3d(pt0 - leechCatenary[h+1]));
        leechCatenary[h] = seamH[h].intersect(seamL).getp();
    }

    h = 0;
    luffCatenary[h] = tack;
    leechCatenary[h] = clew;

    /* Now we cut the radial panels
    *  Panels are oriented with lower and upper edges on vertical catenary
    *  Bottom side is on luff side and top side is on leech side
    *  Left side is at the top of each section
    *  p1 p2 are the end point of bottom side with 
    *  p1 = top of horizontal section, p2 = bottom of horizontal section
    *  The seams between sections are twice the normal seam width
    */
    npanel = 0;
    ng1 = 0;  // initialise number of central panels

    /** Lay the panels from the top section downward to the foot section. */
    for (h = nbSections; h > 0; h--)
    {
        nps[h] = 0;  // counter of panels in current section
        /* Cutting the luff side panels 
         *  which are left of the catenary separating the vertical zones
         */
        for (j = 1; j <= ngLuff; j++)
        {
            if ( j == 1 )
            { // place bottom end points on luff catenary
                pt1 = luffH[h];
                pt2 = luffH[h-1];

                lay[npanel].bottom.fill( pt1, pt2 );

                for (k = 1 ; k < npb -1 ; k++)
                    lay[npanel].bottom[k] = EdgeIntersect( LUFF_EDGE, lay[npanel].bottom[k] , CVector3d( luffH[h-1] - leechH[h-1] ) );
            }
            else
            { // copy previous points
                pt1 = pt3;
                pt2= pt4;
                lay[npanel].bottom.fill( pt1, pt2 );
            }

            // we compute the top end points
            pt3 = luffH[h] + CVector3d( luffCatenary[h] - luffH[h] ) * (real(j) / ngLuff);
            if ( h == nbSections )
                pt3 = EdgeIntersect( GAFF_EDGE,  pt3, gaffVP );

            pt4 = luffH[h-1] + CVector3d( luffCatenary[h-1] - luffH[h-1] ) * (real(j) / ngLuff);

            // we fill the other edges
            lay[npanel].top.fill( pt3 , pt4 );
            lay[npanel].left.fill( pt1 , pt3 );
            lay[npanel].right.fill( pt2 , pt4 );

            if ( h == nbSections )
            {
                for (k = 1; k < npl -1; k++)
                    lay[npanel].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel].left[k] , gaffVP );
            }

            // We compute Z
            lay[npanel] = Zpanel(lay[npanel]);

            // We develop the current panel
            dev[npanel] = lay[npanel].develop(ALIGN_TOP);


            // We add the seams and hems allowance
            if ( h == nbSections )
            { 
                if ( j == 1 )
                    dev[npanel].addHems(hemsW, seamW, 0, hemsW);
                else
                    dev[npanel].addHems(hemsW, seamW, 0, 0);
            }
            else if ( j == 1 )
                dev[npanel].addHems(2*seamW, seamW, 0, hemsW);
            else
                dev[npanel].addHems(2*seamW, seamW, 0, 0);

            nps[h]++;
            npanel++;
        }

        /*  Cut the middle panels which are between the 
         *  vertical catenaries luff and leech sides. */
        if ( h > 1 )
        {
            ng1++;  // we add one more central panel than section above
            a = int(floor(real(ng1) / 2) );
            b = int(ceil(real(ng1) / 2) );

            // we initialise the bottom end points
            pt3 = luffCatenary[h];
            if ( h == nbSections )
                pt3 = EdgeIntersect( GAFF_EDGE,  pt3 , gaffVP );

            pt4 = luffCatenary[h-1];

            // we now compute the other points
            for (j = 1; j <= ng1; j++)
            {
                pt1 = pt3;
                pt2 = pt4;
                pt4 = luffCatenary[h-1] + CVector3d( leechCatenary[h-1] - luffCatenary[h-1] ) * (real(j) / ng1);

                if ( ng1 < 4 )
                {
                    pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / ng1);
                    if ( h == nbSections )
                        pt3 = EdgeIntersect( GAFF_EDGE,  pt3 , gaffVP );
                }
                else
                {
                    if ( j < a )  // before middle point
                    {
                        pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / (ng1-1));
                    }
                    else if ( j > b )  // after middle point
                    {
                        pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j-1) / (ng1-1));
                    }
                    else
                    {
                        pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / ng1);
                    }
                }

                // we fill the 4 edges
                lay[npanel].left.fill( pt1 , pt3 );
                lay[npanel].right.fill( pt2 , pt4 );
                lay[npanel].bottom.fill( pt1 , pt2 );
                lay[npanel].top.fill( pt3 , pt4 );

                if ( h == nbSections )
                {
                    for (k = 1; k < npl-1; k++)
                        lay[npanel].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel].left[k] , gaffVP );
                }

                // Now we go over all the points and calculate their z 
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if ( h == nbSections )
                    dev[npanel].addHems(hemsW, seamW, 0, 0);
                else
                    dev[npanel].addHems(2*seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }
        }
        else  // h = 1 middle panels of bottom section
        {
            // luff side central
            for (j = 1; j <= (ng1 / 2); j++)
            {
                pt1 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j-1) / ng1);
                pt2 = tack;

                pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / ng1);
                pt4 = pt2;

                ptCentre = pt3; // memorise last point on horizontal seam 1

                lay[npanel].left.fill(pt1 , pt3);
                lay[npanel].right.fill(pt2 , pt4);
                lay[npanel].bottom.fill(pt1 , pt2);
                lay[npanel].top.fill(pt3 , pt4);

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams allowance
                dev[npanel].addHems(2*seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // luff side lower central
            pt0 = ( luffCatenary[nbSections] + leechCatenary[nbSections] ) * 0.5;
            ptFoot = EdgeIntersect( FOOT_EDGE,  ptCentre , CVector3d(ptCentre - pt0) );
            a = int( (CVector3d (ptFoot - ptCentre).norm()) / (clothW) );
            if (a < 2)
                a = 2;

            for (j = 1 ; j <= a ; j++)
            {   // from center to foot
                pt1 = ptCentre + CVector3d (ptFoot - ptCentre) * (real(j-1) / a);
                pt2 = tack;
                pt3 = ptCentre + CVector3d (ptFoot - ptCentre) * (real(j) / a);
                pt4 = pt2;

                lay[npanel].left.fill( pt1 , pt3 );
                lay[npanel].right.fill( pt2 , pt4 );
                lay[npanel].bottom.fill( pt1 , pt2 );
                lay[npanel].top.fill( pt3 , pt4 );

                if ( j == a )
                {
                    for (k = 0; k < lay[npanel].top.size(); k++)
                        lay[npanel].top[k] = EdgeIntersect(FOOT_EDGE, lay[npanel].top[k], CVector3d(ptFoot - ptCentre));
                }

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if ( j == a )
                    dev[npanel].addHems(seamW, footHemW, 0, 0);
                else
                    dev[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // leech side lower central
            for (j = 1; j <= a; j++)
            {   // from foot to center
                pt1 = ptFoot + CVector3d ( ptCentre - ptFoot ) * (real(j-1) / a );
                pt2 = clew;
                pt3 = ptFoot + CVector3d ( ptCentre - ptFoot ) * (real(j) / a);
                pt4 = pt2;

                lay[npanel].left.fill( pt1, pt3 );
                lay[npanel].right.fill( pt2, pt4 );
                lay[npanel].bottom.fill( pt1, pt2 );
                lay[npanel].top.fill( pt3, pt4 );

                if ( j == 1 )
                {
                    for (k = 0; k < lay[npanel].bottom.size(); k++)
                        lay[npanel].bottom[k] = EdgeIntersect(FOOT_EDGE, lay[npanel].bottom[k], CVector3d(ptFoot - ptCentre));
                }

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if ( j == 1 )
                    dev[npanel].addHems(0, seamW, 0, footHemW);
                else
                    dev[npanel].addHems(0, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // leech side central
            for (j = ng1/2 +1 ; j <= ng1 ; j++)
            {
                pt1 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j-1) / ng1);
                pt2 = clew ;

                pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / ng1);
                pt4 = pt2;

                lay[npanel].left.fill( pt1, pt3 );
                lay[npanel].right.fill( pt2, pt4 );
                lay[npanel].bottom.fill( pt1, pt2 );
                lay[npanel].top.fill( pt3, pt4 );

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams allowance
                dev[npanel].addHems(2*seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }
        }

        /* Cut the leech side panels */
        for (j = 1; j < (nbGores - ngLuff); j++)
        {
            pt1 = leechCatenary[h] + CVector3d( leechH[h] - leechCatenary[h] ) * (real(j-1) / (nbGores - ngLuff -1) );
            if ( h == nbSections )
                pt1 = EdgeIntersect( GAFF_EDGE,  pt1, gaffVP );

            pt2 = leechCatenary[h-1] + CVector3d( leechH[h-1] - leechCatenary[h-1] ) * (real(j-1) / (nbGores - ngLuff -1) );

            pt3 = leechCatenary[h] + CVector3d( leechH[h] - leechCatenary[h] ) * (real(j) / (nbGores - ngLuff -1) );
            if ( h == nbSections )
                pt3 = EdgeIntersect( GAFF_EDGE,  pt3, gaffVP );

            pt4 = leechCatenary[h-1] + CVector3d( leechH[h-1] - leechCatenary[h-1] ) * (real(j) / (nbGores - ngLuff -1) );

            lay[npanel].bottom.fill( pt1, pt2 );
            lay[npanel].top.fill( pt3, pt4 );
            lay[npanel].left.fill( pt1, pt3 );
            lay[npanel].right.fill( pt2, pt4 );

            if ( h == nbSections )
            {
                for (k = 1; k < npl -1; k++)
                    lay[npanel].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel].left[k] , gaffVP );
            }

            if ( j == nbGores - ngLuff -1)
            {
                for (k = 1; k < npb -1; k++)
                    lay[npanel].top[k] = EdgeIntersect( LEECH_EDGE, lay[npanel].top[k] , CVector3d(luffH[h-1]-leechH[h-1]) );
            }

            // we compute Z
            lay[npanel] = Zpanel(lay[npanel]);

            // We develop the current panel
            dev[npanel] = lay[npanel].develop(ALIGN_TOP);

            // We add the seams and hems allowance
            if ( h == nbSections )
            {   if ( j == nbGores - ngLuff -1 )
                    dev[npanel].addHems(hemsW, leechHemW, 0, 0);
                else
                    dev[npanel].addHems(hemsW, seamW, 0, 0);
            }
            else if (j == nbGores - ngLuff -1)
                dev[npanel].addHems(2*seamW, leechHemW, 0, 0);
            else
                dev[npanel].addHems(2*seamW, seamW, 0, 0);

            nps[h]++;
            npanel++;
        }
    }

    /* Copy 3d lay into 3d sail */
    CPanelGroup sail(npanel);
    for (j = 0 ; j < npanel ; j ++)
        sail[j] = lay[j];

    /* Copy from temporary developed sail into flatsail */
    flatsail = CPanelGroup(npanel);

    for (j = 0; j < npanel; j++)
        flatsail[j] = dev[j];

    /* Prepare the displays version of the developed sail */
    dispsail = flatsail;
    j = 0, h = nbSections, k = 0;
    v = CVector3d(0,0,0);
    xp = 0;
    xm = 0;
    ym = 0;

    for (j = 0; j < npanel; j++)
    {
        if ( k == nps[h] )
        {
            // decrement section and offset x
            h--;
            k = 0;
            xp = xm + 2*seamW +20; // offset for next section
            v.x() = xp;
            v.y() = 0;
        }

        // translation v to stack panel above previous panel
        dispsail[j] = dispsail[j] + v;

        CRect3d pRect = dispsail[j].boundingRect();
        ym = pRect.height();
        v.y() += ym + 2*seamW +20; // adding offset to separate next panel vertically

        x = pRect.max.x();
        if ( x > xm )
            xm = x;
        k++;
    }
    
    return sail;
} /* end layout radial cut ///////////// */



/** Creates a triradial cut sail.  //// NOT USED ////
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine alias Sailcuter
 */
CPanelGroup CSailWorker::LayoutTriRadial( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    unsigned int h=0, j=0, k=0, a=1, b=1;
    unsigned int ngLuff = nbGores/2;  // limit of luff gores
    unsigned int ng1=0;   // number of central panels
    //bool flag=false;
    real x=0, xm=0, xp=0, ym=0;

    CPoint3d pt0, pt1, pt2, pt3, pt4, ptCentre, ptFoot;  // points

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* create number of panels */
    unsigned int npanel=1;
    unsigned int npl= lay[0].right.size();  // number of right/left points
    unsigned int npb= lay[0].bottom.size();  // number of bottom/top points
    unsigned int nps[10];   // number of panels per sections

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    */
    // int t1[MAX_PANELS], t2[MAX_PANELS];

    /* create variables for the development and edge corrections */
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    vector<CVector3d> deviation;
    deviation.resize(npb);

    /* Create arrays of points at horizontal seams ends 10 maximum */
    CPoint3d luffH[10];    // point at the luff end of the horizontal seam
    CPoint3d leechH[10];   // point at the leech end of the horizontal seam
    CSubSpace seamH[10];   // corresponding seam lines
    CSubSpace seamL;  // a seam line

    for (h=0; h<nbSections; h++)  // one more horizontal line than nbSections
    {
        pt1 = tack + luffV * (real(h) / nbSections );
        pt2 = clew + leechV * (real(h) / nbSections );
        luffH[h] = EdgeIntersect( LUFF_EDGE,  pt1, CVector3d(pt1-pt2) );
        leechH[h] = EdgeIntersect( LEECH_EDGE,  pt2, CVector3d(pt2-pt1) );
        seamH[h] = CSubSpace3d::line(luffH[h], CVector3d(leechH[h] - luffH[h]));
    }

    h= nbSections;
    luffH[h] = head;
    leechH[h] = peak;
    seamH[h] = gaffLine;

    /*  Create arrays of points on luff and leech catenaries
    *   Luff and leech catenaries are the lines going from each side
    *     of the middle panel of the gaff to the tack and clew.
    *   They separate the surface of the sail in 3 zones:
    *     the luff zone, the central zone and the leech zone.
    *   The luff and leech zone have the same number of radial
    *     panels from top to bottom while in the central zone
    *     the number of panels increase by one at each section.
    */
    CPoint3d luffCatenary[11], leechCatenary[11];
    pt0 = head + gaffV * ( real(ngLuff) / real(nbGores) );

    luffCatenary[nbSections] = pt0; // top of luff catenary
    pt0 = head + gaffV * ( real(ngLuff+1) / real(nbGores) );
    leechCatenary[nbSections] = pt0; // top of leech catenary

    for (h = nbSections-1; h > 0; h--)
    {
        pt0 = tack + footV*(real(h) / real(2* nbSections));
        seamL = CSubSpace3d::line(luffCatenary[h+1], CVector3d(pt0 - luffCatenary[h+1]));
        luffCatenary[h] = seamH[h].intersect(seamL).getp();

        pt0 = clew - footV*(real(h) / real(2* nbSections));
        seamL = CSubSpace3d::line(leechCatenary[h+1], CVector3d(pt0 - leechCatenary[h+1]));
        leechCatenary[h] = seamH[h].intersect(seamL).getp();
    }

    h = 0;
    luffCatenary[h] = tack;
    leechCatenary[h] = clew;

    /* Now cutting the radial panels.
    *  Panels are oriented with lower edge vertical toward luff
    *  and left side at the top of each section
    */
    npanel = 0;
    ng1 = 0; // initialise number of central panels

    for (h = nbSections; h>0; h--)
    {
        /** Lay the panels from top section downward */
        nps[h]=0; // counter of panels in current section
        /* cutting the luff side panels */
        for (j=1; j<=ngLuff; j++)
        {
            if (j == 1)
            {
                pt1 = luffH[h];
                pt2 = luffH[h-1];
                lay[npanel].bottom.fill(pt1 , pt2);
                for (k=1; k<npb-1; k++)
                    lay[npanel].bottom[k] = EdgeIntersect( LUFF_EDGE, lay[npanel].bottom[k], CVector3d(luffH[h-1]-leechH[h-1]) );
            }
            else
            {
                pt1 = pt3;
                pt2= pt4;
                lay[npanel].bottom.fill(pt1 , pt2);
            }

            pt3 = luffH[h] + CVector3d(luffCatenary[h]-luffH[h]) * (real(j)/ngLuff);
            if (h == nbSections)
                pt3 = EdgeIntersect( GAFF_EDGE,  pt3, gaffVP );
            pt4 = luffH[h-1] + CVector3d(luffCatenary[h-1]-luffH[h-1]) * (real(j)/ngLuff);

            lay[npanel].top.fill(pt3, pt4);
            lay[npanel].left.fill(pt1 , pt3);
            lay[npanel].right.fill(pt2 , pt4);

            if (h == nbSections)
            {
                for (k=1; k<npl-1; k++)
                    lay[npanel].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel].left[k], gaffVP );
            }

            // compute Z
            lay[npanel] = Zpanel(lay[npanel]);

            // We develop the current panel
            dev[npanel] = lay[npanel].develop(ALIGN_TOP);


            // We add the seams and hems allowance
            if (h == nbSections && j == 1)
                dev[npanel].addHems(hemsW, seamW, 0, hemsW);
            else if (j == 1)
                dev[npanel].addHems(seamW, seamW, 0, hemsW);
            else
                dev[npanel].addHems(seamW, seamW, 0, 0);

            nps[h]++;
            npanel++;
        }

        /* cutting the middle panels */
        if ( h > 1 )
        {
            ng1++; // add one more central panel than section above
            a = int(floor(real(ng1)/2));
            b = int(ceil(real(ng1)/2));
            pt3 = luffCatenary[h];
            if (h == nbSections)
                pt3 = EdgeIntersect( GAFF_EDGE,  pt3, gaffVP );
            pt4 = luffCatenary[h-1];
            for (j=1; j<=ng1; j++)
            {
                pt1 = pt3;
                pt2 = pt4;
                pt4 = luffCatenary[h-1] + CVector3d(leechCatenary[h-1]-luffCatenary[h-1])*(real(j)/ng1);
                if (ng1 < 4)
                {
                    pt3 = luffCatenary[h] + CVector3d(leechCatenary[h]-luffCatenary[h])*(real(j)/ng1);
                    if (h == nbSections)
                        pt3 = EdgeIntersect( GAFF_EDGE,  pt3, gaffVP );
                }
                else
                {
                    if (j < a)
                    {
                        pt3 = luffCatenary[h] + CVector3d(leechCatenary[h]-luffCatenary[h])*(real(j)/(ng1-1));
                    }
                    else if (j > b )
                    {
                        pt3 = luffCatenary[h] + CVector3d(leechCatenary[h]-luffCatenary[h])*(real(j-1)/(ng1-1));
                    }
                    else
                    {
                        pt3 = luffCatenary[h] + CVector3d(leechCatenary[h]-luffCatenary[h])*(real(j)/ng1);
                    }
                }

                lay[npanel].left.fill(pt1 , pt3);
                lay[npanel].right.fill(pt2 , pt4);
                lay[npanel].bottom.fill(pt1 , pt2);
                lay[npanel].top.fill(pt3 , pt4);

                if (h == nbSections)
                {
                    for (k=1; k<npl-1; k++)
                        lay[npanel].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel].left[k], gaffVP );
                }

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if (h == nbSections)
                    dev[npanel].addHems(hemsW, seamW, 0, 0);
                else
                    dev[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }
        }
        else
        {
            // bottom section h = 1
            // luff side central
            for (j=1; j<=ng1/2; j++)
            {
                pt1 = luffCatenary[h] + CVector3d(leechCatenary[h]-luffCatenary[h])*(real(j-1)/ng1);
                pt2 = tack;

                pt3 = luffCatenary[h] + CVector3d(leechCatenary[h]-luffCatenary[h])*(real(j)/ng1);
                pt4 = pt2;

                ptCentre = pt3; // memorise last point on horizontal seam 1

                lay[npanel].left.fill(pt1 , pt3);
                lay[npanel].right.fill(pt2 , pt4);
                lay[npanel].bottom.fill(pt1 , pt2);
                lay[npanel].top.fill(pt3 , pt4);

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams allowance
                dev[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // luff side lower central
            pt0=(luffCatenary[nbSections]+leechCatenary[nbSections])*0.5;
            ptFoot = EdgeIntersect( FOOT_EDGE,  ptCentre , CVector3d(ptCentre-pt0) );
            a = int( (CVector3d(ptFoot-ptCentre).norm()) / (clothW) );
            if (a < 2)
                a = 2;

            for (j=1; j<=a; j++)
            {
                pt1 = ptCentre + CVector3d (ptFoot - ptCentre)*(real(j-1)/a);
                pt2 = tack;
                pt3 = ptCentre + CVector3d (ptFoot - ptCentre)*(real(j)/a);
                pt4 = pt2;

                lay[npanel].left.fill(pt1 , pt3);
                lay[npanel].right.fill(pt2 , pt4);
                lay[npanel].bottom.fill(pt1 , pt2);
                lay[npanel].top.fill(pt3 , pt4);

                if (j == a)
                {
                    for (k = 0; k < lay[npanel].top.size(); k++)
                        lay[npanel].top[k] = EdgeIntersect(FOOT_EDGE, lay[npanel].top[k], footVP);
                }

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if (j == a)
                    dev[npanel].addHems(seamW, hemsW, 0, 0);
                else
                    dev[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // leech side lower central
            for (j=1; j<=a; j++)
            {
                pt1 = ptFoot + CVector3d (ptCentre - ptFoot)*(real(j-1)/(a));
                pt2 = clew;
                pt3 = ptFoot + CVector3d (ptCentre - ptFoot)*(real(j)/(a));
                pt4 = pt2;

                lay[npanel].left.fill(pt1 , pt3);
                lay[npanel].right.fill(pt2 , pt4);
                lay[npanel].bottom.fill(pt1 , pt2);
                lay[npanel].top.fill(pt3 , pt4);

                if (j == 1)
                {
                    for (k = 0; k < lay[npanel].bottom.size(); k++)
                        lay[npanel].bottom[k] = EdgeIntersect(FOOT_EDGE, lay[npanel].bottom[k], footVP);
                }

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if (j == 1)
                    dev[npanel].addHems(0, seamW, 0, hemsW);
                else
                    dev[npanel].addHems(0, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // leech side central
            for (j=ng1/2+1; j<=ng1; j++)
            {
                pt1 = luffCatenary[h] + CVector3d(leechCatenary[h]-luffCatenary[h])*(real(j-1)/ng1);
                pt2 = clew ;

                pt3 = luffCatenary[h] + CVector3d(leechCatenary[h]-luffCatenary[h])*(real(j)/ng1);
                pt4 = pt2;

                lay[npanel].left.fill(pt1 , pt3);
                lay[npanel].right.fill(pt2 , pt4);
                lay[npanel].bottom.fill(pt1 , pt2);
                lay[npanel].top.fill(pt3 , pt4);

                // compute Z
                lay[npanel] = Zpanel(lay[npanel]);

                // We develop the current panel
                dev[npanel] = lay[npanel].develop(ALIGN_TOP);

                // We add the seams allowance
                dev[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }
        }

        /* Cutting the leech side panels */
        for (j=1; j<nbGores-ngLuff; j++)
        {
            pt1 = leechCatenary[h] + CVector3d(leechH[h]-leechCatenary[h]) * (real(j-1)/(nbGores-ngLuff-1) );
            if (h == nbSections)
                pt1 = EdgeIntersect( GAFF_EDGE,  pt1, gaffVP );
            pt2 = leechCatenary[h-1] + CVector3d(leechH[h-1]-leechCatenary[h-1]) * (real(j-1)/(nbGores-ngLuff-1) );
            pt3 = leechCatenary[h] + CVector3d(leechH[h]-leechCatenary[h]) * (real(j)/(nbGores-ngLuff-1) );
            if (h == nbSections)
                pt3 = EdgeIntersect( GAFF_EDGE,  pt3, gaffVP );
            pt4 = leechCatenary[h-1] + CVector3d(leechH[h-1]-leechCatenary[h-1]) * (real(j)/(nbGores-ngLuff-1) );

            lay[npanel].bottom.fill(pt1 , pt2);
            lay[npanel].top.fill(pt3 , pt4);
            lay[npanel].left.fill(pt1 , pt3);
            lay[npanel].right.fill(pt2 , pt4);

            if (h == nbSections)
            {
                for (k=1; k<npl-1; k++)
                    lay[npanel].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel].left[k], gaffVP );
            }

            if (j == nbGores-ngLuff-1)
            {
                for (k=1; k<npb-1; k++)
                    lay[npanel].top[k] = EdgeIntersect( LEECH_EDGE, lay[npanel].top[k], CVector3d(luffH[h-1]-leechH[h-1]) );
            }

            // compute Z
            lay[npanel] = Zpanel(lay[npanel]);

            // We develop the current panel
            dev[npanel] = lay[npanel].develop(ALIGN_TOP);

            // We add the seams and hems allowance
            if ( h == nbSections && j == nbGores-ngLuff-1)
                dev[npanel].addHems(hemsW, leechHemW, 0, 0);
            else if (j == nbGores-ngLuff-1)
                dev[npanel].addHems(seamW, leechHemW, 0, 0);
            else
                dev[npanel].addHems(seamW, seamW, 0, 0);

            nps[h]++;
            npanel++;
        }
    }

    /* Copy 3d sail lay into sail */
    CPanelGroup sail(npanel);
    for (j = 0; j < npanel; j ++)
        sail[j] = lay[j];

    /* Copy from temporary developed sail into flatsail */
    flatsail = CPanelGroup(npanel);

    for (j=0; j < npanel; j++)
        flatsail[j] = dev[j];

    /* Prepare the displays version of the developed sail */
    dispsail = flatsail;
    j=0, h=nbSections, k=0;
    v = CVector3d(0,0,0);
    xp = 0;
    xm = 0;
    ym = 0;

    for (j=0; j < npanel; j++)
    {
        if (k == nps[h])
        {
            // decrement section and offset x
            h--;
            k = 0;
            xp = xm + 2*seamW +20; // offset for next section
            v.x() = xp;
            v.y() = 0;
        }

        // translation v to stack panel above previous panel
        dispsail[j] = dispsail[j] + v;

        CRect3d pRect = dispsail[j].boundingRect();
        ym = pRect.height();
        v.y() += ym+ 2*seamW +20; // adding offset to separate next panel vertically

        x = pRect.max.x();
        if (x > xm)
            xm = x;
        k++;
    }
    
    return sail;
} /* end LayoutTriRadial cut /////////  NOT USED /////////// */


/** Creates a mitre cut sail with panels perpendicular to Leech and Foot.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine alias Sailcuter
 */
CPanelGroup CSailWorker::LayoutMitre( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
     *   located on the straight edge of the sail (no round)
     */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* create number of panels */
    unsigned int npanelFoot = 1, npanel = 1;
    unsigned int npl= lay[0].right.size();  // number of right/left points
    unsigned int npb= lay[0].bottom.size(); // number of bottom/top points

    unsigned int j = 0, k = 0, cnt = 0;
    bool flag = false;

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    *  t=5 seam intersect mitre
    */
    int t1[MAX_PANELS], t2[MAX_PANELS];

    /* define point ip for intersections */
    CPoint3d ip;
    /* seam Line */
    CSubSpace seamL; 

    /* create variables for the development and edge corrections */
    CPoint3d pt(0, 0, 0); // test point
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    vector<CVector3d> deviation;
    deviation.resize(npb);
    vector<CVector3d> deviaPrev;
    deviaPrev.resize(npb);
    
    /** Mitre Hem Width is set at twice the Seam Width. */
    real mitreHemW = 2 * seamW; 
    
    /* Other edge hem width */
    real luffHemW = hemsW; 
    // real luffInnerHemW, footInnerHemW;

    /* seam 0 is on the foot of the sail ending at the clew */
    p1[0] = clew; // initialised at clew point
    p2[0] = clew;
    t1[0] = 1;
    t2[0] = 5;    // type=5=mitre intersection

    /** First Cut the foot panels perpendicular to the foot, 
     *  starting at the clew and moving toward the tack. */
     
    for ( npanel = 1 ; npanel < (MAX_PANELS / 2) -1 ; npanel++ )
    {
        real exb = 0; // total correction
        real exc = 0; // current excess of width
        cnt = 0;

        /* Loop for optimising the seam position to fit cloth width */
        do
        {
            cnt++;
            // move base point along the foot
            pt = p1[npanel-1] - (clothW - seamW - exb) * footV.unit();
            seamL = CSubSpace3d::line( pt , footVP );
            p1[npanel] = seamL.intersect(footLine).getp();
            t1[npanel] = 1; // type 1=foot intersection 

            if ( p1[npanel].x() <= tack.x() )
            { // last panel
                p1[npanel] = tack;
                if ( t2[npanel-1] == 5 )
                {
                    p2[npanel] = mitreLuffPt;
                    t2[npanel] = 5;
                }
                else
                {
                    p2[npanel] = tack;
                    t2[npanel] = 2;
                }
                flag = true; // set to get out of FOR
            }
            else
            { // normal panel
                p2[npanel] = seamL.intersect(mitreLine).getp();
                if ( CVector3d(p2[npanel] - mitreLuffPt) * mitreV > EPS )
                {
                    t2[npanel] = 2;
                    p2[npanel] = seamL.intersect(luffLine).getp();
                }
                else
                {
                    t2[npanel] = 5;
                }
            }

            // fill right side points
            if ( t2[npanel-1] == 2 && t2[npanel] == 2 )
            { 
                lay[npanel-1].right.fill(p2[npanel-1] , p2[npanel]);
                for (k=0; k<npl; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].right[k], footVP);
            }
            else if ( (t2[npanel-1] == 5) && (t2[npanel] == 2) )
            { 
                lay[npanel-1].right.fill(p2[npanel-1] , mitreLuffPt, p2[npanel]);

                for (k = npl/2 +1 ; k<npl ; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].right[k], footVP);
            }
            else
            { // cout <<  "CSailWorker::LayoutMitre full mitre" << endl;
                lay[npanel-1].right.fill(p2[npanel-1], p2[npanel]);
            }

            // fill left side points which are all on foot
            lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
            for (k = 0 ; k < npl ; k++)
                lay[npanel-1].left[k] = EdgeIntersect( FOOT_EDGE, lay[npanel-1].left[k], footVP);

            // fill the intermediate points of bottom side
            lay[npanel-1].bottom.fill(lay[npanel-1].left[0] , lay[npanel-1].right[0]);

            // fill the intermediate points of top side on seam
            lay[npanel-1].top.fill(lay[npanel-1].left[npl-1] , lay[npanel-1].right[npl-1]);
            if ( flag == true && t2[npanel] == 5 )
            {
                for (k = 0 ; k < npb ; k++)
                    lay[npanel-1].top[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].top[k] , footV);
            }

            /* Go over all the points of the foot panel and calculate their z */
            lay[npanel-1] = Zpanel(lay[npanel-1]);

            /* Now we develop the current foot panel */
            if ( npanel == 1 )
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_TOP);
            else
            {
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for (k = 1 ; k < npb-1 ; k++)
                    dev[npanel-1].bottom[k] = dev[npanel-1].bottom[k] + deviaPrev[k];
            }

            /* Compute the deviation of top edge of developed panel
             *   and straighten this top edge except if this is the top panel flag==true */
            if ( flag == false )
            {
                vb= CMatrix::rot3d(2,PI/2)*CVector3d(dev[npanel-1].top[npb-1] -dev[npanel-1].top[0]).unit();
                for (k = 1 ; k < npb-1 ; k++)
                {
                    vk = CVector3d (dev[npanel-1].top[k] - dev[npanel-1].top[0]);
                    v = vb * -(vk * vb);
                    deviation[k] = v;
                    dev[npanel-1].top[k] = dev[npanel-1].top[k] + deviation[k];
                }
            }

            /* Add the seam and hems allowance to the foot panels */
            if ( flag == true )
                dev[npanel-1].add6Hems( footHemW, footHemW, luffHemW, 0, 0, 0 );
            else {
                if ( t2[npanel-1] == 2 && t2[npanel] == 2 )
                    dev[npanel-1].add6Hems( footHemW, footHemW, seamW, luffHemW, luffHemW, 0 );
                else if ( t2[npanel-1] == 5 && t2[npanel] < 5 )
                    dev[npanel-1].add6Hems( footHemW, footHemW, seamW, luffHemW, 0, 0 );
                else
                    dev[npanel-1].add6Hems( footHemW, footHemW, seamW, 0, 0, 0 );
                
            }
            /* Check the width of developed foot panel */
            exc = dev[npanel-1].boundingRect().height() - clothW;

            /* Sum previous correction + 80% of current excess of width + 1mm */
            exb += 0.8 * exc + 1;
        }
        while ( exc > 0 && cnt < 9 );
        /* loop as long the excess of width is positive AND counter < 9 */

        deviaPrev = deviation;

        /* Now we reposition the developed panel such that bottom left is X=0 Y=0 */
        dev[npanel-1] = dev[npanel-1].reframe(LOW_LEFT);

        /* check if peak has been reached to break off */
        if ( flag == true )
            break;
    } /* Loop FOR next foot panel seam /////////// */

    /* Store the number of panels in foot */
    npanelFoot = npanel;
    if (npanel == MAX_PANELS/2 -1)
        throw layout_error("CSailWorker::LayoutMitre -5 : Foot got to MAX_PANELS/2 without reaching tack, do increase cloth width.");

    p1[npanel] = clew; // re-initialising at clew point
    p2[npanel] = clew;
    t1[npanel] = 5;    // type=5=mitre intersection
    t2[npanel] = 4;    // type=4=leech intersection

    /** Then lay the leech panels perpendicular to the leech, 
     *  starting from clew and moving upwards to the peak.  */
    
    flag = false;
    for ( npanel = npanel +1 ; npanel < MAX_PANELS -1 ; npanel++ )
    {
        real exb = 0; // total correction
        real exc = 0; // current excess of width
        cnt = 0; // reset counter of iterations

        /* Loop for optimising the seam position to fit cloth width */
        do
        {
            cnt++;
            p2[npanel] = p2[npanel-1] + (clothW -seamW -exb) * leechV.unit();
            t2[npanel] = 4; // type2=4=leech intersection for all horizontally cut panels
            seamL = CSubSpace3d::line(p2[npanel] , leechVP);

            if (CVector3d(p2[npanel]-peak) * leechV >= 0)
            { // we are above peak, stop last panel
                flag = true;
                p2[npanel] = peak;

                // check on which side of the sail the previous point p1 is located
                if ( t1[npanel-1] == 5 )
                { // previous seam on mitre
                    p1[npanel] = head;
                    t1[npanel] = 2; // set on luff
                    // left points on mitrePt-head
                    lay[npanel-1].left.fill(p1[npanel-1] , mitreLuffPt , p1[npanel]);

                    for (k = npl/2 +1 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], leechVP);
                }
                else if ( t1[npanel-1] == 2 )
                { // left points on luff
                    p1[npanel] = head;
                    t1[npanel] = 2;
                    lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], leechVP);
                }
                else
                { // left points on gaff
                    p1[npanel] = peak;
                    t1[npanel] = 3;
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0 ; k < npl-1 ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k], leechVP);
                }

                // fill right points on leech
                lay[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
                for (k = 0 ; k < npl ; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LEECH_EDGE, lay[npanel-1].right[k], leechVP);

                // fill bottom points
                lay[npanel-1].bottom.fill(lay[npanel-1].left[0], lay[npanel-1].right[0]);

                // fill top  points
                lay[npanel-1].top.fill(lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1]);

                if ( t1[npanel] < 3 ) // move all top points to gaff curve
                {
                    for (k = 1 ; k < npb-1 ; k++)
                        lay[npanel-1].top[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].top[k], CVector3d (head.y()-peak.y() , peak.x()-head.x() , 0));
                }
            }
            else // normal panel //////
            { 
                /* find position of luff/seam intersection relative to tack and head
                 * the case when the intersection is not a point needs to be handled */
                if ( seamL.intersect(mitreLine).getdim() == 0 )
                    ip = seamL.intersect(mitreLine).getp();
                else throw layout_error("CSailWorker::LayoutMitre leech-2 : intersection of seam and mitre is not a point!");

                if ( CVector3d(ip - mitreLuffPt) * mitreV <= 0 )
                { // seam intersects mitre
                    p1[npanel] = ip;
                    t1[npanel] = 5;  // mitre intersection
                }
                else
                {
                    ip = seamL.intersect(luffLine).getp();

                    if ( CVector3d( ip - head) * luffV > 0 )
                    { // seam intersects gaff
                        p1[npanel] = seamL.intersect(gaffLine).getp();
                        t1[npanel] = 3;  // gaff type of intersection
                    }
                    else
                    { // seam intersects luff
                        p1[npanel] = ip;
                        t1[npanel] = 2;  // luff
                    }
                }

                /* We now add intermediate points on all sides of the normal panel */

                /* Below is the code for the left side depending
                 *  on t1 for the top side and bottom side
                 */
                if ( t1[npanel-1] == 2 && t1[npanel] == 2 )
                { // full luff
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], leechVP);
                }
                else if ( t1[npanel-1] == 3 && t1[npanel] == 3 )
                { // full gaff
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k = 0 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k] , leechVP);
                }
                else if ( (t1[npanel-1] == 2) && (t1[npanel] == 3) )
                { // luff-head-gaff
                    lay[npanel-1].left.fill(p1[npanel-1] , head , p1[npanel]);
                    for (k = 0 ; k < npl/2 ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k] , leechVP);
                    for (k = npl/2 +1 ; k < npl ; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( GAFF_EDGE, lay[npanel-1].left[k] , leechVP);
                } 
                else if ( (t1[npanel-1] == 5) && (t1[npanel] == 2) )
                { // mitre-luff
                    lay[npanel-1].left.fill(p1[npanel-1] , mitreLuffPt , p1[npanel]);
                    for (k = 0 ; k < npl/2 ; k++)
                        lay[npanel-1].left[k] = MitreIntersect(lay[npanel-1].left[k] , leechVP);
                    for (k=npl/2+1; k<npl; k++)
                        lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k] , leechVP);
                }
                else
                { // full mitre
                    lay[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                }

                // Fill the intermediate points of the right side
                lay[npanel-1].right.fill(p2[npanel-1] , p2[npanel]);
                for (k = 0 ; k < npl ; k++)
                    lay[npanel-1].right[k] = EdgeIntersect( LEECH_EDGE, lay[npanel-1].right[k] , leechVP);

                // Fill the intermediate points of the top side of the leech panel
                lay[npanel-1].top.fill(lay[npanel-1].left[npl-1] , lay[npanel-1].right[npl-1]);

                // Fill the intermediate points of the bottom side of leech panel
                lay[npanel-1].bottom.fill(lay[npanel-1].left[0] , lay[npanel-1].right[0]);

            } /* end else normal panel //////////////  */

            /* Now we go over all the points of the leech panel and calculate their z */
            lay[npanel-1] = Zpanel(lay[npanel-1]);

            /* Now we develop the current leech panel */
            if ( npanel == npanelFoot +1 )
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_TOP);
            else
            {
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for (k = 1 ; k < npb -1 ; k++)
                    dev[npanel-1].bottom[k] = dev[npanel-1].bottom[k] + deviaPrev[k];
            }

            /* Now we compute the deviation of top edge of the developed leech panel
             *   and straighten this top edge except if this is the top panel */
            if ( flag == false )
            {
                vb = CMatrix::rot3d(2,PI/2)*CVector3d(dev[npanel-1].top[npb-1] -dev[npanel-1].top[0]).unit();
                for (k = 1 ; k < npb -1 ; k++)
                {
                    vk = CVector3d (dev[npanel-1].top[k] - dev[npanel-1].top[0]);
                    v = vb * -(vk * vb);
                    deviation[k] = v;
                    dev[npanel-1].top[k] = dev[npanel-1].top[k] + deviation[k];
                }
            }

            /* Add the seam and hems allowance to leech panels */
            if ( flag == true )
                dev[npanel-1].add6Hems( mitreHemW, mitreHemW, seamW, leechHemW, leechHemW, 0 );
            else {
                if ( t1[npanel-1] < 5 && t1[npanel] < 5 )
                    dev[npanel-1].add6Hems( hemsW, hemsW, seamW, leechHemW, leechHemW, 0 );
                else if ( t1[npanel-1] == 5 && t1[npanel] < 5 )
                    dev[npanel-1].add6Hems( mitreHemW, hemsW, seamW, leechHemW, leechHemW, 0 );
                else
                     dev[npanel-1].add6Hems( mitreHemW, mitreHemW, seamW, leechHemW, leechHemW, 0 );
           }
            /* Check the width of developed leech panel */
            exc = dev[npanel-1].boundingRect().height() - clothW;

            /* Sum previous correction + 80% of current excess of width + 1mm */
            exb += 0.8 * exc + 1;
        }
        while ( exc > 0 && cnt < 9 );
        /* loop as long the excess of width is positive AND counter < 9 */

        for (k = 0 ; k < npb ; k++)
            deviaPrev[k] = deviation[k];

        /* Now we reposition the developed panel such that bottom left is X=0 Y=0 */
        dev[npanel-1] = dev[npanel-1].reframe(LOW_LEFT);

        /* check if peak has been reached to break off */
        if ( flag == true )
            break;
    } /* loop FOR next seam of leech panels */

    if ( npanel == MAX_PANELS-1 )
        throw layout_error("CSailWorker::LayoutMitre -3 : Leech got to MAX_PANELS without reaching head, you need to increase cloth width ");

    /* Copy the sails for display */
    CPanelGroup sail(npanel);
    for (j = 0; j < npanel; j++)
        sail[j] = lay[j];

    /* Copy the developed sail into flatsail */
    flatsail = CPanelGroup(npanel);

    for (j = 0 ; j < npanel ; j++)
        flatsail[j] = dev[j];

    /* Prepare the displays version of the developed sail */
    dispsail = flatsail;

    for (j = 1 ; j < npanel ; j++)
    {
        top = dispsail[j-1].top[0];
        bot = dispsail[j].bottom[0];

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = top;
        v.x() -= bot.x();
        v.y() += 2 * seamW +10; // adding offset to separate panels vertically

        dispsail[j] = dispsail[j] + v;
    }

    return sail;
} /* end layoutMitre cut //////////////////// */


/** Creates a Mitre 2 cut sail with panels Parallel to Leech & Foot.
 *
 * @param flatsail the CPanelGroup object that will hold 
 *   the developed sail
 * @param dispsail the CPanelGroup object that will hold 
 *   the display version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine alias Sailcuter
 * Adapted by Peter G. Meuse
 */

CPanelGroup CSailWorker::LayoutMitre2( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
     *   located on the straight edge of the sail (no round)
     */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* create number of panels */
    unsigned int npanelFoot = 1, npanel = 1;
    unsigned int npl = lay[0].right.size();  // number of right/left points
    unsigned int npb = lay[0].bottom.size(); // number of bottom/top points

    unsigned int j = 0, k = 0, cnt = 0, cntMax = 9;
    bool flag = false;

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    *  t=5 seam intersect mitre
    */
    int t1[MAX_PANELS], t2[MAX_PANELS];
    
    /* define point ip for intersections */
    CPoint3d ip;
    /* define seam line */
    CSubSpace seamL; 

    /* create variables for the development and edge corrections */
    CPoint3d pt(0, 0, 0); // test point
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    vector<CVector3d> deviation;
    deviation.resize(npb);
    vector<CVector3d> deviaPrev;
    deviaPrev.resize(npb);

    /* create variable to monitor excess over cloth width */
    real exb = 0, exc = 0;

    /** Mitre Hem Width is set at twice the Seam Width. */
    real mitreHemW = 2 * seamW; 
    
    /* Other edge hem */
    real luffHemW = hemsW; 
    // real luffInnerHemW, footInnerHemW;
    
    /** Start by laying the foot panels parallel to the foot,
     *  from the foot upward toward the mitre */
    
    p1[0] = tack; // initialised at clew point
    p2[0] = clew; // initialised at tack point
    t1[0] = 2;    // type 2=luff intersection.
    t2[0] = 5;    // type 5=mitre intersection

    for ( npanel = 1; npanel < MAX_PANELS/2-1; npanel++ )
    {
        // cout << " ----- FOR LOOP foot npanel = " << npanel << endl;
        exb = 0;
        exc = 0;
        cnt = 0; // reset counter
        
        /* begin the loop for optimising the seam position to fit cloth width */
        do {
            cnt++;
            // move base point perpendicular to foot 
            pt = p1[npanel-1] + (clothW -seamW -exb) * footVP.unit();
            seamL = CSubSpace3d::line( pt, footV );
            
            // find intersection on luff and mitre
            if ( seamL.intersect(luffLine).getdim() == 0 ) {
                p1[npanel] = seamL.intersect(luffLine).getp(); 
                t1[npanel] = 2;
            } else 
                throw layout_error("CSailWorker::LayoutMitre2 foot-a : intersection of seam and luff is not a point!"); // the case when the intersection is not a point needs to be handled 
                
            if ( seamL.intersect(mitreLine).getdim() == 0 ) {
                p2[npanel] = seamL.intersect(mitreLine).getp(); 
                t1[npanel] = 5;
            } else 
                throw layout_error("CSailWorker::LayoutMitre2 foot-b : intersection of seam and mitre is not a point!"); // the case when the intersection is not a point needs to be handled 

            // check if top seam is past luff mitre point
            if ( CVector3d(p2[npanel] - mitreLuffPt) * mitreV > EPS ) {
                // last foot panel 
                p1[npanel] = mitreLuffPt;
                p2[npanel] = mitreLuffPt;
                t2[npanel] = 2;
                flag = true; // to get out of FOR
            }

            // fill right side points which are all on mitre
            lay[npanel-1].right.fill( p2[npanel-1], p2[npanel] );
            for ( k = 0; k < npl; k++ )
                lay[npanel-1].right[k] = MitreIntersect( lay[npanel-1].right[k], footV );

            // fill left side points which are all on luff
            lay[npanel-1].left.fill( p1[npanel-1], p1[npanel] );
            for ( k = 0; k < npl; k++ )
                lay[npanel-1].left[k] = EdgeIntersect( LUFF_EDGE, lay[npanel-1].left[k], footV );

            // fill bottom points
            lay[npanel-1].bottom.fill( lay[npanel-1].left[0], lay[npanel-1].right[0] );
            if ( npanel == 1 ) {
                // bottom is on the foot
                for ( k = 0; k < npb; k++ )
                    lay[npanel-1].bottom[k] = EdgeIntersect( FOOT_EDGE,lay[npanel-1].bottom[k], footVP );
            }

            // fill top side points on seam
            lay[npanel-1].top.fill( lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1] );
            
            /* Now we go over all the points and calculate their Z */
            lay[npanel-1] = Zpanel(lay[npanel-1]);

            /* Now we develop the current panel */
            if ( npanel == 1 )
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_TOP);
            else {
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for( k = 1; k < npb-1; k ++)
                    dev[npanel-1].bottom[k] = dev[npanel-1].bottom[k] + deviaPrev[k];
            }

            /* Now we compute the deviation of top edge of developed panel
            *   and straighten this top edge except if this is the top panel
            */
            if ( flag == false ) {
                vb = CMatrix::rot3d(2, PI/2) * CVector3d(dev[npanel-1].top[npb-1] -dev[npanel-1].top[0]).unit();
                for( k = 1; k < npb-1; k ++) {
                    vk= CVector3d (dev[npanel-1].top[k] - dev[npanel-1].top[0]);
                    v = vb * -(vk*vb);
                    deviation[k] = v;
                    dev[npanel-1].top[k] = dev[npanel-1].top[k] + deviation[k];
                }
            }

            /* Now we add the seam and hems allowance to the foot panels */
            if ( npanel == 1 )    // Foot panel 
                dev[npanel-1].add6Hems( luffHemW, luffHemW, seamW, mitreHemW, mitreHemW, footHemW ); 
            else if ( flag == true )   // last foot section panel 
               dev[npanel-1].add6Hems( luffHemW, luffHemW, luffHemW, mitreHemW, mitreHemW, 0 ); 
            else    // Normal Panel 
                dev[npanel-1].add6Hems( luffHemW, luffHemW, seamW, mitreHemW, mitreHemW, 0 ); 
            
            /* Now we check the width of developed panel */
            exc = dev[npanel-1].boundingRect().height() - clothW; // current excess of width
            exb = exb + (0.8 * exc) +1; // sum previous correction + 80% of current excess of width +1mm
            
            if (cnt == cntMax) 
                cout << "CSailcorker::LayoutMitre2  Foot panel " << npanel << " may be wider than cloth by " << exc << "mm." << endl;
        }
        while ( exc > 0 && cnt < cntMax );
        /* Loop DO as long as the excess of width is positive  AND counter <9 */
        
         deviaPrev = deviation;

        /* Reposition the developed panel such that bottom left is X=0 Y=0 */
        dev[npanel-1] = dev[npanel-1].reframe(LOW_LEFT);

        /* Check if peak has been reached to break off */
        if ( flag == true )
            break;
    }  /* Loop FOR next seam */

    /* Store the number of panels in foot */
    npanelFoot = npanel;
    if ( npanelFoot == MAX_PANELS/2 -1 )
        throw layout_error("CSailWorker::LayoutMitre2 : Foot got to MAX_PANELS/2 without reaching Mitre intersection at Luff, do increase cloth width.");
    
    /** Then continue by laying the leech panels parallel to the leech,
     *  from the leech toward the luff intersection with the mitre. */
    
    p1[npanel] = clew; // re-initialising at clew point.
    p2[npanel] = peak; // initialise at the peak.
    t1[npanel] = 5;    // type=5=mitre intersection.
    t2[npanel] = 3;    // type=3=gaff intersection.
    flag = false;

    for (npanel = npanelFoot+1; npanel < MAX_PANELS-1; npanel++) 
    {
        // printf(" ----- FOR panel = %d \n" , npanel);
        exb = 0;
        exc = 0;
        cnt = 0; // reset loop counter

        /* begin the loop for optimising the seam position to fit cloth width */
        do {
            cnt++;
            /* Determine width of Panel Perpendicular to Leech side. */
            pt = p1[npanel-1] + (clothW -seamW -exb) * leechVP.unit();
            seamL = CSubSpace3d::line(pt, leechV);
            
            if ( seamL.intersect(mitreLine).getdim() == 0 ) {
                p1[npanel] = seamL.intersect(mitreLine).getp();
                if ( p1[npanel].x() >= clew.x() ) // point beyong clew
                    p1[npanel] = clew;
                t1[npanel] = 5; // type1=5= mitre intersection vertically cut panels
            }
            else
               throw layout_error("CSailWorker::LayoutMitre2 leech -c : intersection of seam and mitre is not a point!"); // the case when the intersection is not a point needs to be handled 

            // check if top seam is passed luff mitre point
            if ( p1[npanel].x() <= mitreLuffPt.x() ) { // last panel
                p1[npanel] = mitreLuffPt;
                t1[npanel] = 5;   // Type=5=Mitre instersection.
                p2[npanel] = mitreLuffPt;
                t2[npanel] = 2;   // Type=2=Luff intersection.
                flag=true; // to get out of FOR
            }
            else {
                // printf ("normal panel \n");
                 if ( seamL.intersect(gaffLine).getdim() == 0 )
                    p2[npanel] = seamL.intersect(gaffLine).getp();
                else 
		    throw layout_error("CSailWorker::LayoutMitre2 leech -d : intersection of seam and gaff is not a point!");
                /* the case when the intersection is not a point needs to be handled */

                if ( CVector3d(p2[npanel]-head)*gaffV > EPS )
                    t2[npanel] = 3;   // Intersect is on the Gaff.
                else {
                    if ( seamL.intersect(luffLine).getdim() == 0 )
                        p2[npanel] = seamL.intersect(luffLine).getp();
                    else 
			throw layout_error ("CSailWorker::LayoutMitre2 leech -e : intersection of seam and luff is not a point!");
                    /* the case when the intersection is not a point needs to be handled */
                    t2[npanel] = 2;  // Intersect is on the Luff
                }
            }

            // fill right side points
            if ( t2[npanel-1] == 2 && t2[npanel] == 2 ) { 
                 // cout << "CSailWorker::LayoutMitre2 full luff" << endl;
                lay[npanel-1].right.fill(p2[npanel-1], p2[npanel]);
                for ( k = 0; k < npl; k++ )
                    lay[npanel-1].right[k]=EdgeIntersect( LUFF_EDGE, lay[npanel-1].right[k], leechV);
            }
            else if ( t2[npanel-1] == 3 && t2[npanel] == 2 ) {
                // cout << "CSailWorker::LayoutMitre2 gaff-head-luff" << endl;
                lay[npanel-1].right.fill(p2[npanel-1], head, p2[npanel]);

                for ( k = 0; k < npl/2; k++ )
                    lay[npanel-1].right[k] = EdgeIntersect( GAFF_EDGE,lay[npanel-1].right[k], leechV);

                for ( k = npl/2+1; k < npl; k++ )
                    lay[npanel-1].right[k] = EdgeIntersect( LUFF_EDGE,lay[npanel-1].right[k], leechV);
            }
            else {
                // cout << "CSailWorker::LayoutMitre2 full gaff << endl;
                lay[npanel-1].right.fill(p2[npanel-1], p2[npanel]);

                for ( k = 0; k < npl; k++ )
                    lay[npanel-1].right[k] = EdgeIntersect( GAFF_EDGE,lay[npanel-1].right[k], leechV);
            }
            
            // fill left side points which are all on the mitre
            lay[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
            if (p1[npanel-1] == p1[npanel]) { // clew limit
                for ( k = 0; k < npl; k++ )
                    lay[npanel-1].left[k] = lay[npanel-1].left[k];
            }
            else 
                for ( k = 0; k < npl; k++ )
                    lay[npanel-1].left[k] = MitreIntersect(lay[npanel-1].left[k], leechV);

            // fill bottom points
            lay[npanel-1].bottom.fill(lay[npanel-1].left[0], lay[npanel-1].right[0]);
            if ( npanel == npanelFoot +1 ) {  // bottom is on the leech
                for ( k = 0; k < npb; k++ )
                    lay[npanel-1].bottom[k] = EdgeIntersect( LEECH_EDGE,lay[npanel-1].bottom[k], leechVP);
            }

            // fill top side points on seam
            lay[npanel-1].top.fill(lay[npanel-1].left[npl-1], lay[npanel-1].right[npl-1]);
            /* Now we go over all the points and calculate their z */
            lay[npanel-1] = Zpanel(lay[npanel-1]);

            /* Now we develop the current panel */
            if ( npanel == npanelFoot +1 )
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_TOP);
            else {
                dev[npanel-1] = lay[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for( k = 1; k < npb-1; k ++)
                    dev[npanel-1].bottom[k] = dev[npanel-1].bottom[k] + deviaPrev[k];
            }

            /* Now we compute the deviation of top edge of developed panel
            *  and straighten this top edge except if this is the top panel */
            if ( flag == false ) {
                vb = CMatrix::rot3d(2,PI/2) * CVector3d(dev[npanel-1].top[npb-1] -dev[npanel-1].top[0]).unit();
                for( k = 1; k < npb-1; k++ )
                {
                    vk = CVector3d (dev[npanel-1].top[k] - dev[npanel-1].top[0]);
                    v = vb * -(vk*vb);
                    deviation[k] = v;
                    dev[npanel-1].top[k] = dev[npanel-1].top[k] + deviation[k];
                }
            }
            
            /* Now we add the seam and hems allowance to the leech panels*/
            if ( npanel == npanelFoot +1 )
                dev[npanel-1].add6Hems( 0, 0, seamW, hemsW, hemsW, leechHemW ); 
            else if( flag == true )
                dev[npanel-1].add6Hems( 0, 0, luffHemW, hemsW, hemsW, 0 ); 
            else 
                dev[npanel-1].add6Hems( 0, 0, seamW, hemsW, hemsW, 0 );
            
            /* Now we check the width of developed panel and correct for the next loop */
            exc = dev[npanel-1].boundingRect().height() - clothW;

            exb = exb + (0.8 * exc) + 1; // sum previous correction + 80% of current excess of width +1mm
            
            if (cnt == cntMax) 
                cout << "CSailWorker::LayoutMitre2  Leech panel " << npanel << " may be wider than cloth by " << exc << "mm." << endl;
        }
        while ( exc > 0 && cnt < cntMax );
        /* loop DO as long as the excess of width is positive  AND counter <9 */
        
        deviaPrev = deviation;

        /* Now we reposition the developed panel such that bottom left is X=0 Y=0 */
        dev[npanel-1] = dev[npanel-1].reframe(LOW_LEFT);

        /* check if peak has been reached to break off */
        if ( flag == true )
            break;

    }  /* Loop FOR next seam */
    
    if ( npanel == MAX_PANELS -1 )
        throw layout_error("CSailWorker::LayoutMitre2 -f : MAX_PANELS without reaching Miter Intersect Point at Luff, do increase cloth width.");

    /* Copy the sails for display */
    CPanelGroup sail(npanel);
    for( j = 0; j < npanel; j++ )
        sail[j] = lay[j];

    /* Copy the developed sail into flatsail */
    flatsail = CPanelGroup(npanel);

    for ( j = 0; j < npanel; j++ )
        flatsail[j] = dev[j];

    /*  Prepare the displays version of the developed sail */
    dispsail = flatsail;

    for ( j = 1; j < npanel; j++ )
    {
        top = dispsail[j-1].top[0];
        bot = dispsail[j].bottom[0];

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = top;
        v.x() -= bot.x();
        v.y() += 2*seamW +10; // adding offset to separate panels vertically

        dispsail[j] = dispsail[j] + v;
    }

    return sail;
} /* end LayoutMitre2 ////////////////// */


/** Routine used for computing the cord of the profiles.
 *  Return a 3d point which is the aft intersection of
 *  the horizontal line passing by p1 with the leech.
 *  If the point p1 is above or below the leech segment then
 *  the aft intersection is forced to be on the vertical of the
 *  corresponding peak or clew.
 *
 * @author Robert Laine alias Sailcuter
 */
CPoint3d CSailWorker::AftIntersect( const CPoint3d& pt1 ) const
{
    // real x=0, y=0, z=0; // for debugging only

    CPoint3d pAft = pt1; // aft intersection point is initialised at p1
    CVector3d vH = CVector3d( 1, 0, 0 );

    if ( pt1.y() >= peak.y() )
    {   // rear point above the peak, set output on vertical above peak
        pAft.x() = peak.x();
    }
    else if ( pt1.y() <= clew.y() )
    {   // rear point on or below the clew, set output on vertical below clew
        pAft.x() = clew.x();
    }
    else
    {   // move point on leech which is not straight
        pAft = EdgeIntersect( LEECH_EDGE, pt1, vH );
    }
    // ensure that Z=0
    pAft.z() = 0;
    //
    return pAft;
} 


/** 
 *  Routine for computing the area of the sail taking 
 *  into account the luff and leech round.
 *
 * @author Robert Laine alias Sailcuter
 */
real CSailWorker::Area()
{
    real surface = luffV.cross(footV).norm() / 2;
    surface = surface + leechV.cross(gaffV).norm() / 2;
    surface = surface + .75*(luffL*luffR + footL*footR + leechL*leechR + gaffL*gaffR);

    return ( .01 * floor(surface /10000) );
}


/** 
 *  Routine for computing the diagonal length from head to clew.
 *
 * @author Robert Laine alias Sailcuter
 */
real CSailWorker::Diagonal()
{
    return floor( CVector3d(head-clew).norm() );
}


/** 
 *  Routine for computing the width of the sail at a given
 *  relative height on the leech in accordance with IRC rule.
 *
 * @author Robert Laine alias Sailcuter
 */
real CSailWorker::IRCwidth( const real &HL )
{
    unsigned int i, imax=10;
    real h1=0, h2=0, LL=0, l1=0, w=0;
    CPoint3d p, p1, p2, p3;
    //printf ("IRC height = %f \n", HL);

    /// compute the leech edge length LL
    h1 = 1;
    LL = LeechLength( h1 );
    //printf ("IRC leech length = %f \n", LL);

    /* compute the point on leech for relative length HL */
    h2 = HL;
    //do
    {
        h1 = h2;
        l1 = LeechLength(h1);
        h2 = h1 * (1-(l1 - LL*HL)/(LL*HL));
        //printf (" HL=%f  l1=%f  h1=%f - h2=%f \n", HL, l1, h1, h2);
        h1 = h2;
        l1 = LeechLength(h1);
        h2 = h1 * (1-(l1 - LL*HL)/(LL*HL));
        //printf (" HL=%f  l1=%f  h1=%f - h2=%f \n", HL, l1, h1, h2);
    }
    //while ( fabs(h1-h2)>.001 );

    /* compute the shortest distance to the real luff */
    p1 = clew + leechV*h2;
    p2 = Zpoint(EdgeIntersect( LEECH_EDGE, p1, leechVP));

    p1 = Zpoint(EdgeIntersect( LUFF_EDGE, p2, luffVP));

    w = 0;
    p3 = p1;

    for (i = 1; i <= imax; i++)
    {
        h1 = real(i) / imax;
        p = Zpoint(p1 +CVector3d(p2 - p1) * h1);
        w = w + CVector3d(p - p3).norm();
        p3 = p;
    }
    
    return ( w );
}


/** 
 *  Routine for computing the width of the sail at a given
 *  relative height on the luff and the leech.
 *
 * @author Robert Laine alias Sailcuter
 */
real CSailWorker::SailWidth( const  real  &HL )
{
    unsigned int i, imax=10;
    real h=0, h1=0, h2=0, LeL=0, LuL=0, l1=0, w=0;
    CPoint3d p, p1, p2, p3;
    //printf ("IRC height = %f \n", HL);

    // compute the leech edge length LeL
    h = 1;
    LeL = LeechLength( h );
    //printf ("IOR leech length = %f \n", LeL);

    /* compute the height of point on leech for relative length HL */
    h2 = HL;
    //do
    {
        h = h2;
        l1 = LeechLength( h );
        h2 = h * (1-(l1 - LeL*HL) / (LeL*HL));
        //printf (" HL=%f  l1=%f  h1=%f - h2=%f \n", HL, l1, h, h2);
        h = h2;
        l1 = LeechLength(h);
        h2 = h * (1-(l1 - LeL*HL) / (LeL*HL));
        //printf (" HL=%f  l1=%f  h=%f - h1=%f \n", HL, l1, h, h2);
    }
    //while ( fabs(h1-h2)>.001 );

    /* compute the luff edge length LuL */
    h = 1;
    LuL = LuffLength( h );
    //printf ("IOR luff length = %f \n", LuL);

    /* compute the height of point on luff for relative length HL */
    h1 = HL;
    //do
    {
        h = h1;
        l1 = LuffLength( h );
        h1 = h * (1 - (l1 - LuL*HL) / (LuL*HL) );
        //printf (" HL=%f  l1=%f  h1=%f - h2=%f \n", HL, l1, h, h1);
        h = h1;
        l1 = LuffLength( h );
        h1 = h * (1 - (l1 - LuL*HL) / (LuL*HL) );
        //printf (" HL=%f  l1=%f  h=%f - h1=%f \n", HL, l1, h, h1);
    }
    //while ( fabs(h1-h2)>.001 );


    /* compute the shortest distance real leech to the real luff */
    p = clew + leechV * h2;
    p2 = Zpoint(EdgeIntersect( LEECH_EDGE, p , leechVP) );

    p = tack + luffV * h1;
    p1 = Zpoint(EdgeIntersect( LUFF_EDGE, p , luffVP) );

    w = 0;
    p3 = p1;

    for (i = 1; i <= imax; i++)
    {
        h1 = real(i) / imax;
        p = Zpoint( p1 + CVector3d(p2 - p1) * h1 );
        w = w + CVector3d(p - p3).norm();
        p3 = p;
    }
    return ( w );
}


/** 
 *  Routine for computing the perpendicular width of the sail
 *  from clew to luff, measured perpendicular to the luff.
 *
 * @author Robert Laine alias Sailcuter
 */
real CSailWorker::SailLP( )
{
    unsigned int i, imax=10;
    real h1=0, w=0;
    CPoint3d p, p1, p2, p3;
    CVector3d v;

    p1 = Zpoint( EdgeIntersect( LUFF_EDGE, clew , luffVP) );
    v = CVector3d(clew - p1);
    p2 = p1;

    // compute the distance from the real luff to clew
    for (i = 1 ; i <= imax ; i++)
    {
        h1 = real(i) / imax;
        p =  p1 + v * h1 ;
        p.z() = 0;
        p3 = Zpoint( p );
        w = w + CVector3d(p3 - p2).norm();
        p2 = p3;
    }
    //
    return ( w );
}


/** 
 *  Routine for computing the actual length of the leech edge
 *  up to a given relative heigth on straight leech line.
 *
 * @author Robert Laine alias Sailcuter
 */
real CSailWorker::LeechLength( const real &h )
{
    unsigned int i=0, imax=20;
    CPoint3d p1, p2, p3;
    real l=0, h1=0;

    p1 = clew;

    for (i = 1; i <= imax; i++)
    {
        h1= real(i) / imax;
        p2 = clew + leechV * (h * h1);
        p3 = Zpoint( EdgeIntersect( LEECH_EDGE, p2 , leechVP) );
        l = l + CVector3d(p3 - p1).norm();
        // printf ("step = %f - p2.y = %f - leech length = %f \n", h1, p2.y(), l);
        p1 = p3;
    }
    return ( l );
}


/** 
 *  Routine for computing the actual length of the luff edge
 *  up to a given relative heigth on straight luff line.
 *
 * @author Robert Laine alias Sailcuter
 */
real CSailWorker::LuffLength( const real &h )
{
    unsigned int i=0, imax=20;
    CPoint3d p1=tack, p2, p3;
    real l=0, h1=0;

    for (i = 1 ; i < imax ; i++)
    {
        h1= real(i) / imax;
        p2 = tack + luffV * (h * h1);
        p3 = Zpoint( EdgeIntersect( LUFF_EDGE, p2 , luffVP ) );
        l = l + CVector3d(p3 - p1).norm();
        p1 = p3;
    }
    return ( l );
}


/** 
 *  Routine used for computing the forward end of the cord of the profile.
 *  Return a 3d point which is the forward intersection of
 *  the horizontal line passing by p1 with either foot, luff or gaff.
 *
 * @author Robert Laine alias Sailcuter
 */
CPoint3d CSailWorker::FwdIntersect( const CPoint3d &pt1 ) const
{
    CPoint3d pFwd = pt1; // forward intersection point initialised at p1
    CVector3d vH = CVector3d( 1, 0, 0 );

    if ( pt1.y() <= tack.y() )  // point is at or below tack
    {
        pFwd.x() = tack.x();  // set forward point on vertical below tack
        // pFwd.z() = tack.z();
    }
    else if ( pt1.y() < head.y() ) // forward point is on luff curve
    {
        pFwd = EdgeIntersect( LUFF_EDGE, pt1, vH );
    }
    else if ( pt1.y() == head.y() )  // point exactly at head height
    {
        pFwd.x() = head.x();
        // pFwd.z() = head.z();
    }
    else if ( pt1.y() < peak.y() )
    {   // forward point is on gaff
        pFwd = EdgeIntersect( GAFF_EDGE, pt1, vH );
    }
    else
    { // point is above peak
        pFwd.x() = peak.x(); // set forward point on vertical above peak
        //pFwd.z() = peak.z(); //
    }
    // Ensure Z = 0
    pFwd.z() = 0;
    //
    return pFwd;
} /*  end FwdIntersect //////////////// */


/** 
 *  Routine used for computing the point position on a sail curved edge.
 *  Return a 3D point which is the intersection of the vector v1
 *  passing by pt1 point inside sail area with the Edge curve.
 *
 * @author Robert Laine alias Sailcuter
 */
CPoint3d CSailWorker::EdgeIntersect( const enumEdgeType &Edge, const CPoint3d &pt1, const CVector3d &v1 ) const
{
    if ( v1.norm() <= EPS )
        throw layout_error("CSailWorker::EdgeIntersect : input vector is nul");

    // Input line
    CSubSpace InputLine = CSubSpace3d::line( pt1 , v1 );
    // Edge end points
    CPoint3d pEnd1, pEnd2;
    // Edge Vector
    CVector3d vEdge;
    // Edge perpendicular vector toward outside of sail
    CVector3d vpEdge;
    // Edge round
    real EdgeR = 0;
    // Edge round position from end1 toward end2
    int EdgeRP;

    switch ( Edge )
    {
    case LUFF_EDGE:
        pEnd1 = tack;
        pEnd2 = head;
        EdgeR = luffR;
        EdgeRP = luffRP;
        vEdge = CVector3d( pEnd2 - pEnd1 );
        vpEdge = CMatrix::rot3d(2 , PI/2) * vEdge.unit();
        break;

    case GAFF_EDGE:
        pEnd1 = head;
        pEnd2 = peak;
        EdgeR = gaffR;
        EdgeRP = gaffRP;
        vEdge = CVector3d( pEnd2 - pEnd1 );
        vpEdge = CMatrix::rot3d(2 , PI/2) * vEdge.unit();
        break;

    case FOOT_EDGE:
        pEnd1 = tack;
        pEnd2 = clew;
        EdgeR = footR;
        EdgeRP = footRP;
        vEdge = CVector3d( pEnd2 - pEnd1 );
        vpEdge = CMatrix::rot3d(2 , -PI/2) * vEdge.unit();
        break;

    case LEECH_EDGE:
        pEnd1 = clew;
        pEnd2 = peak;
        EdgeR = leechR;
        EdgeRP = leechRP;
        vEdge = CVector3d( pEnd2 - pEnd1 );
        vpEdge = CMatrix::rot3d(2 , -PI/2) * vEdge.unit();
        break;
    }

    // Edge line
    CSubSpace Line2 = CSubSpace3d::line( pEnd1, vEdge );

    real h1=0, h2=0, d1=0, d2=0;
    CPoint3d p0 , p1 , p2 , p3;
    CVector3d v;

    // find point p0 at intersection of straight edge with input line
    if ( InputLine.intersect( Line2 ).getdim() == 0 ) {
        p0 = InputLine.intersect( Line2 ).getp();
        p2 = p0;    // default output
    }
    else throw layout_error("CSailWorker::EdgeIntersect -1 : intersection with edge is not a point!");

    if ( CVector3d(p0 - pEnd1) * vEdge <= 0 )
        p2 = pEnd1;  // intersection left of edge
    else if ( CVector3d(p0 - pEnd2) * vEdge >= 0 )
        p2 = pEnd2;  // intersection right of edge
    else if ( fabs(EdgeR) > 1 ) {   // intersection is on curved edge
        h1 = CVector3d(p0 - pEnd1).norm() / (vEdge.norm() + EPS); // relative height
        d1 = EdgeR * RoundP(h1 , EdgeRP); // local depth of edge curve
        p1 = p0 + vpEdge * d1;

        // define a line parrallel to edge at distance d1
        Line2 = CSubSpace3d::line( p1 , vEdge );

        // point2 at intersection of input line with parrallel to edge is always valid
        p2 = InputLine.intersect( Line2 ).getp();

        v = CVector3d( p2 - p1);

        if ( v.norm() >= EPS ) {
            // translate point0 on straight edge
            p3 = p0 + v;

            // check if p3 is inside edge
            if ( CVector3d(p3 - pEnd1) * vEdge <= 0 ) // p3 outside left of edge end
                p2 = pEnd1;
            else if ( CVector3d(p3 - pEnd2) * vEdge >= 0 ) // p3 outside right of edge end
                p2 = pEnd2;
            else {   // point is on edge curve
                h2 = CVector3d(p3 - pEnd1).norm() / (vEdge.norm() + EPS);
                d2 = EdgeR * RoundP( h2 , EdgeRP ); // local depth of edge curve
                p2 = p3 + vpEdge * d2;
            }

            v = CVector3d ( p2 - p1 );

            if ( v.norm() <= EPS )
                // keep p1 which is strictly on input line
                p2 = p1;
            else {   // displaced point 2 and p1 are used for Line2
                Line2 = CSubSpace3d::line( p1 , v );
                // compute final intersection point p2
                p2 = InputLine.intersect( Line2 ).getp();
            }
        }
    }
    //
    return p2;
} /* end  EdgeIntersect //////////////////// */


/** 
 *  Routine used for computing the intersection with mitre line.
 *  Return a 3d point which is the intersection of the vector v1
 *  passing by pt1 point with the mitre line.
 *
 * @author: Robert Laine alias Sailcuter
 */
CPoint3d CSailWorker::MitreIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const
{
    if ( v1.norm() <= EPS )
        throw layout_error("CSailWorker::MitreIntersect : input vector is nul");
    // real x=0, y=0, z=0; // for debugging only
    
    /* straight line passing through input point */
    CSubSpace ptv1 = CSubSpace3d::line(pt1 , v1);
    
    CPoint3d p2 = pt1;

    if ( CVector3d(p2 - clew).norm() <= EPS )
        p2 = clew;
    else {
        /* point at intersection of input vector and mitre 
           ckecking if it is inside segment or not is in LayoutMitre */
        if ( ptv1.intersect(mitreLine).getdim() == 0 )
            p2 = ptv1.intersect(mitreLine).getp();
        else 
            throw layout_error("CSailWorker::MitreIntersect -1 : intersection with mitre is not a point!");
    }
    return p2;
} 


/** 
 *  Routine used for computing the Z of a point of the sail.
 *  Return a 3D point which is the input point p1 with its Z modified.
 *
 * @author Robert Laine alias Sailcuter
*/
CPoint3d CSailWorker::Zpoint( const CPoint3d &p1 ) const
{
    CPoint3d p2 = p1;      // p2 will be the returned point with Z added to p1

    real x=0,  z=0, twist=0, pivotX=0, pos=0, cord=1;
    CPoint3d pFwd = FwdIntersect( p1 );   // forward end of the cord
    CPoint3d pAft = AftIntersect( p1 );    // rear end of the cord

    /* computing local cord of the profile */
    cord = CVector3d( pAft - pFwd ).norm();

    /* computing Z from normalised position on profile */
    if ( cord < 1 )   // to avoid division by cord = zero
    {
        pos = 0 ;
        z = 0;
    }
    else   // position on profile
    {
        pos = ( CVector3d( p1 - pFwd ).norm() ) / cord;

        /* computing the relative height on the sail */
        real h1 = (p1.y() - tack.y()) / (peak.y() - tack.y());  // for mould

        /* Now computing actual Z  including twist and sheeting angle
        *  kluff, kleech, depth coefficient used in z1=f(pos, h1) of the
        *  sailmould are interpolated between the respective profile[0..2].
        *  The position of profile[1] is driven by vertpos which is
        *  the vertical position of the middle profile entered in the
        *  left vertical pane of formmouldbase.
        */
        z = cord * mould.interpol( h1 ).z(pos);
    }

    /* computing the twist from the relative height on straight leech */
    real h2 = ( p1.y() - clew.y() ) / leechV.y();

    if ( h2 >= 1 )    // above peak
        h2 = 1;
    else if ( h2 <= 0 )   // below clew
        h2 = 0;

    twist = ( h2 * twistDeg  + sheetDeg ) * PI / 180;

    if ( p1.y() >= head.y() )   // on gaff
        pivotX = head.x();
    else if ( p1.y() <= tack.y() )   // below tack
        pivotX = tack.x();
    else   // on luff
        pivotX = pFwd.x();

    x = p1.x() - pivotX;

    /* applying the twist by rotating the profile around pivotX */
    p2.x() = pivotX + x * cos(twist) - z * sin(twist);
    p2.z() = x * sin(twist) + z * cos(twist);

    return p2;
} /*  end  Zpoint ///////////////// */


/** 
 * Routine used for computing the Z of all the points of a panel.
 * Returns a CPanel with all its points Z's modified.
 * 
 * @author Robert Laine alias Sailcuter
 */
CPanel CSailWorker::Zpanel( const CPanel &p1 ) const
{
    CPanel ret = p1;
    unsigned int k;

    for (k = 0; k < ret.left.size(); k++)
        ret.left[k] = Zpoint(ret.left[k]);

    for (k = 0; k < ret.right.size(); k++)
        ret.right[k] = Zpoint(ret.right[k]);

    for (k = 0; k < ret.top.size(); k++)
        ret.top[k] = Zpoint(ret.top[k]);

    for (k = 0; k < ret.bottom.size(); k++)
        ret.bottom[k] = Zpoint(ret.bottom[k]);

    return ret;
}

