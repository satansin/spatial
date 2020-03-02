#ifndef __OTREE_H
#define __OTREE_H

#include "TriMesh.h"
#include "point.h"
#include "pcr_adv.h"
#include <iostream>
#include <unordered_map>
#include <assert.h>

using namespace std;
using namespace trimesh;

struct OTreeNode {
	float mLoc[3];
	float mSl;

	int mLocID[3];

	int mLevel;

	vector<int> mArrPt;

	OTreeNode* mChildren[8];
	int mOccupy;

	OTreeNode(float loc[3], float sl, int locID[3], int level) {
		for (int i = 0; i < 3; i++) {
			mLoc[i] = loc[i];
		}
		mSl = sl;
		for (int i = 0; i < 3; i++) {
			mLocID[i] = locID[i];
		}
		mLevel = level;
		mOccupy = 0;
	}

	void addPt(int pid) {
		mArrPt.push_back(pid);
	}

	void addChild(OTreeNode* child) {
		assert(mOccupy <= 7);
		mChildren[mOccupy++] = child;
	}

	bool isLeaf() {
		return (mLevel == 0);
	}

	string toString() {
		return "(" + to_string(mLoc[0]) + ", " + to_string(mLoc[1]) + ", " + to_string(mLoc[2]) + "), " +
			   "side length = " + to_string(mSl) + ", " +
			   "(" + to_string(mLocID[0]) + ", " + to_string(mLocID[1]) + ", " + to_string(mLocID[2]) + "), " +
			   "level = " + to_string(mLevel) + ", " +
			   "# pts = " + to_string(mArrPt.size()) + ", " +
			   "# children = " + to_string(mOccupy);
	}
};

class OTree {
private:
	OTreeNode* mRoot;
	float mGridSize;

	long long getNumberHash(int* id) {
	    long long hash;
	    int sum1 = id[0] + id[1];
	    hash = sum1 * (sum1 + 1) / 2 + id[1];
	    long long sum2 = hash + id[2];
	    hash = sum2 * (sum2 + 1) / 2 + id[2];
	    return hash;
	}

	void getLoc(int box[3], float loc[3]) {
		for (int i = 0; i < 3; i++) {
			loc[i] = mRoot->mLoc[i] + (float) box[i] * mGridSize;
		}
	}

	void freeNode(OTreeNode* node) {
		if (!node->isLeaf()) {
			for (int i = 0; i < node->mOccupy; i++) {
				freeNode(node->mChildren[i]);
			}
		}
		delete node;
	}

	bool intersectingAnnulus(OTreeNode* node, Pt3D p, float dist, float eps) {
		double convP[3] = { p.x, p.y, p.z };
		for (int i = 0; i < 3; i++) {
			if (node->mLoc[i] > convP[i] + dist + eps + 0.001 || node->mLoc[i] + node->mSl < convP[i] - dist - eps - 0.001) {
				return false; // node box is outside the annulus
			}
		}
		bool contain = true;
		float maxDist = 0.0;
		for (int i = 0; i < 3; i++) {
			float mid = node->mLoc[i] + node->mSl / 2.0;
			if (convP[i] < mid) {
				maxDist += sq(node->mLoc[i] + node->mSl - convP[i]);
			} else {
				maxDist += sq(convP[i] - node->mLoc[i]);
			}
		}
		maxDist = sqrt(maxDist);
		if (maxDist < dist - eps - 0.001) {
			return false;
		} else {
			return true;
		}
	}

	void intersectingPairNode(OTreeNode* node, Pt3D p, float dist, float eps, TriMesh* mesh, vector<int>& ret) {
		// cout << "Accessing node of level " << node->mLevel << endl;
		if (node->isLeaf()) {
			for (auto &i: node->mArrPt) {
				auto iDist = eucl_dist(p, pt(mesh->vertices[i]));
				if (abs(iDist - dist) <= eps) {
					ret.push_back(i);
				}
			}
		} else {
			for (int i = 0; i < node->mOccupy; i++) {
				auto child = node->mChildren[i];
				// cout << "Testing child: " << child->toString() << endl;
				if (intersectingAnnulus(child, p, dist, eps)) {
					intersectingPairNode(child, p, dist, eps, mesh, ret);
				}
			}
		}
	}
public:
	void intersectingPair(Pt3D p, float dist, float eps, TriMesh* mesh, vector<int>& ret) {
		// cout << "(" << p[0] << ", " << p[1] << ", " << p[2] << "), " << dist << ", " << eps << endl;
		intersectingPairNode(mRoot, p, dist, eps, mesh, ret);
	}

