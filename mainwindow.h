#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMap>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void chooseIconFile();
    void chooseAppFile();
    void kindChanged(int index);
    void onCreate();

private:
    Ui::MainWindow *ui;
    QList<QString> kinds;

    void initUI();
    bool isImageFile(const QString& file);
    bool writeEntryFile(const QMap<QString,QString>& entries,const QString& realName);

};

#endif // MAINWINDOW_H
