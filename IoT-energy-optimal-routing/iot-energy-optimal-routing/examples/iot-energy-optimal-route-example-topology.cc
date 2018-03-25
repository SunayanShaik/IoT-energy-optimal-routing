
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/iot-energy-optimal-routing-helper.h"
#include "ns3/iot-energy-optimal-route-processor.h"

// Iot Energy Optimal Routing Network Topology Example
//
//                
//               Iot Nodes connected on Wifi 10.1.3.0 (Source Nodes)
//
//                   Tier 3  |  Tier 2  |    Tier 1
//                           |          |
//                      n8   |    n5    |    n2       (Sink)    
//                           |          |               GW    10.1.1.0
//                      n9   |    n6    |    n3         n1 -------------- n0
//                           |          |
//                     n10   |    n7    |    n4      
//                           |          |

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("IotEnergyOptimalRouteExampleTopology");

/**
* This Method is used to log Packed had reached Sink from one of the source nodes (IOT nodes) 
*/
void ReceivePacket (Ptr<Socket> socket)
{
  while (socket->Recv ())
    {
	  std::ostringstream oss;
      oss << "[Packet_Received] Received one packet!  Sink Node:10.1.3.1";
      NS_LOG_UNCOND (oss.str());
    }
}

/**
* This Method is used to generate traffic (One packet each time) from one of the source nodes (IOT nodes) 
*/
static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval)
{
  if (pktCount > 0)
  {
	  std::ostringstream oss;
	  Address addr;
	  socket->GetSockName (addr);
	  oss << "[Packet_Generated] Generated one packet from Node Id: " << socket->GetNode ()->GetId ();
	  NS_LOG_UNCOND (oss.str());
    socket->Send (Create<Packet> (pktSize));
    Simulator::Schedule (pktInterval, &GenerateTraffic,
                           socket, pktSize,pktCount - 1, pktInterval);
    }
  else
    {
      socket->Close ();
    }
}

