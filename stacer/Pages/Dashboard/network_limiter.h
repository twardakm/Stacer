#ifndef NETWORK_LIMITER_H
#define NETWORK_LIMITER_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
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
    NetworkLimiter(QComboBox *downloadLimitCmb, QComboBox *uploadLimitCmb,
                   QSpinBox *downloadLimitCustom, QSpinBox *uploadLimitCustom,
                   QPushButton * setLimitsBtn ,QString interface);

    // set bandwidth limits inside Combo Boxes
    void bandwidthLimitsInit();

    // read current network limits
    void readNetworkLimits();

protected:
    typedef std::pair<int, QString> band_pair_def;

    // combo boxes from Dashboard Page
    QComboBox *downloadLimitCmb;
    QComboBox *uploadLimitCmb;
    QSpinBox *downloadLimitCustom;
    QSpinBox *uploadLimitCustom;
    QPushButton *setLimitsBtn;

    // standard bandwidth limits in KB/s
    const std::vector<int> bandwidthLimits = {50,100,150,250,350,500,750,1000};

    /* commands to get current limits
     * ----------------------------------- */
    const QString interface;
    const QString cmdGetDownloadBandwidth = "/sbin/tc filter show dev " + interface + " parent ffff:";
    const QString cmdGetUploadBandwidth = "/sbin/wondershaper " + interface;
    const QRegularExpression regExpValueWithUnit = QRegularExpression("[0-9]+.bit((.*5\n)|( burst))");
    const QRegularExpression regExpValue = QRegularExpression("[0-9]*");
    const QRegularExpression regExpUnit = QRegularExpression("[A-Z]+[a-z]*");
    /* ----------------------------------- */
    QString execNetworkCmd(QString cmd);
    // returns pair: value and unit
    band_pair_def extractBandwidthLimit(QString res, std::vector<QRegularExpression> & rxVec);
    void updateLimitCmb(band_pair_def pair, QComboBox *cmb);

    inline void addBandwidthLimitsToCmb(QComboBox *cmb);
    inline int getKBValue(band_pair_def pair);
};

#endif // NETWORK_LIMITER_H
