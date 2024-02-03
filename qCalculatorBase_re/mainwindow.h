#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QPushButton>
#include <QValidator>
#include <QStandardItemModel>       // treeview 추가

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleButtonPress(); 													// 버튼 누름 처리
    void calculateResult(); 													// 결과 계산
    void clearInputOutput(); 													// 입력과 출력 클리어
    void handleParenthesisPress(); 												// 괄호 버튼 처리
    void handleBackspacePress(); 												// 백스페이스 처리

private:
    Ui::MainWindow *ui;
    double evaluateExpression(const QString &expression); 						// 식 평가
    void executeTopOperation(QList<double> &numbers, QList<QChar> &operators);  // 연산 실행
    bool isOperator(QChar c); 													// 연산자 확인
    int precedence(QChar op); 													// 연산자 우선순위
    void keyPressEvent(QKeyEvent *event); 										// 키 이벤트 처리
};
#endif // MAINWINDOW_H