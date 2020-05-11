#include <iostream>
#include <cmath>
using namespace std;

int main() {
	int m = 600;
	double eta = 0.8;
	double sigma = 0.99;

	int n = 100000;

	int c = 1000;
	int c_prime;

	int d = 10;

	double f;

	for (c_prime = 1; c_prime <= c; c_prime++) {
		f = log(1 - sigma) / log(1 - pow(eta, 5) * c_prime / c) * (c * log(m) + c_prime * d * m * log(n));
		cout << f << endl;
	}

	// double exp = 0.0;
	// for (int k = 1; k <= 100; k++) {
	// 	double prob = pow(0.7, (k - 1)) * 0.3;
	// 	cout << prob << endl;
	// 	exp += prob * k;
	// }
	// cout << "exp: " << exp << endl;
}