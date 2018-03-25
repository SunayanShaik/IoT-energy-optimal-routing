/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "iot-energy-optimal-routing-helper.h"
#include "ns3/pointer.h"
#include "ns3/core-module.h"

namespace ns3 {

IotEnergyOptimalRoutingHelper::IotEnergyOptimalRoutingHelper()
 : Ipv4RoutingHelper ()
{
  objectFactory.SetTypeId ("ns3::IotEnergyOptimalRouting");
}

IotEnergyOptimalRoutingHelper* IotEnergyOptimalRoutingHelper::Copy (void) const {
  return new IotEnergyOptimalRoutingHelper (*this);
}

Ptr<Ipv4RoutingProtocol> IotEnergyOptimalRoutingHelper::Create (Ptr<Node> node) const {
  Ptr<IotEnergyOptimalRouting> agent = objectFactory.Create<IotEnergyOptimalRouting> ();
  node->AggregateObject (agent);
  return agent;
}

void IotEnergyOptimalRoutingHelper::Set (std::string name, const AttributeValue &routingTable) {
  objectFactory.Set(name, routingTable);
}

}

