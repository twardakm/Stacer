#ifndef NETWORK_LIMITER_H
#define NETWORK_LIMITER_H

#include <QWidget>
#include <QComboBox>
#include <QStringList>
#include <QTemporaryFile>

#include <vector>
#include <utility>
#include <string>
#include <algorithm>

#include <Managers/info_manager.h>

class NetworkLimiter
{
public:
    NetworkLimiter(QComboBox *downloadLimitCmb, QComboBox *uploadLimitCmb);

    // set bandwidth limits inside Combo Boxes
    void bandwidthLimitsInit();

    // read current network limits
    void readNetworkLimits();

protected:
    // combo boxes from Dashboard Page
    QComboBox *downloadLimitCmb;
    QComboBox *uploadLimitCmb;

    // standard bandwidth limits in KB/s
    const std::vector<int> bandwidthLimits = {50,100,150,250,350,500,750,1000};

    /* commands to get current limits
     * ----------------------------------- */
    const QString cmdGetDownloadBandwidth = "/sbin/tc filter show dev eth0 parent ffff:";
    const QString cmdGetUploadBandwidth = "/sbin/wondershaper eth0";
    const QString cmdGetKbitValue = "grep -o '[0-9]*Kbit' | grep -o '[0-9]*'";
    const QRegularExpression regExpValueWithUnit = QRegularExpression("[0-9]+.bit((.*5\n)|( burst))");
    const QRegularExpression regExpValue = QRegularExpression("[0-9]*");
    const QRegularExpression regExpUnit = QRegularExpression("[A-Z]+[a-z]*");
    /* ----------------------------------- */
    QString execNetworkCmd(QString cmd);
    // returns pair: value and unit
    std::pair<int, QString> extractBandwidthLimit(QString res, std::vector<QRegularExpression> & rxVec);

    inline void addBandwidthLimitsToCmb(QComboBox *cmb);
    inline int kbitsToKBits(int kbits);
    inline int kBitsToKbits(int kBits);
};

#endif // NETWORK_LIMITER_H
