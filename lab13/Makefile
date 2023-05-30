# Build file for CS61C MapReduce Lab
# You should not need to edit this file.

# This file requires GNU make and depends on paths on instruction machines.

####
.PHONY: clean all show

all:

generate-input:
	/home/st/hadoop-3.3.5/bin/hadoop jar textImporter/wc.jar Importer ./textFiles/billOfRights.txt seqFiles/

clean:
	rm -rf spark-wc-out*

show: clean
	/home/st/spark-2.4.5-bin-hadoop2.7/bin/spark-submit wordCount.py seqFiles/billOfRights.txt.seq
	/home/st/spark-2.4.5-bin-hadoop2.7/bin/spark-submit perWordDocumentCount.py seqFiles/billOfRights.txt.seq
	/home/st/spark-2.4.5-bin-hadoop2.7/bin/spark-submit createIndices.py seqFiles/billOfRights.txt.seq
	/home/st/spark-2.4.5-bin-hadoop2.7/bin/spark-submit mostPopular.py seqFiles/billOfRights.txt.seq