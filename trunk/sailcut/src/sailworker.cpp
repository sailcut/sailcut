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

#include "sailworker.h"
#include "sailcalc.h"

#define MAX_PANELS 210

/*  print debug 
    QString txt;
    txt = "point 1 ----   x=" + QString::number (p1.x() ) + "  y=" + QString::number (p1.y() ) + "  z="+ QString::number (p1.z() );
    qDebug ( txt.toLocal8Bit() );
*/


/** The constructor does some preliminary calculations to set
 *  internal variables.
 */
CSailWorker::CSailWorker(const CSailDef &s) : CSailDef(s)
{
    /* set minimum value for relative height and distance to corners to avoid numerical instabilities */
    minH = 0.000001; 
    
    /* first compute the coordinates of the corner of the sail */
    CVector3d v, v1;
    real x = 0, headstay = 0;

    /* computing the coordinates of the 4 corners of the sail */
    switch (sailType )
    {
    case JIB:
        x = tackY * foreJ / foreI;
        headstay = sqrt(foreI * foreI + foreJ * foreJ);
        rake = foreJ * luffL / headstay;
        tack = CPoint3d( x , tackY , 0 );
        //gaffL=1;
        gaffR = 0;
        gaffRP = 50;
        gaffDeg = atan2( foreI , foreJ ) * 180/PI - 10;
        break;
    default:
        tack = CPoint3d( tackX, tackY, 0 );
        break;
    }

    /// VC++ 6 pre-SP3 bug here if we pass the result of sqrt directly
    /// see : MS knowledgebase 217164)
    real stupid_hack = sqrt(luffL*luffL - rake*rake);
    head = tack + CVector3d(rake, stupid_hack, 0);

    v = CVector3d(0, gaffL, 0);  // initial vector gaff set on vertical
    peak = head + CMatrix::rot3d(2, -asin(rake / luffL) - gaffDeg * PI / 180) * v;

    /* computing triangle tack-peak-clew */
    real aa, b, bb;
    bb = atan2(peak.y() - tack.y() , peak.x() - tack.x() );
    b = CVector3d(peak - tack).norm();
    aa = Atriangle(leechL, b, footL);

    v1 = CVector3d(footL, 0, 0);  // initial foot vector set on horizontal
    if (sailType == WING)
        clew = tack + v1;
    else
        clew = tack + CMatrix::rot3d(2, bb-aa) * v1;
    ////// end of computation of corners of the sail

    /* define useful vectors of edges */
    footV = CVector3d( clew - tack );
    gaffV = CVector3d( peak - head );
    leechV = CVector3d( peak - clew );
    luffV = CVector3d( head - tack );

    /* define usefull vector mitre */
    mitreV = CVector3d( tack - clew ).unit() + leechV.unit();

    /* define the unitary vectors perpendicular to the edges */
    footVP = CMatrix::rot3d(2, PI/2) * footV.unit();
    gaffVP = CMatrix::rot3d(2, PI/2) * gaffV.unit();
    leechVP = CMatrix::rot3d(2, PI/2) * leechV.unit();
    luffVP = CMatrix::rot3d(2, PI/2) * luffV.unit();

    /* define useful straight lines */
    foot = CSubSpace3d::line(tack, footV);
    gaff = CSubSpace3d::line(head, gaffV);
    leech = CSubSpace3d::line(clew, leechV);
    luff = CSubSpace3d::line(tack, luffV);
    mitre = CSubSpace3d::line(clew, mitreV);

    /* define point at intersection of mitre and luff */
    mitrePt = LuffIntersect (clew , mitreV);
    ///
}


/** Make a Sail from its definition.
 */
CPanelGroup CSailWorker::makeSail() const
{
    CPanelGroup flatsail, dispsail;
    return makeSail(flatsail,dispsail);
}


/** Make a Sail from its definition.
 *  This the main routine of all the layout work
 *  The output is a 3D sail, its display and its development
 */
CPanelGroup CSailWorker::makeSail( CPanelGroup &flatsail , CPanelGroup &dispsail) const
{
    CPanelGroup output;

    switch (sailType )
    {
    case WING:
        output = LayoutWing(flatsail , dispsail);
        break;
    default:
        switch (sailCut)
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
        default:
            throw CException("CSailWorker::makeSail : unknown sail cut!");
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


/** Creates a cross cut or horizontal cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine
 */
CPanelGroup CSailWorker::Layout0( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
    *   located on the straight edge of the sail (no round)
    */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS);  // 3D sail
    CPanelGroup dev(MAX_PANELS);  // developed sail

    /* create number of panels */
    unsigned int npanel = 1;
    unsigned int npl = lay.panel[0].right.nbpoints();   // number of right/left points
    unsigned int npb = lay.panel[0].bottom.nbpoints();  // number of bottom/top points

    unsigned int j = 0, k = 0, cnt = 0;
    bool flag = false;  // to check if top of sail is reached

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    */
    int t1[MAX_PANELS], t2[MAX_PANELS];

    /* define point i for intersections */
    CPoint3d i;

    CVector3d seamV(-1, 0, 0);
    CSubSpace seamSP;
    /* define seamV as the vector perpendicular to the leech vector (peak-clew)*/
    if ( sailCut == HORIZONTAL )
        seamV = CVector3d(-1, 0, 0);  // seam orientation for horizontal cut
    else
        seamV = leechVP; // for cross cut

    /* create variables for the development and edge corrections */
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    CVector3d deviation [npb + 1];
    CVector3d deviaPrev [npb + 1];
    for ( k = 0 ; k <= npb ; k++)
    {
        deviation[k] = CVector3d(0, 0, 0); // set all deviations to zero
        deviaPrev[k] = deviation [k];
    }

    /* create variable to monitor excess over cloth width */
    real exb = 0, exc = 0,  ymax = 0;
    real CC = 0, x = 0, y = 0;

    /* seam 0 is on the foot of the sail ending at the clew */
    p1[0] = tack; // initialise seam forward end at tack point
    p2[0] = clew; // initialise seam aft end at clew point
    t1[0] = 1;
    t2[0] = 4;    // type=4=leech intersection

    /** Start laying the panels from the foot upward to the peak */
    for ( npanel = 1 ; npanel < MAX_PANELS -1 ; npanel++)
    {
        exb = 0; // total correction
        exc = 0; // current excess of width
        cnt = 0; // counter of iterations

        do  /// begin the loop for optimising the seam position to fit cloth width */
        {
            cnt++;
            p2[npanel] = p2[npanel-1] + (clothW-seamW-exb)/(seamV*leechVP) * leechV.unit();
            t2[npanel] = 4; // type2 = 4 = leech intersection for all horizontally cut panels
            seamSP = CSubSpace3d::line(p2[npanel], seamV);

            if ( CVector3d(p2[npanel]-peak)*leechV > 0 )  // we are above peak, stop this is last panel
            {
                flag = true;
                p2[npanel] = peak;
                // check on which side of the sail the previous point p1 is located
                if ( t1[npanel-1] < 2 )
                { // previous seam on foot
                    p1[npanel] = head;
                    t1[npanel] = 2; // set on luff
                    // left points on foot-tack-luff
                    lay.panel[npanel-1].left.fill(p1[npanel-1], tack, p1[npanel]);
                    for ( k = 0 ; k < npl / 2 ; k++)
                        lay.panel[npanel-1].left.point[k] = FootIntersect(lay.panel[npanel-1].left.point[k], seamV);
                    for ( k = npl / 2 +1 ; k < npl ; k++)
                        lay.panel[npanel-1].left.point[k] = LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                }
                else if ( t1[npanel-1] == 2 )
                { // left points on luff
                    p1[npanel] = head;
                    t1[npanel] = 2;
                    lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for ( k = 0 ; k < npl ; k++)
                        lay.panel[npanel-1].left.point[k] = LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                }
                else
                { // left points on gaff
                    p1[npanel] = p1[npanel-1];
                    t1[npanel] = 3;
                    lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for ( k = 0 ; k < npl ; k++)
                        lay.panel[npanel-1].left.point[k] = GaffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                }

                // fill right points on leech
                lay.panel[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
                for ( k = 0 ; k < npl ; k++)
                    lay.panel[npanel-1].right.point[k]=LeechIntersect(lay.panel[npanel-1].right.point[k], seamV);

                // fill bottom points
                lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);

                // fill top  points
                lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);

                // move all top points of top panel to gaff curve
                for( k = 1 ; k < npb -1 ; k++)
                    lay.panel[npanel -1].top.point[k] = GaffIntersect(lay.panel[npanel-1].top.point[k], CVector3d (head.y()-peak.y(),peak.x()-head.x(),0));
                ////// end peak panel /////
            }
            else   // normal panel  ///////////////////////////
            {
                /* find position of luff/seam intersection relative to tack and head */
                i = seamSP.intersect(luff).getp();

                /* the case when the intersection is not a point is handled */
                if ( seamSP.intersect(luff).getdim() != 0 )
                    throw CException("CSailDef::Layout0-1 : intersection of seam and luff is not a point!");

                if ( CVector3d(i-tack)*luffV < 0 )
                {   // seam intersects foot
                    p1[npanel] = seamSP.intersect(foot).getp();
                    t1[npanel] =1;  // 1=foot type of intersection

                    if ( npanel == 1 )
                    { // set lower edge to start at same point p1
                        p1[0] = p1[npanel];
                        t1[0] = 1;
                    }
                    /* the case when the intersection is not a point needs to be handled */
                    if ( seamSP.intersect(foot).getdim() != 0 )
                        throw CException("CSailDef::Layout0-2 : intersection of seam and foot is not a point!");
                }
                else if ( CVector3d(i-head)*luffV > 0 )
                {   // seam intersects gaff
                    p1[npanel] = seamSP.intersect(gaff).getp();
                    t1[npanel] = 3;  // 3=gaff type of intersection

                    /* the case when the intersection is not a point needs to be handled */
                    if ( seamSP.intersect(gaff).getdim() != 0 )
                        throw CException("CSailDef::Layout0-3 : intersection of seam and foot is not a point!");
                }
                else
                {   // seam intersects luff
                    p1[npanel] = i;
                    t1[npanel] = 2;  // luff
                    if ( npanel == 1 )
                    {   // force seam 0 to start at the tack
                        p1[0] = tack;
                        t1[0] = 2;
                    }
                }

                /// We now add the intermediate points on all sides of the normal panel  */

                /* Below is the code for the left side depending
                *  on t1 for the top side and bottom side
                */
                if ( t1[npanel-1] == 1  &&  t1[npanel] == 1 )
                {
                    // full foot
                    lay.panel[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for ( k = 0 ; k < npl ; k++)
                        lay.panel[npanel-1].left.point[k] = FootIntersect(lay.panel[npanel-1].left.point[k], seamV);
                }
                else if ( t1[npanel-1] == 2  &&  t1[npanel] == 2 )
                {
                    // full luff
                    lay.panel[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for ( k = 0 ; k < npl ; k++)
                        lay.panel[npanel-1].left.point[k] = LuffIntersect(lay.panel[npanel-1].left.point[k] , seamV);
                }
                else if ( t1[npanel-1] == 3  &&  t1[npanel] == 3 )
                {
                    // full gaff
                    lay.panel[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for ( k = 0 ; k < npl ; k++)
                        lay.panel[npanel-1].left.point[k] = GaffIntersect(lay.panel[npanel-1].left.point[k] , seamV);
                }
                else if ( (t1[npanel-1] == 1) && (t1[npanel] == 2) )
                {
                    // foot-tack-luff
                    lay.panel[npanel-1].left.fill(p1[npanel-1], tack, p1[npanel]);
                    for ( k = 0 ; k < npl / 2 ; k++)
                        lay.panel[npanel-1].left.point[k] = FootIntersect(lay.panel[npanel-1].left.point[k], seamV);
                    for ( k = npl / 2 +1 ; k < npl ; k++)
                        lay.panel[npanel-1].left.point[k] = LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                }
                else if ( (t1[npanel-1] == 2) && (t1[npanel] == 3) )
                {
                    // luff-head-gaff
                    lay.panel[npanel-1].left.fill(p1[npanel-1], head, p1[npanel]);
                    for ( k = 0 ; k < npl/2 ; k++)
                        lay.panel[npanel-1].left.point[k] = LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                    for ( k = npl / 2 +1 ; k < npl ; k++)
                        lay.panel[npanel-1].left.point[k] = GaffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                } // end IF ELSE for left side

                /* Below is the code for the intermediate points of the right side
                *  which are all on the leech for a crosscut layout.
                */
                // first check if upper point is not below lower point 
                if ( CVector3d(p2[npanel]-p2[npanel-1])*leechV < 0) 
                    p2[npanel] = p2[npanel-1];
                
                lay.panel[npanel-1].right.fill(p2[npanel-1] , p2[npanel]);
                for ( k = 0 ; k < npl ; k++)
                    lay.panel[npanel-1].right.point[k] = LeechIntersect(lay.panel[npanel-1].right.point[k], seamV);

                /* Below is the code for the intermediate points of the top and bottom sides.
                *  The first point is identical to the last point of the left side
                *  The last point is identical to the last point of the right side
                */
                lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1] , lay.panel[npanel-1].right.point[npl-1]);
                lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0] , lay.panel[npanel-1].right.point[0]);

                /* Below is the code for the intermediate points of the bottom side of first panel  */
                if ( npanel == 1 )
                { // move bottom side of first panel to foot curve
                    for( k = 1 ; k < npb -1 ; k++)
                        lay.panel[0].bottom.point[k] = FootIntersect(lay.panel[0].bottom.point[k], CVector3d(0,-1,0));
                }
            }  //// end else normal panel ///////////////////////

            /* Now we go over all the points and calculate their z */
            for( k = 0 ; k < npl ; k ++)
                lay.panel[npanel-1].left.point[k] = Zpoint(lay.panel[npanel-1].left.point[k]);
            for( k = 0 ; k < npl ; k ++)
                lay.panel[npanel-1].right.point[k] = Zpoint(lay.panel[npanel-1].right.point[k]);
            for( k = 0 ; k < npb ; k ++)
                lay.panel[npanel-1].top.point[k] = Zpoint(lay.panel[npanel-1].top.point[k]);
            for( k = 0 ; k < npb ; k ++)
                lay.panel[npanel-1].bottom.point[k] = Zpoint(lay.panel[npanel-1].bottom.point[k]);

            /* Now we develop the current panel */
            if ( npanel == 1 )
            {
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_TOP);
            }
            else
            {
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for( k = 1; k < npb-1; k ++)
                    dev.panel[npanel-1].bottom.point[k] = dev.panel[npanel-1].bottom.point[k] + deviaPrev[k];
            }

            /* Now we compute and store the deviation of top edge of
            *   the developed panel and straighten this top edge
            *   except if this is the top panel
            */
            if ( flag == false )
            {
                vb= CMatrix::rot3d(2,PI/2) * CVector3d(dev.panel[npanel-1].top.point[npb-1] - dev.panel[npanel-1].top.point[0]).unit();
                for( k = 1 ; k < npb -1 ; k ++)
                {
                    vk= CVector3d (dev.panel[npanel-1].top.point[k] - dev.panel[npanel-1].top.point[0]);
                    v= vb * -(vk*vb);
                    deviation[k] = v;
                    dev.panel[npanel-1].top.point[k] = dev.panel[npanel-1].top.point[k] + deviation[k];
                }
            }

            /* Add the seam and hems allowance */
            if ( npanel == 1 )
                dev.panel[npanel-1].addHems(hemsW, seamW, leechHemW, hemsW);
            else if ( flag == true )
                dev.panel[npanel-1].addHems(hemsW, hemsW, leechHemW, 0);
            else
                dev.panel[npanel-1].addHems(hemsW, seamW, leechHemW, 0);

            /* Check the width of developed panel */
            ymax = dev.panel[npanel-1].height();
            exc = ymax-clothW;

            exb = exb + .8 * exc +1; // sum previous correction + 80% of current excess of width

            //printf ("P %d count %d exc %f exb %f \n", npanel,cnt,exc,exb); // send to console

        }
        while ( exc > 0  &&  cnt < 9 );
        /* Now loop on DO as long as  the current excess of width is positive
        *   AND the counter is less than 9
        *   AND remaining cloth more than 10%
        */

        ////////////////////////////////////////////
        //printf ("P ----- maxW %f\n", (ymax-ymin));

        for ( k = 0 ; k < npb ; k++)
            deviaPrev[k] = deviation[k];

        /** Now we reposition the developed panel such that
        *  bottom minimum is Y=0 AND left is X=0
        */
        dev.panel[npanel-1] = dev.panel[npanel-1].reframe(LOW_LEFT);

        /// check if peak has been reached to break off */
        if ( flag == true )
            break;
    }  /// loop FOR next seam */
    ////////////////////////////////////////

    if ( npanel == MAX_PANELS-1 )
        throw CException("CSailDef::Layout0 : got to MAX_PANELS without reaching head, do increase cloth width ");

    /** copying the sails for display */
    CPanelGroup sail(npanel);

    for( j = 0 ; j < npanel ; j ++)
        sail.panel[j] = lay.panel[j];

    /** copying the developed sail */
    flatsail = CPanelGroup(npanel);

    for ( j = 0 ; j < npanel ; j++)
    {
        flatsail.panel[j] = dev.panel[j];
    }

    /** preparing the displays version of the developed sail */
    dispsail = flatsail;

    for ( j = 1 ; j < npanel ; j++)
    {
        top = dispsail.panel[j-1].top.point[0];
        bot = dispsail.panel[j].bottom.point[0];

        // rotation to align bottom of panel to top of previous panel
        x = dispsail.panel[j-1].top.point[npb-1].x() - top.x();
        y = dispsail.panel[j-1].top.point[npb-1].y() - top.y();
        CC = atan2(y , x);
        dispsail.panel[j] = dispsail.panel[j].rotate(bot,CMatrix::rot3d(2,CC));

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = CVector3d ( top - CPoint3d(0,0,0) );
        v.x() = v.x() - bot.x();
        v.y() = v.y() + 2 * seamW +20; // adding offset to separate panels vertically

        dispsail.panel[j] = dispsail.panel[j] + v;
    }
    ///
    return sail;
} ///////////// end layout0 = cross cut or horizontal /////////////////////


