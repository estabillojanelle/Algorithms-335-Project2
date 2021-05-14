
///////////////////////////////////////////////////////////////////////////////
// poly_exp.hpp
//
// Definitions for two algorithms that solve the Maximum Subarray Problem,
// and one algorithm that solves the Subset Sum Problem.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <functional>
#include <optional>
#include <vector>
#include <numeric>
using namespace std;
namespace subarray {

// A summed_span represents a non-empty range of indices inside of a vector of
// ints, stored in a begin iterator and end iterator. The class also stores
// the sum of the ints in that range.
//
// Just like in the rest of the C++ standard library, the range includes all
// elements in [begin, end), or in other words the range includes begin, and all
// elements up to BUT NOT INCLUDING end itself.
class summed_span {
public:
  using iterator = std::vector<int>::const_iterator;

private:
  iterator begin_, end_;
  int sum_;

public:

  // Constructor, given the begin iterator, end iterator, and sum of elements
  // in the range. begin must come before end. sum must be the total of all
  // elements in the range. O(1) time.
  summed_span(iterator begin, iterator end, int sum)
  : begin_(begin), end_(end), sum_(sum) {
    assert(begin < end);
  }

  // Constructor, given only the begin and end iterators. The sum is computed
  // in O(n) time.
  summed_span(iterator begin, iterator end)
  : summed_span(begin, end, std::accumulate(begin, end, 0)) {}

  // Equality tests, two spans are equal when each of their iterators are equal.
  bool operator== (const summed_span& rhs) const {
    return (begin_ == rhs.begin_) && (end_ == rhs.end_);
  }

  // Accessors.
  const iterator& begin() const { return begin_; }
  const iterator& end  () const { return end_  ; }
  int sum() const { return sum_; }

  // Compute the number of elements in the span.
  size_t size() const { return end_ - begin_; }

  // Stream insertion operator, so this class is printable.
  friend std::ostream& operator<<(std::ostream& stream, const summed_span& rhs) {
    stream << "summed_span, size=" << rhs.size() << ", sum=" << rhs.sum();
    return stream;
  }
};

// Compute the maximum subarray of input; i.e. the non-empty contiguous span of
// elements with the maximum sum. input must be nonempty. This function uses an
// exhaustive search algorithm that takes O(n^3) time.
summed_span max_subarray_exh(const std::vector<int>& input) {

  assert(!input.empty());
  if(input.size() == 1){
    return summed_span(input.begin(), input.end());
  }
  int sum_best = -9999999;
  int sum = 0;
  // : Rewrite the body of this function so that it actually works. That
  // includes rewriting the return statement. After you do that, delete this
  // comment.
  //  return summed_span(input.begin(), input.begin() + 1);
  int b = 0, e = 1;
  for (int i = 0; i <= input.size() - 2; i++){
      sum = input[i];
      if(sum > sum_best) {
          sum_best= sum;
          b = i;
          e = i;
      }
      for (int j = i+1 ; j <= input.size() -1; j++){
          sum = sum + input[j];
          if (sum > sum_best){
              b = i;
              e = j;
              sum_best = sum;
          }
      }
  }
  std::vector<int>::const_iterator start= input.begin();
  std::vector<int>::const_iterator end= input.begin();
  if (sum_best < input[input.size()-1]) {
    advance(start, input.size()-1); 
    end = input.end();
    return summed_span(start,end);
  }
  advance(start, b); 
  advance(end, e + 1);
  return summed_span(start, end);
}

// Compute the maximum subarray using a decrease-by-half algorithm that takes
// O(n log n) time.

summed_span maximum_subarray_crossing(const std::vector<int>& input, int low, int middle, int high){
    int begin = 0;
    int end = 0;
    std::vector<int>::const_iterator start_ = input.begin();
    std::vector<int>::const_iterator end_ = input.begin();
    int left_sum = -9999999;
    int right_sum = -9999999;
    int sum = 0; 
    for (int i = middle; i >= low; i--) {
        sum += input[i];
        if (sum > left_sum) {
            left_sum = sum;
            begin = i;
        }
    }
    for (int i = middle + 1; i <= high; i++) {
        sum += input[i];
        if (sum > right_sum) {
            right_sum = sum;
            end = i;
        }
    }
    advance(start_, begin);
    advance(end_, end + 1);
    return summed_span(start_, end_);
}
summed_span maximum_subarray_recurse(const std::vector<int>& input, int low, int high) {
    std::vector<int>::const_iterator start_ = input.begin();
    std::vector<int>::const_iterator end_ = input.begin();

    if (low == high) {
        advance(start_, low);
        advance(end_,low + 1);
        return summed_span(start_, end_);
    }
    int middle = (low + high) / 2;
    summed_span entirely_left = maximum_subarray_recurse(input, low, middle);
    summed_span entirely_right = maximum_subarray_recurse(input, middle + 1, high);
    summed_span crossing = maximum_subarray_crossing(input, low, middle, high);
 
    if (entirely_right.sum() > entirely_left.sum() && entirely_right.sum() > crossing.sum()){
        return entirely_right;
    }
    else if (entirely_left.sum() > entirely_right.sum() && entirely_left.sum() > crossing.sum()) {
        return entirely_left;
    }
    else {
        return crossing;
    }
}
summed_span max_subarray_dbh(const std::vector<int>& input) {

  assert(!input.empty());

  // : Rewrite the body of this function so that it actually works. That
  // includes rewriting the return statement. After you do that, delete this
  // comment.
  //  return summed_span(input.begin(), input.begin() + 1);
  return maximum_subarray_recurse(input, 0, input.size() - 1);
}

// Solve the subset sum problem: return a non-empty subset of input that adds
// up to exactly target. If no such subset exists, return an empty optional.
// input must not be empty, and must contain fewer than 64 elements.
// Note that the returned subset must never be empty, even if target == 0.
// This uses an exhaustive search algorithm that takes exponential O(n * 2^n)
// time.
std::optional<std::vector<int>>
subset_sum_exh(const std::vector<int>& input, int target) {
 
  assert(!input.empty());
  assert(input.size() < 64);
  std::vector<int> result;
  int n = input.size();
  int sum_of_elems = 0;;
  for(int i = 0; i <= ((std::pow(2,n))-1); i ++){
    std::vector<int> candidate;
    for (int j = 0; j <= n-1; j++){
      if ( ((i >> j) & 1) == 1 )  {
        candidate.push_back(input[j]);  
      }
      sum_of_elems = std::accumulate(candidate.begin(), candidate.end(), 0);
      if (candidate.size() > 0 &&  sum_of_elems == target){
        return candidate;
      }
    }
  }
  //  Rewrite the body of this function so that it actually works. That
  // includes rewriting the return statement. After you do that, delete this
  // comment.
  return std::nullopt;
 }

}
