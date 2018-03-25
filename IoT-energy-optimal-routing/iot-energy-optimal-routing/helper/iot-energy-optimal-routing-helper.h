/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef IOT_ENERGY_OPTIMAL_ROUTING_HELPER_H
#define IOT_ENERGY_OPTIMAL_ROUTING_HELPER_H

#include "ns3/iot-energy-optimal-routing.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/object-factory.h"
#include "ns3/node.h"

namespace ns3 {
class IotEnergyOptimalRoutingHelper : public Ipv4RoutingHelper {
public:
	IotEnergyOptimalRoutingHelper();

	IotEnergyOptimalRoutingHelper* Copy (void) const;

  virtual Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const;

  void Set (std::string name, const AttributeValue &attributeValue);

private:
  ObjectFactory objectFactory;
};
}

#endif /* IOT_ENERGY_OPTIMAL_ROUTING_HELPER_H */

