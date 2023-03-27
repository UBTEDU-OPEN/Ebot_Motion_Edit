#ifndef CIMPORTMP3PROGRESS_H
#define CIMPORTMP3PROGRESS_H

#include <QWidget>

#include "ubxdialog.h"
#include "mp3projconstants.h"

using namespace Constants;

namespace Ui {
class CImportMP3Progress;
}

class CImportMP3Progress : public CUBXDialog
{
    Q_OBJECT

public:
    explicit CImportMP3Progress(QWidget *parent = 0);
    ~CImportMP3Progress();

public slots:
    void slotOnUpdateMP3Info(MP3Info stMP3Info);

private:
    void updateData(MP3Info stMP3Info);

private:
    Ui::CImportMP3Progress *ui;
};

#endif // CIMPORTMP3PROGRESS_H
