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
	EllipticCurvePoint P(*G);
	EllipticCurvePoint Q(*G);
	
	if (P*(a*b) == Q*(b*a)) cout << "Voila!" << endl;
		
}
