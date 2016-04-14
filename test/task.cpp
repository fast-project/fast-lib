#include <fructose/fructose.h>

#include <fast-lib/message/migfra/task.hpp>

struct Task_tester :
	public fructose::test_base<Task_tester>
{
	void start(const std::string &test_name)
	{
		fast::msg::migfra::Start start1;
		start1.vm_name = "vm1";
		start1.concurrent_execution = true;

		fast::msg::migfra::Start start2;
		start2.from_string(start1.to_string());
		fructose_assert_eq(start2.vm_name, start1.vm_name);
		fructose_assert_eq(start2.concurrent_execution, start1.concurrent_execution);
		
	}

	void stop(const std::string &test_name)
	{
	}

	void migrate(const std::string &test_name)
	{
	}

	void quit(const std::string &test_name)
	{
	}
};

int main(int argc, char **argv)
{
	Task_tester tests;
	tests.add_test("start", &Task_tester::start);
	tests.add_test("stop", &Task_tester::stop);
	tests.add_test("migrate", &Task_tester::migrate);
	tests.add_test("quit", &Task_tester::quit);
	return tests.run(argc, argv);
}
