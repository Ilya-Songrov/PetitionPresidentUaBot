#pragma once

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>


class FileWorker
{
public:
    static bool readFileJson(QJsonDocument *doc, const QString &path);
    static QJsonDocument readFileJson(const QString &path, bool *ok = nullptr);
    static bool writeFileJson(const QJsonDocument &doc, const QString &path);

    static bool readFile(QByteArray *arr, const QString &path);
    static QByteArray readFile(const QString &path, bool *ok = nullptr);
    static bool writeFile(const QByteArray &arr, const QString &path);

    static QStringList getListFilesInDir(const QString &dirName, const bool addAbsoluteFilePath = true);
    static bool clearDirectory(const QString &dirName);
};
