/* XHTML Document Parser.
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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
#include "parsers.hpp"
#include <algorithm>
#include <stack>

namespace xml    = cainteoir::xml;
namespace xmlns  = cainteoir::xml::xmlns;
namespace events = cainteoir::events;
namespace rdf    = cainteoir::rdf;

namespace html
{
	// HTML§12.1.2 -- void elements
	// HTML§14.3.* -- default rendering (styles)

	static const xml::context::entry a_node          = {};
	static const xml::context::entry abbr_node       = {};
	static const xml::context::entry address_node    = { &cainteoir::emphasized_block }; // HTML§14.3.3
	static const xml::context::entry acronym_node    = {};
	static const xml::context::entry applet_node     = {};
	static const xml::context::entry area_node       = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry article_node    = {};
	static const xml::context::entry aside_node      = {};
	static const xml::context::entry audio_node      = { &cainteoir::hidden };
	static const xml::context::entry b_node          = { &cainteoir::strong }; // HTML§14.3.4
	static const xml::context::entry base_node       = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry basefont_node   = { xml::begin_tag_type::open_close }; // HTML§12.2.5.4.4
	static const xml::context::entry bdi_node        = {};
	static const xml::context::entry bdo_node        = {};
	static const xml::context::entry bgsound_node    = { xml::begin_tag_type::open_close }; // HTML§12.2.5.4.4
	static const xml::context::entry big_node        = {};
	static const xml::context::entry blockquote_node = {};
	static const xml::context::entry body_node       = {};
	static const xml::context::entry br_node         = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry button_node     = {};
	static const xml::context::entry canvas_node     = {};
	static const xml::context::entry caption_node    = {};
	static const xml::context::entry center_node     = {};
	static const xml::context::entry cite_node       = { &cainteoir::emphasized }; // HTML§14.3.4
	static const xml::context::entry code_node       = { &cainteoir::monospace  }; // HTML§14.3.4
	static const xml::context::entry col_node        = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry colgroup_node   = {};
	static const xml::context::entry command_node    = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry data_node       = {};
	static const xml::context::entry datalist_node   = {};
	static const xml::context::entry dd_node         = {};
	static const xml::context::entry del_node        = {};
	static const xml::context::entry details_node    = {};
	static const xml::context::entry dfn_node        = { &cainteoir::emphasized }; // HTML§14.3.4
	static const xml::context::entry dir_node        = { &cainteoir::bullet_list }; // HTML§14.3.8
	static const xml::context::entry div_node        = { &cainteoir::paragraph };
	static const xml::context::entry dl_node         = {};
	static const xml::context::entry dt_node         = {};
	static const xml::context::entry em_node         = { &cainteoir::emphasized }; // HTML§14.3.4
	static const xml::context::entry embed_node      = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry fieldset_node   = {};
	static const xml::context::entry figcaption_node = {};
	static const xml::context::entry figure_node     = {};
	static const xml::context::entry font_node       = {};
	static const xml::context::entry footer_node     = {};
	static const xml::context::entry form_node       = {};
	static const xml::context::entry frame_node      = {};
	static const xml::context::entry frameset_node   = {};
	static const xml::context::entry h1_node         = { &cainteoir::heading1 };
	static const xml::context::entry h2_node         = { &cainteoir::heading2 };
	static const xml::context::entry h3_node         = { &cainteoir::heading3 };
	static const xml::context::entry h4_node         = { &cainteoir::heading4 };
	static const xml::context::entry h5_node         = { &cainteoir::heading5 };
	static const xml::context::entry h6_node         = { &cainteoir::heading6 };
	static const xml::context::entry head_node       = {};
	static const xml::context::entry header_node     = {};
	static const xml::context::entry hgroup_node     = {};
	static const xml::context::entry hr_node         = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry html_node       = {};
	static const xml::context::entry i_node          = { &cainteoir::emphasized }; // HTML§14.3.4
	static const xml::context::entry iframe_node     = {};
	static const xml::context::entry img_node        = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry input_node      = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry ins_node        = {};
	static const xml::context::entry isindex_node    = {};
	static const xml::context::entry kbd_node        = {};
	static const xml::context::entry keygen_node     = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry label_node      = {};
	static const xml::context::entry legend_node     = {};
	static const xml::context::entry li_node         = { &cainteoir::list_item };
	static const xml::context::entry link_node       = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry map_node        = {};
	static const xml::context::entry mark_node       = {};
	static const xml::context::entry marquee_node    = {};
	static const xml::context::entry menu_node       = { &cainteoir::bullet_list }; // HTML§14.3.8
	static const xml::context::entry meta_node       = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry meter_node      = {};
	static const xml::context::entry nav_node        = {};
	static const xml::context::entry noad_node       = { xml::begin_tag_type::open_close }; // ad-sense markup? (e.g. m.fanfiction.net)
	static const xml::context::entry noframes_node   = {};
	static const xml::context::entry noscript_node   = {};
	static const xml::context::entry object_node     = {};
	static const xml::context::entry ol_node         = { &cainteoir::number_list }; // HTML§14.3.8
	static const xml::context::entry optgroup_node   = {};
	static const xml::context::entry option_node     = {};
	static const xml::context::entry output_node     = {};
	static const xml::context::entry p_node          = { &cainteoir::paragraph };
	static const xml::context::entry param_node      = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry pre_node        = { &cainteoir::monospace_block }; // HTML§14.3.3
	static const xml::context::entry progress_node   = {};
	static const xml::context::entry q_node          = {};
	static const xml::context::entry rp_node         = {};
	static const xml::context::entry rt_node         = {};
	static const xml::context::entry ruby_node       = {};
	static const xml::context::entry s_node          = {};
	static const xml::context::entry samp_node       = {};
	static const xml::context::entry script_node     = { &cainteoir::hidden }; // HTML§14.3.1
	static const xml::context::entry section_node    = {};
	static const xml::context::entry select_node     = {};
	static const xml::context::entry small_node      = {};
	static const xml::context::entry source_node     = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry span_node       = {};
	static const xml::context::entry strike_node     = {};
	static const xml::context::entry strong_node     = { &cainteoir::strong }; // HTML§14.3.4
	static const xml::context::entry style_node      = { &cainteoir::hidden }; // HTML§14.3.1
	static const xml::context::entry sub_node        = { &cainteoir::subscript }; // HTML§14.3.4
	static const xml::context::entry summary_node    = {};
	static const xml::context::entry sup_node        = { &cainteoir::superscript }; // HTML§14.3.4
	static const xml::context::entry table_node      = { &cainteoir::table };
	static const xml::context::entry tbody_node      = {};
	static const xml::context::entry td_node         = { &cainteoir::table_cell };
	static const xml::context::entry textarea_node   = {};
	static const xml::context::entry tfoot_node      = {};
	static const xml::context::entry th_node         = { &cainteoir::table_cell };
	static const xml::context::entry thead_node      = {};
	static const xml::context::entry time_node       = {};
	static const xml::context::entry title_node      = {};
	static const xml::context::entry tr_node         = { &cainteoir::table_row };
	static const xml::context::entry track_node      = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry tt_node         = { &cainteoir::monospace }; // HTML§14.3.4
	static const xml::context::entry u_node          = { &cainteoir::underlined }; // HTML§14.3.4
	static const xml::context::entry ul_node         = { &cainteoir::bullet_list }; // HTML§14.3.8
	static const xml::context::entry var_node        = { &cainteoir::emphasized }; // HTML§14.3.4
	static const xml::context::entry video_node      = {};
	static const xml::context::entry wbr_node        = { xml::begin_tag_type::open_close }; // HTML§12.1.2

	static const xml::context::entry charset_attr    = {};
	static const xml::context::entry color_attr      = {};
	static const xml::context::entry content_attr    = {};
	static const xml::context::entry href_attr       = {};
	static const xml::context::entry http_equiv_attr = {};
	static const xml::context::entry name_attr       = {};
	static const xml::context::entry rel_attr        = {};
	static const xml::context::entry src_attr        = {};

	static const xml::context::entry abstract_meta    = {};
	static const xml::context::entry creator_meta     = {};
	static const xml::context::entry description_meta = {};
	static const xml::context::entry keywords_meta    = {};
	static const xml::context::entry title_meta       = {};
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
	{ "bgsound",    &html::bgsound_node }, // html4
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
	{ "charset",    &html::charset_attr },
	{ "color",      &html::color_attr },
	{ "content",    &html::content_attr },
	{ "href",       &html::href_attr },
	{ "http-equiv", &html::http_equiv_attr },
	{ "id",         &xml::id_attr },
	{ "lang",       &xml::lang_attr },
	{ "name",       &html::name_attr },
	{ "rel",        &html::rel_attr },
	{ "src",        &html::src_attr },
};

static const std::initializer_list<const xml::context::entry_ref> meta_names =
{
	{ "abstract",     &html::abstract_meta },
	{ "author",       &html::creator_meta },
	{ "description",  &html::description_meta },
	{ "keywords",     &html::keywords_meta },
	{ "shs-author",   &html::creator_meta },
	{ "shs-keywords", &html::keywords_meta },
	{ "shs-summary",  &html::description_meta },
	{ "shs-title",    &html::title_meta },
};

/******************************************************************************
 * HTML Tree Construction
 */

