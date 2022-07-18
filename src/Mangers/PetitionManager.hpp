#pragma once

#include <QObject>
#include <QThread>

#include "ApiClientPetition.hpp"
#include "RequestToBot.hpp"
#include "ResponseFromBot.hpp"

class PetitionManager : public QObject
{
    Q_OBJECT
public:
    explicit PetitionManager(QObject *parent = nullptr);

    void init();

public slots:
    void checkRequestToBot(const RequestToBot requestToBot);

signals:
    void signalListResultReady(QSharedPointer<ResponseFromBot>);

private slots:
    void slotPetitionVotesTotalReceived(int totalVotes);
    void slotPetitionVotesListReceived(QSharedPointer<PetitionVotes> votes);

private:
    void emitResult();
    QSharedPointer<ResponseFromBot> findMatches(const RequestToBot& rq);

private:
    QScopedPointer<ApiClientPetition> apiClientPetition;
    QMap<std::int64_t, RequestToBot> mapRequestToBot;
    QVector<PetitionVotesNode> vecPetitionVotes;
//    QThread thread;
    int lastRqPage;
};

