#ifndef __GOICP_H__
#define __GOICP_H__

#include <queue>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "trans.h"
#include "mesh.h"
#include "util.h"

#include "jly_icp3d.hpp"
#include "jly_sorting.hpp"
#include "jly_3ddt.h"

#define PI 3.1415926536
#define SQRT3 1.732050808

struct GoICP_Stat {
    int max_layer;
    long long num_run;
};

typedef struct _POINT3D
{
	float x, y, z;
}POINT3D;

typedef struct _ROTNODE
{
	float a, b, c, w;
	float ub, lb;
	int l;
	friend bool operator < (const struct _ROTNODE & n1, const struct _ROTNODE & n2)
	{
		if(n1.lb != n2.lb)
			return n1.lb > n2.lb;
		else
			return n1.w < n2.w;
			//return n1.ub > n2.ub;
	}
	
}ROTNODE;

typedef struct _TRANSNODE
{
	float x, y, z, w;
	float ub, lb;
	int l;
	friend bool operator < (const struct _TRANSNODE & n1, const struct _TRANSNODE & n2)
	{
		if(n1.lb != n2.lb)
			return n1.lb > n2.lb;
		else
			return n1.w < n2.w;
			//return n1.ub > n2.ub;
	}
}TRANSNODE;

#define MAXROTLEVEL 20

class GoICP
{
public:
	int Nm, Nd;
	POINT3D * pModel, * pData;

	ROTNODE initNodeRot;
	TRANSNODE initNodeTrans;

	DT3D dt;
	DB_Meshes* pDBMeshes;
	int dbID;
	// Mesh* pMeshQ;

	float rotThresh;
	float transThresh;

	bool use_dt;
	bool verbose;

	ROTNODE optNodeRot;
	TRANSNODE optNodeTrans;

	GoICP();
	float Register();
	void BuildDT();

	float MSEThresh;
	float SSEThresh;
	float icpThresh;

	float stopSSE;

	float optError;
	ICP_Matrix optR;
	ICP_Matrix optT;

	clock_t clockBegin;

	float trimFraction;
	int inlierNum;
	bool doTrim;

	bool icp_only;

	float OuterBnB(GoICP_Stat& gs);
	void Initialize();
	void Clear();

	void printParams();
	float ICP(Trans* xf);


private:
	//temp variables: calculate once
	float * normData;
	float** maxRotDis;
	// float * maxRotDisL;
	
	ICP3D<float> icp3d;
	float * M_icp;
	float * D_icp;

	float InnerBnB(float* maxRotDisL, TRANSNODE* nodeTransOut, int outer_layer, GoICP_Stat& gs);
	float ICP(ICP_Matrix& R_icp, ICP_Matrix& t_icp);

	//temp variables: changes while executing
	float * minDis;
	POINT3D * pDataTemp;
	POINT3D * pDataTempICP;
};


GoICP::GoICP()
{
	initNodeRot.a = -PI;
	initNodeRot.b = -PI;
	initNodeRot.c = -PI;
	initNodeRot.w = 2*PI;
	initNodeRot.l = 0;
	initNodeTrans.l = 0;

	initNodeRot.lb = 0;
	initNodeTrans.lb = 0;

	doTrim = false;
	trimFraction = 0;

	// dt.SIZE = 300;
	dt.SIZE = 200;
	dt.expandFactor = 2.0;
}

// Build Distance Transform
void GoICP::BuildDT()
{
	double* x = (double*)malloc(sizeof(double)*Nm);
	double* y = (double*)malloc(sizeof(double)*Nm);
	double* z = (double*)malloc(sizeof(double)*Nm);
	for(int i = 0; i < Nm; i++)
	{
		x[i] = pModel[i].x;
		y[i] = pModel[i].y;
		z[i] = pModel[i].z;
	}
	dt.Build(x, y, z, Nm);
	delete(x);
	delete(y);
	delete(z);
}

