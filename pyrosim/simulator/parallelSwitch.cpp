#include <iostream>
#include <cstdlib> // for debug
#include <cassert>
#include <cmath>

#include "parallelSwitch.h"

//////// Private methods ////////

void PARALLEL_SWITCH::Select_Current_Option(void) {

	currentOption = -1;

	double max = -1*INFINITY;
	for(int i=0; i<numOptions; i++) {
		if(max < controlInputs[i]) {
			max = controlInputs[i];
			currentOption = i;
		}
	}
	assert(currentOption != -1);
	assert(max != -1*INFINITY);
}

//////// Public methods ////////

void PARALLEL_SWITCH::Read_From_Python(void) {

	static_assert(std::numeric_limits<double>::is_iec559, "IEEE 754 required");

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

	for(unsigned i=0; i<numOptions; i++)
		controlInputs.push_back(-1*INFINITY);
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

void PARALLEL_SWITCH::Set_Control_Neuron_Value(int controlNeuronID, double value) {

	int controlNeuronIdx = -1;
	for(unsigned i=0; i<numOptions; i++) {
		if(controlIDs[i] == controlNeuronID) {
			controlNeuronIdx = i;
			break;
		}
	}
	assert(controlNeuronIdx != -1);

	controlInputs[controlNeuronIdx] = value;

	Select_Current_Option();
}

int PARALLEL_SWITCH::Get_Input_ID(int outputID) {

	int outputNeuronIdx = -1;
	for(unsigned j=0; j<numChannels; j++) {
		if(outputIDs[j] == outputID) {
			outputNeuronIdx = j;
			break;
		}
	}
	assert(outputNeuronIdx != -1);

	assert(currentOption != -1);

	return inputIDs[currentOption][outputNeuronIdx];
}

void PARALLEL_SWITCH::Reset(void) {

	for(auto it=controlInputs.begin(); it!=controlInputs.end(); it++)
		*it = -1*INFINITY;
}
