#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtMath>
#include <QRegularExpression>
#include <QtWidgets>
#include <QTextStream>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton00,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton01,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton02,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton03,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton04,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton05,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton06,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton07,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton08,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton09,         &QPushButton::clicked, this, &MainWindow::handleButtonPress);

    connect(ui->pushButton_point,     &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton_plus,      &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton_minus,     &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton_multiple,  &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton_division,  &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    connect(ui->pushButton_equal,     &QPushButton::clicked, this, &MainWindow::calculateResult);

    connect(ui->pushButton_clear,     &QPushButton::clicked, this, &MainWindow::clearInputOutput);
    connect(ui->pushButton_left,      &QPushButton::clicked, this, &MainWindow::handleParenthesisPress);
    connect(ui->pushButton_right,     &QPushButton::clicked, this, &MainWindow::handleParenthesisPress);
    connect(ui->pushButton_backspace, &QPushButton::clicked, this, &MainWindow::handleBackspacePress);

    ui->lineEdit_input->setPlaceholderText("Enter the formula.");
    QRegularExpression inputRegex("[^0-9()*/+-=.]");
    QValidator *inputValidator = new QRegularExpressionValidator(inputRegex, this);
    ui->lineEdit_input->setValidator(inputValidator);

    ui->lineEdit_output->setPlaceholderText("Result will be displayed.");
    QRegularExpression outputRegex("[^0-9()*/+-=.]");
    QValidator *outputValidator = new QRegularExpressionValidator(outputRegex, this);
    ui->lineEdit_output->setValidator(outputValidator);

    QStringList items = {"Sports", "Math", "Chemistry", "Life", "Tax", "Income", "Time", "Unit"};
    items.sort();
    QStandardItemModel *model = new QStandardItemModel(this);

    foreach(const QString &itemName, items)
    {
        QStandardItem *item = new QStandardItem(itemName);
        model->appendRow(item);
    }
    ui->treeView_socialCalculator->setModel(model);
    ui->treeView_socialCalculator->setSortingEnabled(true);
    ui->treeView_socialCalculator->sortByColumn(0, Qt::DescendingOrder);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleButtonPress()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button)
    {
        QString inputText = ui->lineEdit_input->text();
        QString outputText = ui->lineEdit_output->text();

        auto saveToFile = [&]() {
            QString downloadsPath = QDir::homePath() + "/Downloads";
            QDir().mkpath(downloadsPath);
            QString fileName = downloadsPath + "/" + QDate::currentDate().toString("yyyy-MM-dd_BaseCalculator") + ".txt";
            QFile file(fileName);

            if (file.open(QIODevice::WriteOnly | QIODevice::Append))
            {
                QTextStream out(&file);
                out << QTime::currentTime().toString("[HH:mm:ss]") << "\n";
                out << "Input: " << inputText << "\n";
                out << "Output: " << outputText << "\n";
                out << "◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆" << "\n\n";
                file.close();
            }
        };

        auto clearInputOutput = [&]()
        {
            ui->lineEdit_input->clear();
            ui->lineEdit_output->clear();
        };

        if (!inputText.isEmpty() && !outputText.isEmpty())
        {
            saveToFile();
            clearInputOutput();
        }

        if (button == ui->pushButton_clear ||
            (!inputText.isEmpty() && !outputText.isEmpty() &&
             (button == ui->pushButton_plus || button == ui->pushButton_minus ||
              button == ui->pushButton_point || button->objectName().startsWith("pushButton00") ||
              button->objectName().startsWith("pushButton01") || button->objectName().startsWith("pushButton02") ||
              button->objectName().startsWith("pushButton03") || button->objectName().startsWith("pushButton04") ||
              button->objectName().startsWith("pushButton05") || button->objectName().startsWith("pushButton06") ||
              button->objectName().startsWith("pushButton07") || button->objectName().startsWith("pushButton08") ||
              button->objectName().startsWith("pushButton09"))))
        {
            clearInputOutput();
        }
        else
        {
            QString currentText = ui->lineEdit_input->text();
            ui->lineEdit_input->setText(currentText);
        }

        QString currentText = ui->lineEdit_input->text();
        QChar lastChar = currentText.isEmpty() ? QChar() : currentText.back();

        if (button == ui->pushButton_point)
        {
            int lastIndex = currentText.length() - 1;
            while (lastIndex >= 0 && (currentText[lastIndex].isDigit() || currentText[lastIndex] == '.'))
            {
                if (currentText[lastIndex] == '.')
                    return;
                lastIndex--;
            }
            ui->lineEdit_input->setText(currentText + ".");
        }
        else
        {
            QString newText;
            if (button == ui->pushButton_plus)
                newText = "+";
            else if (button == ui->pushButton_minus)
                newText = "-";
            else if (button == ui->pushButton_multiple)
                newText = "*";
            else if (button == ui->pushButton_division)
                newText = "/";
            else if (button == ui->pushButton_left)
            {
                if (lastChar.isDigit() || lastChar == ')')
                    newText = "*(";
                else
                    newText = "(";
            }
            else if (button->objectName().startsWith("pushButton0"))
                newText = button->objectName().right(1);
            ui->lineEdit_input->setText(currentText + newText);
        }
    }
}