	void buildFromMesh(TriMesh* mesh, float err, bool verbose = false) {
		mesh->need_bbox();
		auto meshBox = mesh->bbox;

		int n = mesh->vertices.size();
		if (verbose) cout << "Size of DB: " << n << endl;

		// span: original span of each dimension
		float span[3];
		float maxSpan = 0.0;

		for (int i = 0; i < 3; i++) {
			span[i] = meshBox.max[i] - meshBox.min[i];
			if (span[i] > maxSpan) {
				maxSpan = span[i];
			}
		}
		if (verbose) cout << "Max span: " << maxSpan << endl;

		// extend original span to a cube of maxSpan defined by ocBoxMin
		float ocBoxMin[3];
		for (int i = 0; i < 3; i++) {
			if (span[i] < maxSpan) {
				ocBoxMin[i] = (meshBox.min[i] + meshBox.max[i] - maxSpan) / 2.0;
			} else {
				ocBoxMin[i] = meshBox.min[i];
			}
		}
		if (verbose)
			cout << "Extended box min:\n  x = " << ocBoxMin[0]
				 << ", y = " << ocBoxMin[1]
				 << ", z = " << ocBoxMin[2] << endl;

		// level of leaf = 0, level of root = l
		// l is the maximum level
		int l = (int) ceil(log2(maxSpan / err));
		cout << "Max level: " << l << endl;
		// w is the grid size <= err
		float w = maxSpan;
		// t = 2^l, num of grids for one dimension
		int t = 1;
		for (int i = 0; i < l; i++) {
			w /= 2.0;
			t *= 2;
		}
		if (verbose) cout << "Grid size: " << w << endl;
		if (verbose) cout << "Num grids for one dimension: " << t << endl;

		int rootBoxID[3] = { 0, 0, 0 };
		mRoot = new OTreeNode(ocBoxMin, maxSpan, rootBoxID, l);
		mGridSize = w;

		// l = 0, only one leaf node in the tree
		if (l == 0) {
			for (int i = 0; i < n; i++) {
				mRoot->addPt(i);
			}
			return;
		}

		unordered_map<long long, OTreeNode*> leafMap;

		for (int i = 0; i < n; i++) {
			auto p = mesh->vertices[i];

			// id from 0 to t-1
			int idBox[3];
			for (int j = 0; j < 3; j++) {
				int id = (int) ceil((p[j] - ocBoxMin[j]) / w);
				idBox[j] = id > (t - 1) ? (t - 1) : id;
			}

			long long hash = getNumberHash(idBox);
			if (leafMap.find(hash) == leafMap.end()) {
				// not found
				// create a new leaf node
				float loc[3];
				getLoc(idBox, loc);
				leafMap[hash] = new OTreeNode(loc, w, idBox, 0);
			}

			leafMap[hash]->addPt(i);
		}

		if (verbose) cout << "Num of leaf nodes: " << leafMap.size() << endl;

		vector<unordered_map<long long, OTreeNode*>> nodeMaps;

		unordered_map<long long, OTreeNode*>& prevMap = leafMap;

		int currLevel = 1;
		int currUnit = 2;
		while (currLevel < l) {
			if (verbose) cout << "Calculating level: " << currLevel << endl;
			unordered_map<long long, OTreeNode*> currMap;

			for (auto prevItr = prevMap.begin(); prevItr != prevMap.end(); prevItr++) {
				auto prevNode = prevItr->second;

				int idBox[3];
				for (int i = 0; i < 3; i++) {
					idBox[i] = prevNode->mLocID[i] / currUnit * currUnit;
				}

				// cout << idBox[0] << "\t" << idBox[1] << "\t" << idBox[2] << endl;

				long long hash = getNumberHash(idBox);
				if (currMap.find(hash) == currMap.end()) {
					// not found
					// create a new node
					float loc[3];
					getLoc(idBox, loc);
					currMap[hash] = new OTreeNode(loc, w * (float) currUnit, idBox, currLevel);
				}

				currMap[hash]->addChild(prevNode);
			}
			if (verbose) cout << "Num of nodes: " << currMap.size() << endl;

			nodeMaps.push_back(currMap);

			currLevel++;
			currUnit *= 2;

			prevMap = currMap;
		}

		for (auto prevItr = prevMap.begin(); prevItr != prevMap.end(); prevItr++) {
			mRoot->addChild(prevItr->second);
		}

	}

	virtual ~OTree() {
		// freeNode(mRoot);
		// delete mRoot;
	}
};

#endif