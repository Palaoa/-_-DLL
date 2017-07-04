#ifndef TestDll_H_
#define TestDll_H_
#ifdef MYLIBDLL
#define MYLIBDLL extern "C" _declspec(dllimport) 
#else
#define MYLIBDLL extern "C" _declspec(dllexport) 
#endif
MYLIBDLL int Add(char* bmpfile, char*pmgfile);
MYLIBDLL int detect(char* filename);
MYLIBDLL int recongize(char* filename);
MYLIBDLL void bmptopmg(char* bmpfile, char*pmgfile);
//You can also write like this:
//extern "C" {
//_declspec(dllexport) int Add(int plus1, int plus2);
//};
#endif