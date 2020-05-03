#include "base_type.h"
#include "configurations.h"
#include "log.h"

Configurations::Configurations(const wstring& configFilePath, int argc, char** argv) {
	spdlog::info("Configurations");
}

string Configurations::toString() {
	string s;
	s.append("MethodAreaType:");
	s.append((this->isExtensibleMethodArea() ? "Exensible" : "Fixed"));
	s.append("\n");
	return s;
}