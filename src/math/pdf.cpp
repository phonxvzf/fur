#include <iostream>
#include <algorithm>

#include "math/pdf.hpp"

int upper_bound_float_recursive(
    const std::vector<std::pair<Float,int>>& v,
    Float x,
    int start,
    int end,
    int nearest)
{
  const int mid = (start + end) >> 1;
  if (start == end) return nearest;
  if (math::COMPARE_EQ(v[mid].first, x)) return mid;
  if ((v[mid].first > x) && (v[mid].first < v[nearest].first)) nearest = mid;
  if (x < v[mid].first) return upper_bound_float_recursive(v, x, start, mid, nearest);
  return upper_bound_float_recursive(v, x, mid + 1, end, nearest);
}

int upper_bound_float(const std::vector<std::pair<Float,int>>& sv, Float x) {
  int index = upper_bound_float_recursive(sv, x, 0, sv.size(), sv.size() - 1);
  if (index < 0) return index;
  return sv[index].second;
}

namespace math {

  pdf1d::pdf1d(int n_samples, Float from, Float to)
    : n_samples(n_samples), from(from), to(to), dx((to - from) / n_samples)
  {
    pdf.resize(n_samples);
    cdf.resize(n_samples);
  }

  void pdf1d::init_cdf() {
    Float cumulative_pdf = 0;
    for (int i = 0; i < n_samples; ++i) {
      cdf[i] = cumulative_pdf + pdf[i] * dx;
      cumulative_pdf = cdf[i];
    }
    for (int i = 0; i < n_samples; ++i) {
      cdf_sorted.emplace_back(cdf[i], i);
    }
    std::sort(cdf_sorted.begin(), cdf_sorted.end());
  }

  Float pdf1d::sample(Float u, Float offset) const {
    int index = upper_bound_float(cdf_sorted, u);
    if (index < 0) return 0;
    return offset + dx * index;
  }
      
  Float pdf1d::sample(Float u) const {
    return sample(u, from);
  }

}
