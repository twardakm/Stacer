#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopServices>
#include <QtConcurrent>

#include <Managers/info_manager.h>
#include "circlebar.h"
#include "linebar.h"
#include "network_limiter.h"
#include "network_interface.h"

namespace Ui {
    class DashboardPage;
}

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = 0);
    ~DashboardPage();

private slots:
    void init();
    void checkUpdate();
    void systemInformationInit();

    void updateCpuBar();
    void updateMemoryBar();
    void updateDiskBar();
    void updateNetworkBar();

    void on_downloadUpdateBtn_clicked();

    void on_setBandwidthLimitsBtn_clicked();

    void on_backButtton_clicked();

    void on_stackedWidget_currentChanged(int arg1);

private:
    Ui::DashboardPage *ui;

private:
    CircleBar* cpuBar;
    CircleBar* memBar;
    CircleBar* diskBar;

    LineBar *downloadBar;
    LineBar *uploadBar;

    QTimer *timer;
    InfoManager *im;
};

#endif // DASHBOARDPAGE_H
