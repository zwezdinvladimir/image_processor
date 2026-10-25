#pragma once
#include "filter.h"
#include <memory>
#include <type_traits>
#include <vector>

struct FilterSequence : public Filter {
  public:
    FilterSequence();
    FilterSequence(std::vector<std::unique_ptr<Filter>>);
    template <typename... Filters>
        requires(std::is_base_of_v<Filter, Filters> && ...)
    FilterSequence(Filters... filters) {
        filters_.reserve(sizeof...(Filters));
        (filters_.push_back(std::make_unique<Filters>(std::move(filters))), ...);
    }
    void add(std::unique_ptr<Filter>);
    void apply(Image &) const override;

  private:
    std::vector<std::unique_ptr<Filter>> filters_;
};