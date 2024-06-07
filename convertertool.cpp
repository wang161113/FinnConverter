#include "convertertool.h"
#include <QImageReader>
#include <QImageWriter>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QPdfWriter>
#include <QPainter>
#include <QProcess>
#include <QSettings>
#include <QMessageBox>
#include "common.h"

#define MAKE_DIR    QFileInfo fileInfo(outputPath);\
                    QDir dir = fileInfo.dir();\
                    if(!dir.exists())\
                    {\
                        dir.mkpath(dir.path());\
                    }


ConverterTool::ConverterTool()
{
    // 获取支持的读取格式
    m_readFormats = QImageReader::supportedImageFormats();
    m_readFormats.append(m_wordFormats);
    m_readFormats.append(m_rawFormats);

    std::sort(m_readFormats.begin(), m_readFormats.end());

    // 获取支持的写入格式
    m_writeFormats = QImageWriter::supportedImageFormats();
    m_writeFormats.append("pdf");
    std::sort(m_writeFormats.begin(), m_writeFormats.end());

}

ConverterTool& ConverterTool::instance()
{
    static ConverterTool inst;
    return inst;
}

bool ConverterTool::convertFileWithLibreOffice(const QString& inputPath, const QString& outputPath, const QString& outputFormat)
{
    MAKE_DIR


    // LibreOffice的可执行文件路径
    QSettings setting(qApp->applicationDirPath()+"/userdata.ini",QSettings::IniFormat);
    setting.beginGroup(OPTION);
    QString program = setting.value(LIBREOFFICE_PATH,DEFAULT_LIBREOFFICE_PATH).toByteArray();
#if defined(Q_OS_WIN)
    program += "/soffice.exe";
#elif defined(Q_OS_MACOS)
    program += "/soffice";
#endif
    setting.endGroup();

    //检查soffice.exe是否存在
    QFile fileExe(program);
    if(!fileExe.exists())
    {
        QMessageBox::information(NULL,NULL,"请点击菜单\"设置->选项\"，进行设置！");
        fileExe.close();
        return false;
    }
    fileExe.close();

    QString strPdf = outputPath;
    strPdf = strPdf.left(strPdf.lastIndexOf(".") + 1);
    strPdf += "pdf";
    QFile fileOut(strPdf);
    bool bExists = fileOut.exists();

    qDebug()<<program;
    // 构建命令行参数
    QStringList arguments;
    arguments << "--headless"                 // 不启动GUI
              << "--convert-to" << "pdf" // 指定输出格式
              << "--outdir" << dir.path()+"/" // 指定输出目录
              << inputPath;                  // 输入文件路径

    // 启动LibreOffice进程
    QProcess libreOfficeProcess;
    libreOfficeProcess.start(program, arguments);
    if (!libreOfficeProcess.waitForFinished(-1))
    { // 等待进程结束
        qDebug() << "Conversion failed:" << libreOfficeProcess.errorString();
        return false;
    }

    if(!fileOut.exists())
    {
        fileOut.close();
        qDebug() << "Conversion failed: " + strPdf;
        return false;
    }
    fileOut.close();

#if defined(Q_OS_WIN)
    //暂时只支持转pdf
    qDebug() << "Conversion successful, file saved to:" << strPdf;
    return true;
#elif defined(Q_OS_MACOS)
    if(outputFormat == "pdf") return true;

    bool bRes = convertFileWithQimage(strPdf, outputPath, outputFormat);
    if(!bExists)
        QFile::remove(strPdf);
    return bRes;
    qDebug() << "Conversion successful, file saved to:" << outputPath;
#else
// 其他平台代码
#endif


    return true;
}

bool ConvertImg2Pdf(const QString& inputPath, const QString& outputPath)
{
    // 载入图片
    QImage image(inputPath); // 替换为你的图片路径
    if (image.isNull()) {
        qWarning("无法载入图片！");
        return false;
    }

    // 创建PDF写入器
    QPdfWriter writer(outputPath); // 输出的PDF文件名
    writer.setPageSize(QPageSize(QPageSize::A4)); // 设置页面大小为A4
    writer.setPageMargins(QMargins(0, 0, 0, 0)); // 设置页面边距

    // 使用QPainter在PDF中绘制图片
    QPainter painter(&writer);
    QRect rect = painter.viewport();
    QSize size = image.size();
    size.scale(rect.size(), Qt::KeepAspectRatio); // 保持图片的宽高比
    painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
    painter.setWindow(image.rect());
    painter.drawImage(0, 0, image);

    return true;
}

