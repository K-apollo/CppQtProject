#include "form_showandhide.h"
#include "ui_form_showandhide.h"

Form_showAndHide::Form_showAndHide(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form_showAndHide)
{
    ui->setupUi(this);
}

Form_showAndHide::~Form_showAndHide()
{
    delete ui;
}
