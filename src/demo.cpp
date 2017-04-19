#include <iostream>
#include "ECC.cpp"

using namespace std;

int main(void) {
	number a = 17; //Alice's private key
	number b = 42; //Bob's private key
	EllipticCurvePoint *G = new EllipticCurvePoint(5,1,NULL);
	EllipticCurve *E = new EllipticCurve(2,2,17,G,17,1);
	G->setCurve(E);
	cout << "Generator Point: " << *G << endl;
EllipticCurvePoint R = (*G)*5;
EllipticCurvePoint P = (*G)*7;
EllipticCurvePoint Q = P + R;
cout << R << endl;
cout << P << endl;
cout << Q << endl;		
}