// Run ICP and calculate sum squared L2 error
float GoICP::ICP(ICP_Matrix& R_icp, ICP_Matrix& t_icp)
{
  int i;
	float error, dis;

	icp3d.Run(D_icp, Nd, R_icp, t_icp); // data cloud, # data points, rotation matrix, translation matrix

	// Transform point cloud and use DT to determine the L2 error
	error = 0;
	for(i = 0; i < Nd; i++)
	{
		POINT3D& p = pData[i];
		pDataTempICP[i].x = R_icp.val[0][0]*p.x+R_icp.val[0][1]*p.y+R_icp.val[0][2]*p.z + t_icp.val[0][0];
		pDataTempICP[i].y = R_icp.val[1][0]*p.x+R_icp.val[1][1]*p.y+R_icp.val[1][2]*p.z + t_icp.val[1][0];
		pDataTempICP[i].z = R_icp.val[2][0]*p.x+R_icp.val[2][1]*p.y+R_icp.val[2][2]*p.z + t_icp.val[2][0];

		if(!doTrim)
		{
			if (use_dt) {
				dis = dt.Distance(pDataTempICP[i].x, pDataTempICP[i].y, pDataTempICP[i].z);
			} else {
				dis = pDBMeshes->cal_corr_err(pDataTempICP[i].x, pDataTempICP[i].y, pDataTempICP[i].z, dbID);
			}
			error += dis*dis;
		}
		else
		{
			if (use_dt) {
				minDis[i] = dt.Distance(pDataTempICP[i].x, pDataTempICP[i].y, pDataTempICP[i].z);
			} else {
				minDis[i] = pDBMeshes->cal_corr_err(pDataTempICP[i].x, pDataTempICP[i].y, pDataTempICP[i].z, dbID);
			}
		}
	}

	if(doTrim)
	{
		//qsort(minDis, Nd, sizeof(float), cmp);
		//myqsort(minDis, Nd, inlierNum);
		intro_select(minDis,0,Nd-1,inlierNum-1);
		for(i = 0; i < inlierNum; i++)
		{
			error += minDis[i]*minDis[i];
		}
	}

	return error;
}

void GoICP::Initialize()
{
	int i, j;
	float sigma, maxAngle;

	// Precompute the rotation uncertainty distance (maxRotDis) for each point in the data and each level of rotation subcube

	// Calculate L2 norm of each point in data cloud to origin
	normData = (float*)malloc(sizeof(float)*Nd);
	for(i = 0; i < Nd; i++)
	{
		normData[i] = sqrt(pData[i].x*pData[i].x + pData[i].y*pData[i].y + pData[i].z*pData[i].z);
	}

	maxRotDis = new float*[MAXROTLEVEL];
	for(i = 0; i < MAXROTLEVEL; i++)
	{
		maxRotDis[i] = (float*)malloc(sizeof(float*)*Nd);

		sigma = initNodeRot.w/pow(2.0,i)/2.0; // Half-side length of each level of rotation subcube
		maxAngle = SQRT3*sigma;

		if(maxAngle > PI)
			maxAngle = PI;
		for(j = 0; j < Nd; j++)
			maxRotDis[i][j] = 2*sin(maxAngle/2)*normData[j];
	}

	// Temporary Variables
	minDis = (float*)malloc(sizeof(float)*Nd);
	pDataTemp = (POINT3D *)malloc(sizeof(POINT3D)*Nd);
	pDataTempICP = (POINT3D *)malloc(sizeof(POINT3D)*Nd);

	// ICP Initialisation
	// Copy model and data point clouds to variables for ICP
	M_icp = (float*)calloc(3*Nm,sizeof(float));
	D_icp = (float*)calloc(3*Nd,sizeof(float));
	for(i = 0, j = 0; i < Nm; i++)
	{
		M_icp[j++] = pModel[i].x;
		M_icp[j++] = pModel[i].y;
		M_icp[j++] = pModel[i].z;
	}
	for(i = 0, j = 0; i < Nd; i++)
	{
		D_icp[j++] = pData[i].x;
		D_icp[j++] = pData[i].y;
		D_icp[j++] = pData[i].z;
	}

	// Build ICP kdtree with model dataset
	icp3d.Build(M_icp,Nm);
	// icp3d.err_diff_def = MSEThresh/10000;
	icp3d.err_diff_def = MSEThresh/100;
	if (verbose) printf("icp3d.err_diff_def:%.6f\n", icp3d.err_diff_def);
	icp3d.trim_fraction = trimFraction;
	icp3d.do_trim = doTrim;

	// Initialise so-far-best rotation and translation nodes
	optNodeRot = initNodeRot;
	optNodeTrans = initNodeTrans;
	// Initialise so-far-best rotation and translation matrices
	optR = ICP_Matrix::eye(3);
	optT = ICP_Matrix::ones(3,1)*0;

	// For untrimmed ICP, use all points, otherwise only use inlierNum points
	if(doTrim)
	{
		// Calculate number of inlier points
		inlierNum = (int)(Nd * (1 - trimFraction));
	}
	else
	{
		inlierNum = Nd;
	}
	// SSEThresh = MSEThresh * inlierNum;
}

