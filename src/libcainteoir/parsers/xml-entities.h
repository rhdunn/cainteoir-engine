/* XML Entity Table.
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

static const entity xml_entities_a[] = {
	{ "amp", "&" },
	{ "apos", "\'" },
};

static const entity_set xml_entity_set_a = {
	xml_entities_a,
	xml_entities_a + (sizeof(xml_entities_a) / sizeof(xml_entities_a[0]))
};

static const entity xml_entities_g[] = {
	{ "gt", ">" },
};

static const entity_set xml_entity_set_g = {
	xml_entities_g,
	xml_entities_g + (sizeof(xml_entities_g) / sizeof(xml_entities_g[0]))
};

static const entity xml_entities_l[] = {
	{ "lt", "<" },
};

static const entity_set xml_entity_set_l = {
	xml_entities_l,
	xml_entities_l + (sizeof(xml_entities_l) / sizeof(xml_entities_l[0]))
};

static const entity xml_entities_q[] = {
	{ "quot", "\"" },
};

static const entity_set xml_entity_set_q = {
	xml_entities_q,
	xml_entities_q + (sizeof(xml_entities_q) / sizeof(xml_entities_q[0]))
};

const entity_set * cainteoir::xml::xml_entities[52] = {
	NULL, // A
	NULL, // B
	NULL, // C
	NULL, // D
	NULL, // E
	NULL, // F
	NULL, // G
	NULL, // H
	NULL, // I
	NULL, // J
	NULL, // K
	NULL, // L
	NULL, // M
	NULL, // N
	NULL, // O
	NULL, // P
	NULL, // Q
	NULL, // R
	NULL, // S
	NULL, // T
	NULL, // U
	NULL, // V
	NULL, // W
	NULL, // X
	NULL, // Y
	NULL, // Z
	&xml_entity_set_a,
	NULL, // b
	NULL, // c
	NULL, // d
	NULL, // e
	NULL, // f
	&xml_entity_set_g,
	NULL, // h
	NULL, // i
	NULL, // j
	NULL, // k
	&xml_entity_set_l,
	NULL, // m
	NULL, // n
	NULL, // o
	NULL, // p
	&xml_entity_set_q,
	NULL, // r
	NULL, // s
	NULL, // t
	NULL, // u
	NULL, // v
	NULL, // w
	NULL, // x
	NULL, // y
	NULL, // z
};
