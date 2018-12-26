extern "C" __declspec(dllexport) int __stdcall Add(int a, int b);
extern "C" __declspec(dllexport) bool __stdcall LoadData(int boid_num, int frame_num, float* data);
extern "C" __declspec(dllexport) int __stdcall GlobalOptimize(int& size, float*& data);
extern "C" __declspec(dllexport) int __stdcall StepOptimize(int& size, float*& data);
extern "C" __declspec(dllexport) void __stdcall ReleaseAll();