void GoICP::Clear()
{
	delete(pDataTemp);
	delete(pDataTempICP);
	delete(normData);
	delete(minDis);
	for(int i = 0; i < MAXROTLEVEL; i++)
	{
		delete(maxRotDis[i]);
	}
	delete(maxRotDis);
	delete(M_icp);
	delete(D_icp);
}

// Inner Branch-and-Bound, iterating over the translation space
float GoICP::InnerBnB(float* maxRotDisL, TRANSNODE* nodeTransOut, int outer_layer, GoICP_Stat& gs)
{
	int i, j;
	float t_transX, t_transY, t_transZ;
	float t_lb, t_ub, t_optErrorT;
	float t_dis, t_maxTransDis;
	TRANSNODE t_nodeTrans, t_nodeTransParent;
	priority_queue<TRANSNODE> queueTrans;

	// Set optimal translation error to overall so-far optimal error
	// Investigating translation nodes that are sub-optimal overall is redundant
	t_optErrorT = optError;

	// Push top-level translation node into the priority queue
	queueTrans.push(initNodeTrans);

	long long count = 0;

	//
	while(1)
	{
		if(queueTrans.empty())
			break;

		t_nodeTransParent = queueTrans.top();
		queueTrans.pop();

		if(t_optErrorT-t_nodeTransParent.lb < stopSSE)
		{
			break;
		}

		if(verbose && count>0 && count%100000 == 0)
			printf("Inner: Print for every 100000 run: w=%f, LB=%f, L=%d, error_diff=%f\n", t_nodeTransParent.w, t_nodeTransParent.lb, t_nodeTransParent.l, t_optErrorT-t_nodeTransParent.lb);
		count ++;

		if (gs.max_layer < outer_layer + t_nodeTransParent.l) {
			gs.max_layer = outer_layer + t_nodeTransParent.l;
		}
		gs.num_run++;

		t_nodeTrans.w = t_nodeTransParent.w/2;
		// if (t_nodeTrans.w < transThresh) {
		// 	continue;
		// }
		// cout << "Inner_w: " << t_nodeTrans.w << endl;
		t_maxTransDis = SQRT3/2.0*t_nodeTrans.w;

		for(j = 0; j < 8; j++)
		{
			t_nodeTrans.x = t_nodeTransParent.x + (j&1)*t_nodeTrans.w ;
			t_nodeTrans.y = t_nodeTransParent.y + (j>>1&1)*t_nodeTrans.w ;
			t_nodeTrans.z = t_nodeTransParent.z + (j>>2&1)*t_nodeTrans.w ;

			t_transX = t_nodeTrans.x + t_nodeTrans.w/2;
			t_transY = t_nodeTrans.y + t_nodeTrans.w/2;
			t_transZ = t_nodeTrans.z + t_nodeTrans.w/2;

			t_nodeTrans.l = t_nodeTransParent.l+1;
			
			// For each data point, calculate the distance to it's closest point in the model cloud
			for(i = 0; i < Nd; i++)
			{
				// Find distance between transformed point and closest point in model set ||R_r0 * x + t0 - y||
				// pDataTemp is the data points rotated by R0
				if (use_dt) {
					minDis[i] = dt.Distance(pDataTemp[i].x + t_transX, pDataTemp[i].y + t_transY, pDataTemp[i].z + t_transZ);
				} else {
					minDis[i] = pDBMeshes->cal_corr_err(pDataTemp[i].x + t_transX, pDataTemp[i].y + t_transY, pDataTemp[i].z + t_transZ, dbID);
				}

				// Subtract the rotation uncertainty radius if calculating the rotation lower bound
				// maxRotDisL == NULL when calculating the rotation upper bound
				if(maxRotDisL)
					minDis[i] -= maxRotDisL[i];

				if(minDis[i] < 0)
				{
					minDis[i] = 0;
				}
			}

			if(doTrim)
			{
				// Sort by distance
				//qsort(minDis, Nd, sizeof(float), cmp);
				//myqsort(minDis, Nd, inlierNum);
				intro_select(minDis,0,Nd-1,inlierNum-1);
			}

			// For each data point, find the incremental upper and lower bounds
			t_ub = 0;
			for(i = 0; i < inlierNum; i++)
			{
				t_ub += minDis[i]*minDis[i];
			}

			t_lb = 0;
			for(i = 0; i < inlierNum; i++)
			{
				// Subtract the translation uncertainty radius
				t_dis = minDis[i] - t_maxTransDis;
				if(t_dis > 0)
					t_lb += t_dis*t_dis;
			}
			

			// If upper bound is better than best, update t_optErrorT and optTransOut (optimal translation node)
			if(t_ub < t_optErrorT)
			{
				t_optErrorT = t_ub;
				if(nodeTransOut)
					*nodeTransOut = t_nodeTrans;
			}

			// Remove subcube from queue if lb is bigger than t_optErrorT
			if(t_lb >= t_optErrorT)
			{
				//discard
				continue;
			}

			t_nodeTrans.ub = t_ub;
			t_nodeTrans.lb = t_lb;
			queueTrans.push(t_nodeTrans);
		}

		// if(t_optErrorT <= SSEThresh)
		// {
		// 	break;
		// }
	}

	return t_optErrorT;
}

