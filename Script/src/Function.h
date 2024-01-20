#pragma once
#include "Defines.h"
#include <string>
#include <vector>
#include <ankerl/unordered_dense.h>

#include "Symbol.h"
#include "Object.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4201)
#pragma pack(push,1)
#define PACKED
#else
#define PACKED __attribute__ ((__packed__))
#endif

union Instruction
{
	struct {
		OpCodes code : 8;
		uint8 target : 8;
		union {
			struct {
				uint8 in1 : 8;
				uint8 in2 : 8;
			};
			uint16 param : 16;
		};
	}PACKED;
	uint32 data : 32;

	Instruction() { data = 0; }
};

#ifdef _MSC_VER
#pragma warning(pop)
#pragma pack(pop)
#endif

struct Scoped
{
	ankerl::unordered_dense::map<std::string, Symbol*> symbols;
	Scoped* parent = nullptr;
	std::list<Scoped> children;

	Symbol* findSymbol(const std::string& name) {
		if (auto it = symbols.find(name); it != symbols.end()) {
			return it->second;
		}
		if (!parent) return nullptr;
		return parent->findSymbol(name);
	}

	Symbol* addSymbol(const std::string& name) {
		auto it = findSymbol(name);
		if (!it) {
			auto s = symbols.emplace(name, new Symbol{});
			return s.first->second;
		}
		return nullptr;
	}

	~Scoped() {
		for (auto& [name, s] : symbols) {
			delete s;
		}
	}
};

struct Function
{
	std::string Name;
	std::string Namespace;
	size_t NamespaceHash;
	bool IsPublic;

	ankerl::unordered_dense::set<std::string> stringTable;
	ankerl::unordered_dense::set<double> numberTable;
	ankerl::unordered_dense::set<size_t> jumpTable;

	std::vector<Function*> functionTable;
	std::vector<ObjectType> typeTable;
	std::vector<Variable*> globalTable;
	std::vector<std::string> functionTableSymbols;
	std::vector<std::string> typeTableSymbols;
	std::vector<std::string> globalTableSymbols;

	ankerl::unordered_dense::map<int, int> debugLines;

	Scoped* scope;

	std::vector<VariableType> Types;

	uint8 ArgCount;
	uint8 RegisterCount;

	std::vector<uint32> Bytecode;

	Function() {
		scope = nullptr;
		ArgCount = 0;
		RegisterCount = 0;
		IsPublic = false;
		NamespaceHash = 0;
	}
	~Function() {
		delete scope;
	}
};