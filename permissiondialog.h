#ifndef PERMISSIONDIALOG_H
#define PERMISSIONDIALOG_H

#include <QDialog>

namespace Ui {
class PermissionDialog;
}

class PermissionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PermissionDialog(QWidget *parent = nullptr);
    ~PermissionDialog();

private slots:
    void on_btnCopyReg_clicked();

    void on_btnActivate_clicked();

private:
    Ui::PermissionDialog *ui;
};

#endif // PERMISSIONDIALOG_H
