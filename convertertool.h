#ifndef CONVERTERTOOL_H
#define CONVERTERTOOL_H
#include <QImage>
#include <libraw.h>

class ConverterTool
{
    ConverterTool();
public:
    static ConverterTool& instance();
    bool Convert(const QString& inputPath, const QString& outputPath, const QString& format);

    QList<QByteArray> m_readFormats;
    QList<QByteArray> m_writeFormats;
    QList<QByteArray> m_rawFormats = {"crw","cr2","cr3","nef","raf","dng","mos","kdc","dcr"};
    QList<QByteArray> m_wordFormats = {"doc","docx","odt","ott","rtf","txt","ppt","pptx","odp"};
protected:
    bool convertFileWithLibreOffice(const QString& inputPath, const QString& outputPath, const QString& outputFormat);
    bool convertFileWithLibRaw(const QString& inputPath, const QString& outputPath, const QString& outputFormat);
    bool convertFileWithQimage(const QString& inputPath, const QString& outputPath, const QString& outputFormat);
private:
    LibRaw m_raw;
};

#define G_ConverterTool (ConverterTool::instance())

#endif // CONVERTERTOOL_H
