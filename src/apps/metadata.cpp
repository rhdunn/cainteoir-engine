/* Metadata Extractor.
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
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/metadata.hpp>
#include <cainteoir/audio.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

static std::string get_author(rdf::graph &metadata, const rdf::uri &subject)
{
	for (auto &query : rql::select(metadata, rql::subject == subject))
	{
		if (rql::predicate(query).ns == rdf::dc || rql::predicate(query).ns == rdf::dcterms)
		{
			const rdf::uri &object = rql::object(query);
			if (object.empty())
			{
				if (rql::predicate(query).ref == "creator")
					return rql::value(query);
			}
			else if (rql::predicate(query).ref == "creator")
			{
				std::string role;
				std::string value;

				for (auto &data : rql::select(metadata, rql::subject == object))
				{
					const std::string &object = rql::value(data);
					if (rql::predicate(data) == rdf::rdf("value"))
						value = object;
					else if (rql::predicate(data) == rdf::opf("role") || rql::predicate(data) == rdf::pkg("role"))
						role = object;
				}

				if (!value.empty() && (role == "aut" || role.empty()))
					return value;
			}
		}
	}
	return {};
}

static std::string get_date(rdf::graph &metadata, const rdf::uri &subject, const char *event)
{
	for (auto &query : rql::select(metadata, rql::subject == subject))
	{
		if (rql::predicate(query).ns == rdf::dc || rql::predicate(query).ns == rdf::dcterms)
		{
			const rdf::uri &object = rql::object(query);
			if (object.empty())
			{
				if (rql::predicate(query).ref == "date")
					return rql::value(query);
			}
			else if (rql::predicate(query).ref == "date")
			{
				std::string type;
				std::string value;

				for (auto &data : rql::select(metadata, rql::subject == object))
				{
					const std::string &object = rql::value(data);
					if (rql::predicate(data) == rdf::rdf("value"))
						value = object;
					else if (rql::predicate(data) == rdf::opf("event"))
						type = object;
				}

				if (!value.empty() && (type == event || type.empty()))
					return value;
			}
		}
	}
	return {};
}

static std::string get_dublincore(rdf::graph &metadata, const rdf::uri &subject, const char *dcmeta)
{
	for (auto &query : rql::select(metadata, rql::subject == subject))
	{
		if (rql::predicate(query).ns == rdf::dc || rql::predicate(query).ns == rdf::dcterms)
		{
			const rdf::uri &object = rql::object(query);
			if (object.empty())
			{
				if (rql::predicate(query).ref == dcmeta)
					return rql::value(query);
			}
			else if (rql::predicate(query).ref == dcmeta)
			{
				for (auto &data : rql::select(metadata, rql::subject == object))
				{
					if (rql::predicate(data) == rdf::rdf("value"))
						return rql::value(data);
				}
			}
		}
	}
	return {};
}

static std::list<std::string> get_dublincore_items(rdf::graph &metadata, const rdf::uri &subject, const char *dcmeta)
{
	std::list<std::string> items;
	for (auto &query : rql::select(metadata, rql::subject == subject))
	{
		if (rql::predicate(query).ns == rdf::dc || rql::predicate(query).ns == rdf::dcterms)
		{
			const rdf::uri &object = rql::object(query);
			if (object.empty())
			{
				if (rql::predicate(query).ref == dcmeta)
					items.push_back(rql::value(query));
			}
			else if (rql::predicate(query).ref == dcmeta)
			{
				for (auto &data : rql::select(metadata, rql::subject == object))
				{
					if (rql::predicate(data) == rdf::rdf("value"))
						items.push_back(rql::value(data));
				}
			}
		}
	}
	return items;
}

int main(int argc, char ** argv)
{
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	try
	{
		enum
		{
			rdf_ntriple,
			rdf_turtle,
			vorbis_comments,
			metadata_author,
			metadata_creation,
			metadata_description,
			metadata_language,
			metadata_modification,
			metadata_publication,
			metadata_publisher,
			metadata_rights,
			metadata_subjects,
			metadata_title,
		} output_type = rdf_ntriple;

		bool print_time = false;
		bool all_metadata = false;

		const option_group general_options = { nullptr, {
			{ 't', "time", bind_value(print_time, true),
			  i18n("Time how long it takes to extract the metadata") },
			{ 'a', "all", bind_value(all_metadata, true),
			  i18n("Extract all available metadata") },
		}};

		const option_group format_options = { i18n("Metadata Format:"), {
			{ 0, "ntriple", bind_value(output_type, rdf_ntriple),
			  i18n("Output RDF N-Triple metadata statements") },
			{ 0, "turtle", bind_value(output_type, rdf_turtle),
			  i18n("Output RDF Turtle metadata statements") },
			{ 0, "vorbis", bind_value(output_type, vorbis_comments),
			  i18n("Output VorbisComment entries") },
		}};

		const option_group extract_options = { i18n("Metadata Extraction:"), {
			{ 0, "author", bind_value(output_type, metadata_author),
			  i18n("Output the document author") },
			{ 0, "creation", bind_value(output_type, metadata_creation),
			  i18n("Output the document creation date") },
			{ 0, "description", bind_value(output_type, metadata_description),
			  i18n("Output the document description") },
			{ 0, "language", bind_value(output_type, metadata_language),
			  i18n("Output the document language") },
			{ 0, "modification", bind_value(output_type, metadata_modification),
			  i18n("Output the document modification date") },
			{ 0, "publication", bind_value(output_type, metadata_publication),
			  i18n("Output the document publication date") },
			{ 0, "publisher", bind_value(output_type, metadata_publisher),
			  i18n("Output the document publisher") },
			{ 0, "rights", bind_value(output_type, metadata_rights),
			  i18n("Output the document rights (e.g. copyright)") },
			{ 0, "subjects", bind_value(output_type, metadata_subjects),
			  i18n("Output the document subjects") },
			{ 0, "title", bind_value(output_type, metadata_title),
			  i18n("Output the document title") },
		}};

		const std::initializer_list<const option_group *> options = {
			&general_options,
			&format_options,
			&extract_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("metadata [OPTION..] DOCUMENT.."),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		cainteoir::stopwatch timer;
		rdf::graph metadata;

		if (argc == 0)
			cainteoir::createDocumentReader(nullptr, metadata, std::string());
		else for(int i = 0; i < argc; ++i)
		{
			auto reader = cainteoir::createDocumentReader(argv[i], metadata, std::string());
			if (!reader)
				fprintf(stderr, i18n("unsupported document format for file \"%s\"\n"), argv[i]);
			if (all_metadata)
				while (reader->read(&metadata)) ;
		}

		if (print_time)
			fprintf(stderr, "Extraction Time: %G\n", timer.elapsed());

		const char *filename = (argc == 1) ? argv[0] : nullptr;
		rdf::uri subject(filename ? filename : std::string(), std::string());

		if (!metadata.empty()) switch (output_type)
		{
		case vorbis_comments:
			{
				const rdf::uri subject = rdf::uri(argv[0], std::string());
				std::list<cainteoir::vorbis_comment> comments;
				cainteoir::add_document_metadata(comments, metadata, subject);
				for (auto &comment : comments)
					std::cout << comment.label << "=" << comment.value << std::endl;
			}
			break;
		case metadata_author:
			fprintf(stdout, "%s\n", get_author(metadata, subject).c_str());
			break;
		case metadata_creation:
			fprintf(stdout, "%s\n", get_date(metadata, subject, "creation").c_str());
			break;
		case metadata_description:
			fprintf(stdout, "%s\n", get_dublincore(metadata, subject, "description").c_str());
			break;
		case metadata_language:
			fprintf(stdout, "%s\n", get_dublincore(metadata, subject, "language").c_str());
			break;
		case metadata_modification:
			fprintf(stdout, "%s\n", get_date(metadata, subject, "modification").c_str());
			break;
		case metadata_publication:
			fprintf(stdout, "%s\n", get_date(metadata, subject, "publication").c_str());
			break;
		case metadata_publisher:
			fprintf(stdout, "%s\n", get_dublincore(metadata, subject, "publisher").c_str());
			break;
		case metadata_rights:
			fprintf(stdout, "%s\n", get_dublincore(metadata, subject, "rights").c_str());
			break;
		case metadata_subjects:
			for (auto &item : get_dublincore_items(metadata, subject, "subject"))
				fprintf(stdout, "%s\n", item.c_str());
			break;
		case metadata_title:
			fprintf(stdout, "%s\n", get_dublincore(metadata, subject, "title").c_str());
			break;
		default:
			{
				(*rdf::create_formatter(std::cout, output_type == rdf_ntriple ? rdf::formatter::ntriple : rdf::formatter::turtle))
					<< rdf::rdf
					<< rdf::rdfa
					<< rdf::rdfs
					<< rdf::xsd
					<< rdf::xml
					<< rdf::owl
					<< rdf::dc
					<< rdf::dcterms
					<< rdf::dcam
					<< rdf::epub
					<< rdf::epv
					<< rdf::opf
					<< rdf::ocf
					<< rdf::pkg
					<< rdf::media
					<< rdf::onix
					<< rdf::marc
					<< rdf::ncx
					<< rdf::dtb
					<< rdf::smil
					<< rdf::xhtml
					<< rdf::skos
					<< rdf::foaf
					<< rdf::ref
					<< rdf::tts
					<< rdf::iana
					<< rdf::subtag
					<< metadata;
			}
			break;
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
