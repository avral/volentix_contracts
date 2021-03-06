#pragma once

#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>
#include <eosio/eosio.hpp>
#include <eosio/transaction.hpp>
#include <string>

#define SYMBOL_PRE_DIGIT 8
#define TOKEN_SYMBOL "VTX"
#define BALANCE_ACC name("vtxstake1111")
#define TOKEN_ACC name("volentixgsys")
#define MIN_STAKE_AMOUNT asset(100000000000, symbol(TOKEN_SYMBOL, SYMBOL_PRE_DIGIT))
#define MAX_STAKE_AMOUNT asset(1000000000000000, symbol(TOKEN_SYMBOL, SYMBOL_PRE_DIGIT))
#define MIN_STAKE_PERIOD 30
#define MAX_STAKE_PERIOD 300
#define STAKE_MULTIPLE_PERIOD 30
#define REWARD_PER 1.1
#define SYMBOL_PRE_F 100000000

using namespace eosio;
using std::string;

class[[eosio::contract]] volentixstak : public contract
{
public:
   using contract::contract;

   [[eosio::on_notify("volentixgsys::transfer")]] void deposit(name from,
                                                               name to,
                                                               asset quantity,
                                                               string memo);

   void stake(name owner, const asset quantity, uint16_t stake_period);
   void registersubs(name owner, uint64_t stake_id);
   void registrglobl(name owner, uint64_t stake_id, asset quantity);
   [[eosio::action]] void unstake(name owner, uint64_t stake_id);
   [[eosio::action]] void clearlck(name owner);
   [[eosio::action]] void clearglobal();
   [[eosio::action]] void addblacklist(const symbol &symbol, name account);
   [[eosio::action]] void rmblacklist(const symbol &symbol, name account);
   [[eosio::action]] void clearamnts(name owner);

private:
   
   struct [[eosio::table]] lck_account
   {
      uint64_t stake_id;
      name account;
      asset stake_amount;
      asset subsidy;
      uint32_t stake_time;
      uint16_t stake_period;

      uint64_t primary_key() const { return stake_id; }
   };

   struct [[eosio::table]] global_amount
   {
      uint64_t stake_id;
      asset stake;
      asset subsidy;
      
      uint64_t primary_key() const { return stake_id; }
   };

   struct [[eosio::table]] total_stake_amount
   {
      asset amount = asset(0, symbol(TOKEN_SYMBOL, SYMBOL_PRE_DIGIT));

      uint64_t primary_key() const { return amount.symbol.code().raw(); }
   };

   struct [[eosio::table]] stake_blacklist
   {
      name account;

      uint64_t primary_key() const { return account.value; }
   };
   typedef eosio::multi_index<name("globalamnts"), global_amount> global_amounts;
   typedef eosio::multi_index<name("accounts"), lck_account> lck_accounts;
   typedef eosio::multi_index<name("stakeamounts"), total_stake_amount> total_stake_amounts;
   typedef eosio::multi_index<name("blacklist"), stake_blacklist> blacklist_table;

   void check_symbol(asset quantity)
   {
      eosio::check( quantity.to_string() != TOKEN_SYMBOL, "Illegal asset symbol");
      check(quantity.is_valid(), "invalid quantity");
      check(quantity.amount > 0, "must be positive quantity");
      check(quantity.symbol == MIN_STAKE_AMOUNT.symbol, "symbol precision mismatch");
   }

   uint32_t stake_period_into_sec(uint16_t stake_period)
   {
      return (60 * 60 * 24 * stake_period);
   }

   void check_blacklist(uint64_t sym_code_raw, name account);
};