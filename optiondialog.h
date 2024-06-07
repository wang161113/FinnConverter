#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

namespace Ui {
class optionwindow;
}

class optionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit optionDialog(QWidget *parent = nullptr);
    ~optionDialog();
    void closeEvent(QCloseEvent *event);

private slots:
    void on_BtnSelFile_clicked();

    void on_btnDownload_clicked();

    void on_btnBaidu_clicked();

    void on_optionwindow_finished(int result);

private:
    Ui::optionwindow *ui;
};

#endif // OPTIONDIALOG_H
