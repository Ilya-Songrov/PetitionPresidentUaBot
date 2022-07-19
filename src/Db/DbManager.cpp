#include "DbManager.hpp"

DbManager& DbManager::instance()
{
    static DbManager dbManager;
    return dbManager;
}

bool DbManager::init(const QString& pathDatabase)
{
    _pathDb = pathDatabase;
    _sqlDb = QSqlDatabase::addDatabase("QSQLITE");
    _sqlDb.setDatabaseName(_pathDb);
    if(!_sqlDb.open()){
        qCCritical(db_category) << "Database not opened:" << _pathDb << Qt::endl;
        return false;
    }
    return true;

}

void DbManager::closeDatabase()
{
    if (_sqlDb.isOpen()){
        _sqlDb.close();
    }
}

bool DbManager::doesDbExists()
{
    return QFile::exists(_pathDb);
}

qint64 DbManager::sizeDb()
{
    return QFile(_pathDb).size();
}

void DbManager::createTables()
{
    QSqlQuery query;
    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 ("
                          " id varchar(36) NOT NULL PRIMARY KEY"
                          " , data JSON NOT NULL"
                          " , number_num INTEGER NOT NULL"
                          " , name TEXT NOT NULL"
                          " , date_ts BIGINT NOT NULL"
                          " );").arg(DbTables::votes));
    DbWorker::instance().runQuery(query);
}

void DbManager::saveDbPetitionVote(const DbPetitionVote& dbPetitionVote)
{
    QString queryStr;
    queryStr.append("INSERT INTO "
                    + DbTables::votes
                    + " ("
                    + DbTables::votesRows
                    + " )"
                    + " VALUES (?, ?, ?, ?, ?);");
    QSqlQuery query;
    query.prepare(queryStr);
    query.addBindValue(dbPetitionVote.id);
    query.addBindValue(dbPetitionVote.toRawJson());
    query.addBindValue(dbPetitionVote.number_num);
    query.addBindValue(dbPetitionVote.name);
    query.addBindValue(dbPetitionVote.date_ts);
    DbWorker::instance().runQuery(query);
}

bool DbManager::dbPetitionVoteExist(const DbPetitionVote& dbPetitionVote)
{
    const QString queryStr = "SELECT count(*) FROM "
            + DbTables::votes
            + " WHERE json_extract(data, '$.name') = '" + dbPetitionVote.name + "'"
            + " AND json_extract(data, '$.date_str') = '" + dbPetitionVote.date_str + "'"
            + " ;";
    const QVector<QVariantList> vec = DbWorker::instance().getQueryResult(queryStr);
    if (!vec.isEmpty() && !vec.first().isEmpty()) {
        return vec.first().at(0).toInt() > 0;
    }
    return false;
}

int DbManager::getCountTotalVotes()
{
    const QString queryStr = "SELECT count(*) FROM " + DbTables::votes + ";";
    const QVector<QVariantList> vec = DbWorker::instance().getQueryResult(queryStr);
    if (!vec.isEmpty() && !vec.first().isEmpty()) {
        return vec.first().at(0).toInt();
    }
    return 0;
}

QVector<QSharedPointer<DbPetitionVote> > DbManager::findMatches(const QStringList& words)
{
    if (words.isEmpty()) {
        return {};
    }
    QString queryStr = "SELECT "
            + DbTables::votesRows
            + " FROM "
            + DbTables::votes
            + " WHERE ";
    for (const QString& word : words) {
        queryStr += " name LIKE '%" + word + "%' AND";
    }
    queryStr.chop(3);
    queryStr += " ;";
    const QVector<QVariantList> vecResVL = DbWorker::instance().getQueryResult(queryStr);
    QVector<QSharedPointer<DbPetitionVote> > vecResSP;
    for (const QVariantList& variant : vecResVL) {
        QSharedPointer<DbPetitionVote> dbPetitionVote(new DbPetitionVote("", "", ""));
        const QString jsonStr = variant.at(1).toString();
        dbPetitionVote->fromJson(jsonStr.toUtf8());
        vecResSP.append(dbPetitionVote);
    }
    return vecResSP;
}

DbManager::DbManager()
{

}

DbManager::~DbManager()
{

}
