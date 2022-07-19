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

void PetitionManager::checkRequestToBot(const RequestToBot requestToBot)
{
    if (mapRequestToBot.isEmpty()) {
        apiClientPetition->requestToGetPetitionVotesTotal();
    }
    mapRequestToBot.insert(requestToBot.chat_id, requestToBot);
}

void PetitionManager::slotPetitionVotesTotalReceived(int totalVotes)
{
    const int countTotalVotes = DbManager::instance().getCountTotalVotes();
    if (countTotalVotes >= totalVotes || totalVotes == -1) {
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
    saveVotesListToDb(votes);
    apiClientPetition->requestToGetPetitionVotesList(++lastRqPage);
}

void PetitionManager::saveVotesListToDb(QSharedPointer<PetitionVotes> votes)
{
    for (const PetitionVotesNode& node : votes->rows) {
        DbPetitionVote dbPetitionVote(node.number,
                                      node.name,
                                      node.date);
        DbManager::instance().saveDbPetitionVote(dbPetitionVote);
    }
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
    QString res = "Знайдені збіги:\n";
    for (const QSharedPointer<DbPetitionVote>& dbPetitionVote : vecRes) {
        QString vote = dbPetitionVote->number_str
                + " "
                + dbPetitionVote->name
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
        res = "Нічого не знайденно";
    }
    QSharedPointer<ResponseFromBot> rs(new ResponseFromBot);
    rs->chat_id = rq.chat_id;
    rs->response_text = res.toStdString();
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
