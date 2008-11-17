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
