#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "DbWorker.hpp"
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

    void saveDbPetitionVote(const DbPetitionVote& dbPetitionVote);
    bool dbPetitionVoteExist(const DbPetitionVote& dbPetitionVote);
    int getCountTotalVotes();
    QVector<QSharedPointer<DbPetitionVote>> findMatches(const QStringList& words);

private:
    Q_DISABLE_COPY_MOVE(DbManager)
    explicit DbManager();
    ~DbManager();

private:
    QSqlDatabase _sqlDb;
    QString _pathDb;
};

