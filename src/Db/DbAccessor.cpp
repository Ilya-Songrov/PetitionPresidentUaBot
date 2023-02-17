#include "DbAccessor.hpp"

#include "LogManager.hpp"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

DbAccessor& DbAccessor::instance()
{
    static DbAccessor db;
    return db;
}

bool DbAccessor::runQueryWithoutResult(QSqlQuery& query)
{
    if(!query.exec()){
        qCDebug(db_category) << "SQL_QUERY:" << query.executedQuery()
                             << Qt::endl
                             << "SQL_BOUND_VALUES:" << query.boundValues()
                             << Qt::endl
                             << "SQL_FAILED: " << query.lastError()
                             << Qt::endl;        return false;
    }
    parseSqlQuery(query);
    return true;
}

bool DbAccessor::runQueryWithoutResult(const QString& strQuery)
{
    QSqlQuery q(strQuery);
    return runQueryWithoutResult(q);
}

QVector<QVariantList> DbAccessor::runQueryWithResult(QSqlQuery &query)
{
    if(!query.exec()){
        qCDebug(db_category) << "SQL_QUERY:" << query.executedQuery()
                             << Qt::endl
                             << "SQL_BOUND_VALUES:" << query.boundValues()
                             << Qt::endl
                             << "SQL_FAILED: " << query.lastError()
                             << Qt::endl;
        return {};
    }
    return parseSqlQuery(query);
}

QVector<QVariantList> DbAccessor::runQueryWithResult(const QString& strQuery)
{
    QSqlQuery query(strQuery);
    return runQueryWithResult(query);
}

QVector<QVariantList> DbAccessor::parseSqlQuery(QSqlQuery &query)
{
    const int count = query.record().count();
    QVector<QVariantList> vec;
    while (query.next()) {
        QVariantList list;
        for (int var = 0; var < count; ++var) {
            list.append(query.value(var));
        }
        vec.append(list);
    }
    qCDebug(db_category) << "SQL_QUERY:" << query.executedQuery()
                         << Qt::endl
                         << "SQL_BOUND_VALUES:" << query.boundValues()
                         << Qt::endl
                         << "SQL_RESULT:" << vec.mid(0,10)
                         << Qt::endl;
    return vec;
}

DbAccessor::DbAccessor(){}
DbAccessor::~DbAccessor()
{
}
