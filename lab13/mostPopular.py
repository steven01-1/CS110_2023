import sys
import re

from pyspark import SparkContext,SparkConf

def splitDocument(document):
    """Returns a list of all words in the document"""
    return list(filter(lambda x: x.lower() not in ["a", "an", "the"], re.findall(r"\w+", document[1])))

def toPairs(word):
    """ Creates `(key, value)` pairs where the word is the key and 1 is the value """
    return (word, 1)

def sumCounts(a, b):
    """ Add up the values for each word, resulting in a count of occurences """
    return a + b

""" TODO: Add functions here to determine the most popular word
    Note that Map/flatMap style functions take in one argument while Reduce functions take in two
"""
def swap(arg):
    return (arg[1], arg[0])

def mostPopular(file_name, output="spark-wc-out-mostPopular"):
    sc = SparkContext("local[8]", "WordCount", conf=SparkConf().set("spark.hadoop.validateOutputSpecs", "false"))
    """ Reads in a sequence file FILE_NAME to be manipulated """
    file = sc.sequenceFile(file_name)

    counts = file.flatMap(splitDocument) \
                 .map(toPairs) \
                 .reduceByKey(sumCounts) \
                 .map(swap) \
                 .sortByKey(False) \
                 .map(swap)
                 # TODO: add appropriate extra transformations here

    """ Takes the dataset stored in counts and writes everything out to OUTPUT """
    counts.coalesce(1).saveAsTextFile(output)

""" Do not worry about this """
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 2:
        mostPopular(argv[1])
    else:
        mostPopular(argv[1], argv[2])

# /home/st/spark-2.4.5-bin-hadoop2.7/bin/spark-submit mostPopular.py seqFiles/billOfRights.txt.seq
