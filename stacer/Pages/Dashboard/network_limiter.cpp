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
    // connect buttons with events
    connect(setLimitsBtn, SIGNAL(released()), this, SLOT(setButton()));
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
    updateLimitCmb(downloadLimit, downloadLimitCmb, downloadLimitCustom);
    updateLimitCmb(uploadLimit, uploadLimitCmb, uploadLimitCustom);
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

QString NetworkLimiter::execNetworkCmd(QString cmd, bool sudo)
{
    QStringList args = cmd.split(" ");
    QString command = args.at(0);
    args.pop_front();
    return sudo ? CommandUtil::sudoExec(command, args) : CommandUtil::exec(command,args);
}

NetworkLimiter::band_pair_def NetworkLimiter::extractBandwidthLimit(QString res, std::vector<QRegularExpression> &rxVec)
{
    int i = 0;
    QStringList matches;
    QString match;
    e_BandwidthUnits unit;
    for (auto & rx: rxVec)
    {
        match = rx.match(res).captured(0);
        matches.push_back(match);
        // two last regular expressions contains value and unit
        if (i < (rxVec.size() - 2)) res = match;
        ++i;
    }

    unit = (matches.at(rxVec.size() - 1) == "Kbit") ? e_BandwidthUnits::kilobit : e_BandwidthUnits::kilobyte;
    return std::make_pair(matches.at(rxVec.size() - 2).toInt(), unit);
}

void NetworkLimiter::updateLimitCmb(band_pair_def pair, QComboBox *cmb, QSpinBox *spin)
{
    std::vector<int>::const_iterator it;
    int kBValue = getKBValue(pair);
    it = std::find(bandwidthLimits.begin(), bandwidthLimits.end(), kBValue);
    // found
    if (it!=bandwidthLimits.end()){
        auto pos = std::distance(bandwidthLimits.begin(), it);
        cmb->setCurrentIndex(pos+1); // first is unlimited
    }
    // if not found but not zero, set Custom
    else if (kBValue != 0)
    {
        cmb->setCurrentIndex(cmb->count() - 1);
        spin->setValue(kBValue);
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
    return (pair.second == e_BandwidthUnits::kilobit) ? (pair.first >> 3) : (pair.first * 125);
}

inline int NetworkLimiter::getkbValue(band_pair_def pair)
{
    // if KB multiply by 8, if MB multiply by 8 and divide by 1024 (finally multiply by 0,0078125)
    return (pair.second == e_BandwidthUnits::kilobyte) ? (pair.first << 3) : (pair.first * 0.0078125);
}

void NetworkLimiter::getBandwidthToSet(int &bandwidth, const QComboBox *cmb, const QSpinBox *spin)
{
    /*
     * if unlimited, set 0
     * if custom, take value from spinbox
     */

    if (cmb->currentIndex() == 0) bandwidth = 0;
    else if (cmb->currentIndex() == cmb->count() - 1)
        bandwidth = getkbValue(std::make_pair(spin->value(), e_BandwidthUnits::kilobyte));
    else bandwidth = getkbValue(std::make_pair(cmb->currentText().split(" ")[0].toInt(), e_BandwidthUnits::kilobyte));
}

void NetworkLimiter::setButton()
{
    QString cmd = cmdSetBandwidth;
    int download, upload;

    getBandwidthToSet(download, downloadLimitCmb, downloadLimitCustom);
    getBandwidthToSet(upload, uploadLimitCmb, uploadLimitCustom);

    cmd.replace("<down>", QString::number(download));
    cmd.replace("<up>", QString::number(upload));

    execNetworkCmd(cmd, true);
}
