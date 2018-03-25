/*
 * iot-energy-optimal-route-processor.h
 *
 *  Created on: May 3, 2017
 *      Author: suny
 */

#ifndef IOT_ENERGY_OPTIMAL_ROUTE_PROCESSOR_H_
#define IOT_ENERGY_OPTIMAL_ROUTE_PROCESSOR_H_

#include "ns3/node.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv4-address.h"
#include "ns3/output-stream-wrapper.h"
#include <string>
#include <map>
#include <utility>

namespace ns3 {

/*
*This class is used as helper class to IotEnergyOptimalRouting.
* Actions performed:
* 1. Sets the tiers and energy of the nodes and maitains the energy states after packet transmission.
* 2. Gets the information of which tier this node belongs.
* 3. Gets the Node with highest energy in a tier
* 4. Reduces the energy from total energy after the packet is traversed.
*/
class IotEnergyOptimalRouteProcessor : public Object
{
public:

	IotEnergyOptimalRouteProcessor ();
  virtual ~IotEnergyOptimalRouteProcessor ();

  static TypeId GetTypeId ();

  void AddNodeTierEnergy (uint16_t tier ,Ipv4Address addr , uint32_t energy);

  Ipv4Address GetHighestEnergyNodeInTier (uint16_t tier);
  uint16_t GetTierFromIpAddress(Ipv4Address addr);
  void ReduceNodeEnergyOnTransitHop (Ipv4Address addr);
  void PrintAvailableEnergyOfAllNodes();

private:

  std::map<Ipv4Address, uint32_t> tier1_nodes_energy;
  std::map<Ipv4Address, uint32_t> tier2_nodes_energy;
  std::map<Ipv4Address, uint32_t> tier3_nodes_energy;
};

}



#endif /* IOT_ENERGY_OPTIMAL_ROUTE_PROCESSOR_H_ */
