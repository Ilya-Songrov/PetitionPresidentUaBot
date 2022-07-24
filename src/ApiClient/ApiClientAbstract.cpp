#include "ApiClientAbstract.hpp"

ApiClientAbstract::ApiClientAbstract(QObject *parent)
    : QObject{parent}
    , _mngr(new QNetworkAccessManager())
    , timeoutMsec(300000)
{

}

QByteArray ApiClientAbstract::getHeadersStr(const QNetworkRequest& rq)
{
    QByteArray arr;
    QList<QByteArray> headerList = rq.rawHeaderList();
    for(const QByteArray& header : headerList) {
        arr += header + ": " + rq.rawHeader(header) + "; ";
    }
    if (!arr.isEmpty()) {
        arr.chop(1);
    }
    return arr;
}

QByteArray ApiClientAbstract::getHeadersStr(const QNetworkReply& rs)
{
    QByteArray arr;
    QList<QByteArray> headerList = rs.rawHeaderList();
    for(const QByteArray& header : headerList) {
        arr += header + ": " + rs.rawHeader(header) + "; ";
    }
    if (!arr.isEmpty()) {
        arr.chop(1);
    }
    return arr;
}

void ApiClientAbstract::slotSSLErrorsGeneral(QNetworkReply* reply, QList<QSslError> errors)
{
    qCDebug(second_category) << QString("error server reply (%1)").arg(reply->url().toString()) << errors << reply->error() << Qt::endl;
    reply->ignoreSslErrors(errors);
}

void ApiClientAbstract::sendPostRequest(const QUrl& url, const QByteArray& arr, const QLoggingCategory& category)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    printRequest(request, arr, category);
    _mngr->post(request, arr);
}

void ApiClientAbstract::printRequest(const QNetworkRequest& request, const QByteArray& arr, const QLoggingCategory& category)
{
    qCDebug(category)
            << "\033[1mRQ_URL:\033[0m" << request.url().toString() << Qt::endl
            << "\033[1mRQ_HEADERS:\033[0m" << getHeadersStr(request) << Qt::endl
            << "\033[1mRQ_BODY:\033[0m" << qPrintable(arr) << Qt::endl
               ;
}

QByteArray ApiClientAbstract::parseNetworkReply(QNetworkReply* reply, const QLoggingCategory& category)
{
    reply->waitForReadyRead(timeoutMsec);
    const QNetworkReply::NetworkError networkError = reply->error();
    const QUrl urlReply = reply->url();
    const QByteArray arrReply = reply->readAll();
    qCDebug(category) << "\033[1mRS_URL:\033[0m" << urlReply.toString() << Qt::endl
                            << "\033[1mRS_HEADERS:\033[0m" << getHeadersStr(*reply) << Qt::endl
                            << "\033[1mRS_BODY:\033[0m" << qPrintable(arrReply) << Qt::endl
                            << "\033[1mRS_NETWORK_ERROR:\033[0m" << reply->error() << (networkError != QNetworkReply::NoError ? reply->errorString() : "") << Qt::endl
                            ;
    reply->deleteLater();
    return arrReply;

}
