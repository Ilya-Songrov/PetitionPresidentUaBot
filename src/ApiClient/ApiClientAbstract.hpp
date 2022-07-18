#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QUrlQuery>
#include <QTimer>

#include "LogManager.hpp"


class ApiClientAbstract : public QObject
{
    Q_OBJECT
public:
    explicit ApiClientAbstract(QObject *parent = nullptr);

    static QByteArray getHeadersStr(const QNetworkRequest& rq);
    static QByteArray getHeadersStr(const QNetworkReply& rs);

signals:
    void signalErrorWhileGettingResponse(const QString apiClientName);

public slots:
    void slotSSLErrorsGeneral(QNetworkReply *reply, QList<QSslError> errors);

protected:
    void sendPostRequest(const QUrl& url, const QByteArray& arr, const QLoggingCategory& category = first_category);
    void printRequest(const QNetworkRequest& request, const QByteArray& arr, const QLoggingCategory& category = first_category);
    QByteArray parseNetworkReply(QNetworkReply *reply, const QLoggingCategory& category = first_category);

protected:
    QScopedPointer<QNetworkAccessManager> _mngr;
};

