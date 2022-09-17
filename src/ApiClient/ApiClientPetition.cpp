#include "ApiClientPetition.hpp"

ApiClientPetition::ApiClientPetition(QObject* parent)
    : ApiClientAbstract{parent}
    , _urlPetitionVotesTotal("https://petition.president.gov.ua/petition/146508")
    , _urlPetitionVotesList("https://petition.president.gov.ua/petition/146508/votes")
    , votingWasEnded(false)
{

}

void ApiClientPetition::init()
{
    createMngrObject();
}

void ApiClientPetition::requestToGetPetitionVotesTotal()
{
    if (votingWasEnded) {
        qDebug() << "VotingWasEnded: " << votingWasEnded << Qt::endl;
        QTimer::singleShot(0, this, std::bind(&ApiClientPetition::signalPetitionVotesTotalReceived, this, -2));
        return;
    }
    QNetworkRequest request(_urlPetitionVotesTotal);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    printRequest(request, "", second_category);
    _mngr->get(request);
}

void ApiClientPetition::requestToGetPetitionVotesList(const int page)
{
    QNetworkRequest request(_urlPetitionVotesList + "/" + QString::number(page));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    printRequest(request, "", second_category);
    _mngr->get(request);
}

void ApiClientPetition::parseResponse(QNetworkReply* reply)
{
    const QString url = reply->url().toString();
    const QNetworkReply::NetworkError networkError = reply->error();
    const QByteArray arrReply = parseNetworkReply(reply, first_category);
    if (networkError != QNetworkReply::NoError) {
        emit signalErrorWhileGettingResponse(QString(this->metaObject()->className()) + "\n" + url);
    }

    if (url == _urlPetitionVotesTotal) {
        votingWasEnded = arrReply.contains("<div class=\"votes_progress_label\"><span>Збір підписів завершено</span></div>");
        static QRegularExpression dataVotes(" data-votes=\"[0-9]*\">");
        QRegularExpressionMatch matchDataVotes = dataVotes.match(arrReply);
        QString totalNumStr;
        if (matchDataVotes.hasMatch()) {
            totalNumStr = matchDataVotes.captured().toUtf8();
            totalNumStr.chop(2);
            totalNumStr.remove(0, 13);
        }
        else{
            totalNumStr = "-1";
            qCWarning(first_category) << "error while extract total votes" << Qt::endl;
        }

        emit signalPetitionVotesTotalReceived(totalNumStr.toInt());
    }
    else if(url.contains(_urlPetitionVotesList)){
        QSharedPointer<PetitionVotes> petitionVotes = parseXmlRespnse(arrReply);
        emit signalPetitionVotesListReceived(petitionVotes);
    }
}

QSharedPointer<PetitionVotes> ApiClientPetition::parseXmlRespnse(const QByteArray& arrReply)
{
//    <div class="table">
//        <div class="table_row">
//            <div class="table_cell number">1.</div>
//            <div class="table_cell name">Яровка Олег Володимирович</div>
//            <div class="table_cell date">16 липня 2022</div>
//        </div>
//        <div class="table_row">
//            <div class="table_cell number">2.</div>
//            <div class="table_cell name">Щербакова Світлана Олександрівна</div>
//            <div class="table_cell date">16 липня 2022</div>
//        </div>
//    </div>

    QSharedPointer<PetitionVotes> petitionVotes(new PetitionVotes());
    QDomDocument doc;
    doc.setContent(arrReply);
    QDomNodeList nodeListTable = doc.childNodes();
    if (nodeListTable.isEmpty()) {
        return petitionVotes;
    }
    QDomNodeList nodeListTableRow = nodeListTable.at(0).childNodes();

    for (int var = 0; var < nodeListTableRow.size(); ++var) {
        QDomNode nodeTableRow           = nodeListTableRow.at(var);
        QDomNodeList nodeListTableCell  = nodeTableRow.childNodes();
        if (nodeListTableCell.size() < 3) {
            continue;
        }
        QString number         = nodeListTableCell.at(0).toElement().text();
        QString name           = nodeListTableCell.at(1).toElement().text();
        QString date           = nodeListTableCell.at(2).toElement().text();
        petitionVotes->rows.append(PetitionVotesNode(number, name, date));
    }
    return petitionVotes;
}

void ApiClientPetition::createMngrObject()
{
    _mngr->disconnect();
    _mngr.reset(new QNetworkAccessManager());
    _mngr->setTransferTimeout(timeoutMsec);
    connect(_mngr.get(), &QNetworkAccessManager::sslErrors, this, &ApiClientAbstract::slotSSLErrorsGeneral);
    connect(_mngr.get(), &QNetworkAccessManager::finished, this, &ApiClientPetition::parseResponse);
}

