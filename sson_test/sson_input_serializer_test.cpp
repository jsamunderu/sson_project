#include "stdafx.h"
#include "CppUnitTest.h"

#include <sson_lib/inc/sson.h>

#include <sstream>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace sson_test
{		
	TEST_CLASS(test_sson_input_serializer)
	{
	public:
		
		TEST_METHOD(test_empty_input_stream)
		{
            Assert::ExpectException<std::runtime_error>([]()
            {
                std::stringstream empty;
                sson::object obj;
                empty >> sson::input_serializer(obj);
            });
		}

        TEST_METHOD(test_one_object_one_value)
        {
            std::stringstream ss;
            ss << "{ name:object; }";
            
            sson::object obj;
            ss >> sson::input_serializer(obj);

            Assert::AreEqual(size_t(1), obj.values().size());
            Assert::AreEqual(std::string("name"), obj.values()[0].first);
            Assert::AreEqual(std::string("object"), obj.values()[0].second);
        }

        TEST_METHOD(test_nested_objects)
        {
            std::stringstream ss;
            ss << "{ name:object; type:nested object; { name:child; type:child object; }}";

            sson::object obj;
            ss >> sson::input_serializer(obj);

            Assert::AreEqual(size_t(2), obj.values().size());
            Assert::AreEqual(size_t(1), obj.children().size());

        }

        TEST_METHOD(test_nested_object_no_end_bracket)
        {
            Assert::ExpectException<std::runtime_error>([]()
            {

                std::stringstream ss;
                ss << "{ name:no end bracket; { value:15; }";

                sson::object obj;
                ss >> sson::input_serializer(obj);

            });

        }

        TEST_METHOD(test_object_no_end_bracket)
        {
            Assert::ExpectException<std::runtime_error>([]()
            {

                std::stringstream ss;
                ss << "{ name:no end bracket; ";

                sson::object obj;
                ss >> sson::input_serializer(obj);

            });
        }

		TEST_METHOD(test_object_no_start_bracket)
		{
			Assert::ExpectException<std::runtime_error>([]()
			{
				std::stringstream ss;
				ss << "name:no end bracket; ";

				sson::object obj;
				ss >> sson::input_serializer(obj);

			});
		}

		TEST_METHOD(test_empty_object)
		{
			std::stringstream ss;
			ss << "{}";

			sson::object obj;
			ss >> sson::input_serializer(obj);

			Assert::AreEqual(size_t(0), obj.values().size());
		}

		TEST_METHOD(test_empty_object_empty_children)
		{
			std::stringstream ss;
			ss << "{{}}";

			sson::object obj;
			ss >> sson::input_serializer(obj);

			Assert::AreEqual(size_t(0), obj.values().size());
			Assert::AreEqual(size_t(1), obj.children().size());
		}

		TEST_METHOD(test_empty_parent_one_child)
		{
			std::stringstream ss;
			ss << "{ {name:object;} }";

			sson::object obj;
			ss >> sson::input_serializer(obj);

			Assert::AreEqual(size_t(0), obj.values().size());
			Assert::AreEqual(size_t(1), obj.children().size());
			Assert::AreEqual(std::string("name"), obj.children()[0].values()[0].first);
			Assert::AreEqual(std::string("object"), obj.children()[0].values()[0].second);
		}

		TEST_METHOD(test_get_value_found)
		{
			std::stringstream ss;
			ss << "{ {name:object;} }";

			sson::object obj;
			ss >> sson::input_serializer(obj);

			Assert::AreEqual(std::string("object"), obj.get_value("name"));
		}


		TEST_METHOD(test_get_value_not_found)
		{
			Assert::ExpectException<sson::object::value_not_found>([]()
			{
				std::stringstream ss;
				ss << "{}";

				sson::object obj;
				ss >> sson::input_serializer(obj);

				std::string value = obj.get_value("key");
			});
		}

		TEST_METHOD(test_get_value_not_found_deep)
		{
			Assert::ExpectException<sson::object::value_not_found>([]()
			{
				std::stringstream ss;
				ss << "{{}}";

				sson::object obj;
				ss >> sson::input_serializer(obj);

				std::string value = obj.get_value("key");
			});
		}

		TEST_METHOD(test_empty_object_output)
		{
			std::stringstream ss;
			ss << "{}";

			sson::object obj;
			ss >> sson::input_serializer(obj);

			std::stringstream result;
			result << sson::output_serializer(obj);

			Assert::AreNotEqual(size_t(0), result.str().size());
			Assert::AreEqual(std::string("{}"), result.str());
		}

		TEST_METHOD(test_one_object_output)
		{
			std::stringstream ss;
			ss << "{ name:object; }";

			sson::object obj;
			ss >> sson::input_serializer(obj);

			std::stringstream result;
			result << sson::output_serializer(obj);

			Assert::AreNotEqual(size_t(0), result.str().size());
			Assert::AreEqual(std::string("{name:object;}"), result.str());
		}

		TEST_METHOD(test_nested_object_output)
		{
			std::stringstream ss;
			ss << "{ name:object; { name2:object2; } }";

			sson::object obj;
			ss >> sson::input_serializer(obj);

			std::stringstream result;
			result << sson::output_serializer(obj);

			Assert::AreNotEqual(size_t(0), result.str().size());
			Assert::AreEqual(std::string("{name:object;{name2:object2;}}"), result.str());
		}
	};
}