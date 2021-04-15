/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/olsr-module.h"
#include "ns3/energy-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/device-energy-model.h"
#include "ns3/mobility-module.h"
#include "ns3/spider-module.h"
#include "ns3/agra-module.h"
#include "ns3/aodv-module.h"
#include "ns3/mesh-helper.h"
#include "ns3/mesh-module.h"
#include "ns3/v4ping-helper.h"
#include "ns3/random-variable-stream.h"
#include <fstream>


//#include "myapp.h"

NS_LOG_COMPONENT_DEFINE ("MyExpr2");

using namespace ns3;

// We want to look at changes in the ns-3 Udp congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off
// application is not created until Application Start time, so we wouldn't be
// able to hook the socket (now) at configuration time.  Second, even if we
// could arrange a call after start time, the socket is not public so we
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass
// this socket into the constructor of our simple application which we then
// install in the source node.
// ===========================================================================
//
class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);
//  std::cout << Simulator::Now ().GetSeconds () << "\t" << "one packet has been sent! PacketSize="<<m_packetSize<<"\n";
  ScheduleTx ();
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds(m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
CourseChange (std::string foo, Ptr<const MobilityModel> mobility)
{
  Vector pos = mobility->GetPosition ();
  std::cout << Simulator::Now ().GetSeconds () << ", Paramedic has changed location -> POS: (" << pos.x << ", " << pos.y << ")." << std::endl;
}

/* 
  Calculate throughput obtain from simulation
    sink : receiver nodes
    oldCur : previous throughput obtained
*/
static void 
SaveTh(Ptr<OutputStreamWrapper> stream, Ptr<PacketSink> sink, double oldCur)
{
  double time = Simulator::Now().GetSeconds();
  double cur = (sink->GetTotalRx() - oldCur) * (double) 8/ 1000000;
  std::cout<<"Throughput at time ["<<time<<"] overall = "<<cur/5<<std::endl;
  *stream->GetStream () << cur/5 << std::endl;
  Simulator::Schedule(Seconds(5), &SaveTh, stream, sink, sink->GetTotalRx());
}

static void  
EnergyRemaning(Ptr<OutputStreamWrapper> stream, EnergySourceContainer sources)
{
        double sum_energy=0;
        for(uint16_t n=0;n<sources.GetN();n++)
        {
                //Ptr<Node> l = DynamicCast<Node> (sources.Get (n));
                //std::cout<<"Node Ipvaddress = "<<l->GetId()<<std::endl;
                Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (n));
		Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);	
                //std::cout<<"Energy consumped at node ["<< n<<"] = "<<1000 - basicSourcePtr->GetRemainingEnergy()<<std::endl;
                sum_energy += basicSourcePtr->GetRemainingEnergy();//(1000 - basicRadioModelPtr->GetTotalEnergyConsumption());
                *stream->GetStream () << basicSourcePtr->GetRemainingEnergy() << std::endl;
        }
        std::cout<<"Energy consumped at time ["<<Simulator::Now().GetSeconds ()<<"] overall = "<<sum_energy<<std::endl;
        *stream->GetStream () << sum_energy << std::endl;
        //Simulator::Schedule (Seconds(60), &EnergyRemaning, stream, sources);
}

/* 
  Instatiate infinite travel of vehicle where 
     vehicle : all mobility nodes in scene
     carSpeed : velocity of mobility node
  When traveling vehicle is hitting border, it will move to the beginning point 
*/
static void 
CheckBound(NodeContainer vehicle, double carSpeed){
    for(uint16_t i=0;i<vehicle.GetN();i++)
    {
            Ptr<ConstantVelocityMobilityModel> mob = vehicle.Get(i)->GetObject<ConstantVelocityMobilityModel>();
            Vector m_position = mob->GetPosition();
            //Vector m_velocity = mob->GetVelocity();
            if (m_position.x <= 0) 
                {
                   m_position.x += 600;
                   //m_velocity.y = -10.0;
                   mob->SetPosition(m_position);
                }
            mob->SetVelocity(Vector(-carSpeed, 0.0, 0.0));
    }
    
}