struct html_tree_builder
{
	html_tree_builder(const std::shared_ptr<xml::reader> &aReader);

	bool read();

	xml::reader::node_type nodeType() const { return mNodeType; }

	const cainteoir::rope &nodeValue() const { return reader->nodeValue(); }

	const xml::context::entry *context() const { return mContext; }

	void set_encoding(const char *encoding) { reader->set_encoding(encoding); }
private:
	std::shared_ptr<xml::reader> reader;
	bool mReprocessToken;
	std::stack<const xml::context::entry *> mOpenElements;
	xml::reader::node_type mNodeType;
	const xml::context::entry *mContext;

	void insert_open_tag(const xml::context::entry *aOpenTag);
	void insert_close_tag(const xml::context::entry *aCloseTag);

	void push_open_tag(const xml::context::entry *aOpenTag);
	void pop_open_tag(const xml::context::entry *aOpenTag);

	bool next_node();
	bool before_html(); // HTML§12.2.5.4.2 - before html
	bool before_head(); // HTML§12.2.5.4.3 - before head
	bool in_head(); // HTML§12.2.5.4.4 - in head
	bool after_head(); // HTML§12.2.5.4.6 - after head
	bool in_body(); // HTML§12.2.5.4.7 - in body
	bool after_body(); // HTML§12.2.5.4.19 - after body
	bool after_after_body(); // HTML§12.2.5.4.22 - after after body