/** Creates a twist foot cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine
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
    unsigned int npl = lay.panel[0].right.nbpoints();  // number of right/left points
    unsigned int npb = lay.panel[0].bottom.nbpoints(); // number of bottom/top points

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

    /* define point i for intersections */
    CPoint3d i;

    /* define seamV as the vector perpendicular to the leech vector (peak-clew)*/
    CVector3d seamV = CMatrix::rot3d(2,PI/2) * leechV.unit();
    CSubSpace seamSP;
    CVector3d seamVT; // seam twisted
    CSubSpace seamSPT;

    /* create variables for the development and edge corrections */
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    CVector3d deviation [30];
    CVector3d deviaPrev [30];
    for (k=0; k<29; k++)
    {
        deviation[k] = CVector3d(0,0,0); // set all deviations to zero
        deviaPrev[k] = deviation[k];
    }

    /* create variable to monitor excess over cloth width */
    real exb=0, exc=0, ymax=0;
    real CC=0, x=0, y=0;

    /* seam 0 is on the foot of the sail ending at the clew */
    p1[0] = tack; // initialised at tack point
    p2[0] = clew;
    t1[0] = 2;
    t2[0] = 4;    // type=4=leech intersection

    /** Start laying the panels from foot upward to the peak */
    for (npanel = 1; npanel < MAX_PANELS-1; npanel++)
    {
        exb=0;
        exc =0;
        cnt =0;

        /** begin the DO loop for optimising the seam position to fit cloth width */
        do
        {
            cnt++;
            p2[npanel] = p2[npanel-1] + (clothW-seamW-exb)/leechL * leechV;
            t2[npanel] = 4; // type2=4=leech intersection for all horizontally cut panels
            seamSP = CSubSpace3d::line(p2[npanel] , seamV);

            if (CVector3d(p2[npanel]-peak)*leechV > 0)
            {  /// we are above peak, stop this is last panel ///
                flag=true;
                p2[npanel] = peak;

                // check where previous point p1 is
                if (t1[npanel-1] <= 2)
                { // left points on luff
                    p1[npanel] = head;
                    t1[npanel] = 2;
                    lay.panel[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay.panel[npanel-1].left.point[k] = LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                }
                else
                { // left points on gaff
                    p1[npanel] = p1[npanel-1];
                    t1[npanel] = 3;
                    lay.panel[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay.panel[npanel-1].left.point[k] = GaffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                }

                // fill right points on leech
                lay.panel[npanel-1].right.fill(p2[npanel-1] , p2[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].right.point[k] = LeechIntersect(lay.panel[npanel-1].right.point[k], seamV);

                // fill bottom points
                lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);

                // fill top  points
                lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);

                // move all top points to gaff curve
                for( k=1; k < npb-1; k++)
                    lay.panel[npanel-1].top.point[k] = GaffIntersect(lay.panel[npanel-1].top.point[k], CVector3d (head.y()-peak.y() , peak.x()-head.x() , 0));

                ///// end peak panel //////
            }
            else
            { //// normal panel  ///////////////////
                /* find nominal position of luff/seam intersection relative to tack and head */
                i = seamSP.intersect(luff).getp();

                /* the case when the intersection is not a point needs to be handled */
                if (seamSP.intersect(luff).getdim() != 0)
                    throw CException("CSailDef::Layout1-1 : twist intersection of seam and luff is not a point!");

                if (CVector3d(i- (seamW + clothW/5)*luffV.unit() -p1[npanel-1])*luffV <= 0.00001)
                { // seam intersects below previous panel luff point + 1/5 clothW
                    p1[npanel] = p1[npanel-1] + (seamW + clothW/5)*luffV.unit();
                    t1[npanel] =2;  // 2=luff type of intersection
                    seamVT = (p1[npanel]-p2[npanel]).unit();
                    seamSPT = CSubSpace3d::line(p2[npanel] , seamVT);
                    i = seamSPT.intersect(luff).getp();
                }
                else if (CVector3d(i-head)*luffV > 0)
                { // seam intersects gaff
                    p1[npanel] = seamSP.intersect(gaff).getp();
                    t1[npanel] = 3;  // 3=gaff type of intersection
                    seamVT = seamV;

                    /* the case when the intersection is not a point needs to be handled */
                    if (seamSP.intersect(gaff).getdim() != 0)
                        throw CException("CSailDef::Layout1-2 : intersection of seam and foot is not a point!");
                }
                else
                { // seam intersects luff normally
                    p1[npanel] = i;
                    t1[npanel] = 2;  // luff
                    seamVT = seamV;
                }

                /** We now add the intermediate points on all sides of the panel */
                /* Below is the code for the left side depending
                 *  on t1 for the top side and bottom side
                 */
                if (t1[npanel-1] == 2 && t1[npanel] == 2)
                { // full luff
                    lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], seamVT);
                }
                else if (t1[npanel-1] == 3 && t1[npanel] == 3)
                { // full gaff
                    lay.panel[npanel-1].left.fill(p1[npanel-1] , p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay.panel[npanel-1].left.point[k] = GaffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                }
                else if ((t1[npanel-1] ==2 ) && (t1[npanel] == 3))
                { // luff-head-gaff
                    lay.panel[npanel-1].left.fill(p1[npanel-1] , head, p1[npanel]);
                    for (k=0; k<npl/2; k++)
                        lay.panel[npanel-1].left.point[k] = LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                    for (k=npl/2+1; k<npl; k++)
                        lay.panel[npanel-1].left.point[k] = GaffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                } // end IF ELSE for left side

                /* Below is the code for the intermediate points of the right side
                 *  which are all on the leech for a twist cut layout.
                 */
                // first check if upper point is not below lower point 
                if ( CVector3d(p2[npanel]-p2[npanel-1])*leechV < 0) 
                    p2[npanel] = p2[npanel-1];
                
                lay.panel[npanel-1].right.fill(p2[npanel-1] , p2[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].right.point[k] = LeechIntersect(lay.panel[npanel-1].right.point[k], seamV);

                /* Below is the code for the intermediate points of the top and bottom sides.
                 *  The first point is identical to the last point of the left side
                 *  The last point is identical to the last point of the right side
                 */
                lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);
                lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);

                /* Below is the code for the intermediate points of the bottom side of first panel
                 */
                if (npanel == 1)
                { // move bottom side of first panel to foot curve
                    for( k=1; k < npb-1; k++)
                        lay.panel[0].bottom.point[k] = FootIntersect(lay.panel[0].bottom.point[k], CVector3d (0,-1,0));
                }
            } //// end else normal panel ///////////////////////

            /* Now we go over all the points and calculate their z */
            for( k = 0; k < npl; k ++)
                lay.panel[npanel-1].left.point[k] = Zpoint(lay.panel[npanel-1].left.point[k]);
            for( k = 0; k < npl; k ++)
                lay.panel[npanel-1].right.point[k] = Zpoint(lay.panel[npanel-1].right.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel-1].top.point[k] = Zpoint(lay.panel[npanel-1].top.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel-1].bottom.point[k] = Zpoint(lay.panel[npanel-1].bottom.point[k]);

            /** Now we develop the current panel */
            if (npanel == 1)
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_TOP);
            else
            {
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to this bottom edge
                for( k = 1; k < npb-1; k ++)
                    dev.panel[npanel-1].bottom.point[k] = dev.panel[npanel-1].bottom.point[k] + deviaPrev[k];
            }

            /** Now we compute and store the deviation of top edge of developed panel
             *   and straighten this top edge Except if this is the top panel
             */
            if (flag == false)
            {
                vb= CMatrix::rot3d(2,PI/2) * CVector3d(dev.panel[npanel-1].top.point[npb-1] - dev.panel[npanel-1].top.point[0]).unit();
                for( k = 1; k < npb-1; k ++)
                {
                    vk= CVector3d (dev.panel[npanel-1].top.point[k] - dev.panel[npanel-1].top.point[0]);
                    v= vb * -(vk*vb);
                    deviation[k] = v;
                    dev.panel[npanel-1].top.point[k] = dev.panel[npanel-1].top.point[k] + deviation[k];
                }
            }

            /** Now we add the seam and hems allowance */
            if (npanel == 1)
                dev.panel[npanel-1].addHems(hemsW, seamW, leechHemW, hemsW);
            else if(flag == true)
                dev.panel[npanel-1].addHems(hemsW, hemsW, leechHemW, 0);
            else
                dev.panel[npanel-1].addHems(hemsW, seamW, leechHemW, 0);

            /** Now we check the width of developed panel */
            ymax = dev.panel[npanel-1].height();
            exc = ymax-clothW;

            exb=exb +.8 * exc +1;

            //printf ("P %d count %d exc %f exb %f \n", npanel,cnt,exc,exb);
        }
        /** loop on DO as long as
         *  the excess of width is positive AND counter <9
         */
        while ( exc > 0 && cnt<9);
        /////////////////////////////////////////
        //printf ("P -- %d --- maxW %f\n",cnt, (ymax-ymin));

        for (k=0; k<npb; k++)
            deviaPrev[k] = deviation[k];

        /** Now we reposition the developed panel such that
         *  bottom minimum is Y=0 AND most left is X=0
         */
        dev.panel[npanel-1] = dev.panel[npanel-1].reframe(LOW_LEFT);

        /** check if peak has been reached to break off */
        if (flag == true)
            break;
    }  /** loop FOR next seam */
    ////////////////////////////////////////

    if (npanel == MAX_PANELS-1)
        throw CException("CSailDef::Layout1-5 : got to MAX_PANELS without reaching head, do increase cloth width ");

    /** copying the sails for display **** */
    CPanelGroup sail(npanel);
    for( j = 0; j < npanel; j ++)
        sail.panel[j] = lay.panel[j];

    /** copying the developed sail */
    flatsail = CPanelGroup(npanel);

    for (j=0; j < npanel; j++)
        flatsail.panel[j] = dev.panel[j];

    /** preparing the displays version of the developed sail */
    dispsail = flatsail;

    for (j=1; j < npanel; j++)
    {
        top = dispsail.panel[j-1].top.point[0];
        bot = dispsail.panel[j].bottom.point[0];

        // rotation to align bottom of panel to top of previous panel
        x = dispsail.panel[j-1].top.point[npb-1].x() - top.x();
        y = dispsail.panel[j-1].top.point[npb-1].y() - top.y();
        CC = atan2(y,x);
        dispsail.panel[j] = dispsail.panel[j].rotate(bot,CMatrix::rot3d(2,CC));

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = top;
        v.x() -= bot.x();
        v.y() += 2 * seamW +20; // adding offset to separate panels vertically

        dispsail.panel[j] = dispsail.panel[j] + v;
    }

    return sail;
} ///////////// end layout1 = twist foot ////////////////////////////////


/** Creates a VERTICAL cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine
 */
