#!/bin/bash


# get votes 1 json
curl -X GET 'https://petition.president.gov.ua/petition/146508/votes/1/json' | jq ".table_html"

# get votes 1 xml
curl -X GET 'https://petition.president.gov.ua/petition/146508/votes/99' 
	

# get votes many 
for page in $(seq 100);  do
  RES_VOTES=$(curl 'https://petition.president.gov.ua/petition/146508/votes/'$page'/json' | jq ".table_html") && echo -e $RES_VOTES
done

# get count
curl -X GET 'https://petition.president.gov.ua/petition/146508' | grep 'data-votes='
