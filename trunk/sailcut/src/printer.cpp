#include "printer.h"
#include "sailpainter.h"


/** Display a dialog then print.
  *
  * @param orientation the initial page orientation
  */
void CPrinter::printDialog(enum QPrinter::Orientation orientation) const
{
    // try printing
    try
    {
        QPrinter myprinter;
        myprinter.setOrientation(orientation);
        myprinter.setFullPage(FALSE);

        // print preview
/*
        CFormPrint dlg(this);
        dlg.exec();
*/

        QPrintDialog printDialog(&myprinter);
        if ( printDialog.exec() == QDialog::Accepted )
        {
            CTextPainter painter(&myprinter);
            for (size_t i = 0; i < pages(); i ++)
            {
                if ( i > 0 )
                    myprinter.newPage();
                print(&painter, i);
            }
        }
    }
    catch (CException e)
    {
        QMessageBox::information(NULL, tr("error"), tr("There was a printing error"));
    }
}

