/* XHTML Document Parser.
 *
 * Copyright (C) 2010-2014 Reece H. Dunn
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

#include <cainteoir/unicode.hpp>
#include <ucd/ucd.h>

namespace xml    = cainteoir::xml;
namespace xmlns  = cainteoir::xml::xmlns;
namespace events = cainteoir::events;
namespace rdf    = cainteoir::rdf;
namespace css    = cainteoir::css;
namespace utf8   = cainteoir::utf8;

/******************************************************************************
 * HTML Elements
 *
 * This is the set of HTML elements and attributes.
 */

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
	static const xml::context::entry center_node     = { &cainteoir::paragraph };
	static const xml::context::entry cite_node       = { &cainteoir::emphasized }; // HTML§14.3.4
	static const xml::context::entry code_node       = { &cainteoir::monospace  }; // HTML§14.3.4
	static const xml::context::entry col_node        = { xml::begin_tag_type::open_close }; // HTML§12.1.2
	static const xml::context::entry colgroup_node   = {};
	static const xml::context::entry data_node       = {};
	static const xml::context::entry datalist_node   = {};
	static const xml::context::entry dd_node         = {};
	static const xml::context::entry del_node        = {};
	static const xml::context::entry details_node    = {};
	static const xml::context::entry dfn_node        = { &cainteoir::emphasized }; // HTML§14.3.4
	static const xml::context::entry dialog_node     = {};
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
	static const xml::context::entry main_node       = {};
	static const xml::context::entry map_node        = {};
	static const xml::context::entry mark_node       = {};
	static const xml::context::entry marquee_node    = {};
	static const xml::context::entry menu_node       = { &cainteoir::bullet_list }; // HTML§14.3.8
	static const xml::context::entry menuitem_node   = { xml::begin_tag_type::open_close }; // HTML§12.1.2
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
	static const xml::context::entry template_node   = {};
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
	static const xml::context::entry code_attr       = {};
	static const xml::context::entry color_attr      = {};
	static const xml::context::entry content_attr    = {};
	static const xml::context::entry data_attr       = {};
	static const xml::context::entry href_attr       = {};
	static const xml::context::entry http_equiv_attr = {};
	static const xml::context::entry label_attr      = {};
	static const xml::context::entry name_attr       = {};
	static const xml::context::entry rel_attr        = {};
	static const xml::context::entry shape_attr      = {};
	static const xml::context::entry src_attr        = {};
	static const xml::context::entry type_attr       = {};
	static const xml::context::entry value_attr      = {};

	static const xml::context::entry abstract_meta    = {};
	static const xml::context::entry creator_meta     = {};
	static const xml::context::entry description_meta = {};
	static const xml::context::entry keywords_meta    = {};
	static const xml::context::entry title_meta       = {};
}

