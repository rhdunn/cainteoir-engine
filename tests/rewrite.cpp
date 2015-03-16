/* Test for applying langdb lexical rewrite rules.
 *
 * Copyright (C) 2015 Reece H. Dunn
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
#include "options.hpp"

#include <cainteoir/language.hpp>

#include <stdexcept>
#include <cstdio>

int main(int argc, char ** argv)
{
	try
	{
		const char *ruleset = nullptr;

		const option_group processing_options = { i18n("Processing Options:"), {
			{ 'r', "ruleset", ruleset, "RULESET",
			  i18n("Use the RULESET pronunciation rule file") },
		}};

		const std::initializer_list<const char *> usage = {
			i18n("rewrite [OPTION..] DOCUMENT"),
			i18n("rewrite [OPTION..]"),
		};

		const std::initializer_list<option_group> options = {
			processing_options,
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		auto rewriter = cainteoir::tts::createLexicalRewriteRules(ruleset);
		if (!rewriter)
			throw std::runtime_error("unable to load the specified ruleset");

		std::shared_ptr<cainteoir::buffer> text;
		if (argc == 1)
			text = cainteoir::make_file_buffer(argv[0]);
		else
			text = cainteoir::make_file_buffer(stdin);

		auto out = rewriter->rewrite(text);
		fwrite(out->begin(), 1, out->size(), stdout);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
