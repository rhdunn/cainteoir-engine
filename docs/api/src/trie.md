# cainteoir::trie_node {: .doc }

A node in a trie data structure.

# cainteoir::trie_node::trie_node {: .doc }

Create a trie node object.

@ch
: The character associated with the node.

# cainteoir::trie_node::get {: .doc }

Get the child node that is associated with the specified character.

@ch
: The character of the child node to locate.

@return
: The node associated with `ch`, or `nullptr` if no matching node was found.

# cainteoir::trie_node::add {: .doc }

Add a child node associated with the specified character.
@ch The character to be associated with the child node.

@return The new node associated with `ch`.

# cainteoir::trie_node::c {: .doc }

The character associated with the node.

# cainteoir::trie_node::children {: .doc }

The list of child nodes from this node.

# cainteoir::trie_node::item {: .doc }

The value associated with the node.

# cainteoir::trie {: .doc }

A trie data structure.

# cainteoir::trie::trie {: .doc }

Create a trie object.

# cainteoir::trie::insert {: .doc }

Insert a new item into the trie object.

@str
: The string index of the item.

@return
: A reference to the inserted item.

# cainteoir::trie::insert {: .doc }

Insert a new item into the trie object.

@str
: The string index of the item.

@value
: The item to be inserted.

# cainteoir::trie::lookup {: .doc }

Lookup an item by its indexed name.

@str
: The string index of the item.

@no_match
: The value to return if an item does not exist at the string index.

@return
: The matching entry value.

# cainteoir::trie::root {: .doc }

Get the root node.

@return
: The root node of the trie object.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
