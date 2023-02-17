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
    void slotOnCommand(const TgBot::Message::Ptr message, QString commandName);
    void slotOnAnyMessageWasWrite(const TgBot::Message::Ptr message);
    void slotOnCallbackQueryWasWrite(const TgBot::CallbackQuery::Ptr callbackQuery);
    void slotResponseFromBotReady(QSharedPointer<ResponseFromBot> rs);

private:
    void setSettings();
    void initDb();
    QWeakPointer<PetitionManager> getPetitionManager(const std::int64_t chat_id);

private:
    QScopedPointer<BotThreadManager> botThreadManager;
    QMap<QString, QSharedPointer<PetitionManager>> mapPetitionManagers; // <petitionUrl, Manager>
    QMap<std::int64_t, QString> mapPetitionUrlForChatId; // <chat_id, petitionUrl>
    QThread threadPetitionManager;
    const QString commandStart          = "/start";
    const QString commandHelp           = "/help";
    const QString commandSearch         = "/search";
    const QString commandCountVotes     = "/count_votes";
    const QString commandSetPetitionUrl = "/set_petition_url";
};