CPanelGroup CSailWorker::LayoutVertical( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
    *   located on the straight edge of the sail (no round)
    */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* create number of panels */
    unsigned int npanel=1;
    unsigned int npl= lay.panel[0].right.nbpoints();  // number of right/left points
    unsigned int npb= lay.panel[0].bottom.nbpoints(); // number of bottom/top points

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

    /* define point i for intersections */
    CPoint3d i;

    /* define seamV as the vector parrallel to the leech vector (peak-clew)*/
    CVector3d seamV = leechV.unit();

    CSubSpace seamSP;

    /* create variables for the development and edge corrections */
    CPoint3d pt(0, 0, 0); // test point
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for panel width correction */
    CVector3d deviation [30];
    CVector3d deviaPrev [30];
    for (k=0; k<29; k++)
    {
        deviation[k] = CVector3d(0,0,0); // set all deviations to zero
        deviaPrev[k] = deviation[k];
    }

    /* create variable to monitor excess over cloth width */
    real exb, exc=0, ymax=0;

    /* seam 0 is on the leech of the sail ending at the peak */
    p1[0] = clew; // initialised at tack point
    p2[0] = peak;
    t1[0] = 1;
    t2[0] = 3;    // type=4=leech intersection

    /** Start laying the panels from the leech toward the tack */
    for (npanel = 1; npanel < MAX_PANELS-1; npanel++)
    {
        // printf(" ----- FOR panel = %d \n" , npanel);
        exb=0;
        exc =0;
        cnt =0;
        //if (npanel==3) flag=true;

        /** begin the loop for optimising the seam position to fit cloth width */
        do
        {
            cnt++;
            pt = p1[npanel-1] + (clothW-seamW-exb)* leechVP.unit();
            seamSP = CSubSpace3d::line(pt, seamV);
            p1[npanel] = seamSP.intersect(foot).getp();
            t1[npanel] = 1; // type1=1= foot intersection vertically cut panels
            /* the case when the intersection is not a point needs to be handled */
            if (seamSP.intersect(foot).getdim() != 0)
            throw CException("CSailDef::Layout vertical-1 : intersection of seam and foot is not a point!");

            if (p1[npanel].x() <= tack.x())
            { // last panel
                p1[npanel]=tack;
                t1[npanel]=1;
                p2[npanel]=tack;
                t2[npanel]=2;
                flag=true; // to get out of FOR
            }
            else
            {
                // printf ("normal panel \n");
                p2[npanel] = seamSP.intersect(gaff).getp();
                /* the case when the intersection is not a point needs to be handled */
                if (seamSP.intersect(gaff).getdim() != 0)
                    throw CException("CSailDef::Layout vertical-2 : intersection of seam and gaff is not a point!");
                    
                if (CVector3d(p2[npanel]-head)*gaffV > 0.00001)
                    t2[npanel] = 3;
                else
                {
                    p2[npanel] = seamSP.intersect(luff).getp();
                    /* the case when the intersection is not a point needs to be handled */
                    if (seamSP.intersect(luff).getdim() != 0)
                    throw CException("CSailDef::Layout vertical-3 : intersection of seam and luff is not a point!");
                    t2[npanel] = 2;
                }
            }

            //// fill right side points
            if (t2[npanel-1]==2 && t2[npanel]==2)
            {
                // printf ("full luff \n");
                lay.panel[npanel-1].right.fill(p2[npanel-1], p2[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].right.point[k]=LuffIntersect(lay.panel[npanel-1].right.point[k], seamV);
            }
            else if ((t2[npanel-1]==3) && (t2[npanel]==2))
            {
                // printf ("gaff-head-luff \n");
                lay.panel[npanel-1].right.fill(p2[npanel-1], head, p2[npanel]);

                for (k=0; k<npl/2; k++)
                    lay.panel[npanel-1].right.point[k]=GaffIntersect(lay.panel[npanel-1].right.point[k], seamV);

                for (k=npl/2+1; k<npl; k++)
                    lay.panel[npanel-1].right.point[k]=LuffIntersect(lay.panel[npanel-1].right.point[k], seamV);
            }
            else
            {
                // printf ("full gaff \n");
                lay.panel[npanel-1].right.fill(p2[npanel-1], p2[npanel]);

                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].right.point[k]=GaffIntersect(lay.panel[npanel-1].right.point[k], seamV);
            }
            //// fill left side points which are all on foot
            lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
            for (k=0; k<npl; k++)
                lay.panel[npanel-1].left.point[k]=FootIntersect(lay.panel[npanel-1].left.point[k], leechV);

            // fill bottom points
            lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);
            if (npanel == 1)
            {
                // bottom is on the leech
                for (k=0; k<npb; k++)
                    lay.panel[npanel-1].bottom.point[k]=LeechIntersect(lay.panel[npanel-1].bottom.point[k], leechVP);
            }

            //// fill top side points on seam
            lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);
            /* Now we go over all the points and calculate their z */
            for( k = 0; k < npl; k ++)
                lay.panel[npanel-1].left.point[k] = Zpoint(lay.panel[npanel-1].left.point[k]);
            for( k = 0; k < npl; k ++)
                lay.panel[npanel-1].right.point[k] = Zpoint(lay.panel[npanel-1].right.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel-1].top.point[k] = Zpoint(lay.panel[npanel-1].top.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel-1].bottom.point[k] = Zpoint(lay.panel[npanel-1].bottom.point[k]);

            /** Now we develop the current panel */
            if (npanel == 1)
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_TOP);
            else
            {
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for( k = 1; k < npb-1; k ++)
                    dev.panel[npanel-1].bottom.point[k] = dev.panel[npanel-1].bottom.point[k] + deviaPrev[k];
            }

            /** Now we compute the deviation of top edge of developed panel
            *   and straighten this top edge except if this is the top panel
            */
            if (flag == false)
            {
                vb= CMatrix::rot3d(2,PI/2)*CVector3d(dev.panel[npanel-1].top.point[npb-1] -dev.panel[npanel-1].top.point[0]).unit();
                for( k = 1; k < npb-1; k ++)
                {
                    vk= CVector3d (dev.panel[npanel-1].top.point[k] - dev.panel[npanel-1].top.point[0]);
                    v= vb * -(vk*vb);
                    deviation[k] = v;
                    dev.panel[npanel-1].top.point[k] = dev.panel[npanel-1].top.point[k] + deviation[k];
                }
            }

            /** Now we add the seam and hems allowance */
            if (npanel == 1)
                dev.panel[npanel-1].addHems(hemsW, seamW, hemsW, leechHemW);
            else if(flag == true)
                dev.panel[npanel-1].addHems(hemsW, hemsW, hemsW, 0);
            else
                dev.panel[npanel-1].addHems(hemsW, seamW, hemsW, 0);

            /** Now we check the width of developed panel */
            ymax = dev.panel[npanel-1].height();
            exc= ymax-clothW; // current excess of width

            exb= exb + .8*exc +1; // sum previous correction + 80% of current excess of width +1mm

        }
        while ( exc > 0 && cnt<9);
        /** loop DO as long as the excess of width is positive  AND counter <9 */
        /////////////////////////////////////////

        for (k=0; k<npb; k++)
            deviaPrev[k] = deviation[k];

        /** Now we reposition the developed panel such that bottom left is X=0 Y=0 */
        dev.panel[npanel-1] = dev.panel[npanel-1].reframe(LOW_LEFT);

        /** check if peak has been reached to break off */
        if (flag == true)
            break;

    }  /** loop FOR next seam */
    /////////////////////////////////////////

    if (npanel == MAX_PANELS-1)
        throw CException("CSailDef::Layout3 : got to MAX_PANELS without reaching tack, do increase cloth width ");

    /* copying the sails for display */
    CPanelGroup sail(npanel);
    for( j = 0; j < npanel; j ++)
        sail.panel[j] = lay.panel[j];

    /** copying the developed sail into flatsail */
    flatsail = CPanelGroup(npanel);

    for (j=0; j < npanel; j++)
        flatsail.panel[j] = dev.panel[j];

    /** preparing the displays version of the developed sail */
    dispsail = flatsail;

    for (j=1; j < npanel; j++)
    {
        top = dispsail.panel[j-1].top.point[0];
        bot = dispsail.panel[j].bottom.point[0];

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = top;
        v.x() -= bot.x();
        v.y() += 2*seamW +10; // adding offset to separate panels vertically

        dispsail.panel[j] = dispsail.panel[j] + v;
    }

    return sail;
} ///////////// end layout vertical cut ////////////////////////////////


/** Creates a WING horizontal cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine
 */
CPanelGroup CSailWorker::LayoutWing( CPanelGroup &flatsail, CPanelGroup &dispsail ) const
{
    /*  First create arrays p1 and p2 of points at the end of each seams
    *   located on the straight edge of the sail (no round)
    */
    CPoint3d p1[MAX_PANELS], p2[MAX_PANELS];

    /* Create two temporary sails lay and the corresponding dev */
    CPanelGroup lay(MAX_PANELS); // 3D sail
    CPanelGroup dev(MAX_PANELS); // developed sail

    /* create number of panels */
    unsigned int npanel=1, np=1;
    unsigned int npl= lay.panel[0].right.nbpoints();   // number of right/left points
    unsigned int npb= lay.panel[0].bottom.nbpoints(); // number of bottom/top points

    /* angle of the half wing from X-Y plane */
    real alfa = (180 - dihedralDeg) *PI/360;

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

    /* define point i for intersections */
    CPoint3d i;

    /* define seamV as the horizontal vector*/
    CVector3d seamV(-1,0,0);
    CSubSpace seamSP;

    /* create variables for the development and edge corrections */
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    CVector3d deviation [30];
    CVector3d deviaPrev [30];
    for (k=0; k<29; k++)
    {
        deviation[k] = CVector3d(0, 0, 0); // set all deviations to zero
        deviaPrev[k] = deviation [k];
    }

    /* create variable to monitor excess over cloth width */
    real CC=0, x=0, y=0;

    /* seam 0 is on the foot of the sail ending at the clew */
    p1[0] = tack; // initialised at tack point
    p2[0] = clew;
    t1[0] = 1;
    t2[0] = 4;    // type=4=leech intersection

    /** This loop FOR will position the seams and determine the number of panels. */
    for (npanel = 1; npanel < MAX_PANELS-1; npanel++)
    {
        p2[npanel] = p2[npanel-1] + (clothW-seamW)/(seamV*leechVP) * leechV.unit();
        t2[npanel] = 4; // type2=4=leech intersection for all horizontally cut panels
        seamSP = CSubSpace3d::line(p2[npanel], seamV);

        if (CVector3d(p2[npanel]-peak)*leechV > 0)  // we are above peak, stop last panel
        {
            flag=true;
            p2[npanel] = peak;
            // check on which side of the sail the previous point p1 is located
            if (t1[npanel-1] < 2 )
            { // previous seam on foot
                p1[npanel] = head;
                t1[npanel] = 2; // set on luff
                // left points on foot-tack-luff
                lay.panel[npanel-1].left.fill(p1[npanel-1], tack, p1[npanel]);
                for (k=0; k<npl/2; k++)
                    lay.panel[npanel-1].left.point[k]=FootIntersect(lay.panel[npanel-1].left.point[k], seamV);
                for (k=npl/2+1; k<npl; k++)
                    lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
            }
            else if (t1[npanel-1] == 2)
            { // left points on luff
                p1[npanel] = head;
                t1[npanel] = 2;
                lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
            }
            else
            { // left points on gaff
                p1[npanel] = p1[npanel-1];
                t1[npanel] = 3;
                lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].left.point[k]=GaffIntersect(lay.panel[npanel-1].left.point[k], seamV);
            }

            // fill right points on leech
            lay.panel[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
            for (k=0; k<npl; k++)
                lay.panel[npanel-1].right.point[k]=LeechIntersect(lay.panel[npanel-1].right.point[k], seamV);

            // fill bottom points
            lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);

            // fill top  points
            lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);

            // move all top points of top panel to gaff curve
            for( k=1; k < npb-1; k++)
                lay.panel[npanel-1].top.point[k]=GaffIntersect(lay.panel[npanel-1].top.point[k], CVector3d (head.y()-peak.y(),peak.x()-head.x(),0));

            ////// end peak panel /////
        }
        else        // normal panel  ///////////////////////////
        {
            /* find position of luff/seam intersection relative to tack and head */
            i = seamSP.intersect(luff).getp();

            /* the case when the intersection is not a point needs to be handled */
            if (seamSP.intersect(luff).getdim() != 0)
                throw CException("CSailDef::Layout wing-1 : intersection of seam and luff is not a point!");

            if (CVector3d(i-tack)*luffV < 0)
            {
                // seam intersects foot
                p1[npanel] = seamSP.intersect(foot).getp();
                t1[npanel] =1;  // 1=foot type of intersection

                if (npanel == 1)
                { // set lower edge to start at same point p1
                    p1[0] = p1[npanel];
                    t1[0] = 1;
                }
                /* the case when the intersection is not a point needs to be handled */
                if (seamSP.intersect(foot).getdim() != 0)
                    throw CException("CSailDef::Layout wing-2 : intersection of seam and foot is not a point!");
            }
            else if (CVector3d(i-head)*luffV > 0)
            {
                // seam intersects gaff
                p1[npanel] = seamSP.intersect(gaff).getp();
                t1[npanel] = 3;  // 3=gaff type of intersection

                /* the case when the intersection is not a point needs to be handled */
                if (seamSP.intersect(gaff).getdim() != 0)
                    throw CException("CSailDef::Layout wing-3 : intersection of seam and foot is not a point!");
            }
            else
            {
                // seam intersects luff
                p1[npanel] = i;
                t1[npanel] = 2;  // luff
                if (npanel == 1)
                {
                    // force seam 0 to start at the tack
                    p1[0] = tack;
                    t1[0] = 2;
                }
            }

            /** We now add the intermediate points on all sides of the normal panel  */

            /* Below is the code for the left side depending
            *  on t1 for the top side and bottom side
            */
            if (t1[npanel-1] == 1 && t1[npanel] == 1)
            {
                // full foot
                lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].left.point[k]=FootIntersect(lay.panel[npanel-1].left.point[k], seamV);
            }
            else if (t1[npanel-1] == 2 && t1[npanel] == 2)
            {
                // full luff
                lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
            }
            else if (t1[npanel-1] == 3 && t1[npanel] == 3)
            {
                // full gaff
                lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].left.point[k]=GaffIntersect(lay.panel[npanel-1].left.point[k], seamV);
            }
            else if ((t1[npanel-1] == 1) && (t1[npanel] == 2))
            {
                // foot-tack-luff
                lay.panel[npanel-1].left.fill(p1[npanel-1], tack, p1[npanel]);
                for (k=0; k<npl/2; k++)
                    lay.panel[npanel-1].left.point[k]=FootIntersect(lay.panel[npanel-1].left.point[k], seamV);
                for (k=npl/2+1; k<npl; k++)
                    lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
            }
            else if ((t1[npanel-1] == 2) && (t1[npanel] == 3))
            {
                // luff-head-gaff
                lay.panel[npanel-1].left.fill(p1[npanel-1], head, p1[npanel]);
                for (k=0; k<npl/2; k++)
                    lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], seamV);
                for (k=npl/2+1; k<npl; k++)
                    lay.panel[npanel-1].left.point[k]=GaffIntersect(lay.panel[npanel-1].left.point[k], seamV);
            } // end IF ELSE for left side

            /* Below is the code for the intermediate points of the right side
            *  which are all on the leech for a crosscut layout.
            */
            lay.panel[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
            for (k=0; k<npl; k++)
                lay.panel[npanel-1].right.point[k]=LeechIntersect(lay.panel[npanel-1].right.point[k], seamV);

            /* Below is the code for the intermediate points of the top and bottom sides.
            *  The first point is identical to the last point of the left side
            *  The last point is identical to the last point of the right side
            */
            lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);
            lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);

            /* Below is the code for the intermediate points of the bottom side of first panel  */
            if (npanel == 1)
            { // move bottom side of first panel to foot curve
                for( k=1; k < npb-1; k++)
                    lay.panel[0].bottom.point[k] = FootIntersect(lay.panel[0].bottom.point[k], CVector3d(0,-1,0));
            }
        } //// end else normal panel ///////////////////////

        /* Now we go over all the points and calculate their z */
        for( k = 0 ; k < npl ; k ++)
            lay.panel[npanel-1].left.point[k] = Zpoint( lay.panel[npanel-1].left.point[k] );

        for( k = 0 ; k < npl ; k ++)
            lay.panel[npanel-1].right.point[k] = Zpoint( lay.panel[npanel-1].right.point[k] );

        for( k = 0 ; k < npb ; k ++)
            lay.panel[npanel-1].top.point[k] = Zpoint( lay.panel[npanel-1].top.point[k] );

        for( k = 0 ; k < npb ; k ++)
            lay.panel[npanel-1].bottom.point[k] = Zpoint( lay.panel[npanel-1].bottom.point[k] ) ;

        /* Now we rotate the panel by the dihedral angle */
        lay.panel[npanel-1] = lay.panel[npanel-1].rotate(tack , CMatrix::rot3d(0 , alfa) );

        /* and if it is the first panel we cut the foot to tack level */
        if ( npanel == 1 )
        {
            for( k=0; k < npb-1; k++)
                lay.panel[0].bottom.point[k].y() = tack.y();
            lay.panel[0].left.point[0].y() = tack.y();
            lay.panel[0].right.point[0].y() = tack.y();
        }

        /* Now we develop the current panel */
        if ( npanel == 1 )
        {
            dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_TOP);
        }
        else
        {
            dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_BOTTOM);
            // add deviation of previous panel top edge to bottom edge
            for( k = 1 ; k < npb-1 ; k ++)
                dev.panel[npanel-1].bottom.point[k] = dev.panel[npanel-1].bottom.point[k] + deviaPrev[k];
        }

        /* Now we compute and store the deviation of top edge of
        *   the developed panel and straighten this top edge
        *   except if this is the top panel
        */
        if ( flag == false )
        {
            vb= CMatrix::rot3d(2,PI/2)*CVector3d(dev.panel[npanel-1].top.point[npb-1] -dev.panel[npanel-1].top.point[0]).unit();
            for( k = 1; k < npb-1; k ++)
            {
                vk= CVector3d (dev.panel[npanel-1].top.point[k] - dev.panel[npanel-1].top.point[0]);
                v= vb * -(vk*vb);
                deviation[k] = v;
                dev.panel[npanel-1].top.point[k] = dev.panel[npanel-1].top.point[k] + deviation[k];
            }
        }

        /** Now we add the seam and hems allowance */
        if ( npanel == 1 )
            dev.panel[npanel-1].addHems(hemsW, seamW, leechHemW, seamW);
        else if ( flag == true )
            dev.panel[npanel-1].addHems(hemsW, hemsW, leechHemW, 0);
        else
            dev.panel[npanel-1].addHems(hemsW, seamW, leechHemW, 0);

        ////////////////////////////////////////////

        for ( k = 0 ; k < npb ; k++)
            deviaPrev[k] = deviation[k];

        /** Now we reposition the developed panel such that
        *  bottom minimum is Y=0 AND left is X=0
        */
        dev.panel[npanel-1] = dev.panel[npanel-1].reframe(LOW_LEFT);

        /** check if peak has been reached to break off */
        if ( flag == true )
            break;
    }  /** loop FOR next seam */
    ////////////////////////////////////////

    if ( npanel == MAX_PANELS/2 -1 )
        throw CException("CSailDef::Layout wing-4 : got to MAX_PANELS without reaching head, do increase cloth width ");

    /** creating the symetrical panels */
    np = npanel;
    for ( j = 0 ; j < np +1 ; j++)
    {
        npanel ++;
        lay.panel[npanel] = lay.panel[j];
        dev.panel[npanel] = dev.panel[j];

        for ( k = 0 ; k < npb ; k++)
        {
            lay.panel[npanel].top.point[k].y() = -lay.panel[npanel].top.point[k].y();
            lay.panel[npanel].bottom.point[k].y() = -lay.panel[npanel].bottom.point[k].y();
        }
        for ( k = 0 ; k < npl ; k++)
        {
            lay.panel[npanel].left.point[k].y() = -lay.panel[npanel].left.point[k].y();
            lay.panel[npanel].right.point[k].y() = -lay.panel[npanel].right.point[k].y();
        }
    }

    /** copying the sails for display */
    CPanelGroup sail( 2 * npanel +1);

    for( j = 0 ; j < npanel ; j ++)
        sail.panel[j] = lay.panel[j];

    /** copying the developed sail */
    flatsail = CPanelGroup( 2 * npanel +1);

    for ( j = 0 ; j < npanel ; j++)
    {
        flatsail.panel[j] = dev.panel[j];
    }

    /** preparing the displays version of the developed sail */
    dispsail = flatsail;

    for ( j = 1 ; j < npanel ; j++)
    {
        top = dispsail.panel[j-1].top.point[0];
        bot = dispsail.panel[j].bottom.point[0];

        // rotation to align bottom of panel to top of previous panel
        x = dispsail.panel[j-1].top.point[npb-1].x()-top.x();
        y = dispsail.panel[j-1].top.point[npb-1].y()-top.y();
        CC= atan2(y,x);
        dispsail.panel[j] = dispsail.panel[j].rotate(bot,CMatrix::rot3d(2,CC));

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = CVector3d (top - CPoint3d(0,0,0));
        v.x() = v.x() - bot.x();
        v.y() = v.y() + 2*seamW + 25;  // adding offset to separate panels vertically

        dispsail.panel[j] = dispsail.panel[j] + v;
    }
    ///
    return sail;
} ///////////// end layoutWing = cross cut or horizontal /////////////////////



