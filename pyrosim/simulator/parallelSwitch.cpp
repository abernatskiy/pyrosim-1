#include <iostream>
#include <cstdlib> // for debug

#include "parallelSwitch.h"

void PARALLEL_SWITCH::Read_From_Python(void) {

	std::cin >> numChannels;
	std::cin >> numOptions;

	int currentID;

	for(unsigned i=0; i<numOptions; i++) {
		std::vector<int> currentOptionIDs;
		for(unsigned j=0; j<numChannels; j++) {
			std::cin >> currentID;
			currentOptionIDs.push_back(currentID);
			allIDs.insert(currentID);
		}
		inputIDs.push_back(currentOptionIDs);
	}

	for(unsigned j=0; j<numChannels; j++) {
		std::cin >> currentID;
		outputIDs.push_back(currentID);
		allIDs.insert(currentID);
	}

	for(unsigned i=0; i<numOptions; i++) {
		std::cin >> currentID;
		controlIDs.push_back(currentID);
		allIDs.insert(currentID);
	}

	Print();
	exit(EXIT_FAILURE);
}


void PARALLEL_SWITCH::Print(void) {

	std::cout << "Parallel switch with " << numChannels << " channels and " << numOptions << " options, a total of " << allIDs.size() << " virtual neurons\n";
	std::cout << "\nInputs:\n";
	for(unsigned i=0; i<numOptions; i++) {
		for(unsigned j=0; j<numChannels; j++)
			std::cout << inputIDs[i][j] << ' ';
		std::cout << '\n';
	}
	std::cout << "\nOutputs:\n";
	for(unsigned j=0; j<numChannels; j++)
		std::cout << outputIDs[j] << ' ';
	std::cout << "\nControls:\n";
	for(unsigned i=0; i<numOptions; i++)
		std::cout << controlIDs[i] << ' ';
	std::cout << '\n';
}
