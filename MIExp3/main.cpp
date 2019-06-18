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
//#include <gsl/gsl_statistics.h>
#include <fst/fst-decl.h>
#include <fst/fstlib.h>
#include <chrono>
#include "src/SqueezinessLib.h"
#include <unistd.h>

using namespace fst;

#define REP 7
#define LEN 100
#define N 10
#define MUT 1000
#define TESTS 2
#define EXP 100
#define INI 0
#define TIME 0.000001

int main(int argc, char * argv[]) {

	srand(time(NULL));

	//Initialization
	IOHandler* IOH = new IOHandler();
	Mutations* Mutator = new Mutations();
	Checkups* Checker = new Checkups();
	Operations* Ops = new Operations();

	std::string Ifile = "binary.fst";
	std::string Sizefile = "size.txt";
	std::string MIfile = "MI" + to_string(TIME) + ".txt";
	std::string Runfile = "Run" + to_string(TIME) + ".txt";

	std::ofstream SizeFile;
	std::ofstream MIFile;
	std::ofstream RunFile;

	Graph* G;
	double wins[N];
	double valid[N];
	list<list<IOpair>>* TS[TESTS];
	double MI[TESTS];
	Graph* GM[MUT];
	bool detected[TESTS][MUT];
	int count[TESTS];
	std::chrono::duration<double> elapsed;
	auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	double sizes[N];
	double MITime[N] = {0,0,0,0,0,0,0,0,0,0};
	double RunTime[N] = {0,0,0,0,0,0,0,0,0,0};
	double GTime = 0;
	int ticks = 0;
	float ti = TIME;

	for (int J = 0; J < REP; J++) {

		MIfile = "MI" + to_string(ti) + ".txt";
		Runfile = "Run" + to_string(ti) + ".txt";

		for (int i = 0; i < N; i++) {
			wins[i] = 0;
			valid[i] = 0;
			sizes[i] = 0;
			MITime[i] = 0;
			RunTime[i] = 0;
			GTime = 0;
		}

		for (int I = INI; I < INI + EXP; I++) {

			Ifile = "./Tests/test" + to_string(I+1) + "/binary.fst";
//			Ifile = "./Tests/Phone/binary.fst";
//			Ifile = "./War of the Worlds/binary.fst";

			ticks = 0;
			G = IOH->readGraph(Ifile, ticks);
			GTime += ticks*ti;

			if (G == NULL) {
				return 1;
			}

			if (!Checker->is_valid(G)) {
				std::cerr << "Not valid graph." << std::endl;
				return 1;

			}

			for (int k = 0; k < N; k++) {
				try {
					//Generate Test Suites
					for (int i = 0; i < TESTS; i++) {
						TS[i] = new list<list<IOpair>>();
					}
					for (int i = 0; i < TESTS; i++) {
						Ops->GenerateTestSuite(G, LEN+(100*k), *TS[i], true, false);
//						Ops->GenerateTestSuite(*TS[i], i+2);
					}
					sizes[k] = LEN+(100*k);

					//Check Mutual Information
					for (int i = 0; i < TESTS; i++) {
						MI[i] = 0;
					}
					start = std::chrono::high_resolution_clock::now();
					for (int i = 0; i < TESTS; i++) {
						Ops->MutualInformation(G, *TS[i], MI[i]);
					}
					finish = std::chrono::high_resolution_clock::now();
					elapsed = finish - start;
					MITime[k] += elapsed.count()/TESTS;

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
					start = std::chrono::high_resolution_clock::now();
					for (int i = 0; i < TESTS; i++) {
						for (int j = 0; j < MUT; j++) {
							detected[i][j] = Checker->checkMutation(GM[j], *TS[i], ticks);
						}
					}
					finish = std::chrono::high_resolution_clock::now();
					elapsed = finish - start;
					RunTime[k] += (elapsed.count() + (ticks*ti))/(TESTS*MUT);

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
						wins[k]++;
					}
					if (count[0] != count[1] && MI[0] != MI[1]) {
						valid[k]++;
					}
//					cout << "run " << to_string(I) << endl;

				} catch (exception &e) {
					cout << "Exception: " << e.what() << endl;
				}
			}

			delete G;

		}

		SizeFile.open(Sizefile);
		if (!SizeFile.is_open()) {
			std::cerr << "I can't create the output file." << std::endl;
			return 1;
		}
		MIFile.open(MIfile);
		if (!MIFile.is_open()) {
			std::cerr << "I can't create the output file." << std::endl;
			return 1;
		}
		RunFile.open(Runfile);
		if (!RunFile.is_open()) {
			std::cerr << "I can't create the output file." << std::endl;
			return 1;
		}

		for (int k = 0; k < N; k++) {

			SizeFile << sizes[k] << endl;
			MIFile << MITime[k]/EXP + GTime/EXP << endl;
			RunFile << RunTime[k]/EXP << endl;
		}

		SizeFile.close();
		MIFile.close();
		RunFile.close();

		cout << "test " << to_string(J+1) << endl;

		ti = ti*10;
	}

	delete IOH;
	delete Mutator;
	delete Checker;
	delete Ops;

	return 0;
}
