#ifndef DOCKWIDGET_SHOWANDHIDE_H
#define DOCKWIDGET_SHOWANDHIDE_H

#include <QDockWidget>

namespace Ui {
class DockWidget_showAndHide;
}

class DockWidget_showAndHide : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockWidget_showAndHide(QWidget *parent = nullptr);
    ~DockWidget_showAndHide();

private:
    Ui::DockWidget_showAndHide *ui;
};

#endif // DOCKWIDGET_SHOWANDHIDE_H