/** Creates a radial cut sail.
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine
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
    unsigned int npl = lay.panel[0].right.nbpoints();  // number of right/left points
    unsigned int npb = lay.panel[0].bottom.nbpoints();  // number of bottom/top points
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
    CVector3d deviation [30];
    for ( k = 0 ; k < 29 ; k++)
        deviation[k] = CVector3d(0, 0, 0); // set all deviations to zero

    /** Create arrays of points at horizontal seams ends (10 maximum)
    */
    CPoint3d luffH[10];    // point at the luff end of the horizontal seam
    CPoint3d leechH[10];   // point at the leech end of the horizontal seam
    CSubSpace seamH[10];   // corresponding seam lines
    CSubSpace seamSP;  // a seam line

    h = 0;
    luffH[h] = tack;
    leechH[h] = clew;
    seamH[h] = foot;

    for ( h=1; h<nbSections; h++)  
    {
        pt1 = tack + luffV * (real(h) / nbSections );
        pt2 = clew + leechV * (real(h) / nbSections );
        luffH[h] = LuffIntersect( pt1, CVector3d( pt1 - pt2 ) );
        leechH[h] = LeechIntersect( pt2, CVector3d( pt2 - pt1) );
        seamH[h] = CSubSpace3d::line(luffH[h], CVector3d( leechH[h] - luffH[h] ));
    }

    h = nbSections; // one more horizontal line than nbSections
    luffH[h] = head;
    leechH[h] = peak;
    seamH[h] = gaff;

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
    for ( h = nbSections-1 ; h > 0 ; h--)
    {
        pt0 = tack + footV*(real(h) / real(nbSections))*(real(ngLuff) / real(nbGores));
        seamSP = CSubSpace3d::line(luffCatenary[h+1], CVector3d(pt0 - luffCatenary[h+1]));
        luffCatenary[h] = seamH[h].intersect(seamSP).getp();

        pt0 = clew - footV*(real(h) / real(nbSections))*(real(nbGores-ngLuff) / real(nbGores));
        seamSP = CSubSpace3d::line(leechCatenary[h+1], CVector3d(pt0 - leechCatenary[h+1]));
        leechCatenary[h] = seamH[h].intersect(seamSP).getp();
    }

    h = 0;
    luffCatenary[h] = tack;
    leechCatenary[h] = clew;

    /** Now we cut the radial panels
    *  Panels are oriented with lower and upper edge on vertical catenary
    *  Bottom side is on luff side and top side is on leech side
    *  Left side is at the top of each section
    *  p1 p2 are the end point of bottom side with p1 = top of horizontal section
    */
    npanel = 0;
    ng1 = 0;  // initialise number of central panels

    /** Sweeping the sail from the top section downward to the foot */
    for (h = nbSections; h>0; h--)
    { // sweeping from top section downward
        nps[h] = 0;  // counter of panels in current section
        /** Cutting the luff side panels */
        for ( j = 1 ; j <= ngLuff ; j++)
        {
            if ( j == 1 )
            { // place bottom end points on luff catenary
                pt1 = luffH[h];
                pt2 = luffH[h-1];

                lay.panel[npanel].bottom.fill( pt1, pt2 );

                for ( k = 1 ; k < npb -1 ; k++)
                    lay.panel[npanel].bottom.point[k] = LuffIntersect(lay.panel[npanel].bottom.point[k] , CVector3d( luffH[h-1] - leechH[h-1] ) );
            }
            else
            { // copy previous points
                pt1 = pt3;
                pt2= pt4;
                lay.panel[npanel].bottom.fill( pt1, pt2 );
            }

            // we compute the top end points
            pt3 = luffH[h] + CVector3d( luffCatenary[h] - luffH[h] ) * (real(j) / ngLuff);
            if ( h == nbSections )
                pt3 = GaffIntersect( pt3, gaffVP );

            pt4 = luffH[h-1] + CVector3d( luffCatenary[h-1] - luffH[h-1] ) * (real(j) / ngLuff);

            // we fill the other edges
            lay.panel[npanel].top.fill( pt3 , pt4 );
            lay.panel[npanel].left.fill( pt1 , pt3 );
            lay.panel[npanel].right.fill( pt2 , pt4 );

            if ( h == nbSections )
            {
                for ( k = 1 ; k < npl -1 ; k++)
                    lay.panel[npanel].left.point[k] = GaffIntersect(lay.panel[npanel].left.point[k] , gaffVP );
            }

            // We compute Z
            for( k = 0 ; k < npl ; k ++)
                lay.panel[npanel].left.point[k] = Zpoint( lay.panel[npanel].left.point[k] );
            for( k = 0 ; k < npl ; k ++)
                lay.panel[npanel].right.point[k] = Zpoint( lay.panel[npanel].right.point[k] );
            for( k = 0 ; k < npb ; k ++)
                lay.panel[npanel].top.point[k] = Zpoint( lay.panel[npanel].top.point[k] );
            for( k = 0 ; k < npb ; k ++)
                lay.panel[npanel].bottom.point[k] = Zpoint( lay.panel[npanel].bottom.point[k] );

            // We develop the current panel
            dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);


            // We add the seams and hems allowance
            if ( h == nbSections  &&  j == 1 )
                dev.panel[npanel].addHems(hemsW, seamW, 0, hemsW);
            else if ( j == 1 )
                dev.panel[npanel].addHems(seamW, seamW, 0, hemsW);
            else
                dev.panel[npanel].addHems(seamW, seamW, 0, 0);

            nps[h]++;
            npanel++;
        }

        /** Cutting the middle panels */
        if ( h > 1 )
        {
            ng1++;  // we add one more central panel than section above
            a = int(floor(real(ng1) / 2) );
            b = int(ceil(real(ng1) / 2) );

            // we initialise the bottom end points
            pt3 = luffCatenary[h];
            if ( h == nbSections )
                pt3 = GaffIntersect( pt3, gaffVP );

            pt4 = luffCatenary[h-1];

            // we now compute the other points
            for ( j = 1 ; j <= ng1 ; j++)
            {
                pt1 = pt3;
                pt2 = pt4;
                pt4 = luffCatenary[h-1] + CVector3d( leechCatenary[h-1] - luffCatenary[h-1] ) * (real(j) / ng1);

                if (ng1 < 4)
                {
                    pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / ng1);
                    if ( h == nbSections )
                        pt3 = GaffIntersect( pt3, gaffVP );
                }
                else
                {
                    if ( j < a )  // before middle point
                    {
                        pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / (ng1-1));
                    }
                    else if (j > b )  // after middle point
                    {
                        pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j-1) / (ng1-1));
                    }
                    else
                    {
                        pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / ng1);
                    }
                }

                // we fill the 4 edges
                lay.panel[npanel].left.fill( pt1 , pt3 );
                lay.panel[npanel].right.fill( pt2 , pt4 );
                lay.panel[npanel].bottom.fill( pt1 , pt2 );
                lay.panel[npanel].top.fill( pt3 , pt4 );

                if ( h == nbSections )
                {
                    for ( k = 1 ; k < npl -1 ; k++)
                        lay.panel[npanel].left.point[k] = GaffIntersect(lay.panel[npanel].left.point[k] , gaffVP );
                }

                // we compute Z
                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint( lay.panel[npanel].left.point[k] );
                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint( lay.panel[npanel].right.point[k] );
                for( k = 0 ; k < npb ; k ++)
                    lay.panel[npanel].top.point[k] = Zpoint( lay.panel[npanel].top.point[k] );
                for( k = 0 ; k < npb ; k ++)
                    lay.panel[npanel].bottom.point[k] = Zpoint( lay.panel[npanel].bottom.point[k] );

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if ( h == nbSections )
                    dev.panel[npanel].addHems(hemsW, seamW, 0, 0);
                else
                    dev.panel[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }
        }
	else  // h = 1 middle panels of bottom section 
        {
            // luff side central
            for ( j = 1 ; j <= ng1 / 2 ; j++)
            {
                pt1 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j-1) / ng1);
                pt2 = tack;

                pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / ng1);
                pt4 = pt2;

                ptCentre = pt3; // memorise last point on horizontal seam 1

                lay.panel[npanel].left.fill(pt1 , pt3);
                lay.panel[npanel].right.fill(pt2 , pt4);
                lay.panel[npanel].bottom.fill(pt1 , pt2);
                lay.panel[npanel].top.fill(pt3 , pt4);

                // compute Z
                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint( lay.panel[npanel].left.point[k] );

                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint( lay.panel[npanel].right.point[k] );

                for( k = 0 ; k < npb ; k ++)
                    lay.panel[npanel].top.point[k] = Zpoint( lay.panel[npanel].top.point[k] );

                for( k = 0 ; k < npb ; k ++)
                    lay.panel[npanel].bottom.point[k] = Zpoint( lay.panel[npanel].bottom.point[k] );

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams allowance
                dev.panel[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // luff side lower central
            pt0 = ( luffCatenary[nbSections] + leechCatenary[nbSections] ) * 0.5;
            ptFoot = FootIntersect( ptCentre , CVector3d(ptCentre-pt0) );
            a = int( (CVector3d(ptFoot-ptCentre).norm()) / (clothW) );
            if (a < 2)
                a = 2;

            for ( j = 1 ; j <= a ; j++)
            {   // from center to foot
                pt1 = ptCentre + CVector3d (ptFoot - ptCentre) * (real(j-1) / a);
                pt2 = tack;
                pt3 = ptCentre + CVector3d (ptFoot - ptCentre) * (real(j) / a);
                pt4 = pt2;

                lay.panel[npanel].left.fill( pt1 , pt3 );
                lay.panel[npanel].right.fill( pt2 , pt4 );
                lay.panel[npanel].bottom.fill( pt1 , pt2 );
                lay.panel[npanel].top.fill( pt3 , pt4 );

                // compute Z
                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint( lay.panel[npanel].left.point[k] );

                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint( lay.panel[npanel].right.point[k] );

                for( k = 0 ; k < npb ; k ++)
                    lay.panel[npanel].bottom.point[k] = Zpoint( lay.panel[npanel].bottom.point[k] );

                for( k = 0 ; k < npb ; k ++)
                {
                    if ( j == a )
                        pt0 = FootIntersect( lay.panel[npanel].top.point[k] , CVector3d( ptFoot- ptCentre ) );
                    else
                        pt0 = lay.panel[npanel].top.point[k];

                    lay.panel[npanel].top.point[k] = Zpoint( pt0 );
                }

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if ( j == a )
                    dev.panel[npanel].addHems(seamW, hemsW, 0, 0);
                else
                    dev.panel[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // leech side lower central
            for ( j = 1 ; j <= a ; j++)
            {   // from foot to center
                pt1 = ptFoot + CVector3d ( ptCentre - ptFoot ) * (real(j-1) / a );
                pt2 = clew;
                pt3 = ptFoot + CVector3d ( ptCentre - ptFoot ) * (real(j) / a);
                pt4 = pt2;

                lay.panel[npanel].left.fill( pt1, pt3 );
                lay.panel[npanel].right.fill( pt2, pt4 );
                lay.panel[npanel].bottom.fill( pt1, pt2 );
                lay.panel[npanel].top.fill( pt3, pt4 );

                // compute Z
                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint( lay.panel[npanel].left.point[k] );

                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint( lay.panel[npanel].right.point[k] );

                for( k = 0 ; k < npb ; k ++)
                    lay.panel[npanel].top.point[k] = Zpoint( lay.panel[npanel].top.point[k] );

                for( k = 0 ; k < npb ; k ++)
                {
                    if ( j == 1 )
                        pt0 = FootIntersect( lay.panel[npanel].bottom.point[k] , CVector3d( ptFoot- ptCentre ) );
                    else
                        pt0 = lay.panel[npanel].bottom.point[k];

                    lay.panel[npanel].bottom.point[k] = Zpoint( pt0 );
                }

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if ( j == 1 )
                    dev.panel[npanel].addHems(0, seamW, 0, hemsW);
                else
                    dev.panel[npanel].addHems(0, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // leech side central
            for ( j = ng1/2 +1 ; j <= ng1 ; j++)
            {
                pt1 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j-1) / ng1);
                pt2 = clew ;

                pt3 = luffCatenary[h] + CVector3d( leechCatenary[h] - luffCatenary[h] ) * (real(j) / ng1);
                pt4 = pt2;

                lay.panel[npanel].left.fill( pt1, pt3 );
                lay.panel[npanel].right.fill( pt2, pt4 );
                lay.panel[npanel].bottom.fill( pt1, pt2 );
                lay.panel[npanel].top.fill( pt3, pt4 );

                // compute Z
                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint( lay.panel[npanel].left.point[k] );

                for( k = 0 ; k < npl ; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint( lay.panel[npanel].right.point[k] );

                for( k = 0 ; k < npb ; k ++)
                    lay.panel[npanel].top.point[k] = Zpoint( lay.panel[npanel].top.point[k] );

                for( k = 0 ; k < npb ; k ++)
                    lay.panel[npanel].bottom.point[k] = Zpoint( lay.panel[npanel].bottom.point[k] );

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams allowance
                dev.panel[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }
        }

        /** cutting the leech side panels */
        for ( j = 1 ; j < nbGores-ngLuff ; j++)
        {
            pt1 = leechCatenary[h] + CVector3d( leechH[h] - leechCatenary[h] ) * (real(j-1) / (nbGores-ngLuff-1) );
            if ( h == nbSections )
                pt1 = GaffIntersect( pt1, gaffVP );

            pt2 = leechCatenary[h-1] + CVector3d( leechH[h-1] - leechCatenary[h-1] ) * (real(j-1) / (nbGores-ngLuff-1) );

            pt3 = leechCatenary[h] + CVector3d( leechH[h] - leechCatenary[h] ) * (real(j) / (nbGores-ngLuff-1) );
            if ( h == nbSections )
                pt3 = GaffIntersect( pt3, gaffVP );

            pt4 = leechCatenary[h-1] + CVector3d( leechH[h-1] - leechCatenary[h-1] ) * (real(j) / (nbGores-ngLuff-1) );

            lay.panel[npanel].bottom.fill( pt1, pt2 );
            lay.panel[npanel].top.fill( pt3, pt4 );
            lay.panel[npanel].left.fill( pt1, pt3 );
            lay.panel[npanel].right.fill( pt2, pt4 );

            if ( h == nbSections )
            {
                for ( k = 1 ; k < npl -1 ; k++)
                    lay.panel[npanel].left.point[k] = GaffIntersect(lay.panel[npanel].left.point[k] , gaffVP );
            }

            if ( j == nbGores - ngLuff -1)
            {
                for ( k = 1 ; k < npb -1 ; k++)
                    lay.panel[npanel].top.point[k] = LeechIntersect(lay.panel[npanel].top.point[k] , CVector3d(luffH[h-1]-leechH[h-1]) );
            }

            // we compute Z
            for( k = 0 ; k < npl ; k ++)
                lay.panel[npanel].left.point[k] = Zpoint( lay.panel[npanel].left.point[k] );
            for( k = 0 ; k < npl ; k ++)
                lay.panel[npanel].right.point[k] = Zpoint( lay.panel[npanel].right.point[k] );
            for( k = 0 ; k < npb ; k ++)
                lay.panel[npanel].top.point[k] = Zpoint( lay.panel[npanel].top.point[k] );
            for( k = 0 ; k < npb ; k ++)
                lay.panel[npanel].bottom.point[k] = Zpoint( lay.panel[npanel].bottom.point[k] );

            // We develop the current panel
            dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

            // We add the seams and hems allowance
            if ( h == nbSections  &&  j == nbGores-ngLuff-1)
                dev.panel[npanel].addHems(hemsW, leechHemW, 0, 0);
            else if (j == nbGores - ngLuff -1)
                dev.panel[npanel].addHems(seamW, leechHemW, 0, 0);
            else
                dev.panel[npanel].addHems(seamW, seamW, 0, 0);

            nps[h]++;
            npanel++;
        }
    }

    /** copying 3d lay into 3d sail */
    CPanelGroup sail(npanel);
    for( j = 0 ; j < npanel ; j ++)
        sail.panel[j] = lay.panel[j];

    /** copying from temporary developed sail into flatsail */
    flatsail = CPanelGroup(npanel);

    for (j=0 ; j < npanel ; j++)
        flatsail.panel[j] = dev.panel[j];

    /** preparing the displays version of the developed sail */
    dispsail = flatsail;
    j = 0, h = nbSections, k = 0;
    v = CVector3d(0,0,0);
    xp = 0;
    xm = 0;
    ym = 0;

    for ( j = 0 ; j < npanel ; j++)
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
        dispsail.panel[j] = dispsail.panel[j] + v;

        CRect3d pRect = dispsail.panel[j].boundingRect();
        ym = pRect.height();
        v.y() += ym+ 2*seamW +20; // adding offset to separate next panel vertically

        x = pRect.max.x();
        if ( x > xm )
            xm = x;
        k++;
    }
    ///
    return sail;
} ///////////// end layout radial cut ///////////////////////////////////



/** Creates a triradial cut sail.  //////// NOT USED //////////
 *
 * @param flatsail the CPanelGroup object that will hold the developed sail
 * @param dispsail the CPanelGroup object that will hold the display
 *                 version of the developed sail
 * @return CPanelGroup
 * @author Robert Laine
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
    unsigned int npl= lay.panel[0].right.nbpoints();  // number of right/left points
    unsigned int npb= lay.panel[0].bottom.nbpoints();  // number of bottom/top points
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
    CVector3d deviation [30];
    for (k=0; k<29 ; k++)
        deviation[k] = CVector3d(0, 0, 0); // set all deviations to zero

    /* Create arrays of points at horizontal seams ends 10 maximum */
    CPoint3d luffH[10];    // point at the luff end of the horizontal seam
    CPoint3d leechH[10];   // point at the leech end of the horizontal seam
    CSubSpace seamH[10];   // corresponding seam lines
    CSubSpace seamSP;  // a seam line

    for ( h=0; h<nbSections; h++)  // one more horizontal line than nbSections
    {
        pt1 = tack + luffV * (real(h) / nbSections );
        pt2 = clew + leechV * (real(h) / nbSections );
        luffH[h] = LuffIntersect( pt1, CVector3d(pt1-pt2) );
        leechH[h] = LeechIntersect( pt2, CVector3d(pt2-pt1) );
        seamH[h] = CSubSpace3d::line(luffH[h], CVector3d(leechH[h] - luffH[h]));
    }

    h= nbSections;
    luffH[h] = head;
    leechH[h] = peak;
    seamH[h] = gaff;

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

    for ( h=nbSections-1; h>0; h--)
    {
        pt0 = tack + footV*(real(h) / real(2* nbSections));
        seamSP = CSubSpace3d::line(luffCatenary[h+1], CVector3d(pt0 - luffCatenary[h+1]));
        luffCatenary[h] = seamH[h].intersect(seamSP).getp();

        pt0 = clew - footV*(real(h) / real(2* nbSections));
        seamSP = CSubSpace3d::line(leechCatenary[h+1], CVector3d(pt0 - leechCatenary[h+1]));
        leechCatenary[h] = seamH[h].intersect(seamSP).getp();
    }

    h=0;
    luffCatenary[h] = tack;
    leechCatenary[h] = clew;

    /** Now cutting the radial panels.
    *   Panels are oriented with lower edge vertical toward luff
    *   and left side at the top of each section
    */
    npanel = 0;
    ng1 = 0; // initialise number of central panels

    for (h = nbSections; h>0; h--)
    {
        // sweeping from top section downward
        nps[h]=0; // counter of panels in current section
        /** cutting the luff side panels */
        for (j=1; j<=ngLuff; j++)
        {
            if (j == 1)
            {
                pt1 = luffH[h];
                pt2 = luffH[h-1];
                lay.panel[npanel].bottom.fill(pt1 , pt2);
                for (k=1; k<npb-1; k++)
                    lay.panel[npanel].bottom.point[k] = LuffIntersect(lay.panel[npanel].bottom.point[k], CVector3d(luffH[h-1]-leechH[h-1]) );
            }
            else
            {
                pt1 = pt3;
                pt2= pt4;
                lay.panel[npanel].bottom.fill(pt1 , pt2);
            }

            pt3 = luffH[h] + CVector3d(luffCatenary[h]-luffH[h]) * (real(j)/ngLuff);
            if (h == nbSections)
                pt3 = GaffIntersect( pt3, gaffVP );
            pt4 = luffH[h-1] + CVector3d(luffCatenary[h-1]-luffH[h-1]) * (real(j)/ngLuff);

            lay.panel[npanel].top.fill(pt3, pt4);
            lay.panel[npanel].left.fill(pt1 , pt3);
            lay.panel[npanel].right.fill(pt2 , pt4);

            if (h == nbSections)
            {
                for (k=1; k<npl-1; k++)
                    lay.panel[npanel].left.point[k] = GaffIntersect(lay.panel[npanel].left.point[k], gaffVP );
            }

            // compute Z
            for( k = 0; k < npl; k ++)
                lay.panel[npanel].left.point[k] = Zpoint(lay.panel[npanel].left.point[k]);
            for( k = 0; k < npl; k ++)
                lay.panel[npanel].right.point[k] = Zpoint(lay.panel[npanel].right.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel].top.point[k] = Zpoint(lay.panel[npanel].top.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel].bottom.point[k] = Zpoint(lay.panel[npanel].bottom.point[k]);

            // We develop the current panel
            dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);


            // We add the seams and hems allowance
            if (h == nbSections && j == 1)
                dev.panel[npanel].addHems(hemsW, seamW, 0, hemsW);
            else if (j == 1)
                dev.panel[npanel].addHems(seamW, seamW, 0, hemsW);
            else
                dev.panel[npanel].addHems(seamW, seamW, 0, 0);

            nps[h]++;
            npanel++;
        }

        /** cutting the middle panels */
        if ( h > 1 )
        {
            ng1++; // add one more central panel than section above
            a = int(floor(real(ng1)/2));
            b = int(ceil(real(ng1)/2));
            pt3 = luffCatenary[h];
            if (h == nbSections)
                pt3 = GaffIntersect( pt3, gaffVP );
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
                        pt3 = GaffIntersect( pt3, gaffVP );
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

                lay.panel[npanel].left.fill(pt1 , pt3);
                lay.panel[npanel].right.fill(pt2 , pt4);
                lay.panel[npanel].bottom.fill(pt1 , pt2);
                lay.panel[npanel].top.fill(pt3 , pt4);

                if (h == nbSections)
                {
                    for (k=1; k<npl-1; k++)
                        lay.panel[npanel].left.point[k] = GaffIntersect(lay.panel[npanel].left.point[k], gaffVP );
                }

                // compute Z
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint(lay.panel[npanel].left.point[k]);
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint(lay.panel[npanel].right.point[k]);
                for( k = 0; k < npb; k ++)
                    lay.panel[npanel].top.point[k] = Zpoint(lay.panel[npanel].top.point[k]);
                for( k = 0; k < npb; k ++)
                    lay.panel[npanel].bottom.point[k] = Zpoint(lay.panel[npanel].bottom.point[k]);

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if (h == nbSections)
                    dev.panel[npanel].addHems(hemsW, seamW, 0, 0);
                else
                    dev.panel[npanel].addHems(seamW, seamW, 0, 0);

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

                lay.panel[npanel].left.fill(pt1 , pt3);
                lay.panel[npanel].right.fill(pt2 , pt4);
                lay.panel[npanel].bottom.fill(pt1 , pt2);
                lay.panel[npanel].top.fill(pt3 , pt4);

                // compute Z
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint(lay.panel[npanel].left.point[k]);
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint(lay.panel[npanel].right.point[k]);
                for( k = 0; k < npb; k ++)
                    lay.panel[npanel].top.point[k] = Zpoint(lay.panel[npanel].top.point[k]);
                for( k = 0; k < npb; k ++)
                    lay.panel[npanel].bottom.point[k] = Zpoint(lay.panel[npanel].bottom.point[k]);

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams allowance
                dev.panel[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }

            // luff side lower central
            pt0=(luffCatenary[nbSections]+leechCatenary[nbSections])*0.5;
            ptFoot = FootIntersect( ptCentre , CVector3d(ptCentre-pt0) );
            a = int( (CVector3d(ptFoot-ptCentre).norm()) / (clothW) );
            if (a < 2)
                a = 2;

            for (j=1; j<=a; j++)
            {
                pt1 = ptCentre + CVector3d (ptFoot - ptCentre)*(real(j-1)/a);
                pt2 = tack;
                pt3 = ptCentre + CVector3d (ptFoot - ptCentre)*(real(j)/a);
                pt4 = pt2;

                lay.panel[npanel].left.fill(pt1 , pt3);
                lay.panel[npanel].right.fill(pt2 , pt4);
                lay.panel[npanel].bottom.fill(pt1 , pt2);
                lay.panel[npanel].top.fill(pt3 , pt4);

                // compute Z
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint(lay.panel[npanel].left.point[k]);
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint(lay.panel[npanel].right.point[k]);
                for( k = 0; k < npb; k ++)
                {
                    pt0 = lay.panel[npanel].top.point[k];
                    if (j == a)
                        pt0 = FootIntersect(lay.panel[npanel].top.point[k], footVP);
                    lay.panel[npanel].top.point[k] = Zpoint(pt0);
                }
                for( k = 0; k < npb; k ++)
                    lay.panel[npanel].bottom.point[k] = Zpoint(lay.panel[npanel].bottom.point[k]);

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if (j == a)
                    dev.panel[npanel].addHems(seamW, hemsW, 0, 0);
                else
                    dev.panel[npanel].addHems(seamW, seamW, 0, 0);

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

                lay.panel[npanel].left.fill(pt1 , pt3);
                lay.panel[npanel].right.fill(pt2 , pt4);
                lay.panel[npanel].bottom.fill(pt1 , pt2);
                lay.panel[npanel].top.fill(pt3 , pt4);

                // compute Z
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint(lay.panel[npanel].left.point[k]);
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint(lay.panel[npanel].right.point[k]);
                for( k = 0; k < npb; k ++)
                    lay.panel[npanel].top.point[k] = Zpoint(lay.panel[npanel].top.point[k]);
                for( k = 0; k < npb; k ++)
                {
                    pt0 = lay.panel[npanel].bottom.point[k];
                    if (j == 1)
                        pt0 = FootIntersect(lay.panel[npanel].bottom.point[k], footVP);
                    lay.panel[npanel].bottom.point[k] = Zpoint(lay.panel[npanel].bottom.point[k]);
                }

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams and hems allowance
                if (j == 1)
                    dev.panel[npanel].addHems(0, seamW, 0, hemsW);
                else
                    dev.panel[npanel].addHems(0, seamW, 0, 0);

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

                lay.panel[npanel].left.fill(pt1 , pt3);
                lay.panel[npanel].right.fill(pt2 , pt4);
                lay.panel[npanel].bottom.fill(pt1 , pt2);
                lay.panel[npanel].top.fill(pt3 , pt4);

                // compute Z
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].left.point[k] = Zpoint(lay.panel[npanel].left.point[k]);
                for( k = 0; k < npl; k ++)
                    lay.panel[npanel].right.point[k] = Zpoint(lay.panel[npanel].right.point[k]);
                for( k = 0; k < npb; k ++)
                    lay.panel[npanel].top.point[k] = Zpoint(lay.panel[npanel].top.point[k]);
                for( k = 0; k < npb; k ++)
                    lay.panel[npanel].bottom.point[k] = Zpoint(lay.panel[npanel].bottom.point[k]);

                // We develop the current panel
                dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

                // We add the seams allowance
                dev.panel[npanel].addHems(seamW, seamW, 0, 0);

                nps[h]++;
                npanel++;
            }
        }

        /** cutting the leech side panels */
        for (j=1; j<nbGores-ngLuff; j++)
        {
            pt1 = leechCatenary[h] + CVector3d(leechH[h]-leechCatenary[h]) * (real(j-1)/(nbGores-ngLuff-1) );
            if (h == nbSections)
                pt1 = GaffIntersect( pt1, gaffVP );
            pt2 = leechCatenary[h-1] + CVector3d(leechH[h-1]-leechCatenary[h-1]) * (real(j-1)/(nbGores-ngLuff-1) );
            pt3 = leechCatenary[h] + CVector3d(leechH[h]-leechCatenary[h]) * (real(j)/(nbGores-ngLuff-1) );
            if (h == nbSections)
                pt3 = GaffIntersect( pt3, gaffVP );
            pt4 = leechCatenary[h-1] + CVector3d(leechH[h-1]-leechCatenary[h-1]) * (real(j)/(nbGores-ngLuff-1) );

            lay.panel[npanel].bottom.fill(pt1 , pt2);
            lay.panel[npanel].top.fill(pt3 , pt4);
            lay.panel[npanel].left.fill(pt1 , pt3);
            lay.panel[npanel].right.fill(pt2 , pt4);

            if (h == nbSections)
            {
                for (k=1; k<npl-1; k++)
                    lay.panel[npanel].left.point[k] = GaffIntersect(lay.panel[npanel].left.point[k], gaffVP );
            }

            if (j == nbGores-ngLuff-1)
            {
                for (k=1; k<npb-1; k++)
                    lay.panel[npanel].top.point[k] = LeechIntersect(lay.panel[npanel].top.point[k], CVector3d(luffH[h-1]-leechH[h-1]) );
            }

            // compute Z
            for( k = 0; k < npl; k ++)
                lay.panel[npanel].left.point[k] = Zpoint(lay.panel[npanel].left.point[k]);
            for( k = 0; k < npl; k ++)
                lay.panel[npanel].right.point[k] = Zpoint(lay.panel[npanel].right.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel].top.point[k] = Zpoint(lay.panel[npanel].top.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel].bottom.point[k] = Zpoint(lay.panel[npanel].bottom.point[k]);

            // We develop the current panel
            dev.panel[npanel] = lay.panel[npanel].develop(ALIGN_TOP);

            // We add the seams and hems allowance
            if ( h == nbSections && j == nbGores-ngLuff-1)
                dev.panel[npanel].addHems(hemsW, leechHemW, 0, 0);
            else if (j == nbGores-ngLuff-1)
                dev.panel[npanel].addHems(seamW, leechHemW, 0, 0);
            else
                dev.panel[npanel].addHems(seamW, seamW, 0, 0);

            nps[h]++;
            npanel++;
        }
    }

    /** copying 3d sail lay into sail */
    CPanelGroup sail(npanel);
    for( j = 0; j < npanel; j ++)
        sail.panel[j] = lay.panel[j];

    /** copy from temporary developed sail into flatsail */
    flatsail = CPanelGroup(npanel);

    for (j=0; j < npanel; j++)
        flatsail.panel[j] = dev.panel[j];

    /** preparing the displays version of the developed sail */
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
        dispsail.panel[j] = dispsail.panel[j] + v;

        CRect3d pRect = dispsail.panel[j].boundingRect();
        ym = pRect.height();
        v.y() += ym+ 2*seamW +20; // adding offset to separate next panel vertically

        x = pRect.max.x();
        if (x > xm)
            xm = x;
        k++;
    }
    ///
    return sail;
} ///////////// end layout triradial cut /////////  NOT USED ////////////

