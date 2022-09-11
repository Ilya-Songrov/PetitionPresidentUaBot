## Run app in docker local
```
# clone repo
git clone --single-branch --branch master <this_repo>
cd <this_repo>/docker/

# download any version of build (for exemple 1.1.0)
wget https://github.com/Ilya-Songrov/PetitionPresidentUaBot/releases/download/1.2.0/DeployKit_PetitionPresidentUaBot.zip
unzip DeployKit_PetitionPresidentUaBot.zip
rm DeployKit_PetitionPresidentUaBot.zip

# copy needed files
sudo apt install jq -y
cp start_bot_docker.sh DeployKit_PetitionPresidentUaBot/
cp ../Data/GlobalConfig.json DeployKit_PetitionPresidentUaBot/
cat DeployKit_PetitionPresidentUaBot/GlobalConfig.json | jq '.log.dirToSave = "../logs"' | sponge DeployKit_PetitionPresidentUaBot/GlobalConfig.json
cat DeployKit_PetitionPresidentUaBot/GlobalConfig.json | jq '.dbFilePath = "../db/DbPetitionPresidentUaBot.db"' | sponge DeployKit_PetitionPresidentUaBot/GlobalConfig.json
cat DeployKit_PetitionPresidentUaBot/GlobalConfig.json | jq '.tokenFilePath = "./token.txt"' | sponge DeployKit_PetitionPresidentUaBot/GlobalConfig.json
echo "<your_token>" > DeployKit_PetitionPresidentUaBot/token.txt

# build docker image
sudo docker build  . --tag petition-president-ua-bot
sudo docker image ls

# run docker on remote host
mkdir -p ~/petition-president-ua-bot/db
mkdir -p ~/petition-president-ua-bot/logs
sudo docker run 
    --volume ~/petition-president-ua-bot/db:/bot-folder-inside-docker/db \
    --volume ~/petition-president-ua-bot/logs:/bot-folder-inside-docker/logs \ 
    --restart=always --detach petition-president-ua-bot 
sudo docker ps
```

## Run app in docker on remote host
```
# copy image to remote host 
sudo docker save petition-president-ua-bot > ~/bot-image.tar
scp ~/bot-image.tar <user>@<host>:~/

# run docker on remote host
ssh <user>@<host>
sudo apt-get update -y
sudo apt install docker.io -y
sudo docker load -i ~/bot-image.tar
mkdir -p ~/petition-president-ua-bot/db
mkdir -p ~/petition-president-ua-bot/logs
sudo docker run 
    --volume ~/petition-president-ua-bot/db:/bot-folder-inside-docker/db \
    --volume ~/petition-president-ua-bot/logs:/bot-folder-inside-docker/logs \ 
    --restart=always --detach petition-president-ua-bot 
sudo docker ps
```
