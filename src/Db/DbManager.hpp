#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "DbAccessor.hpp"
#include "DbTables.hpp"
#include "LogManager.hpp"
#include "DbPetitionVote.hpp"

class DbManager : public QObject
{
    Q_OBJECT
public:
    static DbManager& instance();

    bool init(const QString &pathDatabase);

    void closeDatabase();
    bool doesDbExists();
    qint64 sizeDb();

    void createTables();

    void insertOrUpdatePetitionUrlForChatId(const std::string& petitionUrl, const std::int64_t chat_id);
    QString getPetitionUrlForChatId(const std::int64_t chat_id);

    void insertOrReplaceDefaultPetitionUrl(const std::string& petitionUrl);
    QString getDefaultPetitionUrl();

    void saveDbPetitionVote(const DbPetitionVote& dbPetitionVote, const QString& petition_url);
    bool dbPetitionVoteExist(const DbPetitionVote& dbPetitionVote, const QString& petition_url);
    int getCountTotalVotes(const QString& petition_url);
    QVector<QSharedPointer<DbPetitionVote>> findMatches(const QStringList& words, const QString& petition_url);

private:
    Q_DISABLE_COPY_MOVE(DbManager)
    explicit DbManager();
    ~DbManager();

private:
    QSqlDatabase _sqlDb;
    QString _pathDb;
};