	decltype(&html_tree_builder::before_html) mInsertionMode;
};

html_tree_builder::html_tree_builder(const std::shared_ptr<xml::reader> &aReader)
	: reader(aReader)
	, mReprocessToken(true) // createXmlReader is pointing to the root html node
	, mInsertionMode(&html_tree_builder::before_html) // createXmlReader processes doctype & comments
	, mNodeType(xml::reader::error)
	, mContext(&xml::unknown_context)
{
	reader->set_predefined_entities(xml::html_entities);
	reader->set_nodes(std::string(), html_nodes, cainteoir::buffer::ignore_case);
	reader->set_attrs(std::string(), html_attrs, cainteoir::buffer::ignore_case);
	reader->set_nodes(xmlns::xhtml,  html_nodes);
	reader->set_attrs(xmlns::xhtml,  html_attrs);
	reader->set_nodes(xmlns::html40, html_nodes);
	reader->set_attrs(xmlns::html40, html_attrs);
	reader->set_attrs(xmlns::xml,    xml::attrs);
}

bool html_tree_builder::read()
{
	return (this->*mInsertionMode)();
}

void html_tree_builder::insert_open_tag(const xml::context::entry *aOpenTag)
{
	mReprocessToken = true;
	mNodeType = xml::reader::beginTagNode;
	mContext = aOpenTag;
}

void html_tree_builder::insert_close_tag(const xml::context::entry *aCloseTag)
{
	mReprocessToken = true;
	mNodeType = xml::reader::endTagNode;
	mContext = aCloseTag;
}

void html_tree_builder::push_open_tag(const xml::context::entry *aOpenTag)
{
	mOpenElements.push(aOpenTag);
}

void html_tree_builder::pop_open_tag(const xml::context::entry *aOpenTag)
{
	mOpenElements.pop();
}

