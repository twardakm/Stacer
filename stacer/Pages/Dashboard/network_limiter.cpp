#include "network_limiter.h"

const QString NetworkLimiter::cmdGetNetworkInterfaces = "ip link show";
const QRegularExpression NetworkLimiter::regExpInterface = QRegularExpression("[0-9]+\: ([a-zA-Z0-9]+)\:");

NetworkLimiter::NetworkLimiter(QComboBox *downloadLimitCmb, QComboBox *uploadLimitCmb,
                               QSpinBox *downloadLimitCustom, QSpinBox *uploadLimitCustom,
                               QPushButton * setLimitsBtn ,QString interface) :
    downloadLimitCmb(downloadLimitCmb),
    uploadLimitCmb(uploadLimitCmb),
    downloadLimitCustom(downloadLimitCustom),
    uploadLimitCustom(uploadLimitCustom),
    setLimitsBtn(setLimitsBtn),
    interface(interface)
{
}

void NetworkLimiter::bandwidthLimitsInit()
{    
    addBandwidthLimitsToCmb(downloadLimitCmb);
    addBandwidthLimitsToCmb(uploadLimitCmb);
}

void NetworkLimiter::readNetworkLimits()
{
    QString downloadResult = execNetworkCmd(cmdGetDownloadBandwidth);
    QString uploadResult = execNetworkCmd(cmdGetUploadBandwidth);

    // group all regular expresions into one vector
    std::vector<QRegularExpression> rxVec = {regExpValueWithUnit,
                                            regExpValue,
                                            regExpUnit};

    // get pairs for download and upload limit
    band_pair_def downloadLimit = extractBandwidthLimit(downloadResult, rxVec);
    band_pair_def uploadLimit = extractBandwidthLimit(uploadResult, rxVec);

    // set combo boxes
    updateLimitCmb(downloadLimit, downloadLimitCmb);
    updateLimitCmb(uploadLimit, uploadLimitCmb);
}

QStringList NetworkLimiter::getNetworkInterfacesList()
{
    QStringList interfaces;
    QString interface;

    // find all interfaces at once
    QRegularExpressionMatchIterator i = regExpInterface.globalMatch(execNetworkCmd(cmdGetNetworkInterfaces));
    while (i.hasNext()) if((interface = i.next().captured(1)) != "lo") interfaces << interface;

    return interfaces;
}

QString NetworkLimiter::execNetworkCmd(QString cmd)
{
    QStringList args = cmd.split(" ");
    QString command = args.at(0);
    args.pop_front();
    return CommandUtil::exec(command, args);
}

NetworkLimiter::band_pair_def NetworkLimiter::extractBandwidthLimit(QString res, std::vector<QRegularExpression> &rxVec)
{
    int i = 0;
    QStringList matches;
    QString match;
    for (auto & rx: rxVec)
    {
        match = rx.match(res).captured(0);
        matches.push_back(match);
        // two last regular expressions contains value and unit
        if (i < (rxVec.size() - 2)) res = match;
        ++i;
    }
    return std::make_pair(matches.at(rxVec.size() - 2).toInt(), matches.at(rxVec.size() - 1));
}

void NetworkLimiter::updateLimitCmb(band_pair_def pair, QComboBox *cmb)
{
    std::vector<int>::const_iterator it;
    it = std::find(bandwidthLimits.begin(), bandwidthLimits.end(), getKBValue(pair));
    // found
    if (it!=bandwidthLimits.end()){
        auto pos = std::distance(bandwidthLimits.begin(), it);
        cmb->setCurrentIndex(pos+1); // first is unlimited
    }
    else cmb->setCurrentIndex(0);
}

inline void NetworkLimiter::addBandwidthLimitsToCmb(QComboBox *cmb)
{
    cmb->addItem("unlimited");
    for (auto const& val: bandwidthLimits) cmb->addItem(QString::number(val) + " KB/s");
    cmb->addItem("custom");
}

inline int NetworkLimiter::getKBValue(band_pair_def pair)
{
    // if kilobits divide by 8, if megabits divide by 0,008 (multiply by 125)
    return (pair.second == "Kbit") ? (pair.first >> 3) : (pair.first * 125);
}
