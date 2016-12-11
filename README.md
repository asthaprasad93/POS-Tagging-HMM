# POS-Tagging-HMM
Implements the Forward, Backward and Viterbi algorithms to detect parts of speech.

## Viterbi Algorithm
Input: Text file where each line is a sentence (tokens separated by spaces)
Output: Same as input with a classifier appended to each token

To run, enter:

`g++ viterbi.cpp -o viterbi -std=c++11`

`./viterbi dev.txt`


## Forward and Backward Algorithms
Input: Text file where each line is a sentence (tokens separated by spaces)
Output: Marginal probability of each sentence

To run the Backward algorithm, enter:

`g++ backward.cpp -o backward -std=c++11`

`./backward dev.txt`

To run the Forward algorithm, enter:

`g++ forward.cpp -o forward -std=c++11`

`./forward dev.txt`


## Other files
The training files include: 

hmm-trans.txt: Transision probabilities
hmm-emit.txt: Emission probabilities
hmm-pripr.txt: Prior probabilities


