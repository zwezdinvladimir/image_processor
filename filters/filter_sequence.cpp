#include "filter_sequence.h"

FilterSequence::FilterSequence() {}
FilterSequence::FilterSequence(std::vector<std::unique_ptr<Filter>> filters) {
    filters_.reserve(filters.size());
    for (auto &filter : filters) {
        add(std::move(filter));
    }
}
void FilterSequence::add(std::unique_ptr<Filter> filter) {
    if (!filter) {
        throw std::invalid_argument("Filter must not bu nullptr");
    }
    filters_.push_back(std::move(filter));
}
void FilterSequence::apply(Image &image) const {
    for (auto &filter : filters_) {
        filter->apply(image);
    }
}