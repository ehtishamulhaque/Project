#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <string>

using namespace eosio;
using std::string;

class [[eosio::contract]] ipfs_contract : public contract {
public:
    ipfs_contract(name receiver, name code, datastream<const char*> ds) 
        : contract(receiver, code, ds), files(receiver, receiver.value) {}

    [[eosio::action]]
    void store(string filename, string ipfs_hash) {
        require_auth(get_self());


        auto itr = files.find(hash(filename));
        check(itr == files.end(), "File already exists");

        files.emplace(get_self(), [&](auto& f) {
            f.id = files.available_primary_key();
            f.filename = filename;
            f.ipfs_hash = ipfs_hash;
        });
    }

    [[eosio::action]]
    void retrieve(string filename) {
        auto itr = files.find(hash(filename));
        check(itr != files.end(), "File not found");

        string ipfs_hash = itr->ipfs_hash;

        print(ipfs_hash);
    }

private:
    struct [[eosio::table]] file {
        uint64_t id;
        string filename;
        string ipfs_hash;

        uint64_t primary_key() const { return id; }
        uint64_t by_filename() const { return hash(filename); }
    };

    typedef eosio::multi_index<name("files"), file,
        indexed_by<name("filename"), const_mem_fun<file, uint64_t, &file::by_filename>>
    > file_table;

    file_table files;

    static uint64_t hash(const string& s) {
        return eosio::sha256(s.c_str(), s.length()).extract_as_byte_array()[0];
    }
};