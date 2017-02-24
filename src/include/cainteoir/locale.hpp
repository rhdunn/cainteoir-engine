/* BCP 47 Locale Support.
 *
 * Copyright (C) 2011-2015 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_LOCALE_HPP
#define CAINTEOIR_ENGINE_LOCALE_HPP

#include <string>

namespace cainteoir { namespace language
{
	struct tag
	{
		std::string lang;
		std::string extlang;
		std::string script;
		std::string region;
		std::string variant;
		std::string private_use;

		std::string str() const;

		tag(const std::string &l,
		    const std::string &e = std::string(),
		    const std::string &s = std::string(),
		    const std::string &r = std::string(),
		    const std::string &v = std::string(),
		    const std::string &p = std::string())
			: lang(l)
			, extlang(e)
			, script(s)
			, region(r)
			, variant(v)
			, private_use(p)
		{
		}
	};

	bool operator==(const tag &a, const tag &b);

	bool operator<(const tag &a, const tag &b);

	bool issubtag(const tag &a, const tag &b);

	tag make_lang(const std::string &lang);
}}

namespace cainteoir
{
	struct languages
	{
		std::string language(const language::tag &id) const;

		std::string script(const language::tag &id) const;

		std::string region(const language::tag &id) const;

		std::string operator()(const std::string & langid);
	};
}

#endif
