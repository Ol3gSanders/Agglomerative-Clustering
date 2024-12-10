#ifndef CLUSTER_H
#define CLUSTER_H

#include <vector>

using namespace std;

class Cluster {

public:
	Cluster();
	Cluster(std::vector<std::vector<double>> points, int index);
	void push_back( const vector<double>& vec );
	void erase( size_t index );
	size_t size() const;
	vector<double>& operator[]( size_t index );
	const vector<double>& operator[]( size_t index ) const;

	int ndim() const {
		return points.size() > 0 ? 2 : 1;
	}


	std::pair<int, int> shape() const {
		return { points.size(), points[0].size() };
	}

	Cluster* mean(int axis = -1) {
		std::vector<double> meanValues(points[0].size(), 0.0);
		for (const auto& point : points) {
			for (size_t i = 0; i < point.size(); ++i) {
				meanValues[i] += point[i];
			}
		}

		for (double& val : meanValues) {
			val /= points.size();
		}

		return new Cluster({ meanValues }, index);
	}

public:
	int index;
	vector<vector<double> > points;
	vector<Cluster> childrens;
};
#endif // CLUSTER_H
