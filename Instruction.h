#include <functional>
#include <string>

namespace Instructions
{
	template<typename T>
	class Instruction
	{
	public:
		Instruction() {}
		Instruction(std::function<void(T)> func) { function = func; }
		~Instruction() {}
		void operator()() { if (function) { function(); } }
	private:
		std::function<void(T)> function;
	};
}