--------------------------------------------------------------------------------------------------------
Files included in the folder:
--------------------------------------------------------------------------------------------------------
iot-energy-optimal-routing : Module Folder

download-install-dependency-libraries.sh : Shell script to download & install dependency libraries

iot-energy-optimal-route-example-topology.cc : Topology implemented in the project. This file also included in 
					       iot-energy-optimal-routing/examples/iot-energy-optimal-route-example-topology.cc

********************************************************************************************************
Installation Steps to be followed:
********************************************************************************************************
1. Download all the dependency libraries:

We need to install a few libraries in Ubuntu in order to have ns-3 work as expected. The following bash(download-install-dependency-libraries.sh) has various libraries that are needed to be installed. This is combined in the source code directory submitted. Execute the following command inside project directory and provide the system password to install all the corresponding libraries using apt-get:

./download-install-dependency-libraries.sh

This project can be executed from any folder. For instance, considering /home/suny/cs646/project as the base folder for installing NS-3(Please use any relative path).
 
********************************************************************************************************
2. Clone ns-3-allinone repo:

mkdir -pv /home/suny/cs646/project/repos

cd /home/suny/cs646/project/repos

hg clone http://code.nsnam.org/ns-3-allinone

********************************************************************************************************
3. Download all the corresponding source code of ns-3-dev:

cd /home/suny/cs646/project/repos/ns-3-allinone

./download.py -n ns-3-dev

********************************************************************************************************
4. Add  iot-energy-optimal-routing module to ns-3:

Copy the iot-energy-optimal-routing module provided in the submission folder and place it in ns-3-allinone/ns-3-dev/src folder.

cd <Project_folder>

cp -r iot-energy-optimal-routing  /home/suny/cs646/project/repos/ns-3-allinone/ns-3-dev/src/iot-energy-optimal-routing

    

 **Please make sure iot-energy-optimal-routing folder is copied successfully into /home/suny/cs646/project/repos/ns-3-allinone/ns-3-dev/src folder.
  
********************************************************************************************************
5. Build source code: (Trigger build.py file From ns-3-dev folder. This step will take some time)

cd /home/suny/cs646/project/repos/ns-3-allinone/

./build.py

********************************************************************************************************
6. Copy topology simulation file to scratch folder:

Copy the example topology simulation file(iot-energy-optimal-route-example-topology.cc) from ns-3-dev/src/iot-energy-optimal-routing/examples/iot-energy-optimal-route-example-topology.cc to ns-3-dev/scratch/iot-energy-optimal-route-example-topology.cc

cp /home/suny/cs646/project/repos/ns-3-allinone/ns-3-dev/src/iot-energy-optimal-routing/examples/iot-energy-optimal-route-example-topology.cc /home/suny/cs646/project/repos/ns-3-allinone/ns-3-dev/scratch/iot-energy-optimal-route-example-topology.cc

********************************************************************************************************
7. Run topology simulation file using waf;

cd /home/suny/cs646/project/repos/ns-3-allinone/ns-3-dev

./waf --run scratch/iot-energy-optimal-route-example-topology

