#include "network_interface.h"
#include "ui_network_interface.h"

NetworkInterface::NetworkInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkInterface)
{
    ui->setupUi(this);

    std::shared_ptr<NetworkLimiter> nL =
            std::make_shared<NetworkLimiter>(ui->downloadLimitCmb, ui->uploadLimitCmb,
                                             ui->downloadLimitCustom, ui->uploadLimitCustom,
                                             ui->setLimitsBtn, "eth0");
    setNetworkLimiterPtr(nL);
    networkLimiter->bandwidthLimitsInit();
    networkLimiter->readNetworkLimits();
}

NetworkInterface::~NetworkInterface()
{
    delete ui;
}
