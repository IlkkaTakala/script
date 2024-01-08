#pragma once
#include "Defines.h"
#include <mutex>
#include <condition_variable>
#include <queue>
#include <stack>
#include <future>
#include <span>
#include "ankerl/unordered_dense.h"

#include "Eril/Eril.hpp"
#include "Eril/Variable.h"
#include "Function.h"
#include "Object.h"

using namespace Eril;

class VM;
struct CompileOptions
{
	std::string Path;
	ScriptHandle Handle = 0;
	Options UserOptions;
};

struct CallObject 
{
	const Function* FunctionPtr;
	size_t Location;
	uint16 StackOffset;
	std::vector<Variable> Arguments;
	std::promise<Variable> Return;

	CallObject(Function* function);
	CallObject(const CallObject& obj) = delete;
	CallObject operator=(const CallObject& obj) = delete;
};

class Runner
{
public:
	Runner(VM* vm);
	~Runner();

	void operator()();

private:
	VM* Owner;
	std::stack<CallObject*> CallStack;
	std::stack<Variable, std::vector<Variable>> Stack;
};

class VM
{
public:
	VM();
	~VM();

	void ReinitializeGrammar(const char* grammar);
	ScriptHandle Compile(const char* path, const Options& options);

	size_t GetFunctionID(const std::string& name);

	size_t CallFunction(FunctionHandle handle, const std::span<Variable>& args);
	Variable GetReturnValue(size_t index);

	inline bool IsRunning() const { return VMRunning; }
	//void Step();

private:
	friend class Parser;
	friend class Runner;

	// When adding new compile targets
	std::mutex CompileMutex;
	// When merging compile results to VM
	std::mutex MergeMutex;
	// When getting new handles
	std::mutex HandleMutex;
	// When editing call queue
	std::mutex CallMutex;

	std::condition_variable QueueNotify;
	std::queue<CompileOptions> CompileQueue;
	std::vector<std::thread> ParserPool;
	size_t HandleCounter = 0;
	bool CompileRunning;

	std::condition_variable CallQueueNotify;
	std::queue<CallObject*> CallQueue;
	std::vector<std::thread> RunnerPool;
	std::vector<std::future<Variable>> ReturnValues;
	std::stack<size_t> ReturnFreeList;
	bool VMRunning;

	ankerl::unordered_dense::map<uint32, Function> FunctionMap;
	ankerl::unordered_dense::map<std::string, uint32> NameToFunctionMap;

	ObjectManager Manager;
	std::vector<Variable> GlobalVariables;
	// Variable heap?
};