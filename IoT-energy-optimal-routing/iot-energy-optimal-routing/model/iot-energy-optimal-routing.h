/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef IOT_ENERGY_OPTIMAL_ROUTING_H
#define IOT_ENERGY_OPTIMAL_ROUTING_H

#include <list>
#include "ns3/ipv4-routing-protocol.h"
#include "iot-energy-optimal-route-processor.h"

namespace ns3 {
/*
*This is the main class which implements Ipv4RoutingProtocol and is used by nodes to route packets to next nodes
*/
class IotEnergyOptimalRouting : public Ipv4RoutingProtocol
{
public:
  static TypeId GetTypeId (void);

  IotEnergyOptimalRouting();
  virtual ~IotEnergyOptimalRouting();

  virtual Ptr<Ipv4Route> RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);

  virtual bool RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
                           UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                           LocalDeliverCallback lcb, ErrorCallback ecb);
  virtual void NotifyInterfaceUp (uint32_t interface);
  virtual void NotifyInterfaceDown (uint32_t interface);
  virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void SetIpv4 (Ptr<Ipv4> ipv4);
  virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit) const;
  void SetRouteProcessor (Ptr<IotEnergyOptimalRouteProcessor> p);

protected:
private:
  Ptr<IotEnergyOptimalRouteProcessor> routeProcessor;
  Ipv4Address localIpAddress;
  Ipv4Address dest_gateway_address;
  Ptr<Ipv4> m_ipv4;
  uint32_t interfaceId;
};

} //namespace ns3

#endif /* IOT_ENERGY_OPTIMAL_ROUTING_H */

