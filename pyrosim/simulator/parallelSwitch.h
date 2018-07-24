#ifndef _PARALLEL_SWITCH_H
#define _PARALLEL_SWITCH_H

#include <set>
#include <vector>
#include <algorithm>

class PARALLEL_SWITCH {

private:
	unsigned numChannels;
	unsigned numOptions;
	std::set<int> allIDs;
	std::vector<std::vector<int>> inputIDs;
	std::vector<int> outputIDs;
	std::vector<int> controlIDs;

	std::vector<double> controlInputs;
	int currentOption;

	inline unsigned Num_Inputs(void) {return numChannels*numOptions;};
	void Select_Current_Option(void);

public:
	PARALLEL_SWITCH(void) : numChannels(0),
	                        numOptions(0),
	                        currentOption(0) // FIXME: generally, sensors must be polled and passed through the networks first to select the controller
	                                         // Zeroth controller selected arbitrarily at the first timestep
	{};
	void Read_From_Python(void);
	void Print(void);
	bool Is_A_Control_Neuron_Of_This_Switch(int ID) {return std::find(controlIDs.begin(), controlIDs.end(), ID) != controlIDs.end();};
	bool Is_A_Virtual_Output_Neuron_Of_This_Switch(int ID) {return std::find(outputIDs.begin(), outputIDs.end(), ID) != outputIDs.end();};
	int Get_Total_Number_Of_Virtual_Neurons(void) {return numChannels;};
	void Set_Control_Neuron_Value(int controlNeuronID, double value);
	int Get_Input_ID(int outputID);
	void Reset(void);
	int Get_Current_Control_ID(void) {return controlIDs[currentOption];};

};

#endif // _PARALLEL_SWITCH_H