bool html_tree_builder::next_node()
{
	if (mReprocessToken)
	{
		mReprocessToken = false;
		mNodeType = reader->nodeType();
		mContext = reader->context();
		return mNodeType != xml::reader::endOfData;
	}
	if (!reader->read())
		return false;

	mNodeType = reader->nodeType();
	mContext = reader->context();
	return true;
}

bool html_tree_builder::before_html()
{
	while (next_node()) switch (nodeType())
	{
	case xml::reader::beginTagNode:
		if (context() == &html::html_node)
		{
			mInsertionMode = &html_tree_builder::before_head;
			push_open_tag(&html::html_node);
			return true;
		}
		else
		{
			mInsertionMode = &html_tree_builder::before_head;
			insert_open_tag(&html::html_node);
			push_open_tag(&html::html_node);
			return true;
		}
		break;
	}
	mInsertionMode = &html_tree_builder::before_head;
	insert_open_tag(&html::html_node);
	push_open_tag(&html::html_node);
	return true;
}

bool html_tree_builder::before_head()
{
	while (next_node()) switch (nodeType())
	{
	case xml::reader::attribute:
		return true;
	case xml::reader::beginTagNode:
		if (context() == &html::head_node)
		{
			mInsertionMode = &html_tree_builder::in_head;
			push_open_tag(&html::head_node);
			return true;
		}
		else
		{
			mInsertionMode = &html_tree_builder::in_head;
			insert_open_tag(&html::head_node);
			push_open_tag(&html::head_node);
			return true;
		}
		break;
	}
	mInsertionMode = &html_tree_builder::in_head;
	insert_open_tag(&html::head_node);
	push_open_tag(&html::head_node);
	return true;
}

bool html_tree_builder::in_head()
{
	while (next_node()) switch (nodeType())
	{
	case xml::reader::cdataNode:
	case xml::reader::textNode:
	case xml::reader::attribute:
		return true;
	case xml::reader::endTagNode:
		if (context() == &html::head_node)
		{
			mInsertionMode = &html_tree_builder::after_head;
			pop_open_tag(&html::head_node);
			return true;
		}
		else
			return true;
		break;
	case xml::reader::beginTagNode:
		if (context() == &html::base_node ||
		    context() == &html::basefont_node ||
		    context() == &html::bgsound_node ||
		    context() == &html::link_node ||
		    context() == &html::meta_node ||
		    context() == &html::title_node)
			return true;
		else
		{
			mInsertionMode = &html_tree_builder::after_head;
			insert_close_tag(&html::head_node);
			pop_open_tag(&html::head_node);
			return true;
		}
		break;
	}
	mInsertionMode = &html_tree_builder::after_head;
	insert_close_tag(&html::head_node);
	pop_open_tag(&html::head_node);
	return true;
}

bool html_tree_builder::after_head()
{
	while (next_node()) switch (nodeType())
	{
	case xml::reader::beginTagNode:
		if (context() == &html::body_node)
		{
			mInsertionMode = &html_tree_builder::in_body;
			push_open_tag(&html::body_node);
			return true;
		}
		else
		{
			mInsertionMode = &html_tree_builder::in_body;
			insert_open_tag(&html::body_node);
			push_open_tag(&html::body_node);
			return true;
		}
		break;
	}
	mInsertionMode = &html_tree_builder::in_body;
	insert_open_tag(&html::body_node);
	push_open_tag(&html::body_node);
	return true;
}

bool html_tree_builder::in_body()
{
	while (next_node()) switch (nodeType())
	{
	case xml::reader::endTagNode:
		if (context() == &html::body_node)
		{
			mInsertionMode = &html_tree_builder::after_body;
			pop_open_tag(&html::body_node);
			return true;
		}
		return true;
	default:
		return true;
	}
	mInsertionMode = &html_tree_builder::after_body;
	insert_close_tag(&html::body_node);
	pop_open_tag(&html::body_node);
	return true;
}

bool html_tree_builder::after_body()
{
	while (next_node()) switch (nodeType())
	{
	case xml::reader::endTagNode:
		if (context() == &html::html_node)
		{
			mInsertionMode = &html_tree_builder::after_after_body;
			return true;
		}
		return true;
	}
	mInsertionMode = &html_tree_builder::after_after_body;
	insert_close_tag(&html::html_node);
	return true;
}

