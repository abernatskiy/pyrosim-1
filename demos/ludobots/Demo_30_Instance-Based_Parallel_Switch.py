import pyrosim
import math
import sys

def makeWavebot(x,y):
    '''WAVEBOT!!!!!!!!'''
    ARM_LENGTH = 0.5
    ARM_RADIUS = ARM_LENGTH / 10.0

    cyl1 = sim.send_cylinder(x=x, y=y, z=ARM_LENGTH/2.0 + 2*ARM_RADIUS,
                             r1=0, r2=0, r3=1,
                             length=ARM_LENGTH, radius=ARM_RADIUS)
    cyl2 = sim.send_cylinder(x=x, y=y+ARM_LENGTH/2.0, z=ARM_LENGTH + 2 *
                             ARM_RADIUS, r1=0, r2=1, r3=0,
                             length=ARM_LENGTH, radius=ARM_RADIUS)

    joint1 = sim.send_hinge_joint(first_body_id=cyl1, second_body_id=cyl2,
                                  x=x, y=y, z=ARM_LENGTH + 2*ARM_RADIUS,
                                  n1=1, n2=0, n3=0,
                                  lo=-3.14159/4.0, hi=+3.14159/4.0)
    joint2 = sim.send_hinge_joint(first_body_id=cyl1,
                                  second_body_id=pyrosim.Simulator.WORLD,
                                  x=x, y=y, z=ARM_LENGTH/2.0 + 2*ARM_RADIUS)

    mneuron = sim.send_motor_neuron(joint_id=joint1)

    return mneuron

sim = pyrosim.Simulator(eval_time=500, xyz=(3,-3,3), debug=True)

mn1 = makeWavebot(0,0)
mn2 = makeWavebot(1,0)

fast_fneuron = sim.send_function_neuron(function=lambda x: math.sin(x*10))
slow_fneuron = sim.send_function_neuron(function=math.sin)
switch_fneuron = sim.send_function_neuron(function=lambda x: math.sin(x/4.))
# switch_bias = sim.send_bias_neuron(value=0.0)

# Both bots either wave fast or slow
# The regime is governed by the slowest function neuron
# psid, psoutput = sim.send_instance_based_parallel_switch(2, 2, 1, [[fast_fneuron, fast_fneuron], [slow_fneuron, slow_fneuron]], [switch_fneuron], {(-2,):0, (2,):1})
# sim.send_synapse(psoutput[0], mn1, weight=1.)
# sim.send_synapse(psoutput[1], mn2, weight=1.)


psid, psoutput = sim.send_instance_based_parallel_switch(1, 2, 1, [[fast_fneuron], [slow_fneuron]], [switch_fneuron], {(-2,):0, (2,):1})
sim.send_synapse(psoutput[0], mn1, weight=1.)

ccsen = sim.send_current_controller_sensor(psid)

switch_fneuron2 = sim.send_function_neuron(function=lambda x: math.sin(math.pi + (x/4.)))
psid2, psoutput2 = sim.send_instance_based_parallel_switch(1, 2, 1, [[fast_fneuron], [slow_fneuron]], [switch_fneuron2], {(-2,):0, (2,):1})
sim.send_synapse(psoutput2[0], mn2, weight=1.)

ccsen2 = sim.send_current_controller_sensor(psid2)

sim.start()

sim.wait_to_finish()

ccsendata = sim.get_sensor_data(ccsen)
ccsendata2 = sim.get_sensor_data(ccsen2)
for ts in range(len(ccsendata)):
 	print(str(ccsendata[ts]) + ' ' + str(ccsendata2[ts]))
