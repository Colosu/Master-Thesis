/*
 * Checkups.h
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#ifndef CHECKUPS_H_
#define CHECKUPS_H_

#include <fst/fstlib.h>
#include <string>
#include "Graph.h"

namespace fst {

class Checkups {
public:
	Checkups();
	~Checkups();
	bool are_equivalent(Graph* g1, Graph* g2);
	bool is_valid(Graph* g);
	bool is_validMutation(Graph* g);
	bool checkMutation(Graph* g, list<list<IOpair>> TS, int &ticks);

private:
	bool checkMutation(Graph* g, list<IOpair> T, int &ticks);
};

} /* namespace std */

#endif /* CHECKUPS_H_ */
