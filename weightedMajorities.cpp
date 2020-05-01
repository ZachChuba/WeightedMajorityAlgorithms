#include <iostream>
#include <istream>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>
#define NUM_TRIALS 100
using namespace std;

int weightedMajority(int m, double n, ifstream &in, vector<double> &weights,
		vector<int> &numMistakes) {
	int predictions[m];
	int totalcorrect = 0;
	int truthvalue;
	char c;
	int loc = 0;

	while (in.get(c)) {
		if (loc < m)
			predictions[loc] = c - '0';
		else if (loc == m + 1)
			truthvalue = c - '0';
		loc++;
		if (loc == m + 2) {
			double alpha = 0;
			double beta = 0;
			int prediction;
			for (int i = 0; i < m; i++) {
				int predict = predictions[i];
				if (predict)
					alpha += weights[i];
				else
					beta += weights[i];
			}
			if (alpha >= beta)
				prediction = 1;
			else
				prediction = 0;
			for (int i = 0; i < m; i++) {
				if (predictions[i] != truthvalue) {
					weights[i] = weights[i] * (1 - n);
					numMistakes[i]++;
				}
			}
			if (prediction == truthvalue)
				totalcorrect++;
			loc = 0;
		}
	}
	in.clear();
	in.seekg(0);
	return totalcorrect;
}

int randomSample(vector<double> &weights, int *predictions, mt19937& gen) {
	double LO = 0;
	double HI = 0;
	for (double weight : weights)
		HI += weight;
	uniform_real_distribution<> dist(LO, HI);
	double random = dist(gen);
	for (int i = weights.size() - 1; i >= 0; i--) {
		HI -= weights[i];
		if (random >= HI) {
			//cout << i << endl;
			return predictions[i];
		}
	}
	return predictions[0];

}

int randomWeightedMajority(int m, double n, ifstream &in,
		vector<double> &weights, vector<int> &numMistakes, mt19937& gen) {
	int predictions[m];
	int totalcorrect = 0;
	int truthvalue;
	char c;
	int loc = 0;

	while (in.get(c)) {
		if (loc < m)
			predictions[loc] = c - '0';
		else if (loc == m + 1)
			truthvalue = c - '0';
		loc++;
		if (loc == m + 2) {
			// take random sample
			int prediction = randomSample(weights, predictions, gen);
			for (int i = 0; i < m; i++) {
				if (predictions[i] != truthvalue) {
					weights[i] = weights[i] * (1 - n);
					numMistakes[i]++;
				}
			}
			if (prediction == truthvalue)
				totalcorrect++;
			loc = 0;
		}

	}
	in.clear();
	in.seekg(0);
	return totalcorrect;
}

void weightedMajorityInit(int m, double n, ifstream &in) {
	vector<double> weights;
	vector<int> numMistakes;
	int totalCorrect;
	for (int i = 0; i < m; i++)
		weights.push_back(1);
	for (int i = 0; i < m; i++)
		numMistakes.push_back(0);
	totalCorrect = weightedMajority(m, n, in, weights, numMistakes);
	cout << "Weighted Majority" << endl;
	cout << "Total mispredictions = " << (1000 - totalCorrect) << endl;
	int min = 99999;
	for (int person : numMistakes) {
		//cout << person << " ";
		if (person < min)
			min = person;
	}
	cout << "Best guesser got " << min << " wrong." << endl;
}

void randomWeightedMajorityInit(int m, double n, ifstream &in) {
	random_device rd;
	mt19937 gen(time(NULL));
	vector<int> numMistakes;
	int totalCorrect;
	int min_g = 1000;
	int max_g = 0;
	int avg = 0;
	for (int i = 0; i < m; i++)
		numMistakes.push_back(0);
	for (int i = 0; i < NUM_TRIALS; i++) {
		vector<double> weights;
		for (int i = 0; i < m; i++)
			weights.push_back(1);
		totalCorrect = randomWeightedMajority(m, n, in, weights, numMistakes, gen);
		avg += totalCorrect;
		if (totalCorrect < min_g)
			min_g = totalCorrect;
		if (totalCorrect > max_g)
			max_g = totalCorrect;
	}
	avg /= NUM_TRIALS;
	cout << "\n Random Weighted Majority " << endl;
	cout << "The expected number of incorrect guesses is " << (1000 - avg)
			<< endl;
	cout << "The minimum number of correct guesses in " << NUM_TRIALS
			<< " trials was " << min_g << endl;
	cout << "The max: " << max_g << endl;
	int minMistake = 999999;
	for (int mistake : numMistakes)
		if (mistake < minMistake)
			minMistake = mistake;
	cout << "The minimum number of mistakes for n = " << NUM_TRIALS << " is: " << minMistake << "\n\n";
}

int main() {
	ifstream data("hw4.txt");
	weightedMajorityInit(16, 0.01, data);
	randomWeightedMajorityInit(16, 0.01, data);
	return 0;
}