bool ConverterTool::convertFileWithLibRaw(const QString &inputPath, const QString &outputPath, const QString &outputFormat)
{
    MAKE_DIR
    // 尝试打开RAW文件

#if defined(Q_OS_WIN)
    wchar_t szInputPath[MAX_PATH];
    memset(szInputPath,0,MAX_PATH*sizeof(wchar_t));
    inputPath.toWCharArray(szInputPath);
    int nRes = m_raw.open_file(szInputPath);
#elif defined(Q_OS_MACOS)
    int nRes = m_raw.open_file(inputPath.toStdString().c_str());
#elif defined(Q_OS_LINUX)
    // Linux 平台代码
#else
    // 其他平台代码
#endif

    if (nRes != LIBRAW_SUCCESS) {
        qDebug() << "Cannot open file" << inputPath;
        return false;
    }

    // 解压RAW数据
    if (m_raw.unpack() != LIBRAW_SUCCESS) {
        qDebug() << "Cannot unpack" << inputPath;
        return false;
    }

    // 处理图像
    if (m_raw.dcraw_process() != LIBRAW_SUCCESS) {
        qDebug() << "Processing failed";
        return false;
    }

    // 生成内存图像
    libraw_processed_image_t *processedImage = m_raw.dcraw_make_mem_image();

    if (!processedImage) {
        qDebug() << "Failed to create memory image";
        return false;
    }

    // 创建QImage
    QImage image(processedImage->data, processedImage->width, processedImage->height, QImage::Format_RGB888);

    // 检查QImage是否有效
    if (image.isNull()) {
        qDebug() << "Failed to convert to QImage";
        LibRaw::dcraw_clear_mem(processedImage);
        return false;
    }

    if(outputFormat != "pdf")
    {
        // 保存为outputFormat
        if (!image.save(outputPath, outputFormat.toStdString().c_str())) {
            qDebug() << "Failed to save image";
        } else {
            qDebug() << "Image saved successfully";
        }
    }
    else
    {
        // 保存为png
        QString strPath = outputPath;
        strPath = strPath.left(strPath.lastIndexOf(".") + 1);
        strPath += "png";
        if (!image.save(strPath, "png")) {
            qDebug() << "Failed to save image";
        } else {
            qDebug() << "Image saved successfully";
        }
        ConvertImg2Pdf(strPath, outputPath);
        QFile::remove(strPath);
    }


    // 清理
    LibRaw::dcraw_clear_mem(processedImage);
    m_raw.recycle();
    return true;
}

bool ConverterTool::convertFileWithQimage(const QString &inputPath, const QString &outputPath, const QString &outputFormat)
{
    QImage img;
    bool bRes = img.load(inputPath);
    if(!bRes)
    {
        qDebug()<<"load File Failed: " + inputPath;
        return bRes;
    }

    if(outputFormat == "pdf")
    {
        return ConvertImg2Pdf(inputPath,outputPath);
    }

    MAKE_DIR

    bRes = img.save(outputPath,outputFormat.toStdString().c_str());
    return bRes;
}


bool ConverterTool::Convert(const QString& inputPath, const QString& outputPath, const QString& format)
{
    QFile fileInput(inputPath);
    QString strFileName = fileInput.fileName();
    QString strExt = strFileName.right(strFileName.size()
                    - strFileName.lastIndexOf(".") - 1);
    strExt = strExt.toLower();
    //处理raw格式
    if(std::find(m_rawFormats.begin(),m_rawFormats.end(),strExt)
        != m_rawFormats.end())
    {
        return convertFileWithLibRaw(inputPath, outputPath, format);
    }

    //处理word ppt文档
    if(std::find(m_wordFormats.begin(),m_wordFormats.end(),strExt)
        != m_wordFormats.end())
    {
        return convertFileWithLibreOffice(inputPath, outputPath, format);
    }

    return convertFileWithQimage(inputPath, outputPath, format);
}
