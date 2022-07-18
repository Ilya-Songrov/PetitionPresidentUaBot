#include "FileWorker.h"


bool FileWorker::readFileJson(QJsonDocument *doc, const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        const QString error = QString("Error QIODevice::ReadOnly | QIODevice::Text (%1) (%2)").arg(file.errorString(), file.fileName());
        qWarning() << error << Qt::endl;
        return false;
    }
    const QByteArray arr = file.readAll();
    QJsonParseError errorJson;
    *doc = QJsonDocument::fromJson(arr, &errorJson);
    if (errorJson.error != QJsonParseError::NoError) {
        const QString error = "QJsonParseError: " + errorJson.errorString() + " arr: " + arr + "fileName:" + file.fileName();
        qWarning() << error << Qt::endl;
        return false;
    }
    return true;
}

QJsonDocument FileWorker::readFileJson(const QString &path, bool *ok)
{
    QJsonDocument doc;
    const bool ret = readFileJson(&doc, path);
    if (ok) {
        *ok = ret;
    }
    return doc;
}

bool FileWorker::writeFileJson(const QJsonDocument &doc, const QString &path)
{
    return writeFile(doc.toJson(), path);
}

bool FileWorker::readFile(QByteArray *arr, const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)){
        const QString error = QString("Error QIODevice::ReadOnly (%1) (%2)").arg(file.errorString(), file.fileName());
        qWarning() << error << Qt::endl;
        return false;
    }
    *arr = file.readAll();
    return true;
}

QByteArray FileWorker::readFile(const QString &path, bool *ok)
{
    QByteArray arr;
    const bool ret = readFile(&arr, path);
    if (ok) {
        *ok = ret;
    }
    return arr;
}

bool FileWorker::writeFile(const QByteArray &arr, const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)){
        const QString error = QString("Error QFile::WriteOnly | QFile::Truncate (%1) (%2)").arg(file.errorString(), file.fileName());
        qWarning() << error << Qt::endl;
        return false;
    }
    file.write(arr);
    return true;
}


QStringList FileWorker::getListFilesInDir(const QString &dirName, const bool addAbsoluteFilePath)
{
    QStringList list;
    QDir directory(QFileInfo(dirName).absoluteFilePath());
    const auto infoList = directory.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for(const QFileInfo &info: infoList) {

        if(!info.isDir()){
            list.append(addAbsoluteFilePath ? info.absoluteFilePath() : info.fileName());
        }
    }
    return list;
}

bool FileWorker::clearDirectory(const QString &dirName)
{
    bool result = true;
    const QString absoluteDirName = QFileInfo(dirName).absoluteFilePath();
    QDir dir(absoluteDirName);

    if (dir.exists(absoluteDirName)) {
        const auto infoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
        for (const QFileInfo &info : infoList) {

            if(QFileInfo(info.absoluteFilePath()).isFile()){
                const bool ret = QFile::remove(info.absoluteFilePath());
                if (!result) {
                    qDebug() << "File not removed: " <<  info.absoluteFilePath() << Qt::endl;
                    result = ret;
                }
            }
            else{
                clearDirectory(info.absoluteFilePath());
            }
        }
    }
    return result;
}
