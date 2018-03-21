#ifndef _NEURAL_NETWORK_H
#define _NEURAL_NETWORK_H

// CTRNN
// Updates:
// v_i(t+dt) = tanh(\alpha*v_i(t) + \tau*\sum_{j,w_{ij}\neq 0} w_{ij}(t)*v_j(t))

#include <string>

#include "neuron.h"
#include "synapse.h"
#include "parallelSwitch.h"
#include "datastruct.h"
#include "constants.h"

#include "environment.h"
class ENVIRONMENT; // AB: circular dependence in declaration is required to be able to connect neurons to various entities in environment

class NEURAL_NETWORK {

private:
	int numNeurons;
	int numSynapses;
	int numParallelSwitches;

	NEURON* neurons[MAX_NEURONS];
	SYNAPSE* synapses[MAX_SYNAPSES];
	PARALLEL_SWITCH* parallelSwitches[MAX_SWITCHES];

public:
	NEURAL_NETWORK(void) : numNeurons(0),
	                       numSynapses(0),
	                       numParallelSwitches(0),
	                       neurons{NULL},
	                       synapses{NULL},
	                       parallelSwitches{NULL} {};
	~NEURAL_NETWORK(void) {}; // FIXME: clean up after your NEURAL_NETWORK!

	bool Is_A_Neuron_Type(std::string typeName);
	bool Is_A_Synapse_Type(std::string typeName);
	bool Is_A_Switch_Type(std::string typeName);

	void Read_Neuron_From_Python(std::string neuronTypeStr, ENVIRONMENT* environment, Data* data);
	void Read_Synapse_From_Python(std::string synapseTypeStr, ENVIRONMENT* environment, Data* data);
	void Read_Switch_From_Python(std::string switchTypeStr, ENVIRONMENT* environment, Data* data);

	void Update(int timeStep);

private:
	void Push_Current_Values_To_Previous_Values(void);

	void Reset_Neuron_Values(int timeStep);
	void Reset_Switches(void);

	void Threshold_Neurons(void);

	void Update_Synapses(int timeStep);

	void Update_Neurons(void);
};

#endif // _NEURAL_NETWORK_H
