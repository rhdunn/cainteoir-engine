/* XHTML Document Parser.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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

#include "parsers.hpp"
#include <cainteoir/xmlreader.hpp>
#include <algorithm>
#include <stack>

namespace xml   = cainteoir::xml;
namespace xmlns = cainteoir::xml::xmlns;
namespace rdf   = cainteoir::rdf;

enum html_node
{
	node_unknown,
	attr_id,
	attr_lang,
	node_head,
	node_html,
	node_li,
	node_link,
	node_meta,
	node_script,
	node_style,
	node_title,
};

namespace html
{
	namespace events = cainteoir::events;

	// HTML§12.1.2 -- void elements
	// HTML§14.3.* -- default rendering (styles)

	static const xml::context::entry a_node          = { events::unknown,   0 };
	static const xml::context::entry abbr_node       = { events::unknown,   0 };
	static const xml::context::entry address_node    = { events::paragraph, events::emphasized }; // HTML§14.3.3
	static const xml::context::entry acronym_node    = { events::unknown,   0 };
	static const xml::context::entry applet_node     = { events::unknown,   0 };
	static const xml::context::entry area_node       = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry article_node    = { events::unknown,   0 };
	static const xml::context::entry aside_node      = { events::unknown,   0 };
	static const xml::context::entry audio_node      = { events::unknown,   0 };
	static const xml::context::entry b_node          = { events::span,      events::strong }; // HTML§14.3.4
	static const xml::context::entry base_node       = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry basefont_node   = { events::unknown,   0 };
	static const xml::context::entry bdi_node        = { events::unknown,   0 };
	static const xml::context::entry bdo_node        = { events::unknown,   0 };
	static const xml::context::entry big_node        = { events::unknown,   0 };
	static const xml::context::entry blockquote_node = { events::unknown,   0 };
	static const xml::context::entry body_node       = { events::unknown,   0 };
	static const xml::context::entry br_node         = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry button_node     = { events::unknown,   0 };
	static const xml::context::entry canvas_node     = { events::unknown,   0 };
	static const xml::context::entry caption_node    = { events::unknown,   0 };
	static const xml::context::entry center_node     = { events::unknown,   0 };
	static const xml::context::entry cite_node       = { events::span,      events::emphasized }; // HTML§14.3.4
	static const xml::context::entry code_node       = { events::span,      events::monospace  }; // HTML§14.3.4
	static const xml::context::entry col_node        = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry colgroup_node   = { events::unknown,   0 };
	static const xml::context::entry command_node    = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry data_node       = { events::unknown,   0 };
	static const xml::context::entry datalist_node   = { events::unknown,   0 };
	static const xml::context::entry dd_node         = { events::unknown,   0 };
	static const xml::context::entry del_node        = { events::unknown,   0 };
	static const xml::context::entry details_node    = { events::unknown,   0 };
	static const xml::context::entry dfn_node        = { events::span,      events::emphasized }; // HTML§14.3.4
	static const xml::context::entry dir_node        = { events::list,      events::bullet }; // HTML§14.3.8
	static const xml::context::entry div_node        = { events::paragraph, 0 };
	static const xml::context::entry dl_node         = { events::unknown,   0 };
	static const xml::context::entry dt_node         = { events::unknown,   0 };
	static const xml::context::entry em_node         = { events::span,      events::emphasized }; // HTML§14.3.4
	static const xml::context::entry embed_node      = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry fieldset_node   = { events::unknown,   0 };
	static const xml::context::entry figcaption_node = { events::unknown,   0 };
	static const xml::context::entry figure_node     = { events::unknown,   0 };
	static const xml::context::entry font_node       = { events::unknown,   0 };
	static const xml::context::entry footer_node     = { events::unknown,   0 };
	static const xml::context::entry form_node       = { events::unknown,   0 };
	static const xml::context::entry frame_node      = { events::unknown,   0 };
	static const xml::context::entry frameset_node   = { events::unknown,   0 };
	static const xml::context::entry h1_node         = { events::heading,   1 };
	static const xml::context::entry h2_node         = { events::heading,   2 };
	static const xml::context::entry h3_node         = { events::heading,   3 };
	static const xml::context::entry h4_node         = { events::heading,   4 };
	static const xml::context::entry h5_node         = { events::heading,   5 };
	static const xml::context::entry h6_node         = { events::heading,   6 };
	static const xml::context::entry head_node       = { events::unknown,   0 };
	static const xml::context::entry header_node     = { events::unknown,   0 };
	static const xml::context::entry hgroup_node     = { events::unknown,   0 };
	static const xml::context::entry hr_node         = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry html_node       = { events::unknown,   0 };
	static const xml::context::entry i_node          = { events::span,      events::emphasized }; // HTML§14.3.4
	static const xml::context::entry iframe_node     = { events::unknown,   0 };
	static const xml::context::entry img_node        = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry input_node      = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry ins_node        = { events::unknown,   0 };
	static const xml::context::entry isindex_node    = { events::unknown,   0 };
	static const xml::context::entry kbd_node        = { events::unknown,   0 };
	static const xml::context::entry keygen_node     = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry label_node      = { events::unknown,   0 };
	static const xml::context::entry legend_node     = { events::unknown,   0 };
	static const xml::context::entry li_node         = { events::unknown,   0 };
	static const xml::context::entry link_node       = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry map_node        = { events::unknown,   0 };
	static const xml::context::entry mark_node       = { events::unknown,   0 };
	static const xml::context::entry marquee_node    = { events::unknown,   0 };
	static const xml::context::entry menu_node       = { events::list,      events::bullet }; // HTML§14.3.8
	static const xml::context::entry meta_node       = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry meter_node      = { events::unknown,   0 };
	static const xml::context::entry nav_node        = { events::unknown,   0 };
	static const xml::context::entry noad_node       = { events::unknown,   0, xml::context::implicit_end_tag }; // ad-sense markup? (e.g. m.fanfiction.net)
	static const xml::context::entry noframes_node   = { events::unknown,   0 };
	static const xml::context::entry noscript_node   = { events::unknown,   0 };
	static const xml::context::entry object_node     = { events::unknown,   0 };
	static const xml::context::entry ol_node         = { events::list,      events::number }; // HTML§14.3.8
	static const xml::context::entry optgroup_node   = { events::unknown,   0 };
	static const xml::context::entry option_node     = { events::unknown,   0 };
	static const xml::context::entry output_node     = { events::unknown,   0 };
	static const xml::context::entry p_node          = { events::paragraph, 0 };
	static const xml::context::entry param_node      = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry pre_node        = { events::paragraph, events::monospace }; // HTML§14.3.3
	static const xml::context::entry progress_node   = { events::unknown,   0 };
	static const xml::context::entry q_node          = { events::unknown,   0 };
	static const xml::context::entry rp_node         = { events::unknown,   0 };
	static const xml::context::entry rt_node         = { events::unknown,   0 };
	static const xml::context::entry ruby_node       = { events::unknown,   0 };
	static const xml::context::entry s_node          = { events::unknown,   0 };
	static const xml::context::entry samp_node       = { events::unknown,   0 };
	static const xml::context::entry script_node     = { events::unknown,   0, xml::context::hidden }; // HTML§14.3.1
	static const xml::context::entry section_node    = { events::unknown,   0 };
	static const xml::context::entry select_node     = { events::unknown,   0 };
	static const xml::context::entry small_node      = { events::unknown,   0 };
	static const xml::context::entry source_node     = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry span_node       = { events::unknown,   0 };
	static const xml::context::entry strike_node     = { events::unknown,   0 };
	static const xml::context::entry strong_node     = { events::span,      events::strong }; // HTML§14.3.4
	static const xml::context::entry style_node      = { events::unknown,   0, xml::context::hidden }; // HTML§14.3.1
	static const xml::context::entry sub_node        = { events::span,      events::subscript }; // HTML§14.3.4
	static const xml::context::entry summary_node    = { events::unknown,   0 };
	static const xml::context::entry sup_node        = { events::span,      events::superscript }; // HTML§14.3.4
	static const xml::context::entry table_node      = { events::unknown,   0 };
	static const xml::context::entry tbody_node      = { events::unknown,   0 };
	static const xml::context::entry td_node         = { events::unknown,   0 };
	static const xml::context::entry textarea_node   = { events::unknown,   0 };
	static const xml::context::entry tfoot_node      = { events::unknown,   0 };
	static const xml::context::entry th_node         = { events::unknown,   0 };
	static const xml::context::entry thead_node      = { events::unknown,   0 };
	static const xml::context::entry time_node       = { events::unknown,   0 };
	static const xml::context::entry title_node      = { events::unknown,   0 };
	static const xml::context::entry tr_node         = { events::unknown,   0 };
	static const xml::context::entry track_node      = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2
	static const xml::context::entry tt_node         = { events::span,      events::monospace }; // HTML§14.3.4
	static const xml::context::entry u_node          = { events::span,      events::underline }; // HTML§14.3.4
	static const xml::context::entry ul_node         = { events::list,      events::bullet }; // HTML§14.3.8
	static const xml::context::entry var_node        = { events::span,      events::emphasized }; // HTML§14.3.4
	static const xml::context::entry video_node      = { events::unknown,   0 };
	static const xml::context::entry wbr_node        = { events::unknown,   0, xml::context::implicit_end_tag }; // HTML§12.1.2

	static const xml::context::entry name_attr       = { events::unknown,   0 };
	static const xml::context::entry content_attr    = { events::unknown,   0 };
}

static const std::initializer_list<const xml::context::entry_ref> html_nodes =
{
	{ "a",          &html::a_node },
	{ "abbr",       &html::abbr_node },
	{ "address",    &html::address_node },
	{ "acronym",    &html::acronym_node }, // html4
	{ "applet",     &html::applet_node },
	{ "area",       &html::area_node },
	{ "article",    &html::article_node },
	{ "aside",      &html::aside_node },
	{ "audio",      &html::audio_node },
	{ "b",          &html::b_node },
	{ "base",       &html::base_node },
	{ "basefont",   &html::basefont_node }, // html4
	{ "bdi",        &html::bdi_node },
	{ "bdo",        &html::bdo_node },
	{ "big",        &html::big_node }, // html4
	{ "blockquote", &html::blockquote_node },
	{ "body",       &html::body_node },
	{ "br",         &html::br_node },
	{ "button",     &html::button_node },
	{ "canvas",     &html::canvas_node },
	{ "caption",    &html::caption_node },
	{ "center",     &html::center_node }, // html4
	{ "cite",       &html::cite_node },
	{ "code",       &html::code_node },
	{ "col",        &html::col_node },
	{ "colgroup",   &html::colgroup_node },
	{ "command",    &html::command_node },
	{ "data",       &html::data_node },
	{ "datalist",   &html::datalist_node },
	{ "dd",         &html::dd_node },
	{ "del",        &html::del_node },
	{ "details",    &html::details_node },
	{ "dfn",        &html::dfn_node },
	{ "dir",        &html::dir_node },
	{ "div",        &html::div_node },
	{ "dl",         &html::dl_node },
	{ "dt",         &html::dt_node },
	{ "em",         &html::em_node },
	{ "embed",      &html::embed_node },
	{ "fieldset",   &html::fieldset_node },
	{ "figcaption", &html::figcaption_node },
	{ "figure",     &html::figure_node },
	{ "font",       &html::font_node }, // html4
	{ "footer",     &html::footer_node },
	{ "form",       &html::form_node },
	{ "frame",      &html::frame_node }, // html4
	{ "frameset",   &html::frameset_node }, // html4
	{ "h1",         &html::h1_node },
	{ "h2",         &html::h2_node },
	{ "h3",         &html::h3_node },
	{ "h4",         &html::h4_node },
	{ "h5",         &html::h5_node },
	{ "h6",         &html::h6_node },
	{ "head",       &html::head_node },
	{ "header",     &html::header_node },
	{ "hgroup",     &html::hgroup_node },
	{ "hr",         &html::hr_node },
	{ "html",       &html::html_node },
	{ "i",          &html::i_node },
	{ "iframe",     &html::iframe_node },
	{ "img",        &html::img_node },
	{ "input",      &html::input_node },
	{ "ins",        &html::ins_node },
	{ "isindex",    &html::isindex_node }, // html4
	{ "kbd",        &html::kbd_node },
	{ "keygen",     &html::keygen_node },
	{ "label",      &html::label_node },
	{ "legend",     &html::legend_node },
	{ "li",         &html::li_node },
	{ "link",       &html::link_node },
	{ "map",        &html::map_node },
	{ "mark",       &html::mark_node },
	{ "marquee",    &html::marquee_node },
	{ "menu",       &html::menu_node },
	{ "meta",       &html::meta_node },
	{ "meter",      &html::meter_node },
	{ "nav",        &html::nav_node },
	{ "noad",       &html::noad_node }, // adsense, e.g. m.fanfiction.net
	{ "noframes",   &html::noframes_node }, // html4
	{ "noscript",   &html::noscript_node },
	{ "object",     &html::object_node },
	{ "ol",         &html::ol_node },
	{ "optgroup",   &html::optgroup_node },
	{ "option",     &html::option_node },
	{ "output",     &html::output_node },
	{ "p",          &html::p_node },
	{ "param",      &html::param_node },
	{ "pre",        &html::pre_node },
	{ "progress",   &html::progress_node },
	{ "q",          &html::q_node },
	{ "rp",         &html::rp_node },
	{ "rt",         &html::rt_node },
	{ "ruby",       &html::ruby_node },
	{ "s",          &html::s_node },
	{ "samp",       &html::samp_node },
	{ "script",     &html::script_node },
	{ "section",    &html::section_node },
	{ "select",     &html::select_node },
	{ "small",      &html::small_node },
	{ "source",     &html::source_node },
	{ "span",       &html::span_node },
	{ "strike",     &html::strike_node }, // html4
	{ "strong",     &html::strong_node },
	{ "style",      &html::style_node },
	{ "sub",        &html::sub_node },
	{ "summary",    &html::summary_node },
	{ "sup",        &html::sup_node },
	{ "table",      &html::table_node },
	{ "tbody",      &html::tbody_node },
	{ "td",         &html::td_node },
	{ "textarea",   &html::textarea_node },
	{ "tfoot",      &html::tfoot_node },
	{ "th",         &html::th_node },
	{ "thead",      &html::thead_node },
	{ "time",       &html::time_node },
	{ "title",      &html::title_node },
	{ "tr",         &html::tr_node },
	{ "track",      &html::track_node },
	{ "tt",         &html::tt_node },
	{ "u",          &html::u_node },
	{ "ul",         &html::ul_node },
	{ "var",        &html::var_node },
	{ "video",      &html::video_node },
	{ "wbr",        &html::wbr_node },
};

static const std::initializer_list<const xml::context::entry_ref> html_attrs =
{
	{ "content", &html::content_attr },
	{ "id",      &xml::id_attr },
	{ "lang",    &xml::lang_attr },
	{ "name",    &html::name_attr },
};

void skipNode(xml::reader &reader, const cainteoir::buffer name)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
			return;
		break;
	}
}

std::string parseTitleNode(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	std::string title;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (reader.context() == &html::title_node)
			return title;
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		{
			title = reader.nodeValue().normalize()->str();
			if (!title.empty())
			{
				aGraph.statement(aSubject, rdf::dc("title"), rdf::literal(title));
				events.toc_entry(0, aSubject, title);
			}
		}
		break;
	}
	return title;
}

void parseMetaNode(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	std::string name;
	std::string lang;
	std::string content;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (reader.context() == &html::meta_node)
		{
			if (name == "shs-author")
			{
				aGraph.statement(aSubject, rdf::dc("creator"), rdf::literal(content));
			}
			else if (name == "shs-keywords")
			{
				std::istringstream keywords(content);
				std::string keyword;
				while (std::getline(keywords, keyword, ','))
				{
					keyword.erase(0, keyword.find_first_not_of(" \t"));
					keyword.erase(keyword.find_last_not_of(" \t")+1);

					aGraph.statement(aSubject, rdf::dc("subject"), rdf::literal(keyword));
				}
			}
			else if (name == "shs-summary")
			{
				aGraph.statement(aSubject, rdf::dc("description"), rdf::literal(content, lang));
			}
			else if (name == "shs-title")
			{
				aGraph.statement(aSubject, rdf::dc("title"), rdf::literal(content, lang));
			}
			return;
		}
		break;
	case xml::reader::attribute:
		if (reader.context() == &html::name_attr)
			name = reader.nodeValue().normalize()->str();
		else if (reader.context() == &html::content_attr)
			content = reader.nodeValue().normalize()->str();
		else if (reader.context() == &xml::lang_attr)
			lang = reader.nodeValue().normalize()->str();
		break;
	}
}

std::string parseHeadNode(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	std::string title;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context() == &html::title_node)
			title = parseTitleNode(reader, aSubject, events, aGraph);
		else if (reader.context() == &html::meta_node)
			parseMetaNode(reader, aSubject, events, aGraph);
		else if (reader.context()->parse_type == xml::context::hidden)
			skipNode(reader, reader.nodeName());
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &html::head_node)
			return title;
		break;
	}
	return title;
}

void parseListNode(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, const xml::context::entry *list_ctx)
{
	int number = 1;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context() == &html::li_node)
		{
			events.begin_context(cainteoir::events::list_item);
			if (list_ctx->parameter == cainteoir::events::bullet)
				events.text(std::make_shared<cainteoir::buffer>("\xE2\x80\xA2 "));
			else
			{
				char text[100];
				int len = snprintf(text, sizeof(text), "%d. ", number);
				text[len] = '\0';

				std::shared_ptr<cainteoir::buffer> data = std::make_shared<cainteoir::data_buffer>(len);
				strcpy((char *)data->begin(), text);
				events.text(data);

				++number;
			}
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		{
			std::shared_ptr<cainteoir::buffer> text = reader.nodeValue().content();
			if (text)
				events.text(text);
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context() == list_ctx)
		{
			events.end_context();
			return;
		}
		if (reader.context() == &html::li_node)
			events.end_context();
		break;
	}
}

void parseBodyNode(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph, const xml::context::entry *body_ctx, std::string title, bool first)
{
	rdf::uri href(aSubject.str(), std::string());
	cainteoir::rope htext;
	int hid = 0;
	bool genAnchor = false;

	if (title.empty() && first)
	{
		std::string title = aSubject.str();
		std::string::size_type sep = title.rfind('/');
		if (sep != std::string::npos)
			title = title.substr(sep + 1);

		events.toc_entry(0, aSubject, title);
		events.anchor(aSubject, std::string());
	}

	std::string lang;
	bool in_body = true;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &xml::id_attr)
		{
			href = rdf::uri(aSubject.str(), reader.nodeValue().str());
			events.anchor(href, std::string());
			genAnchor = false;
		}
		else if (in_body && reader.context() == &xml::lang_attr && lang.empty())
		{
			lang = reader.nodeValue().buffer()->str();
			aGraph.statement(aSubject, rdf::dc("language"), rdf::literal(lang));
		}
		break;
	case xml::reader::beginTagNode:
		in_body = false;
		if (reader.context() == &html::script_node || reader.context() == &html::style_node)
			skipNode(reader, reader.nodeName());
		else if (reader.context()->context != cainteoir::events::unknown)
		{
			events.begin_context((cainteoir::events::context)reader.context()->context, reader.context()->parameter);
			if (reader.context()->context == cainteoir::events::list)
				parseListNode(reader, aSubject, events, reader.context());

			if (reader.context()->context == cainteoir::events::heading)
			{
				htext.clear();
				genAnchor = true;
			}
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		{
			std::shared_ptr<cainteoir::buffer> text = reader.nodeValue().content();
			bool is_title_header = text && text->compare(title.c_str()) == 0;

			if (genAnchor)
			{
				if (!is_title_header)
				{
					std::stringstream ref;
					ref << "genid.h" << hid;

					++hid;
					href = rdf::uri(aSubject.str(), ref.str());
					events.anchor(href, std::string());
				}
				genAnchor = false;
			}

			if (text)
			{
				events.text(text);
				if (!is_title_header)
					htext += text;
			}
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context() == body_ctx)
			return;
		if (reader.context()->context != cainteoir::events::unknown)
		{
			events.end_context();
			if (reader.context()->context == cainteoir::events::heading)
				title.clear();

			if (reader.context()->context == cainteoir::events::heading && !htext.empty())
			{
				std::shared_ptr<cainteoir::buffer> text = htext.normalize();
				for (char *c = (char *)text->begin(), *last = (char *)text->end(); c != last; ++c)
				{
					switch (*c)
					{
					case '\n':
						*c = ' ';
						break;
					default:
						break;
					}
				}
				events.toc_entry(0, href, text->str());
				href.ref = std::string();
			}
		}
		break;
	}
}

void parseHtmlNode(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	std::string lang;
	std::string title;
	bool first = true;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context() == &html::head_node)
			title = parseHeadNode(reader, aSubject, events, aGraph);
		else
		{
			parseBodyNode(reader, aSubject, events, aGraph, reader.context(), title, first);
			first = false;
		}
		break;
	case xml::reader::attribute:
		if (reader.context() == &xml::lang_attr && lang.empty())
		{
			lang = reader.nodeValue().buffer()->str();
			aGraph.statement(aSubject, rdf::dc("language"), rdf::literal(lang));
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &html::html_node)
			return;
	}
}

void cainteoir::parseXHtmlDocument(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	reader.set_predefined_entities(xml::html_entities);
	reader.set_nodes(std::string(), html_nodes, cainteoir::buffer::ignore_case);
	reader.set_attrs(std::string(), html_attrs, cainteoir::buffer::ignore_case);
	reader.set_nodes(xmlns::xhtml,  html_nodes);
	reader.set_attrs(xmlns::xhtml,  html_attrs);
	reader.set_attrs(xmlns::xml,    xml::attrs);

	parseHtmlNode(reader, aSubject, events, aGraph);
}

void cainteoir::parseHtmlDocument(std::shared_ptr<cainteoir::buffer> data, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	xml::reader reader(data, xml::html_entities);
	reader.set_nodes(std::string(), html_nodes, cainteoir::buffer::ignore_case);
	reader.set_attrs(std::string(), html_attrs, cainteoir::buffer::ignore_case);
	reader.set_attrs(xmlns::xml,    xml::attrs);

	std::string title;
	bool first = true;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context() == &html::html_node)
			parseHtmlNode(reader, aSubject, events, aGraph);
		else if (reader.context() == &html::head_node)
			title = parseHeadNode(reader, aSubject, events, aGraph);
		else
		{
			parseBodyNode(reader, aSubject, events, aGraph, reader.context(), title, first);
			first = false;
		}
		break;
	}
}

/** References
  *
  *    HTML [http://www.whatwg.org/specs/web-apps/current-work/multipage/] -- HTML Living Standard
  */
