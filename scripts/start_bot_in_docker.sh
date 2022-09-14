#!/bin/bash


BOT_ROOT=/bot-folder-inside-docker/DeployKit_PetitionPresidentUaBot

echo -e "\n\n\033[95mStart: $(date +"%Y-%m-%d_%H:%M:%S.%3N") \033[0m\n"

if ! pidof PetitionPresidentUaBot ; then 
	cd $BOT_ROOT && 
	./PetitionPresidentUaBot.sh
fi

echo -e "\n\n\033[95mFinish: $(date +"%Y-%m-%d_%H:%M:%S.%3N") \033[0m\n"

