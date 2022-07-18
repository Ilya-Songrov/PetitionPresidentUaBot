#pragma once

#include <QSerializer>

#include "GlobalConfig.hpp"
#include "FileWorker.hpp"

class GlobalConfigInstance
{
public:
    static GlobalConfigInstance& instance()
    {
        static GlobalConfigInstance gc;
        return gc;
    }
    static const GlobalConfig& getGlobalConfig()
    {
        return _gc;
    }
    static void loadConfig(const QString& configFilePath)
    {
        const QByteArray arr = FileWorker::readFile(configFilePath);
        _gc.fromJson(arr);
    }
    static QString getTokenFromFile()
    {
        const QString token = FileWorker::readFile(_gc.tokenFilePath).trimmed();
        if (token.isEmpty()) {
            qCritical() << "Token is empty" << Qt::endl;
        }
        return token;
    }

private:
    Q_DISABLE_COPY_MOVE(GlobalConfigInstance)
    GlobalConfigInstance() {}
    ~GlobalConfigInstance() {}

private:
    inline static GlobalConfig _gc;
};

