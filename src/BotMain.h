#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QtConcurrent>

#include "FileWorker.hpp"
#include "PetitionManager.hpp"
#include "BotThreadManager.hpp"
#include "DbManager.hpp"
#include "GlobalConfigInstance.hpp"

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
    void initDb();

private:
    QScopedPointer<BotThreadManager> botThreadManager;
    QScopedPointer<PetitionManager> petitionManager;
    QThread threadPetitionManager;
};

