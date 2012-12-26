/* Test driver for generated speech events from XML documents using CSS.
 *
 * Copyright (C) 2012 Reece H. Dunn
 *
 * This file is part of cainteoir-engine.
 *
 * cainteoir-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cainteoir-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cainteoir/xmlreader.hpp>
#include <stdexcept>
#include <list>
#include <map>

namespace xml = cainteoir::xml;

namespace cainteoir
{
	struct context_manager
	{
		struct context
		{
			std::string name;
			std::map<std::string, std::string> attrs;
		};

		context_manager();

		void push_context(const std::string &ns, const std::string &name);

		void pop_context(const std::string &ns, const std::string &name);

		void add_context_attribute(const std::string &ns, const std::string &name, const std::string &value);

		xml::begin_tag_type get_begin_tag_type() const
		{
			return current_styles.begin_tag_type;
		}
	private:
		void print_context();

		struct style_data
		{
			xml::begin_tag_type begin_tag_type;

			style_data(xml::begin_tag_type aBeginTagType)
				: begin_tag_type(aBeginTagType)
			{
			}
		};

		std::list<context> ctx;
		std::list<std::pair<context, style_data>> styles;
		style_data current_styles;
	};
}

cainteoir::context_manager::context_manager()
	: current_styles(xml::begin_tag_type::open)
{
}

void cainteoir::context_manager::push_context(const std::string &ns, const std::string &name)
{
	ctx.push_back({name});
	print_context();

	bool found = false;
	for (auto &style : styles)
	{
		std::string &style_name = style.first.name;
		if (cainteoir::buffer::ignore_case(style_name.c_str(), name.c_str(), style_name.size()) == 0)
		{
			found = true;
			current_styles.begin_tag_type = style.second.begin_tag_type;
		}
	}

	if (!found)
	{
		current_styles.begin_tag_type = xml::begin_tag_type::open;
	}
}

void cainteoir::context_manager::pop_context(const std::string &ns, const std::string &name)
{
	if (ctx.back().name != name)
	{
		fprintf(stdout, "|error| expected end-tag '%s', got '%s'\n", ctx.back().name.c_str(), name.c_str());
	}

	ctx.pop_back();
}

void cainteoir::context_manager::add_context_attribute(const std::string &ns, const std::string &name, const std::string &value)
{
	if (!ctx.empty() && name.find("xmlns") == std::string::npos)
	{
		ctx.back().attrs[name] = value;
		print_context();
	}
}

void cainteoir::context_manager::print_context()
{
	fprintf(stdout, "context >>");
	for (auto &item : ctx)
	{
		fprintf(stdout, " %s", item.name.c_str());
		if (!item.attrs.empty())
		{
			fprintf(stdout, "[");
			for (auto &attr : item.attrs)
			{
				fprintf(stdout, " @%s=\"%s\"", attr.first.c_str(), attr.second.c_str());
			}
			fprintf(stdout, " ]");
		}
	}
	fprintf(stdout, "\n");
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		cainteoir::context_manager styles;
		xml::reader reader(cainteoir::make_file_buffer(argv[0]), "windows-1252");
		/*
		 * reader.nodePrefix()   -- namespace prefix
		 * reader.namespaceUri() -- namespace value
		 * reader.nodeName()     -- local name
		 * reader.nodeValue()    -- node context
		 */
		while (reader.read()) switch (reader.nodeType())
		{
		case xml::reader::beginTagNode:
			styles.push_context(reader.namespaceUri(), reader.nodeName().str());
			reader.set_begin_tag_type(styles.get_begin_tag_type());
			break;
		case xml::reader::endTagNode:
			styles.pop_context(reader.namespaceUri(), reader.nodeName().str());
			break;
		case xml::reader::attribute:
			styles.add_context_attribute(reader.namespaceUri(), reader.nodeName().str(), reader.nodeValue().str());
			break;
		case xml::reader::cdataNode:
		case xml::reader::textNode:
			break;
		case xml::reader::commentNode:
		case xml::reader::beginProcessingInstructionNode:
		case xml::reader::endProcessingInstructionNode:
			// ignore
			break;
		case xml::reader::error:
			fprintf(stdout, "|error| internal parser error\n");
			break;
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