static const std::initializer_list<const xml::context::entry_ref> html_nodes =
{
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "a",          &html::a_node          }, // 1.2        //            //         //
	{ "abbr",       &html::abbr_node       }, // 4.0        //            //         //
	{ "acronym",    &html::acronym_node    }, // 4.0        //            // 5.0     //
	{ "address",    &html::address_node    }, // 1.2        //            //         //
	{ "applet",     &html::applet_node     }, // 3.2        // 4.0        // 5.0     //
	{ "area",       &html::area_node       }, // 3.2        //            //         //
	{ "article",    &html::article_node    }, // 5.0        //            //         //
	{ "aside",      &html::aside_node      }, // 5.0        //            //         //
	{ "audio",      &html::audio_node      }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "b",          &html::b_node          }, // 1.2        //            //         //
	{ "base",       &html::base_node       }, // 1.2        //            //         //
	{ "basefont",   &html::basefont_node   }, // 3.2        // 4.0        // 5.0     //
	{ "bdi",        &html::bdi_node        }, // 5.0        //            //         //
	{ "bdo",        &html::bdo_node        }, // 4.0        //            //         //
	{ "bgsound",    &html::bgsound_node    }, // ---        //            //         // non-standard
	{ "big",        &html::big_node        }, // 3.2        //            // 5.0     //
	{ "blockquote", &html::blockquote_node }, // 1.2        //            //         //
	{ "body",       &html::body_node       }, // 1.2        //            //         //
	{ "br",         &html::br_node         }, // 2.0        //            //         //
	{ "button",     &html::button_node     }, // 4.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "canvas",     &html::canvas_node     }, // 5.0        //            //         //
	{ "caption",    &html::caption_node    }, // 3.2        //            //         //
	{ "center",     &html::center_node     }, // 3.2        // 4.0        // 5.0     //
	{ "cite",       &html::cite_node       }, // 1.2        //            //         //
	{ "code",       &html::code_node       }, // 1.2        //            //         //
	{ "col",        &html::col_node        }, // 4.0        //            //         //
	{ "colgroup",   &html::colgroup_node   }, // 4.0        //            //         //
	{ "command",    &html::menuitem_node   }, // WHATWG     //            //         // old HTML5 draft
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "data",       &html::data_node       }, // 5.1        //            //         //
	{ "datalist",   &html::datalist_node   }, // 5.0        //            //         //
	{ "dd",         &html::dd_node         }, // 1.2        //            //         //
	{ "del",        &html::del_node        }, // 4.0        //            //         //
	{ "details",    &html::details_node    }, // 5.0        //            //         //
	{ "dfn",        &html::dfn_node        }, // 1.2        //            //         //
	{ "dialog",     &html::dialog_node     }, // 5.0        //            //         //
	{ "dir",        &html::dir_node        }, // 1.2        // 4.0        // 5.0     //
	{ "div",        &html::div_node        }, // 3.2        //            //         //
	{ "dl",         &html::dl_node         }, // 1.2        //            //         //
	{ "dt",         &html::dt_node         }, // 1.2        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "em",         &html::em_node         }, // 1.2        //            //         //
	{ "embed",      &html::embed_node      }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "fieldset",   &html::fieldset_node   }, // 4.0        //            //         //
	{ "figcaption", &html::figcaption_node }, // 5.0        //            //         //
	{ "figure",     &html::figure_node     }, // 5.0        //            //         //
	{ "font",       &html::font_node       }, // 3.2        // 4.0        // 5.0     //
	{ "footer",     &html::footer_node     }, // 5.0        //            //         //
	{ "form",       &html::form_node       }, // 2.0        //            //         //
	{ "frame",      &html::frame_node      }, // 4.0        //            // 5.0     //
	{ "frameset",   &html::frameset_node   }, // 4.0        //            // 5.0     //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "h1",         &html::h1_node         }, // 1.2        //            //         //
	{ "h2",         &html::h2_node         }, // 1.2        //            //         //
	{ "h3",         &html::h3_node         }, // 1.2        //            //         //
	{ "h4",         &html::h4_node         }, // 1.2        //            //         //
	{ "h5",         &html::h5_node         }, // 1.2        //            //         //
	{ "h6",         &html::h6_node         }, // 1.2        //            //         //
	{ "head",       &html::head_node       }, // 1.2        //            //         //
	{ "header",     &html::header_node     }, // 5.0        //            //         //
	{ "hgroup",     &html::hgroup_node     }, // WHATWG     //            //         //
	{ "hr",         &html::hr_node         }, // 2.0        //            //         //
	{ "html",       &html::html_node       }, // 1.2        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "i",          &html::i_node          }, // 1.2        //            //         //
	{ "iframe",     &html::iframe_node     }, // 4.0        //            //         //
	{ "img",        &html::img_node        }, // 1.2        //            //         //
	{ "input",      &html::input_node      }, // 2.0        //            //         //
	{ "ins",        &html::ins_node        }, // 4.0        //            //         //
	{ "isindex",    &html::isindex_node    }, // 1.2        // 4.0        // 5.0     //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "kbd",        &html::kbd_node        }, // 1.2        //            //         //
	{ "keygen",     &html::keygen_node     }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "label",      &html::label_node      }, // 4.0        //            //         //
	{ "legend",     &html::legend_node     }, // 4.0        //            //         //
	{ "li",         &html::li_node         }, // 1.2        //            //         //
	{ "link",       &html::link_node       }, // 1.2        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "main",       &html::main_node       }, // 5.0        //            //         //
	{ "map",        &html::map_node        }, // 3.2        //            //         //
	{ "mark",       &html::mark_node       }, // 5.0        //            //         //
	{ "marquee",    &html::marquee_node    }, // ---        //            //         // non-standard
	{ "menu",       &html::menu_node       }, // 1.2        // 4.0        //         // changed meaning in HTML5
	{ "menuitem",   &html::menuitem_node   }, // 5.0        //            //         //
	{ "meta",       &html::meta_node       }, // 3.2        //            //         //
	{ "meter",      &html::meter_node      }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "nav",        &html::nav_node        }, // 5.0        //            //         //
	{ "noad",       &html::noad_node       }, // ---        //            //         // non-standard
	{ "noframes",   &html::noframes_node   }, // 4.0        //            // 5.0     //
	{ "noscript",   &html::noscript_node   }, // 4.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "object",     &html::object_node     }, // 4.0        //            //         //
	{ "ol",         &html::ol_node         }, // 1.2        //            //         //
	{ "optgroup",   &html::optgroup_node   }, // 4.0        //            //         //
	{ "option",     &html::option_node     }, // 2.0        //            //         //
	{ "output",     &html::output_node     }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "p",          &html::p_node          }, // 1.2        //            //         //
	{ "param",      &html::param_node      }, // 3.2        //            //         //
	{ "pre",        &html::pre_node        }, // 1.2        //            //         //
	{ "progress",   &html::progress_node   }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "q",          &html::q_node          }, // 4.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "rp",         &html::rp_node         }, // 5.0        //            //         //
	{ "rt",         &html::rt_node         }, // 5.0        //            //         //
	{ "ruby",       &html::ruby_node       }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "s",          &html::s_node          }, // 4.0        // 4.01       //         // changed meaning in HTML5
	{ "samp",       &html::samp_node       }, // 1.2        //            //         //
	{ "script",     &html::script_node     }, // 3.2        //            //         //
	{ "section",    &html::section_node    }, // 5.0        //            //         //
	{ "select",     &html::select_node     }, // 2.0        //            //         //
	{ "small",      &html::small_node      }, // 3.2        //            //         //
	{ "source",     &html::source_node     }, // 5.0        //            //         //
	{ "span",       &html::span_node       }, // 4.0        //            //         //
	{ "strike",     &html::strike_node     }, // 3.2        // 4.0        // 5.0     //
	{ "strong",     &html::strong_node     }, // 1.2        //            //         //
	{ "style",      &html::style_node      }, // 3.2        //            //         //
	{ "sub",        &html::sub_node        }, // 3.2        //            //         //
	{ "summary",    &html::summary_node    }, // 5.0        //            //         //
	{ "sup",        &html::sup_node        }, // 3.2        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "table",      &html::table_node      }, // 3.2        //            //         //
	{ "tbody",      &html::tbody_node      }, // 4.0        //            //         //
	{ "td",         &html::td_node         }, // 3.2        //            //         //
	{ "template",   &html::template_node   }, // 5.0        //            //         //
	{ "textarea",   &html::textarea_node   }, // 2.0        //            //         //
	{ "tfoot",      &html::tfoot_node      }, // 4.0        //            //         //
	{ "th",         &html::th_node         }, // 3.2        //            //         //
	{ "thead",      &html::thead_node      }, // 4.0        //            //         //
	{ "time",       &html::time_node       }, // 5.0        //            //         //
	{ "title",      &html::title_node      }, // 1.2        //            //         //
	{ "tr",         &html::tr_node         }, // 3.2        //            //         //
	{ "track",      &html::track_node      }, // 5.0        //            //         //
	{ "tt",         &html::tt_node         }, // 1.2        //            // 5.0     //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "u",          &html::u_node          }, // 1.2        // 4.0        //         //
	{ "ul",         &html::ul_node         }, // 1.2        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "var",        &html::var_node        }, // 1.2        //            //         //
	{ "video",      &html::video_node      }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment
	{ "wbr",        &html::wbr_node        }, // 5.0        //            //         //
	// Element    // Context                  // Introduced // Deprecated // Removed // Comment

	// NOTE: HTML 4 is the baseline implementation, so any elements obsoleted in
	// that specification or earlier are not supported. This also includes the
	// HTML 3.2 version of ISINDEX.
};