/** Creates a mitre cut sail.
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
    unsigned int npanelFoot=1, npanel=1;
    unsigned int npl= lay.panel[0].right.nbpoints();  // number of right/left points
    unsigned int npb= lay.panel[0].bottom.nbpoints(); // number of bottom/top points

    unsigned int j=0, k=0, cnt=0;
    bool flag=false;

    /* create arrays t1 and t2 of type of intersection
    *  respectively at points p1 on luff side and p2 on leech side
    *  t=1 seam intersect foot at point p1
    *  t=2 seam intersect luff
    *  t=3 seam intersect gaff
    *  t=4 seam intersect leech
    *  t=5 seam intersect mitre
    */
    int t1[MAX_PANELS], t2[MAX_PANELS];

    /* define point i for intersections */
    CPoint3d i;

    CSubSpace seamSP;

    /* create variables for the development and edge corrections */
    CPoint3d pt(0, 0, 0); // test point
    CPoint3d top(0, 0, 0);
    CPoint3d bot(0, 0, 0);
    CVector3d v(0, 0, 0);
    CVector3d vb(0, 0, 0);
    CVector3d vk(0, 0, 0);

    /* create variable for edge correction */
    CVector3d deviation [30];
    CVector3d deviaPrev [30];
    for (k=0; k<29; k++)
    {
        deviation[k] = CVector3d (0,0,0); // set all deviations to zero
        deviaPrev[k] = deviation[k];
    }

    /* create variable to monitor excess over cloth width */
    real exb=0, exc=0, ymax=0;

    /* seam 0 is on the foot of the sail ending at the clew */
    p1[0] = clew; // initialised at clew point
    p2[0] = clew;
    t1[0] = 1;
    t2[0] = 5;    // type=5=mitre intersection

    /** start cutting with the foot panels */
    for (npanel = 1; npanel < MAX_PANELS-1/2; npanel++)
    { // printf(" ----- FOR panel = %d \n" , npanel);
        exb=0;
        exc =0;
        cnt =0;
        //if (npanel==3) flag=true;

        do
        {   /** begin loop for optimising the seam position to fit cloth width */
            cnt++;
            pt = p1[npanel-1] - (clothW-seamW-exb)* footV.unit();
            seamSP = CSubSpace3d::line(pt, footVP);
            p1[npanel] = seamSP.intersect(foot).getp();
            t1[npanel] = 1; // type1=1=foot intersection vertically cut panels

            if (p1[npanel].x() <= tack.x())
            { // last panel
                p1[npanel]=tack;
                if (t2[npanel-1] == 5 )
                {
                    p2[npanel]=mitrePt;
                    t2[npanel]=5;
                }
                else
                {
                    p2[npanel]=tack;
                    t2[npanel]=2;
                }
                flag=true; // to get out of FOR
            }
            else
            { // printf ("normal panel \n");
                p2[npanel] = seamSP.intersect(mitre).getp();
                if (CVector3d(p2[npanel]-mitrePt)*mitreV > 0.00001)
                {
                    t2[npanel] = 2;
                    p2[npanel] = seamSP.intersect(luff).getp();
                }
                else
                {
                    t2[npanel] = 5;
                }
            }

            //// fill right side points
            if (t2[npanel-1]==2 && t2[npanel]==2)
            { // printf ("full luff \n");
                lay.panel[npanel-1].right.fill(p2[npanel-1], p2[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].right.point[k]=LuffIntersect(lay.panel[npanel-1].right.point[k], footVP);
            }
            else if ((t2[npanel-1]==5) && (t2[npanel]==2))
            { // printf ("mitre-luff \n");
                lay.panel[npanel-1].right.fill(p2[npanel-1], mitrePt, p2[npanel]);

                for (k=npl/2+1; k<npl; k++)
                    lay.panel[npanel-1].right.point[k]=LuffIntersect(lay.panel[npanel-1].right.point[k], footVP);
            }
            else
            { // printf ("full mitre \n");
                lay.panel[npanel-1].right.fill(p2[npanel-1], p2[npanel]);
            }

            //// fill left side points which are all on foot
            lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
            for (k=0; k<npl; k++)
                lay.panel[npanel-1].left.point[k]=FootIntersect(lay.panel[npanel-1].left.point[k], footVP);

            //// fill the intermediate points of bottom side
            lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);

            //// fill the intermediate points of top side on seam
            lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);
            if (flag == true && t2[npanel] == 5)
            {
                for (k=0 ; k<npb; k++)
                    lay.panel[npanel-1].top.point[k]=LuffIntersect(lay.panel[npanel-1].top.point[k], footV);
            }

            /** Now we go over all the points of the foot panel and calculate their z */
            for( k = 0; k < npl; k ++)
                lay.panel[npanel-1].left.point[k] = Zpoint(lay.panel[npanel-1].left.point[k]);
            for( k = 0; k < npl; k ++)
                lay.panel[npanel-1].right.point[k] = Zpoint(lay.panel[npanel-1].right.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel-1].top.point[k] = Zpoint(lay.panel[npanel-1].top.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel-1].bottom.point[k] = Zpoint(lay.panel[npanel-1].bottom.point[k]);

            /** Now we develop the current foot panel */
            if (npanel == 1)
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_TOP);
            else
            {
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for( k = 1; k < npb-1; k ++)
                    dev.panel[npanel-1].bottom.point[k] = dev.panel[npanel-1].bottom.point[k] + deviaPrev[k];
            }

            /** Now we compute the deviation of top edge of developed panel
             *   and straighten this top edge except if this is the top panel
             */
            if (flag == false)
            {
                vb= CMatrix::rot3d(2,PI/2)*CVector3d(dev.panel[npanel-1].top.point[npb-1] -dev.panel[npanel-1].top.point[0]).unit();
                for( k = 1; k < npb-1; k ++)
                {
                    vk= CVector3d (dev.panel[npanel-1].top.point[k] - dev.panel[npanel-1].top.point[0]);
                    v= vb * -(vk*vb);
                    deviation[k] = v;
                    dev.panel[npanel-1].top.point[k] = dev.panel[npanel-1].top.point[k] + deviation[k];
                }
            }

            /** Now we add the seam and hems allowance to the foot panels*/
            if (flag == true)
                dev.panel[npanel-1].addHems(hemsW, hemsW, 0, 0);
            else
                dev.panel[npanel-1].addHems(hemsW, seamW, 0, 0);

            /** Now we check the width of developed foot panel */
            ymax = dev.panel[npanel-1].height();
            exc= ymax-clothW; // current excess of width

            exb= exb + .8*exc +1; // sum previous correction + 80% of current excess of width +1mm

            //printf (" count %d ymax %f ymin %f \n", cnt,ymax,ymin);
            //printf (" count %d exc %f exb %f \n", cnt,exc,exb);

        }  /** loop DO as long as the excess of width > 0  AND counter <9 */
        while ( exc > 0 && cnt<9);
        /////////////////////////////////////////////////////////
        //printf ("P ----- maxW %f\n", (ymax-ymin));
        for (k=0; k<npb; k++)
            deviaPrev[k] = deviation[k];

        /** Now we reposition the developed panel such that bottom left is X=0 Y=0 */
        dev.panel[npanel-1] = dev.panel[npanel-1].reframe(LOW_LEFT);

        /** check if peak has been reached to break off */
        if (flag == true)
            break;
    } /** loop FOR next foot panel seam */
    /////////////////////////////////////////

    /** store the number of panels in foot */
    npanelFoot=npanel;
    if (npanel == MAX_PANELS/2 -1)
        throw CException("CSailDef::Layout5 : panelling of Foot got to MAX_PANELS/2 without reaching tack, do increase cloth width ");

    p1[npanel] = clew; // re-initialising at clew point
    p2[npanel] = clew;
    t1[npanel] = 5;    // type=5=mitre intersection
    t2[npanel] = 4;    // type=4=leech intersection
    flag=false;

    /** Now we position the seams perpendicular to the leech
    *   and determine the total number of panels.
    */
    for (npanel = npanel+1; npanel < MAX_PANELS-1; npanel++)
    {
        exb = 0; // total correction
        exc = 0; // current excess of width
        cnt = 0; // counter of iterations
        do
        {   /** loop for optimising the seam position to fit cloth width */
            cnt++;
            p2[npanel] = p2[npanel-1] + (clothW-seamW-exb) * leechV.unit();
            t2[npanel] = 4; // type2=4=leech intersection for all horizontally cut panels
            seamSP = CSubSpace3d::line(p2[npanel], leechVP);

            if (CVector3d(p2[npanel]-peak)*leechV > 0)
            { //// we are above peak, stop last panel
                flag=true;
                p2[npanel] = peak;

                // check on which side of the sail the previous point p1 is located
                if (t1[npanel-1] == 5 )
                { // previous seam on mitre
                    p1[npanel] = head;
                    t1[npanel] = 2; // set on luff
                    // left points on mitrePt-head
                    lay.panel[npanel-1].left.fill(p1[npanel-1], mitrePt, p1[npanel]);

                    for (k=npl/2+1; k<npl; k++)
                        lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                }
                else if (t1[npanel-1] == 2)
                { // left points on luff
                    p1[npanel] = head;
                    t1[npanel] = 2;
                    lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                }
                else
                { // left points on gaff
                    p1[npanel] = p1[npanel-1];
                    t1[npanel] = 3;
                    lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay.panel[npanel-1].left.point[k]=GaffIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                }

                //// fill right points on leech
                lay.panel[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].right.point[k]=LeechIntersect(lay.panel[npanel-1].right.point[k], leechVP);

                //// fill bottom points
                lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);

                //// fill top  points
                lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);

                // move all top points of top panel to gaff curve
                for( k=1; k < npb-1; k++)
                    lay.panel[npanel-1].top.point[k]=GaffIntersect(lay.panel[npanel-1].top.point[k], CVector3d (head.y()-peak.y(),peak.x()-head.x(),0));

                ////// end peak panel /////
            }
            else
            { //// normal panel  ///////////////////////////
                /* find position of luff/seam intersection relative to tack and head
                 * the case when the intersection is not a point needs to be handled
                 */
                if (seamSP.intersect(mitre).getdim() != 0)
                    throw CException("CSailDef::Layout5 : intersection of seam and mitre is not a point!");
                else
                    i = seamSP.intersect(mitre).getp();

                if (CVector3d(i-mitrePt)*mitreV < 0.00001)
                { // seam intersects mitre
                    p1[npanel] = i;
                    t1[npanel] =5;  // type=5=mitre intersection
                }
                else
                {
                    i = seamSP.intersect(luff).getp();

                    if (CVector3d(i-head)*luffV > 0)
                    { // seam intersects gaff
                        p1[npanel] = seamSP.intersect(gaff).getp();
                        t1[npanel] = 3;  // 3=gaff type of intersection
                    }
                    else
                    { // seam intersects luff
                        p1[npanel] = i;
                        t1[npanel] = 2;  // luff
                    }
                }

                /** We now add intermediate points on all sides of the normal panel */

                /* Below is the code for the left side depending
                 *  on t1 for the top side and bottom side
                 */
                if (t1[npanel-1] == 2 && t1[npanel] == 2)
                { // full luff
                    lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                }
                else if (t1[npanel-1] == 3 && t1[npanel] == 3)
                { // full gaff
                    lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                    for (k=0; k<npl; k++)
                        lay.panel[npanel-1].left.point[k]=GaffIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                }
                else if ((t1[npanel-1] == 2) && (t1[npanel] == 3))
                { // luff-head-gaff
                    lay.panel[npanel-1].left.fill(p1[npanel-1], head, p1[npanel]);
                    for (k=0; k<npl/2; k++)
                        lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                    for (k=npl/2+1; k<npl; k++)
                        lay.panel[npanel-1].left.point[k]=GaffIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                } // end IF ELSE for left side
                else if ((t1[npanel-1] == 5) && (t1[npanel] == 2))
                { // mitre-luff
                    lay.panel[npanel-1].left.fill(p1[npanel-1], mitrePt, p1[npanel]);
                    for (k=0; k<npl/2; k++)
                        lay.panel[npanel-1].left.point[k]=MitreIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                    for (k=npl/2+1; k<npl; k++)
                        lay.panel[npanel-1].left.point[k]=LuffIntersect(lay.panel[npanel-1].left.point[k], leechVP);
                }
                else
                { // full mitre
                    lay.panel[npanel-1].left.fill(p1[npanel-1], p1[npanel]);
                }

                //// Fill the intermediate points of the right side
                lay.panel[npanel-1].right.fill(p2[npanel-1],p2[npanel]);
                for (k=0; k<npl; k++)
                    lay.panel[npanel-1].right.point[k]=LeechIntersect(lay.panel[npanel-1].right.point[k], leechVP);

                //// Fill the intermediate points of the top side of the leech panel
                lay.panel[npanel-1].top.fill(lay.panel[npanel-1].left.point[npl-1], lay.panel[npanel-1].right.point[npl-1]);

                //// Fill the intermediate points of the bottom side of leech panel
                lay.panel[npanel-1].bottom.fill(lay.panel[npanel-1].left.point[0], lay.panel[npanel-1].right.point[0]);

            } //// end else normal panel ///////////////////////

            /** Now we go over all the points of the leech panel and calculate their z */
            for( k = 0; k < npl; k ++)
                lay.panel[npanel-1].left.point[k] = Zpoint(lay.panel[npanel-1].left.point[k]);
            for( k = 0; k < npl; k ++)
                lay.panel[npanel-1].right.point[k] = Zpoint(lay.panel[npanel-1].right.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel-1].top.point[k] = Zpoint(lay.panel[npanel-1].top.point[k]);
            for( k = 0; k < npb; k ++)
                lay.panel[npanel-1].bottom.point[k] = Zpoint(lay.panel[npanel-1].bottom.point[k]);

            /** Now we develop the current leech panel */
            if (npanel == npanelFoot+1)
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_TOP);
            else
            {
                dev.panel[npanel-1] = lay.panel[npanel-1].develop(ALIGN_BOTTOM);
                // add deviation of previous panel top edge to bottom edge
                for( k = 1; k < npb-1; k ++)
                    dev.panel[npanel-1].bottom.point[k] = dev.panel[npanel-1].bottom.point[k] + deviaPrev[k];
            }

            /** Now we compute the deviation of top edge of the developed leech panel
             *   and straighten this top edge except if this is the top panel
             */
            if (flag == false)
            {
                vb= CMatrix::rot3d(2,PI/2)*CVector3d(dev.panel[npanel-1].top.point[npb-1] -dev.panel[npanel-1].top.point[0]).unit();
                for( k = 1; k < npb-1; k ++)
                {
                    vk= CVector3d (dev.panel[npanel-1].top.point[k] - dev.panel[npanel-1].top.point[0]);
                    v= vb * -(vk*vb);
                    deviation[k] = v;
                    dev.panel[npanel-1].top.point[k] = dev.panel[npanel-1].top.point[k] + deviation[k];
                }
            }

            /* Now we add the seam and hems allowance to leech panels*/
            if (flag == true)
                dev.panel[npanel-1].addHems(seamW, hemsW, leechHemW, 0);
            else
                dev.panel[npanel-1].addHems(seamW, seamW, leechHemW, 0);

            /* Now we check the width of developed leech panel */
            ymax =  dev.panel[npanel-1].height();
            exc= ymax-clothW;

            exb= exb + .8*exc +1; // sum previous correction + 80% of current excess of width

            //printf ("P %d count %d exc %f exb %f \n", npanel,cnt,exc,exb);

        }
        /* now loop DO as long as the current excess of width is positive
         *  AND the counter is less than 9
         *  AND remaining cloth more than 10%
         */
        while ( exc > 0 && cnt < 9 );
        ////////////////////////////////////////////
        //printf ("P ----- maxW %f\n", (ymax-ymin));
        for (k=0; k<npb; k++)
            deviaPrev[k] = deviation[k];

        /* Now we reposition the developed panel such that bottom left is X=0 Y=0 */
        dev.panel[npanel-1] = dev.panel[npanel-1].reframe(LOW_LEFT);

        /* check if peak has been reached to break off */
        if (flag == true)
            break;
    }  /** loop FOR next seam of leech panels */
    ///////////////////////////////////////////////////////////////

    if (npanel == MAX_PANELS-1)
        throw CException("CSailDef::Layout5 : panelling leech got to MAX_PANELS without reaching head, do increase cloth width ");

    /** copying the sails for display */
    CPanelGroup sail(npanel);
    for( j = 0; j < npanel; j ++)
        sail.panel[j] = lay.panel[j];

    /** copying the developed sail into flatsail */
    flatsail = CPanelGroup(npanel);

    for (j=0; j < npanel; j++)
        flatsail.panel[j] = dev.panel[j];

    /** preparing the displays version of the developed sail */
    dispsail = flatsail;

    for (j=1; j < npanel; j++)
    {
        top = dispsail.panel[j-1].top.point[0];
        bot = dispsail.panel[j].bottom.point[0];

        // translation v to align panel bottom edge origin to previous panel upper edge origin
        v = top;
        v.x() -= bot.x();
        v.y() += 2*seamW +10; // adding offset to separate panels vertically

        dispsail.panel[j] = dispsail.panel[j] + v;
    }

    return sail;
} ///////////// end layout5 = mitre cut //////////////////////////////////


