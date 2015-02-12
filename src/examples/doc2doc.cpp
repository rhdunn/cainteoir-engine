/* Document converter.
 *
 * Copyright (C) 2012-2015 Reece H. Dunn
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

/* This is a demonstration of using the Cainteoir Text-to-Speech document reader
 * APIs to create a document conversion utility.
 */

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/metadata.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/path.hpp>
#include <stdexcept>
#include <stack>

namespace css    = cainteoir::css;
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
			case css::display::block:
			case css::display::table:
			case css::display::table_row:
				if (need_linebreak)
				{
					fwrite("\n\n", 1, 2, stdout);
					need_linebreak = false;
				}
				break;
			case css::display::table_cell:
			case css::display::list_item:
				if (need_linebreak)
				{
					fwrite("\n", 1, 1, stdout);
					need_linebreak = false;
				}
				break;
			default:
				break;
			}
		}
		if (reader->type & cainteoir::events::text)
		{
			fwrite(reader->content->begin(), 1, reader->content->size(), stdout);
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
			case css::display::inlined:
				if (reader->styles->font_weight == css::font_weight::bold)
					context = { "strong", false };
				else if (reader->styles->font_style == css::font_style::italic)
					context = { "strong", false };
				else if (reader->styles->vertical_align == css::vertical_align::sub)
					context = { "sub", false };
				else if (reader->styles->vertical_align == css::vertical_align::super)
					context = { "sup", false };
				else if (reader->styles->text_decoration == css::text_decoration::underline)
					context = { "u", false };
				else if (reader->styles->font_family == "monospace")
					context = { "tt", false };
				else
					context = { "span", false };
				break;
			case css::display::block:
				if (reader->styles->font_family == "monospace")
					context = { "pre", false };
				else if (reader->styles->list_style_type == "disc")
					context = { "ul", false };
				else if (reader->styles->list_style_type == "decimal")
					context = { "ol", false };
				else if (reader->styles->role == css::role::heading) switch (reader->styles->aria_level)
				{
				case 1:  context = { "h1", true }; break;
				case 2:  context = { "h2", true }; break;
				case 3:  context = { "h3", true }; break;
				case 4:  context = { "h4", true }; break;
				case 5:  context = { "h5", true }; break;
				default: context = { "h6", true }; break;
				}
				else
					context = { "p", false };
				break;
			case css::display::table:
				context = { "table", false };
				break;
			case css::display::table_row:
				context = { "tr", false };
				break;
			case css::display::table_cell:
				context = { "td", false };
				break;
			case css::display::list_item:
				context = { "li", false };
				break;
			default:
				break;
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
			if (!reader->content->empty())
				text += reader->content;
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

static void writeMediaOverlays(std::shared_ptr<cainteoir::document_reader> reader)
{
	int depth = 0;
	int media_overlay_depth = -1;
	cainteoir::rope text;
	rdf::uri audio;
	css::time media_begin;
	css::time media_end;
	uint32_t from = 0;
	uint32_t to = 0;
	fprintf(stdout, "AudioPath,StartTime,EndTime,FromChar,ToChar,Text\n");
	while (reader->read())
	{
		if ((reader->type & cainteoir::events::end_context && depth == media_overlay_depth) ||
		    (reader->type & cainteoir::events::media_ref && media_overlay_depth != -1))
		{
			auto output = text.buffer();
			auto audiofile = cainteoir::path(audio.str()).zip_path();

			fprintf(stdout, "%s,", audiofile.str().c_str());
			fprintf(stdout, "%G,%G,", media_begin.value(), media_end.value());
			fprintf(stdout, "%u,%u,", from, to);
			fwrite(output->begin(), 1, output->size(), stdout);
			fwrite("\n", 1, 1, stdout);

			media_overlay_depth = -1;
			text.clear();
		}

		if (reader->type & cainteoir::events::begin_context)
		{
			++depth;
		}

		if (reader->type & cainteoir::events::end_context)
		{
			--depth;
		}

		if (reader->type & cainteoir::events::media_ref)
		{
			audio = reader->anchor;
			media_begin = reader->media_begin.as(css::time::seconds);
			media_end = reader->media_end.as(css::time::seconds);
			media_overlay_depth = depth;
			from = to;
		}

		if (reader->type & cainteoir::events::text)
		{
			if (media_overlay_depth != -1)
				text += reader->content;

			to += reader->content->size();
		}
	}
}

int main(int argc, char ** argv)
{
	try
	{
		bool show_toc = false;
		decltype(writeTextDocument) *writer = nullptr;
		std::pair<size_t, size_t> nav_range = { -1, -1 };

		const option_group general_options = { nullptr, {
			{ 0, "text", bind_value(writer, &writeTextDocument),
			  i18n("Output as plain text.") },
			{ 0, "html", bind_value(writer, &writeHtmlDocument),
			  i18n("Output as a HTML document.") },
			{ 0, "media-overlays", bind_value(writer, &writeMediaOverlays),
			  i18n("Output media overlay information.") },
		}};

		const option_group toc_options = { i18n("Table of Contents:"), {
			{ 'c', "contents", bind_value(show_toc, true),
			  i18n("List the table of contents for the specified document") },
			{ 'f', "from", nav_range.first, "FROM",
			  i18n("Start reading/recoding from contents marker FROM") },
			{ 't', "to", nav_range.second, "TO",
			  i18n("Finish reading/recording after contents marker TO") },
		}};

		const std::initializer_list<const char *> usage = {
			i18n("doc2doc [OPTION..] DOCUMENT"),
			i18n("doc2doc [OPTION..]"),
		};

		if (!parse_command_line({ general_options, toc_options }, usage, argc, argv))
			return 0;

		const char *filename = (argc == 1) ? argv[0] : nullptr;
		rdf::uri subject(filename ? filename : std::string(), std::string());

		if (!writer && !show_toc)
			throw std::runtime_error(i18n("unsupported format to convert to (html and text only)"));

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, i18n("unsupported document format for file \"%s\"\n"), filename ? filename : "<stdin>");
			return EXIT_FAILURE;
		}

		cainteoir::document doc(reader, metadata);
		auto listing = cainteoir::navigation(metadata, subject, rdf::epv("toc"));

		if (show_toc)
		{
			int toc_number = 1;
			for (auto &entry : listing)
			{
				printf(" %4d ", toc_number);
				for (int i = 0; i < entry.depth; ++i)
					printf("... ");
				printf("%s\n", entry.title.c_str());
				++toc_number;
			}
			return 0;
		}

		auto children = cainteoir::createDocumentReader(doc.children(listing, nav_range));
		writer(children);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