static const std::initializer_list<const xml::context::entry_ref> html_attrs =
{
	{ "charset",    &html::charset_attr },
	{ "code",       &html::code_attr },
	{ "color",      &html::color_attr },
	{ "content",    &html::content_attr },
	{ "data",       &html::data_attr },
	{ "href",       &html::href_attr },
	{ "http-equiv", &html::http_equiv_attr },
	{ "id",         &xml::id_attr },
	{ "label",      &html::label_attr },
	{ "lang",       &xml::lang_attr },
	{ "name",       &html::name_attr },
	{ "rel",        &html::rel_attr },
	{ "shape",      &html::shape_attr },
	{ "src",        &html::src_attr },
	{ "type",       &html::type_attr },
	{ "value",      &html::value_attr },
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
 * ePub Elements
 *
 * This is the set of ePub attributes used within HTML documents.
 */

namespace epub
{
	static const xml::context::entry type_attr = {};
}

static const std::initializer_list<const xml::context::entry_ref> epub_attrs =
{
	{ "type", &epub::type_attr },
};

// epub:type values -- http://www.idpf.org/epub/vocab/structure/
static const std::initializer_list<std::pair<const char *, css::role>> epub_types =
{
	{ "toc", css::role::table_of_contents },
};

/******************************************************************************
 * HTML Tree Construction
 *
 * This is an implementation of the algorithm described in HTML§12.2.5 (Tree
 * construction) that handles HTML tag soup.
 */

struct html_tree_builder
{
	html_tree_builder(const std::shared_ptr<xml::reader> &aReader);

	bool read();

	xml::reader::node_type nodeType() const { return mNodeType; }

	bool isXmlNamespaceDecl() const
	{
		return !reader->nodePrefix().compare("xmlns") ||
		      (!reader->nodeName().compare("xmlns") && reader->nodePrefix().empty());
	}

	const cainteoir::rope &nodeValue() const { return reader->nodeValue(); }

	const xml::context::entry *context() const { return mContext; }

	void set_encoding(const char *encoding) { reader->set_encoding(encoding); }

	void hold_event() { mHoldEvent = true; }
private:
	std::shared_ptr<xml::reader> reader;
	bool mHoldEvent;
	bool mReprocessToken;
	std::list<const xml::context::entry *> mOpenElements;
	xml::reader::node_type mNodeType;
	const xml::context::entry *mContext;

	void insert_open_tag(const xml::context::entry *aOpenTag);
	void insert_close_tag(const xml::context::entry *aCloseTag);

	void push_open_tag(const xml::context::entry *aOpenTag);
	void pop_open_tag(const xml::context::entry *aOpenTag);

	bool in_scope(const xml::context::entry *aElement);

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
	, mHoldEvent(false)
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
	reader->set_attrs(xmlns::epub,   epub_attrs);
}

bool html_tree_builder::read()
{
	if (mHoldEvent)
	{
		mHoldEvent = false;
		return true;
	}
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
	mOpenElements.push_back(aOpenTag);
}

void html_tree_builder::pop_open_tag(const xml::context::entry *aOpenTag)
{
	mOpenElements.pop_back();
}

bool html_tree_builder::in_scope(const xml::context::entry *aElement)
{
	for (const auto &element : mOpenElements)
	{
		if (element == aElement)
			return true;
	}
	return false;
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
	// NOTE: This algorithm currently only ensures that the open and close tags
	// match up, it does not currently follow the HTML rules (e.g. when to close
	// 'p' elements).
	while (next_node()) switch (nodeType())
	{
	case xml::reader::beginTagNode:
		push_open_tag(mContext);
		return true;
	case xml::reader::endTagNode:
		if (context() == &html::body_node)
		{
			auto top = mOpenElements.back();
			if (top == &html::body_node)
				mInsertionMode = &html_tree_builder::after_body;
			else
				insert_close_tag(top);
			pop_open_tag(&html::body_node);
		}
		else if (context() == &html::html_node)
		{
			auto top = mOpenElements.back();
			if (top == &html::body_node)
			{
				mInsertionMode = &html_tree_builder::after_body;
				insert_close_tag(&html::body_node);
			}
			else
				insert_close_tag(top);
			pop_open_tag(&html::body_node);
		}
		else if (in_scope(context()))
		{
			if (context() != mOpenElements.back())
				insert_close_tag(mOpenElements.back());
			pop_open_tag(mContext);
		}
		else
			continue;
		return true;
	default:
		return true;
	}
	auto top = mOpenElements.back();
	if (top == &html::body_node)
		mInsertionMode = &html_tree_builder::after_body;
	insert_close_tag(top);
	pop_open_tag(top);
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

/******************************************************************************
 * HTML Tree Construction - test hook
 *
 * This code is used by the tests/xmlreader test program to print the events
 * from the tree construction code, so it can be tested independently of the
 * document parser code.
 */

namespace cainteoir
{
	void print_html_tree(const std::shared_ptr<xml::reader> &reader, bool silent);
}

typedef std::pair<const char *, const std::initializer_list<const xml::context::entry_ref> &>
        context_ns;

static const std::initializer_list<context_ns> node_sets =
{
	{ nullptr, html_nodes },
};

static const std::initializer_list<context_ns> attr_sets =
{
	{ nullptr, html_attrs },
	{ "xml",   xml::attrs },
	{ "epub",  epub_attrs },
};

static void print_node_name(const xml::context::entry *aEntry,
                            const std::initializer_list<context_ns> &aEntries)
{
	for (const auto &entry_set : aEntries)
	{
		for (const auto &entry : entry_set.second)
		{
			if (entry.data == aEntry)
			{
				if (entry_set.first)
					fprintf(stdout, "%s:%s", entry_set.first, entry.name);
				else
					fprintf(stdout, "%s", entry.name);
				return;
			}
		}
	}
	fprintf(stdout, "(unknown)");
}

void cainteoir::print_html_tree(const std::shared_ptr<xml::reader> &aReader, bool silent)
{
	html_tree_builder reader(aReader);
	while (reader.read()) if (!silent) switch (reader.nodeType())
	{
	default:
		fprintf(stdout, "|%s| ", xml::node_type_name(reader.nodeType()));
		print_node_name(reader.context(), node_sets);
		fprintf(stdout, "\n");
		break;
	case xml::reader::commentNode:
	case xml::reader::cdataNode:
	case xml::reader::textNode:
		fprintf(stdout, "|%s| \"\"\"%s\"\"\"\n",
		        xml::node_type_name(reader.nodeType()),
		        reader.nodeValue().str().c_str());
		break;
	case xml::reader::attribute:
		if (reader.isXmlNamespaceDecl())
			continue; // xml namespace -- skip
		fprintf(stdout, "|%s| ", xml::node_type_name(reader.nodeType()));
		print_node_name(reader.context(), attr_sets);
		fprintf(stdout, "=\"\"\"%s\"\"\"\n", reader.nodeValue().str().c_str());
		break;
	case xml::reader::error:
		fprintf(stdout, "|error| internal parser error\n");
		break;
	}
}

/******************************************************************************
 * HTML Document Reader
 *
 * This is the main HTML parser that generates document reader events and RDF
 * metadata. It uses the HTML Tree Construction code to ensure that the HTML
 * tag soup is converted to well-formed XML events.
 */

static css::role lookup_role(const cainteoir::buffer &name,
                             const std::initializer_list<std::pair<const char *, css::role>> &roles)
{
	for (const auto &role : roles)
	{
		if (name.compare(role.first) == 0)
			return role.second;
	}
	return css::role::none;
}

struct html_document_reader : public cainteoir::document_reader
{
	html_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aMimeType, const std::string &aTitle);

	bool read(rdf::graph *aMetadata);
private:
	rdf::uri mSubject;
	std::string mTitle;
	html_tree_builder reader;
	rdf::uri href;
	cainteoir::rope htext;
	int hid;
	bool genAnchor;
	cainteoir::css::style_manager stylemgr;
	cainteoir::whitespace trim_left;

	std::string mLanguage;
	css::role mRole;

	bool parse_document_root(rdf::graph *aMetadata);
	bool parse_html_node(rdf::graph *aMetadata);
	bool parse_head_node(rdf::graph *aMetadata);
	bool parse_body_attrs(rdf::graph *aMetadata);

	bool parse_body_node(rdf::graph *aMetadata);
	bool parse_list_node(rdf::graph *aMetadata);
	bool parse_heading_node(rdf::graph *aMetadata);
	bool parse_nav_node(rdf::graph *aMetadata);
	bool parse_toc_node(rdf::graph *aMetadata);
	bool parse_node(rdf::graph *aMetadata);

	bool generate_title_event(rdf::graph *aMetadata);

	void parse_hidden_node();
	void parse_text_node();

	void reset_block_scope();

	struct context_data
	{
		const xml::context::entry *ctx;
		decltype(&html_document_reader::parse_node) handler;
		uint32_t parameter;
	};

	std::stack<context_data> ctx;
};

html_document_reader::html_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aMimeType, const std::string &aTitle)
	: reader(aReader)
	, mSubject(aSubject)
	, href(aSubject.str(), std::string())
	, hid(0)
	, genAnchor(false)
	, trim_left(cainteoir::whitespace::preserve)
	, mRole(css::role::none)
{
	stylemgr.parse("/css/counterstyles.css");

	ctx.push({ nullptr, &html_document_reader::parse_document_root, 0 });
	read(&aPrimaryMetadata);

	if (!mLanguage.empty())
		aPrimaryMetadata.statement(aSubject, rdf::dc("language"), rdf::literal(mLanguage));

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

	ctx.push({ &html::title_node, &html_document_reader::generate_title_event, 0 });

	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal(aMimeType));
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

