/**************************************************************************
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/.
**
**************************************************************************/

#pragma once

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlResult>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>


class DbAccessor : public QObject
{
public:
    static DbAccessor& instance();

    bool runQueryWithoutResult(QSqlQuery& query);
    bool runQueryWithoutResult(const QString& strQuery);
    QVector<QVariantList> runQueryWithResult(QSqlQuery& query);
    QVector<QVariantList> runQueryWithResult(const QString& strQuery);

private:
    QVector<QVariantList> parseSqlQuery(QSqlQuery& query);

private:
    Q_DISABLE_COPY_MOVE(DbAccessor)
    DbAccessor();
    ~DbAccessor();
};

