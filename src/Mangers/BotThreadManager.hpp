#pragma once

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "tgbot/tgbot.h"

class BotThreadManager : public QThread
{
    Q_OBJECT
public:
    explicit BotThreadManager(const QString& token, QObject *parent = nullptr);
    ~BotThreadManager();

    TgBot::Bot& bot();

signals:
    void signalOnCommand(const TgBot::Message::Ptr message, QString commandName = QString());
    void signalOnAnyMessage(const TgBot::Message::Ptr message);
    void signalOnCallbackQuery(const TgBot::CallbackQuery::Ptr callbackQuery);

protected:
    void run() override;

private slots:
    void slotOnCommand(const TgBot::Message::Ptr message, QString commandName);
    void slotOnAnyMessage(const TgBot::Message::Ptr message);
    void slotOnCallbackQuery(const TgBot::CallbackQuery::Ptr callbackQuery);

private:
    TgBot::Bot _bot;
    QMutex mutex;
    QWaitCondition condition;
};
