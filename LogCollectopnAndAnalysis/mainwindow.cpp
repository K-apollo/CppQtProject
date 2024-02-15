#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    StyleSheetSetting();
    ConnectFunc();
    ExtensionFiles();
    TableWidgetDirectoryFiles();
    UpdateProgressBar(0);
    toggleLogAnalysisTab(ui->checkBox_logAnalysis->checkState());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StyleSheetSetting()
{
    ui->lineEdit_path->setPlaceholderText("Please enter the folder address value");

    for (int i = 0; i < ui->tableWidget_directoryFilesView->rowCount(); ++i)
        for (int j = 0; j < ui->tableWidget_directoryFilesView->columnCount(); ++j)
        {
            QTableWidgetItem *item = ui->tableWidget_directoryFilesView->item(i, j);
            if (item) item->setTextAlignment(Qt::AlignCenter);
        }
}

void MainWindow::ConnectFunc()
{
    connect(ui->pushButton_path, &QPushButton::clicked, this, &MainWindow::onPushButtonOpenDirectory);
    connect(ui->combox_extensionFiles, &QComboBox::currentTextChanged, this, &MainWindow::updateDirectoryContents);
    connect(ui->checkBox_logAnalysis, &QCheckBox::stateChanged, this, &MainWindow::toggleLogAnalysisTab);
}

void MainWindow::onPushButtonOpenDirectory()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Folder", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folderPath.isEmpty())
    {
        ui->lineEdit_path->setText(folderPath);
        updateDirectoryContents();
    }
}

void MainWindow::ExtensionFiles()
{
    QStringList extensions = {"None", ".log", ".txt", ".ini", "cfg || .config", ".csv", ".bat", ".psl", ".vbs", ".js", ".html || .htm", ".xml", ".css", ".php", ".jsp", ".asp", ".json", ".md"};
    ui->combox_extensionFiles->addItems(extensions);
}

void MainWindow::TableWidgetDirectoryFiles()
{
    ui->tableWidget_directoryFilesView->setColumnCount(5);
    QStringList headers = {"No", "Folder Address", "File", "Creation Date", "Creation Time"};
    ui->tableWidget_directoryFilesView->setHorizontalHeaderLabels(headers);
    for (int i = 0; i < 4; ++i)
        ui->tableWidget_directoryFilesView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    ui->tableWidget_directoryFilesView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableWidget_directoryFilesView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_directoryFilesView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::UpdateProgressBar(int value)
{
    ui->progressBar_loading->setValue(value);
}

void MainWindow::updateDirectoryContents()
{
    QString folderPath = ui->lineEdit_path->text();
    QString selectedExtension = ui->combox_extensionFiles->currentText();
    QDir dir(folderPath);
    QStringList filters;
    if (selectedExtension != "None") filters << "*" + selectedExtension;
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::NoSort);
    ui->tableWidget_directoryFilesView->setRowCount(files.size());
    for (int i = 0; i < files.count(); ++i)
    {
        QFileInfo fileInfo = files.at(i);
        QStringList rowData = {
                               QString::number(i + 1), fileInfo.absolutePath(), fileInfo.fileName(), fileInfo.lastModified().toString("yyyy-MM-dd"), fileInfo.lastModified().toString("HH:mm:ss")
        };
        for (int j = 0; j < rowData.size(); ++j)
            ui->tableWidget_directoryFilesView->setItem(i, j, new QTableWidgetItem(rowData.at(j)));
    }
    UpdateProgressBar(files.isEmpty() ? 0 : 100);
}

void MainWindow::toggleLogAnalysisTab(int state)
{
    bool isEnabled = (state == Qt::Checked);
    ui->tabWidget->setTabEnabled(1, isEnabled);
}
