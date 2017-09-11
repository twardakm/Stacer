#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include <QWidget>
#include <memory>

#include "network_limiter.h"

namespace Ui {
class NetworkInterface;
}

class NetworkInterface : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkInterface(QWidget *parent = 0, QString interface = "eth0");
    ~NetworkInterface();
    void updateNetworkLimits();

private slots:
    void on_downloadLimitCustom_editingFinished();

    void on_uploadLimitCustom_editingFinished();

private:
    Ui::NetworkInterface *ui;

    void setNetworkLimiterPtr(std::shared_ptr<NetworkLimiter> &nL){networkLimiter = nL;}
    std::shared_ptr<NetworkLimiter> networkLimiter;
    QString interface;
};

#endif // NETWORK_INTERFACE_H
