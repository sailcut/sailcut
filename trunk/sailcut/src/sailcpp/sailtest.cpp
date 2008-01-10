#include "sailworker.h"

int main()
{
    CSailDef def;
    CSailWorker worker(def);

    // output
    CPanelGroup sail;
    CPanelGroup sail2;
    CRect3d rect;

    for (size_t i = 0; i < 10; i++)
    {
        // create sail
        sail = worker.makeSail();

        // rotate sail
        CMatrix rot = CMatrix::rot3d(1, PI/3);
        sail2 = sail.rotate(1, rot);

        // calculate bounding rectangle
        rect = sail2.boundingRect();
    }
    return 0;
}
