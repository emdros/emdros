# emdros

Emdros is a database engine for objects that exhibit the relations
"is_inside" and "is_in_sequence".

Example domains include: Text, DNA, historical events, health
measurements, other physical measurements, stock tickers, etc.

## Emdros in a nutshell

Emdros is a database engine that stores objects with a well-defined
placement in and extent over a stream of events.

Emdros caters to anything that has an underlying linear structure with
objects that have a placement in that sub-structure.

Emdros makes it easy to query the objects in the structure for
relations such as "is_before", "is_after", "is_adjacent", "is_inside",
and "overlaps".

## Emdros's database model in a nutshell

Emdros stores objects with features (attributes).  Each object has a
feature which is a non-empty, arbitrary set of integers.

The integers are called "monads" in Emdros. A monad can be thought of
as representing an indivisible "event".  The sequence of the integers
(monads) dictates the logical sequence of "events".

Monads are integers. Emdros organizes objects through sets of
monads. Because of the mathematical properties of sets of integers,
there exist:

- well-defined "is_before" and "is_after" relations on sets of monads.

- well-defined "is_inside" relations between sets of monads (the
  subset relation)

- well-defined "overlaps" relations between sets of monads (non-empty
  set intersection)

- well-defined "is_a_gap_in" relations between sets of monads.

- ... and other relations, defined in terms of sets of monads
  (integers).

Objects are grouped in *object types*.  The object types dictate what
features an object has.  The features are strongly typed.

Objects always have at least two features:

- The feature "monads" is a non-empty set of monads.
- The feature "self" is a database-widely unique integer ID.

Emdros does not dictate what features an object type may have, beyond
mandating the above two on all object types.

## Examples of object types

Here is how to create an object type, `Token`, which has a "surface"
feature.  The "monads" and "self" features are created implicitly.

```
CREATE OBJECT TYPE
WITH SINGLE MONAD OBJECTS
[Token
   surface : STRING;   
]
GO
```

The `Token` object type is declared to be able to contain object with
exactly one monad in the monad sets.

Here is how to create an object type, `Phrase`, which extends over one
or more contiguous monads.

```
CREATE OBJECT TYPE
WITH SINGLE RANGE OBJECTS
[Phrase
   phrase_type : STRING FROM SET;
]
GO
```

The `Phrase` object type's "monads" feature may only contain sets of
monads that are contiguous, or singletons.  No gaps are allowed.

Here is how to create an object type, `Clause`, which may have gaps in
its "monads" feature.

```
CREATE OBJECT TYPE
WITH MULTIPLE RANGE OBJECTS
[Clause]
GO
```

The `Clause` object type has no features apart from the mandatory
"monads" and "self" features.


## Examples of object creation

Here is how to create some objects of type `Token`.

```
CREATE OBJECTS WITH OBJECT TYPE [Token]
CREATE OBJECT
FROM MONADS = {1}
[
   surface := "The";
]
CREATE OBJECT
FROM MONADS = {2}
[
   surface := "door,";
]
CREATE OBJECT
FROM MONADS = {3}
[
   surface := "which";
]
CREATE OBJECT
FROM MONADS = {4}
[
   surface := "opened";
]
CREATE OBJECT
FROM MONADS = {5}
[
   surface := "towards";
]
CREATE OBJECT
FROM MONADS = {6}
[
   surface := "the";
]
CREATE OBJECT
FROM MONADS = {7}
[
   surface := "East,";
]
CREATE OBJECT
FROM MONADS = {8}
[
   surface := "was";
]
CREATE OBJECT
FROM MONADS = {9}
[
   surface := "blue.";
]
GO
```

This creates the `Token` objects for the sentence "The door, which
opened towards the East, was blue."

The "self" feature is assigned a unique integer automatically by
Emdros.


Here is how to create some objects of type `Phrase`.


