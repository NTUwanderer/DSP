#!/bin/bash

source set_htk_path.sh

config=lib/pretrain_config.cfg
macro=hmm/macros
model=hmm/models

test_data_list=scripts/training.scp
dictionary=lib/dict

out_mlf=result/train.mlf

model_list=lib/models_sp.lst
word_net=lib/wdnet_sp

HVite -D -H $macro -H $model -S $test_data_list -C $config -w $word_net \
	-l '*' -i $out_mlf -p 0.0 -s 0.0 $dictionary $model_list
	
