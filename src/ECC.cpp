#include <iostream>
#include "ECC.h"
#include <random>
#include <climits>
#include <map>

using namespace std;

random_device rd;     // only used once to initialise (seed) engine
mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
uniform_int_distribution<int> uni(1,INT_MAX);


Point::Point(number x_, number y_) {
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

Point& Point::operator=(const Point &p) {
	x = p.x, y = p.y; return *this;
}

EllipticCurvePoint SimpleEllipticCurve::add (EllipticCurvePoint &P, EllipticCurvePoint &Q) {
	number x,y,s;
	
	if (P.x != Q.x) {
		s = 1.0*(P.y - Q.y) / (P.x - Q.x);
		x = s*s - (P.x + Q.x);
		y = s*(P.x - x) - P.y;
	}
	else if (P == Q) { //doubling
		s = (3*P.x*P.x + a) / (2*P.y);
		x = s*s - 2*P.x;
		y = s*(P.x - x) - P.y;	
	}
	else if (P.x == Q.x) { //return infinity point instance
		return InfinityPoint::instance();
	}
	return EllipticCurvePoint(x,y,this);
} 

EllipticCurvePoint SimpleEllipticCurve::operator() (number x) {
	return *new EllipticCurvePoint(x,x*x*x + a*x +b,this);
}

//Some number theory
number EllipticCurve::gcd(number a, number b) {
	return a == 0 ? b : gcd(b, b % a);	
}

number EllipticCurve::multiplicativeInverse(number x, number p) {
	//assume p is prime
	//number g = gcd(x,p);
	return fastPowMod(x, p-2, p);
}

number EllipticCurve::fastPowMod(number a, number n, number p) {
	if (n == 0) return 1;
	else if (n == 1) return a;
	number x = fastPowMod(a, n / 2,p) % p;
	return n % 2 == 0 ? x*x % p : a*x*x % p;	
}

EllipticCurvePoint EllipticCurve::add (EllipticCurvePoint &P, EllipticCurvePoint &Q) {
	number x,y,s;
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
	if (x < 0) x += p;	
	if (y < 0) y += p;
	
	
	return EllipticCurvePoint(x,y,this);
} 

EllipticCurvePoint EllipticCurve::operator() (number x) {
	return *new EllipticCurvePoint(x % p,(x*x*x + a*x + b) % p, this);
}

EllipticCurvePoint EllipticCurvePoint::operator+(EllipticCurvePoint other) {
	return E->add(*this,other);
}

EllipticCurvePoint& EllipticCurvePoint::operator+= (EllipticCurvePoint other) {
	*this = *new EllipticCurvePoint(E->add(*this, other));
	return *this;	
}

EllipticCurvePoint& EllipticCurvePoint::operator=(const EllipticCurvePoint &other) {
	x = other.x;
	y = other.y;
	E = other.E;
	return *this;	
}

EllipticCurvePoint EllipticCurvePoint::operator*(number m) {
	EllipticCurvePoint res(x,y,E); //holds our result
	EllipticCurvePoint X(res); // point to double
	m--;
	
	while (m > 0) {
		if (m & 1) res = res + X; 	
		X = X + X;
		m /= 2;
	}
	
	return res;
}

bool EllipticCurvePoint::operator==(const EllipticCurvePoint &other) {return x == other.x && y == other.y;}

SharedKey::SharedKey(number a, number b, number p, EllipticCurvePoint *G_, number n, number h) {
	curve = new EllipticCurve(a,b,p,G_,n,h);
	generator_point = G_; //add assertion 
	generator_point->setCurve(curve);
}

number SharedKey::random() {return uni(rng) % curve->order();}

EllipticCurvePoint* SharedKey::getGeneratorPoint() {return generator_point;}

ECDH::ECDH (SharedKey *s) {
	sh_key = s;
	number x = s->random();
	pr_key = x;
	EllipticCurvePoint Q(*s->getGeneratorPoint()  * x);
	pub_key = Q;
}

EllipticCurvePoint ECDH::encrypt() {return pub_key;}

EllipticCurvePoint ECDH::decrypt(ECDH& other) {return other.encrypt()*pr_key;  }

number ECDH::multiplicativeInverse(number a, number b, number s0 = 1, number s1 = 0)
{
    return b==0 ? s0: ECDH::multiplicativeInverse(b, a%b, s1, s0 - s1*(a/b));
}

pair<number,number> ECDH::signECDSA(number z) {
	number k = sh_key->random();
	EllipticCurvePoint P(*sh_key->getGeneratorPoint() * k);
	number n = sh_key->curve->order();
	number r = P.getX() % n;
	if (r == 0) signECDSA(z);
	else if (r < 0) r += n;
	number t = multiplicativeInverse(k,n);
	if (t < 0) t+=n;
	number s = (t * (z + r*pr_key)) % n;
	if (s == 0) signECDSA(z);
	else if (s < 0) s += n;
	return pair<number,number>(r,s);	
	
}

int ECDH::verify(pair<number,number> signature, number z) {
	number r = signature.first;
	number s = signature.second;
	number n = sh_key->curve->order();
	number t = multiplicativeInverse(s,n);
	if (t < 0) t += n;
	number u1 = (t * z) % n;
	if (u1 < 0) u1 += n;
	number u2 = (t * r) % n;
	if (u2 < 0) u2 += n;
	EllipticCurvePoint P((*sh_key->getGeneratorPoint()*u1) + (pub_key*u2));
	return (P.getX() - r) % n == 0 ? 1 : 0;
}

int main(void) {
	SharedKey *s = new SharedKey(2,2,17,new EllipticCurvePoint(5,1,NULL),17,1);
	ECDH bob(s);
	ECDH alice(s);	
	cout << alice.decrypt(bob) << " " << bob.decrypt(alice) << endl;
	number z = 11;

	cout << alice.verify(bob.signECDSA(z), z);
	
}

