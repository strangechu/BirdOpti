#include "BirdOptiPlugin.h"
#include <vector>
#include <map>
#include "Vector3.h"
#include <cmath>

#include <nlopt.hpp>

 std::vector<float> data;
 std::vector<float> param_data;
 std::vector<Vector3> ray_data;
 std::vector<float> distances;
 std::vector<double> temp_distances;
 float *data_out;
 int boid_max;
 int frame_max;
 int current_frame;
 float tra_weight;
 float vel_weight;

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

	 return sum;
 }

 double bird_step_opti_func(const std::vector<double> &x, std::vector<double> &grad, void *data)
 {
	 if (current_frame == 0)
		 return 0;

	 double sum = 0;
	 temp_distances = x;

	 // position near
	 if (current_frame == 1) {
		 for (int i = 0; i < boid_max; i++) {
			 Vector3 pos = GetRayData(i, current_frame) * x[i];
			 Vector3 last_pos = GetRayData(i, current_frame - 1) * GetDistance(i, current_frame - 1);
			 double length = (pos - last_pos).length();
			 double value = abs(param_data[0] - length);

			 sum += value;
		 }
	 }

	 // trajectory smoothness
	 if (current_frame >= 2) {
		 for (int i = 0; i < boid_max; i++) {
			 Vector3 pos = GetRayData(i, current_frame) * x[i];
			 Vector3 last_pos = GetRayData(i, current_frame - 1) * GetDistance(i, current_frame - 1);
			 Vector3 last_last_pos = GetRayData(i, current_frame - 2) * GetDistance(i, current_frame - 2);
			 Vector3 v1 = last_pos - last_last_pos;
			 Vector3 v2 = pos - last_pos;
			 double angle = acos(v1.dot(v2) / (v1.length() * v2.length()));

			 sum += angle * param_data[1];

			 double length = (pos - last_pos).length();
			 double value = abs(param_data[0] - length);

			 sum += value * param_data[2];
		 }

		// for (int i = 0; i < boid_max; i++) {
		//	 Vector3 pos = GetRayData(i, current_frame) * x[i];
		//	 Vector3 last_pos = GetRayData(i, current_frame - 1) * GetDistance(i, current_frame - 1);
		//	 Vector3 last_last_pos = GetRayData(i, current_frame - 2) * GetDistance(i, current_frame - 2);
		//	 double len1 = (last_pos - last_last_pos).length();
		//	 double len2 = (pos - last_pos).length();
		//	 sum += abs(len1 - len2);
		// }
	 }

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

	tra_weight = 0.5f;
	vel_weight = 0.5f;

	return true;
}

__declspec(dllexport) int __stdcall GlobalOptimize(int& size, float*& return_data)
{
	int param_num = boid_max * frame_max;
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
	return result;
}

__declspec(dllexport) int __stdcall StepOptimize(int& size, float*& return_data, float min, float max, float* param)
{
	param_data = std::vector<float>(param, param + 5 * sizeof(float));

	int param_num = boid_max * frame_max;

	for (int frame = 0; frame < frame_max; frame++) {
		current_frame = frame;
		nlopt::opt opt(nlopt::LN_COBYLA, boid_max);
		std::vector<double> lb;
		for (int i = 0; i < boid_max; i++) {
			if (frame == 0) lb.push_back((min + max) / 2 + i);
			else lb.push_back(min);
		}
		std::vector<double> ub;
		for (int i = 0; i < boid_max; i++) {
			if (frame == 0) ub.push_back((min + max) / 2 + i);
			else ub.push_back(max);
		}
		opt.set_lower_bounds(lb);
		opt.set_upper_bounds(ub);

		opt.set_min_objective(bird_step_opti_func, NULL);
		opt.set_xtol_rel(1e-4);

		std::vector<double> guess;
		for (int i = 0; i < boid_max; i++) {
			float guess_num = 20;
			guess.push_back(guess_num);
		}
		double min;

		try {
			nlopt::result result = opt.optimize(guess, min);
		}
		catch (std::exception &e) {
			e.what();
		}

		for (int i = 0; i < boid_max; i++) {
			SetDistance(i, frame, guess[i]);
		}
	}

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
	return 1;
}

__declspec(dllexport) void __stdcall ReleaseAll()
{
	delete[] data_out;
}