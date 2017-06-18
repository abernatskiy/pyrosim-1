import sys
sys.path.insert(0, '..')

import pyrosim


def run(test):

    if test:
        kwargs = {'debug': False, 'play_blind': True, 'eval_time': 500}
    else:
        kwargs = {'debug': True, 'play_blind': False, 'eval_time': 500}

    ARM_LENGTH = 0.5
    ARM_RADIUS = ARM_LENGTH / 10.0
    PI = 3.14159

    sim = pyrosim.Simulator(**kwargs)

    cyl1 = sim.send_cylinder(x=0, y=0, z=ARM_LENGTH/2.0 + ARM_RADIUS,
                             r1=0, r2=0, r3=1,
                             length=ARM_LENGTH, radius=ARM_RADIUS)
    cyl2 = sim.send_cylinder(x=0, y=ARM_LENGTH/2.0, z=ARM_LENGTH + ARM_RADIUS,
                             r1=0, r2=1, r3=0,
                             length=ARM_LENGTH, radius=ARM_RADIUS)

    joint = sim.send_hinge_joint(first_body_id=cyl1, second_body_id=cyl2,
                                 x=0, y=0, z=ARM_LENGTH + ARM_RADIUS,
                                 n1=1, n2=0, n3=0, lo=-PI/4.0, hi=+PI/4.0)

    touch1 = sim.send_touch_sensor(body_id=cyl1)
    touch2 = sim.send_touch_sensor(body_id=cyl2)

    sneuron1 = sim.send_sensor_neuron(sensor_id=touch1)
    sneuron2 = sim.send_sensor_neuron(sensor_id=touch2)

    mneuron = sim.send_motor_neuron(joint_id=joint)

    # neurons now connected so movement is possible
    #'closes' the joint up
    sim.send_synapse(source_neuron_id=sneuron1,
                     target_neuron_id=mneuron, weight=1.0)
    sim.send_synapse(source_neuron_id=sneuron2,
                     target_neuron_id=mneuron, weight=1.0)
    sim.start()
    sim.wait_to_finish()

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == 'test':
        run(test=True)
        print('Successfully made synapses')
    else:
        run(test=False)