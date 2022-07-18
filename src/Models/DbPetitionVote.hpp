#pragma once

#include <QSerializer>

class DbPetitionVote : public QSerializer
{
    Q_GADGET
    QS_SERIALIZABLE
    QS_FIELD                (QString, id);
    QS_FIELD                (QString, number_str)
    QS_FIELD                (int, number_num)
    QS_FIELD                (QString, name)
    QS_FIELD                (QString, date_str)
    QS_FIELD                (QDate, date_type)
    QS_FIELD                (qint64, date_ts)
    QS_FIELD                (qint64, create_ts)
    QS_FIELD                (qint64, update_ts)

public:
    DbPetitionVote(const QString& a_number,
                      const QString& a_name,
                      const QString& a_date)
        : id(QUuid::createUuid().toString(QUuid::WithoutBraces))
        , create_ts(QDateTime::currentMSecsSinceEpoch())
        , update_ts(QDateTime::currentMSecsSinceEpoch())
    {
        number_str  = a_number;
        number_num  = extractOnlyNumbers(a_number);
        name        = a_name;
        date_str    = a_date;
        date_type   = getDateFromUkString(a_date);
        date_ts     = QDateTime(date_type, QTime(0,0,0,0)).toMSecsSinceEpoch();
    }

private:
    qint64 extractOnlyNumbers(const QString& str)
    {
        QString res;
        for (const QChar ch : str) {
            if (ch.isDigit()) {
                res += ch;
            }
        }
        if (res.isEmpty()) {
            res = "-1";
        }
        return res.toInt();
    }
    QDate getDateFromUkString(const QString& dateStrUk)
    {
        // 17 липня 2022
        QString dateStrCopy = dateStrUk;
        static const QLocale localeUk = QLocale(QLocale::Ukrainian, QLocale::Ukraine);
        static QStringList monthsUk;
        if (monthsUk.isEmpty()) {
            for (int var = 0; var < 12; ++var) {
                monthsUk << localeUk.monthName(var, QLocale::LongFormat);
            }
        }
        for (int var = 0; var < monthsUk.size(); ++var) {
            const QString& mu = monthsUk.at(var);
            if (monthsUk.contains(mu)) {
                dateStrCopy.replace(mu, QLocale(QLocale::English).monthName(var, QLocale::LongFormat));
            }
        }
        return QDate::fromString(dateStrCopy, "dd MMMM yyyy");
    }
};