/** Routine used for computing the cord of the profiles.
 *  Return a 3d point which is the aft intersection of
 *  the horizontal line passing by p1 with the leech.
 *  If the point p1 is above or below the leech segment then
 *  the aft intersection is forced to be on the vertical of the
 *  corresponding peak or clew.
 *
 * @author Robert Laine
 */
CPoint3d CSailWorker::AftIntersect( const CPoint3d& p1 ) const
{
    // real x=0, y=0, z=0; // for debugging only

    CPoint3d li = p1; // aft intersection point is initialised at p1

    real hi=0, d1=0, d2=0, dh=0, dx=0;

    hi = (p1.y()-clew.y()) / (peak.y()-clew.y() + EPS); // compute the relative height on straight leech

    if ( hi >= 1 )  // rear point above the peak, set li on vertical above peak
    {
        li.x() = peak.x();
        li.z() = peak.z();
    }
    else if ( hi <= minH )  // rear point on or below the clew, set li on vertical below clew
    {
        li.x() = clew.x();
        li.z() = clew.z();
    }
    else   // point is on leech segment
    {
        if (fabs(leechR) < 1)
        { // leech is straight
            li.x() = clew.x() + (p1.y()-clew.y())*leechV.x()/leechV.y();
        }
        else
        {
            d1 = leechR * RoundP(hi , leechRP); // local depth of leech curve
            // displace point to leech curve
            dh = d1 * leechV.x() / leechV.y();
            d2 = leechR * RoundP(hi + dh / leechV.norm() , leechRP); // local depth of leech curve
            dx = (d1/(2*d1-d2)) * d1 * leechV.norm() / leechV.y();
            li.x() = clew.x() + (p1.y()-clew.y())*leechV.x()/leechV.y() + dx;
        }
    } // end IF else

    return li;
} //////////////// AftIntersect /////////////////////////////////////////


