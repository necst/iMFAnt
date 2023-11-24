#!/usr/bin/bash

docker build -t docker-artifact -f docker/Dockerfile.x86 .

docker run -ti -v ${PWD}/plots:/iMFAnt/plots docker-artifact
