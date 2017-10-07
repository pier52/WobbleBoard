CMS Inspection Vehicle GUI Documentation                         {#mainpage}
========================================

The CMS Inspection Vehicle is a prototype for a robotics platform, intended to be able to operate inside the CMS-detector cavern at CERN. This is the implementation of the graphical user interface, which controls and receives information from the vehicle. It is implemented in C++ Qt, and is intended to work on both Windows and Linux platforms (Tested and developed on Windows 10 and Ubuntu 16.04).

The GUI can, among other things, do the following:
	* Send control commands (steering, throttle, camera position etc.) using the keyboard
	* Display real time video stream from the vehicle's on-board cameras
	* Graphically display the current vehicle status (wheel direction, camera position etc.)
	* Demand and plot sensor values from the inspection vehicle
	* Display error and status messages in a message area
	* Send configuration parameters to the inspection vehicle (camera resolution, PID parameters etc.)

The idea has been to keep the main window as clean as possible, making it easy for a non-expert to operate the vehicle, and putting all the advanced settings in different menus with provided default values.