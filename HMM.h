//Headerfiles
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <queue>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <limits>
#include "logsum.hpp"

using namespace std;

typedef vector<string> vS;
typedef vector<vS> vvS;
typedef vector<double> vD;
typedef vector <vD> vvD;
typedef map <string, vD> mvD;

string GetTag(int ind)
{
	switch (ind)
	{
		case 0: return "_PR";
		case 1: return "_VB";
		case 2: return "_RB";
		case 3: return "_NN";
		case 4: return "_PC";
		case 5: return "_JJ";
		case 6: return "_DT";
		case 7: return "_OT";
		default: cout << "No match" << endl;
				return "_dunno";

	}
}

vvS ExtractDev(ifstream &dev_file)
{
	vvS dev;
	string line;
	size_t pos;
	string word;

	while(getline(dev_file, line))
	{
		vS TempLine;

		while(line.length() != 0 && line.find(' ') != string::npos)
		{
			pos = line.find_first_of(' ');
			word = line.substr(0, pos);
			transform(word.begin(), word.end(), word.begin(), ::tolower);
			TempLine.push_back(word);
			line.erase(0, pos+1);
		}

		// Transform and push last word in line
		transform(line.begin(), line.end(), line.begin(), ::tolower);
		TempLine.push_back(line);
		dev.push_back(TempLine);
		TempLine.clear();
	}

	return dev;
}

vvD ExtractTrans(ifstream &hmmTrans_file)
{
	vvD trans;
	string row;
	size_t pos;

	while(getline(hmmTrans_file, row))
	{
		vD TempLine;
		pos = row.find_first_of(' ');
		row.erase(0, pos+1+3);
		//cout << row << endl;

		while(row.length() != 0 && row.find(' ') != string::npos)
		{
			string prob;
			pos = row.find_first_of(' ');
			prob = row.substr(0, pos);
			TempLine.push_back(stod(prob));
			row.erase(0, pos+1+3);

		}

		trans.push_back(TempLine);
		TempLine.clear();
	}

	return trans;
}

mvD ExtractEmit(ifstream &hmmEmit_file)
{
	mvD emit;
	string row;
	string word;
	string prob;
	size_t pos, pos_c, pos_s;

	while(getline(hmmEmit_file, row))
	{
		pos = row.find_first_of(' ');
		row.erase(0, pos+1);
		int count = 0;

		while(row.length() != 0 && row.find(' ') != string::npos)
		{
			pos_c = row.find_first_of(':');
			word = row.substr(0, pos_c);
			row.erase(0, pos_c+1);

			pos_s = row.find_first_of(' ');
			prob = row.substr(0, pos_s);
			row.erase(0, pos_s+1);

			auto it = emit.find(word);

			//if exists, push prob to vD. else, add to map. 
			if(it != emit.end())
			{
				it->second.push_back(stod(prob));
			}
			else
			{
				vD probs;
				probs.push_back(stod(prob));
				emit.insert({word, probs});
			}
		}
	}

	return emit;
}


vD ExtractPrior(ifstream &hmmPrior_file)
{
	vD prior;
	string row;
	size_t pos;

	while(getline(hmmPrior_file, row))
	{
		pos = row.find_first_of(' ');
		row.erase(0, pos+1);
		prior.push_back(stod(row));

	}

	return prior;
}


void PrintvvD(vvD a)
{
	for (int i = 0; i < a.size(); i++)
	{
		cout << endl;
		for(int j = 0; j < a[i].size(); j++)
		{
			cout << a[i][j] << "  \t";
		}
	}

	cout << endl << "------------------------" << endl;
}

void Forward(vS sentence, vvD trans, mvD emit, vD prior)
{
	vvD alphaTable;
	double prob;
	double logsum = log(0);
	double a, b, alpha; 
	double temp;

	auto it = emit.find(sentence[0]);

	//Initial state
	for (int i = 0; i < prior.size(); i++)
	{
		b = it->second[i];
		prob = log(prior[i]) + log(b); 

		vD row;
		for(int t = 0; t < sentence.size(); t++)
		{
			if (t == 0)
				row.push_back(prob);
			else
				row.push_back(0);
		}
		
		alphaTable.push_back(row);

	}

	//PrintvvD(alphaTable);

	//Subsequent states
	for (int t = 0; t < sentence.size()-1; t++)
	//for (int t = 0; t < 1; t++)
	{	
		//Fill alphaTable (column t)
		for(int i = 0; i < prior.size(); i++)
		{
			logsum = log(0);
			for(int j = 0; j < prior.size(); j++)
			{
				temp = alphaTable[j][t] + log(trans[j][i]);
				logsum = log_sum(logsum, temp); 
				//cout << temp << '\t';
			}
			//cout << endl;


			auto it = emit.find(sentence[t+1]);
			b = it->second[i];

			alphaTable[i][t+1] = log(b) + logsum;

			// cout << "b :" << b << endl;
		}

		//PrintvvD(alphaTable);
	}

	logsum = log(0);
	int T = sentence.size();
	for(int i = 0; i < prior.size(); i++)
	{
		temp = alphaTable[i][T-1];
		logsum = log_sum(logsum, temp);
	}
	cout << logsum << endl;

}



