//mkdir build && cd build/
//cmake DCMAKE_BUILD_TYPE=Debug ..
//make
//ifconfig (and note down the ip address)
//Find all instances of inet_pton, replace hard-coded ip address in this function call with the ip address obtained from the previous step
//sudo iptables -A OUTPUT -p tcp --tcp-flags RST RST -j DROP
//sudo ./test2
//In a separate terminal window run nc <ip address> 12345 
//Type input in the new terminal window, and it should appear in the terminal running the test2 binary


//To run order matching engine
//sudo ./exchange

//To run client
//sudo ./client

Output should look like:
A
A
E

//To enable GUI Debugging with gdb in vscode
//Run: sudo chmod a+x ./gdb from the hft_project_rework directory
//Edit sudoers: sudo visudo
//Add the following line to the bottom of the file: user_name ALL=(ALL) NOPASSWD:/usr/bin/gdb
//Save and replace the existing /etc/sudoers file
//Create a .vscode directory if not there already, and add the launch.json file in the hft_project_rework directory to the .vscode directory
//Use the launch.json file as a starting place, it contains a task to launch the client executable, if you set breakpoints in src/client.cpp they will be caught


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
further:
-- add update order message
-- improve the interface to show the order book
-- document the structure of the project
-- document the various executables produced
-- document what the files contain