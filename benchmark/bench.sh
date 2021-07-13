#!/bin/bash
    
cd $(dirname $0)

export div=1
export DATASET="enwiki-1e5_en_5_gram"
export ARPA="dataset/${DATASET}/${DATASET}.arpa"
BINS=(
pbin
bin
nbin
n1bin
)
#ncnvbin

function run {
    OUT=product/${DATASET}/reverse
    if [[ -d ${OUT} ]]; then
        rm -rf ${OUT}
    fi
    $1/build_dalm -m reverse -f ${ARPA} -o ${OUT} -d ${div}
}
export -f run

parallel -j 1 run ::: ${BINS[@]}

