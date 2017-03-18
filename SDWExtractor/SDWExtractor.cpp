// SDWExtractor.cpp: определяет точку входа для консольного приложения.
//
#define FILE_SIGNATURE 0x0301
#include "stdafx.h"
#include <iostream>
#include <algorithm>

using namespace std;
typedef unsigned __int16 WORD;
typedef struct
{
	WORD sdwType;
	char foo[6];
	int count;
} SDWHeader;
typedef struct
{
	int startoffset;
	int filesize;
	char foo; //WTF??
	char filename[12];
	char foo2[3];
} SDWFile;


bool Extract(char* filename)
{
	FILE *file = fopen(filename, "rb");
	if (!file) return false;
	size_t res;
	SDWHeader sh;
	// читаем заголовок
	res = fread(&sh, 1, sizeof(SDWHeader), file);
	if (res != sizeof(SDWHeader)) { fclose(file); return false; }
	// проверяем сигнатуру
	if (sh.sdwType != FILE_SIGNATURE) { fclose(file); return false; }

	SDWFile* files = new SDWFile[sh.count];
	for (int i = 0; i < sh.count; i++)
	{
		char buf[sizeof(SDWFile)];
		fread(buf, 1, sizeof(SDWFile), file);
		files[i] = *reinterpret_cast<SDWFile*>(&buf[0]);
	}

	for (int i = 0; i < sh.count; i++)
	{
		fseek(file, files[i].startoffset, SEEK_SET);
		FILE *tmp_file = fopen(files[i].filename, "wb");
		char *imgbuf = new char[files[i].filesize];
		fread(imgbuf, 1, files[i].filesize, file);
		if (fwrite(imgbuf, files[i].filesize, 1, tmp_file) == 1) cout << "Extracting file " << files[i].filename << " - OK" << endl;
		fclose(tmp_file);
	}
	return true;
}

int main(int argc, char* argv[])

{
	if (argc == 1)
	{
		cout << "Usage: SDWExtactor \"filename1\" \"filename2\" \"filename3\"" << endl;
		return 0;
	}
	
	for (int i = 1; i < argc; i++)
	{
		Extract(argv[i]);
	}

	return 0;
}

