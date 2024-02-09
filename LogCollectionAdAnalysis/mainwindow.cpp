#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    StyleSheetSetting();
    ConnectFunc();
    ExtensionFiles();
    TableWidgetDirectoryFiles();
    UpdateProgressBar(0);                       // 진행률 막대 = 0% 초기화
    onLogAnalysis();
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
    connect(ui->checkBox_logAnalysis, &QCheckBox::stateChanged, this, &MainWindow::onLogAnalysis);
}

void MainWindow::onPushButtonOpenDirectory()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
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

    for (int row = 0; row < ui->tableWidget_directoryFilesView->rowCount(); ++row)
    {
        for (int col = 1; col <= 2; ++col)
        {
            QTableWidgetItem *item = ui->tableWidget_directoryFilesView->item(row, col);
            if (item)
            {
                QString text = item->text();
                QFontMetrics metrics(item->font());
                int width = ui->tableWidget_directoryFilesView->columnWidth(col);
                QString elidedText = metrics.elidedText(text, Qt::ElideMiddle, width);
                item->setText(elidedText);
            }
        }
    }
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

    int fileCount = files.count();
    for (int i = 0; i < fileCount; ++i)
    {
        QFileInfo fileInfo = files.at(i);

        QStringList rowData;
        rowData << QString::number(i + 1) << fileInfo.absolutePath() << fileInfo.fileName() << fileInfo.lastModified().toString("yyyy-MM-dd") << fileInfo.lastModified().toString("HH:mm:ss");

        for (int j = 0; j < 5; ++j)
            ui->tableWidget_directoryFilesView->setItem(i, j, new QTableWidgetItem(rowData.at(j)));
    }

    int progress = fileCount > 0 ? 100 : 0;
    UpdateProgressBar(progress);
}

void MainWindow::onLogAnalysis()
{
    bool isChecked = ui->checkBox_logAnalysis->isChecked();
    ui->tab02_LogAnalysis->setEnabled(isChecked);

    if (isChecked)
    {
        ui->comboBox_currentFile->clear();
        ui->comboBox_currentFile->addItem("None");

        QString selectedFile = ui->comboBox_currentFile->currentText();
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
        if (!filePath.isEmpty())
        {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextCodec *codec = QTextCodec::codecForLocale();
                QStringList codecs;
                codecs << "UTF-8" << "Ansi" << "UTF-16 BE" << "UTF-16 LE" << "UTF-8(BOM)";
                QString selectedCodec = QInputDialog::getItem(this, tr("Select Encoding"), tr("Encoding:"), codecs, 0, false);
                if (!selectedCodec.isEmpty())
                {
                    codec = QTextCodec::codecForName(selectedCodec.toUtf8());
                    QTextStream in(&file);
                    in.setCodec(codec);
                    ui->listWidget_fileContentOutput->clear();
                    QFont font("Arial", 10);
                    ui->listWidget_fileContentOutput->setFont(font);
                    while (!in.atEnd())
                    {
                        QString line = in.readLine();
                        QListWidgetItem *item = new QListWidgetItem(line);
                        item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                        ui->listWidget_fileContentOutput->addItem(item);
                    }
                }
                file.close();
            }
        }
    }
}
