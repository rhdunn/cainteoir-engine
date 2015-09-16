/* Command Line Options
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_OPTIONS_HPP
#define CAINTEOIR_ENGINE_OPTIONS_HPP

#include <functional>
#include <vector>
#include <string>

#include <getopt.h>
#include <stdio.h>

enum class argument_t
{
	none,
	string,
	integer,
	real,
};

template <typename T>
struct bind_value_t
{
	T &data;
	const T &value;

	bind_value_t(T &aData, const T &aValue)
		: data(aData)
		, value(aValue)
	{
	}
};

template <typename T>
bind_value_t<T> bind_value(T &aData, const T &aValue)
{
	return bind_value_t<T>(aData, aValue);
}

template <typename T>
bind_value_t<const T *> bind_value(const T * &aData, const T *aValue)
{
	return bind_value_t<const T *>(aData, aValue);
}

template <typename T>
bind_value_t<T *> bind_value(T * &aData, T *aValue)
{
	return bind_value_t<T *>(aData, aValue);
}

struct option_t
{
	typedef uint64_t value_t;

	char short_name;
	const char *long_name;
	argument_t has_argument;
	const char *arg_name;
	const char *help_string;

	void *data;
	value_t value;
	char data_size;

	// no argument
	template <typename T>
	option_t(char aShortName,
	         const char *aLongName,
	         bind_value_t<T> aData,
                 const char *aHelpString)
		: short_name(aShortName)
		, long_name(aLongName)
		, has_argument(argument_t::none)
		, arg_name(nullptr)
		, help_string(aHelpString)
		, data((void *)&aData.data)
		, value((value_t)aData.value)
		, data_size(sizeof(T))
	{
		static_assert(sizeof(T) <= sizeof(value),
		              "T is too big to be used as an option value.");
	}

	// string argument
	option_t(char aShortName,
	         const char *aLongName,
	         const char * &aData,
	         const char *aArgName,
                 const char *aHelpString)
		: short_name(aShortName)
		, long_name(aLongName)
		, has_argument(argument_t::string)
		, arg_name(aArgName)
		, help_string(aHelpString)
		, data(&aData)
		, value(0)
		, data_size(0)
	{
	}

	// integer argument
	template <typename T,
	          class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	option_t(char aShortName,
	         const char *aLongName,
	         T &aData,
	         const char *aArgName,
                 const char *aHelpString)
		: short_name(aShortName)
		, long_name(aLongName)
		, has_argument(std::is_integral<T>::value ? argument_t::integer : argument_t::real)
		, arg_name(aArgName)
		, help_string(aHelpString)
		, data(&aData)
		, value(0)
		, data_size(sizeof(T))
	{
		static_assert(sizeof(T) <= sizeof(value),
		              "T is too big to be used as an option value.");
	}

	void operator()(const char *arg) const
	{
		switch (has_argument)
		{
		case argument_t::none:
			switch (data_size)
			{
			case 1: *(uint8_t  *)data = value; break;
			case 2: *(uint16_t *)data = value; break;
			case 4: *(uint32_t *)data = value; break;
			case 8: *(uint64_t *)data = value; break;
			}
			break;
		case argument_t::string:
			*(const char **)data = arg;
			break;
		case argument_t::integer:
			switch (data_size)
			{
			case 1: *(int8_t  *)data = atoi(arg); break;
			case 2: *(int16_t *)data = atoi(arg); break;
			case 4: *(int32_t *)data = atoi(arg); break;
			case 8: *(int64_t *)data = atoi(arg); break;
			}
			break;
		case argument_t::real:
			switch (data_size)
			{
			case 4: *(float  *)data = atof(arg); break;
			case 8: *(double *)data = atof(arg); break;
			}
			break;
		}
	}
};

void print_help(const option_t &opt)
{
	int padding = 20;
	char argname_separator = '=';
	if (opt.short_name && opt.long_name)
	{
		fprintf(stdout, "  -%c, --", opt.short_name);
		padding -= fprintf(stdout, "%s", opt.long_name);
	}
	else if (opt.short_name)
	{
		fprintf(stdout, "  -%c", opt.short_name);
		argname_separator = ' ';
	}
	else
	{
		fprintf(stdout, "      --");
		padding -= fprintf(stdout, "%s", opt.long_name);
	}
	if (opt.arg_name)
		padding -= fprintf(stdout, "%c%s", argname_separator, opt.arg_name);
	if (padding > 0)
		fprintf(stdout, "%*s%s\n", padding, " ", opt.help_string);
	else
		fprintf(stdout, "\n                            %s\n", opt.help_string);
}

struct option_group
{
	const char *name;
	const std::initializer_list<option_t> options;
};

void print_help(const option_group &group)
{
	if (group.name)
	{
		fprintf(stdout, "%s\n", group.name);
		fprintf(stdout, "\n");
	}
	for (const auto &opt : group.options)
		print_help(opt);
	fprintf(stdout, "\n");
}

void print_help(const std::initializer_list<const option_group *> &groups,
                const std::initializer_list<const char *> usage)
{
	for (const auto &use : usage)
		fprintf(stdout, "%s %s\n", i18n("Usage:"), use);
	fprintf(stdout, "\n");
	fprintf(stdout, "  -h, --help                Show this help message\n");
	for (const auto &group : groups)
		print_help(*group);
	fprintf(stdout, i18n("Report bugs to msclrhd@gmail.com\n"));
}

bool parse_command_line(const std::initializer_list<const option_group *> &groups,
                        const std::initializer_list<const char *> usage,
                        int &argc,
                        char ** &argv)
{
	std::string options_short = "h";
	std::vector<option> options;
	const option_t *option_table[256];
	int index = 200;

	options.push_back({ "help", no_argument, nullptr, 'h' });
	for (const auto &group : groups)
	{
		for (const auto &opt : group->options)
		{
			uint8_t c = opt.short_name ? opt.short_name : index++;
			if (opt.short_name)
			{
				options_short.push_back(opt.short_name);
				if (opt.has_argument != argument_t::none)
					options_short.push_back(':');
			}
			if (opt.long_name)
				options.push_back({ opt.long_name, opt.has_argument == argument_t::none ? no_argument : required_argument, nullptr, c });
			option_table[c] = &opt;
		}
	}
	options.push_back({ nullptr, 0, nullptr, 0 });

	while (true)
	{
		int c = getopt_long(argc, argv, options_short.c_str(), &options[0], nullptr);
		switch (c)
		{
		case -1:
			argc -= optind;
			argv += optind;
			return true;
		case 'h':
			print_help(groups, usage);
			return false;
		case '?':
			fprintf(stderr, "error: unrecognised option: -%c\n", optopt);
			break;
		case ':':
			fprintf(stderr, "error: option -%c requires an argument\n", optopt);
			break;
		default:
			{
				const auto &opt = option_table[(uint8_t)c];
				if (opt) (*opt)(optarg);
			}
			break;
		}
	}
}

#endif
