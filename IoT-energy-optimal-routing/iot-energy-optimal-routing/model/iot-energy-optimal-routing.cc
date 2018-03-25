/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "iot-energy-optimal-routing.h"
#include "iot-energy-optimal-route-processor.h"
#include <list>
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/log.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-route.h"
#include "ns3/pointer.h"
#include "ns3/double.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/core-module.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("IotEnergyOptimalRouting");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (IotEnergyOptimalRouting);

/*
*The RoutingProcessor Attribute is used to set the IotEnergyOptimalProcessor object created in example
* The Methods to watch out are 
* 1. RouteOutput (For originating packets)
* 2. RouteInput (For forwarding packets)
*/
TypeId IotEnergyOptimalRouting::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::IotEnergyOptimalRouting")
    .SetParent<Ipv4RoutingProtocol> ()
    .AddConstructor<IotEnergyOptimalRouting> ()
    .AddAttribute ("RoutingProcessor", "Pointer to Route processor class.",
                   PointerValue (),
                   MakePointerAccessor (&IotEnergyOptimalRouting::SetRouteProcessor),
                   MakePointerChecker<IotEnergyOptimalRouteProcessor> ());
  return tid;
}

/*
*This method is called when packets are generated in that node.(When node is a sink)
* It does the following actions:
* --> Finds the Tier which this node belongs.
* --> Forwards the packet to a node in the downstream tier with highest energy. (If node belogs to Tier 1 it directly forwards packet to the destination gateway sink)
* --> Prints the amount of energy remaining in the nodes after the packet is transmitted.
*/
Ptr<Ipv4Route> 
IotEnergyOptimalRouting::RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, enum Socket::SocketErrno &sockerr) 
{
	Ptr<Ipv4Route> route = Create<Ipv4Route> ();
	uint16_t tier = routeProcessor->GetTierFromIpAddress(localIpAddress);
//	NS_LOG_UNCOND ("[INFO]   Packet Originated Node Source: " << localIpAddress  << " Node Tier: " << tier << " Destination : " << header.GetDestination ());
	Ipv4Address gatewayAddress;
	if(tier == 1) {
		gatewayAddress = dest_gateway_address;
	} else {
		gatewayAddress = routeProcessor->GetHighestEnergyNodeInTier(tier-1);
	}

	route->SetGateway(gatewayAddress);
	route->SetSource(localIpAddress);
	route->SetDestination(header.GetDestination());
	route->SetOutputDevice (m_ipv4->GetNetDevice (interfaceId));
	routeProcessor->ReduceNodeEnergyOnTransitHop(localIpAddress);
    NS_LOG_UNCOND ("[INFO]   Packet Originated Node Source:" << localIpAddress << " Node Tier: " << tier << "  Destination:" << header.GetDestination () << "  Next Hop:" << gatewayAddress << "  Next Tier:" << tier-1);
	routeProcessor->PrintAvailableEnergyOfAllNodes();
	sockerr = Socket::ERROR_NOTERROR;
	return route;
}

/*
* This method is called when packets are arrived to be forwared. (Not originating in this node)
*It does the following actions:
* --> Finds the Tier which this node belongs.
* --> Forwards the packet to a node in the downstream tier with highest energy. (If node belogs to Tier 1 it directly forwards packet to the destination gateway sink)
* --> Prints the amount of energy remaining in the nodes after the packet is transmitted.
*/
bool 
IotEnergyOptimalRouting::RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
                             UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                             LocalDeliverCallback lcb, ErrorCallback ecb) 
{
	if(header.GetDestination() == localIpAddress) {
		routeProcessor->ReduceNodeEnergyOnTransitHop(localIpAddress);
		routeProcessor->PrintAvailableEnergyOfAllNodes();
		NS_LOG_UNCOND ("[INFO]   Packet reached destination " << localIpAddress);
		lcb (p, header, m_ipv4->GetInterfaceForDevice (idev));
		return true;
	} else {
		Ptr<Ipv4Route> route = Create<Ipv4Route> ();
		uint16_t tier = routeProcessor->GetTierFromIpAddress(localIpAddress);
		Ipv4Address gatewayAddress;
		if(tier == 1) {
			gatewayAddress = dest_gateway_address;
		} else {
			gatewayAddress = routeProcessor->GetHighestEnergyNodeInTier(tier-1);
		}
		route->SetGateway(gatewayAddress);
		route->SetSource(header.GetSource());
		route->SetDestination(header.GetDestination());
		route->SetOutputDevice (m_ipv4->GetNetDevice (interfaceId));
		routeProcessor->ReduceNodeEnergyOnTransitHop(localIpAddress);
		NS_LOG_UNCOND ("[INFO]   Forwarding Packet from Node:" << localIpAddress << "  Source:" << header.GetSource () << "  Destination:" << header.GetDestination () <<  "  Next Hop:" << gatewayAddress << "  Next Tier:" << tier-1);
		routeProcessor->PrintAvailableEnergyOfAllNodes();
		ucb (route, p, header);
		return true;
	}

	return false;
}

/*
* All the Remaining functions are default virtual functions or constructors or destructors or any helper functions to set the variables.
*/
IotEnergyOptimalRouting::IotEnergyOptimalRouting () {
  interfaceId = 32;
  dest_gateway_address = Ipv4Address("10.1.3.1");
  NS_LOG_FUNCTION_NOARGS ();
}

IotEnergyOptimalRouting::~IotEnergyOptimalRouting () {
  NS_LOG_FUNCTION_NOARGS ();
}

void IotEnergyOptimalRouting::NotifyInterfaceUp (uint32_t interface) {
  NS_LOG_FUNCTION (this << interface);
}

void IotEnergyOptimalRouting::NotifyInterfaceDown (uint32_t interface) {
  NS_LOG_FUNCTION (this << interface);
}

void IotEnergyOptimalRouting::NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address) {
  interfaceId = interface;
  localIpAddress = address.GetLocal ();
}

void IotEnergyOptimalRouting::NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address) {
  NS_LOG_FUNCTION(this << interface << address);
}

void IotEnergyOptimalRouting::SetIpv4 (Ptr<Ipv4> ipv4) {
  NS_LOG_FUNCTION(this << ipv4);
  m_ipv4 = ipv4;
}

void IotEnergyOptimalRouting::PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit) const {
  *stream->GetStream () << "Node: " << m_ipv4->GetObject<Node> ()->GetId ();
}

void IotEnergyOptimalRouting::SetRouteProcessor (Ptr<IotEnergyOptimalRouteProcessor> p)
{
  NS_LOG_FUNCTION(p);
  routeProcessor = p;
}
}

