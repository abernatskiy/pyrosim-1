#!/usr/bin/env python3

import math
import numpy as np
import math
import pyrosim

if __name__ == "__main__":

    seconds = 30.0
    dt = 0.05
    eval_time = int(seconds/dt)
    gravity = 0.0
    max_torque = 5.

    sim = pyrosim.Simulator(eval_time=eval_time, debug=False,
                               play_paused=False,
                               gravity=gravity,
                               play_blind=False,
                               use_textures=True,
                               capture=False,
                               dt=dt,
                               xyz=[3, -4, 4])


    box0 = sim.send_box(x=0, y=0, z=2., length=1.6, width=1.0, height=0.6, r=0, g=1, b=0)
    rcw0 = sim.send_reaction_control_wheel(box0, max_torque=max_torque, momentum_budgets=(0.05,0.05,0.05))
    pcrcw0 = sim.send_proprioceptive_sensor(rcw0)

    mneurons = [0]*3
    fneurons = [0]*3
    sdata = []
    for i in range(3):
        mneurons[i] = sim.send_motor_neuron(rcw0, input_index=i, tau=0.1, alpha=0.)
        fneurons[i] = sim.send_function_neuron(lambda x: 0. if x<5.*i else math.cos(x*3))
        sim.send_synapse(fneurons[i], mneurons[i], 1.0)

    sim.create_collision_matrix('all')
    sim.start()
    sim.wait_to_finish()

    import matplotlib.pyplot as plt
    legend = ['x','y','z']
    for i in range(3):
        sdata.append(sim.get_sensor_data(pcrcw0, svi=i))
        plt.plot(sdata[-1], label=legend[i])
    plt.title('Accumulated rotational momentum vs time')
    plt.xlabel('Momentum gauge')
    plt.ylabel('Time, simulation ticks')
    plt.legend()
    plt.show()
