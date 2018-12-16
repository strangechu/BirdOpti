extern "C" __declspec(dllexport) int __stdcall Add(int a, int b);
extern "C" __declspec(dllexport) bool __stdcall LoadData(int size, float* data);
extern "C" __declspec(dllexport) float* __stdcall OutputData(int& size, float*& data);
extern "C" __declspec(dllexport) void __stdcall ReleaseAll();