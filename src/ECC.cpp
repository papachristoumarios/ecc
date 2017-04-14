#include <iostream>
#include "ECC.h"

using namespace std;

Point::Point(long x_, long y_) {
	x = x_; y = y_;
}

bool Point::operator==(Point &other) {
	return (x == other.x) && (y == other.y);
}

Point Point::operator+(Point &other) {
	return *new Point(x+other.x, y+other.y);
}

ostream& operator << (ostream &out, const Point &p) {
	out << "(" << p.x << "," << p.y << ")";
	return out;
}

Point::Point(const Point &p) {
	x = p.x; y = p.y;	
}

Point Point::operator=(const Point &p) {
	return *new Point(p.x, p.y);	
}

EllipticCurvePoint SimpleEllipticCurve::add (EllipticCurvePoint &P, EllipticCurvePoint &Q) {
	long x,y;
	
	if (P.x != Q.x) {
		float s = 1.0*(P.y - Q.y) / (P.x - Q.x);
		x = s*s - (P.x + Q.x);
		y = s*(P.x - x) - P.y;
	}
	else if (P == Q) { //doubling
		float s = (3*P.x*P.x + a) / (2*P.y);
		x = s*s - 2*P.x;
		y = s*(P.x - x) - P.y;	
	}
	else if (P.x == Q.x) { //return infinity point instance
		return InfinityPoint::instance();
	}
	return EllipticCurvePoint(x,y,this);
} 

EllipticCurvePoint SimpleEllipticCurve::operator() (long x) {
	return *new EllipticCurvePoint(x,x*x*x + a*x +b,this);
}

//Some number theory
long EllipticCurve::gcd(long a, long b) {
	return a == 0 ? b : gcd(b, b % a);	
}

long EllipticCurve::multiplicativeInverse(long x, long p) {
	//assume p is prime
	long g = gcd(x,p);
	if (g != 1) return -1;
	return fastPowMod(x, p-2, p);
}

long EllipticCurve::fastPowMod(long a, long n, long p) {
	if (n == 0) return 1;
	else if (n == 1) return a;
	long x = fastPowMod(a, n / 2,p) % p;
	return n % 2 == 0 ? x*x % p : a*x*x % p;	
}

EllipticCurvePoint EllipticCurve::add (EllipticCurvePoint &P, EllipticCurvePoint &Q) {
	long x,y,s;
	if (P.isInfinity()) return Q;
	if (Q.isInfinity()) return P;
	if (P.x != Q.x) {
		s = ((P.y - Q.y) * multiplicativeInverse(P.x - Q.x, p)) % p;
		x = (s*s - (P.x + Q.x)) % p;
		y = (s*(P.x - x) - P.y) % p;
	}
	else if (P == Q) { //doubling
		s = ((3*P.x*P.x + a) * multiplicativeInverse(2*P.y, p)) % p;
		x = (s*s - 2*P.x) % p;
		y = (s*(P.x - x) - P.y) % p;	
	}
	else if (P.x == Q.x) {
		return InfinityPoint::instance();
	}
	return EllipticCurvePoint(x,y,this);
} 

EllipticCurvePoint EllipticCurve::operator() (long x) {
	return *new EllipticCurvePoint(x % p,(x*x*x + a*x + b) % p, this);
}

EllipticCurvePoint EllipticCurvePoint::operator+(EllipticCurvePoint &other) {
	return E->add(*this,other);
}

EllipticCurvePoint EllipticCurvePoint::operator*(long m) {
	EllipticCurvePoint Q(x,y,E);
	EllipticCurvePoint R(Q);
	
	while (m / 2 != 0) {
		Q = Q + Q;	
		if (m % 2 == 1) Q = Q + R;
		m /= 2;
	}
	return Q;
}

int main(void) {
	Point a(10,10);
	Point b(20,20);
	cout << a + b << endl;
}
