#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfoList>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QFile>
#include <QInputDialog> // 추가

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void StyleSheetSetting();
    void ConnectFunc();
    void onPushButtonOpenDirectory();
    void ExtensionFiles();
    void TableWidgetDirectoryFiles();
    void UpdateProgressBar(int);
    void updateDirectoryContents();
    void toggleLogAnalysisTab(int);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