bool html_tree_builder::after_after_body()
{
	return false;
}

namespace cainteoir
{
	void print_html_tree(const std::shared_ptr<xml::reader> &reader, bool silent);
}

static const char *node_name(const xml::context::entry *aEntry,
                             const std::initializer_list<const xml::context::entry_ref> &aEntries)
{
	for (const auto &entry : aEntries)
	{
		if (entry.data == aEntry)
			return entry.name;
	}
	return "(unknown)";
}

void cainteoir::print_html_tree(const std::shared_ptr<xml::reader> &aReader, bool silent)
{
	html_tree_builder reader(aReader);
	while (reader.read()) if (!silent) switch (reader.nodeType())
	{
	default:
		fprintf(stdout, "|%s| %s\n",
		        xml::node_type_name(reader.nodeType()),
		        node_name(reader.context(), html_nodes));
		break;
	case xml::reader::commentNode:
	case xml::reader::cdataNode:
	case xml::reader::textNode:
		fprintf(stdout, "|%s| \"\"\"%s\"\"\"\n",
		        xml::node_type_name(reader.nodeType()),
		        reader.nodeValue().str().c_str());
		break;
	case xml::reader::attribute:
		fprintf(stdout, "|%s| %s=\"\"\"%s\"\"\"\n",
		        xml::node_type_name(reader.nodeType()),
		        node_name(reader.context(), html_attrs),
		        reader.nodeValue().str().c_str());
		break;
	case xml::reader::error:
		fprintf(stdout, "|error| internal parser error\n");
		break;
	}
}

/******************************************************************************
 * HTML Document Reader
 */

struct context_data
{
	const xml::context::entry *ctx;
	uint32_t parameter;

	context_data(const xml::context::entry *aContext, uint32_t aParameter)
		: ctx(aContext)
		, parameter(aParameter)
	{
	}
};

struct html_document_reader : public cainteoir::document_reader
{
	html_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aMimeType, const std::string &aTitle);

	bool read();
private:
	rdf::uri mSubject;
	std::string mTitle;
	html_tree_builder reader;
	rdf::uri href;
	cainteoir::rope htext;
	int hid;
	bool genAnchor;
	std::stack<context_data> ctx;
	cainteoir::css::style_manager stylemgr;

	std::string parseLangAttr();

	void parse_hidden_node();
};

static std::string parseHeadNode(html_tree_builder &reader, const rdf::uri &aSubject, rdf::graph &aGraph);

html_document_reader::html_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aMimeType, const std::string &aTitle)
	: reader(aReader)
	, mSubject(aSubject)
	, href(aSubject.str(), std::string())
	, hid(0)
	, genAnchor(false)
{
	stylemgr.parse("/css/counterstyles.css");

	std::string lang;
	reader.read();
	if (reader.context() == &html::html_node)
	{
		lang = parseLangAttr();
		bool processing = true;
		while (processing && reader.nodeType() != xml::reader::beginTagNode)
			processing = reader.read();
	}

	if (reader.context() == &html::head_node)
	{
		mTitle = parseHeadNode(reader, aSubject, aPrimaryMetadata);
		bool processing = true;
		while (processing && reader.nodeType() != xml::reader::beginTagNode)
			processing = reader.read();
	}

	if (lang.empty() && reader.context() == &html::body_node)
		lang = parseLangAttr();

	if (!lang.empty())
		aPrimaryMetadata.statement(aSubject, rdf::dc("language"), rdf::literal(lang));

	if (!mTitle.empty())
		aPrimaryMetadata.statement(aSubject, rdf::dc("title"), rdf::literal(mTitle));

	if (mTitle.empty())
		mTitle = aTitle;

	if (mTitle.empty())
	{
		mTitle = aSubject.str();
		std::string::size_type sep = mTitle.rfind('/');
		if (sep != std::string::npos)
			mTitle = mTitle.substr(sep + 1);
	}

	ctx.push({ &html::body_node, 0 });
	ctx.push({ &html::title_node, 0 });

	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal(aMimeType));
}

