/* Document converter.
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

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"

#include <cainteoir/metadata.hpp>
#include <cainteoir/document.hpp>
#include <stdexcept>
#include <stack>

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

static void writeTextDocument(std::shared_ptr<cainteoir::document_reader> reader)
{
	bool need_linebreak = false;
	while (reader->read())
	{
		if (reader->type & cainteoir::events::begin_context)
		{
			if (reader->styles) switch (reader->styles->display)
			{
			case cainteoir::display::table:
			case cainteoir::display::table_row:
				if (need_linebreak)
				{
					fwrite("\n\n", 1, 2, stdout);
					need_linebreak = false;
				}
				break;
			case cainteoir::display::table_cell:
				if (need_linebreak)
				{
					fwrite("\n", 1, 1, stdout);
					need_linebreak = false;
				}
				break;
			}
			else switch (reader->context)
			{
			case events::paragraph:
			case events::heading:
			case events::list:
				if (need_linebreak)
				{
					fwrite("\n\n", 1, 2, stdout);
					need_linebreak = false;
				}
				break;
			case events::list_item:
				if (need_linebreak)
				{
					fwrite("\n", 1, 1, stdout);
					need_linebreak = false;
				}
				break;
			}
		}
		if (reader->type & cainteoir::events::text)
		{
			fwrite(reader->text->begin(), 1, reader->text->size(), stdout);
			need_linebreak = true;
		}
	}
}

static void writeHtmlDocument(std::shared_ptr<cainteoir::document_reader> reader)
{
	bool first = true;
	cainteoir::rope text;
	std::stack<std::string> ctx;

	fprintf(stdout, "<html>\n");
	fprintf(stdout, "<body>\n");

	while (reader->read())
	{
		if (reader->type & cainteoir::events::begin_context)
		{
			std::pair<std::string, bool> context;
			if (reader->styles) switch (reader->styles->display)
			{
			case cainteoir::display::inlined:
				if (reader->styles->font_weight == cainteoir::font_weight::bold)
					context = { "strong", false };
				else if (reader->styles->font_style == cainteoir::font_style::italic)
					context = { "strong", false };
				else if (reader->styles->vertical_align == cainteoir::vertical_align::sub)
					context = { "sub", false };
				else if (reader->styles->vertical_align == cainteoir::vertical_align::super)
					context = { "sup", false };
				break;
			case cainteoir::display::table:
				context = { "table", false };
				break;
			case cainteoir::display::table_row:
				context = { "tr", false };
				break;
			case cainteoir::display::table_cell:
				context = { "td", false };
				break;
			}
			else switch (reader->context)
			{
			case events::paragraph: context = { "p", true }; break;
			case events::heading:
				switch (reader->parameter)
				{
				case 1:  context = { "h1", true }; break;
				case 2:  context = { "h2", true }; break;
				case 3:  context = { "h3", true }; break;
				case 4:  context = { "h4", true }; break;
				case 5:  context = { "h5", true }; break;
				default: context = { "h6", true }; break;
				}
				break;
			case events::span:
				switch (reader->parameter)
				{
				case events::underline:   context = { "u", false }; break;
				case events::monospace:   context = { "pre", false }; break;
				default:                  context = { "span", false }; break;
				}
				break;
			case events::list:      context = { "ol", true }; break;
			case events::list_item: context = { "li", true }; break;
			default:                context = { "span", true }; break;
			}

			if (context.second && !first)
				fprintf(stdout, "\n<%s>", context.first.c_str());
			else
				fprintf(stdout, "<%s>", context.first.c_str());
			ctx.push(context.first);
			first = false;
		}

		if (reader->type & cainteoir::events::text)
		{
			if (!reader->text->empty())
				text += reader->text;
		}

		if (reader->type & cainteoir::events::end_context)
		{
			if (!text.empty())
			{
				auto norm = text.normalize();
				text.clear();

				if (!norm->empty())
					fwrite(norm->begin(), 1, norm->size(), stdout);
			}

			if (!ctx.empty())
			{
				fprintf(stdout, "</%s>", ctx.top().c_str());
				ctx.pop();
			}
		}
	}

	fprintf(stdout, "\n</body>");
	fprintf(stdout, "\n</html>");
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		const char *format   = (argc >= 1) ? argv[0] : nullptr;
		const char *filename = (argc == 2) ? argv[1] : nullptr;

		decltype(writeTextDocument) *writer = nullptr;
		if (format)
		{
			if (!strcmp(format, "text"))
				writer = writeTextDocument;
			else if (!strcmp(format, "html"))
				writer = writeHtmlDocument;
		}

		if (!writer)
			throw std::runtime_error(i18n("unsupported format to convert to (html and text only)"));

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, i18n("unsupported document format for file \"%s\"\n"), filename ? filename : "<stdin>");
			return EXIT_FAILURE;
		}

		writer(reader);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
