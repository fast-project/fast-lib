#include <fructose/fructose.h>

#include <fast-lib/optional.hpp>

using namespace fast;

struct Person :
	Serializable
{
	int id;
	Person(int id = -1) :
		id(id)
	{
	}
	YAML::Node emit() const override
	{
		YAML::Node node;
		node["id"] = id;
		return node;
	}
	void load(const YAML::Node &node) override
	{
		fast::load(id, node[id]);
	}
};
YAML_CONVERT_IMPL(Person)

struct Task_tester :
	public fructose::test_base<Task_tester>
{
	void optional_bool(const std::string &test_name)
	{
		(void) test_name;
		const std::string tag = "b";
		Optional<bool> b(tag);
		fructose_assert(!b);
		fructose_assert(!b.is_valid());
		fructose_assert_eq(tag, b.get_tag());
		fructose_assert_exception(b.get(), std::runtime_error);
		fructose_assert_exception(*b, std::runtime_error);
		b = false;
		fructose_assert(b);
		fructose_assert(b.is_valid());
		fructose_assert(*b == false);
		fructose_assert(b.get() == false);
		fructose_assert_no_exception(b.get());
		fructose_assert_no_exception(*b);
	}

	void optional_struct(const std::string &test_name)
	{
		(void) test_name;
			
		const std::string tag = "person";
		Optional<Person> p(tag);
		fructose_assert(!p);
		fructose_assert(!p.is_valid());
		fructose_assert_eq(tag, p.get_tag());
		fructose_assert_exception(p.get(), std::runtime_error);
		fructose_assert_exception(*p, std::runtime_error);
		fructose_assert_exception((void) p->id, std::runtime_error);
		p = Person(1);
		fructose_assert(p);
		fructose_assert(p.is_valid());
		fructose_assert_no_exception(p.get());
		fructose_assert_no_exception(*p);
		fructose_assert_no_exception((void) (*p).id);
		fructose_assert_no_exception((void) p.get().id);
		fructose_assert_no_exception((void) p->id);
		fructose_assert((*p).id == 1);
		fructose_assert(p.get().id == 1);
		fructose_assert(p->id == 1);
	}

	void optional_copy(const std::string &test_name)
	{
		(void) test_name;
		std::string name1_str = "bob";
		std::string name2_str = "baumeister";
		Optional<std::string> name1("name1", name1_str);
		Optional<std::string> name2("name2", name2_str);
		// Copy constructor
		Optional<std::string> copy_constr(name1);
		fructose_assert(name1.is_valid());
		fructose_assert(copy_constr.is_valid());
		fructose_assert_eq(name1.get(), name1_str);
		fructose_assert_eq(copy_constr.get(), name1_str);
		// Copy assignment
		Optional<std::string> copy_assign("empty");
		copy_assign = name2;
		fructose_assert(name2.is_valid());
		fructose_assert(copy_assign.is_valid());
		fructose_assert_eq(name2.get(), name2_str);
		fructose_assert_eq(copy_assign.get(), name2_str);
		// Move constructor
		Optional<std::string> move_constr(std::move(name1));
		fructose_assert(!name1.is_valid());
		fructose_assert(move_constr.is_valid());
		fructose_assert_eq(move_constr.get(), name1_str);
		// Move assignment
		Optional<std::string> move_assign("empty_move");
		move_assign = std::move(name2);
		fructose_assert(!name2.is_valid());
		fructose_assert(move_assign.is_valid());
		fructose_assert_eq(move_assign.get(), name2_str);
	}
};

int main(int argc, char **argv)
{
	Task_tester tests;
	tests.add_test("optional-bool", &Task_tester::optional_bool);
	tests.add_test("optional-struct", &Task_tester::optional_struct);
	tests.add_test("optional-copy", &Task_tester::optional_copy);
	return tests.run(argc, argv);
}