std::string html_document_reader::parseLangAttr()
{
	std::string lang;
	while (reader.read() && reader.nodeType() == xml::reader::attribute)
	{
		if (reader.context() == &xml::lang_attr && lang.empty())
		{
			lang = reader.nodeValue().buffer()->str();
		}
	}
	return lang;
}

static void parseMetaNode(html_tree_builder &reader, const rdf::uri &aSubject, rdf::graph &aGraph)
{
	static xml::context names(std::string(), meta_names, cainteoir::buffer::ignore_case);

	std::shared_ptr<cainteoir::buffer> name;
	std::string lang;
	std::string content;
	std::string http_equiv;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (reader.context() == &html::meta_node)
		{
			if (content.empty())
				return;

			if (!strcasecmp(http_equiv.c_str(), "content-type"))
			{
				std::string::const_iterator first = content.begin();
				std::string::const_iterator last  = content.end();

				while (first != last && *first != ';')
					++first;
				++first;

				while (first != last && (*first == ' ' || *first == '\t' || *first == '\r' || *first == '\n'))
					++first;

				if (first != last)
				{
					std::string::const_iterator arg_first = first;
					while (first != last && (*first >= 'a' && *first <= 'z'))
						++first;

					if (std::string(arg_first, first) == "charset")
					{
						while (first != last && (*first == ' ' || *first == '\t' || *first == '\r' || *first == '\n' || *first == '='))
							++first;

						std::string::const_iterator val_first = first;
						while (first != last && !(*first == ' ' || *first == '\t' || *first == '\r' || *first == '\n' || *first == '=' || *first == ';'))
							++first;

						reader.set_encoding(std::string(val_first, first).c_str());
					}
				}
			}

			if (!name)
				return;

			const xml::context::entry *ctx = names.lookup(std::string(), *name);
			if (ctx == &html::creator_meta)
			{
				aGraph.statement(aSubject, rdf::dc("creator"), rdf::literal(content));
			}
			else if (ctx == &html::keywords_meta)
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
			else if (ctx == &html::description_meta)
			{
				aGraph.statement(aSubject, rdf::dc("description"), rdf::literal(content, lang));
			}
			else if (ctx == &html::title_meta)
			{
				aGraph.statement(aSubject, rdf::dc("title"), rdf::literal(content, lang));
			}
			else if (ctx == &html::abstract_meta)
			{
				aGraph.statement(aSubject, rdf::dcterms("abstract"), rdf::literal(content, lang));
			}
			return;
		}
		break;
	case xml::reader::attribute:
		if (reader.context() == &html::name_attr)
			name = reader.nodeValue().normalize();
		else if (reader.context() == &html::http_equiv_attr)
			http_equiv = reader.nodeValue().normalize()->str();
		else if (reader.context() == &html::content_attr)
			content = reader.nodeValue().normalize()->str();
		else if (reader.context() == &xml::lang_attr)
			lang = reader.nodeValue().normalize()->str();
		else if (reader.context() == &html::charset_attr)
			reader.set_encoding(reader.nodeValue().normalize()->str().c_str());
		break;
	default:
		break;
	}
}

static std::string parseHeadNode(html_tree_builder &reader, const rdf::uri &aSubject, rdf::graph &aGraph)
{
	std::string title;
	const xml::context::entry *context = nullptr;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context() == &html::title_node)
			context = &html::title_node;
		else if (reader.context() == &html::meta_node)
			parseMetaNode(reader, aSubject, aGraph);
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &html::head_node)
			return title;
		if (reader.context() == &html::title_node)
			context = &html::head_node;
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		if (context == &html::title_node)
			title = reader.nodeValue().normalize()->str();
		break;
	default:
		break;
	}
	return title;
}

