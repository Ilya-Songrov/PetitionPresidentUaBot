#pragma once

#include <QObject>
#include <QThread>

#include "ApiClientPetition.hpp"
#include "RequestToBot.hpp"
#include "ResponseFromBot.hpp"
#include "DbManager.hpp"

class PetitionManager : public QObject
{
    Q_OBJECT
public:
    explicit PetitionManager(QObject *parent = nullptr);

    void init();
    void fillDatabase();

public slots:
    void executeRequestToBot(const RequestToBot requestToBot);

signals:
    void signalListResultReady(QSharedPointer<ResponseFromBot>);

private slots:
    void slotPetitionVotesTotalReceived(int totalVotes);
    void slotPetitionVotesListReceived(QSharedPointer<PetitionVotes> votes);
    bool saveVotesListToDb(QSharedPointer<PetitionVotes> votes);

private:
    void emitResult();
    QSharedPointer<ResponseFromBot> findMatches(const RequestToBot& rq);
    void runfillingDatabase();

private:
    QScopedPointer<ApiClientPetition> apiClientPetition;
    QMap<std::int64_t, RequestToBot> mapRequestToBot;
    int lastRqPage;
};

