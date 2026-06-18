#include <costcat/costcat.hpp>
#include <nlohmann/json.hpp>

void nathcat::cost::to_json(nlohmann::json &j,
                            const struct transaction_request &t) {
  j = nlohmann::json{{"amount", t.amount},
                     {"payees", t.payees},
                     {"description", t.description}};
}

void nathcat::cost::from_json(const nlohmann::json &j,
                              struct transaction_request &t) {
  j.at("amount").get_to(t.amount);
  j.at("payees").get_to(t.payees);
  j.at("description").get_to(t.description);
}
