#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtMath>
#include <QRegularExpression>
#include <QtWidgets>
#include <QTextStream>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QTime>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 버튼 클릭 시그널을 handleButtonPress 슬롯에 연결하는 람다 함수
    auto connectButton = [this](QPushButton* button)
    {
        connect(button, &QPushButton::clicked, this, &MainWindow::handleButtonPress);
    };

    // 계산기 버튼 목록
    QList<QPushButton*> buttons = {
                                    ui->pushButton00, ui->pushButton01, ui->pushButton02, ui->pushButton03, ui->pushButton04,
                                    ui->pushButton05, ui->pushButton06, ui->pushButton07, ui->pushButton08, ui->pushButton09,
                                    ui->pushButton_point, ui->pushButton_plus, ui->pushButton_minus, ui->pushButton_multiple,
                                    ui->pushButton_division, ui->pushButton_clear, ui->pushButton_left, ui->pushButton_right,
                                    ui->pushButton_backspace
    };

    // 각 버튼을 connectButton 람다 함수를 사용하여 클릭 이벤트에 연결
    for (auto& button : buttons)
        connectButton(button);

    connect(ui->pushButton_equal, &QPushButton::clicked, this, &MainWindow::calculateResult);   // '=' 버튼 클릭 시 결과 계산 함수 호출
    ui->lineEdit_input->setPlaceholderText("Enter the formula.");                               // 입력 필드의 플레이스홀더 텍스트 설정
    ui->lineEdit_output->setPlaceholderText("Result will be displayed.");

    // 연속으로 지울 경우 대략 0.1초 정도 지연이 될 경우 연속적으로 제거
    eraseTimer = new QTimer(this);                                                              // 타이머 초기화
    eraseTimer->setInterval(100);                                                               // 100ms 간격으로 설정
    connect(eraseTimer, &QTimer::timeout, this, &MainWindow::handleBackspacePress);             // 타이머 신호를 handleBackspacePress 함수에 연결
    connect(ui->pushButton_backspace, &QPushButton::pressed, eraseTimer, static_cast<void (QTimer::*)()>(&QTimer::start));  // 백스페이스 버튼의 pressed와 released 신호를 타이머의 시작과 정지에 연결
    connect(ui->pushButton_backspace, &QPushButton::released, eraseTimer, &QTimer::stop);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleButtonPress() // 사용자가 계산기 버튼을 클릭할 때마다 적절한 동작을 실행
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;

    if (button == ui->pushButton_clear) {
        clearInputOutput();
        return;
    }

    if (button == ui->pushButton_backspace) {
        handleBackspacePress();
        return;
    }

    if (button == ui->pushButton_equal) {
        calculateResult();
        return;
    }

    QString currentText = ui->lineEdit_input->text();
    QString newText = button->text(); // 기본적으로 버튼의 텍스트 사용

    // 소수점 처리
    if (button == ui->pushButton_point && currentText.contains('.')) {
        return; // 이미 소수점이 있으면 추가하지 않음
    }

    // 괄호 처리
    if (button == ui->pushButton_left || button == ui->pushButton_right) {
        int openParentheses = currentText.count('(');
        int closeParentheses = currentText.count(')');
        if (button == ui->pushButton_right && openParentheses <= closeParentheses) {
            return; // 닫는 괄호가 더 필요하지 않으면 추가하지 않음
        }
    }

    // 사칙연산 처리
    if (button == ui->pushButton_plus || button == ui->pushButton_minus ||
        button == ui->pushButton_multiple || button == ui->pushButton_division) {
        if (!currentText.isEmpty()) {
            QChar lastChar = currentText[currentText.length() - 1];
            if (lastChar.isDigit() || lastChar == ')') {
                currentText += newText; // 연산자 추가
            }
        }
    } else {
        currentText += newText; // 기타 문자 추가
    }

    ui->lineEdit_input->setText(currentText);
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

void MainWindow::handleParenthesisPress()   // 괄호 버튼 클릭 시 괄호를 입력 필드에 추가하는 함수
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());        // 클릭된 버튼 객체를 확인
    if (button)                                                         // 유효한 버튼 객체인 경우
    {
        QString newText = button == ui->pushButton_left ? "(" : ")";    // 왼쪽 괄호 버튼이면 "(" 추가, 아니면 ")"
        QString currentText = ui->lineEdit_input->text();               // 현재 입력 필드의 텍스트 가져오기
        ui->lineEdit_input->setText(currentText + newText);             // 괄호를 현재 텍스트에 추가
    }
}

