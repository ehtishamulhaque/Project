//This SC is designed to allow multiple edge nodes to execute the "runiot" action in parallel.
// The consensus process involving the 25 consensus nodes.

#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>

using namespace eosio;

class [[eosio::contract]] iotcontract : public contract {
public:
    using contract::contract;

    [[eosio::action]]
    void runiot(uint64_t edge_node_id, uint64_t num_iterations) {
        require_auth(get_self());

        for (uint64_t i = 0; i < num_iterations; ++i) {
            // simulate IoT node reading
            uint64_t sensor_value = read_sensor();

            // process the node value
            uint64_t processed_value = process_node_value(node_value);

            // store the processed value on the BC
            store_value(edge_node_id, i + 1, processed_value);

            eosio::print("Edge Node ", edge_node_id, " IoT operation ", i + 1, "/", num_iterations, " completed with value: ", processed_value, "\n");
        }

        eosio::print("Edge Node ", edge_node_id, " IoT node test completed successfully.");
    }

private:
    // simulated node reading fn
    uint64_t read_node() {
        
        return current_time_point().sec_since_epoch() % 100;
    }

    // simulated node value processing fn
    uint64_t process_node_value(uint64_t value) {
        return value * 2;
    }

    // fn to store the processed value on the BC
    void store_value(uint64_t edge_node_id, uint64_t operation_id, uint64_t value) {
        values_table values(get_self(), get_self().value);

        values.emplace(get_self(), [&](auto& row) {
            row.edge_node_id = edge_node_id;
            row.operation_id = operation_id;
            row.value = value;
        });
    }


    struct [[eosio::table]] value_row {
        uint64_t edge_node_id;
        uint64_t operation_id;
        uint64_t value;

        uint64_t primary_key() const { return (edge_node_id << 32) | operation_id; }
    };

    using values_table = eosio::multi_index<"values"_n, value_row>;
};

EOSIO_DISPATCH(iotcontract, (runiot))