void Backward(vS sentence, vvD trans, mvD emit, vD prior)
{
	vvD betaTable;
	double prob;
	double logsum = log(0);
	double a, b, alpha; 
	double temp;

	auto it = emit.find(sentence[0]);

	//Initial state
	for (int i = 0; i < prior.size(); i++)
	{
		vD row;
		for(int t = 0; t < sentence.size(); t++)
		{
			if (t == sentence.size()-1)
				row.push_back(log(1));
			else
				row.push_back(0);
		}
		
		betaTable.push_back(row);

	}

	//PrintvvD(betaTable);

	//Subsequent states
	for (int t = sentence.size()-1-1; t >= 0; t--)
	//for (int t = 0; t < 1; t++)
	{	
		//Fill betaTable (column t)
		for(int i = 0; i < prior.size(); i++)
		{
			logsum = log(0);
			for(int j = 0; j < prior.size(); j++)
			{
				auto it = emit.find(sentence[t+1]);
				b = it->second[j];

				temp = betaTable[j][t+1] + log(trans[i][j]) + log(b);
				logsum = log_sum(logsum, temp); 
				//cout << temp << '\t';
			}

			betaTable[i][t] = logsum;

			// cout << "b :" << b << endl;
		}

		//PrintvvD(betaTable);
	}

	logsum = log(0);
	int T = sentence.size();
	it = emit.find(sentence[0]);
	for(int i = 0; i < prior.size(); i++)
	{
		b = it->second[i];

		temp = log(prior[i]) + log(b) + betaTable[i][0];
		logsum = log_sum(logsum, temp);
	}
	cout << logsum << endl;

}


void Viterbi(vS sentence, vvD trans, mvD emit, vD prior)
{
	vvD vTable;
	vvD Q;
	double prob;
	double logsum = log(0);
	double a, b, alpha; 
	double temp;
	double index;

	auto it = emit.find(sentence[0]);

	//Initial state
	for (int i = 0; i < prior.size(); i++)
	{
		b = it->second[i];
		prob = log(prior[i]) + log(b); 

		vD row;
		vD Qrow;
		for(int t = 0; t < sentence.size(); t++)
		{
			if (t == 0)
			{
				Qrow.push_back(i);
				row.push_back(prob);
			}
			else
			{
				Qrow.push_back(0);
				row.push_back(0);
			}
		}
		
		vTable.push_back(row);
		Q.push_back(Qrow);

	}


	//Subsequent states
	for (int t = 0; t < sentence.size()-1; t++)
	//for (int t = 0; t < 2; t++)
	{	
		//Fill vTable (column t)
		for(int i = 0; i < prior.size(); i++)
		{
			double max = -std::numeric_limits<double>::infinity();
			index = 0;
			auto it = emit.find(sentence[t+1]);
			b = it->second[i];
			for(int j = 0; j < prior.size(); j++)
			{
				temp = vTable[j][t] + log(trans[j][i]) + log(b);
				if (temp > max)
				{
					index = j;
					max = temp;
				}
			}
			
			vTable[i][t+1] = max;
			Q[i][t+1] = index;
		}

		// PrintvvD(vTable);
		// PrintvvD(Q);
	}

	double max = -std::numeric_limits<double>::infinity();
	index = 0;
	int T = sentence.size();
	for (int i = 0; i < prior.size(); i++)
	{
		temp = vTable[i][T-1];
		if(temp > max)
		{
			max = temp;
			index = i;
		}

	}

	vD list;

	list.push_back(index);
	string result;
	for(int t = sentence.size()-1; t >= 0; t--)
	{
		result = sentence[t] + GetTag(index) + ' ' + result;
		//cout << sentence[t] << GetTag(index) << ' ';
		index = Q[index][t];
	}

	cout << result << endl;


}



