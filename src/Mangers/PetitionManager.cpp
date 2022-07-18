#include "PetitionManager.hpp"

PetitionManager::PetitionManager(QObject *parent)
    : QObject{parent}
    , apiClientPetition(nullptr)
    , lastRqPage(0)
{
// 17 липня 2022
}

void PetitionManager::init()
{
    apiClientPetition.reset(new ApiClientPetition());
//    apiClientPetition->moveToThread(&thread);
    apiClientPetition->init();
    connect(apiClientPetition.get(), &ApiClientPetition::signalPetitionVotesTotalReceived,
            this, &PetitionManager::slotPetitionVotesTotalReceived);
    connect(apiClientPetition.get(), &ApiClientPetition::signalPetitionVotesListReceived,
            this, &PetitionManager::slotPetitionVotesListReceived);
}

void PetitionManager::checkRequestToBot(const RequestToBot requestToBot)
{
    qDebug() << "print_function:" << __FUNCTION__ << "request_text:" << requestToBot.request_text.c_str() << Qt::endl;
    if (!vecPetitionVotes.isEmpty()) {
        emitResult();
        return;
    }

    if (mapRequestToBot.isEmpty()) {
        apiClientPetition->requestToGetPetitionVotesTotal();
    }
    mapRequestToBot.insert(requestToBot.chat_id, requestToBot);
}

void PetitionManager::slotPetitionVotesTotalReceived(int totalVotes)
{
    if (totalVotes == vecPetitionVotes.size() || totalVotes == -1) {
        emitResult();
        return;
    }
    vecPetitionVotes.clear();
    lastRqPage = 0;
    apiClientPetition->requestToGetPetitionVotesList(++lastRqPage);
}

void PetitionManager::slotPetitionVotesListReceived(QSharedPointer<PetitionVotes> votes)
{
    if (votes->rows.isEmpty()) {
        emitResult();
        return;
    }
    vecPetitionVotes += votes->rows;
    apiClientPetition->requestToGetPetitionVotesList(++lastRqPage);
}

void PetitionManager::emitResult()
{
    for (const RequestToBot& rq: qAsConst(mapRequestToBot)) {
        QSharedPointer<ResponseFromBot> rs = findMatches(rq);
        emit signalListResultReady(rs);
        qDebug() << "print_function:" << __FUNCTION__ << "response_text:" << rs->response_text.c_str() << Qt::endl;
    }
    vecPetitionVotes.clear();
}

QSharedPointer<ResponseFromBot> PetitionManager::findMatches(const RequestToBot& rq)
{
    const QString textQt = QString::fromStdString(rq.request_text);
    const QStringList list = textQt.split(' ');
    QString res;
    for (const PetitionVotesNode& node : vecPetitionVotes) {
        for (const QString& value : list) {
            if (node.name.contains(value)) {
                res += node.number
                        + " "
                        + node.name
                        + " ("
                        + node.date
                        + ")"
                        + "\n"
                        ;
            }
        }
    }
    if (res.isEmpty()) {
        res = "Нічого не знайденно";
    }
    QSharedPointer<ResponseFromBot> rs(new ResponseFromBot);
    rs->chat_id = rq.chat_id;
    rs->response_text = res.toStdString();
    return rs;
}
