#pragma once

#include <QObject>
#include <QQueue>

#include "ApiClientAbstract.hpp"
#include "PetitionVotes.hpp"

class ApiClientPetition : public ApiClientAbstract
{
    Q_OBJECT
public:
    explicit ApiClientPetition(QObject *parent = nullptr);

    void init();

    void requestToGetPetitionVotesTotal();
    void requestToGetPetitionVotesList(const int page);

signals:
    void signalPetitionVotesTotalReceived(int totalVotes);
    void signalPetitionVotesListReceived(QSharedPointer<PetitionVotes>);

private:
    void parseResponse(QNetworkReply *reply);
    QSharedPointer<PetitionVotes> parseXmlRespnse(const QByteArray& arrReply);

    void createMngrObject();

private:
    const QString _urlPetitionVotesTotal;
    const QString _urlPetitionVotesList;
    bool votingWasEnded;
};