/** Routine for computing the area of the sail
 *
 * @author Robert Laine
 */
real CSailWorker::Area()
{
    real surface = luffV.cross(footV).norm() / 2;
    surface = surface + leechV.cross(gaffV).norm() / 2;
    surface = surface + .75*(luffL*luffR + footL*footR + leechL*leechR + gaffL*gaffR);
    return (.01*floor(surface /10000));
}


/** Routine for computing the diagonal head to clew
 *
 * @author Robert Laine
 */
real CSailWorker::Diagonal()
{
    return floor(CVector3d(head-clew).norm());
}


/** Routine for computing the width of the sail at a given
 *   relative height on the leech in accordance with IRC rule
 *
 * @author Robert Laine
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

    /// compute the point on leech for relative length HL
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

    /// compute the shortest distance to the real luff
    p1 = clew + leechV*h2;
    p2 = Zpoint(LeechIntersect(p1, leechVP));
    
    p1 = Zpoint(LuffIntersect(p2, luffVP));

    w = 0;
    p3 = p1;
    for (i=1; i<=imax; i++)
    {
        h1 = real(i) / imax;
        p = Zpoint(p1 +CVector3d(p2-p1)*h1);
        w = w+ CVector3d(p-p3).norm();
        p3 = p;
    }
    //
    return ( w );
}


/** Routine for computing the width of the sail at a given
 *   relative height on the luff and the leech
 *
 * @author Robert Laine
 */
real CSailWorker::SailWidth( const  real  &HL )
{
    unsigned int i, imax=10;
    real h=0, h1=0, h2=0, LeL=0, LuL=0, l1=0, w=0;
    CPoint3d p, p1, p2, p3;
    //printf ("IRC height = %f \n", HL);

    /// compute the leech edge length LeL
    h = 1;
    LeL = LeechLength( h );
    //printf ("IOR leech length = %f \n", LeL);

    /// compute the height of point on leech for relative length HL
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

    /// compute the luff edge length LuL
    h = 1;
    LuL = LuffLength( h );
    //printf ("IOR luff length = %f \n", LuL);

    /// compute the height of point on luff for relative length HL
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


    /// compute the shortest distance real leech to the real luff
    p = clew + leechV * h2;
    p2 = Zpoint(LeechIntersect(p , leechVP) );
    
    p = tack + luffV * h1;
    p1 = Zpoint(LuffIntersect(p , luffVP) );
    
    w = 0;
    p3 = p1;
    for (i = 1; i <= imax; i++)
    {
        h1 = real(i) / imax;
        p = Zpoint( p1 + CVector3d(p2 - p1) * h1 );
        w = w + CVector3d(p - p3).norm();
        p3 = p;
    }
    //
    return ( w );
}


/** Routine for computing the width LP of the sail
 *   perpendicular to the luff
 *
 * @author Robert Laine
 */
real CSailWorker::SailLP( )
{
    unsigned int i, imax=10;
    real h1=0,  w=0;
    CPoint3d p, p1, p2, p3;
    //printf ("IRC height = %f \n", HL);

    /// compute the distance to the real luff
    p1 = Zpoint(LuffIntersect(clew , luffVP) );
    p2 = clew;

    w = 0;
    p3 = p1;
    for ( i = 1; i <= imax; i++ )
    {
        h1 = real(i) / imax;
        p = Zpoint( p1 + CVector3d(p2 - p1) * h1 );
        w = w + CVector3d(p - p3).norm();
        p3 = p;
    }
    //
    return ( w );
}


/** Routine for computing the length of the leech edge
 *  up to a given relative heigth on straight leech line
 *
 * @author Robert Laine
 */
real CSailWorker::LeechLength( const real &h )
{
    unsigned int i=0, imax=20;
    CPoint3d p1, p2, p3;
    real l=0, h1=0;

    p1 = clew;

    for ( i = 1; i <= imax; i++ )
    {
        h1= real(i) / imax;
        p2 = clew + leechV * (h * h1);
        p3 = Zpoint( LeechIntersect(p2 , leechVP) );
        l = l + CVector3d(p3 - p1).norm();
        // printf ("step = %f - p2.y = %f - leech length = %f \n", h1, p2.y(), l);
        p1 = p3;
    }

    return ( l );
}


/** Routine for computing the length of the luff edge
 *  up to a given relative heigth on straight luff line
 *
 * @author Robert Laine
 */
real CSailWorker::LuffLength( const real &h )
{
    unsigned int i=0, imax=20;
    CPoint3d p1=tack, p2, p3;
    real l=0, h1=0;

    for ( i = 1; i < imax; i++)
    {
        h1= real(i) / imax;
        p2 = tack + luffV * (h * h1);
        p3 = Zpoint( LuffIntersect (p2 , luffVP) );
        l = l + CVector3d(p3 - p1).norm();
        p1 = p3;
    }

    return ( l );
}


/** Routine used for computing the cord of the profile.
 *  Return a 3d point which is the forward intersection of
 *  the horizontal line passing by p1 with either foot, luff or gaff.
 *
 * @author Robert Laine
 */
CPoint3d CSailWorker::FwdIntersect( const CPoint3d &p1 ) const
{
    //real x=0, y=0, z=0; // for debugging only
    real h1=0;  // relative position on luff/foot/gaff
    real d1=0, d2=0;  // local depth of forward curve
    real dh=0;  // change of h1
    real dx=0;  // change of X

    CPoint3d lu = p1; // forward intersection point initialised at p1

    if ( p1.y() >= head.y() )  // forward point is above luff
    {
        if ( p1.y() >= peak.y() - minH )  // point is above peak
            lu.x() = peak.x(); // set forward point on vertical above peak

        else if ( p1.y() == head.y() )  // point exactly at head height
            lu.x() = head.x();

        else  // forward point is on gaff segment
        {
            h1 = (p1.y() - head.y()) / (gaffV.y() + EPS);
            d1 = gaffR * RoundP(h1 , gaffRP);
            if (fabs(gaffR) < 1 )
            { // gaff is straight
                dx = 0;
            }
            else
            { // displace point to gaff curve
                dh = -d1 * gaffV.x() / gaffV.y();
                d2 = gaffR * RoundP(h1 + dh/gaffL , gaffRP);
                dx = (d1/(2*d1-d2)) * dh * gaffL/gaffV.x();
            }

            lu.x() = head.x() + (p1.y() - head.y())* gaffV.x() / (gaffV.y() + EPS) + dx;
        }
    }
    else if ( p1.y() <= tack.y() + minH )  // point is below luff
    {
        lu.x() = tack.x();  // set forward point on vertical below tack
        lu.z() = tack.z();
    }
    else  // forward point is on luff curve
    {
        if (fabs(luffR) < 1)
        { // luff is straight
            dx = 0;
        }
        else
        {
            h1 = (p1.y() - tack.y()) / (luffV.y() + EPS);
            d1 = luffR * RoundP(h1 , luffRP);
            // displace point to luff curve
            dh = -d1 * luffV.x() / (luffV.y() + EPS);
            d2 = luffR * RoundP(h1 + dh/luffL , luffRP);
            dx = (d1/(2*d1-d2)) * (d1 * luffL / (luffV.y() + EPS)) ;
        }

        lu.x() = tack.x() + (p1.y()-tack.y()) * luffV.x() / (luffV.y() + EPS) - dx;
    }

    return lu;
} ////////////// FwdIntersect ////////////////////////////////////////////


/** Routine used for computing the real position of foot points.
 *  Return a 3d point which is the forward intersection of the
 *  vector v1 passing by pt1 point inside sail area with the foot curve
 *
 * @author Robert Laine
 */
CPoint3d CSailWorker::FootIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const
{
    if (v1.norm() == 0)
        throw CException("CSailDef::FootIntersect : input vector is nul");
        
    CPoint3d p2 = pt1;

    /* useful straight lines */
    CSubSpace ptv1 = CSubSpace3d::line(pt1, v1);

    real h0=0;  /// a position on foot
    real hr0=0;  // its relative position
    real d0=0;   // its local depth of curve
    real hr1=0;  // its relative position
    real d1=0;   // its local depth
    real hr2=0;  // its relative position
    real d2=0;   // its local depth

    /* find point p0 at intersection of straight foot with ptv1 */
    CPoint3d p0 = ptv1.intersect(foot).getp();
        /* the case when the intersection is not a point needs to be handled */
    if (ptv1.intersect(foot).getdim() != 0)
        throw CException("CSailDef::FootIntersect-1 : intersection with foot is not a point!");

    if ( CVector3d(p0 - tack) * footV <= minH )
        p2 = tack;  // intersection left of tack
    else if ( CVector3d(p0 - clew)*footV >= 0 )
        p2 = clew;  // intersection right of clew
    else if ( fabs(footR) < 1)
        p2 = p0;    // foot is straight
    else
    {  // intersection is on a curved foot
        h0 = CVector3d(p0 - tack).norm();
        hr0 = h0 / (footL + EPS); // relative position
        d0 = footR * RoundP(hr0 , footRP); // local depth of foot curve

        /* loop to converge on real foot point */
        unsigned int j = 1;
        real  dh1 = 0 , dh2 = 0;
        CPoint3d p1 = p0;

        d1 = d0; // initialise

        for ( j = 1 ; j < 3 ; j++)
        { /* p1 is p0 moved on curve */
            p1 = p0 + CMatrix::rot3d(2 , -PI/2) * footV.unit() * d1;
            // define a line parrallel to foot at distance d1
            CSubSpace parallel = CSubSpace3d::line(p1, footV);
            // point intersection of ptv1 with parrallel
            p2 = ptv1.intersect(parallel).getp();
                /* the case when the intersection is not a point needs to be handled */
            if (ptv1.intersect(parallel).getdim() != 0)
                throw CException("CSailDef::FootIntersect-2 : intersection is not a point!");
                
            dh2 = CVector3d(p2 - p0) * footV.unit(); // projection on foot
            hr2 = (h0 + dh2) / (footL + EPS);
            if ( hr2 <= 0 )
                d2 = 0;
            else if ( hr2 >= 1 )
                d2 = 0;
            else
                d2 = footR * RoundP(hr2 , footRP); // local depth of foot curve

            dh1 = dh2 * d1 / (2*d1 - d2);
            hr1 = (h0 + dh1) / (footL + EPS);
            if ( hr1 <= 0 )
                d1 = 0;
            else if ( hr1 >= 1 )
                d1 = 0;
            else
                d1 = footR * RoundP(hr1 , footRP); // local depth of foot curve
        } // end FOR loop

        /* final convergence on mean point */
        d1 = (d1 + d2) / 2;
        p1 = p0 + CMatrix::rot3d(2 , -PI/2) * footV.unit() * d1;
        
        // define a line parrallel to leech at distance d1
        CSubSpace parallel = CSubSpace3d::line(p1,  footV);
        
        // point intersection of ptv1 with parrallel
        p2 = ptv1.intersect(parallel).getp();
            /* the case when the intersection is not a point needs to be handled */
        if (ptv1.intersect(parallel).getdim() != 0)
            throw CException("CSailDef::FootIntersect-3 : intersection is not a point!");
    } // end IF ELSE

    return p2;
} /////////////// FootIntersect ///////////////////////////////////////


