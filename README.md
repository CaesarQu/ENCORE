# SPIDER3D
repository for the SPIDER3D algorithm.

We introduce a novel environmentally-aware cooperative drone path computation scheme viz., **SPIDER-3D** that can serve needs of full autonomy in multi-drone, multi-truck last-mile parcel delivery tasks. 
First, our 3D \textit{offline} approach builds upon the geographic routing scheme in SPIDER, and involves proactively ensuring energy efficiency, while maximizing the throughput of parcel delivery. Our SPIDER-3D approach that is inherently offline-based, provides resilience to FANETs that are relevant to the delivery application scenario by overcoming barriers due to obstacles that impact A2A and A2G network communications. The resilience feature in SPIDER-3D geographic routing is due to our ability to ensure that the reconstruction of the A2A or A2G network communications has limited overhead on the Oracle delivery task, which is attributed to the state-of-the-art *parcel delivery scheduling algorithm*.

## Apply SPIDER-3D in parcel delivery procedure

**SPIDER-3D** is first used inside parcel delivery applications. To apply SPIDER-3D, a scheduling algorithm result needs to be given. For instance, in our use case, we use [mFSTSP algorithm](https://github.com/optimatorlab/mFSTSP). We considered 1-4 drones and 0-2 trucks on the parcel delivery scenario. 

## NS-3 scripts

Folder [ns-3_sim](./ns-3_sim) provides SPIDER3D implementation scripts. SPIDER3D needs to have location-based module in NS-3 as a add-on, which is also provided in the folder.

## Preliminary results:

|      | Drone | GEAR (with $\theta$ = 0.75) |      1     | SPIDER-3D (with $\theta$ = 0) |    0.25    |     0.5    |    0.75    |      1     |    AODV    |    HWMP    |
|:----:|:-----:|:---------------------------:|:----------:|:-----------------------------:|:----------:|:----------:|:----------:|:----------:|:----------:|:----------:|
|  low |   1   |          525$\pm$70         | 522$\pm$46 |           519$\pm$34          | 520$\pm$37 | 519$\pm$42 | 522$\pm$37 | 520$\pm$31 | 489$\pm$27 | 563$\pm$61 |
|      |   3   |          514$\pm$14         | 515$\pm$13 |           514$\pm$12          | 507$\pm$18 | 506$\pm$18 | 506$\pm$50 | 501$\pm$40 | 480$\pm$31 | 532$\pm$89 |
| high |   1   |          502$\pm$52         | 497$\pm$47 |           503$\pm$49          | 498$\pm$36 | 498$\pm$40 | 500$\pm$39 | 501$\pm$40 | 445$\pm$25 | 555$\pm$52 |
|      |   3   |          510$\pm$13         | 499$\pm$16 |           504$\pm$48          | 504$\pm$46 | 495$\pm$40 | 493$\pm$39 | 493$\pm$39 | 465$\pm$26 | 531$\pm$86 |

## Detailed setup steps:

If using NS-3 version 33:

During the compile procedure.
1. spider-packet.cc: Line 307: change to: m_dstPosy == o.m_dstPosy
2. All isEqual (e.g. id.IsEqual(i→first)) change to ‘==’ (Line 65, 110, 507)
3. If AGRA model added, change all the content mentioned in 1 and 2 from AGRA as well.

During the experiment procedure:
1. YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default (); change to YansWifiPhyHelper wifiPhy = YansWifiPhyHelper();
2. Consider other energy helper model (MeshEnergyHelper is deprecated), or not use hwmp:
   	  // configure mesh energy model	
	  // uncomment for hwmp	
	  MeshRadioEnergyModelHelper meshEnergyHelper; 	
	  meshEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174));	
	  meshEnergyHelper.Set ("RxCurrentA", DoubleValue (0.0174));	
	  DeviceEnergyModelContainer deviceModels = meshEnergyHelper.Install (devices, sources); 	
		
	  // configure radio energy model 	
	  // uncomment for other routing	
	  //WifiRadioEnergyModelHelper radioEnergyHelper; 	
	  //radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174)); 	
	  //radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0.0174));	
	  //DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (devices, sources); 
3. YansWifiPhyHelper 'Set' function parameters change a lot from version to version. Need to be careful on setting up values (Radio Signal, Energy, CcaModel) (Old version is tested can be run on ns-3.25) 

If using NS-3 version 25:

Since Python2.7 is no longer supportted for any future NS-3 version. If you want to run the original project, you need to install back the python2.7. Two ways to achieve this: 1) install python27 in system or 2) install inside virtualenv.

Recommand: use vitualenv (references: https://computingforgeeks.com/how-to-install-python2-with-virtualenv-on-ubuntu/)

Detailed setups:
1. inside ns-3.xx folder, type:
$ CXXFLAGS="-Wall -g -O0" ./waf configure
$ ./waf
(wait for all packets compiled. Ignore warnings)

2. Copy model: Spider, location-based, and Agra inside ns-3.xx/src folder
Note: for version 33, update all the PrintRoutingTable function (e.g.: inside agra.h, line 110: change to 'PrintRoutingTable (ns3::Ptr<ns3::OutputStreamWrapper>,Time::Unit)' to avoid any error)
$ ./waf 
(compile again, ignore warnings)

3. Copy experiments: 
Two experiments are introduced: 
1) $ ./waf --run SPIDER_moving_vehicle_sim (Provide transmission package with different mobility speed. According to high mobility, collect data for every 5 seconds.)
2) $ ./waf --run SPIDER_failure_sim (Provide transmission package logs with different percentage of obstacles)

Parameters:
1) lambda value: change inside both experiments, determine the value on focuing more energy or more latency in reward functions.
2) hwmp options: hwmp protocol using other packages start with mesh notation. Requires mesh module: see https://www.nsnam.org/docs/models/html/mesh.html, and need to comment out all hwmp related code inside each experiments to activate hwmp experiments.
3) obstacle percentage: change FailureProb inside SPIDER_failure_sim, 0.05 (5 percent) default.
4) packet details: comment out following line to collect packet info. If comment, only throughput info will be shown.
	std::cout << Simulator::Now ().GetSeconds () << "\t" << "one packet has been sent! PacketSize="<<m_packetSize<<"\n";
5) use fileHelper module to save all information into files. See: https://www.nsnam.org/docs/tutorial/html/data-collection.html
	Note: be aware of the output format in the terminal, or no content will be stored!


## Future Plan

Provide security strategies inside SPIDER3D, add pre-existing packages on NS-3 to support. Experiments with attack dataset on IoT devices/sensors are considered.