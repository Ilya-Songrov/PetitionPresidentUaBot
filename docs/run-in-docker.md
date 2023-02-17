## Run app in docker local host
```
# variables
ROOT_FOLDER=~/Downloads/deleteme/PetitionPresidentUaBot
mkdir -p $ROOT_FOLDER

# clone repo
cd $ROOT_FOLDER
git clone --single-branch --branch master https://github.com/Ilya-Songrov/PetitionPresidentUaBot.git
cd $ROOT_FOLDER/PetitionPresidentUaBot/scripts/
SCRIPT_FOLDER_ROOT=$PWD

# download any version of build (for exemple 2.0.0)
cd $SCRIPT_FOLDER_ROOT
wget https://github.com/Ilya-Songrov/PetitionPresidentUaBot/releases/download/2.0.0/DeployKit_PetitionPresidentUaBot.zip
unzip DeployKit_PetitionPresidentUaBot.zip
rm DeployKit_PetitionPresidentUaBot.zip

# copy needed files
sudo apt install jq -y
cd $SCRIPT_FOLDER_ROOT
DEPLOY_KIT_ROOT=$SCRIPT_FOLDER_ROOT/DeployKit_PetitionPresidentUaBot
cp start_bot_in_docker.sh $DEPLOY_KIT_ROOT/
cp ../Data/GlobalConfig.json $DEPLOY_KIT_ROOT/
echo "$(jq '.log.dirToSave = "../logs"' $DEPLOY_KIT_ROOT/GlobalConfig.json)" > $DEPLOY_KIT_ROOT/GlobalConfig.json
echo "$(jq '.dbFilePath = "../db/DbPetitionPresidentUaBot.db"' $DEPLOY_KIT_ROOT/GlobalConfig.json)" > $DEPLOY_KIT_ROOT/GlobalConfig.json
echo "$(jq '.tokenFilePath = "./token.txt"' $DEPLOY_KIT_ROOT/GlobalConfig.json)" > $DEPLOY_KIT_ROOT/GlobalConfig.json
echo '<your_token>' > $DEPLOY_KIT_ROOT/token.txt

# build docker image
cd $SCRIPT_FOLDER_ROOT
sudo docker build  . \
    --no-cache \
    --force-rm \
    --file DockerfileRun \
    --tag petition-president-ua-bot:2.0.0
sudo docker image ls

# run docker on local host
mkdir -p $ROOT_FOLDER/petition-president-ua-bot/db
mkdir -p $ROOT_FOLDER/petition-president-ua-bot/logs
# sudo docker ps # show all running dockers
# sudo docker stop <container-id> # stop old version
sudo docker run --detach \
    --volume $ROOT_FOLDER/petition-president-ua-bot/db:/bot-folder-inside-docker/db \
    --volume $ROOT_FOLDER/petition-president-ua-bot/logs:/bot-folder-inside-docker/logs \
    --restart=always \
    petition-president-ua-bot:2.0.0
sudo docker ps
# sudo docker ps --latest # show the latest created container (includes all states)
# sudo docker rm -f <container_id> # to remove other running instances
```

## Run app in docker on remote host
```
# copy image to remote host 
sudo docker save petition-president-ua-bot:2.0.0 > $ROOT_FOLDER/bot-image.tar
scp $ROOT_FOLDER/bot-image.tar <user>@<host>:~/

# run docker on remote host
ssh <user>@<host>
sudo apt-get update -y
sudo apt install docker.io -y
sudo docker load -i ~/bot-image.tar
mkdir -p ~/petition-president-ua-bot/db
mkdir -p ~/petition-president-ua-bot/logs
sudo docker run --detach \
    --volume ~/petition-president-ua-bot/db:/bot-folder-inside-docker/db \
    --volume ~/petition-president-ua-bot/logs:/bot-folder-inside-docker/logs \
    --restart=always \
    petition-president-ua-bot:2.0.0
sudo docker ps
```
