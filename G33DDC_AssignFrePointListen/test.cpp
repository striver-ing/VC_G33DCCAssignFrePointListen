#include <stdio.h>
#include "Controller.h"
#include <functional>

typedef std::function< int(int)> Functional;

Functional func;

class TestClass
{
public:
	int b = 2;
	int ClassMember(int a) { return a + b; }
	static int StaticMember(int a) { return a; }

	void bind() {
		func = std::bind(&TestClass::ClassMember, this, std::placeholders::_1);
	}
};

int main()
{
	//(const char* serialNumber, int channel, int frequence, const char* filePath, int fileTotalTime
	Controller* controller = new Controller();
	controller->doTask("11D28423", 0, 720000, "D:\\test.wav", 10);

	/*TestClass te;
	te.bind();
	printf("%d",func(3));
	getchar();*/
	return 0;
}