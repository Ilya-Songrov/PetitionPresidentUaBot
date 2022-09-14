## Run app in docker local host
```
# clone repo
git clone --single-branch --branch master <this_repo>
cd <this_repo>/scripts/
DOCKER_FOLDER_ROOT=$PWD

# download any version of build (for exemple 1.1.0)
cd $DOCKER_FOLDER_ROOT
wget https://github.com/Ilya-Songrov/PetitionPresidentUaBot/releases/download/1.2.0/DeployKit_PetitionPresidentUaBot.zip
unzip DeployKit_PetitionPresidentUaBot.zip
rm DeployKit_PetitionPresidentUaBot.zip

# copy needed files
sudo apt install jq -y
cd $DOCKER_FOLDER_ROOT
DEPLOY_KIT_ROOT=$DOCKER_FOLDER_ROOT/DeployKit_PetitionPresidentUaBot
cp start_bot_docker.sh $DEPLOY_KIT_ROOT/
cp ../Data/GlobalConfig.json $DEPLOY_KIT_ROOT/
echo "$(jq '.log.dirToSave = "../logs"' $DEPLOY_KIT_ROOT/GlobalConfig.json)" > $DEPLOY_KIT_ROOT/GlobalConfig.json
echo "$(jq '.dbFilePath = "../db/DbPetitionPresidentUaBot.db"' $DEPLOY_KIT_ROOT/GlobalConfig.json)" > $DEPLOY_KIT_ROOT/GlobalConfig.json
echo "$(jq '.tokenFilePath = "./token.txt"' $DEPLOY_KIT_ROOT/GlobalConfig.json)" > $DEPLOY_KIT_ROOT/GlobalConfig.json
echo "<your_token>" > $DEPLOY_KIT_ROOT/token.txt

# build docker image
cd $$DOCKER_FOLDER_ROOT
sudo docker build  . --file DockerfileRun --tag petition-president-ua-bot
sudo docker image ls

# run docker on local host
mkdir -p ~/petition-president-ua-bot/db
mkdir -p ~/petition-president-ua-bot/logs
sudo docker run \
    --volume ~/petition-president-ua-bot/db:/bot-folder-inside-docker/db \
    --volume ~/petition-president-ua-bot/logs:/bot-folder-inside-docker/logs \
    --restart=always \
    --detach petition-president-ua-bot
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
sudo docker run \
    --volume ~/petition-president-ua-bot/db:/bot-folder-inside-docker/db \
    --volume ~/petition-president-ua-bot/logs:/bot-folder-inside-docker/logs \
    --restart=always \
    --detach petition-president-ua-bot
sudo docker ps
```
