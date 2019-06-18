/*
 * IOpair.h
 *
 *  Created on: 12 sept. 2018
 *      Author: colosu
 */

#ifndef SRC_IOPAIR_H_
#define SRC_IOPAIR_H_

#include <string>

namespace fst {

class IOpair {
public:
	IOpair();
	IOpair(string in, string out);
	virtual ~IOpair();
	const string& getInput();
	void setInput(const string& input);
	const string& getOutput();
	void setOutput(const string& output);
	bool equal(IOpair* other);
	friend bool operator==(const IOpair& first, const IOpair& second);
	inline friend bool operator!=(const IOpair& first, const IOpair& second){ return !(first == second); }
	friend bool operator<(const IOpair& first, const IOpair& second);
	inline friend bool operator>(const IOpair& first, const IOpair& second){ return second < first; }
	inline friend bool operator<=(const IOpair& first, const IOpair& second){ return !(first > second); }
	inline friend bool operator>=(const IOpair& first, const IOpair& second){ return !(first < second); }

private:
	string input;
	string output;
};

} /* namespace fst */

#endif /* SRC_IOPAIR_H_ */
