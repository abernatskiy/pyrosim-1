#ifndef _NEURAL_NETWORK_CPP
#define _NEURAL_NETWORK_CPP

#include <iostream>
#include <cmath>
#include <cstdlib>

#include "neuralNetwork.h"

#include "constants.h"

extern const int SENSOR_NEURON;
extern const int BIAS_NEURON;
extern const int HIDDEN_NEURON;
extern const int MOTOR_NEURON;
extern const int FUNCTION_NEURON;

bool NEURAL_NETWORK::Is_A_Neuron_Type(std::string typeName) {
	return stringToNeuronTypeMap.find(typeName) != stringToNeuronTypeMap.end();
}

bool NEURAL_NETWORK::Is_A_Switch_Type(std::string typeName) {
	return typeName.compare("ParallelSwitch") == 0;
}

bool NEURAL_NETWORK::Is_A_Synapse_Type(std::string typeName) {
	return typeName.compare("Synapse") == 0;
}

int NEURAL_NETWORK::Get_Current_Controller_By_Parallel_Switch_ID(int psid) {

	for(int k=0; k<numParallelSwitches; k++) {
		if(parallelSwitches[k]) {
			int curpsid = parallelSwitches[k]->Get_ID();
			if(curpsid == psid)
				return parallelSwitches[k]->Get_Current_Control_ID();
		}
	}
	return -1;
}

void NEURAL_NETWORK::Read_Neuron_From_Python(std::string neuronTypeStr, ENVIRONMENT* env, Data* data) {

	neurons[numNeurons] = new NEURON();
	neurons[numNeurons]->Read_From_Python(neuronTypeStr, data);

	// Certain types of neurons get special treatment

	if(neuronTypeStr.compare("MotorNeuron") == 0) {

		int actuatorInputIndex, actuatorID;
		std::cin >> actuatorID;
		std::cin >> actuatorInputIndex;

		ACTUATOR* act = env->Get_Actuator(actuatorID);
		act->Connect_To_Motor_Neuron(neurons[numNeurons], actuatorInputIndex);
	}

	if(neuronTypeStr.compare("SensorNeuron") == 0) {

		int sensorValueIndex, sensorID;
		std::cin >> sensorID;
		std::cin >> sensorValueIndex;

		// We attempt to connect the sensor neuron to every possible object and actuator
		// FIXME: replace once a global list of sensors is added to the environment
		bool done = false;
		for(int i=0; (!done) && i<env->Get_Number_Of_Objects(); i++) {
			if(env->Get_Object(i)->Connect_Sensor_To_Sensor_Neuron(sensorID, sensorValueIndex, neurons[numNeurons]))
				done = true;
		}
		for(int i=0; (!done) && i<env->Get_Number_Of_Actuators(); i++) {
			if(env->Get_Actuator(i)->Connect_Sensor_To_Sensor_Neuron(sensorID, sensorValueIndex, neurons[numNeurons]))
				done = true;
		}
	}

	numNeurons++;
}

void NEURAL_NETWORK::Read_Synapse_From_Python(std::string synapseTypeStr, ENVIRONMENT* environment, Data* data) {

	synapses[numSynapses] = new SYNAPSE();
	synapses[numSynapses]->Read_From_Python();
	numSynapses++;
}

void NEURAL_NETWORK::Read_Switch_From_Python(std::string switchTypeStr, ENVIRONMENT* environment, Data* data) {

	parallelSwitches[numParallelSwitches] = new PARALLEL_SWITCH();
	parallelSwitches[numParallelSwitches]->Read_From_Python();

	int numVirtualNeurons = parallelSwitches[numParallelSwitches]->Get_Total_Number_Of_Virtual_Neurons();
	for(int i=numNeurons; i<numNeurons+numVirtualNeurons; i++)
		neurons[i] = NULL;
	numNeurons += numVirtualNeurons;

	numParallelSwitches++;
}

void NEURAL_NETWORK::Update(int timeStep) {

	Push_Current_Values_To_Previous_Values();

	Reset_Neuron_Values(timeStep);
	Reset_Switches();

	Update_Synapses(timeStep);

	Update_Neurons();
	Threshold_Neurons();
}

// ------------------------- Private methods -----------------------------

void NEURAL_NETWORK::Push_Current_Values_To_Previous_Values(void) {

	for (int n = 0 ; n < numNeurons ; n++ )
		if(neurons[n])
			neurons[n]->Push_Current_Value_To_Previous_Value();
}

void NEURAL_NETWORK::Reset_Neuron_Values(int timeStep) {

	for ( int n = 0 ; n < numNeurons ; n++ )
		if(neurons[n])
			neurons[n]->Reset(timeStep);
}

void NEURAL_NETWORK::Reset_Switches(void) {

	for(int psi=0; psi<numParallelSwitches; psi++)
		parallelSwitches[psi]->Reset();
}

void NEURAL_NETWORK::Threshold_Neurons(void) {

	for ( int n = 0 ; n < numNeurons ; n++ )
		if(neurons[n])
			neurons[n]->Threshold();
}

void NEURAL_NETWORK::Update_Synapses(int timeStep){

	for (int s=0; s<numSynapses; s++)
		synapses[s]->Update_Weight(timeStep);
}

void NEURAL_NETWORK::Update_Neurons(void) {

	// We begin by setting the input values of the control virtual neurons of switches,
	// so that by the time we get to updating the values of real neurons the switches will
	// "know" which inputs to use
	for(int n=0; n<numNeurons; n++) {

		if(neurons[n]) {
			int nid = neurons[n]->Get_ID();
			for(int psi=0; psi<numParallelSwitches; psi++) {
				if(parallelSwitches[psi]->Is_A_Control_Neuron_Of_This_Switch(nid))
					parallelSwitches[psi]->Set_Control_Neuron_Value(nid, neurons[n]->Get_Previous_Value());
			}
		}
	}

	for(int s=0; s<numSynapses; s++) {

		int tni = synapses[s]->Get_Target_Neuron_Index();
		int sni = synapses[s]->Get_Source_Neuron_Index();

		// Now that the switches "know" which inputs to use, we simply forward inputs through them and update neurons as usual
		bool forward = true;
		while(forward) {
			forward = false;
			for(int psi=0; psi<numParallelSwitches; psi++) {
				if(parallelSwitches[psi]->Is_A_Virtual_Output_Neuron_Of_This_Switch(sni)) {
					sni = parallelSwitches[psi]->Get_Input_ID(sni);
					forward = true;
				}
			}
		}

		double w = synapses[s]->Get_Weight();

		double influence = w * neurons[sni]->Get_Previous_Value();

//		int testNeuronID = 7;
//		if(tni == testNeuronID)
//		std::cerr << "Neuron " << sni << " influenced neuron " << tni << " by " << influence << ". Pointers: " << neurons[sni] << " " << neurons[tni] << "\n";

		neurons[tni]->Set( neurons[tni]->Get_Value() + influence );
	}

}

#endif // _NEURAL_NETWORK_CPP
