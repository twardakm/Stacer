#include "network_limiter.h"

NetworkLimiter::NetworkLimiter(QComboBox *downloadLimitCmb, QComboBox *uploadLimitCmb) :
    downloadLimitCmb(downloadLimitCmb),
    uploadLimitCmb(uploadLimitCmb)
{
}

void NetworkLimiter::bandwidthLimitsInit()
{
    addBandwidthLimitsToCmb(downloadLimitCmb);
    addBandwidthLimitsToCmb(uploadLimitCmb);
}

void NetworkLimiter::readNetworkLimits()
{
    // /sbin/tc filter show dev eth0 parent ffff: | grep -o "[0-9]*Kbit" | grep -o "[0-9]*" && /sbin/wondershaper eth0 | grep "prio 5" | grep -o "[0-9]*Kbit" | grep -o "[0-9]*"
    QString downloadResult = execNetworkCmd(cmdGetDownloadBandwidth);
    QString uploadResult = execNetworkCmd(cmdGetUploadBandwidth);

    // group all regular expresions into one vector
    std::vector<QRegularExpression> rxVec = {regExpValueWithUnit,
                                            regExpValue,
                                            regExpUnit};

    // get pairs for download and upload limit
    std::pair<int, QString> downloadLimit = extractBandwidthLimit(downloadResult, rxVec);
    std::pair<int, QString> uploadLimit = extractBandwidthLimit(uploadResult, rxVec);

    QString temp = downloadLimit.second;
}

QString NetworkLimiter::execNetworkCmd(QString cmd)
{
    QStringList args = cmd.split(" ");
    QString command = args.at(0);
    args.pop_front();
    return CommandUtil::exec(command, args);
}

std::pair<int, QString> NetworkLimiter::extractBandwidthLimit(QString res, std::vector<QRegularExpression> &rxVec)
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

inline void NetworkLimiter::addBandwidthLimitsToCmb(QComboBox *cmb)
{
    cmb->addItem("unlimited");
    for (auto const& val: bandwidthLimits) cmb->addItem(QString::number(val) + " KB/s");
}

inline int NetworkLimiter::kbitsToKBits(int kbits) {return kbits<<3;}
inline int NetworkLimiter::kBitsToKbits(int kBits) {return kBits>>3;}
