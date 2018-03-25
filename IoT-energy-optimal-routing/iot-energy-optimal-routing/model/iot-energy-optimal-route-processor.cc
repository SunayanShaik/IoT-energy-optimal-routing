/*
 * iot-energy-optimal-route-processor.cc
 *
 *  Created on: Apr 30, 2017
 *      Author: suny
 */

#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/mobility-model.h"
#include <string>
#include <boost/lexical_cast.hpp>
#include "iot-energy-optimal-route-processor.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("IotEnergyOptimalRouteProcessor");

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (IotEnergyOptimalRouteProcessor);

TypeId
IotEnergyOptimalRouteProcessor::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::IotEnergyOptimalRouteProcessor")
    .SetParent<Object> ()
    .AddConstructor<IotEnergyOptimalRouteProcessor> ()
    ;
  return tid;
}

IotEnergyOptimalRouteProcessor::IotEnergyOptimalRouteProcessor ()
{}

IotEnergyOptimalRouteProcessor::~IotEnergyOptimalRouteProcessor ()
{}

/*
* This method add the Tier and energy information of nodes into a Map to maintain the state.
*/
void
IotEnergyOptimalRouteProcessor::AddNodeTierEnergy(uint16_t tier ,Ipv4Address ipv4Addr , uint32_t energy) {
	if(tier == 3) {
		tier3_nodes_energy.insert(std::pair<Ipv4Address,uint32_t>(ipv4Addr,energy));
		NS_LOG_UNCOND("[INFO]   Added Nodes in tier 3 : " << ipv4Addr << " Energy : " << energy);
	}else if (tier == 2) {
		tier2_nodes_energy.insert(std::pair<Ipv4Address,uint32_t>(ipv4Addr,energy));
		NS_LOG_UNCOND("[INFO]   Added Nodes in tier 2 : " << ipv4Addr << " Energy : " << energy);
	} else if (tier == 1) {
		tier1_nodes_energy.insert(std::pair<Ipv4Address,uint32_t>(ipv4Addr,energy));
		NS_LOG_UNCOND("[INFO]   Added Nodes in tier 1 : " << ipv4Addr << " Energy : " << energy);
	}
}

/*
*This methods gets the nodes in a tier with highest energy for Tier 1 and Tier 2.
* Tier 3 we need not calculate because that is the highest tier in implementation and despite of any scenario it should send packets to one of the nodes in tier 2
*/
Ipv4Address
IotEnergyOptimalRouteProcessor::GetHighestEnergyNodeInTier (uint16_t tier) {
	Ipv4Address highEnergyNodeIpAddress;
	if(tier == 1) {
		std::map<Ipv4Address, uint32_t>::iterator it = tier1_nodes_energy.begin();
		uint32_t highEnergyAvailable = 0.0;
		while(it != tier1_nodes_energy.end()) {
			Ipv4Address ipv4Address =  it->first;
			uint32_t energy = it->second;
			if(energy > highEnergyAvailable) {
				highEnergyNodeIpAddress = ipv4Address;
				highEnergyAvailable = energy;
			}
			it++;
		}
	}else if(tier == 2) {
		std::map<Ipv4Address, uint32_t>::iterator it = tier2_nodes_energy.begin();
		uint32_t highEnergyAvailable = 0;
		while(it != tier2_nodes_energy.end()) {
			Ipv4Address ipv4Address =  it->first;
			uint32_t energy = it->second;
			if(energy > highEnergyAvailable) {
				highEnergyNodeIpAddress = ipv4Address;
				highEnergyAvailable = energy;
			}
			it++;
		}
	}
	return highEnergyNodeIpAddress;
}

/*
* This methods takes ipAddress and from the map gets the tier information of that node
*/
uint16_t
IotEnergyOptimalRouteProcessor::GetTierFromIpAddress (Ipv4Address ipAddress) {

	std::map<Ipv4Address, uint32_t>::iterator it_1 = tier1_nodes_energy.begin();
	while(it_1 != tier1_nodes_energy.end()) {
		Ipv4Address ipv4Address =  it_1->first;
		if (ipv4Address == ipAddress) {
			return 1;
		}
		it_1++;
	}

	std::map<Ipv4Address, uint32_t>::iterator it_2 = tier2_nodes_energy.begin();
	while(it_2 != tier2_nodes_energy.end()) {
		Ipv4Address ipv4Address =  it_2->first;
		if (ipv4Address == ipAddress) {
			return 2;
		}
		it_2++;
	}

	std::map<Ipv4Address, uint32_t>::iterator it_3 = tier3_nodes_energy.begin();
	while(it_3 != tier3_nodes_energy.end()) {
		Ipv4Address ipv4Address =  it_3->first;
		if (ipv4Address == ipAddress) {
			return 3;
		}
		it_3++;
	}
   return 0;
}

/*
* Once the Packet has done a hop on any node the energy level of the node is reduced by 10 units and this method takes care of that.
**/
void
IotEnergyOptimalRouteProcessor::ReduceNodeEnergyOnTransitHop (Ipv4Address ipAddress) {

	std::map<Ipv4Address, uint32_t>::iterator it_1 = tier1_nodes_energy.begin();
	while(it_1 != tier1_nodes_energy.end()) {
		Ipv4Address ipv4Address =  it_1->first;
		if (ipv4Address == ipAddress) {
			uint32_t energy = it_1->second;
			it_1->second = energy - 10.0;
		}
		it_1++;
	}

	std::map<Ipv4Address, uint32_t>::iterator it_2 = tier2_nodes_energy.begin();
	while(it_2 != tier2_nodes_energy.end()) {
		Ipv4Address ipv4Address =  it_2->first;
		if (ipv4Address == ipAddress) {
			uint32_t energy = it_2->second;
			it_2->second = energy - 10.0;
		}
		it_2++;
	}

	std::map<Ipv4Address, uint32_t>::iterator it_3 = tier3_nodes_energy.begin();
	while(it_3 != tier3_nodes_energy.end()) {
		Ipv4Address ipv4Address =  it_3->first;
		if (ipv4Address == ipAddress) {
			uint32_t energy = it_3->second;
			it_3->second = energy - 10.0;
		}
		it_3++;
	}
}

/*
*This method prints the amount of Energy that is available in each node
*/
void
IotEnergyOptimalRouteProcessor::PrintAvailableEnergyOfAllNodes() {

	std::map<Ipv4Address, uint32_t>::iterator it_1 = tier1_nodes_energy.begin();
		while(it_1 != tier1_nodes_energy.end()) {
			Ipv4Address ipv4Address =  it_1->first;
			uint32_t energy = it_1->second;
			NS_LOG_UNCOND("[INFO]   Energy remaining-->Tier1-->" << ipv4Address << "-->" << energy);
			it_1++;
		}

	std::map<Ipv4Address, uint32_t>::iterator it_2 = tier2_nodes_energy.begin();
		while(it_2 != tier2_nodes_energy.end()) {
			Ipv4Address ipv4Address =  it_2->first;
			uint32_t energy = it_2->second;
			NS_LOG_UNCOND("[INFO]   Energy remaining-->Tier2-->" << ipv4Address << "-->" << energy);
			it_2++;
		}

	std::map<Ipv4Address, uint32_t>::iterator it_3 = tier3_nodes_energy.begin();
		while(it_3 != tier3_nodes_energy.end()) {
			Ipv4Address ipv4Address =  it_3->first;
			uint32_t energy = it_3->second;
			NS_LOG_UNCOND("[INFO]   Energy remaining-->Tier3-->" << ipv4Address << "-->" << energy);;
			it_3++;
		}
}
}


