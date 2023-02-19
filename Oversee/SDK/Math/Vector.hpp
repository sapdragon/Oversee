#pragma once
#include <math.h>

class Vector
{
public:
	Vector(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    Vector( ) { this->x = 0; this->y = 0; this->z = 0; };
    Vector(float fx, float fy, float fz)
    {
        this->x = fx;
        this->y = fy;
        this->z = fz;
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }

    Vector operator*(float fl)
    {
        return Vector(x * fl, y * fl, z * fl);
    }

    Vector operator*(const Vector& v)
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }

	__inline void Mul(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}

    float &operator[](int i)
    {
        return ((float*)this)[i];
    }
	
    float operator[](int i) const
    {
        return ((float*)this)[i];
    }

	bool __inline IsZero()
	{
		return (!x && !y && !z);
	}

    void __inline Zero()
    {
        x = y = z = 0.0f;
    }

    bool operator==(const Vector &src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
	
    bool operator!=(const Vector &src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    Vector &operator+=(const Vector &v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
	
    Vector &operator-=(const Vector &v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
	
    Vector &operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
	
    Vector &operator*=(const Vector &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
	
    Vector &operator/=(const Vector &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
	
    Vector &operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
	
    Vector &operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
	
    Vector &operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    void NormalizeInPlace()
    {
        *this = Normalized();
    }
	
    Vector Normalized() const
    {
        Vector res = *this;
        float l = res.Length();
        if(l != 0.0f) {
            res /= l;
        } else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }

	float Normalize() const
	{
		Vector res = *this;
		float l = res.Length();
		if (l != 0.0f)
		{
			res /= l;
		}
		else
		{
			res.x = res.y = res.z = 0.0f;
		}
		return l;
	}

    float DistTo(const Vector &vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
	
    float DistToSqr(const Vector &vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
	
    float Dot(const Vector &vOther) const
    {
        return (x*vOther.x + y*vOther.y + z*vOther.z);
    }

	void VectorCrossProduct(const Vector &a, const Vector &b, Vector &result)
	{
		result.x = a.y*b.z - a.z*b.y;
		result.y = a.z*b.x - a.x*b.z;
		result.z = a.x*b.y - a.y*b.x;
	}

	Vector Cross(const Vector &vOther)
	{
		Vector res;
		VectorCrossProduct(*this, vOther, res);
		return res;
	}
	
    float Length() const
    {
        return sqrt(x*x + y*y + z*z);
    }
	
    float LengthSqr(void) const
    {
        return (x*x + y*y + z*z);
    }
	
    float Length2DSqr(void) const
    {
        return (x * x + y * y );
    }

    float Length2D() const
    {
        return sqrt(x*x + y*y);
    }

    Vector &operator=(const Vector &vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    Vector operator-(void) const
    {
        return Vector(-x, -y, -z);
    }
	
    Vector operator+(const Vector &v) const
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

	Vector operator+(float fl) const
	{
		return Vector(x + fl, y + fl, z + fl);
	}
	
    Vector operator-(const Vector &v) const
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
	
	Vector operator-(float fl) const
	{
		return Vector(x - fl, y - fl, z - fl);
	}

    Vector operator*(float fl) const
    {
        return Vector(x * fl, y * fl, z * fl);
    }
	
    Vector operator*(const Vector &v) const
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }
	
    Vector operator/(float fl) const
    {
        return Vector(x / fl, y / fl, z / fl);
    }
	
    Vector operator/(const Vector &v) const
    {
        return Vector(x / v.x, y / v.y, z / v.z);
    }

	float x;
	float y;
	float z;
};