bool html_document_reader::read(rdf::graph *aMetadata)
{
	while (!ctx.empty())
	{
		auto handler = ctx.top().handler;
		if ((this->*handler)(aMetadata))
			return true;
	}
	return false;
}

bool html_document_reader::parse_document_root(rdf::graph *aMetadata)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context() == &html::html_node)
		{
			ctx.push({ nullptr, &html_document_reader::parse_html_node, 0 });
			return false;
		}
		break;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_html_node(rdf::graph *aMetadata)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &xml::lang_attr && mLanguage.empty())
			mLanguage = reader.nodeValue().buffer()->str();
		break;
	case xml::reader::beginTagNode:
		if (reader.context() == &html::head_node)
			ctx.push({ nullptr, &html_document_reader::parse_head_node, 0 });
		else if (reader.context() == &html::body_node)
			ctx.push({ nullptr, &html_document_reader::parse_body_attrs, 0 });
		return false;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_head_node(rdf::graph *aMetadata)
{
	const xml::context::entry *context = nullptr;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context() == &html::title_node)
			context = &html::title_node;
		else if (reader.context() == &html::meta_node)
			parseMetaNode(reader, mSubject, *aMetadata);
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &html::head_node)
		{
			ctx.pop();
			return false;
		}
		else if (reader.context() == &html::title_node)
			context = nullptr;
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		if (context == &html::title_node)
			mTitle = reader.nodeValue().normalize()->str();
		break;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_body_attrs(rdf::graph *aMetadata)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &xml::lang_attr && mLanguage.empty())
			mLanguage = reader.nodeValue().buffer()->str();
		break;
	case xml::reader::beginTagNode:
		ctx.push({ &html::body_node, &html_document_reader::parse_body_node, 0 });
		reader.hold_event();
		return true;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_body_node(rdf::graph *aMetadata)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &xml::id_attr)
		{
			anchor = href = rdf::uri(mSubject.str(), reader.nodeValue().str());
			type   = events::anchor;
			return true;
		}
		break;
	case xml::reader::beginTagNode:
		styles = reader.context()->styles;
		type   = events::begin_context;
		if (reader.context() == &html::nav_node)
		{
			ctx.push({ reader.context(), &html_document_reader::parse_nav_node, 0 });
			return false;
		}
		if (!styles)
			continue;
		if (styles->display == css::display::none)
		{
			parse_hidden_node();
			continue;
		}
		reset_block_scope();
		if (styles->display == css::display::list_item)
		{
			content = std::make_shared<cainteoir::buffer>(" ");
			type    = events::begin_context | events::text;
			styles  = reader.context()->styles;
			ctx.push({ reader.context(), &html_document_reader::parse_node, 0 });
			return true;
		}
		else if (styles->role == css::role::heading)
		{
			htext.clear();
			genAnchor = true;
			ctx.push({ reader.context(), &html_document_reader::parse_heading_node, 0 });
		}
		else if (!styles->list_style_type.empty())
			ctx.push({ reader.context(), &html_document_reader::parse_list_node, 1 });
		else
			ctx.push({ reader.context(), &html_document_reader::parse_node, 0 });
		return true;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_list_node(rdf::graph *aMetadata)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &xml::id_attr)
		{
			anchor = href = rdf::uri(mSubject.str(), reader.nodeValue().str());
			type   = events::anchor;
			return true;
		}
		break;
	case xml::reader::beginTagNode:
		styles = reader.context()->styles;
		if (styles && styles->display == css::display::list_item)
		{
			auto list_type = ctx.top().ctx->styles->list_style_type;
			auto counter   = stylemgr.get_counter_style(list_type);
			if (counter)
			{
				int i = ctx.top().parameter++;
				std::string marker = counter->marker(i);
				content = cainteoir::make_buffer(marker.c_str(), marker.size());
			}
			else
				content = std::make_shared<cainteoir::buffer>(" ");
			type    = events::begin_context | events::text;
			reset_block_scope();
			if (mRole == css::role::table_of_contents)
				ctx.push({ reader.context(), &html_document_reader::parse_toc_node, 0 });
			else
				ctx.push({ reader.context(), &html_document_reader::parse_node, 0 });
			return true;
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context() == ctx.top().ctx)
		{
			type   = events::end_context;
			styles = reader.context()->styles;
			anchor = rdf::uri();
			ctx.pop();
			reset_block_scope();
			return true;
		}
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_heading_node(rdf::graph *aMetadata)
{
	bool is_title_header = false;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &xml::id_attr)
		{
			anchor    = href = rdf::uri(mSubject.str(), reader.nodeValue().str());
			type      = events::anchor;
			genAnchor = false;
			return true;
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		parse_text_node();
		is_title_header = content && !content->empty() && content->compare(mTitle.c_str()) == 0;
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
				reader.hold_event();
				return true;
			}
		}
		if (content && !content->empty())
		{
			if (!is_title_header)
				htext += content;

			type   = events::text;
			anchor = rdf::uri();
			return true;
		}
		break;
	case xml::reader::beginTagNode:
		styles = reader.context()->styles;
		if (styles)
		{
			type   = events::begin_context;
			anchor = rdf::uri();
			reset_block_scope();
			ctx.push({ reader.context(), &html_document_reader::parse_node, 0 });
			return true;
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context() == ctx.top().ctx)
		{
			mTitle.clear();
			type   = events::end_context;
			styles = reader.context()->styles;
			anchor = rdf::uri();
			ctx.pop();
			reset_block_scope();
			if (!htext.empty())
			{
				content = htext.normalize();
				for (char *c = (char *)content->begin(), *last = (char *)content->end(); c != last; ++c) switch (*c)
				{
				case '\n':
					*c = ' ';
					break;
				default:
					break;
				}
				if (!content->empty())
				{
					anchor = href;
				}
				href.ref = std::string();
			}
			return true;
		}
		break;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_nav_node(rdf::graph *aMetadata)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &epub::type_attr)
			mRole = lookup_role(*reader.nodeValue().normalize(), epub_types);
		else if (reader.context() == &xml::id_attr)
		{
			anchor = href = rdf::uri(mSubject.str(), reader.nodeValue().str());
			type   = events::anchor;
			return true;
		}
		break;
	case xml::reader::beginTagNode:
		styles = reader.context()->styles;
		type   = events::begin_context;
		if (!styles)
			continue;
		reset_block_scope();
		if (!styles->list_style_type.empty())
			ctx.push({ reader.context(), &html_document_reader::parse_list_node, 1 });
		else
			ctx.push({ reader.context(), &html_document_reader::parse_node, 0 });
		return true;
	case xml::reader::endTagNode:
		if (reader.context() == ctx.top().ctx)
		{
			mRole  = css::role::none;
			ctx.pop();
			reset_block_scope();
			return false;
		}
		break;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_toc_node(rdf::graph *aMetadata)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &html::href_attr)
			anchor = href = rdf::uri(reader.nodeValue().str());
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		parse_text_node();
		if (content && !content->empty())
		{
			type = events::text;
			return true;
		}
		break;
	case xml::reader::beginTagNode:
		styles = reader.context()->styles;
		if (styles)
		{
			type   = events::begin_context;
			anchor = rdf::uri();
			reset_block_scope();
			ctx.push({ reader.context(), &html_document_reader::parse_node, 0 });
			return true;
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context() == ctx.top().ctx)
		{
			type   = events::end_context;
			styles = reader.context()->styles;
			anchor = rdf::uri();
			ctx.pop();
			reset_block_scope();
			return true;
		}
		break;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::parse_node(rdf::graph *aMetadata)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		parse_text_node();
		if (content && !content->empty())
		{
			type   = events::text;
			anchor = rdf::uri();
			return true;
		}
		break;
	case xml::reader::beginTagNode:
		styles = reader.context()->styles;
		if (styles)
		{
			type   = events::begin_context;
			anchor = rdf::uri();
			reset_block_scope();
			ctx.push({ reader.context(), &html_document_reader::parse_node, 0 });
			return true;
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context() == ctx.top().ctx)
		{
			type   = events::end_context;
			styles = reader.context()->styles;
			anchor = rdf::uri();
			ctx.pop();
			reset_block_scope();
			return true;
		}
		break;
	}
	ctx.pop();
	return false;
}

