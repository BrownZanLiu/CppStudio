#include <iostream>

int main()
{
	int i = 0;
	int a[3] = {0, 0, 0};

	for (; i <= 3; ++i) {
		a[i] = 0;
		std::cout << i << ": Hellow, brown!" << std::endl;
	}

	return 0;
}