bool html_document_reader::read()
{
	if (ctx.top().ctx == &html::title_node)
	{
		type    = events::toc_entry | events::anchor;
		styles  = &cainteoir::heading0;
		content = cainteoir::make_buffer(mTitle);
		anchor  = mSubject;
		ctx.pop();
		return true;
	}

	do switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &xml::id_attr)
		{
			anchor    = href = rdf::uri(mSubject.str(), reader.nodeValue().str());
			type      = events::anchor;
			genAnchor = false;
			reader.read();
			return true;
		}
		break;
	case xml::reader::beginTagNode:
		if (reader.context()->styles && reader.context()->styles->display == cainteoir::css::display::list_item)
		{
			std::string marker;
			auto list_styles = ctx.top().ctx->styles;
			if (list_styles)
			{
				auto counter = stylemgr.get_counter_style(list_styles->list_style_type);
				if (counter)
				{
					int i = ctx.top().parameter++;
					marker = counter->marker(i);
				}
				else
					marker = ' ';
			}
			else
				marker = ' ';
			content = cainteoir::make_buffer(marker.c_str(), marker.size());
			type    = events::begin_context | events::text;
			styles  = reader.context()->styles;
			reader.read();
			return true;
		}
		else if (ctx.top().ctx == &html::body_node && reader.context()->styles)
		{
			auto style = reader.context()->styles;
			if (style->display == cainteoir::css::display::none)
			{
				parse_hidden_node();
				continue;
			}

			if (!style->list_style_type.empty())
				ctx.push({ reader.context(), 1 });

			if (style->role == cainteoir::css::role::heading)
			{
				htext.clear();
				genAnchor = true;
			}

			type   = events::begin_context;
			styles = style;
			reader.read();
			return true;
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		if (ctx.top().ctx == &html::body_node)
		{
			content = reader.nodeValue().content();
			if (content && (!styles || styles->whitespace == cainteoir::css::whitespace::normal))
				content = cainteoir::normalize(content, cainteoir::collapse_space, cainteoir::collapse_space);

			bool is_title_header = content && !content->empty() && content->compare(mTitle.c_str()) == 0;

			if (genAnchor)
			{
				genAnchor = false;
				if (!is_title_header)
				{
					std::stringstream ref;
					ref << "genid.h" << hid;
					++hid;

					anchor = href = rdf::uri(mSubject.str(), ref.str());
					type   = events::anchor;
					return true;
				}
			}

			if (content && !content->empty())
			{
				if (!is_title_header)
					htext += content;

				type   = events::text;
				anchor = rdf::uri();
				reader.read();
				return true;
			}
		}
		else if (ctx.top().ctx->styles && ctx.top().ctx->styles->display != cainteoir::css::display::none)
		{
			content = reader.nodeValue().content();
			if (content)
			{
				type   = events::text;
				anchor = rdf::uri();
				reader.read();
				return true;
			}
		}
		break;
	case xml::reader::endTagNode:
		if (ctx.top().ctx == &html::body_node)
		{
			if (reader.context()->styles)
			{
				type   = events::end_context;
				styles = reader.context()->styles;
				anchor = rdf::uri();

				if (styles->role == cainteoir::css::role::heading)
					mTitle.clear();

				if (styles->role == cainteoir::css::role::heading && !htext.empty())
				{
					content = htext.normalize();
					for (char *c = (char *)content->begin(), *last = (char *)content->end(); c != last; ++c)
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
					if (!content->empty())
					{
						type |= events::toc_entry;
						anchor = href;
					}
					href.ref = std::string();
				}

				reader.read();
				return true;
			}
		}
		else if (reader.context()->styles && reader.context()->styles->display == cainteoir::css::display::list_item && ctx.top().ctx->styles)
		{
			type   = events::end_context;
			styles = reader.context()->styles;
			anchor = rdf::uri();
			reader.read();
			return true;
		}
		else if (reader.context() == ctx.top().ctx)
		{
			type   = events::end_context;
			styles = reader.context()->styles;
			anchor = rdf::uri();
			ctx.pop();
			reader.read();
			return true;
		}
		break;
	default:
		break;
	} while (reader.read());

	return false;
}

void html_document_reader::parse_hidden_node()
{
	uint32_t depth = 1;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		++depth;
		break;
	case xml::reader::endTagNode:
		if (--depth == 0)
			return;
		break;
	default:
		break;
	}
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createHtmlReader(const std::shared_ptr<xml::reader> &aReader,
                            const rdf::uri &aSubject,
                            rdf::graph &aPrimaryMetadata,
                            const std::string &aTitle,
	                    const char *aMimeType)
{
	return std::make_shared<html_document_reader>(aReader, aSubject, aPrimaryMetadata, aMimeType, aTitle);
}

/* References
 *
 *    HTML [http://www.whatwg.org/specs/web-apps/current-work/multipage/] -- HTML Living Standard
 */
