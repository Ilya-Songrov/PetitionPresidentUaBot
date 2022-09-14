## Build app in docker
```
# clone project
ROOT_FOLDER=~/Downloads/deleteme/PetitionPresidentUaBot
mkdir -p $ROOT_FOLDER

# build qt in docker
cd $ROOT_FOLDER
git clone --recurse-submodules <this_repo>
cd $ROOT_FOLDER/PetitionPresidentUaBot/docker
sudo docker build . \
    --force-rm \
    --file ./DockerfileQtInDockerUbuntu20 \
    --tag qt-in-docker-ubunut20-04 


# build app
sudo docker run \
    --interactive --tty \
    --volume $ROOT_FOLDER/PetitionPresidentUaBot:/folder-inside-docker/app-folder \
    qt-in-docker-ubunut20-04 

# run command in docker
cd /folder-inside-docker/app-folder
./configure.sh
mkdir build
cd build
qmake ..
make
```

