#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QtConcurrent>

#include "FileWorker.h"
#include "PetitionManager.hpp"
#include "BotThreadManager.hpp"

#include "tgbot/tgbot.h"

class BotMain : public QObject
{
    Q_OBJECT

public:
    explicit BotMain(QObject *parent = nullptr);

    void startBot();

private slots:
    void slotOnCommand(const TgBot::Message::Ptr message);
    void slotOnAnyMessageWasWrite(const TgBot::Message::Ptr message);
    void slotOnCallbackQueryWasWrite(const TgBot::CallbackQuery::Ptr callbackQuery);
    void slotListResultReady(QSharedPointer<ResponseFromBot> rs);

private:
    void setSettings();
    QString getTokenFromFile();

private:
    QScopedPointer<BotThreadManager> botThreadManager;
    QScopedPointer<PetitionManager> petitionManager;
    QThread threadPetitionManager;
};

