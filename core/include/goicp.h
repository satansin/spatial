#ifndef __GOICP_H__
#define __GOICP_H__

#include <queue>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "trans.h"
#include "mesh.h"

#include "jly_icp3d.hpp"
#include "jly_sorting.hpp"
#include "jly_3ddt.h"

#define PI 3.1415926536
#define SQRT3 1.732050808

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
	// DB_Meshes* pDBMeshes;
	// int dbID;
	// Mesh* pMeshQ;

	ROTNODE optNodeRot;
	TRANSNODE optNodeTrans;

	GoICP();
	float Register();
	void BuildDT();

	float MSEThresh;
	float SSEThresh;
	float icpThresh;

	float optError;
	Matrix optR;
	Matrix optT;

	clock_t clockBegin;

	float trimFraction;
	int inlierNum;
	bool doTrim;

	bool icp_only;

	float OuterBnB();
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

	float InnerBnB(float* maxRotDisL, TRANSNODE* nodeTransOut);
	float ICP(Matrix& R_icp, Matrix& t_icp);

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

	initNodeRot.lb = 0;
	initNodeTrans.lb = 0;

	doTrim = false;
	trimFraction = 0;

	dt.SIZE = 300;
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
float GoICP::ICP(Matrix& R_icp, Matrix& t_icp)
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
			dis = dt.Distance(pDataTempICP[i].x, pDataTempICP[i].y, pDataTempICP[i].z);
			// dis = pDBMeshes->cal_corr_err(pDataTempICP[i].x, pDataTempICP[i].y, pDataTempICP[i].z, dbID);
			error += dis*dis;
		}
		else
		{
			minDis[i] = dt.Distance(pDataTempICP[i].x, pDataTempICP[i].y, pDataTempICP[i].z);
			// minDis[i] = pDBMeshes->cal_corr_err(pDataTempICP[i].x, pDataTempICP[i].y, pDataTempICP[i].z, dbID);
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
	icp3d.err_diff_def = MSEThresh/10000;
	// printf("icp3d.err_diff_def:%lf\n", icp3d.err_diff_def);
	icp3d.trim_fraction = trimFraction;
	icp3d.do_trim = doTrim;

	// Initialise so-far-best rotation and translation nodes
	optNodeRot = initNodeRot;
	optNodeTrans = initNodeTrans;
	// Initialise so-far-best rotation and translation matrices
	optR = Matrix::eye(3);
	optT = Matrix::ones(3,1)*0;

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
float GoICP::InnerBnB(float* maxRotDisL, TRANSNODE* nodeTransOut)
{
	int i, j;
	float transX, transY, transZ;
	float lb, ub, optErrorT;
	float dis, maxTransDis;
	TRANSNODE nodeTrans, nodeTransParent;
	priority_queue<TRANSNODE> queueTrans;

	// Set optimal translation error to overall so-far optimal error
	// Investigating translation nodes that are sub-optimal overall is redundant
	optErrorT = optError;

	// Push top-level translation node into the priority queue
	queueTrans.push(initNodeTrans);

	//
	while(1)
	{
		if(queueTrans.empty())
			break;

		nodeTransParent = queueTrans.top();
		queueTrans.pop();

		if(optErrorT-nodeTransParent.lb < SSEThresh)
		{
			break;
		}

		nodeTrans.w = nodeTransParent.w/2;
		maxTransDis = SQRT3/2.0*nodeTrans.w;

		for(j = 0; j < 8; j++)
		{
			nodeTrans.x = nodeTransParent.x + (j&1)*nodeTrans.w ;
			nodeTrans.y = nodeTransParent.y + (j>>1&1)*nodeTrans.w ;
			nodeTrans.z = nodeTransParent.z + (j>>2&1)*nodeTrans.w ;

			transX = nodeTrans.x + nodeTrans.w/2;
			transY = nodeTrans.y + nodeTrans.w/2;
			transZ = nodeTrans.z + nodeTrans.w/2;
			
			// For each data point, calculate the distance to it's closest point in the model cloud
			for(i = 0; i < Nd; i++)
			{
				// Find distance between transformed point and closest point in model set ||R_r0 * x + t0 - y||
				// pDataTemp is the data points rotated by R0
				minDis[i] = dt.Distance(pDataTemp[i].x + transX, pDataTemp[i].y + transY, pDataTemp[i].z + transZ);
				// minDis[i] = pDBMeshes->cal_corr_err(pDataTemp[i].x + transX, pDataTemp[i].y + transY, pDataTemp[i].z + transZ, dbID);

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
			ub = 0;
			for(i = 0; i < inlierNum; i++)
			{
				ub += minDis[i]*minDis[i];
			}

			lb = 0;
			for(i = 0; i < inlierNum; i++)
			{
				// Subtract the translation uncertainty radius
				dis = minDis[i] - maxTransDis;
				if(dis > 0)
					lb += dis*dis;
			}
			

			// If upper bound is better than best, update optErrorT and optTransOut (optimal translation node)
			if(ub < optErrorT)
			{
				optErrorT = ub;
				if(nodeTransOut)
					*nodeTransOut = nodeTrans;
			}

			// Remove subcube from queue if lb is bigger than optErrorT
			if(lb >= optErrorT)
			{
				//discard
				continue;
			}

			nodeTrans.ub = ub;
			nodeTrans.lb = lb;
			queueTrans.push(nodeTrans);
		}
	}

	return optErrorT;
}

float GoICP::OuterBnB()
{
	int i, j;
	ROTNODE nodeRot, nodeRotParent;
	TRANSNODE nodeTrans;
	float v1, v2, v3, t, ct, ct2,st, st2;
	float tmp121, tmp122, tmp131, tmp132, tmp231, tmp232;
	float R11, R12, R13, R21, R22, R23, R31, R32, R33;
	float lb, ub, error, dis;
	clock_t clockBeginICP;
	priority_queue<ROTNODE> queueRot;

	// Calculate Initial Error
	optError = 0;

	for(i = 0; i < Nd; i++)
	{
		minDis[i] = dt.Distance(pData[i].x, pData[i].y, pData[i].z);
		// minDis[i] = pDBMeshes->cal_corr_err(pData[i].x, pData[i].y, pData[i].z, dbID);
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
	// cout << "Error*: " << optError << " (Init)" << endl;
	printf("Error*: %.10f(sqrt) (Init)\n", sqrt(optError));

	Matrix R_icp = optR;
	Matrix t_icp = optT;

	// Run ICP from initial state
	clockBeginICP = clock();
	error = ICP(R_icp, t_icp);
	if(error < optError)
	{
		optError = error;
		optR = R_icp;
		optT = t_icp;
		// cout << "Error*: " << error << " (ICP " << (double)(clock()-clockBeginICP)/CLOCKS_PER_SEC << "s)" << endl;
		printf("Error*: %.10f(sqrt) ", sqrt(error));
		cout << "(ICP " << (double)(clock()-clockBeginICP)/CLOCKS_PER_SEC << "s)" << endl;
		cout << "ICP-ONLY Rotation Matrix:" << endl;
		cout << R_icp << endl;
		cout << "ICP-ONLY Translation Vector:" << endl;
		cout << t_icp << endl;
	}

	// Push top-level rotation node into priority queue
	queueRot.push(initNodeRot);

	// Keep exploring rotation space until convergence is achieved
	long long count = 0;
	while(1)
	{
		if(queueRot.empty())
		{
		  cout << "Rotation Queue Empty" << endl;
		  // cout << "Error*: " << optError << ", LB: " << lb << endl;
		  printf("Error*: %.10f(sqrt) , LB: %.10f(sqrt)\n", sqrt(optError), sqrt(lb));
		  break;
		}

		// Access rotation cube with lowest lower bound...
		nodeRotParent = queueRot.top();
		// ...and remove it from the queue
		queueRot.pop();

		// Exit if the optError is less than or equal to the lower bound plus a small epsilon
		if((optError-nodeRotParent.lb) <= SSEThresh)
		{
			// cout << "Error*: " << optError << ", LB: " << nodeRotParent.lb << ", epsilon: " << SSEThresh << endl;
		  printf("Error*: %.10f(sqrt) , LB: %.10f(sqrt), epsilon: %.10f(sqrt)\n", sqrt(optError), sqrt(nodeRotParent.lb), sqrt(SSEThresh));
			break;
		}

		if(count>0 && count%300 == 0)
			printf("Print for every 300 run: LB=%f  L=%d\n",nodeRotParent.lb,nodeRotParent.l);
		count ++;
		
		// Subdivide rotation cube into octant subcubes and calculate upper and lower bounds for each
		nodeRot.w = nodeRotParent.w/2;
		nodeRot.l = nodeRotParent.l+1;
		// For each subcube,
		for(j = 0; j < 8; j++)
		{
		  // Calculate the smallest rotation across each dimension
			nodeRot.a = nodeRotParent.a + (j&1)*nodeRot.w ;
			nodeRot.b = nodeRotParent.b + (j>>1&1)*nodeRot.w ;
			nodeRot.c = nodeRotParent.c + (j>>2&1)*nodeRot.w ;

			// Find the subcube centre
			v1 = nodeRot.a + nodeRot.w/2;
			v2 = nodeRot.b + nodeRot.w/2;
			v3 = nodeRot.c + nodeRot.w/2;

			// Skip subcube if it is completely outside the rotation PI-ball
			if(sqrt(v1*v1+v2*v2+v3*v3)-SQRT3*nodeRot.w/2 > PI)
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
			ub = InnerBnB(NULL /*Rotation Uncertainty Radius*/, &nodeTrans);

			// If the upper bound is the best so far, run ICP
			if(ub < optError)
			{
				// Update optimal error and rotation/translation nodes
				optError = ub;
				optNodeRot = nodeRot;
				optNodeTrans = nodeTrans;

				optR.val[0][0] = R11; optR.val[0][1] = R12; optR.val[0][2] = R13;
				optR.val[1][0] = R21; optR.val[1][1] = R22; optR.val[1][2] = R23;
				optR.val[2][0] = R31; optR.val[2][1] = R32; optR.val[2][2] = R33;
				optT.val[0][0] = optNodeTrans.x+optNodeTrans.w/2;
				optT.val[1][0] = optNodeTrans.y+optNodeTrans.w/2;
				optT.val[2][0] = optNodeTrans.z+optNodeTrans.w/2;

				// cout << "Error*: " << optError << endl;
				printf("Error*: %.10f(sqrt)\n", sqrt(optError));

				// Run ICP
				clockBeginICP = clock();
				R_icp = optR;
				t_icp = optT;
				error = ICP(R_icp, t_icp);
				//Our ICP implementation uses kdtree for closest distance computation which is slightly different from DT approximation, 
				//thus it's possible that ICP failed to decrease the DT error. This is no big deal as the difference should be very small.
				if(error < optError)
				{
					optError = error;
					optR = R_icp;
					optT = t_icp;
					
					// cout << "Error*: " << error << "(ICP " << (double)(clock() - clockBeginICP)/CLOCKS_PER_SEC << "s)" << endl;
					printf("Error*: %.10f(sqrt) ", sqrt(error));
					cout << "(ICP " << (double)(clock() - clockBeginICP)/CLOCKS_PER_SEC << "s)" << endl;
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
			lb = InnerBnB(maxRotDis[nodeRot.l], NULL /*Translation Node*/);

			// If the best error so far is less than the lower bound, remove the rotation subcube from the queue
			if(lb >= optError)
			{
				continue;
			}

			// Update node and put it in queue
			nodeRot.ub = ub;
			nodeRot.lb = lb;
			queueRot.push(nodeRot);
		}
	}

	return optError;
}

float GoICP::Register()
{
	Initialize();
	OuterBnB();
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
	// cout << "Trim Frac: " << trimFraction << endl;
	// cout << "# inliers: " << inlierNum << endl;
	// cout << "Trim? " << doTrim << endl;
}

void loadPointCloud(Mesh* mesh, POINT3D** p) {
	*p = (POINT3D *) malloc(sizeof(POINT3D) * mesh->size());
	for (int i = 0; i < mesh->size(); i++) {
		(*p)[i].x = mesh->get_pt(i).x;
		(*p)[i].y = mesh->get_pt(i).y;
		(*p)[i].z = mesh->get_pt(i).z;
	}
}

// convert data to GoICP structure
void loadGoICP(Mesh* mesh_p, Mesh* mesh_q, double delta, GoICP& goicp) {
    // load GoICP model (DB)
    loadPointCloud(mesh_p, &goicp.pModel);
	goicp.Nm = mesh_p->size();
	// goicp.pDBMeshes = db_meshes;
	// goicp.dbID = 0;
	
	double max_scale = mesh_p->box_max_scale();
	// goicp.initNodeTrans.x = (mesh_p->get_box_min(0) + mesh_p->get_box_max(0)) * 0.5 - max_scale;
	// goicp.initNodeTrans.y = (mesh_p->get_box_min(1) + mesh_p->get_box_max(1)) * 0.5 - max_scale;
	// goicp.initNodeTrans.z = (mesh_p->get_box_min(2) + mesh_p->get_box_max(2)) * 0.5 - max_scale;
	// goicp.initNodeTrans.w = max_scale * 2.0;
	goicp.initNodeTrans.x = 1400.0;
	goicp.initNodeTrans.y = 2000.0;
	goicp.initNodeTrans.z = 500.0;
	goicp.initNodeTrans.w = 1000.0;

	// load GoICP data (query)
	loadPointCloud(mesh_q, &goicp.pData);
	goicp.Nd = mesh_q->size();
	// goicp.pMeshQ = mesh_q;

	goicp.SSEThresh = sq(delta);
	goicp.MSEThresh = goicp.SSEThresh / (double) goicp.Nd;
}

// Run ICP and calculate sum squared L2 error
float GoICP::ICP(Trans* xf) {
	Matrix rot(3, 3), tra(3, 1);
	rot.val[0][0] = xf->r11; rot.val[0][1] = xf->r12; rot.val[0][2] = xf->r13;
	rot.val[1][0] = xf->r21; rot.val[1][1] = xf->r22; rot.val[1][2] = xf->r23;
	rot.val[2][0] = xf->r31; rot.val[2][1] = xf->r32; rot.val[2][2] = xf->r33;
	tra.val[0][0] = xf->tx;  tra.val[1][0] = xf->ty;  tra.val[2][0] = xf->tz;
	return ICP(rot, tra);
}

#endif