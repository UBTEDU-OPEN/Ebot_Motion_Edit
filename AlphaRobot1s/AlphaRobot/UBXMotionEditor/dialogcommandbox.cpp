#include "dialogcommandbox.h"
#include "ui_dialogcommandbox.h"

DialogCommandBox::DialogCommandBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCommandBox)
{
    ui->setupUi(this);

    setWindowTitle(tr("Save Action Template"));
    m_strCommandBoxName = "";
    ui->lineEditBlockName->setAttribute(Qt::WA_MacShowFocusRect, false);
}

DialogCommandBox::~DialogCommandBox()
{
    delete ui;
}

void DialogCommandBox::on_btnOK_clicked()
{
    QString strText = ui->lineEditBlockName->text();
    m_strCommandBoxName = strText;
    done(QDialog::Accepted);
}

void DialogCommandBox::on_btnCancel_clicked()
{
    done(QDialog::Rejected);
}