```
CREATE OBJECTS WITH OBJECT TYPE [Phrase]
CREATE OBJECT
FROM MONADS = {1-2}
[
   phrase_type := "NP";
]
CREATE OBJECT
FROM MONADS = {3}
[
   phrase_type := "NPRel";
]
CREATE OBJECT
FROM MONADS = {4}
[
   phrase_type := "VP";
]
CREATE OBJECT
FROM MONADS = {5-7}
[
   phrase_type := "PP";
]
CREATE OBJECT
FROM MONADS = {6-7}
[
   phrase_type := "NP";
]
CREATE OBJECT
FROM MONADS = {8}
[
   phrase_type := "VP";
]
CREATE OBJECT
FROM MONADS = {9}
[
   phrase_type := "AP";
]
GO
```

This creates the bracketed phrase structure

```
   [NP The door]
   [NPRel which]
   [VP opened]
   [PP towards
      [NP the East]
   ]
   [VP was]
   [AP blue]
```

Now let us put some clauses on top.

```
CREATE OBJECTS WITH OBJECT TYPE [Clause]
CREATE OBJECT
FROM MONADS = {1-2, 8-9}
[]
CREATE OBJECT
FROM MONADS = {3-7}
[]
GO
```

This creates a clause with a gap: "The door ... was blue", due to the
one with the monad set `{1-2, 8-9}`.  The other one is the relative
clause inside the gap: "which opened towards the East", with the monad
set {3-7}.

NOTE: It is false to say that a monad *is* a Token in this database.
On the contrary, a Token is an object with three features, one of
which is the "monads" set of monads.  The monad set of a Token happens
to have exactly one monad, and that monad coincides with the
"indivisible event" of this database.  But it need not be so -- the
sequence of monads could also contain other kinds of events.

## Emdros's query language in a nutshell

Joins between object types happen by means of relations between sets
of monads.  You can query two object types for the "embedded inside",
"comes before", "comes after", "is before and adjacent", "is after and
adjacent" relations in a declarative query language.

Here is how to retrieve all Phrase objects:

```
SELECT ALL OBJECTS
WHERE
[Phrase]
GO
```

Here is how to retrieve all Phrase objects that are embedded inside a
clause:

```
SELECT ALL OBJECTS
WHERE
[Clause
   [Phrase]
]
GO
```

Because the `Phrase` object block is embedded physically inside the
surrounding `Clause` object block, the `Phrase` objects it finds must
also be physically "inside" the surrounding `Clause` object.  That a
`Phrase` is "inside" a `Clause` means that the "monads" set of monads
of the `Phrase` is a subset of the surrounding `Clause`'s set of
monads.

This illustrates the "embedded in" or "is_inside" relation.  Emdros
also supports "is_before" and "is_after" declaratively:

```
SELECT ALL OBJECTS
WHERE
[Clause
   [Phrase
      phrase_type = "NPRel"
   ]
   [Phrase
      phrase_type = "VP"
   ]
]
GO
```

Here the "NP" phrase must be immediately before, and adjacent to, the
"VP" phrase.  Both must be inside (or embedded in) the surrounding
Clause.

But "is_after" does not necessarily mean "is_adjacent".  In order to
search for adjacent as well as non-adjacent objects, the "power block"
("`..`") can be used:

SELECT ALL OBJECTS
WHERE
[Clause
   [Phrase
      phrase_type = "NPRel"
   ]
   .. 
   [Phrase
      phrase_type = "PP"
   ]
]
GO
```

Now the "NPRel" Phrase does not need to be adjacent to the "PP"
Phrase, but the "PP" Phrase must be after the NPRel, and both must be
inside the confines of the same clause (i.e., both must be subsets of
the surrounding Clause object).

Thus, we can formulate the principle of Emdros's MQL query language:

```The structure of the query
mirrors
the structure of the objects found
with respect to
sequence and embedding.
```



# Download

Please see the Emdros website for downloads:

https://emdros.org/download.html

# Whodunnit?

Crist-Jan Doedens developed the underlying database model and the
query language QL in his PhD thesis.

Ulrik Sandborg-Petersen implemented Doedens' ideas in Emdros, and got
both PhD, MA, and B.Sc. degrees based on the effort (and some more
work).

# Commercial support?

We would be happy to serve you and discuss your needs.

Please write to us for an informal discussion of how we help you
succeed with Emdros.

https://emdros.org/contact.html

Enjoy!

Ulrik Sandborg-Petersen