void MainWindow::calculateResult()
{
    QString input = ui->lineEdit_input->text();
    if (input.endsWith('.'))
        input += "0";
    double result = evaluateExpression(input);
    ui->lineEdit_output->setText(QString::number(result));
}

void MainWindow::clearInputOutput()
{
    ui->lineEdit_input->clear();
    ui->lineEdit_output->clear();
}

void MainWindow::handleParenthesisPress()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button)
    {
        QString newText = button == ui->pushButton_left ? "(" : ")";
        QString currentText = ui->lineEdit_input->text();
        ui->lineEdit_input->setText(currentText + newText);
    }
}

double MainWindow::evaluateExpression(const QString &expression)
{
    QList<double> numbers;
    QList<QChar> operators;

    QString streamInput(expression);
    QTextStream iss(&streamInput);

    while (!iss.atEnd())
    {
        QChar next;
        iss >> next;
        if (next.isDigit() || next == '.')
        {
            iss.seek(iss.pos() - 1);
            double value;
            iss >> value;
            numbers.append(value);
        }
        else
        {
            if (next == '(')
                operators.append(next);
            else if (next == ')')
            {
                while (!operators.isEmpty() && operators.last() != '(')
                    executeTopOperation(numbers, operators);
                operators.takeLast();
            }
            else if (isOperator(next))
            {
                while (!operators.isEmpty() && precedence(operators.last()) >= precedence(next))
                    executeTopOperation(numbers, operators);
                operators.append(next);
            }
        }
    }

    while (!operators.isEmpty())
        executeTopOperation(numbers, operators);

    return numbers.isEmpty() ? 0 : numbers.last();
}

void MainWindow::executeTopOperation(QList<double> &numbers, QList<QChar> &operators)
{
    if (numbers.size() < 2) return;

    double right = numbers.takeLast();
    double left = numbers.takeLast();
    QChar op = operators.takeLast();

    double result = 0;
    switch (op.toLatin1())
    {
    case '+':
        result = left + right;
        break;
    case '-':
        result = left - right;
        break;
    case '*':
        result = left * right;
        break;
    case '/':
        result = left / right;
        break;
    }

    numbers.append(result);
}

bool MainWindow::isOperator(QChar c)
{
    return QString("+-*/").contains(c);
}

int MainWindow::precedence(QChar op)
{
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

void MainWindow::handleBackspacePress()
{
    QString currentText = ui->lineEdit_input->text();
    if (!currentText.isEmpty())
    {
        currentText.chop(1);
        ui->lineEdit_input->setText(currentText);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QPushButton *button = nullptr;

    switch (event->key()) {
    case Qt::Key_0:
        button = (event->modifiers() & Qt::KeypadModifier) ? ui->pushButton00 : nullptr;
        break;
    case Qt::Key_1:
        button = (event->modifiers() & Qt::KeypadModifier) ? ui->pushButton01 : nullptr;
        break;
    case Qt::Key_9:
        button = (event->modifiers() & Qt::KeypadModifier) ? ui->pushButton09 : nullptr;
        break;
    case Qt::Key_Plus:
        button = ui->pushButton_plus;
        break;
    case Qt::Key_Minus:
        button = ui->pushButton_minus;
        break;
    case Qt::Key_Asterisk:
        button = ui->pushButton_multiple;
        break;
    case Qt::Key_Slash:
        button = ui->pushButton_division;
        break;
    }

    if (button)
        button->animateClick();
}
