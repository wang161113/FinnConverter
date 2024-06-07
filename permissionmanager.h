#ifndef PERMISSIONMANAGER_H
#define PERMISSIONMANAGER_H
#include <QString>
class PermissionManager
{
public:
    PermissionManager();

    static bool isValid(const QString& regCode, const QString& activaCode);
    static QString generateRegCode();
    static QString generateActivationCode(const QString &regCode);

private:

};

#endif // PERMISSIONMANAGER_H
