//Headers
#include <iostream>

#ifndef ELLIPTIC_CURVE_CRYPTOGRAPHY_H
	#define ELLIPTIC_CURVE_CRYPTOGRAPHY_H
#endif

class Point;
class InfinityPoint;
class SimpleEllipticCurve;
class EllipticCurve;
class EllipticCurvePoint;	

class SimpleEllipticCurve { //defines simple EC
	protected:
		long a,b; //parameters
		EllipticCurvePoint *G; //generator point
		long n; //order
	private:
		virtual EllipticCurvePoint add (EllipticCurvePoint &P, EllipticCurvePoint &Q);
	public:
		SimpleEllipticCurve(long a_, long b_) : a(a_), b(b_) {};
		virtual EllipticCurvePoint operator() (long x);
		friend class EllipticCurvePoint;
};

class EllipticCurve : public SimpleEllipticCurve { //Elliptic Curve over Z/pZ
	protected:
		long p;
	private:
		EllipticCurvePoint add (EllipticCurvePoint &P, EllipticCurvePoint &Q);
		static long multiplicativeInverse(long x, long p);
		static long gcd(long a, long b);
		static long fastPowMod(long a, long b, long n);	
		EllipticCurvePoint operator() (long x);
};


class Point {
	protected:
		long x,y;
	public:
		Point(long x_, long y_);
		Point(const Point& p);
		Point operator=(const Point &p);
		virtual Point operator+ (Point &other);
		virtual bool operator== (Point &other);
		friend std::ostream& operator << (std::ostream &out, const Point &p);
};

class EllipticCurvePoint : public Point {
	protected:
		SimpleEllipticCurve *E;
	public:
		EllipticCurvePoint (long x_, long y_, SimpleEllipticCurve *E_) : Point(x_, y_), E(E_) {};
		EllipticCurvePoint operator+(EllipticCurvePoint &other);
		EllipticCurvePoint operator* (long m);
		virtual bool isInfinity() { return false; }
		friend class SimpleEllipticCurve;
		friend class EllipticCurve;
};

//infinity singleton
class InfinityPoint : public EllipticCurvePoint {
	public:
		bool isInfinity() { return true; }
		static InfinityPoint& instance() {
			static InfinityPoint& INFINITY_POINT = *new InfinityPoint();
			return INFINITY_POINT;
		} 
		EllipticCurvePoint operator+(const EllipticCurvePoint &p) {return p;}
		InfinityPoint(const InfinityPoint&) = delete;
		void operator=(const InfinityPoint&) = delete;
	private:
		InfinityPoint() : EllipticCurvePoint(0,0,NULL) {};
};