double MainWindow::evaluateExpression(const QString &expression)
{
    QList<double> numbers;              // 숫자 저장
    QList<QChar> operators;             // 연산자 저장

    QString streamInput(expression);    // 입력된 식을 스트림으로 변환
    QTextStream iss(&streamInput);

    while (!iss.atEnd()) // 스트림 끝까지 반복
    {
        QChar next;
        iss >> next;                        // 다음 문자 읽기
        if (next.isDigit() || next == '.')  // 숫자나 소수점인 경우
        {
            iss.seek(iss.pos() - 1);        // 스트림 위치 조정
            double value;
            iss >> value;                   // 숫자 읽기
            numbers.append(value);          // 숫자 리스트에 추가
        }
        else                                // 연산자인 경우
        {
            if (next == '(')                // 여는 괄호
                operators.append(next);
            else if (next == ')')           // 닫는 괄호
            {
                while (!operators.isEmpty() && operators.last() != '(')     // 여는 괄호를 만날 때까지 연산 실행
                    executeTopOperation(numbers, operators);
                operators.takeLast();                   // 여는 괄호 제거
            }
            else if (isOperator(next))                  // 사칙연산자인 경우
            {
                while (!operators.isEmpty() && precedence(operators.last()) >= precedence(next))    // 연산 우선순위를 고려하여 연산 실행
                    executeTopOperation(numbers, operators);
                operators.append(next);                                     // 연산자 추가
            }
        }
    }

    while (!operators.isEmpty())                    // 남은 연산 처리
        executeTopOperation(numbers, operators);

    return numbers.isEmpty() ? 0 : numbers.last(); // 계산 결과 반환
}

void MainWindow::executeTopOperation(QList<double> &numbers, QList<QChar> &operators)       // 스택에서 숫자 두 개와 연산자를 꺼내 해당 연산을 수행하고 결과를 다시 스택에 추가
{
    if (numbers.size() < 2) return;

    double right = numbers.takeLast();  // 오른쪽 피연산자
    double left = numbers.takeLast();   // 왼쪽 피연산자
    QChar op = operators.takeLast();    // 연산자

    double result = 0;                  // 연산자에 따라 적절한 연산 수행
    switch (op.toLatin1())
    {
    case '+': result = left + right; break; // 덧셈
    case '-': result = left - right; break; // 뺄셈
    case '*': result = left * right; break; // 곱셈
    case '/': result = left / right; break; // 나눗셈
    }

    numbers.append(result);
}

bool MainWindow::isOperator(QChar c)                    // 주어진 문자가 사칙연산자인지 확인
{
    return QString("+-*/").contains(c);
}

int MainWindow::precedence(QChar op)                    // 연산자의 우선순위를 반환
{
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

void MainWindow::handleBackspacePress()                 // 백스페이스의 기능
{
    QString currentText = ui->lineEdit_input->text();
    if (!currentText.isEmpty())
    {
        currentText.chop(1);
        ui->lineEdit_input->setText(currentText);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)                // 키보드 입력에 따라 계산기 버튼 클릭 애니메이션 실행
{
    QPushButton *button = nullptr;

    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)     // 숫자 키 처리
    {
        int num = event->key() - Qt::Key_0;                         // 키 코드를 숫자로 변환
        QString buttonName = QString("pushButton0%1").arg(num);     // 버튼 객체 이름 생성
        button = findChild<QPushButton*>(buttonName);               // 버튼 객체 찾기
    }
    else
    {
        // 사칙 연산 및 백스페이스 키 처리
        QMap<int, QPushButton*> keyToButton = {
            {Qt::Key_Plus, ui->pushButton_plus},           	// 더하기
            {Qt::Key_Minus, ui->pushButton_minus},         	// 빼기
            {Qt::Key_Asterisk, ui->pushButton_multiple},   	// 곱하기
            {Qt::Key_Slash, ui->pushButton_division},      	// 나누기
            {Qt::Key_Backspace, ui->pushButton_backspace}	// 백스페이스 기능
        };
        button = keyToButton.value(event->key(), nullptr);  // 해당 키에 맞는 버튼 찾기
    }

    if (event->key() == Qt::Key_Escape) this->close();                                                      // ESC 키로 창 닫기
    else if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier)) clearInputOutput();   // Ctrl+C로 내용 지우기
    else if (button) button->animateClick();                                                                // 버튼 클릭 애니메이션 실행
}
