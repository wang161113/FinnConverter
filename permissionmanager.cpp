#include "permissionmanager.h"
#include <QNetworkInterface>
#include <QCryptographicHash>
#include <QRandomGenerator>

PermissionManager::PermissionManager() {}

QString getMacAddress()
{
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if(interface.flags().testFlag(QNetworkInterface::IsUp) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            foreach(QNetworkAddressEntry entry, interface.addressEntries())
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                {
                    return interface.hardwareAddress();
                }
            }
        }
    }
    return QString();
}

QString generateSalt(const QString &suijizifu = "FinnConverterVip666888999")
{
    const int randomStringLength = 8; // 例如，盐的长度为8

    QString randomString;
    for(int i = 0; i < randomStringLength; ++i)
    {
        int index = QRandomGenerator::global()->bounded(suijizifu.length());
        QChar nextChar = suijizifu.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

QString PermissionManager::generateRegCode()
{
    QString strMac = getMacAddress();
    QCryptographicHash crypto(QCryptographicHash::Sha256);
    QByteArray data;
    data.append(strMac.toUtf8());
    data.append(generateSalt().toUtf8());
    crypto.addData(data);
    return crypto.result().toHex();
}

QString PermissionManager::generateActivationCode(const QString &regCode)
{
    QString strTmp = regCode + "FinnSoftFinnConvertervip666";
    QCryptographicHash crypto(QCryptographicHash::Sha256);
    QByteArray data;
    data.append(strTmp.toUtf8());
    crypto.addData(data);
    return crypto.result().toHex(); // Convert the hash to a hex string
}

bool PermissionManager::isValid(const QString &regCode, const QString &activaCode)
{
    if(generateActivationCode(regCode) != activaCode)
        return false;

    return true;
}

