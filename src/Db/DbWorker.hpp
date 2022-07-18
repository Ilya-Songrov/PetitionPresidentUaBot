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


class DbWorker : public QObject
{
public:
    static DbWorker& instance();

    bool runQuery(const QSqlQuery& query);
    bool runQuery(const QString& strQuery);
    QVector<QVariantList> getQueryResult(const QString& strQuery);

private:
    Q_DISABLE_COPY_MOVE(DbWorker)
    DbWorker();
    ~DbWorker();
};

