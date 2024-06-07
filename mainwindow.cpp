#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "convertertool.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>
#include <QDebug>
#include <thread>
#include <QDateTime>
#include "optiondialog.h"
#include "common.h"
#include "permissiondialog.h"
#include "permissionmanager.h"
#include <QProcess>

#define INPUT_TYPE "输入格式"
#define OUTPUT_TYPE "输出格式"
#define INPUT_PATH "输入路径"
#define OUTPUT_PATH "输出路径"
#define BAT "批量转换"
#define WIDTH "窗口宽度"
#define HEIGHT "窗口高度"
#define FINDER "访达"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_settings(qApp->applicationDirPath()+"/userdata.ini",QSettings::IniFormat)
{
    ui->setupUi(this);

    QString str = PER_FILE_PATH;
    QFile filePer(PER_FILE_PATH);
    QFileInfo fileInfo(PER_FILE_PATH);
    QDir dir = fileInfo.dir();
    if(!dir.exists())
    {
            dir.mkpath(dir.path());
    }
    if(!filePer.open(QIODevice::ReadWrite))
    {
         qDebug() << filePer.errorString();
    }
    filePer.close();

#if defined(Q_OS_WIN)
    // Windows 平台代码
    SetFileAttributes(PER_FILE_PATH.toStdWString().c_str() , FILE_ATTRIBUTE_HIDDEN);
    ui->checkOpenFinder->setText("转换完成后，在文件资源管理器中打开");
#endif
    QString strRegCode = PermissionManager::generateRegCode();
    QSettings setting(PER_FILE_PATH, QSettings::IniFormat);
    setting.beginGroup("code");
    strRegCode = setting.value("reg",strRegCode).toByteArray();
    QString strActCode = setting.value("activate","").toByteArray();
    setting.endGroup();

    if(strActCode == PermissionManager::generateActivationCode(strRegCode))
    {
        m_bIsVIP = true;
    }

    auto readFormats = G_ConverterTool.m_readFormats;
    qDebug() << "INPUT FORMAT SUPPORT:";
    for(const auto& format : readFormats)
    {
        ui->cmbInput->addItem(format);
        qDebug() << format;
    }

    auto writeForamts = G_ConverterTool.m_writeFormats;
    qDebug() << "OUTPUT FORMAT SUPPORT:";
    for (const auto& format : writeForamts)
    {
        ui->cmbOutput->addItem(format);
        qDebug() << format;
    }

    m_settings.beginGroup(qApp->applicationName());
    QString strInputType = m_settings.value(INPUT_TYPE,"png").toString();
    QString strOutputType = m_settings.value(OUTPUT_TYPE,"ico").toString();
    QString strInputPath = m_settings.value(INPUT_PATH,"").toString();
    QString strOutputPath = m_settings.value(OUTPUT_PATH,"").toString();
    int nWidth = m_settings.value(WIDTH, 224).toInt();
    int nHeight = m_settings.value(HEIGHT, 450).toInt();
    bool bBat = m_settings.value(BAT,false).toBool();
    bool bOpenFinder = m_settings.value(FINDER,true).toBool();
    m_settings.endGroup();

    ui->cmbInput->setCurrentText(strInputType);
    ui->cmbOutput->setCurrentText(strOutputType);
    ui->EditInput->setText(strInputPath);
    ui->EditOutput->setText(strOutputPath);
    this->resize(nWidth,nHeight);
    bBat ? ui->radioBat->setChecked(true)
         : ui->radioSingle->setChecked(true);
    ui->checkOpenFinder->setChecked(bOpenFinder);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_settings.beginGroup(qApp->applicationName());
    m_settings.setValue(INPUT_TYPE,ui->cmbInput->currentText());
    m_settings.setValue(OUTPUT_TYPE,ui->cmbOutput->currentText());
    m_settings.setValue(INPUT_PATH,ui->EditInput->text());
    m_settings.setValue(OUTPUT_PATH,ui->EditOutput->text());
    m_settings.setValue(BAT,ui->radioBat->isChecked());
    m_settings.setValue(FINDER, ui->checkOpenFinder->isChecked());
    m_settings.setValue(WIDTH, this->frameGeometry().width());
    m_settings.setValue(HEIGHT, this->frameGeometry().height());
    m_settings.endGroup();
    QMainWindow::closeEvent(event);
}