float GoICP::OuterBnB(GoICP_Stat& gs)
{
	int i, j;
	ROTNODE t_nodeRot, t_nodeRotParent;
	TRANSNODE t_nodeTrans;
	float v1, v2, v3, t, ct, ct2, st, st2;
	float tmp121, tmp122, tmp131, tmp132, tmp231, tmp232;
	float R11, R12, R13, R21, R22, R23, R31, R32, R33;
	float t_lb, t_ub, t_error;
	priority_queue<ROTNODE> queueRot;

	// Calculate Initial Error
	optError = 0;

	for(i = 0; i < Nd; i++)
	{
		if (use_dt) {
			minDis[i] = dt.Distance(pData[i].x, pData[i].y, pData[i].z);
		} else {
			minDis[i] = pDBMeshes->cal_corr_err(pData[i].x, pData[i].y, pData[i].z, dbID);
		}
	}
	if(doTrim)
	{
		// Sort by distance
		//qsort(minDis, Nd, sizeof(float), cmp);
		//myqsort(minDis, Nd, inlierNum);
		intro_select(minDis,0,Nd-1,inlierNum-1);
	}
	for(i = 0; i < inlierNum; i++)
	{
		optError += minDis[i]*minDis[i];
	}
	if (verbose) printf("Error*: %.6f (Init)\n", optError);

	ICP_Matrix t_R_icp = optR;
	ICP_Matrix t_T_icp = optT;

	// Run ICP from initial state
	if (verbose) timer_start();
	t_error = ICP(t_R_icp, t_T_icp);
	if (verbose) cout << "ICP time: " << timer_end(SECOND) << "(s)" << endl;

	if(t_error < optError)
	{
		optError = t_error;
		optR = t_R_icp;
		optT = t_T_icp;
		if (verbose) printf("Error*: %.6f \n", t_error);
		// cout << "ICP-ONLY Rotation Matrix:" << endl;
		// cout << t_R_icp << endl;
		// cout << "ICP-ONLY Translation Vector:" << endl;
		// cout << t_T_icp << endl;
	}

	// Push top-level rotation node into priority queue
	queueRot.push(initNodeRot);

	// Keep exploring rotation space until convergence is achieved
	long long count = 0;
	while(1)
	{
		if(queueRot.empty())
		{
		  if (verbose) cout << "Rotation Queue Empty" << endl;
		  if (verbose) printf("Error*: %.6f, LB: %.6f\n", optError, t_lb);
		  break;
		}

		// Access rotation cube with lowest lower bound...
		t_nodeRotParent = queueRot.top();
		// ...and remove it from the queue
		queueRot.pop();

		// Exit if the optError is less than or equal to the lower bound plus a small epsilon
		if((optError-t_nodeRotParent.lb) <= stopSSE)
		{
			// cout << "Error*: " << optError << ", LB: " << t_nodeRotParent.lb << ", epsilon: " << SSEThresh << endl;
		  printf("Error*: %.6f, LB: %.6f, SSE: %.6f\n", optError, t_nodeRotParent.lb, 0.001); // SSEThresh);
			break;
		}

		// // added by me to filter by delta
		// if (t_nodeRotParent.lb > SSEThresh) {
		// 	continue;
		// }

		if(verbose && count>0 && count%100 == 0)
			printf("Outer: Print for every 100 run: w=%f, LB=%f, L=%d, error_diff=%f\n", t_nodeRotParent.w, t_nodeRotParent.lb, t_nodeRotParent.l, optError-t_nodeRotParent.lb);
		count ++;

		if (gs.max_layer < t_nodeRotParent.l) {
			gs.max_layer = t_nodeRotParent.l;
		}
		gs.num_run++;
		
		// Subdivide rotation cube into octant subcubes and calculate upper and lower bounds for each
		t_nodeRot.w = t_nodeRotParent.w/2;
		// TODO: do not split anymore if less than rot_thresh
		// if (t_nodeRot.w < rotThresh) {
		// 	continue;
		// }
		// cout << "Outer_w: " << t_nodeRot.w << endl;
		t_nodeRot.l = t_nodeRotParent.l+1;

		// For each subcube,
		for(j = 0; j < 8; j++)
		{
		  // Calculate the smallest rotation across each dimension
			t_nodeRot.a = t_nodeRotParent.a + (j&1)*t_nodeRot.w ;
			t_nodeRot.b = t_nodeRotParent.b + (j>>1&1)*t_nodeRot.w ;
			t_nodeRot.c = t_nodeRotParent.c + (j>>2&1)*t_nodeRot.w ;

			// Find the subcube centre
			v1 = t_nodeRot.a + t_nodeRot.w/2;
			v2 = t_nodeRot.b + t_nodeRot.w/2;
			v3 = t_nodeRot.c + t_nodeRot.w/2;

			// Skip subcube if it is completely outside the rotation PI-ball
			if(sqrt(v1*v1+v2*v2+v3*v3)-SQRT3*t_nodeRot.w/2 > PI)
			{
				continue;
			}

			// Convert angle-axis rotation into a rotation matrix
			t = sqrt(v1*v1 + v2*v2 + v3*v3);
			if(t > 0)
			{
				v1 /= t;
				v2 /= t;
				v3 /= t;

				ct = cos(t);
				ct2 = 1 - ct;
				st = sin(t);
				st2 = 1 - st;

				tmp121 = v1*v2*ct2; tmp122 = v3*st;
				tmp131 = v1*v3*ct2; tmp132 = v2*st;
				tmp231 = v2*v3*ct2; tmp232 = v1*st;

				R11 = ct + v1*v1*ct2;		R12 = tmp121 - tmp122;		R13 = tmp131 + tmp132;
				R21 = tmp121 + tmp122;		R22 = ct + v2*v2*ct2;		R23 = tmp231 - tmp232;
				R31 = tmp131 - tmp132;		R32 = tmp231 + tmp232;		R33 = ct + v3*v3*ct2;

				// Rotate data points by subcube rotation matrix
				for(i = 0; i < Nd; i++)
				{
					POINT3D& p = pData[i];
					pDataTemp[i].x = R11*p.x + R12*p.y + R13*p.z;
					pDataTemp[i].y = R21*p.x + R22*p.y + R23*p.z;
					pDataTemp[i].z = R31*p.x + R32*p.y + R33*p.z;
				}
			}
			// If t == 0, the rotation angle is 0 and no rotation is required
			else
			{
				memcpy(pDataTemp, pData, sizeof(POINT3D)*Nd);
			}

			// Upper Bound
			// Run Inner Branch-and-Bound to find rotation upper bound
			// Calculates the rotation upper bound by finding the translation upper bound for a given rotation,
			// assuming that the rotation is known (zero rotation uncertainty radius)
			t_ub = InnerBnB(NULL /*Rotation Uncertainty Radius*/, &t_nodeTrans, t_nodeRotParent.l, gs);
			// printf("Current UB: %.6f, optError: %.6f\n", t_ub, optError);

			// If the upper bound is the best so far, run ICP
			if(t_ub < optError)
			{
				// Update optimal error and rotation/translation nodes
				optError = t_ub;
				optNodeRot = t_nodeRot;
				optNodeTrans = t_nodeTrans;

				optR.val[0][0] = R11; optR.val[0][1] = R12; optR.val[0][2] = R13;
				optR.val[1][0] = R21; optR.val[1][1] = R22; optR.val[1][2] = R23;
				optR.val[2][0] = R31; optR.val[2][1] = R32; optR.val[2][2] = R33;
				optT.val[0][0] = optNodeTrans.x+optNodeTrans.w/2;
				optT.val[1][0] = optNodeTrans.y+optNodeTrans.w/2;
				optT.val[2][0] = optNodeTrans.z+optNodeTrans.w/2;

				if (verbose) printf("Error*: %.6f\n", optError);

				// Run ICP
				if (verbose) timer_start();
				t_R_icp = optR;
				t_T_icp = optT;
				t_error = ICP(t_R_icp, t_T_icp);
				if (verbose) cout << "ICP time: " << timer_end(SECOND) << "(s)" << endl;

				//Our ICP implementation uses kdtree for closest distance computation which is slightly different from DT approximation, 
				//thus it's possible that ICP failed to decrease the DT error. This is no big deal as the difference should be very small.
				if(t_error < optError)
				{
					optError = t_error;
					optR = t_R_icp;
					optT = t_T_icp;
					
					if (verbose) printf("Error*: %.6f\n", t_error);
				}

				// Discard all rotation nodes with high lower bounds in the queue
				priority_queue<ROTNODE> queueRotNew;
				while(!queueRot.empty())
				{
					ROTNODE node = queueRot.top();
					queueRot.pop();
					if(node.lb < optError)
						queueRotNew.push(node);
					else
						break;
				}
				queueRot = queueRotNew;
			}

			// Lower Bound
			// Run Inner Branch-and-Bound to find rotation lower bound
			// Calculates the rotation lower bound by finding the translation upper bound for a given rotation,
			// assuming that the rotation is uncertain (a positive rotation uncertainty radius)
			// Pass an array of rotation uncertainties for every point in data cloud at this level
			t_lb = InnerBnB(maxRotDis[t_nodeRot.l], NULL /*Translation Node*/, t_nodeRotParent.l, gs);
			// printf("Current LB: %.6f, optError: %.6f\n", t_lb, optError);

			// If the best error so far is less than the lower bound, remove the rotation subcube from the queue
			if(t_lb >= optError)
			{
				continue;
			}

			// Update node and put it in queue
			t_nodeRot.ub = t_ub;
			t_nodeRot.lb = t_lb;
			queueRot.push(t_nodeRot);
		}

		// if (optError <= SSEThresh) {
		// 	if (verbose) printf("Accepted, optError: %.6f, SSE: %.6f\n", optError, SSEThresh);
		// 	break;
		// }
	}

	return optError;
}

