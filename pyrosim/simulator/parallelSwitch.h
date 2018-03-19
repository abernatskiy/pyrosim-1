#ifndef _PARALLEL_SWITCH_H
#define _PARALLEL_SWITCH_H

#include <set>
#include <vector>

class PARALLEL_SWITCH {

private:
	unsigned numChannels;
	unsigned numOptions;
	std::set<int> allIDs;
	std::vector<std::vector<int>> inputIDs;
	std::vector<int> outputIDs;
	std::vector<int> controlIDs;

	inline unsigned Num_Inputs(void) {return numChannels*numOptions;};

public:
	PARALLEL_SWITCH(void) : numChannels(0), numOptions(0) {};
	void Read_From_Python(void);
	void Print(void);
};

#endif // _PARALLEL_SWITCH_H