void MainWindow::on_btnClose_clicked()
{
    close();
}

void ReplaceFormat(QString &strPath, const QString &format)
{
    strPath = strPath.left(strPath.lastIndexOf(".") + 1);
    strPath += format;
}

QStringList getFileListUnderDir(const QString &dirPath, const QString &format)
{
    QStringList fileList;
    QDir dir(dirPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (auto fileInfo, fileInfoList)
    {
        if(fileInfo.isDir())
        {

            getFileListUnderDir(fileInfo.absoluteFilePath(), format);
        }

        if(fileInfo.isFile())
        {
            QString strPath = fileInfo.absoluteFilePath();
            QString strExt = strPath.right(strPath.size()
                                               - strPath.lastIndexOf(".") - 1);
            strExt = strExt.toLower();
            if(strExt == format)
            {
                fileList.append(strPath);
            }

        }
    }
    return fileList;
}

void OpenInFinder(const QString& strPath)
{
#if defined(Q_OS_WIN)
    QStringList arguments;
    arguments << "/select," << QDir::toNativeSeparators(strPath);
    QProcess::startDetached("explorer", arguments);
#elif defined(Q_OS_MACOS)
    QStringList arguments;
    arguments << "-e" << QString("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(strPath);
    QProcess::startDetached("osascript", arguments);

    arguments.clear();
    arguments << "-e" << "tell application \"Finder\" to activate";
    QProcess::startDetached("osascript", arguments);
#else
// 其他平台代码
#endif

}

void MainWindow::startConvent()
{
    ui->labelPrompt->setText("正在转换中...，请稍后");
    ui->labelPrompt->update();
    ui->btnConvert->setEnabled(false);

    bool bBat = ui->radioBat->isChecked();
    if(!bBat)
    {
      bool bRes =  G_ConverterTool.Convert(ui->EditInput->text(), ui->EditOutput->text(), ui->cmbOutput->currentText());
      bRes ?  ui->labelPrompt->setText("转换完成！") :  ui->labelPrompt->setText("转换失败！");
    }
    else
    {
        QString strInputFloder = ui->EditInput->text();
        QString strOutputFloder = ui->EditOutput->text();

        QStringList fileList = getFileListUnderDir(strInputFloder, ui->cmbInput->currentText());

        int iProgress = 0;
        int iSuccess = 0;
        for(QString &path : fileList)
        {
            QString outputPath = path;
            outputPath.replace(strInputFloder, strOutputFloder);
            ReplaceFormat(outputPath, ui->cmbOutput->currentText());
            bool bRes = G_ConverterTool.Convert(path,outputPath, ui->cmbOutput->currentText());
            if(bRes) ++iSuccess;
            ++iProgress;
            ui->labelPrompt->setText("正在转换，已完成" + QString::number(iProgress) + "/"
                                     + QString::number(fileList.size()) + "，请稍后...");

        }
        ui->labelPrompt->setText("转换完成！成功转换" + QString::number(iSuccess) + "/"
            + QString::number(fileList.size()) + "!");
    }

    ui->btnConvert->setEnabled(true);
    OpenInFinder(ui->EditOutput->text());
}

void MainWindow::on_btnConvert_clicked()
{
    QString strInputPath = ui->EditInput->text();
    QString strOutputPath = ui->EditOutput->text();
    if(strInputPath.isEmpty() || strOutputPath.isEmpty())
    {
        QMessageBox::warning(this,"警告", "输入或输出路径为空，请选择路径！");
        return;
    }

    //office
    if(std::find(G_ConverterTool.m_wordFormats.begin()
                  ,G_ConverterTool.m_wordFormats.end(),ui->cmbInput->currentText())
        != G_ConverterTool.m_wordFormats.end())
    {
        QSettings setting(qApp->applicationDirPath()+"/userdata.ini",QSettings::IniFormat);
        setting.beginGroup(OPTION);
        QString program = setting.value(LIBREOFFICE_PATH,DEFAULT_LIBREOFFICE_PATH).toByteArray();
#if defined(Q_OS_WIN)
        program += "/soffice.exe";
#elif defined(Q_OS_MACOS)
        program += "/soffice";
#else
// 其他平台代码
#endif
        setting.endGroup();

        //检查soffice.exe是否存在
        QFile file(program);
        if(!file.exists())
        {
            on_actionoption_triggered();
            return;
        }
    }

    if(std::find(G_ConverterTool.m_rawFormats.begin()
                  ,G_ConverterTool.m_rawFormats.end(),ui->cmbInput->currentText())
        != G_ConverterTool.m_rawFormats.end())
    {
        QFileInfo fileInfo(PER_FILE_PATH);
        QDateTime fileTime = fileInfo.birthTime();
        int nDays = fileTime.daysTo(QDateTime::currentDateTime());
        if(nDays > 7 && !m_bIsVIP)
        {
            QMessageBox::information(this,NULL,"试用期7天已过，raw格式转换需激活授权，详情见“授权管理”窗口！");
            on_actionPermission_triggered();
            return;
        }
    }

    std::thread th(&MainWindow::startConvent,this);
    th.detach();
}


void MainWindow::on_BtnInput_clicked()
{
    bool bChecked = ui->radioBat->isChecked();
    if(!bChecked)
    {
        QString strFilter = "文件(*" + ui->cmbInput->currentText() +")";
        QString fileName = QFileDialog::getOpenFileName(this,"选择导入文件", NULL, strFilter);
        if(fileName.isEmpty()) return;
        ui->EditInput->setText(fileName);
        //fileName.replace(ui->cmbInput->currentText(),ui->cmbOutput->currentText());
        ReplaceFormat(fileName, ui->cmbOutput->currentText());
        ui->EditOutput->setText(fileName);

    }
    else
    {
        QString floderName = QFileDialog::getExistingDirectory(this,"选择导入文件夹");
        if(floderName.isEmpty()) return;
        ui->EditInput->setText(floderName);
        ui->EditOutput->setText(floderName+"_导出");
    }
}


void MainWindow::on_BtnOutput_clicked()
{
    bool bChecked = ui->radioBat->isChecked();
    if(!bChecked)
    {
        QString strFilter = "文件(*" + ui->cmbOutput->currentText() +")";
        QString fileName = QFileDialog::getSaveFileName(this,"选择导出文件", NULL, strFilter);
        if(fileName.isEmpty()) return;
        ui->EditOutput->setText(fileName);
    }
    else
    {
        QString floderName = QFileDialog::getExistingDirectory(this,"选择导出文件夹");
        ui->EditInput->setText(floderName);
        ui->EditOutput->setText(floderName);
    }
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    ui->EditInput->setText("");
    ui->EditOutput->setText("");
}


void MainWindow::on_cmbInput_currentIndexChanged(int index)
{
    ui->EditInput->setText("");
    ui->EditOutput->setText("");

#if defined(Q_OS_WIN)
    bool bIsWin = true;
#elif defined(Q_OS_MACOS)
    bool bIsWin = false;
#else
// 其他平台代码
#endif

    if(std::find(G_ConverterTool.m_wordFormats.begin()
                  ,G_ConverterTool.m_wordFormats.end(),ui->cmbInput->currentText())
        != G_ConverterTool.m_wordFormats.end() && bIsWin)
    {
        ui->cmbOutput->setCurrentText("pdf");
        ui->cmbOutput->setEnabled(false);
    }
    else
    {
        ui->cmbOutput->setEnabled(true);
    }
}


void MainWindow::on_cmbOutput_currentIndexChanged(int index)
{
    if(!ui->EditInput->text().isEmpty() && ui->radioSingle->isChecked())
    {
        QString strText = ui->EditOutput->text();
        ReplaceFormat(strText, ui->cmbOutput->currentText());
        ui->EditOutput->setText(strText);
    }
}


void MainWindow::on_actionhelp_triggered()
{
    QDesktopServices::openUrl(QUrl("https://www.progdomain.com/FinnConverter"));
}


void MainWindow::on_actionoption_triggered()
{
    optionDialog dlg(this);
    dlg.exec();
}


void MainWindow::on_radioSingle_clicked()
{
    ui->EditInput->setText("");
    ui->EditOutput->setText("");
}


void MainWindow::on_radioBat_clicked()
{
    ui->EditInput->setText("");
    ui->EditOutput->setText("");
}


void MainWindow::on_actionPermission_triggered()
{
    PermissionDialog dlg(this);
    dlg.exec();
}