bool html_document_reader::generate_title_event(rdf::graph *aMetadata)
{
	type    = events::anchor;
	styles  = &cainteoir::heading0;
	content = cainteoir::make_buffer(mTitle);
	anchor  = mSubject;
	ctx.pop();
	return true;
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

void html_document_reader::parse_text_node()
{
	content = reader.nodeValue().buffer();
	if (content)
	{
		css::whitespace space = css::whitespace::normal;
		if (styles)
			space = styles->whitespace;

		cainteoir::whitespace whitespace;
		cainteoir::whitespace newlines;
		switch (space)
		{
		case css::whitespace::normal:
		case css::whitespace::nowrap:
			whitespace = cainteoir::whitespace::collapse;
			newlines   = cainteoir::whitespace::collapse;
			break;
		case css::whitespace::preserved:
		case css::whitespace::preserved_wrap:
			whitespace = cainteoir::whitespace::preserve;
			newlines   = cainteoir::whitespace::preserve;
			break;
		case css::whitespace::preserved_line:
			whitespace = cainteoir::whitespace::collapse;
			newlines   = cainteoir::whitespace::preserve;
			break;
		}

		uint32_t c;
		utf8::read(content->begin(), c);

		content = cainteoir::normalize(content, whitespace, newlines,
		                               trim_left,
		                               cainteoir::whitespace::preserve);
		if (content && content->size() >= 1)
		{
			const char *str = utf8::prev(content->end());
			uint32_t ch;
			utf8::read(str, ch);
			trim_left = ucd::isspace(ch) ? cainteoir::whitespace::collapse : cainteoir::whitespace::preserve;
		}
		else if (ucd::isspace(c) && trim_left == cainteoir::whitespace::preserve)
		{
			content = std::make_shared<cainteoir::buffer>(" ");
			trim_left = cainteoir::whitespace::collapse;
		}
	}
}

void html_document_reader::reset_block_scope()
{
	switch (styles->display)
	{
	case css::display::inlined:
	case css::display::inherit:
		break;
	default:
		switch (styles->whitespace)
		{
		case css::whitespace::normal:
		case css::whitespace::nowrap:
		case css::whitespace::preserved_line:
			trim_left = cainteoir::whitespace::collapse;
			break;
		case css::whitespace::preserved:
		case css::whitespace::preserved_wrap:
			trim_left = cainteoir::whitespace::preserve;
			break;
		}
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
 *    v1.2   : [Hypertext Markup Language (HTML)](http://www.w3.org/MarkUp/draft-ietf-iiir-html-01.txt)
 *    v2.0   : [HTML 2.0](http://tools.ietf.org/html/rfc1866)
 *    v3.2   : [HTML 3.2](http://www.w3.org/TR/REC-html32)
 *    v4.01  : [HTML 4.01](http://www.w3.org/TR/REC-html40)
 *    v5.0   : [HTML 5](http://www.w3.org/TR/html5/)
 *    WHATWG : [HTML Living Standard](http://www.whatwg.org/specs/web-apps/current-work/multipage/)
 */
