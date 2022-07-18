#pragma once

#include <QSerializer>

class GlobalConfigLog : public QSerializer
{
    Q_GADGET
    QS_SERIALIZABLE
    QS_FIELD                (int, quantityFiles)
    QS_FIELD                (int, maxFileSizeMB)
    QS_FIELD                (QString, loggingType)
    QS_FIELD                (QString, dirToSave)
    QS_FIELD                (QStringList, loggingCategories)
public:
    GlobalConfigLog()
        : quantityFiles(3)
        , maxFileSizeMB(50)
        , loggingType("DEBUG")
        , dirToSave("Log")
        {}
};
