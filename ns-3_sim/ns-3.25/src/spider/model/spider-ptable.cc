/****************************************************************************/
/* This file is part of SPIDER project.                                       */
/*                                                                          */
/* SPIDER is free software: you can redistribute it and/or modify             */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* SPIDER is distributed in the hope that it will be useful,                  */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with SPIDER.  If not, see <http://www.gnu.org/licenses/>.            */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*  Author:    Dmitrii Chemodanov, University of Missouri-Columbia          */
/*  Title:     SPIDER: AI-augmented Geographic Routing Approach for IoT-based */
/*             Incident-Supporting Applications                             */
/*  Revision:  1.0         6/19/2017                                        */
/****************************************************************************/
#include "spider-ptable.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/energy-module.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include <algorithm>
#include <cfloat>
#include <limits>

NS_LOG_COMPONENT_DEFINE ("SpiderTable");

namespace ns3 {
namespace spider {

/*
 SPIDER position table
 */

PositionTable::PositionTable() {
	m_txErrorCallback = MakeCallback(&PositionTable::ProcessTxError, this);
	m_entryLifeTime = Seconds(0.6); //2.25 for 5 m/s and 0.6 for 20 m/s //FIXME fazer isto parametrizavel de acordo com tempo de hello

}

Time PositionTable::GetEntryUpdateTime(Ipv4Address id) {
	if (id == Ipv4Address::GetZero()) {
		return Time(Seconds(0));
	}
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i = m_table.find(id);
	return i->second.second;
}

/**
 * \brief Adds entry in position table
 */
void PositionTable::AddEntry(Ipv4Address id, Vector position) {
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i = m_table.find(
			id);
	if (i != m_table.end() || id.IsEqual(i->first)) {
		m_table.erase(id);
		m_table.insert(
				std::make_pair(id, std::make_pair(position, Simulator::Now())));
	} else {
		m_table.insert(
				std::make_pair(id, std::make_pair(position, Simulator::Now())));
	}
}

/**
 * \brief Deletes entry in position table and from planarized neighbors
 */
void PositionTable::DeleteEntry(Ipv4Address id) {
	m_table.erase(id);
	//m_planarized_neighbors.erase(id);
}

/**
 * \brief Gets position from position table
 * \param id Ipv4Address to get position from
 * \return Position of that id or NULL if not known
 */
Vector PositionTable::GetPosition(Ipv4Address id) {

	NodeList::Iterator listEnd = NodeList::End();
	for (NodeList::Iterator i = NodeList::Begin(); i != listEnd; i++) {
		Ptr<Node> node = *i;
		if (node->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal() == id) {
			return node->GetObject<MobilityModel>()->GetPosition();
		}
	}
	return PositionTable::GetInvalidPosition();

}

/**
 * \brief Checks if a node is a neighbour
 * \param id Ipv4Address of the node to check
 * \return True if the node is neighbour, false otherwise
 */
bool PositionTable::isNeighbour(Ipv4Address id) {

	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i = m_table.find(
			id);
	if (i != m_table.end() || id.IsEqual(i->first)) {
		return true;
	}

	return false;
}

/**
 * \brief remove entries with expired lifetime
 */
void PositionTable::Purge() {

	if (m_table.empty()) {
		return;
	}

	std::list<Ipv4Address> toErase;

	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i =
			m_table.begin();
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator listEnd =
			m_table.end();

	for (; !(i == listEnd); i++) {

		if (m_entryLifeTime + GetEntryUpdateTime(i->first)
				<= Simulator::Now()) {
			toErase.insert(toErase.begin(), i->first);

		}
	}
	toErase.unique();

	std::list<Ipv4Address>::iterator end = toErase.end();

	for (std::list<Ipv4Address>::iterator it = toErase.begin(); it != end;
			++it) {

		m_table.erase(*it);
//		m_planarized_neighbors.erase(*it);
	}

	m_ip.clear();
	node = NodeContainer::GetGlobal();
	int n=0;
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator j;
	std::map<Ipv4Address, int>::iterator m;
	for (j = m_table.begin(); !(j == m_table.end()); j++)
	{
		m_ip.insert(std::make_pair(j->first,n));
		n++;
		m=m_ip.find(j->first);
		//std::cout<<"Here ["<<n<<"] ="<<(int)m->second<<std::endl;
	}
	

}

/**
 * \brief clears all entries
 */
void PositionTable::Clear() {
	m_table.clear();
	m_planarized_neighbors.clear();
}

void PositionTable::PrintNeighbors(std::ostream &os) {
	Purge();
	os << "Neighbors: ";
	if (m_table.empty()) {
		os << "Neighbor table is empty!";
		return;
	}     //if table is empty (no neighbours)

	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i;
	for (i = m_table.begin(); !(i == m_table.end()); i++) {
		Ipv4Address ip = i->first;
		bool is_planarized = m_planarized_neighbors.find(ip)
				!= m_planarized_neighbors.end();
		Vector pos = i->second.first;
		os << "ip=" << ip << " [" << pos.x << "," << pos.y << ","
				<< is_planarized << "] ";
	}
}

/**
 * \brief Gets next hop according to SPIDER protocol
 * \param position the position of the destination node
 * \param nodePos the position of the node that has the packet
 * \return Ipv4Address of the next hop, Ipv4Address::GetZero () if no nighbour was found in greedy mode
 */
Ipv4Address PositionTable::BestNeighbor(Vector position, Vector nodePos, double lamda) {
	Purge();
	double b_energy=0,b_neighbor=0;
	NodeContainer node = NodeContainer::GetGlobal();
	Ptr<EnergySourceContainer> EnergySourceContainerOnNode = node.Get(0)->GetObject<EnergySourceContainer>();
	Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (EnergySourceContainerOnNode->Get(0));
	Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
	//b_energy=basicRadioModelPtr->GetTotalEnergyConsumption();
	b_energy=basicSourcePtr->GetRemainingEnergy();	
	double b_energy_max =- std::numeric_limits<double>::infinity();// = b_energy;
	double b_energy_min = std::numeric_limits<double>::infinity();// = b_energy;

	double initialDistance = CalculateDistance(nodePos, position);
	
/*
	Ptr<EnergySourceContainer> EnergySourceContainerOnNode1 = node.Get(39)->GetObject<EnergySourceContainer>();
	Ptr<BasicEnergySource> basicSourcePtr1 = DynamicCast<BasicEnergySource> (EnergySourceContainerOnNode1->Get(0));
	Ptr<DeviceEnergyModel> basicRadioModelPtr1 = basicSourcePtr1->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
	std::cout << "Current energy1=" << basicSourcePtr1->GetRemainingEnergy() << " Energy Consumped1=" <<basicRadioModelPtr1->GetTotalEnergyConsumption()<< std::endl;
	std::cout << "Current energy=" << basicSourcePtr->GetRemainingEnergy() << " Energy Consumped=" <<basicRadioModelPtr->GetTotalEnergyConsumption()<< std::endl;
	b_energy=basicRadioModelPtr->GetTotalEnergyConsumption();
*/
        
	if (m_table.empty()) {
		NS_LOG_DEBUG("BestNeighbor table is empty; Position: " << position);
		return Ipv4Address::GetZero();
	}     //if table is empty (no neighbours)
        
	Ipv4Address bestFoundID = Ipv4Address::GetZero();
	m_nextNodes.clear();
	//m_table.begin()->first;
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i=m_table.begin();
	b_neighbor=CalculateDistance(i->second.first, position);
	double maxDistance = - std::numeric_limits<double>::infinity();// =  b_neighbor;
	double minDistance = std::numeric_limits<double>::infinity();//=  b_neighbor;
	double minObj = std::numeric_limits<double>::infinity();// = lamda * b_neighbor + (1-lamda) * b_energy;
	for (i = m_table.begin(); !(i == m_table.end()); i++) {
		b_neighbor=CalculateDistance(i->second.first, position);	
		if (initialDistance > b_neighbor)
		{
			m_nextNodes.insert(
				std::make_pair(i->first, std::make_pair(i->second.first, i->second.second)));
			if(b_neighbor>maxDistance)
			{
				maxDistance = b_neighbor;
			}
			if(b_neighbor<minDistance)
			{
				minDistance = b_neighbor;
			}
			// Compute Energy Remaining
			Ptr<EnergySourceContainer> EnergySourceContainerOnNode = node.Get((int)m_ip.find(i->first)->second)->GetObject<EnergySourceContainer>();
			Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (EnergySourceContainerOnNode->Get(0));
			Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
			//std::cout << "At node =" << i->first << " remaining energy=" << basicSourcePtr->GetRemainingEnergy() << " energy consumption=" <<basicRadioModelPtr->GetTotalEnergyConsumption()<< std::endl;
			b_energy=basicSourcePtr->GetRemainingEnergy();				
			//b_energy=basicRadioModelPtr->GetTotalEnergyConsumption();
			if(b_energy>b_energy_max)
			{
				b_energy_max = b_energy;
			}
			if(b_energy<b_energy_min)
			{
				b_energy_min = b_energy;
			}
		}
	}
	//std::cout<<"At time [" << Simulator::Now().GetSeconds ()<<"]"<<std::endl;
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator j;
	for (j = m_nextNodes.begin(); !(j == m_nextNodes.end()); j++) {
		b_neighbor=CalculateDistance(j->second.first, position);
		// Compute Energy Remaining
		Ptr<EnergySourceContainer> EnergySourceContainerOnNode = node.Get((int)m_ip.find(j->first)->second)->GetObject<EnergySourceContainer>();
		Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (EnergySourceContainerOnNode->Get(0));
		Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
		b_energy=basicSourcePtr->GetRemainingEnergy();	
		//b_energy=basicRadioModelPtr->GetTotalEnergyConsumption();
		// normalization
		b_neighbor = (b_neighbor-minDistance)/(maxDistance-minDistance);
		b_energy = (b_energy-b_energy_min)/(b_energy_max-b_energy_min);
		double Obj = lamda * b_neighbor + (1-lamda) * -b_energy;
		//std::cout<<"At node ["<<m_ip.find(j->first)->second<<"]"<< " objective function = " <<Obj<<std::endl;
		if (minObj > Obj) {
			bestFoundID = j->first;
			minObj = Obj;
		}
	}
	return bestFoundID;	
}

/**
 * \brief Gets next hop according to Electrostatics based Greedy Forwarding
 * \param position the position of the destination node
 * \param nodePos the position of the node that has the packet
 * \return Ipv4Address of the next hop, Ipv4Address::GetZero () if no nighbour was found in Repulsion mode
 */
Ipv4Address PositionTable::ElectrostaticBestNeighbor(Vector position, Vector nodePos,
		double locationX, double locationY, double radius, double lamda) {
	Purge();
	double b_energy=0;
	NodeContainer node = NodeContainer::GetGlobal();
	Ptr<EnergySourceContainer> EnergySourceContainerOnNode = node.Get(0)->GetObject<EnergySourceContainer>();
	Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (EnergySourceContainerOnNode->Get(0));
	Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
	b_energy=basicSourcePtr->GetRemainingEnergy();	
	//b_energy=basicRadioModelPtr->GetTotalEnergyConsumption();
	double b_energy_max =- std::numeric_limits<double>::infinity();//= b_energy;
	double b_energy_min = std::numeric_limits<double>::infinity();//= b_energy;
	double q = 1;
	double n = 2;
	Vector holeC(locationX,locationY,0);
	double holeR = radius;

	//calculate hole charge ql
	double b = CalculateDistance(holeC, position);
	double ql = (q * std::pow(holeR, n + 1)) / (n * std::pow(b + holeR, 2));

	double initPotential = -q / CalculateDistance(nodePos, position)
			+ ql / (std::pow(CalculateDistance(nodePos, holeC), n));


	if (m_table.empty()) {
		NS_LOG_DEBUG("BestNeighbor table is empty; Position: " << position);
		return Ipv4Address::GetZero();
	}     //if table is empty (no neighbours)

	Ipv4Address bestFoundID = Ipv4Address::GetZero();
	m_nextNodes.clear();
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i=m_table.begin();
	double tmpPotential = -q / CalculateDistance(i->second.first, position)
				+ ql / (std::pow(CalculateDistance(i->second.first, holeC), n));
	double maxPotential = - std::numeric_limits<double>::infinity();// = tmpPotential;
	double minPotential = std::numeric_limits<double>::infinity();// = tmpPotential;
	double minObj = std::numeric_limits<double>::infinity();// = lamda * tmpPotential + (1-lamda) * b_energy;

	for (i = m_table.begin(); !(i == m_table.end()); i++) {
		tmpPotential = -q / CalculateDistance(i->second.first, position)
				+ ql / (std::pow(CalculateDistance(i->second.first, holeC), n));	
		if (initPotential > tmpPotential)
		{
			m_nextNodes.insert(
				std::make_pair(i->first, std::make_pair(i->second.first, i->second.second)));
			if(tmpPotential>maxPotential)
			{
				maxPotential = tmpPotential;
			}
			if(tmpPotential<minPotential)
			{
				minPotential = tmpPotential;
			}
			// Compute Energy Remaining
		Ptr<EnergySourceContainer> EnergySourceContainerOnNode = node.Get((int)m_ip.find(i->first)->second)->GetObject<EnergySourceContainer>();
		Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (EnergySourceContainerOnNode->Get(0));
		Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
		b_energy=basicSourcePtr->GetRemainingEnergy();	
		//b_energy=basicRadioModelPtr->GetTotalEnergyConsumption();
			if(b_energy>b_energy_max)
			{
				b_energy_max = b_energy;
			}
			if(b_energy<b_energy_min)
			{
				b_energy_min = b_energy;
			}
		}
	}
	//std::cout<<"At time [" << Simulator::Now().GetSeconds ()<<"]"<<std::endl;
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator j;
	for (j = m_nextNodes.begin(); !(j == m_nextNodes.end()); j++) {
		tmpPotential = -q / CalculateDistance(j->second.first, position)
				+ ql / (std::pow(CalculateDistance(j->second.first, holeC), n));
		// Compute Energy Remaining
		Ptr<EnergySourceContainer> EnergySourceContainerOnNode = node.Get((int)m_ip.find(j->first)->second)->GetObject<EnergySourceContainer>();
		Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (EnergySourceContainerOnNode->Get(0));
		Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
		b_energy=basicSourcePtr->GetRemainingEnergy();	
		//b_energy=basicRadioModelPtr->GetTotalEnergyConsumption();
		// normalization
		tmpPotential = (tmpPotential-minPotential)/(maxPotential-minPotential);
		b_energy = (b_energy-b_energy_min)/(b_energy_max-b_energy_min);
		double Obj = lamda * tmpPotential + (1-lamda) * -b_energy;
		//std::cout<<"At node ["<<m_ip.find(j->first)->second<<"]"<< " objective function = " <<Obj<<std::endl;
		if (minObj > Obj) {
			
			bestFoundID = j->first;
			minObj = Obj;
		}

	}
	return bestFoundID;	
}

/**
 * \brief Gets next hop according to SPIDER recovery-mode protocol (right hand rule)
 * \param previousHop the position of the node that sent the packet to this node
 * \param nodePos the position of the destination node
 * \return Ipv4Address of the next hop, Ipv4Address::GetZero () if no nighbour was found in greedy mode
 */
Ipv4Address PositionTable::BestAngle(Vector previousHop, Vector nodePos) {
	Purge();
	PlanarizeNeighbors(nodePos);

	if (m_table.empty()) {
		NS_LOG_DEBUG("BestNeighbor table is empty; Position: " << nodePos);
		return Ipv4Address::GetZero();
	}     //if table is empty (no neighbours)

	double tmpAngle;
	Ipv4Address bestFoundID = Ipv4Address::GetZero();
	double bestFoundAngle = 360;
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i;

	for (i = m_table.begin(); !(i == m_table.end()); i++) {
		if (m_planarized_neighbors.find(i->first)
				== m_planarized_neighbors.end()) {
			tmpAngle = GetAngle(nodePos, previousHop, i->second.first);
			if (bestFoundAngle > tmpAngle && tmpAngle != 0) {
				bestFoundID = i->first;
				bestFoundAngle = tmpAngle;
			}
		}
	}

	if (bestFoundID == Ipv4Address::GetZero())
	{
		bestFoundID = m_table.begin()->first;
	}

	return bestFoundID;
}

//Gives angle between the vector CentrePos-Refpos to the vector CentrePos-node counterclockwise
double PositionTable::GetAngle(Vector centrePos, Vector refPos, Vector node) {
	double const PI = 4 * atan(1);

	std::complex<double> A = std::complex<double>(centrePos.x, centrePos.y);
	std::complex<double> B = std::complex<double>(node.x, node.y);
	std::complex<double> C = std::complex<double>(refPos.x, refPos.y); //Change B with C if you want angles clockwise

	std::complex<double> AB; //reference edge
	std::complex<double> AC;
	std::complex<double> tmp;
	std::complex<double> tmpCplx;

	std::complex<double> Angle;

	AB = B - A;
	AB = (real(AB) / norm(AB))
			+ (std::complex<double>(0.0, 1.0) * (imag(AB) / norm(AB)));

	AC = C - A;
	AC = (real(AC) / norm(AC))
			+ (std::complex<double>(0.0, 1.0) * (imag(AC) / norm(AC)));

	tmp = log(AC / AB);
	tmpCplx = std::complex<double>(0.0, -1.0);
	Angle = tmp * tmpCplx;
	Angle *= (180 / PI);
	if (real(Angle) < 0)
		Angle = 360 + real(Angle);

	return real(Angle);

}

/**
 * \ProcessTxError
 */
void PositionTable::ProcessTxError(WifiMacHeader const & hdr) {
}

//FIXME ainda preciso disto agr que o LS ja n está aqui???????

/**
 * \brief Returns true if is in search for destionation
 */
bool PositionTable::IsInSearch(Ipv4Address id) {
	return false;
}

bool PositionTable::HasPosition(Ipv4Address id) {
	return true;
}

void PositionTable::PlanarizeNeighbors(Vector nodePos) {
	//check planarization
	/*
	 Sensor u = s;
	 for (Sensor v : u.getNeighbors())
	 for (Sensor w : u.getNeighbors())
	 if (w.equals(v))
	 continue;
	 else if (EuclDist.d(u, v) > Math.max(EuclDist.d(u, w), EuclDist.d(v, w)))
	 {
	 u.addProhibitedNeighbor(v);
	 break;
	 }*/

	m_planarized_neighbors.clear();
	Vector u = nodePos;
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator i;
	std::map<Ipv4Address, std::pair<Vector, Time> >::iterator j;
	for (i = m_table.begin(); !(i == m_table.end()); i++) {
		Vector v = i->second.first;
		for (j = m_table.begin(); !(j == m_table.end()); j++) {
			Vector w = j->second.first;
			if (i->first.IsEqual(j->first)) {
				continue;
			} else if (CalculateDistance(u, v)
					> std::max(CalculateDistance(u, w),
							CalculateDistance(v, w))) {
				m_planarized_neighbors.insert(i->first);
				break;
			}
		}
	}
}

}   // spider
} // ns3
