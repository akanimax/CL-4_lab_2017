#!/bin/bash
mpic++ $1
HOSTS="theslave1 theslave2"
for HOSTNAME in ${HOSTS} ; do
    scp a.out pict@${HOSTNAME}:/home/pict/AK_CL4/A1
done
