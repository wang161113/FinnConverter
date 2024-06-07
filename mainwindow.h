#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class optionDialog;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnClose_clicked();

    void on_btnConvert_clicked();

    void on_BtnInput_clicked();

    void on_BtnOutput_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_cmbInput_currentIndexChanged(int index);

    void on_cmbOutput_currentIndexChanged(int index);

    void on_actionhelp_triggered();

    void startConvent();
    void on_actionoption_triggered();

    void on_radioSingle_clicked();

    void on_radioBat_clicked();

    void on_actionPermission_triggered();

private:
    Ui::MainWindow *ui;
    QSettings m_settings;
    bool m_bIsVIP = false;
};
#endif // MAINWINDOW_H
