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

	OFile << "| #Test | Percentage of success |" << std::endl;


	Graph* G;
	double wins;
	double valid;
	list<list<IOpair>>* TS[TESTS];
	double MI[TESTS];
	Graph* GM[MUT];
	bool detected[TESTS][MUT];
	int count[TESTS];
	double mean = 0;
	double total = 0;

	for (int J = 0; J < REP; J++) {

		wins = 0;
		valid = 0;

		for (int I = INI; I < INI + EXP; I++) {

			Ifile = "./Tests/test" + to_string(I+1) + "/binary.fst";
//			Ifile = "./Tests/Phone/binary.fst";
//			Ifile = "./War of the Worlds/binary.fst";

			G = IOH->readGraph(Ifile);

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
				for (int i = 0; i < TESTS; i++) {
					Ops->GenerateTestSuite(G, LEN, *TS[i], true, false);
//					Ops->GenerateTestSuite(*TS[i], i+2);
				}

				//Check Mutual Information
				for (int i = 0; i < TESTS; i++) {
					MI[i] = 0;
				}
				for (int i = 0; i < TESTS; i++) {
					Ops->MutualInformation(G, *TS[i], MI[i]);
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
				for (int i = 0; i < TESTS; i++) {
					for (int j = 0; j < MUT; j++) {
						detected[i][j] = Checker->checkMutation(GM[j], *TS[i]);
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
				if ((count[0] > count[1] && MI[0] < MI[1]) || (count[1] > count[0] && MI[1] < MI[0])) {
					wins++;
				}
				if (count[0] != count[1] && MI[0] != MI[1]) {
					valid++;
				}
//					cout << "run " << to_string(I) << endl;

			} catch (exception &e) {
				cout << "Exception: " << e.what() << endl;
			}
		}

		delete G;

		cout << "test " << to_string(J+1) << endl;
		if (valid != 0) {
			OFile << J+1 << " & "  << wins/valid << "\\% " << " \\\\" << std::endl;
		} else {
			OFile << J+1 << " & ?\\% \\\\" << std::endl;
		}
		OFile << "\\hline" << std::endl;

		mean += wins;
		total += valid;
	}

	OFile << "Mean & " << mean/total << "\\% " << " \\\\" << std::endl;
	OFile << "\\hline" << std::endl;

	OFile.close();

	delete IOH;
	delete Mutator;
	delete Checker;
	delete Ops;

	return 0;
}
