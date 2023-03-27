#include "dialogabout.h"
#include "ui_dialogabout.h"

#include <QDir>

#include "../common/UBXPublic.h"
#include "../common/Util.h"
#include "../zxLib/configs.h"


RETRANSLATEUIFUNCIMPL(DialogAbout)
DialogAbout::DialogAbout(QWidget *parent) :
    CUBXDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
	connect(ui->pBClose, &QPushButton::clicked, this, &DialogAbout::onCloseDlgClicked);
    loadInfo();
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::onCloseDlgClicked()
{
    close();
}

void DialogAbout::loadInfo()
{
    QString strIcon = "";
    QString sIconTempPath = "";

#ifdef Q_OS_MAC
    sIconTempPath = "AlphaRobot.icns";
#elif defined(Q_OS_WIN)
    sIconTempPath = "AlphaRobot.ico";
#endif    
    strIcon = CConfigs::getLocalResPath(sIconTempPath);

    QImage *image=new QImage(strIcon);
    ui->IconLable->setGeometry(0,0,500,500);
    ui->IconLable->setPixmap(QPixmap::fromImage(*image)); //显示图片

    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    ui->label_appname->setText(fileinfo.sProductName + " " + fileinfo.sFileVer);
    //ui->label_version->setText(fileinfo.sFileVer);
    ui->label_version->setVisible(false);
    ui->label_company->setText("www.ubtrobot.com");
    ui->label_copyright->setText(fileinfo.sLegalCopyright);
}
