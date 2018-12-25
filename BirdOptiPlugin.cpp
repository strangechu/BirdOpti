#include "BirdOptiPlugin.h"
#include <vector>
#include <map>
#include "Vector3.h"

#include <nlopt.hpp>

 std::vector<float> data;
 std::vector<Vector3> ray_data;
 std::vector<float> distances;
 std::vector<double> temp_distances;
 float *data_out;
 int boid_max;
 int frame_max;

 Vector3 GetRayData(int boid, int frame) {
	 return ray_data[boid * frame_max + frame];
 }

 void SetRayData(int boid, int frame, Vector3 value) {
	 ray_data[boid * frame_max + frame] = value;
 }

 float GetDistance(int boid, int frame) {
	 return distances[boid * frame_max + frame];
 }

 float GetTempDistance(int boid, int frame) {
	 return temp_distances[boid * frame_max + frame];
 }

 void SetDistance(int boid, int frame, float value) {
	 distances[boid * frame_max + frame] = value;
 }

 Vector3 GetPosition(int boid, int frame) {
	 return GetRayData(boid, frame) * GetDistance(boid, frame);
 }

 Vector3 GetTempPosition(int boid, int frame) {
	 return GetRayData(boid, frame) * GetTempDistance(boid, frame);
 }

 Vector3 GetTempLastPosition(int boid, int frame) {
	 if (frame == 0) return GetTempPosition(boid, frame);
	 return GetTempPosition(boid, frame - 1);
 }

 double bird_opti_func(const std::vector<double> &x, std::vector<double> &grad, void *data)
 {
	 double sum = 0;
	 //v_data *d = reinterpret_cast<v_data*>(data);
	 //std::vector<Vector3> v = d->v;

	 if (!grad.empty()) {
		 //
	 }

	 temp_distances = x;
	 //for (int now_frame = 0; now_frame < frame_max; now_frame++) {
		// for (int now_boid = 0; now_boid < boid_max; now_boid++) {
		//	 for (int i = 0; i < boid_max; i++) {
		//		 if (i == now_boid) continue;
		//		 double distance = (GetTempPosition(now_boid, now_frame) - GetTempPosition(i, now_frame)).length();
		//		 sum += distance;
		//	 }
		// }
	 //}

	 for (int i = 0; i < boid_max; i++) {
		 for (int j = 0; j < frame_max; j++) {
			 Vector3 pos = GetTempPosition(i, j);
			 Vector3 last_pos = GetTempLastPosition(i, j);
			 double length = (pos - last_pos).length();
			 sum += length;
		 }
	 }

	 std::vector<Vector3> p;
	 //for (int i = 0; i < v.size(); i++) {
	 //	 p.push_back(v[i] * x[0]);
	 //}

	 return sum;
 }

__declspec(dllexport) int __stdcall Add(int a, int b)
{
	return a + b;
}

__declspec(dllexport) bool __stdcall LoadData(int boid_num, int frame_num, float* d)
{
	ray_data.clear();
	distances.clear();
	boid_max = boid_num;
	frame_max = frame_num;
	data = std::vector<float>(d, d + boid_num * frame_num * sizeof(float));
	int index = 0;
	for (int i = 0; i < boid_max; i++) {
		for (int j = 0; j < frame_max; j++) {
			float x = d[index++];
			float y = d[index++];
			float z = d[index++];
			
			Vector3 ray(x, y, z);
			ray_data.push_back(ray);
			distances.push_back(0.0f);
		}
	}
	return true;
}

__declspec(dllexport) float* __stdcall OutputData(int& size, float*& return_data)
{
	int param_num = boid_max * frame_max;
	//nlopt_opt opt;
	//opt = nlopt_create(NLOPT_LN_COBYLA, param_num);
	nlopt::opt opt(nlopt::LN_COBYLA, param_num);
	std::vector<double> lb;
	for (int i = 0; i < param_num; i++) {
		if (i % frame_max == 0) lb.push_back(15 + i / frame_max);
		else lb.push_back(10);
	}
	std::vector<double> ub;
	for (int i = 0; i < param_num; i++) {
		if (i % frame_max == 0) ub.push_back(15 + i / frame_max);
		else ub.push_back(20);
	}
	opt.set_lower_bounds(lb);
	opt.set_upper_bounds(ub);

	opt.set_min_objective(bird_opti_func, NULL);
	opt.set_xtol_rel(1e-4);

	std::vector<double> guess;
	for (int i = 0; i < param_num; i++) {
		float guess_num = 20;
		guess.push_back(guess_num);
	}
	double min;
	nlopt::result result = opt.optimize(guess, min);
	
	for (int i = 0; i < param_num; i++) distances[i] = guess[i];

	size = 3 * boid_max * frame_max;
	data_out = new float[size];
	int index = 0;
	for (int i = 0; i < boid_max; i++) {
		for (int j = 0; j < frame_max; j++) {
			data_out[index++] = GetRayData(i, j).x * GetDistance(i, j);
			data_out[index++] = GetRayData(i, j).y * GetDistance(i, j);
			data_out[index++] = GetRayData(i, j).z * GetDistance(i, j);
		}
	}

	return_data = data_out;

	return data_out;
}

__declspec(dllexport) void __stdcall ReleaseAll()
{
	delete[] data_out;
}