#include <atomic>
#include "paxoslib/role.hpp"
#include "paxoslib/instance.hpp"
#include <spdlog/spdlog.h>

std::atomic<uint64_t> g_msgid{1};
namespace paxoslib::role
{
Role::Role(Instance *pInstance)
{
  m_pInstance = pInstance;
}
void Role::Broadcast(BroadcastReceiverType oReceiverType, BroadcastType oBroadcastType, const Message &oMessage)
{
  Message oNewMessage{oMessage};
  oNewMessage.set_id(g_msgid++);
  oNewMessage.set_from_node_id(this->m_pInstance->GetNodeId());
  SPDLOG_DEBUG("Broadcast {}", oNewMessage.ShortDebugString());
  const std::map<BroadcastReceiverType, std::set<RoleType>> mapReceiver{
      {BroadcastReceiverType::BORADCAST_RECEIVER_TYPE_ACCEPTER, {RoleType::ROLE_TYPE_ACCEPTER}}, {BroadcastReceiverType::BORADCAST_RECEIVER_TYPE_PROPOSER, {RoleType::ROLE_TYPE_PROPOSER}}, {BroadcastReceiverType::BORADCAST_RECEIVER_TYPE_LEARNER, {RoleType::ROLE_TYPE_LEARNER}}};
  uint64_t mask = 0;
  for (auto a : mapReceiver.at(oReceiverType))
  {
    mask = mask | (1 << a);
  }
  // if (oBroadcastType == BroadcastType::BROAD_CAST_TYPE_SELF_THEN_OTHERS)
  // {
  //   oNewMessage.set_to_node_id(this->m_pInstance->GetNodeId());

  //   //this->m_pInstance->OnMessage(oNewMessage);
  //   //wait notify
  // }
  for (auto pPeer : m_pInstance->GetPeers())
  {
    if (pPeer->GetRoleTypesMask() & mask)
    {
      oNewMessage.set_to_node_id(pPeer->GetPeerID());
      pPeer->SendMessage(oNewMessage);
    }
  }
  // if (oBroadcastType == BroadcastType::BROAD_CAST_TYPE_ALL)
  // {
  //   oNewMessage.set_to_node_id(this->m_pInstance->GetNodeId());
  //   //this->m_pInstance->OnMessage(oNewMessage);
  // }
}
int Role::OnMessage(const Message &oMessage)
{
  std::lock_guard<std::mutex> oGurad{m_oMessageMutex};
  this->OnReceiveMessage(oMessage);
}
void Role::SendMessageTo(uint32_t dwNodeId, const Message &oMessage)
{
  Message oNewMessage{oMessage};
  oNewMessage.set_id(g_msgid++);
  for (auto pPeer : m_pInstance->GetPeers())
  {
    if (pPeer->GetPeerID() == dwNodeId)
    {
      oNewMessage.set_from_node_id(this->m_pInstance->GetNodeId());
      oNewMessage.set_to_node_id(dwNodeId);
      pPeer->SendMessage(oNewMessage);
    }
  }
}
}; // namespace paxoslib::role
