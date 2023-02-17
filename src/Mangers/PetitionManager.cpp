#include "PetitionManager.hpp"
#include "DbManager.hpp"

PetitionManager::PetitionManager(QObject *parent)
    : QObject{parent}
    , apiClientPetition(nullptr)
{

}

void PetitionManager::init(const QString &petitionUrl)
{
    apiClientPetition.reset(new ApiClientPetition(petitionUrl));
    apiClientPetition->init();
    connect(apiClientPetition.get(), &ApiClientPetition::signalPetitionVotesTotalReceived,
            this, &PetitionManager::slotPetitionVotesTotalReceived);
    connect(apiClientPetition.get(), &ApiClientPetition::signalPetitionVotesListReceived,
            this, &PetitionManager::slotPetitionVotesListReceived);

    mapRequestToBot.clear();
    listRequestTypeInProgress.clear();
}

void PetitionManager::executeRequestToBot(const RequestToBot requestToBot)
{
    if (requestToBot.request_type == RequestToBot::RequestType::RequestToSearch && !listRequestTypeInProgress.contains(RequestToBot::RequestType::RequestToSearch)){
        apiClientPetition->requestToGetPetitionVotesList(1); //NOTE: The last person to vote is on the first page
        listRequestTypeInProgress.append(RequestToBot::RequestType::RequestToSearch);
    }
    else if (requestToBot.request_type == RequestToBot::RequestType::CheckCountVotes && !listRequestTypeInProgress.contains(RequestToBot::RequestType::CheckCountVotes)){
        apiClientPetition->requestToGetPetitionVotesTotal();
        listRequestTypeInProgress.append(RequestToBot::RequestType::CheckCountVotes);
    }
    else if (requestToBot.request_type == RequestToBot::RequestType::GetDescription){
        apiClientPetition->requestToGetPetitionVotesTotal();
        QSharedPointer<ResponseFromBot> rs(new ResponseFromBot(requestToBot.chat_id, "Ви слідкуєте за наступною петицією: " + apiClientPetition->getPetitionUrl().toStdString()));
        emit signalResponseFromBotReady(rs);
        return;
    }
    mapRequestToBot.insert(requestToBot.chat_id, requestToBot);
    QSharedPointer<ResponseFromBot> rs(new ResponseFromBot(requestToBot.chat_id, "Зачекайте будь ласка. Оновлюю базу..."));
    emit signalResponseFromBotReady(rs);
}

void PetitionManager::slotPetitionVotesTotalReceived(int totalVotesFromServer)
{
    const int totalVotesFromDb = DbManager::instance().getCountTotalVotes(apiClientPetition->getPetitionUrl());
    qDebug() << "TotalVotes from server:" << totalVotesFromServer
             << Qt::endl
             << "TotalVotes from db:" << totalVotesFromDb
             << Qt::endl;
    const int totalVotesToReturn = totalVotesFromServer == -2 ? apiClientPetition->getLastTotalPetitionVotes() : totalVotesFromServer;
    QVector<std::int64_t> vecChatIds;
    for (const RequestToBot& rq: qAsConst(mapRequestToBot)) {
        if (rq.request_type == RequestToBot::RequestType::CheckCountVotes) {
            const std::string countRes = "Підписали: " + std::to_string(totalVotesToReturn);
            QSharedPointer<ResponseFromBot> rs(new ResponseFromBot(rq.chat_id, countRes));
            emit signalResponseFromBotReady(rs);
            vecChatIds.append(rq.chat_id);
        }
    }
    removeChatIdsFromMapRequestToBot(vecChatIds);
    listRequestTypeInProgress.removeAll(RequestToBot::RequestType::CheckCountVotes);
}

void PetitionManager::slotPetitionVotesListReceived(QSharedPointer<PetitionVotes> votes, int page)
{
    bool isEmitResult = votes->rows.isEmpty();
    if (!votes->rows.isEmpty()) {
        const bool isDataExist = saveVotesListToDb(votes);
        isEmitResult = isDataExist;
    }
    if (isEmitResult) {
        QVector<std::int64_t> vecChatIds;
        for (const RequestToBot& rq: qAsConst(mapRequestToBot)) {
            QSharedPointer<ResponseFromBot> rs = findMatches(rq);
            emit signalResponseFromBotReady(rs);
            vecChatIds.append(rq.chat_id);
        }
        removeChatIdsFromMapRequestToBot(vecChatIds);
        listRequestTypeInProgress.removeAll(RequestToBot::RequestType::RequestToSearch);
        return;
    }
    QTimer::singleShot(500, [page, this](){ apiClientPetition->requestToGetPetitionVotesList(page + 1); });
}

bool PetitionManager::saveVotesListToDb(QSharedPointer<PetitionVotes> votes)
{
    int countExistingVotes = 0;
    for (const PetitionVotesNode& node : votes->rows) {
        DbPetitionVote dbPetitionVote(node.number,
                                      node.name,
                                      node.date);
        if (DbManager::instance().dbPetitionVoteExist(dbPetitionVote, apiClientPetition->getPetitionUrl())) {
            ++countExistingVotes;
            continue;
        }
        DbManager::instance().saveDbPetitionVote(dbPetitionVote, apiClientPetition->getPetitionUrl());
    }
    return countExistingVotes >= votes->rows.count();
}

void PetitionManager::removeChatIdsFromMapRequestToBot(const QVector<int64_t> &vecChatIds)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    for(auto it = mapRequestToBot.begin(); it != mapRequestToBot.end(); ) {
        if(vecChatIds.contains(it.key())){
            it = mapRequestToBot.erase(it);
        }
        else {
            ++it;
        }
    }
#else
    mapRequestToBot.removeIf([&vecChatIds](std::pair<std::int64_t, const RequestToBot&> pair){ return vecChatIds.contains(pair.first); });
#endif
}

QSharedPointer<ResponseFromBot> PetitionManager::findMatches(const RequestToBot& rq)
{
    const int maxRsSize = 4096;
    const QString dots = "...";
    const QString textQt = QString::fromStdString(rq.request_text);
    const QStringList list = textQt.split(' ');
    const QVector<QSharedPointer<DbPetitionVote>> vecRes = DbManager::instance().findMatches(list, apiClientPetition->getPetitionUrl());
    QString res = "📌 Ці люди підписали петицію:\n";
    for (const QSharedPointer<DbPetitionVote>& dbPetitionVote : vecRes) {
        QString vote = dbPetitionVote->name
                + " ("
                + dbPetitionVote->date_str
                + ")"
                + "\n"
                ;
        if (res.size() + vote.size() + dots.size() > maxRsSize) {
            res += dots;
            break;
        }
        res += vote;
    }
    if (vecRes.isEmpty()) {
        res = "Нічого не знайденно🤷‍";
    }
    QSharedPointer<ResponseFromBot> rs(new ResponseFromBot(rq.chat_id, res.toStdString()));
    return rs;
}

