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
    explicit NetworkInterface(QWidget *parent = 0);
    ~NetworkInterface();

private:
    Ui::NetworkInterface *ui;

    void setNetworkLimiterPtr(std::shared_ptr<NetworkLimiter> &nL){networkLimiter = nL;}
    std::shared_ptr<NetworkLimiter> networkLimiter;
};

#endif // NETWORK_INTERFACE_H