float GoICP::Register()
{
	Initialize();
	// OuterBnB();
	Clear();

	return optError;
}

void GoICP::printParams() {
	cout << "Printing GoICP parameters..." << endl;
	cout << "# mod pts: " << Nm << endl;
	cout << "# dat pts: " << Nd << endl;
	cout << "Init rotat: [" << initNodeRot.a << ", " << initNodeRot.b << ", " << initNodeRot.c << "] -> " << initNodeRot.w << endl;
	cout << "Init trans: [" << initNodeTrans.x << ", " << initNodeTrans.y << ", " << initNodeTrans.z << "] -> " << initNodeTrans.w << endl;
	cout << "MSE thresh: " << MSEThresh << endl;
	cout << "SSE thresh: " << SSEThresh << endl;
	cout << "Stop SSE: " << stopSSE << endl;
	// cout << "Trim Frac: " << trimFraction << endl;
	// cout << "# inliers: " << inlierNum << endl;
	// cout << "Trim? " << doTrim << endl;
}

// Run ICP and calculate sum squared L2 error
float GoICP::ICP(Trans* xf) {
	ICP_Matrix rot(3, 3), tra(3, 1);
	rot.val[0][0] = xf->r11; rot.val[0][1] = xf->r12; rot.val[0][2] = xf->r13;
	rot.val[1][0] = xf->r21; rot.val[1][1] = xf->r22; rot.val[1][2] = xf->r23;
	rot.val[2][0] = xf->r31; rot.val[2][1] = xf->r32; rot.val[2][2] = xf->r33;
	tra.val[0][0] = xf->tx;  tra.val[1][0] = xf->ty;  tra.val[2][0] = xf->tz;
	return ICP(rot, tra);
}

