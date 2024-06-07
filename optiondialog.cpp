#include "optiondialog.h"
#include "ui_optiondialog.h"
#include <QFileDialog>
#include <QDesktopServices>
#include "mainwindow.h"
#include "common.h"

optionDialog::optionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::optionwindow)
{
    ui->setupUi(this);
    auto &setting = ((MainWindow*)parent)->m_settings;
    setting.beginGroup(OPTION);
    QString strPath = setting.value(LIBREOFFICE_PATH,DEFAULT_LIBREOFFICE_PATH).toByteArray();
    setting.endGroup();

    ui->EditLibreOffice->setText(strPath);
}

optionDialog::~optionDialog()
{
    delete ui;
}

void optionDialog::closeEvent(QCloseEvent *event)
{


    QDialog::closeEvent(event);
}

void optionDialog::on_BtnSelFile_clicked()
{
    QString strPath = QFileDialog::getExistingDirectory(this,"选择LibreOffice所在目录",DEFAULT_LIBREOFFICE_PATH);
    if(strPath.isEmpty()) return;

    ui->EditLibreOffice->setText(strPath);
}


void optionDialog::on_btnDownload_clicked()
{
    QDesktopServices::openUrl(QUrl("https://zh-cn.libreoffice.org/download/libreoffice/"));
}


void optionDialog::on_btnBaidu_clicked()
{
#if defined(Q_OS_WIN)
    QDesktopServices::openUrl(QUrl("https://pan.baidu.com/s/16iZFDR20Yn-BCRBaGkvpVQ?pwd=tcr6"));
#elif defined(Q_OS_MACOS)
    QDesktopServices::openUrl(QUrl("https://pan.baidu.com/s/1Lfl8vB7T_HagS70RRXdayA?pwd=ut8b"));
#else
// 其他平台代码
#endif

}


void optionDialog::on_optionwindow_finished(int result)
{
    QSettings setting(qApp->applicationDirPath()+"/userdata.ini",QSettings::IniFormat);
    setting.beginGroup(OPTION);
    setting.setValue(LIBREOFFICE_PATH,ui->EditLibreOffice->text());
    setting.endGroup();
}

