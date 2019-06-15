#include <vector>

/*************** UNIMPLEMENTED *******************/

class Noise {
	std::vector<int> permutationVector;
public:
	Noise();
	Noise(unsigned int seed);
	double generateNoise(double x, double y, double z);
private:
	double fadeAlgorithm(double t);
	double lerpAlgorithm(double t, double a, double b);
	double gradientAlgorithm(int hash, double x, double y, double z);
};
