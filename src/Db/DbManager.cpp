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
    DbAccessor::instance().runQueryWithoutResult(QString("CREATE TABLE IF NOT EXISTS %1 ("
                                                         " id varchar(36) NOT NULL PRIMARY KEY"
                                                         " , data JSON NOT NULL"
                                                         " , number_num INTEGER NOT NULL"
                                                         " , name TEXT NOT NULL"
                                                         " , date_ts BIGINT NOT NULL"
                                                         " , petition_url TEXT NOT NULL"
                                                         " );").arg(DbTables::votes));
    DbAccessor::instance().runQueryWithoutResult(QString("CREATE TABLE IF NOT EXISTS %1 ("
                                                         " petition_url TEXT NOT NULL"
                                                         " , chat_id BIGINT NOT NULL PRIMARY KEY"
                                                         " );").arg(DbTables::petition_url_for_chat_id));
    DbAccessor::instance().runQueryWithoutResult(QString("CREATE TABLE IF NOT EXISTS %1 ("
                                                         " id varchar(36) NOT NULL PRIMARY KEY"
                                                         " , petition_url TEXT NOT NULL"
                                                         " );").arg(DbTables::default_petition_url));
}

void DbManager::insertOrUpdatePetitionUrlForChatId(const std::string &petitionUrl, const int64_t chat_id)
{
    QString queryStr;
    queryStr.append("INSERT INTO "
                    + DbTables::petition_url_for_chat_id
                    + " ("
                    + DbTables::petition_url_for_chat_idRows
                    + " )"
                    + " VALUES (?, ?)"
                    + " ON CONFLICT (chat_id) DO"
                    + " UPDATE SET petition_url=EXCLUDED.petition_url;");
    QSqlQuery query;
    query.prepare(queryStr);
    query.addBindValue(QString::fromStdString(petitionUrl));
    query.addBindValue(qint64(chat_id));
    DbAccessor::instance().runQueryWithoutResult(query);
}

QString DbManager::getPetitionUrlForChatId(const int64_t chat_id)
{
    QString queryStr = "SELECT "
            + DbTables::petition_url_for_chat_idRows
            + " FROM "
            + DbTables::petition_url_for_chat_id
            + " WHERE chat_id = '"
            + QString::number(chat_id)
            + "';";
    const QVector<QVariantList> vec = DbAccessor::instance().runQueryWithResult(queryStr);
    if (!vec.isEmpty() && !vec.first().isEmpty()) {
        return vec.first().at(0).toString();
    }
    return getDefaultPetitionUrl();
}

void DbManager::insertOrReplaceDefaultPetitionUrl(const std::string &petitionUrl)
{
    QString queryStr;
    queryStr.append("INSERT OR REPLACE INTO "
                    + DbTables::default_petition_url
                    + " ("
                    + DbTables::default_petition_urlRows
                    + " )"
                    + " VALUES (?, ?);");
    QSqlQuery query;
    query.prepare(queryStr);
    query.addBindValue("00000000-0000-0000-0000-000000000000");
    query.addBindValue(QString::fromStdString(petitionUrl));
    DbAccessor::instance().runQueryWithoutResult(query);
}

QString DbManager::getDefaultPetitionUrl()
{
    QString queryStr = "SELECT "
            + QString(" petition_url")
            + " FROM "
            + DbTables::default_petition_url
            + " WHERE id = '00000000-0000-0000-0000-000000000000';";
    const QVector<QVariantList> vec = DbAccessor::instance().runQueryWithResult(queryStr);
    if (!vec.isEmpty() && !vec.first().isEmpty()) {
        return vec.first().at(0).toString();
    }
    return "https://petition.president.gov.ua/petition/146508";
}

void DbManager::saveDbPetitionVote(const DbPetitionVote& dbPetitionVote, const QString &petition_url)
{
    QString queryStr;
    queryStr.append("INSERT INTO "
                    + DbTables::votes
                    + " ("
                    + DbTables::votesRows
                    + " )"
                    + " VALUES (?, ?, ?, ?, ?, ?);");
    QSqlQuery query;
    query.prepare(queryStr);
    query.addBindValue(dbPetitionVote.id);
    query.addBindValue(dbPetitionVote.toRawJson());
    query.addBindValue(dbPetitionVote.number_num);
    query.addBindValue(dbPetitionVote.name);
    query.addBindValue(dbPetitionVote.date_ts);
    query.addBindValue(petition_url);
    DbAccessor::instance().runQueryWithoutResult(query);
}

bool DbManager::dbPetitionVoteExist(const DbPetitionVote& dbPetitionVote, const QString &petition_url)
{
    QSqlQuery query;
    query.prepare("SELECT count(*) FROM "
                  + DbTables::votes
                  + " WHERE json_extract(data, '$.name') = :name"
                  + " AND json_extract(data, '$.date_str') = :date_str"
                  + " AND petition_url = :petition_url"
                  + " ;");
    query.bindValue(":name", dbPetitionVote.name);
    query.bindValue(":date_str", dbPetitionVote.date_str);
    query.bindValue(":petition_url", petition_url);
    const QVector<QVariantList> vec = DbAccessor::instance().runQueryWithResult(query);
    if (!vec.isEmpty() && !vec.first().isEmpty()) {
        return vec.first().at(0).toInt() > 0;
    }
    return false;
}

int DbManager::getCountTotalVotes(const QString &petition_url)
{
    QSqlQuery query;
    query.prepare("SELECT count(*) FROM "
                  + DbTables::votes
                  + " WHERE petition_url = :petition_url"
                  + " ;");
    query.bindValue(":petition_url", petition_url);
    const QVector<QVariantList> vec = DbAccessor::instance().runQueryWithResult(query);
    if (!vec.isEmpty() && !vec.first().isEmpty()) {
        return vec.first().at(0).toInt();
    }
    return 0;
}

QVector<QSharedPointer<DbPetitionVote> > DbManager::findMatches(const QStringList& words, const QString &petition_url)
{
    if (words.isEmpty()) {
        return {};
    }
    QString queryStr = "SELECT "
            + DbTables::votesRows
            + " FROM "
            + DbTables::votes
            + " WHERE "
            + " petition_url = '"
            + petition_url
            + "'";
    for (const QString& word : words) {
        queryStr += " AND name LIKE '%" + word + "%'";
    }
    queryStr += " ;";
    const QVector<QVariantList> vecResVL = DbAccessor::instance().runQueryWithResult(queryStr);
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
