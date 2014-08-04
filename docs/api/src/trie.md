#<cldoc:cainteoir::trie_node>

A node in a trie data structure.

#<cldoc:cainteoir::trie_node::trie_node>

Create a trie node object.
@ch The character associated with the node.

#<cldoc:cainteoir::trie_node::get>

Get the child node that is associated with the specified character.
@ch The character of the child node to locate.

@return The node associated with `ch`, or `nullptr` if no matching node was found.

#<cldoc:cainteoir::trie_node::add>

Add a child node associated with the specified character.
@ch The character to be associated with the child node.

@return The new node associated with `ch`.

#<cldoc:cainteoir::trie_node::c>

The character associated with the node.

#<cldoc:cainteoir::trie_node::children>

The list of child nodes from this node.

#<cldoc:cainteoir::trie_node::item>

The value associated with the node.

#<cldoc:cainteoir::trie>

A trie data structure.

#<cldoc:cainteoir::trie::trie>

Create a trie object.

#<cldoc:cainteoir::trie::insert>

Insert a new item into the trie object.
@str The string index of the item.

@return A reference to the inserted item.

#<cldoc:cainteoir::trie::insert>

Insert a new item into the trie object.
@str   The string index of the item.
@value The item to be inserted.

#<cldoc:cainteoir::trie::lookup>

Lookup an item by its indexed name.
@str      The string index of the item.
@no_match The value to return if an item does not exist at the string index.

@return The matching entry value.

#<cldoc:cainteoir::trie::root>

Get the root node.

@return The root node of the trie object.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
