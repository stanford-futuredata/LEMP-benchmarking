/**
 * @file ballbound_impl.hpp
 *
 * Bounds that are useful for binary space partitioning trees.
 * Implementation of BallBound ball bound metric policy class.
 *
 * @experimental
 *
 * This file is part of mlpack 1.0.12.
 *
 * mlpack is free software; you may redstribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef __MY_MLPACK_CORE_TREE_BALLBOUND_IMPL_HPP
#define __MY_MLPACK_CORE_TREE_BALLBOUND_IMPL_HPP

// In case it hasn't been included already.
#include "ballbound.hpp"

#include <string>

namespace mips {
namespace bound {

//! Get the range in a certain dimension.
template<typename VecType>
math::Range BallBound<VecType>::operator[](const size_t i) const
{
  if (radius < 0)
    return math::Range();
  else
    return math::Range(center[i] - radius, center[i] + radius);
}

/**
 * Determines if a point is within the bound.
 */
template<typename VecType>
bool BallBound<VecType>::Contains(const VecType& point) const
{
  if (radius < 0)
    return false;
  else
    return metric::EuclideanDistance::Evaluate(center, point) <= radius;
}

/**
 * Gets the center.
 *
 * Don't really use this directly.  This is only here for consistency
 * with DHrectBound, so it can plug in more directly if a "centroid"
 * is needed.
 */
template<typename VecType>
void BallBound<VecType>::CalculateMidpoint(VecType& centroid) const
{
  centroid = center;
}

/**
 * Calculates minimum bound-to-point squared distance.
 */
template<typename VecType>
double BallBound<VecType>::MinDistance(const VecType& point) const
{
  if (radius < 0)
    return DBL_MAX;
  else
    return math::ClampNonNegative(metric::EuclideanDistance::Evaluate(point,
        center) - radius);
}

/**
 * Calculates minimum bound-to-bound squared distance.
 */
template<typename VecType>
double BallBound<VecType>::MinDistance(const BallBound& other) const
{
  if (radius < 0)
    return DBL_MAX;
  else
  {
    double delta = metric::EuclideanDistance::Evaluate(center, other.center)
        - radius - other.radius;
    return math::ClampNonNegative(delta);
  }
}

/**
 * Computes maximum distance.
 */
template<typename VecType>
double BallBound<VecType>::MaxDistance(const VecType& point) const
{
  if (radius < 0)
    return DBL_MAX;
  else
    return metric::EuclideanDistance::Evaluate(point, center) + radius;
}

/**
 * Computes maximum distance.
 */
template<typename VecType>
double BallBound<VecType>::MaxDistance(const BallBound& other) const
{
  if (radius < 0)
    return DBL_MAX;
  else
    return metric::EuclideanDistance::Evaluate(other.center, center) + radius
        + other.radius;
}

/**
 * Calculates minimum and maximum bound-to-bound squared distance.
 *
 * Example: bound1.MinDistanceSq(other) for minimum squared distance.
 */
template<typename VecType>
math::Range BallBound<VecType>::RangeDistance(const VecType& point)
    const
{
  if (radius < 0)
    return math::Range(DBL_MAX, DBL_MAX);
  else
  {
    double dist = metric::EuclideanDistance::Evaluate(center, point);
    return math::Range(math::ClampNonNegative(dist - radius),
                                              dist + radius);
  }
}

template<typename VecType>
math::Range BallBound<VecType>::RangeDistance(
    const BallBound& other) const
{
  if (radius < 0)
    return math::Range(DBL_MAX, DBL_MAX);
  else
  {
    double dist = metric::EuclideanDistance::Evaluate(center, other.center);
    double sumradius = radius + other.radius;
    return math::Range(math::ClampNonNegative(dist - sumradius),
                                              dist + sumradius);
  }
}

/**
 * Expand the bound to include the given bound.
 *
template<typename VecType>
const BallBound<VecType>&
BallBound<VecType>::operator|=(
    const BallBound<VecType>& other)
{
  double dist = metric::EuclideanDistance::Evaluate(center, other);

  // Now expand the radius as necessary.
  if (dist > radius)
    radius = dist;

  return *this;
}*/

/**
 * Expand the bound to include the given point.
 */
template<typename VecType>
template<typename MatType>
const BallBound<VecType>&
BallBound<VecType>::operator|=(const MatType& data)
{
  if (radius < 0)
  {
    center = data.col(0);
    radius = 0;
  }

  // Now iteratively add points.  There is probably a closed-form solution to
  // find the minimum bounding circle, and it is probably faster.
  for (size_t i = 1; i < data.n_cols; ++i)
  {
    double dist = metric::EuclideanDistance::Evaluate(center, (VecType)
        data.col(i)) - radius;

    if (dist > 0)
    {
      // Move (dist / 2) towards the new point and increase radius by
      // (dist / 2).
      arma::vec diff = data.col(i) - center;
      center += 0.5 * diff;
      radius += 0.5 * dist;
    }
  }

  return *this;
}
/**
 * Returns a string representation of this object.
 */
template<typename VecType>
std::string BallBound<VecType>::ToString() const
{
  std::ostringstream convert;
  convert << "BallBound [" << this << "]" << std::endl;
  convert << "Radius:  " << radius << std::endl;
  convert << "Center:  " << std::endl << center;
  return convert.str();
}

}; // namespace bound
}; // namespace mlpack

#endif // __MLPACK_CORE_TREE_DBALLBOUND_IMPL_HPP
