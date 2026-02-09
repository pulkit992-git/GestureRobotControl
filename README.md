* **Actuated Physics Simulation:** Replaced traditional physics handles with a motor-driven constraint system, simulating real-world robotic gripping via friction and normal force.
* **Non-Kinematic Interaction:** Developed a torque-based control loop for end-effectors, allowing the manipulator to interact with objects of variable geometry without predefined animations.
* **Low-Level Control Logic:** Engineered a C++ interface that translates external gesture data into joint-space torque commands, mimicking industrial PLC (Programmable Logic Controller) behavior.