void loadPointCloud(Mesh* mesh, POINT3D** p) {
	*p = (POINT3D *) malloc(sizeof(POINT3D) * mesh->size());
	for (int i = 0; i < mesh->size(); i++) {
		(*p)[i].x = mesh->get_pt(i)->x;
		(*p)[i].y = mesh->get_pt(i)->y;
		(*p)[i].z = mesh->get_pt(i)->z;
	}
}

// convert data to GoICP structure
void loadGoICP(DB_Meshes* db_meshes, int db_id, Mesh* mesh_q, double sse, double mse, GoICP* goicp, bool verbose=false, bool use_dt=true, int dt_size=200) {
	// load GoICP model (DB)
	auto mesh_p = db_meshes->get_mesh(db_id);
	loadPointCloud(mesh_p, &goicp->pModel);
	goicp->Nm = mesh_p->size();
	goicp->pDBMeshes = db_meshes;
	goicp->dbID = db_id;

	double max_scale = mesh_p->box_max_scale();
	goicp->initNodeTrans.x = (mesh_p->get_box_min(0) + mesh_p->get_box_max(0)) * 0.5 - max_scale;
	goicp->initNodeTrans.y = (mesh_p->get_box_min(1) + mesh_p->get_box_max(1)) * 0.5 - max_scale;
	goicp->initNodeTrans.z = (mesh_p->get_box_min(2) + mesh_p->get_box_max(2)) * 0.5 - max_scale;
	goicp->initNodeTrans.w = max_scale * 2.0;
	// goicp->initNodeTrans.x = 1400.0;
	// goicp->initNodeTrans.y = 2000.0;
	// goicp->initNodeTrans.z = 500.0;
	// goicp->initNodeTrans.w = 1000.0;

	// goicp->rotThresh = 0.3; // loose
	goicp->rotThresh = 0.01; // tight
	// goicp->transThresh = goicp->initNodeTrans.w / 4.0 - 10.0; // loose
	goicp->transThresh = goicp->initNodeTrans.w / 32.0 - 1.0; // tight
	// goicp->transThresh = goicp->initNodeTrans.w / 64.0 - 1.0; // tight

	// load GoICP data (query)
	loadPointCloud(mesh_q, &goicp->pData);
	goicp->Nd = mesh_q->size();
	// goicp->pMeshQ = mesh_q;

	goicp->SSEThresh = sse;
	goicp->MSEThresh = mse;
	// goicp->stopSSE = mesh_q->get_bsphere_d() * 0.001 * mesh_q->size();
	goicp->stopSSE = mesh_q->get_bsphere_d() * 0.1 * mesh_q->size();

	goicp->verbose = verbose;

	goicp->use_dt = use_dt;
	if (goicp->use_dt) {
		goicp->dt.SIZE = dt_size;

		// Build Distance Transform
		if (verbose) cout << "Building Distance Transform..." << endl;
		if (verbose) timer_start();
		goicp->BuildDT();
		if (verbose) cout << "DT built in " << timer_end(SECOND) << "(s)" << endl;
		if (verbose) cout << endl;
	}
}

#endif