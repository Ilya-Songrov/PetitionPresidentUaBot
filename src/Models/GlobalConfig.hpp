#pragma once

#include <QSerializer>

#include "GlobalConfigLog.hpp"

class GlobalConfig : public QSerializer
{
    Q_GADGET
    QS_SERIALIZABLE
    QS_OBJECT               (GlobalConfigLog, log)
    QS_FIELD                (QString, dbFilePath)
    QS_FIELD                (QString, tokenFilePath)
};
