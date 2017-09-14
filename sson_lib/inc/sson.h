#ifndef __JSON_H__
#define __JSON_H__

#include <string>
#include <vector>
#include <iosfwd>

namespace sson
{
    ///// stupid simple object notation
    /*
    format -
        {
            key:value;
            key:value;
            {
                key:value;
            }
            {
            }
        }
    */
    class object;

    typedef std::string key_t;
    typedef std::string value_t;
    typedef std::pair<key_t, value_t> property;

    typedef std::vector<property> property_list;

    typedef std::vector<object> object_list;
    
    class input_serializer
    {
        object &obj_;
        bool parse_object(std::istream &is, object &obj);
        property parse_kvp(std::istream &is);
    public:
        input_serializer(object &obj);
        void parse(std::istream &is);
    };
    std::istream & operator>>(std::istream &is, input_serializer &isobj);

    class output_serializer
    {
        const object &obj_;
    public:
		std::string to_string() const;
        output_serializer(const object &obj);
    };
    std::ostream & operator<<(std::ostream &os, const output_serializer &ops);
    
    class object
    {
        property_list values_;
        std::vector<object> children_;
    public:
		class value_not_found : public std::exception
		{
			const key_t& key;
			std::string buf;
		public:
			value_not_found(const key_t& key);
			const key_t& get_key() const;
			virtual const char* what() const noexcept (true);
			virtual ~value_not_found();
		};
        void add_value(const property &kvp);
        void add_child(const object &child);

	const value_t& get_value(const key_t& value) const;

        const property_list& values() const;
        const object_list& children() const;
    };
}
#endif // __JSON_H__
