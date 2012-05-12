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

#include <cainteoir/metadata.hpp>
#include <cainteoir/document.hpp>
#include <stdexcept>
#include <stack>

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

struct text_printer : public cainteoir::document_events
{
	void text(std::shared_ptr<cainteoir::buffer> aText)
	{
		fwrite(aText->begin(), 1, aText->size(), stdout);
	}

	void begin_context(events::context aContext, uint32_t aParameter)
	{
		switch (aContext)
		{
		case events::paragraph:
		case events::heading:
		case events::list:
		case events::list_item:
			fwrite("\n\n", 1, 2, stdout);
			break;
		}
	}
};

struct html_printer : public cainteoir::document_events
{
	html_printer()
		: mFirst(true)
	{
		fprintf(stdout, "<html>\n");
		fprintf(stdout, "<body>\n");
	}

	~html_printer()
	{
		fprintf(stdout, "\n</body>");
		fprintf(stdout, "\n</html>");
	}

	void text(std::shared_ptr<cainteoir::buffer> aText)
	{
		if (!aText->empty())
			mText += aText;
	}

	void begin_context(events::context aContext, uint32_t aParameter)
	{
		std::pair<std::string, bool> context;
		switch (aContext)
		{
		case events::paragraph: context = { "p", true }; break;
		case events::heading:
			switch (aParameter)
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
			switch (aParameter)
			{
			case events::superscript: context = { "sup", false }; break;
			case events::subscript:   context = { "sub", false }; break;
			case events::emphasized:  context = { "em", false }; break;
			case events::strong:      context = { "strong", false }; break;
			case events::underline:   context = { "u", false }; break;
			case events::monospace:   context = { "pre", false }; break;
			default:                  context = { "span", false }; break;
			}
			break;
		case events::list:      context = { "ol", true }; break;
		case events::list_item: context = { "li", true }; break;
		default:                context = { "span", true }; break;
		}

		if (context.second && !mFirst)
			fprintf(stdout, "\n<%s>", context.first.c_str());
		else
			fprintf(stdout, "<%s>", context.first.c_str());
		ctx.push(context.first);
		mFirst = false;
	}

	void end_context()
	{
		if (!mText.empty())
		{
			auto norm = mText.normalize();
			mText.clear();

			if (!norm->empty())
				fwrite(norm->begin(), 1, norm->size(), stdout);
		}

		if (!ctx.empty())
		{
			fprintf(stdout, "</%s>", ctx.top().c_str());
			ctx.pop();
		}
	}

	bool mFirst;
	cainteoir::rope mText;
	std::stack<std::string> ctx;
};

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc != 2)
			throw std::runtime_error("no document specified");

		std::shared_ptr<cainteoir::document_events> events;
		if (!strcmp(argv[0], "text"))
			events = std::make_shared<text_printer>();
		else if (!strcmp(argv[0], "html"))
			events = std::make_shared<html_printer>();
		else
			throw std::runtime_error("unsupported format to convert to (html and text only)");

		rdf::graph metadata;
		if (!cainteoir::parseDocument(argv[1], *events, metadata))
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[1]);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
