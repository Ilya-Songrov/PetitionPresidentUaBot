## Telegram Bot

![Screen Shot](md/MergedDocument.png)

## Additional Information
1. How to get bot token: open Telegram --> Search: BotFather --> /token
2. Telegram Bot API https://core.telegram.org/bots/api
3. C++14 library for Telegram bot API https://github.com/reo7sp/tgbot-cpp

## How to build (Ubuntu 20.04) (Qt 5.15.2)
```
sudo apt-get update
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install binutils
sudo apt-get install cmake
sudo apt-get install libssl-dev
sudo apt-get install libboost-system-dev
sudo apt-get install zlib1g-dev
sudo apt-get install curl
sudo apt-get install libcurl4-openssl-dev

git clone <this_repo>
cd <this_repo>
git checkout master
git pull
./configure
mkdir build
cd build
qmake ..
make
```
