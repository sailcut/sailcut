/*
 * Copyright (C) 1993-2015 Robert & Jeremy Laine
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

#include <cstdio>

#include <sys/time.h>

#include "hullworker.h"
#include "sailworker.h"

static struct timeval start_time;

static void tic()
{
    gettimeofday(&start_time, NULL);
}

static double toc()
{
  struct timeval end;
  gettimeofday(&end, NULL);
  return (end.tv_sec - start_time.tv_sec) + (double)(end.tv_usec - start_time.tv_usec) / 1000000;
}

int main()
{
    const size_t iters = 100;
    CMatrix rot = CMatrix::rot3d(1, PI/3);

    // output
    CPanelGroup output, output2;
    CRect3d rect;

    printf("Running %i iterations for each operation\n", int(iters));

    // SAIL OPERATIONS
    CSailDef saildef;
    CSailWorker sailworker(saildef);

    // sail creation
    tic();
    for (size_t i = 0; i < iters; i++)
        output = sailworker.makeSail();
    printf("Sail creation: %lf iterations/sec\n", double(iters) / toc());

    // sail rotation
    tic();
    for (size_t i = 0; i < iters; i++)
        output2 = output.rotate(1, rot);
    printf("Sail rotation: %lf iterations/sec\n", double(iters) / toc());

    // bounding rectangle calculation
    tic();
    for (size_t i = 0; i < iters; i++)
        rect = output2.boundingRect();
    printf("Sail bounding rect: %lf iterations/sec\n", double(iters) / toc());

    // HULL OPERATIONS
    CHullDef hulldef;
    CHullWorker hullworker(hulldef);

    // hull creation
    tic();
    for (size_t i = 0; i < iters; i++)
        output = hullworker.makeHull();
    printf("Hull creation: %lf iterations/sec\n", double(iters) / toc());

    // hull rotation
    tic();
    for (size_t i = 0; i < iters; i++)
        output2 = output.rotate(1, rot);
    printf("Hull rotation: %lf iterations/sec\n", double(iters) / toc());

    return 0;
}
