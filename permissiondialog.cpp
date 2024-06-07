#include "permissiondialog.h"
#include "ui_permissiondialog.h"
#include "permissionmanager.h"
#include <QClipboard>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QWidget>
#include "common.h"
#include <QDateTime>

PermissionDialog::PermissionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PermissionDialog)
{
    ui->setupUi(this);

    QString strRegCode = PermissionManager::generateRegCode();
    QSettings setting(PER_FILE_PATH, QSettings::IniFormat);
    setting.beginGroup("code");
    strRegCode = setting.value("reg",strRegCode).toByteArray();
    QString strActCode = setting.value("activate","").toByteArray();
    setting.endGroup();

    ui->EditReg->setText(strRegCode);
    ui->EditActivate->setText(strActCode);

    if(strActCode != PermissionManager::generateActivationCode(strRegCode))
    {
        QFileInfo fileInfo(PER_FILE_PATH);
        QDateTime fileTime = fileInfo.birthTime();
        int nDays = fileTime.daysTo(QDateTime::currentDateTime());
        int nShen = 7-nDays;
        nShen < 0 ? ui->labelTime->setText("试用期已结束！")
                  : ui->labelTime->setText("试用期还剩余" + QString::number(nShen)+"天！");

        return;
    }
    else
    {
        ui->labelTime->setText("<html><head/><body><p><span style=\" color:#ff0000;\">您已激活！感谢您使用FinnConverter!</span></p></body></html>");
    }

}

PermissionDialog::~PermissionDialog()
{
    delete ui;
}

void PermissionDialog::on_btnCopyReg_clicked()
{
    QClipboard *pClip = QApplication::clipboard();
    pClip->setText(ui->EditReg->text());

    QSettings setting(PER_FILE_PATH, QSettings::IniFormat);
    setting.beginGroup("code");
    setting.setValue("reg",ui->EditReg->text());
    setting.endGroup();
    QMessageBox::information(this,NULL,"复制成功！");
}


void PermissionDialog::on_btnActivate_clicked()
{
    QString strRegCode = ui->EditReg->text();
    qDebug()<<"注册码："<<strRegCode;
    QString StrActCode = PermissionManager::generateActivationCode(strRegCode);
    qDebug()<<"激活码："<<StrActCode;
    if(StrActCode == ui->EditActivate->text())
    {
        QSettings setting(PER_FILE_PATH, QSettings::IniFormat);
        setting.beginGroup("code");
        setting.setValue("reg",strRegCode);
        setting.setValue("activate",StrActCode);
        setting.endGroup();
        ui->labelTime->setText("<html><head/><body><p><span style=\" color:#ff0000;\">您已激活！感谢您使用FinnConverter!</span></p></body></html>");
        QMessageBox::information(this,NULL,"激活成功！感谢您的使用！");
    }
    else
    {
        ui->labelTime->setText("<html><head/><body><p><span style=\" color:#ff0000;\">激活失败!</span></p></body></html>");
        QMessageBox::warning(this,NULL,"请检查您的激活码是否正确，或联系FinnSoft！");
    }
}

