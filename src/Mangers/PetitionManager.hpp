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

    void init(const QString &petitionUrl);

public slots:
    void executeRequestToBot(const RequestToBot requestToBot);

signals:
    void signalResponseFromBotReady(QSharedPointer<ResponseFromBot>);

private slots:
    void slotPetitionVotesTotalReceived(int totalVotesFromServer);
    void slotPetitionVotesListReceived(QSharedPointer<PetitionVotes> votes, int page);
    bool saveVotesListToDb(QSharedPointer<PetitionVotes> votes);

private:
    void removeChatIdsFromMapRequestToBot(const QVector<std::int64_t>& vecChatIds);
    QSharedPointer<ResponseFromBot> findMatches(const RequestToBot& rq);

private:
    QScopedPointer<ApiClientPetition> apiClientPetition;
    QMap<std::int64_t, RequestToBot> mapRequestToBot;
    QList<RequestToBot::RequestType> listRequestTypeInProgress;
};

