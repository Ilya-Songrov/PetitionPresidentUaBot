## Telegram Bot

![Screen Shot](md/MergedDocument.png)

## How to build on pc
```
git clone --recurse-submodules https://github.com/Ilya-Songrov/PetitionPresidentUaBot.git
cd PetitionPresidentUaBot
./configure.sh
mkdir build && cd build
qmake ..
make 
```

## How to build and run in docker
- Read to build: [build-in-docker](/docs/build-in-docker.md)
- Read to build: [run-in-docker](/docs/run-in-docker.md)

## Additional Information
1. How to get bot token: open Telegram --> Search: BotFather --> /token
2. Telegram Bot API https://core.telegram.org/bots/api
3. C++14 library for Telegram bot API https://github.com/reo7sp/tgbot-cpp

