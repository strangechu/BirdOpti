#include "BirdOptiPlugin.h"
#include <vector>

 std::vector<float> data;
 float *data_out;

__declspec(dllexport) int __stdcall Add(int a, int b)
{
	return a + b;
}

__declspec(dllexport) bool __stdcall LoadData(int size, float* d)
{
	data = std::vector<float>(d, d + size * sizeof(float));
	return true;
}

__declspec(dllexport) float* __stdcall OutputData(int& size, float*& return_data)
{
	data_out = new float[3];
	data_out[0] = data[0] + 1;
	data_out[1] = data[1] + 2;
	data_out[2] = data[2] + 3;

	size = 3;

	return_data = data_out;

	return data_out;
}

__declspec(dllexport) void __stdcall ReleaseAll()
{
	delete[] data_out;
}