int main (int argc, char *argv[])
{
  bool enableFlowMonitor = false;
  //ErpOfdmRate54Mbps
  //DsssRate11Mbps
  std::string phyMode ("ErpOfdmRate54Mbps");
  uint16_t RepulsionMode = (uint16_t) 0;
  //Init the location and radius of obstacle
  double locationX=300,locationY=450;
  double object_radius=282;
  double lambda=0;
  Time FailureTime = Seconds(30.0);
  double FailureProb = 0.0;
  Time StopTime = Seconds(720.0);
  Time LocationTime = Seconds(180.0);
  double SrcSpeed = 2.8; //[m/s] speed of jogging
  double carSpeed = 20; //5 //20 //[m/s] speed of car running
  std::cout<<"lambda value = "<<lambda<<std::endl;

  CommandLine cmd;
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("RepulsionMode", "Enable Repulsion during Greedy Forwarding", RepulsionMode);
  cmd.AddValue ("FailureTime", "Nodes Failure Time", FailureTime);
  cmd.AddValue ("FailureProb", "Nodes Failure Probability", FailureProb);
  cmd.AddValue ("StopTime", "Time to Stop Simulation", StopTime);
  cmd.AddValue ("LocationTime", "Time src spends at each location", LocationTime);
  cmd.AddValue ("SrcSpeed", "Speed of the paramedic who acts as a src between locations", SrcSpeed);
  cmd.Parse (argc, argv);

  //
  // Explicitly create the nodes required by the topology (shown above).
  //
  NS_LOG_INFO ("Create nodes.");
  NodeContainer c1; // sink and source
  c1.Create(8);
  NodeContainer c2;
  c2.Create(2);
  NodeContainer c3;
  c3.Create(7);
  NodeContainer c4;
  c4.Create(7);
  NodeContainer c5;
  c5.Create(1);

  NodeContainer allTransmissionNodes;
  allTransmissionNodes.Add(c1);
  allTransmissionNodes.Add(c2);
  allTransmissionNodes.Add(c3);
  allTransmissionNodes.Add(c4);
  allTransmissionNodes.Add(c5);

  NodeContainer vehicle;
  vehicle.Create(6);
  allTransmissionNodes.Add(vehicle);

  NodeContainer sinkSrc;
  sinkSrc.Create(2);

  //add to one container
  NodeContainer c;
  c.Add(sinkSrc); //add sink and source
  c.Add(allTransmissionNodes);


  // Set up WiFi
  WifiHelper wifi;
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
  YansWifiChannelHelper wifiChannel ;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel",
	  	  	  		  	  	  	    "SystemLoss", DoubleValue(1),
                                    "HeightAboveZ", DoubleValue(1.5));
  // For range near 250m
  wifiPhy.Set ("TxPowerStart", DoubleValue(20));
  wifiPhy.Set ("TxPowerEnd", DoubleValue(20));
  wifiPhy.Set ("TxPowerLevels", UintegerValue(1));
  wifiPhy.Set ("TxGain", DoubleValue(6));
  wifiPhy.Set ("RxGain", DoubleValue(0));
  wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-68.8));//-64.5));
  wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-71.8));//-67.5));

  wifiPhy.SetChannel (wifiChannel.Create ());
  NetDeviceContainer devices;

  //for all cases except HWMP uncomment below ============================================
  // Add a non-QoS upper mac
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");

  // Set 802.11g standard
  wifi.SetStandard (WIFI_PHY_STANDARD_80211g);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue(phyMode),
                                "ControlMode",StringValue(phyMode));
  devices = wifi.Install (wifiPhy, wifiMac, c);
  //============================================

  //for HWMP uncomment below ============================================
