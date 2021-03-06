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
 std::vector<double> last_guess;
 float *data_out;
 int boid_max;
 int frame_max;
 int current_frame;

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
			 Vector3 last_step_v = last_pos - last_last_pos;
			 Vector3 step_v = pos - last_pos;
			 double cos_theta = last_step_v.dot(step_v) / (last_step_v.length() * step_v.length());
			 double angle = acos(cos_theta);

			 if (cos_theta > 1 || cos_theta < -1)
				 angle = 0.0;

			 //sum += angle * param_data[1];

			 //sum += abs(last_step_v.length() - step_v.length()) * param_data[1];

			 float step_length = (pos - last_pos).length();
			 float step_speed_diff = (last_step_v - step_v).length();

			 sum += step_speed_diff * param_data[1];

			 // boid rule similarity
			 Vector3 separate = Vector3 (0.0f, 0.0f, 0.0f);
			 Vector3 coherence = Vector3 (0.0f, 0.0f, 0.0f);
			 int separate_num = 0;
			 int coherence_num = 0;
			 for (int j = 0; j < boid_max; j++) {
				 if(i == j) {
					 continue;
				 }
				 
				 Vector3 other_pos = GetRayData(j, current_frame - 1) * x[j];
				 Vector3 diff = last_pos - other_pos;
				 float distance = diff.length();
				 if (distance < param_data[3]) {
					 Vector3 force = diff.normalize() / distance;
					 separate = separate + force;
					 separate_num++;
				 }
				 coherence = coherence + (other_pos - last_pos);
				 coherence_num++;
			 }
			 if (separate_num > 0) {
				 separate = separate / separate_num;
				 separate = separate.normalize();
			 }
			 if (coherence_num > 0) {
				 coherence = coherence / coherence_num;
				 coherence = coherence.normalize();
			 }
			 Vector3 boid_pos = last_pos + (separate * (param_data[4]) + coherence * (1 - param_data[4]));
			 float boid_diff = (pos - boid_pos).length();

			 sum += boid_diff * param_data[2];
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
	data = std::vector<float>(d, d + boid_num * frame_num);
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
	param_data = std::vector<float>(param, param + 5);

	int param_num = boid_max * frame_max;

	int return_result = 1;

	for (int frame = 0; frame < frame_max; frame++) {
		current_frame = frame;
		nlopt::opt opt(nlopt::LN_COBYLA, boid_max);
		std::vector<double> lb;
		std::vector<double> ub;
		if (frame == 0) {
			for (int i = 0; i < boid_max; i++) {
				double start_pos = (min + max) / 2 + i;
				lb.push_back(start_pos);
				ub.push_back(start_pos);
			}
		}
		else {
			for (int i = 0; i < boid_max; i++) {
				lb.push_back(last_guess[i] - param_data[0]);
				ub.push_back(last_guess[i] + param_data[0]);
			}
		}
		opt.set_lower_bounds(lb);
		opt.set_upper_bounds(ub);

		opt.set_min_objective(bird_step_opti_func, NULL);
		opt.set_xtol_rel(1e-4);

		std::vector<double> guess;
		if (frame == 0) {
			for (int i = 0; i < boid_max; i++) {
				float guess_num = 20;
				guess.push_back(guess_num);
			}
		}
		else {
			guess = last_guess;
		}
		double min;

		try {
			nlopt::result result = opt.optimize(guess, min);
			last_guess = guess;
		}
		catch (std::exception &e) {
			last_guess = guess;
			e.what();
			return_result = -1;
			break;
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
	return return_result;
}

__declspec(dllexport) void __stdcall ReleaseAll()
{
	delete[] data_out;
}