#include "BirdOptiPlugin.h"
#include <vector>
#include <map>
#include "Vector3.h"

 std::vector<float> data;
 std::vector<Vector3> ray_data;
 std::vector<float> distances;
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

 void SetDistance(int boid, int frame, float value) {
	 distances[boid * frame_max + frame] = value;
 }

__declspec(dllexport) int __stdcall Add(int a, int b)
{
	return a + b;
}

__declspec(dllexport) bool __stdcall LoadData(int boid_num, int frame_num, float* d)
{
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
			distances.push_back(10.0f);
		}
	}
	return true;
}

__declspec(dllexport) float* __stdcall OutputData(int& size, float*& return_data)
{
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