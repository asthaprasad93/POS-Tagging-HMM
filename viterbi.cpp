#include "HMM.h"

int main(int argc, char* argv[])
{
	ifstream dev_file;
	ifstream hmmTrans_file;
	ifstream hmmEmit_file;
	ifstream hmmPrior_file;

	if (argc > 4)
	{
		dev_file.open(argv[1]);
		hmmTrans_file.open(argv[2]);
		hmmEmit_file.open(argv[3]);
		hmmPrior_file.open(argv[4]);
		
	}

	//Parse dev file
	vvS dev = ExtractDev(dev_file);

	//Parse transition matrix
	vvD trans = ExtractTrans(hmmTrans_file);

	//Parse emit matrix
	mvD emit = ExtractEmit(hmmEmit_file);

	//Parse prior
	vD prior = ExtractPrior(hmmPrior_file);

	//VITERBI
	for (int d = 0; d < dev.size(); d++)
	{
		vS sentence = dev[d];
		Viterbi(sentence, trans, emit, prior);
	}

}
