#include <iostream>
#include <fstream>

#include <gtest/gtest.h>

TEST(TestFstream, OpenOrCreate)
{
	using namespace std;
	ofstream vFile1("/tmp/file1.txt");
	if (vFile1.is_open()) {
		vFile1 << "Create an output file stream will create a file." << endl;
		vFile1.close();
		cout << "Create an output file stream will create a file." << endl;
	} else {
		cout << "Create an output file stream won't create a file." << endl;
	}

	ifstream vFile2("/tmp/file2.txt");
	if (vFile2.is_open()) {
		vFile2.close();
		cout << "Create an input file stream will also create a file." << endl;
	} else {
		cout << "Create an intput file stream won't create a file." << endl;
	}

	fstream vFile3("/tmp/file3.txt");
	if (vFile3.is_open()) {
		vFile3.close();
		cout << "Create an input/output file stream will also create a file." << endl;
	} else {
		cout << "Create an intput/output file stream won't create a file." << endl;
	}

	ofstream vFile4("/tmp/file1.txt", ios::in | ios::out | ios::noreplace);
	if (vFile4.is_open()) {
		vFile4.close();
		cout << "Create an output file stream of existed file with noreplace will succeed." << endl;
	} else {
		cout << "Create an output file stream of existed file with noreplace won't succeed." << endl;
	}

	ofstream vFile5("/tmp/file1.txt");
	if (vFile5.is_open()) {
		vFile5.close();
		cout << "Create an output file stream of existed file without noreplace will succeed." << endl;
	} else {
		cout << "Create an output file stream of existed file without noreplace won't succeed." << endl;
	}
}
