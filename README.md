# quad101
  
<pre>
  ________  ___  ___  ________  ________    _____  ________    _____
 |\   __  \|\  \|\  \|\   __  \|\   ___ \  / __  \|\   __  \  / __  \
 \ \  \|\  \ \  \\\  \ \  \|\  \ \  \_|\ \|\/_|\  \ \  \|\  \|\/_|\  \
  \ \  \\\  \ \  \\\  \ \   __  \ \  \ \\ \|/ \ \  \ \  \\\  \|/ \ \  \
   \ \  \\\  \ \  \\\  \ \  \ \  \ \  \_\\ \   \ \  \ \  \\\  \   \ \  \
    \ \_____  \ \_______\ \__\ \__\ \_______\   \ \__\ \_______\   \ \__\
     \|___| \__\|_______|\|__|\|__|\|_______|    \|__|\|_______|    \|__|
           \|__|
</pre>


Early stage of our planned quadcopter project, which is to be realized by using
the Arduino 101 with its builtin Intel's Curie Module as the main flight controller.

Right now, we are working on the implementation of positional control with a PID controller and different sensor sources
<ul>
<li>Onboard gyro and accelerometer (done)</li>
<li>3axis-magnetometer (done)</li>
<li>Ultrasonic Rangefinder (done)</li>
<li>Pressure sensor (done)</li>
<li>GPS Module (beta)</li>
</ul>

We did succed in building the first layer positional controller that stabilizes the pitch and roll rotation of the quad. 
Next up is are flying tests with a RC-receiver. After that we will continue developing the autonomous flying features. 
<ul>
<li>Pitch & Roll controller (done)</li>
<li>Jaw controller (beta)</li>
<li>Height controller (beta)</li>
<li>BLE PID tuner (beta)</li>
<li>RC mode (done)</li>
<li>Location controller</li>
</ul>

The main goal is the process of learning and not really the final product. If we can however accomplish to build
a good working prototype, we will consider making a usable quadcopter from it.
