#include "base_type.h"
#include "configurations.h"
#include "log.h"

Configurations::Configurations(const wstring& configFilePath, int argc, char** argv) {
	spdlog::info("Configurations");
	bootStrapClassPath = L"D:/jvm/modules/java.base/classes";
	appClassPath = L"D:/github/minijvm/MiniJVM/testclasses";
	// ��д���ˡ����ԣ�֮���Ӳ�������������
	targetClass = L"icu.mianshi.main.HelloWorld";
}


string Configurations::toString() {
	string s;
	s.append("MethodAreaType:");
	s.append((this->isExtensibleMethodArea() ? "Exensible" : "Fixed"));
	s.append("\n");
	return s;
}