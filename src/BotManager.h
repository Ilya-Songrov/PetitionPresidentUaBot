#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QtConcurrent>

#include "FileWorker.h"
#include "PetitionManager.hpp"
#include "SignalEmitterManager.hpp"

#include "tgbot/tgbot.h"

using namespace TgBot;

class BotManager : public QObject
{
    Q_OBJECT

public:
    explicit BotManager(QObject *parent = nullptr);

    void startBot();

private slots:
    void slotOnCommand(const Message::Ptr message);
    void slotOnAnyMessageWasWrite(const Message::Ptr message);
    void slotOnCallbackQueryWasWrite(const CallbackQuery::Ptr callbackQuery);
    void slotListResultReady(QSharedPointer<ResponseFromBot> rs);

private:
    void setSettings();
    QString getTokenFromFile();

private:
    QScopedPointer<Bot> bot;
    QScopedPointer<PetitionManager> petitionManager;
    QScopedPointer<SignalEmitterManager> signalEmitterManager;
    QThread threadPetitionManager;
};

