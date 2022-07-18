#include "DbWorker.hpp"

#include "LogManager.hpp"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

DbWorker& DbWorker::instance()
{
    static DbWorker db;
    return db;
}

bool DbWorker::runQuery(const QSqlQuery& query)
{

    qCDebug(db_category) << "Query:" << query.executedQuery()
                         << Qt::endl
                         << "BoundValues:" << query.boundValues()
                         << Qt::endl;
    if(!const_cast<QSqlQuery&>(query).exec()){ // TODO: delete const_cast<>
        qCWarning(db_category) << "failed: " << query.lastError() << Qt::endl;
        return false;
    }
    return true;
}

bool DbWorker::runQuery(const QString& strQuery)
{
    QSqlQuery q(strQuery);
    return runQuery(q);
}

QVector<QVariantList> DbWorker::getQueryResult(const QString& strQuery)
{
    qCDebug(db_category) << "Query:" << strQuery << Qt::endl;
    QSqlQuery query(strQuery);
    if(!query.exec()){
        qCWarning(db_category) << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const int count = query.record().count();
    QVector<QVariantList> vec;
    while (query.next()) {
        QVariantList list;
        for (int var = 0; var < count; ++var) {
            list.append(query.value(var));
        }
        vec.append(list);
    }
    return vec;
}

DbWorker::DbWorker(){}
DbWorker::~DbWorker()
{
}
