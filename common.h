#ifndef COMMON_H
#define COMMON_H

#include <QStandardPaths>

#if defined(Q_OS_WIN)
    #define DEFAULT_LIBREOFFICE_PATH "C:/Program Files/LibreOffice/program"
#elif defined(Q_OS_MACOS)
#define DEFAULT_LIBREOFFICE_PATH  ("/Applications/LibreOffice.app/Contents/MacOS")
#elif defined(Q_OS_LINUX)
// Linux 平台代码
#else
// 其他平台代码
#endif


#define OPTION "选项"
#define LIBREOFFICE_PATH "Libreoffice"


#define PER_FILE_PATH (QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).first() + "/.data")



#endif // COMMON_H
