#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <memory>
namespace paxoslib
{
namespace config
{
class Config;
};
namespace network
{
class Network;
class Peer;
} // namespace network
class Proposal;
class Instance
{
public:
  Instance(const paxoslib::config::Config &oConfig, std::shared_ptr<network::Network> pNetwork);
  uint64_t GetInstanceId() const;
  const Proposal &GetProposal() const;
  uint64_t GetNodeId() const;
  int Propose(const std::string &);
  std::vector<std::shared_ptr<network::Peer>> GetPeers() const;
  ~Instance();

private:
  class InstanceImpl;
  InstanceImpl *pImpl;
};
}; // namespace paxoslib