# ecc
A simple implementation of Elliptic Curves on Z/pZ in C++. 

## Usage

In order to create a curve on Z/pZ you should choose its parameters a,b,p, a generator point G its order n and cofactor h. For demonstration purposes we shall choose a = 2, b = 2, p = 17, G = (5,1), n = 17, h = 1: 

``` C++
EllipticCurvePoint *G = new EllipticCurvePoint(5,1,NULL);
EllipticCurve *E = new EllipticCurve(2,2,17,G,17,1);
G->setCurve(E);
```
Now we can pick any point on E by calculating multiples of G:

```C++
EllipticCurvePoint R = (*G)*5;
```

We can also add points on E:

```C++
EllipticCurvePoint R = (*G)*5;
EllipticCurvePoint P = (*G)*7;
EllipticCurvePoint Q = P + R;
cout << R << endl;
cout << P << endl;
cout << Q << endl;
```

which would output:

```C++
(9,16)
(0,6)
(0,11)
```

## References
[1] [Elliptic Curve Cryptography Lemma on Wikipedia](https://en.wikipedia.org/wiki/Elliptic_curve_cryptography)
