#include <sson_lib/inc/sson.h>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace sson {

/////////////////////////////////////////////////////////////////////////
/// sson input_serializer
input_serializer::input_serializer(object &obj)
    :obj_(obj)
{
}

void input_serializer::parse(std::istream &is)
{
    if (is.get() != '{')
        throw std::runtime_error("invalid stream - not an sson stream");
    if (!parse_object(is, obj_))
        throw std::runtime_error("failed to find closing '}'");
}

std::istream& operator>>(std::istream &is, input_serializer &isobj)
{
    isobj.parse(is);
    return is;
}

property input_serializer::parse_kvp(std::istream &is)
{
    bool isvalue = false;
    std::string key, value;
    char c;
    while ((c = is.get()) != EOF)
    {
        if (std::isspace(c) && !isvalue && key.empty()) // skip leading whitespace
            continue;
        if (c == ';')
            break;
        else if (c == ':')
        {
            isvalue = true;
            continue;
        }
        else
        {
            if (!isvalue)
                key.push_back(c);
            else
                value.push_back(c);
        }
    }
    if (value.empty() || key.empty())
        throw std::runtime_error("invalid key_value pair");

    return property(key, value);
}

bool input_serializer::parse_object(std::istream &is, object &obj)
{
    char c;
    while ((c = is.get()) != EOF)
    {
        if (std::isspace(c)) // skip whitespace
            continue;

        if (c == '{')
        {
            object o;
            if (!parse_object(is, o))
                throw std::runtime_error("failed to find closing '}'");
            obj.add_child(o);
        }
        else if (c == '}')
        {
            return true;
        }
        else
        {
            is.putback(c);
            obj.add_value(parse_kvp(is));
        }
    }
    return false;

}

/////////////////////////////////////////////////////////////////////////
/// sson output_serializer

output_serializer::output_serializer(const object &obj)
    :obj_(obj)
{
}

std::string output_serializer::to_string() const
{
	std::string result("{");

	for (auto prop : obj_.values())
	{
		result += prop.first;
		result += ":";
		result += prop.second;
		result += ";";
	}

	for (const auto child : obj_.children())
	{
		output_serializer child_serial(child);
		result += child_serial.to_string();
	}

	result += "}";
	return result;
}

std::ostream& operator<<(std::ostream &os, const output_serializer &obj)
{
	os << obj.to_string();
    return os;
}


////////////////////////////////////////////////////////////////////////
// sson object

void object::add_value(const property& kvp)
{
   values_.emplace_back(kvp);
}

void object::add_child(const object & child)
{
   children_.emplace_back(child);
}

const property_list & object::values() const
{
    return values_;
}

const object_list & object::children() const
{
    return children_;
}

const value_t& object::get_value(const key_t& key) const
{
	auto result = std::find_if(values_.begin(), values_.end(), [&key](const property& prop) {
		return prop.first == key;
	});
	if (result == values_.end())
	{
		for (const auto& child : children_)
		{
			try
			{
				return child.get_value(key);
			}
			catch (value_not_found& e)
			{
				(void)e; // ignore and check in other children
			}
		}
		throw value_not_found(key);
	}

	return result->second;
}

object::value_not_found::value_not_found(const key_t& key)
	:key(key)
{
	std::stringstream ss;
	ss << "key [" << key << "] not found";
	buf = ss.str();
}

const key_t& object::value_not_found::get_key() const
{
	return key;
}

const char* object::value_not_found::what() const noexcept (true)
{
	return buf.c_str();
}

object::value_not_found::~value_not_found()
{
}

}
