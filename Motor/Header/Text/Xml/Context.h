#pragma once

#include <stack>
#include <string>

namespace KaputEngine::Text::Xml
{
	class XmlSerializeContext
	{
	public:
		XmlSerializeContext(std::ostream& stream, bool indent);

		/// <summary>
		/// Serializes a self-closing tag for an empty object.
		/// </summary>
		/// <param name="name">Tag name</param>
		void emptyObject(const std::string& name);

		/// <summary>
		/// Serializes the start of an object or array and changes context to inside the object.
		/// </summary>
		/// <param name="name">Name of the tag</param>
		void startObject(std::string&& name);

		/// <summary>
		/// Serializes the end of the current object or array and returns to the outer context.
		/// </summary>
		void endObject();

		/// <summary>
		/// Serializes a value at the current context.
		/// </summary>
		/// <param name="name">Tag name. Using an empty tag name inserts the value in the current node rather than a sub-node.</param>
		template <typename T>
		void value(const std::string& name, const T& value);

	private:
		bool m_indent;
		size_t m_depth;
		std::ostream& m_stream;
		std::stack<std::string> m_nameStack;
	};
}