/*
         std::string root = "ff:ff:ff:ff:ff:ff";
         MeshHelper mesh = MeshHelper::Default(); 
         mesh.SetRemoteStationManager ("ns3::ConstantRateWifiManager");//,
         mesh.SetStackInstaller ("ns3::Dot11sStack"); 
         mesh.SetSpreadInterfaceChannels (MeshHelper::SPREAD_CHANNELS); 
         mesh.SetNumberOfInterfaces (1);
         devices = mesh.Install (wifiPhy, c);
*/
  //============================================

  //==============Set up battery lion=========
  BasicEnergySourceHelper basicSourceHelper;
  // configure energy source
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (1000));
  // install source
  EnergySourceContainer sources = basicSourceHelper.Install (c);
  
  // configure mesh energy model
  // uncomment for hwmp
  //MeshRadioEnergyModelHelper meshEnergyHelper; 
  //meshEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174));
  //meshEnergyHelper.Set ("RxCurrentA", DoubleValue (0.0174));
  //DeviceEnergyModelContainer deviceModels = meshEnergyHelper.Install (devices, sources); 

  // configure radio energy model 
  // uncomment for other routing
  WifiRadioEnergyModelHelper radioEnergyHelper; 
  radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174)); 
  radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0.0174));
  DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (devices, sources); 
  //==========================================

  // setup routing protocol section ==========================================
  /*
  AgraHelper agra;
  agra.Set("RepulsionMode",UintegerValue(RepulsionMode));
  agra.Set("locationX",DoubleValue(locationX));
  agra.Set("locationY",DoubleValue(locationY));
  agra.Set("object_radius",DoubleValue(object_radius));
  */
  SpiderHelper spider;
  spider.Set("RepulsionMode",UintegerValue(RepulsionMode));
  spider.Set("locationX",DoubleValue(locationX));
  spider.Set("locationY",DoubleValue(locationY));
  spider.Set("object_radius",DoubleValue(object_radius));
  spider.Set("lambda",DoubleValue(lambda));
  
  AodvHelper aodv;
  InternetStackHelper internet;
  internet.SetRoutingHelper (spider); //uncomment to use spider
  //internet.SetRoutingHelper (agra); //uncomment to use agra or egf
  //internet.SetRoutingHelper (aodv); //uncomment to use aodv
  internet.Install (c);
  //============================================

  // Set up Addresses
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifcont = ipv4.Assign (devices);

  // Set Mobility for all nodes
  /*
  Choose location for mobility node with 
  GridWidth : number of element on a rowLBScheme
  MinX,MinY : location based
  DeltaX, DeltaY: the distant between 2 nodes
  LayoutType: Row-wise model
  */
  MobilityHelper mobility1;
  mobility1.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (100.0),
                                 "MinY", DoubleValue (300.0),
                                 "DeltaX", DoubleValue (50),
                                 "DeltaY", DoubleValue (50),
                                 "GridWidth", UintegerValue (1),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility1.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility1.Install(c1);

  MobilityHelper mobility2;
  mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (400.0),
                                 "MinY", DoubleValue (300.0),
                                 "DeltaX", DoubleValue (50),
                                 "DeltaY", DoubleValue (50),
                                 "GridWidth", UintegerValue (1),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility2.Install(c2);

  MobilityHelper mobility3;
  mobility3.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (500.0),
                                 "MinY", DoubleValue (350.0),
                                 "DeltaX", DoubleValue (50),
                                 "DeltaY", DoubleValue (50),
                                 "GridWidth", UintegerValue (1),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility3.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility3.Install(c3);

  MobilityHelper mobility4;
  mobility4.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (150.0),
                                 "MinY", DoubleValue (650.0),
                                 "DeltaX", DoubleValue (50),
                                 "DeltaY", DoubleValue (50),
                                 "GridWidth", UintegerValue (7),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility4.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility4.Install(c4);

  MobilityHelper mobility5;
  mobility5.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (450.0),
                                 "MinY", DoubleValue (350.0),
                                 "DeltaX", DoubleValue (50),
                                 "DeltaY", DoubleValue (50),
                                 "GridWidth", UintegerValue (5),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility5.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility5.Install(c5);

  // Set original locations for moving objects (vehicles)	
  MobilityHelper stationMobilityHelper;
  Ptr<ListPositionAllocator> stationPositionAlloc = CreateObject<ListPositionAllocator>();
  stationPositionAlloc -> Add(Vector(550.0, 150.0, 0.0));
  stationPositionAlloc -> Add(Vector(450.0, 150.0, 0.0));
  stationPositionAlloc -> Add(Vector(350.0, 150.0, 0.0));
  stationPositionAlloc -> Add(Vector(250.0, 150.0, 0.0));
  stationPositionAlloc -> Add(Vector(150.0, 150.0, 0.0));
  stationPositionAlloc -> Add(Vector(50.0 , 150.0, 0.0));
  stationMobilityHelper.SetPositionAllocator(stationPositionAlloc);
  stationMobilityHelper.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  stationMobilityHelper.Install(vehicle);
  
  // Start moving vehicle in simulation 
  for(uint32_t i=0;i<vehicle.GetN();i++){
    (vehicle.Get(i) -> GetObject<ConstantVelocityMobilityModel>()) -> SetVelocity(Vector(-carSpeed, 0.0, 0.0));
  }
  for (uint n=0 ; n < 720 ; n++)
  {
    Simulator::Schedule(Seconds(n), &CheckBound, vehicle, carSpeed);
  }        

  //sink is static and represents adhoc network edge, e.g., internet gateway
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator>();
  positionAlloc ->Add(Vector(0, 650, 0)); // sink
  mobility.SetPositionAllocator(positionAlloc);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(sinkSrc.Get(1));

  //paramedic acts as a src and moves from location 1 to location 3 through location 2.

  MobilityHelper mobilitySrc;
  Ptr<ListPositionAllocator> positionAllocSrc = CreateObject <ListPositionAllocator>();
  positionAllocSrc ->Add(Vector(100, 0, 0)); // source
  mobilitySrc.SetPositionAllocator(positionAllocSrc);
  mobilitySrc.SetMobilityModel ("ns3::WaypointMobilityModel",
                               "InitialPositionIsWaypoint", BooleanValue (true));
  mobilitySrc.Install(sinkSrc.Get(0));

  Waypoint location1 (LocationTime, Vector(100, 0, 0));
  Time nextWaypointTime = LocationTime + Seconds(250/SrcSpeed);
  Waypoint location21 (nextWaypointTime, Vector(350,0,0));
  Waypoint location22 (nextWaypointTime + LocationTime, Vector(350,0,0));
  Time lastWaypointTime = nextWaypointTime + LocationTime + Seconds(250/SrcSpeed);
  Waypoint location3 (lastWaypointTime, Vector(600,0,0));

  //Add waypoints to the src
  Ptr<WaypointMobilityModel> srcModel = sinkSrc.Get(0)->GetObject<WaypointMobilityModel> ();
  srcModel->AddWaypoint(location1);
  srcModel->AddWaypoint(location21);
  srcModel->AddWaypoint(location22);
  srcModel->AddWaypoint(location3);

  //istall agra headers to all nodes
  //agra.Install ();
  spider.Install ();

  //setup applications
  NS_LOG_INFO ("Create Applications.");

  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (ifcont.GetAddress (1), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (sinkSrc.Get (1));
  sinkApps.Start (Seconds (1.9));//1.9
  sinkApps.Stop (StopTime);

  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (sinkSrc.Get (0), UdpSocketFactory::GetTypeId ());
  //ns3UdpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3UdpSocket, sinkAddress, 1448, DataRate ("5Mbps"));
  sinkSrc.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (2));//2
  app->SetStopTime (StopTime);

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("cwnd_mobicom_expr2.txt");
  ns3UdpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
 
  Ptr<PacketSink> sink = StaticCast<PacketSink> (sinkApps.Get(0));
  double oldCur = sink -> GetTotalRx();
  Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream ("thr_high_mobility_GEAR0.txt");
  Simulator::Schedule(Seconds(0), &SaveTh, stream1, sink, oldCur);
  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream ("energy_high_mobility_GEAR0.txt");
  Simulator::Schedule(StopTime, &EnergyRemaning, stream2, sources);
/*
  V4PingHelper ping (ifcont.GetAddress (1));
  ping.SetAttribute ("Verbose", BooleanValue (true));

  ApplicationContainer p = ping.Install (sinkSrc.Get (0));
  p.Start (Seconds (5));
  p.Stop (StopTime - Seconds (0.001));

  std::cout <<"src ip="<<ifcont.GetAddress (0, 0)<<" id="<<sinkSrc.Get (0)->GetId() <<"; sink ip="<<ifcont.GetAddress(1, 0)<<" id="<<sinkSrc.Get (1)->GetId() <<std::endl;
*/

  //log only src movements
  Config::Connect ("/NodeList/31/$ns3::MobilityModel/CourseChange",
                     MakeCallback (&CourseChange));

  // Trace devices (pcap)
  // wifiPhy.EnablePcap ("mobicom_expr2", devices.Get(0)); //save pcap file for src
  wifiPhy.EnablePcap ("mobicom_expr2", devices.Get(1)); //save pcap file for sink
  // wifiPhy.EnablePcapAll ("mobicom_expr2");

  // Now, do the actual simulation.
  NS_LOG_INFO ("Run Simulation.");

  Simulator::Stop (StopTime);
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
