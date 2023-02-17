## Build app in docker
```
# clone project
ROOT_FOLDER=~/Downloads/deleteme/PetitionPresidentUaBot
mkdir -p $ROOT_FOLDER

# build qt in docker
cd $ROOT_FOLDER
git clone --recurse-submodules https://github.com/Ilya-Songrov/PetitionPresidentUaBot.git
cd $ROOT_FOLDER/PetitionPresidentUaBot/scripts
sudo docker build . \
    --no-cache \
    --force-rm \
    --file ./DockerfileQtInDockerUbuntu20 \
    --tag qt-in-docker-ubunut20-04 


# build app
cd $ROOT_FOLDER
sudo docker run \
    --interactive --tty \
    --volume $ROOT_FOLDER/PetitionPresidentUaBot:/folder-inside-docker/app-folder \
    qt-in-docker-ubunut20-04 \
        -c 'cd /folder-inside-docker/app-folder && \
        ./configure.sh && \
        mkdir build && \
        cd build && \
        qmake .. && \
        make && \
        exit'
```