/** Routine used for computing the real position of Gaff points.
 *  Return a 3d point which is the forward intersection of the
 *  vector v1 passing by pt1 point inside sail area with the gaff curve.
 *
 * @author Robert Laine
 */
CPoint3d CSailWorker::GaffIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const
{
    if (v1.norm() == 0)
        throw CException("CSailDef::GaffIntersect : input vector is nul");
    // real x=0, y=0, z=0; // for debugging only
    CPoint3d p2 = pt1;

    /* useful straight lines */
    CSubSpace ptv1 = CSubSpace3d::line(pt1, v1);

    real h0=0;  /// a position on gaff
    real hr0=0;  // its relative position
    real d0=0;   // its local depth of curve
    real hr1=0;  // its relative position
    real d1=0;   // its local depth
    real hr2=0;  // its relative position
    real d2=0;   // its local depth

    /* find point p0 at intersection of straight gaff with ptv1 */
    CPoint3d p0 = ptv1.intersect(gaff).getp();
        /* the case when the intersection is not a point needs to be handled */
    if (ptv1.intersect(gaff).getdim() != 0)
        throw CException("CSailDef::GaffIntersect-1 : intersection with gaff is not a point!");

    if (CVector3d(p0 - head) * gaffV <= minH)
        p2 = head;  // intersection left of gaff
    else if (CVector3d(p0 - peak) * gaffV >= 0 )
        p2 = peak;  // intersection above peak
    else if (fabs(gaffR) < 1)
        p2 = p0;    // gaff is straight
    else
    {  // intersection is on curved gaff
        h0 = CVector3d(p0 - head).norm();
        hr0 = h0 / (gaffV.norm() + EPS); // relative height
        d0 = gaffR * RoundP(hr0 , gaffRP); // local depth of gaff curve

        /* loop to converge on real gaff point */
        unsigned int j=1;
        real  dh1=0, dh2=0;
        CPoint3d p1=p0;

        d1 = d0; // initialise

        for ( j = 1;  j<3;  j++)
        {
            p1 = p0 + CMatrix::rot3d(2, PI/2) * gaffV.unit() * d1;
            // define a line parrallel to gaff at distance d1
            CSubSpace parallel = CSubSpace3d::line(p1 , gaffV);

            // point intersection of ptv1 with parrallel
            p2 = ptv1.intersect(parallel).getp();
                /* the case when the intersection is not a point needs to be handled */
            if (ptv1.intersect(parallel).getdim() != 0)
                throw CException("CSailDef::GaffIntersect-2 : intersection is not a point!");

            dh2 = CVector3d(p2 - p0) * gaffV.unit(); // projection on gaff
            hr2 = (h0 + dh2) / (gaffV.norm() + EPS);
            if ( hr2 <= 0 )
                d2 = 0;
            else if ( hr2 >= 1 )
                d2 = 0;
            else
                d2 = gaffR * RoundP(hr2 , gaffRP); // local depth of gaff curve

            dh1 = dh2 * d1 / (2*d1 - d2);
            hr1 = (h0 + dh1) / (gaffV.norm() + EPS);
            if ( hr1 <= 0 )
                d1 = 0;
            else if (hr1 >= 1)
                d1 = 0;
            else
                d1 = gaffR * RoundP(hr1 , gaffRP); // local depth of curve
        } // end FOR loop

        // final convergence on mean point
        d1 = (d1 + d2) / 2;
        p1 = p0 + CMatrix::rot3d(2 , PI/2) * gaffV.unit()*d1;
        
        // define a line parrallel to leech at distance d1
        CSubSpace parallel = CSubSpace3d::line(p1 , gaffV);
        
        // point intersection of ptv1 with parrallel
        p2 = ptv1.intersect(parallel).getp();
            /* the case when the intersection is not a point needs to be handled */
        if (ptv1.intersect(parallel).getdim() != 0)
            throw CException("CSailDef::GaffIntersect-3 : intersection is not a point!");
    } // end ELSE
    return p2;
} //////////////// GaffIntersect //////////////////////////////////////////


/** Routine used for computing the real position of leech points.
 *  Return a 3d point which is the forward intersection of the
 *  vector v1 passing by pt1 point inside sail area with the leech curve
 *
 * @author Robert Laine
*/
CPoint3d CSailWorker::LeechIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const
{
    if (v1.norm() == 0)
        throw CException("CSailDef::LeechIntersect : input vector is nul");
        
    // real x=0, y=0, z=0; // for debugging only
    CPoint3d p2 = pt1;

    /* useful straight lines */
    CSubSpace ptv1 = CSubSpace3d::line(pt1, v1);

    real h0=0;  /// a position on leech
    real hr0=0;  // its relative position on leech
    real d0=0;   // its local depth of leech curve
    real hr1=0;  // its relative position
    real d1=0;   // its local depth
    real hr2=0;  // its relative position
    real d2=0;   // its local depth

    /* find point p0 at intersection of straight leech with ptv1 */
    CPoint3d p0 = ptv1.intersect(leech).getp();
        /* the case when the intersection is not a point needs to be handled */
    if (ptv1.intersect(leech).getdim() != 0)
        throw CException("CSailDef::LeechIntersect-1 : intersection with leech is not a point!");

    if (CVector3d(p0 - clew) * leechV <= minH)
        p2 = clew;  // intersection below clew
    else if (CVector3d(p0 - peak) * leechV >= 0 )
        p2 = peak;  // intersection above peak
    else if (fabs(leechR) < 1)
        p2 = p0;    // leech is straight
    else
    {  // intersection is on curved leech
        h0 = CVector3d(p0 - clew).norm();
        hr0 = h0 / (leechV.norm() + EPS); // relative height
        d0 = leechR * RoundP(hr0 , leechRP); // local depth of leech curve

        /* loop to converge on real leech point */
        unsigned int j = 1;
        real dh1 = 0, dh2 = 0;
        CPoint3d p1 = p0;

        d1 = d0; // initialise

        for ( j = 1; j < 3; j++)
        {
            p1 = p0 + CMatrix::rot3d(2, -PI/2) * leechV.unit() * d1;
            
            // define a line parrallel to leech at distance d1
            CSubSpace parallel = CSubSpace3d::line(p1 , leechV);
            
            // point intersection of ptv1 with parrallel
            p2 = ptv1.intersect(parallel).getp();
            /* the case when the intersection is not a point needs to be handled */
            if (ptv1.intersect(parallel).getdim() != 0)
                throw CException("CSailDef::LeechIntersect-2 : intersection is not a point!");

            dh2 = CVector3d(p2 - p0) * leechV.unit(); // projection on leech
            hr2 = (h0 + dh2) / (leechV.norm() + EPS);
            if (hr2 <= 0)
                d2 = 0;
            else if (hr2 >= 1)
                d2 = 0;
            else
                d2 = leechR * RoundP(hr2 , leechRP); // local depth of leech curve

            dh1 = dh2 * d1 / (2 * d1 - d2);
            hr1 = (h0 + dh1) / (leechV.norm() + EPS);
            if (hr1 <= 0)
                d1 = 0;
            else if (hr2 >= 1)
                d1 = 0;
            else
                d1 = leechR * RoundP(hr1 , leechRP); // local depth of leech curve
        } // end FOR loop

        // final convergence on mean point
        d1 = (d1 + d2) / 2;
        p1 = p0 + CMatrix::rot3d(2 , -PI/2) * leechV.unit() * d1;
        
        // define a line parrallel to leech at distance d1
        CSubSpace parallel = CSubSpace3d::line(p1 , leechV);
        
        // point intersection of ptv1 with parrallel
        p2 = ptv1.intersect(parallel).getp();
            /* the case when the intersection is not a point needs to be handled */
        if (ptv1.intersect(parallel).getdim() != 0)
            throw CException("CSailDef::LeechIntersect-3 : intersection is not a point!");
    } // end ELSE
    return p2;
} /////////// LeechIntersect ///////////////////////////////////////////


/** Routine used for computing the real position of luff points.
 *  Return a 3d point which is the forward intersection of the
 *  vector v1 passing by pt1 point inside sail area with the luff curve
 *
 * @author: Robert Laine
 */
CPoint3d CSailWorker::LuffIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const
{
    if (v1.norm() == 0)
        throw CException("CSailDef::LuffIntersect : input vector is nul");
            
    // real x=0, y=0, z=0; // for debugging only
    CPoint3d p2=pt1;

    /* useful straight lines */
    CSubSpace ptv1 = CSubSpace3d::line(pt1 , v1);

    real h0=0;  // a position on luff
    real hr0=0;  // its relative position on luff
    real d0=0;  //  its local depth of luff curve
    real hr1=0;  // its relative position on luff
    real d1=0;  //  its local depth of luff curve
    real hr2=0;  // its relative position on luff
    real d2=0;  //  its local depth of luff curve

    /* find point p0 at intersection of straight luff with ptv1 */
    CPoint3d p0 = ptv1.intersect(luff).getp();
    /* the case when the intersection is not a point needs to be handled */
    if (ptv1.intersect(luff).getdim() != 0)
        throw CException("CSailDef::LuffIntersect-1 : intersection with luff is not a point!");

    if (CVector3d(p0 - tack) * luffV <= minH)
        p2 = tack;  // intersection below tack
    else if (CVector3d(p0 - head) * luffV >= 0 )
        p2 = head;  // intersection above head
    else if (fabs(luffR) < 1)
        p2 = p0;    // luff is straight
    else
    {  // intersection is on curved luff
        h0 = CVector3d(p0 - tack).norm();
        hr0 = h0 / (luffV.norm() + EPS); // relative height
        d0 = luffR * RoundP(hr0, luffRP); // local depth of luff curve

        /* loop to converge on real luff point */
        unsigned int j = 1;
        real dh1 = 0, dh2 = 0;
        CPoint3d p1 = p0;

        d1 = d0; // initialise

        for ( j = 1; j < 4; j++)
        {
            p1 = p0 + CMatrix::rot3d(2 , PI/2) * luffV.unit() * d1;
            // define a line parrallel to luff at distance d1
            CSubSpace parallel = CSubSpace3d::line(p1 , luffV);
            // point intersection of ptv1 with parrallel to luff
            p2 = ptv1.intersect(parallel).getp();
            /* the case when the intersection is not a point needs to be handled */
            if (ptv1.intersect(parallel).getdim() != 0)
                throw CException("CSailDef::LuffIntersect-2 : intersection is not a point!");

            dh2 = CVector3d(p2 - p0) * luffV.unit(); // projection on luff
            hr2 = (h0+dh2) / (luffV.norm() + EPS);
            if (hr2 <= 0)
                d2 = 0;
            else if (hr2 >= 1)
                d2 = 0;
            else
                d2 = luffR * RoundP(hr2 , luffRP); // local depth of luff curve

            dh1 = dh2 * d1 / (2*d1-d2);
            hr1 = (h0 + dh1) / (luffV.norm() + EPS);
            if (hr1 <= 0)
                d1 = 0;
            else if (hr2 >= 1)
                d1 = 0;
            else
                d1 = luffR * RoundP(hr1, luffRP); // local depth of luff curve
        } // end FOR loop

        // final convergence on mean point
        d1 = (d1 + d2) / 2;
        p1 = p0 + CMatrix::rot3d(2 , PI/2) * luffV.unit() * d1;
        // define a line parrallel to luff at distance d1
        CSubSpace parallel = CSubSpace3d::line(p1 , luffV);
        // point intersection of ptv1 with parrallel to luff
        p2 = ptv1.intersect(parallel).getp();
        /* the case when the intersection is not a point needs to be handled */
        if (ptv1.intersect(parallel).getdim() != 0)
            throw CException("CSailDef::LuffIntersect-3 : intersection is not a point!");
    } // end ELSE
    return p2;
} ////////////// LuffIntersect /////////////////////////////////////////


/** Routine used for computing the intersection with mitre line.
 *  Return a 3d point which is the intersection of the
 *  vector v1 passing by pt1 point with the mitre line.
 *
 * @author: Robert Laine
 */
CPoint3d CSailWorker::MitreIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const
{
    if (v1.norm() == 0)
        throw CException("CSailDef::MitreIntersect : input vector is nul");
    // real x=0, y=0, z=0; // for debugging only
    CPoint3d p2 = pt1;

    if (CVector3d(p2 - clew).norm() < minH)
        p2 = clew;
    else
    {
        /* straight line passing through input point */
        CSubSpace ptv1 = CSubSpace3d::line(pt1 , v1);

        /* point at intersection of input vector and mitre */
        p2 = ptv1.intersect(mitre).getp();
        /* the case when the intersection is not a point needs to be handled */
        if (ptv1.intersect(mitre).getdim() != 0)
            throw CException("CSailDef::MitreIntersect-1 : intersection with mitre is not a point!");
    }
    return p2;
} ///////////// MitreIntersect ////////////////////////////////////////


/** Routine used for computing the Z of any point of the sail.
 *  Return a CPoint3D p2 which is p1 point with its Z modified
 *
 * @author Robert Laine
*/
CPoint3d CSailWorker::Zpoint( const CPoint3d &p1 ) const
{
    CPoint3d p2 = p1;            // will be the returned point with Z added

    real x=0,  z=0, twist=0, pivotX=0, pos=0;
    CPoint3d pFwd = FwdIntersect( p1 );   // forward end of the cord
    CPoint3d pAft = AftIntersect( p1 );    // rear end of the cord

    /* computing local cord of the profile */
    real cord = CVector3d(pAft - pFwd).norm();

    /* computing Z from normalised position on profile */
    if ( cord < minH )   // to avoid division by cord = zero
    {
        pos = 0 ;
        z = 0;
    }
    else   // position on profile
    {
        pos = ( CVector3d(p1 - pFwd).norm() ) / cord;
        /* computing the relative height on the sail */
        real h1 = (p1.y() - tack.y()) / (peak.y() - tack.y() + EPS);  // for mould
        /* computing actual Z  including twist and sheeting angle
        *  kluff, kleech, depth coefficient used in z1=f(pos, h1) of the
        *  sailmould are interpolated between the respective profile[0..2].
        *  The position of profile[1] is driven by vertpos which is
        *  the vertical position of the middle profile entered in the
        *  left vertical pane of formmouldbase.
        */
        z = cord * mould.interpol( h1 ).z(pos);
    }

    /* computing the twist from the relative height on straight leech */
    real h2 = (p1.y() - clew.y()) / (peak.y() - clew.y() + EPS);  // for twist
    // h2 range is limited to 0..1
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
} /////////////// Zpoint ///////////////////////////////////////////////

