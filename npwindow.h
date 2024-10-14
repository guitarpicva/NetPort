#ifndef NPWINDOW_H
#define NPWINDOW_H

#include "netport.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>

namespace Ui {
class NPWindow;
}

class NPWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NPWindow(QWidget *parent = nullptr);
    ~NPWindow();
    void closeEvent(QCloseEvent *event);

private slots:
    void on_action_New_NetPort_triggered();
    void addNPToSettings(NetPort *np, const QString name);
    void loadSettings();
    void on_action_NetPort_triggered();

private:
    Ui::NPWindow *ui;
    QSettings *settings;
    int numWidgets = 1; // at least one
    void createNetPort(const QString name, bool newport = false);
    void saveSettings();
    void deleteWidgets();
};

#endif // NPWINDOW_H
