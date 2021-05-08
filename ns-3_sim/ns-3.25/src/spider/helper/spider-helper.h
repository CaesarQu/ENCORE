/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
#ifndef SPIDERHELPER_H_
#define SPIDERHELPER_H_

#include "ns3/object-factory.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/ipv4-routing-helper.h"

namespace ns3 {
/**
 * \ingroup spider
 * \brief Helper class that adds spider routing to nodes.
 */
class SpiderHelper : public Ipv4RoutingHelper
{
public:
  SpiderHelper ();

  /**
   * \internal
   * \returns pointer to clone of this OlsrHelper
   *
   * This method is mainly for internal use by the other helpers;
   * clients are expected to free the dynamic memory allocated by this method
   */
  SpiderHelper* Copy (void) const;

  /**
   * \param node the node on which the routing protocol will run
   * \returns a newly-created routing protocol
   *
   * This method will be called by ns3::InternetStackHelper::Install
   *
   * TODO: support installing spider on the subset of all available IP interfaces
   */
  virtual Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const;
  /**
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set.
   *
   * This method controls the attributes of ns3::spider::RoutingProtocol
   */
  void Set (std::string name, const AttributeValue &value);

  void Install (void) const;

private:
  ObjectFactory m_agentFactory;
};

}
#endif /* SPIDERHELPER_H_ */
