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

struct option_t
{
	char short_name;
	const char *long_name;
	int has_argument;
	const char *arg_name;
	const char *help_string;
	std::function<void (const char *)> handler;
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

void print_help(const std::initializer_list<option_group> &groups,
                const std::initializer_list<const char *> usage)
{
	for (const auto &use : usage)
		fprintf(stdout, "%s %s\n", i18n("Usage:"), use);
	fprintf(stdout, "\n");
	fprintf(stdout, "  -h, --help                Show this help message\n");
	for (const auto &group : groups)
		print_help(group);
	fprintf(stdout, i18n("Report bugs to msclrhd@gmail.com\n"));
}

bool parse_command_line(const std::initializer_list<option_group> &groups,
                        const std::initializer_list<const char *> usage,
                        int &argc,
                        char ** &argv)
{
	std::string options_short = "h";
	std::vector<option> options;
	std::function<void (const char *)> handlers[256];
	int index = 200;

	options.push_back({ "help", no_argument, nullptr, 'h' });
	for (const auto &group : groups)
	{
		for (const auto &opt : group.options)
		{
			uint8_t c = opt.short_name ? opt.short_name : index++;
			if (opt.short_name)
			{
				options_short.push_back(opt.short_name);
				if (opt.has_argument == required_argument)
					options_short.push_back(':');
			}
			if (opt.long_name)
				options.push_back({ opt.long_name, opt.has_argument, nullptr, c });
			handlers[c] = opt.handler;
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
		default:
			handlers[(uint8_t)c](optarg);
			break;
		}
	}
}

#endif
