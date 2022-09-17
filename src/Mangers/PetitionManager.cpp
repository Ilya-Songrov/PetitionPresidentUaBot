#include "PetitionManager.hpp"

PetitionManager::PetitionManager(QObject *parent)
    : QObject{parent}
    , apiClientPetition(nullptr)
    , lastRqPage(0)
{

}

void PetitionManager::init()
{
    apiClientPetition.reset(new ApiClientPetition());
    apiClientPetition->init();
    connect(apiClientPetition.get(), &ApiClientPetition::signalPetitionVotesTotalReceived,
            this, &PetitionManager::slotPetitionVotesTotalReceived);
    connect(apiClientPetition.get(), &ApiClientPetition::signalPetitionVotesListReceived,
            this, &PetitionManager::slotPetitionVotesListReceived);

    mapRequestToBot.clear();
    lastRqPage = 0;
}

void PetitionManager::fillDatabase()
{
    runfillingDatabase();
    init();
}

void PetitionManager::executeRequestToBot(const RequestToBot requestToBot)
{
    if (mapRequestToBot.isEmpty()) {
        apiClientPetition->requestToGetPetitionVotesTotal();
    }
    mapRequestToBot.insert(requestToBot.chat_id, requestToBot);
    QSharedPointer<ResponseFromBot> rs(new ResponseFromBot(requestToBot.chat_id, "Зачекайте будь ласка. Оновлюю базу..."));
    emit signalListResultReady(rs);
}

void PetitionManager::slotPetitionVotesTotalReceived(int totalVotesFromServer)
{
    const int totalVotesFromDb = DbManager::instance().getCountTotalVotes();
    qDebug() << "TotalVotes from server:" << totalVotesFromServer << Qt::endl;
    qDebug() << "TotalVotes from db:" << totalVotesFromDb << Qt::endl;
    totalVotesFromServer = totalVotesFromServer == -2 ? apiClientPetition->getLastTotalPetitionVotes() : totalVotesFromServer;
    QVector<std::int64_t> vecRemoveRs;
    for (const RequestToBot& rq: qAsConst(mapRequestToBot)) {
        if (rq.request_type == RequestToBot::CheckCountVotes) {
            const std::string countRes = "Підписали: " + std::to_string(totalVotesFromServer);
            QSharedPointer<ResponseFromBot> rs(new ResponseFromBot(rq.chat_id, countRes));
            emit signalListResultReady(rs);
            vecRemoveRs.append(rq.chat_id);
        }
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    for(auto it = mapRequestToBot.begin(); it != mapRequestToBot.end(); ) {
        if(vecRemoveRs.contains(it.key())){
            it = mapRequestToBot.erase(it);
        }
        else {
            ++it;
        }
    }
#else
    mapRequestToBot.removeIf([&vecRemoveRs](std::pair<std::int64_t, const RequestToBot&> pair){ return vecRemoveRs.contains(pair.first); });
#endif
    if (mapRequestToBot.isEmpty()) {
        return;
    }

    if (totalVotesFromDb >= totalVotesFromServer
            || totalVotesFromServer == -1
            || totalVotesFromServer == -2) {
        emitResult();
        return;
    }
    lastRqPage = 0;
    apiClientPetition->requestToGetPetitionVotesList(++lastRqPage);
}

void PetitionManager::slotPetitionVotesListReceived(QSharedPointer<PetitionVotes> votes)
{
    if (votes->rows.isEmpty()) {
        emitResult();
        return;
    }
    const bool isDataExist = saveVotesListToDb(votes);
    if (isDataExist) {
        emitResult();
        return;
    }
    apiClientPetition->requestToGetPetitionVotesList(++lastRqPage);
}

bool PetitionManager::saveVotesListToDb(QSharedPointer<PetitionVotes> votes)
{
    int countExistingVotes = 0;
    for (const PetitionVotesNode& node : votes->rows) {
        DbPetitionVote dbPetitionVote(node.number,
                                      node.name,
                                      node.date);
        if (DbManager::instance().dbPetitionVoteExist(dbPetitionVote)) {
            ++countExistingVotes;
            continue;
        }
        DbManager::instance().saveDbPetitionVote(dbPetitionVote);
    }
    return countExistingVotes >= 30;
}

void PetitionManager::emitResult()
{
    for (const RequestToBot& rq: qAsConst(mapRequestToBot)) {
        QSharedPointer<ResponseFromBot> rs = findMatches(rq);
        emit signalListResultReady(rs);
    }
    mapRequestToBot.clear();
}

QSharedPointer<ResponseFromBot> PetitionManager::findMatches(const RequestToBot& rq)
{
    const int maxRsSize = 4096;
    const QString dots = "...";
    const QString textQt = QString::fromStdString(rq.request_text);
    const QStringList list = textQt.split(' ');
    const QVector<QSharedPointer<DbPetitionVote>> vecRes = DbManager::instance().findMatches(list);
    QString res = "👍 Ці люди молодці:\n";
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

void PetitionManager::runfillingDatabase()
{
    bool allDataIsReceived  = false;
    auto functionSaveVotesListToDb = [this, &allDataIsReceived](QSharedPointer<PetitionVotes> votes)
    {
        saveVotesListToDb(votes);
        allDataIsReceived = votes->rows.isEmpty();
    };

    apiClientPetition.reset(new ApiClientPetition());
    apiClientPetition->init();
    connect(apiClientPetition.get(), &ApiClientPetition::signalPetitionVotesListReceived,
            this, functionSaveVotesListToDb);

    while(true)
    {
        if (allDataIsReceived) {
            return;
        }
        QEventLoop loop;
        QTimer timer;
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        QObject::connect(apiClientPetition.get(), &ApiClientPetition::signalPetitionVotesListReceived,
                         &loop, &QEventLoop::quit);
        timer.start(3600000);
        apiClientPetition->requestToGetPetitionVotesList(++lastRqPage);
        loop.exec();
        QThread::msleep(500);
    }

}