/**
*  Start Execution from main method
*/
int
main (int argc, char *argv[])
{
  uint32_t numberOfIotDevices = 9;
  bool tracing = true;
  uint32_t packetSize = 1000; // bytes
  uint32_t numPackets = 1;
  double interval = 1.0;

  CommandLine cmd;
  cmd.Parse (argc,argv);

  Time interPacketInterval = Seconds (interval);

  // Create Gateway Nodes , devices and channels
  NodeContainer gatewayNodes;
  gatewayNodes.Create (2);
  
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer gatewayInternetDevices;
  gatewayInternetDevices = pointToPoint.Install (gatewayNodes);
  NodeContainer gatewayNode = gatewayNodes.Get (0);

  
  // Create IOT Nodes , devices and channels
  NodeContainer iotNodes;
  iotNodes.Create (numberOfIotDevices);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifiHelper;
  wifiHelper.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper wifiMacHelper;
  Ssid ssid = Ssid ("ns-3-ssid");
  wifiMacHelper.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer iotDevices;
  iotDevices = wifiHelper.Install (phy, wifiMacHelper, iotNodes);

  wifiMacHelper.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer gatewayDevices;
  gatewayDevices = wifiHelper.Install (phy, wifiMacHelper, gatewayNode);

  // Set mobility as IOT devices are static we will be using ConstantPositionMobilityModel
  MobilityHelper mobilityHelper;
  mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityHelper.Install (iotNodes);
  mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityHelper.Install (gatewayNodes);

  // Install internet on Gateway nodes
  InternetStackHelper gatewayInternetStackHelper;
  gatewayInternetStackHelper.Install (gatewayNodes);

  //Creating a new routing protocol which ovverides the default routing using InternetStackHelper for IOT nodes connected on WIFI
  IotEnergyOptimalRoutingHelper iotEnergyOptimalRoutingHelper;
  Ptr<IotEnergyOptimalRouteProcessor> iotEnergyOptimalRouteProcessor = CreateObject<IotEnergyOptimalRouteProcessor> ();
  iotEnergyOptimalRoutingHelper.Set ("RoutingProcessor", PointerValue (iotEnergyOptimalRouteProcessor));

  InternetStackHelper iotNodesInternetStackHelper;
  iotNodesInternetStackHelper.SetRoutingHelper(iotEnergyOptimalRoutingHelper);
  iotNodesInternetStackHelper.Install (iotNodes);

  /* Assign Ip addressess to all Nodes.
  * Gateway IP address will be 10.1.3.1
  * All IOT nodes are assigned IP address from 10.1.3.2 to 10.1.3.10 (We have 9 IOT nodes arranged in 3 tiers)
  */
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer gatewayInternetInterfaces;
  gatewayInternetInterfaces = address.Assign (gatewayInternetDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer iotInterfaces;
  Ipv4InterfaceContainer gatewayInterfaces;
  gatewayInterfaces = address.Assign (gatewayDevices);
  iotInterfaces = address.Assign (iotDevices);

  
  /**
  *Assign Tiers and energy for all the nodes
  */

  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(1,iotInterfaces.GetAddress(2),140);
  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(1,iotInterfaces.GetAddress(1),140);
  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(1,iotInterfaces.GetAddress(0),140);

  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(2,iotInterfaces.GetAddress(5),120);
  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(2,iotInterfaces.GetAddress(4),120);
  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(2,iotInterfaces.GetAddress(3),120);

  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(3,iotInterfaces.GetAddress(8),100);
  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(3,iotInterfaces.GetAddress(7),100);
  iotEnergyOptimalRouteProcessor->AddNodeTierEnergy(3,iotInterfaces.GetAddress(6),100);


  /*
  *Creating few Source nodes on each tier of IOT nodes to simulate traffic that travels from nodes to Gateway.
  */
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  InetSocketAddress remote = InetSocketAddress (gatewayInterfaces.GetAddress (0), 80);

  /*
  * Source in Tier 3
  */
  Ptr<Socket> source_tier3_0 = Socket::CreateSocket (iotNodes.Get (8), tid);
  source_tier3_0->Connect (remote);

  Ptr<Socket> source_tier3_1 = Socket::CreateSocket (iotNodes.Get (7), tid);
  source_tier3_1->Connect (remote);

  /*
  * Source in Tier 2
  */
  Ptr<Socket> source_tier2_0 = Socket::CreateSocket (iotNodes.Get (5), tid);
  source_tier2_0->Connect(remote);

  Ptr<Socket> source_tier2_1 = Socket::CreateSocket (iotNodes.Get (5), tid);
  source_tier2_1->Connect(remote);

  /*
  * Source in Tier 1
  */
  Ptr<Socket> source_tier1_0 = Socket::CreateSocket (iotNodes.Get (2), tid);
  source_tier1_0->Connect(remote);


  /**
  * Creating a Sink node for Gateway. This simulation has only one sink node and that is Gateway
  */
  Ptr<Socket> recvSink_1 = Socket::CreateSocket (gatewayNodes.Get (0), tid);
  InetSocketAddress local_1 = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink_1->Bind (local_1);
  recvSink_1->SetRecvCallback (MakeCallback (&ReceivePacket));


  /*
  *Creating simulation each simulation sends a packet from one of the tier into sink in a time interval of 1 second
  */
  Simulator::ScheduleWithContext (source_tier3_0->GetNode ()->GetId (),
                                  Seconds (1.0), &GenerateTraffic,
                                  source_tier3_0, packetSize, numPackets, interPacketInterval);

  Simulator::ScheduleWithContext (source_tier3_0->GetNode ()->GetId (),
                                  Seconds (2.0), &GenerateTraffic,
                                  source_tier3_0, packetSize, numPackets, interPacketInterval);

  Simulator::ScheduleWithContext (source_tier3_1->GetNode ()->GetId (),
                                  Seconds (3.0), &GenerateTraffic,
                                  source_tier3_1, packetSize, numPackets, interPacketInterval);

  Simulator::ScheduleWithContext (source_tier3_1->GetNode ()->GetId (),
                                  Seconds (4.0), &GenerateTraffic,
                                  source_tier3_1, packetSize, numPackets, interPacketInterval);

  Simulator::ScheduleWithContext (source_tier2_0->GetNode ()->GetId (),
                                  Seconds (5.0), &GenerateTraffic,
                                  source_tier2_0, packetSize, numPackets, interPacketInterval);

  Simulator::ScheduleWithContext (source_tier2_0->GetNode ()->GetId (),
                                  Seconds (6.0), &GenerateTraffic,
                                  source_tier2_0, packetSize, numPackets, interPacketInterval);

  Simulator::ScheduleWithContext (source_tier2_1->GetNode ()->GetId (),
                                  Seconds (7.0), &GenerateTraffic,
                                  source_tier2_1, packetSize, numPackets, interPacketInterval);

  Simulator::ScheduleWithContext (source_tier2_1->GetNode ()->GetId (),
                                  Seconds (8.0), &GenerateTraffic,
                                  source_tier2_1, packetSize, numPackets, interPacketInterval);

  Simulator::ScheduleWithContext (source_tier1_0->GetNode ()->GetId (),
                                  Seconds (9.0), &GenerateTraffic,
                                  source_tier1_0, packetSize, numPackets, interPacketInterval);

  Simulator::Stop (Seconds (10.0));

  /*
  * Tracing is by default enabled to be true and this creates the corresponding tcpdump files.
  */
  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("iot_energy_optimal_topology_example");
      phy.EnablePcap ("iot_energy_optimal_topology_example_gateway", gatewayDevices.Get (0));
      phy.EnablePcap ("iot_energy_optimal_topology_example_devices", iotDevices);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
