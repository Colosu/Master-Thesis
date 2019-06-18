/*
 * main.cpp
 *
 *  Created on: 19 sept. 2017
 *      Author: colosu
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <fst/fst-decl.h>
#include <fst/fstlib.h>
#include <chrono>
#include "src/SqueezinessLib.h"
#include <unistd.h>

using namespace fst;

#define REP 50
#define LEN 100
#define MUT 1000
#define TESTS 2
#define EXP 100
#define INI 0
#define TIME 0
#define POOL 50

int main(int argc, char * argv[]) {

	srand(time(NULL));

	//Initialization
	IOHandler* IOH = new IOHandler();
	Mutations* Mutator = new Mutations();
	Checkups* Checker = new Checkups();
	Operations* Ops = new Operations();

	std::string Ifile = "binary.fst";
	std::string Ofile = "Results.txt";

	std::ofstream OFile;

	OFile.open(Ofile);
	if (!OFile.is_open()) {
		std::cerr << "I can't create the output file." << std::endl;
		return 1;
	}

	OFile << "| #Test | Percentage of success MIGA | Percentage of success TSDm |" << std::endl;


	Graph* G;
	double winsMIGA;
	double winsTSDm;
	double valid;
	list<list<IOpair>>* TS[TESTS];
	Graph* GM[MUT];
	bool detected[TESTS][MUT];
	int count[TESTS];
	double meanMIGA = 0;
	double meanTSDm = 0;
	double total = 0;
	int size = LEN;
//	std::chrono::duration<double> elapsed;
//	auto start = std::chrono::high_resolution_clock::now();
//	auto finish = std::chrono::high_resolution_clock::now();
//	double timeMIGA = 0;
//	double timeTSDm = 0;
	int ticks = 0;

	for (int J = 0; J < REP; J++) {

		winsMIGA = 0;
		winsTSDm = 0;
		valid = 0;

		for (int I = INI; I < INI + EXP; I++) {

			Ifile = "./Tests/test" + to_string(I+1) + "/binary.fst";
//			Ifile = "./Tests/Phone/binary.fst";
//			Ifile = "./War of the Worlds/binary.fst";

			ticks = 0;
			G = IOH->readGraph(Ifile, ticks);

			if (G == NULL) {
				return 1;
			}

			if (!Checker->is_valid(G)) {
				std::cerr << "Not valid graph." << std::endl;
				return 1;
			}

			try {
				//Generate Test Suites
				for (int i = 0; i < TESTS; i++) {
					TS[i] = new list<list<IOpair>>();
				}

//				start = std::chrono::high_resolution_clock::now();
				size = Ops->GenerateTSDmTestSuite(G, LEN, POOL, *TS[0], *TS[1]);
				if (size == -1) {
					return 1;
				}
//				finish = std::chrono::high_resolution_clock::now();
//				elapsed = finish - start;
//				timeTSDm += elapsed.count();
//				start = std::chrono::high_resolution_clock::now();
//				Ops->GenerateGeneticTestSuite(G, size, *TS[0]);
//				finish = std::chrono::high_resolution_clock::now();
//				elapsed = finish - start;
//				timeMIGA += elapsed.count();
				for (int i = 2; i < TESTS; i++) {
					Ops->GenerateRandomTestSuite(G, size, *TS[i], true, false);
				}

				//Generate Mutants
				for (int i = 0; i < MUT; i++) {
					GM[i] = Mutator->mutateState(G);
					while (!Checker->is_validMutation(GM[i])) {
						delete GM[i];
						GM[i] = Mutator->mutateState(G);
					}
				}


				//Check Fail Detection
				ticks = 0;
				for (int i = 0; i < TESTS; i++) {
					for (int j = 0; j < MUT; j++) {
						detected[i][j] = Checker->checkMutation(GM[j], *TS[i], ticks);
					}
				}

				//Delete test suites
				for (int i = 0; i < TESTS; i++) {
					delete TS[i];
				}

				//Delete mutants
				for (int i = 0; i < MUT; i++) {
					delete GM[i];
				}

				//Count fail detection
				for (int i = 0; i < TESTS; i++) {
					count[i] = 0;
				}
				for (int i = 0; i < TESTS; i++) {
					for (int j = 0; j < MUT; j++) {
						if(detected[i][j]) {
							count[i]++;
						}
					}
				}

				//Check if our measure detected the best test suite
//				if (count[0] > (int)*max_element(&count[1], &count[TESTS])) {
//					winsMIGA++;
//				} else if (count[1] > (int)*max_element(&count[2], &count[TESTS])) {
//					winsTSDm++;
//				}
//				if (count[0] != (int)*max_element(&count[1], &count[TESTS]) && count[1] != (int)*max_element(&count[2], &count[TESTS])) {
//					valid++;
//				}
				if (count[0] > count[1]) {
					winsMIGA++;
				} else if (count[0] < count[1]) {
					winsTSDm++;
				}
				if (count[0] != count[1]) {
					valid++;
				}

			} catch (exception &e) {
				cout << "Exception: " << e.what() << endl;
			}

			delete G;
			cout << I << endl;
		}

		cout << "test " << to_string(J+1) << endl;
		if (valid != 0) {
			OFile << J+1 << " & " << winsMIGA/valid  << "\\% & " << winsTSDm/valid << "\\% \\\\" << std::endl;
		} else {
			OFile << J+1 << " & ?\\% & ?\\% \\\\" << std::endl;
		}
		OFile << "\\hline" << std::endl;

		meanMIGA += winsMIGA;
		meanTSDm += winsTSDm;
		total += valid;
	}

	OFile << "Mean & " << meanMIGA/total << "\\% & " << meanTSDm/total << "\\% \\\\" << std::endl;
	OFile << "\\hline" << std::endl;
//	OFile << "Time & " << timeMIGA/EXP << "\\% & " << timeTSDm/EXP << "\\% \\\\" << std::endl;
//	OFile << "\\hline" << std::endl;

	OFile.close();

	delete IOH;
	delete Mutator;
	delete Checker;
	delete Ops;

	return 0;
}
