#include "network_interface.h"
#include "ui_network_interface.h"

NetworkInterface::NetworkInterface(QWidget *parent, QString interface) :
    QWidget(parent),
    ui(new Ui::NetworkInterface),
    interface(interface)
{
    ui->setupUi(this);
    ui->networkInterfaceLabel->setText(interface);

    std::shared_ptr<NetworkLimiter> nL =
            std::make_shared<NetworkLimiter>(ui->downloadLimitCmb, ui->uploadLimitCmb,
                                             ui->downloadLimitCustom, ui->uploadLimitCustom,
                                             ui->setLimitsBtn, interface);
    setNetworkLimiterPtr(nL);
    networkLimiter->bandwidthLimitsInit();
    networkLimiter->readNetworkLimits();
}

NetworkInterface::~NetworkInterface()
{
    delete ui;
}

void NetworkInterface::updateNetworkLimits()
{
    networkLimiter->readNetworkLimits();
}

void NetworkInterface::on_downloadLimitCustom_editingFinished()
{
    // when custom value edited set 'custom' in ComboBox
    ui->downloadLimitCmb->setCurrentIndex(ui->downloadLimitCmb->count() - 1);
}

void NetworkInterface::on_uploadLimitCustom_editingFinished()
{
    // when custom value edited set 'custom' in ComboBox
    ui->uploadLimitCmb->setCurrentIndex(ui->uploadLimitCmb->count() - 1);
}
