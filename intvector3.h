#include <Urho3D/Container/Str.h>

#ifndef INTVECTOR3_H
#define INTVECTOR3_H


/// Three-dimensional vector with integer values.
class IntVector3
{
public:
    /// Construct a zero vector.
    IntVector3() :
        x_(0),
        y_(0),
        z_(0)
    {
    }

    /// Construct from coordinates.
    IntVector3(int x, int y, int z) :
        x_(x),
        y_(y),
        z_(z)
    {
    }

    /// Construct from an int array.
    IntVector3(const int* data) :
        x_(data[0]),
        y_(data[1]),
        z_(data[2])
    {
    }

    /// Copy-construct from another vector.
    IntVector3(const IntVector3& rhs) :
        x_(rhs.x_),
        y_(rhs.y_),
        z_(rhs.z_)
    {
    }

    /// Assign from another vector.
    IntVector3& operator =(const IntVector3& rhs)
    {
        x_ = rhs.x_;
        y_ = rhs.y_;
        z_ = rhs.z_;
        return *this;
    }

    /// Test for equality with another vector.
    bool operator ==(const IntVector3& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_; }

    /// Test for inequality with another vector.
    bool operator !=(const IntVector3& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_ || z_ != rhs.z_; }

    /// Add a vector.
    IntVector3 operator +(const IntVector3& rhs) const { return IntVector3(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_); }

    /// Return negation.
    IntVector3 operator -() const { return IntVector3(-x_, -y_, -z_); }

    /// Subtract a vector.
    IntVector3 operator -(const IntVector3& rhs) const { return IntVector3(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_); }

    /// Multiply with a scalar.
    IntVector3 operator *(int rhs) const { return IntVector3(x_ * rhs, y_ * rhs, z_ * rhs); }

    /// Divide by a scalar.
    IntVector3 operator /(int rhs) const { return IntVector3(x_ / rhs, y_ / rhs, z_ / rhs); }

    /// Add-assign a vector.
    IntVector3& operator +=(const IntVector3& rhs)
    {
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;
        return *this;
    }

    /// Subtract-assign a vector.
    IntVector3& operator -=(const IntVector3& rhs)
    {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
        z_ -= rhs.z_;
        return *this;
    }

    /// Multiply-assign a scalar.
    IntVector3& operator *=(int rhs)
    {
        x_ *= rhs;
        y_ *= rhs;
        z_ *= rhs;
        return *this;
    }

    /// Divide-assign a scalar.
    IntVector3& operator /=(int rhs)
    {
        x_ /= rhs;
        y_ /= rhs;
        z_ /= rhs;
        return *this;
    }

    /// Return integer data.
    const int* Data() const { return &x_; }

    /// Return as string.
    Urho3D::String ToString() const;

    /// X coordinate.
    int x_;
    /// Y coordinate.
    int y_;
    /// Z coordinate.
    int z_;

    /// Zero vector.
    static const IntVector3 ZERO;
};

/// Multiply IntVector3 with a scalar.
inline IntVector3 operator *(int lhs, const IntVector3& rhs) { return rhs * lhs; }

#endif // INTVECTOR3_H
