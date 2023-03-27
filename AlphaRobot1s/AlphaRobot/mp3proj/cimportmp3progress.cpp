#include "cimportmp3progress.h"
#include "ui_cimportmp3progress.h"

CImportMP3Progress::CImportMP3Progress(QWidget *parent) :
    CUBXDialog(parent),
    ui(new Ui::CImportMP3Progress)
{
    ui->setupUi(this);

    ui->progressBarImportMP3->hide();

    connect(ui->widgetTitleBar, &MyTitleBar::sigClose, this, &CImportMP3Progress::close);
}

CImportMP3Progress::~CImportMP3Progress()
{
    delete ui;
}

void CImportMP3Progress::slotOnUpdateMP3Info(MP3Info stMP3Info)
{
    updateData(stMP3Info);
}

void CImportMP3Progress::updateData(MP3Info stMP3Info)
{
    ui->labMP3Name->setText(stMP3Info.m_strMP3FilePath);
    ui->labMP3DurationValue->setText(QString("%1").arg(stMP3Info.m_nDuration));
    ui->labMP3TempoValue->setText(QString("%1").arg(stMP3Info.m_nBPM));
}
