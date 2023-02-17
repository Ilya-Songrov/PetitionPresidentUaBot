#pragma once

#include <QString>
#include <QStringView>
#include <QStringLiteral>

namespace DbTables
{
    const static QString votes                          ("votes");
    const static QString votesRows                      ("id, data, number_num, name, date_ts, petition_url");
    const static QString petition_url_for_chat_id       ("petition_url_for_chat_id");
    const static QString petition_url_for_chat_idRows   ("petition_url, chat_id");
    const static QString default_petition_url           ("default_petition_url");
    const static QString default_petition_urlRows       ("id, petition_url");
}
