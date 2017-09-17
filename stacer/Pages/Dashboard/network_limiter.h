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

class NetworkLimiter : QObject
{
    Q_OBJECT
public:
    NetworkLimiter(QComboBox *downloadLimitCmb, QComboBox *uploadLimitCmb,
                   QSpinBox *downloadLimitCustom, QSpinBox *uploadLimitCustom,
                   QPushButton * setLimitsBtn, QString interface);

    // set bandwidth limits inside Combo Boxes
    void bandwidthLimitsInit();

    // read current network limits
    void readNetworkLimits();

    // return all network interfaces available in the system
    static QStringList getNetworkInterfacesList();

protected:
    typedef enum e_BandwidthUnits
    {
        kilobit,
        megabit,
        kilobyte,
        megabyte
    } e_BandwidthUnits;

    typedef std::pair<int, e_BandwidthUnits> band_pair_def;

    // combo boxes from Dashboard Page
    QComboBox *downloadLimitCmb;
    QComboBox *uploadLimitCmb;
    QSpinBox *downloadLimitCustom;
    QSpinBox *uploadLimitCustom;
    QPushButton *setLimitsBtn;

    // standard bandwidth limits in KB/s
    const std::vector<int> bandwidthLimits = {50,100,150,250,350,500,750,1000};

    /*
     * ----------------------------------- */
    static const QString cmdGetNetworkInterfaces;
    static const QRegularExpression regExpInterface;
    /* ----------------------------------- */

    /* commands to get current limits
     * ----------------------------------- */
    const QString interface;
    const QString cmdGetDownloadBandwidth = "/sbin/tc filter show dev " + interface + " parent ffff:";
    const QString cmdGetUploadBandwidth = "/sbin/wondershaper " + interface;
    const QRegularExpression regExpValueWithUnit = QRegularExpression("[0-9]+.bit((.*5\n)|( burst))");
    const QRegularExpression regExpValue = QRegularExpression("[0-9]*");
    const QRegularExpression regExpUnit = QRegularExpression("[A-Z]+[a-z]*");
    const QString cmdSetBandwidth = "/sbin/wondershaper " + interface + " <down> <up>";
    /* ----------------------------------- */
    static QString execNetworkCmd(QString cmd, bool sudo = false);
    // returns pair: value and unit
    band_pair_def extractBandwidthLimit(QString res, std::vector<QRegularExpression> & rxVec);
    void updateLimitCmb(band_pair_def pair, QComboBox *cmb, QSpinBox *spin);

    inline void addBandwidthLimitsToCmb(QComboBox *cmb);

    // convert between KB, MB and Kb, Mb
    inline int getKBValue(band_pair_def pair);
    inline int getkbValue(band_pair_def pair);

    void getBandwidthToSet(int & bandwidth, const QComboBox * cmb, const QSpinBox * spin);

private slots:
    void setButton();
};

#endif // NETWORK_LIMITER_H
